# 跨模块系统串联

- 文档目的：解释 90-cross-module/system-wiring.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 90-cross-module/system-wiring.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## 主运行时链

```text
cuInit
 → cuiInit / globals / TLS / managers
 → cuCtxCreate_v2 / CUctx / current TLS
 → cuMemAlloc_v2 / memdesc / memobj / UVA registration
 → cuStreamCreate / stream pool / QMD / semaphores
 → cuModule + cuFunction
 → cuLaunchKernel
 → parameter pack + syscall tracking + HAL check
 → streamBeginPush
 → HAL finalize/control + channelTrackQMD
 → streamEndPush
 → marker completion / optional synchronize
 → detach/reclaim / context destroy
```

证据分别见 `[src/api/apiinit.c:19-47]`、`[src/cui/cuiinit.c:3060-3208]`、`[src/api/apimem.c:52-118]`、`[src/cui/cuistream.c:1741-1877]`、`[src/cui/cuilaunch.c:229-338,582-817]`、`[src/cui/cuistream.c:1926-1954,2004-2088]`。

## 内存池化与 VA 分工

```text
memobj request
 → descriptor compatibility
 → suballocator radix tree best-fit
 → existing memblock region OR new memblock
 → DMAL physical backing + UVA/device mapping
 → memobj pointer lookup / launch tracking
 → free region coalesce OR last-memobj memblock release
```

`CUheap` 只维护 VA reservation/lookup；`CUsuballocatorRadixTree` 维护 block 内 free region；DMAL 才是物理资源边界。M04 的 Graph scheduler backing 也经由 `memobjAlloc` 进入这条链，而 QMD/constant-bank/stream pool 属于执行资源池（静态确认：[src/cui/memobj.c:265-375]；[src/cui/suballocator.c:163-220,343-405]；[src/cui/memblock.c:471-562]；[src/cui/cuigraph.c:1898-1932]）。

## Graph 资源串联

```text
capture stream
 → CUIgraph nodes/dependencies
 → clone/flatten + per-context ctxData
 → QMD/constant-bank/internal stream/marker
 → optional scheduler host/device backing
 → launch memory tracking + UVM DAG running
 → topological stream push
 → completion markers
 → destroy: detach + qmd/const/memobj release
```

Graph exec 的静态 context 集合在 instantiate 确定，但 launch stream 可能追加临时 context lock；错误路径必须同时回滚 UVM running、锁数量和临时 stream 替换（静态确认：[src/cui/cuigraph.c:3304-3492,4056-4162]；静态风险：[src/cui/cuigraph.c:3457-3492,4116-4157]）。

## Graph 的执行计划

Graph capture 首先形成 context-independent 的 `CUIgraph` DAG；instantiate clone 后递归 flatten child/conditional graph，并可能把 memset 降为 kernel chain。随后 `cuiGraphSetupScheduling` 进行拓扑排序并把边分配到 pushbuffer、QMD chaining 或 device scheduler；per-context `CUIgraphCtxData` 持有 QMD、constant-bank、internal stream、completion marker 和 scheduler backing。launch 再以 API marker/UVM DAG 为入口，按拓扑序把计划提交到各 internal stream，叶节点 completion 汇聚到 context marker（静态确认：[src/cui/cuigraph.c:2320-2437,2534-2627,2692-2923,3304-3492,3741-4162]）。

## OpenCL 与工具旁路

- OpenCL ICD/vendor dispatch 最终进入 `src/cl` public object；`CLIobjectData` 的 public/internal refcount tree 独立于 CUI `CUctx`、`CUmemobj` 和 GPU marker（[src/cl/cliobject.h:98-169]）。context destroy 必须先停 worker/callback thread 与 task graph，再清 pinned tracking 和 CUI contexts（[src/cl/clicontext.c:467-545]）。
- `clWaitForEvents` 先等 submitted，再按 device 聚合 marker；GPU completed 快速路径仍需显式 clear pinned memcpy tracking（[src/cl/clevent.c:106-217]）。
- Tools callback 从 launch begin 到 launch end 横跨 M06/M05；debugger 的 shared state、memcheck device table 和 profiler perfmon 可改变或观察主路径，但不取代 marker completion（[src/cui/cuilaunch.c:468-503,635-710,779-817]；[src/devtools/memcheck/memcheck.c:120-256]）。
- GL/external interop 在 host 侧登记 resource 或由 DMAL 打开 handle，再创建 memobj/semaphore backing；fence、handle close 与 RM 所有权仍是外部边界（[src/cl/clgl.c:93-204]；[src/cui/cuiextinterop.c:21-171]）。

## 共享状态

- `CUctx` 是 API 检查、memmgr、stream manager、module/function 和 sticky error 的关联根。
- `CUmemobj` 由 memory manager 建立，也被 launch tracking、UVA/P2P 和 tools 观察。
- `CUIstream` 连接 QMD、channel、constant bank、marker 和 UVM registration；GPFIFO 的 pushbuffer 空间只有在 tracking semaphore 完成后才能回收。
- `CUmod` 卸载同时撤销 function、shared ELF、syscall imports、UVM namespace/atomics、CNP cache 和 tools 状态。
## 异步与错误

提交成功只表示 pushbuffer 已建立/提交，不代表 GPU 完成；stream detach 将 public handle 立即失效，但保留内部对象至 marker 完成。launch 失败需要结束 push、撤销 UVM running 状态并发送 tools end callback。具体设备错误由外部 DMAL/RM 产生的部分未知。

## 关联入口

- 内存释放必须在 base pointer、API source 和 context 同步规则下进行。
- capture launch 不立即 push，而创建 graph node；普通 launch 才进入 M05 提交路径。
- OpenCL ICD 最终把 vendor dispatch 接到相同 CUI/context/memory/stream 体系，但 enqueue/object 生命周期尚未完全闭合。
- M10 通过 `basic_sanity` 和 stream/memory 单测验证这些边界；目前仅静态确认，未运行。
- M10 当前没有专用 CUDA Graph/capture 测试；channel queued-dependency graph 只能证明提交层图，不覆盖 Graph API 的 instantiate/launch/destroy 资源生命周期。

## 相关文档
- [项目入口](../README.md)
- [分析状态](../00-overview/analysis-state.md)
- [源码证据索引](../00-overview/evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
