# M06 实现：launch setup 和 graph 分支

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

graph launch 动态补入 launch stream 的 context lock，首次 launch 做 per-context 初始化；每次 launch 等待前一轮 completion marker 和 texture-header update，通知 UVM DAG running，按拓扑序逐节点提交，结束后插入当前 completion 依赖。中途失败则 rollback UVM running 状态，并始终释放多 context 锁及 tools launch 通知（静态确认：[src/cui/cuigraph.c:4056-4162]）。

graph launch 还会临时处理 API stream 替换：若执行 stream 与 graph API stream 属于同一 context，节点中的 stream 指针在提交期间替换，完成后恢复原值。执行 stream 若不在 instantiate 时的 context 集合中，则将其 internals mutex 临时加入锁数组并在退出时撤回。因而 graph exec 的静态 context 集合与实际 launch stream 并非总是相同，错误路径必须同时回滚 UVM running、临时锁数量和 marker 状态（静态确认：[src/cui/cuigraph.c:4056-4162]）。

module unload 不是单纯释放 ELF：它先从 context module list 摘除，刷新非 internal module 的 syscall 数据，清空 CNP QMD cache，销毁 function/texture/surface/constant/sampler 引用、symbol table 与 module-scope memobj；随后在 `binload_cs` 下递减 shared ELF refcount，并按 syscall imports 递减 active/refcount，最后撤销 UVM namespace/atomics、ISR/trap handler 和 tools handle 后 free module（静态确认：[src/cui/cuimod.c:2767-2931]）。

## 内存追踪

launch memory tracking 收集 function、syscall、context local memory、QMD/texture/sampler pool、context memmgr、per-launch constant bank、module scope 和 function scope objects，防止异步执行期间依赖资源提前回收（静态确认：[src/cui/cuilaunch.c:163-218]）。
