# M06 实现：launch setup 和 graph 分支

- 文档目的：解释 01-modules/M06-module-launch/implementation.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M06-module-launch/implementation.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## API common

`cuapiLaunchKernelCommon` 取得 TLS，通过 `cuiFuncInitCheck` 得到 function context，通过 `cuiStreamInitCheckWithFlags` 得到 stream context；二者不相等时返回 `CUDA_ERROR_INVALID_HANDLE`。随后锁 context，按 stream capture 状态选择 graph 或普通 launch（静态确认：[src/api/apilaunch.c:223-251,252-301]）。

capture 分支在 graph capture lock 下构造 `CUDA_KERNEL_NODE_PARAMS`，读取依赖集合，调用 `cuiGraphNodeValidateKernelParams`；graphs-specific validation failure 会转为 `CUDA_ERROR_STREAM_CAPTURE_UNSUPPORTED` 并使 capture 失效；成功则创建 node、更新 capture info，更新失败销毁 node（静态确认：[src/api/apilaunch.c:252-286]）。

## Non-reentrant launch

`cuiLaunchKernel_nonreentrant` 先处理旧 module 参数元数据兼容，再调用 `cuiFuncValidateParams` 填充 launchData.params，设置 block shape 和 shared memory，验证 grid，写入 grid dimensions，调用 `cuiProfilerLaunch`。随后更新持久 function state；若参数已经 packed，必要时复制到 `func->params.v`（静态确认：[src/api/apilaunch.c:124-209]）。

## Setup 阶段

`cuiLaunchSetup_common` 负责 context local memory 配置、kernel syscall callback、`hal.launchCheck`、constant-bank 参数、QMD 绑定、local-memory state 更新和 `hal.encodeAbiConstBankGridParams`（静态确认：[src/cui/cuilaunch.c:229-319]）。这一步是通用 launch 与架构编码之间的边界；真正 push 继续进入 M05。

## 参数打包

`cuiLaunchFinalizeParams` 在参数未 packed 且 packedSize>0 时调用 `cuiFuncPackParameters` 写入 buffer，标记 packed；legacy/extra 路径则认为参数已 packed（静态确认：[src/cui/cuilaunch.c:1195-1211]）。

## Module、ELF 与 JIT 生命周期

内部 syscall/CNP 等模块复用 `cuiModuleLoadInternalAndLink`。该入口强制 `isInternal`、允许 unresolved extern、跳过初次 function download；汇编镜像还关闭若干已在构建阶段处理过的 WAR。加载成功后逐 function patch 外部 text relocation，再 bulk download；debugger 未附加时执行 binary deduplication，任一步失败都统一 `cuiModuleUnload`（静态确认：[src/cui/cuimod.c:919-1003]）。

`cuiModuleLoadBinary` 复制 ELF image 后调用 `cuiElfModuleLoad`，并在 `globals.binload_cs` 保护的 hash 中按 image 查找共享 ELF。首次加载创建 `CUsharedelf` 并令 `ref_count=1`；重复加载增加引用并释放 module 私有副本。卸载在同一把锁下递减引用，归零才删除 hash 项并释放 image（静态确认：[src/cui/cuimod.c:1835-1891]；卸载：[src/cui/cuimod.c:2859-2871]）。

`cuiModuleLoadDataEx` 将 host-relocation 能力、JIT options/feedback、目标 cubin、managed-variable namespace、module binary、system/thread stack、UVM system-wide atomics、context synchronize、CNP 版本、syscall module-load 和 function processing 串成一个事务；失败时清除未注册 namespace 并以 `cuiModuleUnloadAfterLoadFailure` 反向释放（静态确认：[src/cui/cuimod.c:2363-2548]）。JIT feedback 的 log/statistics/wall-time 由 `cuiBeginJitSequence` 持有 `globals.gpgpucompMutex`，结束时恢复 compiler log hook 并刷新反馈（静态确认：[src/cui/cuimod.c:1074-1162]）。

ELF 层先检查 magic，再依据 ELF class 分派到 32/64 位 loader；重定位按目标区分 function text、constant bank 和 bindless texture/sampler/surface，bindless relocation 延迟到 launch 使用（静态确认：[src/cui/cuielf.c:82-125,1845-1928,1955-1972]）。

## 参数与函数资源

`cuiFuncOnModuleLoad` 将 syscall、trap/CNP、cooperative barrier、shared memory、CRS/sync stack 和动态 shared-memory上限合并到最终 function 资源；可启动 CNP function 还下载 CNP state，并把 imports/最大 system stack 合并回 module，完成后设置 `initialized`（静态确认：[src/cui/cuifunc.c:2501-2637]）。`cuiFuncPackParameters` 依据 metadata 的 offset/size 复制 value 参数；`cuiFuncProcessParams` 在 packed/unpacked 两种 ABI 间转换，`cuiFuncValidateParams` 拒绝未知 extra、互斥参数来源、空指针参数和错误 packed size（静态确认：[src/cui/cuifunc.c:2803-2985]）。

## Graph instantiate/launch

instantiate 先做 cycle/conditional validation，clone 并 flatten graph，转换 memset、给节点分配 context、建立 scheduling；随后统计每个 context 的 kernel/QMD/device-node 资源，在所有相关 `persistentState.internalsMutex` 上加锁调用 `cuiGraphInstantiate_UnderLock`，最后登记 QMD semaphore pool。失败会通知 tools 并销毁 exec graph（静态确认：[src/cui/cuigraph.c:3304-3492]）。

per-context 资源不只是 QMD 计数：`allocateExecutionResourcesFunctor` 取得 graph constant-bank pool 节点，分配 QMD、HAL launch staging 和 constant-bank arrays；存在 device scheduler node 时，还分配 host graph/index backing 与一个 driver-owned device `CUmemobj`。node instantiate 再创建/继承无 public handle 的 internal stream、创建 completion marker 并绑定资源 slice（静态确认：[src/cui/cuigraph.c:1835-1933,1621-1809]）。

graph launch 动态补入 launch stream 的 context lock，首次 launch 做 per-context 初始化；每次 launch 等待前一轮 completion marker 和 texture-header update，通知 UVM DAG running，按拓扑序逐节点提交，结束后插入当前 completion 依赖。中途失败则 rollback UVM running 状态，并始终释放多 context 锁及 tools launch 通知（静态确认：[src/cui/cuigraph.c:4056-4162]）。

graph launch 还会临时处理 API stream 替换：若执行 stream 与 graph API stream 属于同一 context，节点中的 stream 指针在提交期间替换，完成后恢复原值。执行 stream 若不在 instantiate 时的 context 集合中，则将其 internals mutex 临时加入锁数组并在退出时撤回。因而 graph exec 的静态 context 集合与实际 launch stream 并非总是相同，错误路径必须同时回滚 UVM running、临时锁数量和 marker 状态（静态确认：[src/cui/cuigraph.c:4056-4162]）。

destroy 在已 launch 的 exec 上先传播 completion QMD，再获取 graph context locks，销毁 node/marker、detach internal stream，并由 `destroyCtxDataFunctor` 释放 QMD、constant-bank、scheduler device memobj、host backing 和 HAL staging（静态确认：[src/cui/cuigraph.c:1035-1064,1093-1205]）。由于 scheduler backing 走 `memobjFree`，active context 上会触发 context synchronize；不能把 host-side destroy 简化成完全无等待的 `free()`（[src/cui/memobj.c:946-964]）。

`cuGraphExecUpdate` 先要求 node-count/topology 可映射，再限制 kernel function、memcpy memory type/context、memset dimension/context 和 host callback。通过检查后只更新既有 exec node 参数，不重建 per-context QMD/constant-bank/stream 拓扑（静态确认：[src/api/apigraph.c:1332-1420]；[src/cui/cuigraph.c:4619-4915]）。

本轮还发现两个待运行验证的静态疑点：QMD semaphore pool 注册失败可能在 `ctxLocks` 写入 exec 后先 free 数组再进入 graph destroy；node launch 中途失败可能绕过 API stream 指针恢复。详见 [Graph 资源生命周期](graph-resource-lifecycle.md) 和 [风险与技术债](risks-and-debt.md)。

module unload 不是单纯释放 ELF：它先从 context module list 摘除，刷新非 internal module 的 syscall 数据，清空 CNP QMD cache，销毁 function/texture/surface/constant/sampler 引用、symbol table 与 module-scope memobj；随后在 `binload_cs` 下递减 shared ELF refcount，并按 syscall imports 递减 active/refcount，最后撤销 UVM namespace/atomics、ISR/trap handler 和 tools handle 后 free module（静态确认：[src/cui/cuimod.c:2767-2931]）。

## 内存追踪

launch memory tracking 收集 function、syscall、context local memory、QMD/texture/sampler pool、context memmgr、per-launch constant bank、module scope 和 function scope objects，防止异步执行期间依赖资源提前回收（静态确认：[src/cui/cuilaunch.c:163-218]）。

## Graph instantiate、调度与 launch 细节

`cuiGraphInstantiate` 的 host-side 编译顺序是：cycle/conditional 检查 → `cuiGraphCloneExec` → `cuiGraphFlatten` → memset lowering → context assignment → scheduling。之后统计每个 context 的 kernel、outer conditional 和 device-node 数量，收集并锁定相关 `internalsMutex`，调用 `cuiGraphInstantiate_UnderLock` 分配资源并实例化节点；最后注册 QMD semaphore pools。任一步失败都进入统一销毁路径（静态确认：[src/cui/cuigraph.c:3304-3492]）。

`cuiGraphSetupScheduling` 用 Kahn 风格的未排序依赖计数产生拓扑顺序：根节点先入队，处理节点时递减后继计数。简单的同 context kernel 边选择 QMD chaining；非 kernel、CNP、跨 context、硬件不支持或显式禁用 chaining 的边标记 pushbuffer；分叉/条件体则插入 scheduler node。连续串行 kernel 按 QMD cache 容量分组并设置 `cacheQmd`（静态确认：[src/cui/cuigraph.c:2630-2653,2692-2923]）。

设备 scheduler backing 中每个 `CUIgraphDeviceNode` 写入 successor index、`nPredsRemain`；kernel 节点编码由 HAL 生成的 PCAS packet，conditional 节点保存 body roots、body end nodes 和 end semaphore。scheduler kernel 参数直到所有 node resource index 确定后才 finalize（静态确认：[src/cui/cuigraph.c:1949-2051,2127-2137]）。具体 scheduler kernel 的设备执行和架构 packet 语义属于外部 compiler/HAL/RM 边界，当前仅静态确认。

`cuiGraphNodeLaunch` 先为根节点等待 API stream marker，为跨 stream 或可能带 QMD semaphore 的前驱插入 marker wait；kernel group 一次 begin/end push，按 `updatePending` 选择完整 `launchFinalize` 或图专用 patch，初始化 QMD semaphore、登记 memory tracking，再由 HAL `launchControl` 发出 root QMD。非 kernel 节点通过 memcpy、host callback、memset 或空操作路径执行，并把下一个 work 的 dependency marker 写回节点；叶节点的 marker 汇入 per-context graph completion marker（静态确认：[src/cui/cuigraph.c:3495-3731,3741-3898]）。

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
