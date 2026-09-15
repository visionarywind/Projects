# M06 数据结构

- 文档目的：解释 01-modules/M06-module-launch/data-structures.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M06-module-launch/data-structures.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## 核心对象

| 对象 | 关键内容 | 依赖 |
|---|---|---|
| `CUmod` | module 镜像、context 归属、CNP/syscall imports | context、module memobj |
| `CUfunc` | 函数元数据、参数布局、grid/launch 状态 | `CUmod`、function memobj |
| `CUIlaunchData` | 本次 grid/block/shared memory、参数、QMD 和 ABI staging | stream、QMD、constant bank |
| `CUIkernelParams` | packed/unpacked 参数及大小 | function metadata、调用者参数 |
| `CUqmd` | 架构提交描述和 launch semaphore 信息 | M05 stream/QMD pool、M03 HAL |
| `CUsharedelf` | 共享 ELF image 与引用计数 | `globals.binload_cs`、module unload |
| `CUlinkState` | JIT/link options、elfLink handle、input copies、final cubin | `gpgpucompMutex`、external elfLink |
| `CUIgraphCtxData` | graph 每 context 的 marker、stream、QMD/device-node 资源统计 | context `internalsMutex` |

## Graph 资源字段

| 资源 | 持有者 | 创建 | 释放/异步边界 |
|---|---|---|---|
| 内部 `CUIstream` | `CUIgraph.streams` / node `stream` | instantiate 为未继承 stream 创建 | graph destroy 中 detach；detach 先处理 capture/UVM，再进入 detached/free pool |
| `CUctxMarker` | node 或 `CUIgraphCtxData.completionMarker` | node instantiate / context 注册 | destroyCtxData 或 node destroy；launch 通过 marker 建依赖 |
| `CUqmd` 数组 | `CUIgraphCtxData.qmds` | 每 context 按 kernel+outer conditional 数量分配 | `qmdDeallocate`；QMD 释放必须晚于异步 launch tracking |
| constant-bank slice | `paramConstBankNodes`、`constBankAllocations` | `cuiConstBankAcquire` + per-node slice | `cuiConstBankRelease`；completion marker 作为复用门槛 |
| scheduler backing | `schedGraphBackingHost` + `schedGraphBackingDev` | device-node 存在时 host malloc + driver `memobjAlloc` | `memobjFree`/host free；device copy 通过 stream tracking 保护 |
| HAL launch staging | `halLaunchDataAllocations` | 每 kernel node 按 `halLaunchDataSize` 分配 | graph destroy 时 free |

## 所有权与异步

module/function 由 context 相关链表管理；launch 期间 `doLaunchMemTracking` 把 function、syscall、module/function scope 和 constant-bank 资源纳入 tracking，防止 GPU 尚未完成时提前释放（静态确认：[src/cui/cuilaunch.c:163-218]）。`CUsharedelf` 的 image 由最后一个 module 引用释放；module unload 在释放自身资源前递减 syscall imports、注销 managed variables/system-wide atomics，并在最后发出 tools unload completed（静态确认：[src/cui/cuimod.c:2767-2943]）。`CUlinkState` 在 `cuiLinkComplete` 后释放 input copies，final cubin 由 state 持有到 destroy；`cuiLinkAddCubinData` 必须复制输入，因为 elfLink 要求数据持续到 finish（静态确认：[src/cui/cuijitlink.c:219-255,360-402]）。

Graph exec 保存每 context 的锁、completion marker、内部 stream/QMD、constant-bank 和 device scheduler backing；instantiate 的资源分配失败销毁整个 exec graph，launch 期间用 marker、UVM DAG 和 memory tracking 保证跨 context/异步依赖（静态确认：[src/cui/cuigraph.c:1835-1933,2054-2141,3304-3492,4056-4162]）。

## Graph 对象模型

| 层次 | 对象/字段 | 作用 |
|---|---|---|
| 公共句柄 | `CUgraph_st.graph`、`CUgraphExec_st.graph` | API wrapper 与内部 `CUIgraph` 的间接层 |
| 描述图 | `CUIgraph.nodesHead/tail`、`lastIssuedPerGraphNodeId` | 保存节点顺序、局部 ID 和 capture 图状态 |
| 节点 | `CUgraphNode_st.dependencies/dependentNodes` | 双向 DAG 边；`originalNode` 用于 clone/update 映射 |
| 执行节点 | `ctx`、`stream`、`launchMode`、`deviceNodeIdx` | 绑定 context、internal stream 和提交后端 |
| 执行资源 | `CUIgraphCtxData` | 按 context 聚合 QMD、constant-bank、HAL staging、marker 和 scheduler backing |
| 完成关系 | node `completionMarker` + ctx `completionMarker` | 让后继节点、下一次 graph launch 和 destroy 观察异步完成 |
| capture 状态 | `capture.origin`、`mode`、`tlsData`、`eventsHead`、`invalidationStatus` | 管理多 stream capture 的所有权、线程模式和失败状态 |

节点的 `launchMode` 不是 API 可见属性，而是 instantiate 对设备能力和 DAG 形状做出的编译决策：`PUSHBUFFER` 表示 host 顺序提交，`QMD_CHAINING` 表示硬件 QMD 直接衔接，`SCHEDULER` 表示由 device scheduler 推进（静态确认：[src/cui/cuigraph.h:134-216,218-267]）。

## Scheduler 数据布局

`CUIgraphCtxData.schedGraphBackingHost` 同时容纳 `CUIgraphDeviceNode` 数组和 `devNodeIndices` 数组；device copy 由 `schedGraphBackingDev` 持有，其 device virtual address 被写入 scheduler kernel 参数。每个 device node 记录 successor 范围、总 predecessor 数和剩余 predecessor 计数；conditional node 额外记录 body roots/end nodes 与 end semaphore（静态确认：[src/cui/cuigraph.h:244-267]；[src/cui/cuigraph.c:1949-2051]）。

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
