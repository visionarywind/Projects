# M06 Module/Launch：模块、函数、参数与图

- 文档目的：解释 01-modules/M06-module-launch/README.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M06-module-launch/README.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


- **职责**：加载 CUDA module/fatbinary/ELF/JIT 产物，建立 `CUmod`/`CUfunc`，解析 kernel 参数元数据，并把 launch 请求转换成 `CUIlaunchData`、constant-bank 参数和 QMD。
- **证据状态**：API launch、参数、tracking、HAL setup、module ELF/JIT、shared image、graph instantiate/launch/unload 已静态确认；架构 QMD/ABI、compiler 和最终 push/设备执行未闭合。
- **核心文件**：`cuimod.c`、`cuifunc.c`、`cuiparam.c`、`cuilaunch.c`、`cuigraph.c`、`cuielf.c`、`cuielf32.c`、`cuielf64.c`、`cuijitlink.c`。
- **资源专题**：[Graph 资源生命周期](graph-resource-lifecycle.md)。Graph exec 会持有 per-context stream、marker、QMD、constant-bank 和 scheduler backing；这些资源必须跨 launch 和 destroy 成对管理。

## 两种执行模式

```text
普通 stream:
API → CUIlaunchData → parameter pack → profiler/tools → setup → HAL/QMD → submit

capture stream:
API → validate kernel params → graph node + dependencies → later graph launch
```

普通和 capture 共享 API 的 function/stream/context 检查，但副作用不同：capture 创建 graph node，普通路径推进异步提交（静态确认：[src/api/apilaunch.c:235-301]）。capture end 还会检查 origin stream、invalidation 和 unjoined streams；失败时直接销毁 graph（[src/cui/cuistream.c:2218-2268]）。

## 参数与函数状态

旧编译器缺少 `EIATTR_KPARAM_INFO` 时，带 `kernelParams` 的现代 launch 返回 `CUDA_ERROR_INVALID_IMAGE`；参数验证产生 packed/unpacked 状态，launch 后更新 function 的 block shape、parameter size 和 dynamic shared size（静态确认：[src/api/apilaunch.c:141-207]）。

## 资源所有权摘要

- instantiate 为每个 context 分配 QMD、constant-bank slice、HAL staging、内部 stream/marker；有 scheduler node 时额外分配 host/device graph backing（静态确认：[src/cui/cuigraph.c:1835-1933]）。
- launch 使用 UVM DAG running 状态、completion marker 和 memory tracking 保持异步引用直到 `streamEndPush` 完成（静态确认：[src/cui/cuigraph.c:4056-4162,3495-3575]）。
- destroy 释放 node、stream、QMD、constant-bank、scheduler memobj 和 host staging；`memobjFree` 可能先同步 context（静态确认：[src/cui/cuigraph.c:1144-1205]；[src/cui/memobj.c:946-964]）。

## 相关模块

M04 提供 launch memory tracking 需要的对象；M05 提供 stream/channel/QMD/marker；M03 提供架构 HAL；M07 提供 device syscall 和生成 kernel。

## 文档元数据（规范补充）

- 文档目的：说明 `01-modules/M06-module-launch/README.md` 的源码分析范围、结论和维护入口。
- 适用范围：当前项目对应模块/入口的静态源码与测试分析。
- 对应源码版本：以本项目 `00-overview/analysis-state.md` 或同页版本字段为准。
- 证据状态：静态源码证据；未执行的构建、测试、GPU、网络或多进程行为保持“未验证”。
- 最后更新：2026-09-15
- 前置阅读：本项目根 README 与 `00-overview/analysis-state.md`。
- 后续阅读：本模块/示例的实现、测试和风险页面。

## 深度审计

| 分析对象 | 入口落地 | 正常路径 | 分支 | 异常 | 清理 | 数据生命周期 | 执行上下文 | 行级证据 | Demo 映射 | 状态/缺口 |
|---|---|---|---|---|---|---|---|---|---|---|
| `01-modules/M06-module-launch` | 已完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 已映射或不适用 | 部分完成：动态行为、边界或专用变体仍需验证 |

## Graph 的设计与实现

Graph 在本树中不是“把若干 kernel 参数存起来”的数组，而是一个两阶段对象：capture 阶段记录 context-independent 的节点/DAG；instantiate 阶段把它编译成可重复执行的 graph exec。`CUgraph_st`/`CUgraphNode_st` 通过 `externalHandle` 暴露公共句柄，内部节点同时保存双向依赖集合、执行 context、internal stream、completion marker、clone 关系和 launch mode（静态确认：[src/cui/cuigraph.h:141-216,269-342]）。

```text
cuStreamBeginCapture
 → CUIgraph + origin/TLS/event state
 → API operation creates node + dependency edges
 → cuStreamEndCapture (join/invalidation validation)
 → cuGraphInstantiate: clone/flatten/memset lowering
 → context resources + scheduling compilation
 → cuGraphLaunch: marker waits + node submission
 → completion tracking
 → cuGraphExecDestroy: detach and reverse release
```

instantiate 会按 context 聚合 QMD、constant-bank、HAL staging、completion marker 和 internal stream；若节点不能由 host pushbuffer 直接串联，则生成 device-side scheduler graph（host/device backing、successor indices、predecessor counters 和 kernel PCAS packet）。调度器因此是对执行计划的编译，不是另一个公共 Graph API（静态确认：[src/cui/cuigraph.c:1835-1933,1978-2051,2054-2141]）。

launch 时根节点等待 API stream marker，跨 internal stream 的边等待前驱 completion marker；kernel 节点按 scheduling group 合并到 pushbuffer，满足同 context、单前驱/单后继且启用 QMD chaining 的边可直接链 QMD，其余边使用 pushbuffer 或 scheduler。每次执行仍动态建立 memory tracking 和 completion 引用；提交成功不等于 GPU 已完成（静态确认：[src/cui/cuigraph.c:2630-2653,2692-2923,3495-3880,4056-4162]）。

`cuGraphExecUpdate` 只更新与原 graph node ID 对齐且拓扑、节点类型、函数/context、memory type 和资源形状兼容的参数；它不重建 exec 的执行资源。Graph destroy 则在已 launch 时传播 completion QMD，再在 context locks 下反向释放节点、stream、marker、QMD、constant-bank 和 scheduler backing（静态确认：[src/api/apigraph.c:1332-1420]；[src/cui/cuigraph.c:4619-4915,1093-1205]）。

## 相关文档
- [项目入口](../../README.md)
- [分析状态](../../00-overview/analysis-state.md)
- [源码证据索引](../../00-overview/evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](../../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
