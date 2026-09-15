# CUDA Graph 资源生命周期与失败路径

- 文档目的：补齐 graph/capture 从创建、实例化、执行到销毁的资源所有权与同步边界。
- 适用范围：`source/cuda/src/api` 和 `src/cui` 的 host-side graph 实现；设备 scheduler kernel、RM 和 HAL 末端仅追踪到接口边界。
- 对应源码版本：`source/cuda` 快照，内部 API 版本 CUDA 10.2。
- 证据状态：Graph host-side 主路径和资源清理已静态确认；GPU 执行、性能和并发竞态未验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

Graph 不是只保存 kernel 参数的静态 DAG。普通 graph 在 capture 期间保存节点和依赖；instantiate 会 clone/flatten、为每个 context 建立资源表，分配内部 stream、completion marker、QMD、constant-bank backing、HAL staging 和可选的 device-side scheduler graph；launch 通过 marker、UVM DAG 和 stream push 建立跨 context 的异步依赖。Graph exec destroy 反向释放这些资源，并通过 memobj/stream 的同步契约避免 GPU 仍在使用时回收 backing。

## Capture 到 Graph

`cuStreamBeginCapture` 先校验 mode、stream 和 null-stream 限制，在 context lock 下创建 `CUIgraph`，保存 origin stream、capture mode、TLS 关联、invalidation status 和 captured events（[src/api/apistream.c:538-575]；[src/cui/cuistream.c:2180-2211]）。capturing stream 上的 `cuLaunchKernel` 不提交 GPU，而是在 graph capture lock 下验证参数、读取 `nextPushDeps`、创建 kernel node 并更新 capture 信息；Graph-specific validation 会把返回码转换为 `CUDA_ERROR_STREAM_CAPTURE_UNSUPPORTED`，同时将 capture 标记 invalidated（[src/api/apilaunch.c:223-301]）。

`cuStreamEndCapture` 要求在 origin stream 上结束，检查错误状态和所有节点是否从 end-capture 依赖可达；错误或 unjoined capture 会销毁 graph 并返回 NULL，成功才把 `CUIgraph.externalHandle` 交给调用者（[src/api/apistream.c:627-667]；[src/cui/cuistream.c:2218-2268]）。因此 capture graph 的 host 对象在失败路径也必须完整释放。

## Instantiate 的资源分层

`cuiGraphInstantiate` 先做 cycle/conditional validation，clone 并 flatten graph，转换 memset，给节点分配 context 并建立 scheduling；随后按 context 统计 kernel、conditional 和 device-node 数量，分配 context-lock 数组（[src/cui/cuigraph.c:3304-3445]）。

`allocateExecutionResourcesFunctor` 为每个 context 分配：

- kernel 参数 constant-bank pool 节点、每 kernel 的 constant-bank slice 和 HAL launch-data staging；
- kernel/conditional QMD 数组；
- 有 device nodes 时的 host scheduler graph backing，以及一个 `CU_MEM_TYPE_GENERIC` device `CUmemobj`，并把 nodes/indexes 映射到该 backing（[src/cui/cuigraph.c:1835-1933]）。

随后 `cuiGraphNodeInstantiate` 为没有继承 stream 的节点创建无 public handle 的内部 stream 和 completion marker；kernel 节点固定 grid id、QMD/constant-bank slice、`lmemNoResize`，调用 `cuiLaunchSetup_common` 完成 HAL/QMD/ABI staging（[src/cui/cuigraph.c:1621-1735]；[src/cui/cuigraph.c:1753-1809]）。所有节点资源和 context 数据均保存在 `CUIgraphCtxData` 中（[src/cui/cuigraph.h:218-267]）。

## Launch 的执行与引用

实例化成功后，graph exec 持有每 context 的 `internalsMutex`、内部 stream 链表、QMD/constant-bank 资源和 completion marker。launch 若执行 stream 的 context 不在静态集合中，会临时追加一把 context lock；首次 launch 初始化 scheduler backing 并获取参数 bank，后续 launch 先等待上次 graph completion marker 和 texture-header update，再调用 `cuiUvmDagSetRunning`（[src/cui/cuigraph.c:3973-4054]；[src/cui/cuigraph.c:4056-4110]）。

节点按 instantiate 后的拓扑序提交：root 节点获取 API stream marker，跨 stream 或 QMD 节点依赖通过 `cuiStreamWaitMarker` 建立；kernel group 在 push 前建立 memory-tracking list，追踪 function、texture/sampler、local memory、vidmem heap、constant banks 和 syscall memory，`streamEndPush` 将引用挂到异步完成边界（[src/cui/cuigraph.c:3495-3575]；[src/cui/cuigraph.c:3741-3880]）。launch 完成后等待各 context completion marker；失败则回滚 UVM running 状态并释放临时 lock（[src/cui/cuigraph.c:4111-4162]）。

## Destroy 与所有权回收

destroy 首先在已 launch 的 graph 上传播 completion QMD，必要时取得全部 context locks；随后销毁节点、detach 内部 streams、释放 QMD、constant-bank、completion marker、scheduler device memobj、host backing、HAL staging 和 constant-bank arrays，最后解除 clone links 并释放 graph（[src/cui/cuigraph.c:1093-1205]）。`memobjFree` 本身在 active context 上会先执行 `cuiCtxSynchronize`，所以 graph exec 的资源释放不是“无条件立即 free”；实际等待边界由 context/DMAL 实现决定（[src/cui/memobj.c:946-964]）。

## Update 与资源稳定性

`cuGraphExecUpdate` 只接受 topology、node type、function、1D memcpy、1D memset 和 host callback 仍兼容的 graph；它通过 per-graph-node ID 建立 update array，再逐节点检查并更新参数（[src/api/apigraph.c:1332-1420]；[src/cui/cuigraph.c:4619-4915]）。当前实现不会因为普通 kernel 参数更新而重新分配 graph exec 的 QMD/constant-bank 拓扑资源；拓扑或资源形状变化应返回 update failure。

## 静态风险（待运行验证）

1. **Instantiate error cleanup 的锁数组悬挂风险**：`cuiGraphInstantiate` 在将 `graphExec->ctxLocks = locksToTake` 写入 exec 后，仍可能因 QMD semaphore pool 注册失败跳到统一 Error；Error 先 `free(locksToTake)`，再调用 `cuiGraphDestroy`，而 destroy 会依据非空 `ctxLocks` 再次 lock/unlock 该指针（[src/cui/cuigraph.c:3457-3492]；destroy：[src/cui/cuigraph.c:1144-1187]）。这是静态控制流疑点，需注入注册失败或用 ASan 验证，不能直接断言必现 UAF。
2. **Launch error 的 API stream 恢复缺口**：当 graph 使用 API stream 替换节点 stream 时，替换发生在 [src/cui/cuigraph.c:4116-4120]，恢复只位于成功提交后的 [src/cui/cuigraph.c:4132-4136]；节点中途 launch 失败会直接跳到 Done，静态上留下临时 stream 指针，需构造失败节点验证后续 launch 行为。
3. **多 context 资源边界**：context 集合在 instantiate 固定，但 launch stream 可带来额外 context lock；任何修改必须保持 `numLocks` 增减、UVM rollback 和 marker 释放成对（[src/cui/cuigraph.c:4075-4082,4150-4160]）。

## 测试与缺口

当前 `tests/cuda_test/tests.nvmk` 未注册专用 graph/capture 测试；现有 channel/marker 测试验证的是内部 queued-dependency graph，不等价于 CUDA Graph API。应补充 capture success/invalidation/unjoined、instantiate OOM、multi-context launch、destroy-after-launch、update topology failure 及上述两条失败路径的注入测试。当前均未执行 GPU/nvmake 验证。

## 深度审计

| 分析对象 | 入口落地 | 正常路径 | 分支 | 异常 | 清理 | 数据生命周期 | 执行上下文 | 行级证据 | Demo 映射 | 状态/缺口 |
|---|---|---|---|---|---|---|---|---|---|---|
| capture/graph exec | 已完成 | 已完成 | 已完成 | 部分完成 | 已完成 | 已完成 | 多 context/stream 已确认 | 已完成 | 当前无专用 Graph Demo | 部分完成：失败路径和设备端待运行验证 |

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
