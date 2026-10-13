# Module 与 Kernel 执行：行级分析

- 文档目的：module kernel：关键代码位置的行级证据与解读。
- 适用范围：M07 模块的《Module 与 Kernel 执行：行级分析》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


## 证据表

| 行号 | 观察 | 结论状态 |
|---|---|---|
| [src/driver/mu_module.cpp:87-120] | `muapiModuleLoad`：InitPlatform -> 文件读入 -> 转 `muapiModuleLoadData`；失败映射 `MUSA_ERROR_FILE_NOT_FOUND` | 已确认 |
| [src/driver/mu_module.cpp:122-146] | `muapiModuleLoadData`：TlsCtxTop -> `CreateModule` -> `LoadFatBinary`；失败 `DestroyModule` 回滚 | 已确认 |
| [src/driver/mu_module.cpp:157-178] | `muapiModuleGetFunction`：句柄/名字校验 -> `Module::GetFunction` -> 回填 `*hfunc` | 已确认 |
| [src/driver/mu_module.cpp:231-268] | `muapiLaunchKernel`：组装 `MUSA_KERNEL_NODE_PARAMS` -> `GeneralLaunchKernel` | 已确认 |
| [src/driver/mu_module.cpp:270-334] | `muapiLaunchKernelEx`：解析 `MUlaunchConfig` attrs，cluster/PDL 支持，其余 attr 返回 `MUSA_ERROR_NOT_SUPPORTED` | 已确认 |
| [src/musa/core/module.cpp:20-54] | `LoadFatBinary`：`Hal::LibraryCreateInfo` + `CreateLibrary` + eager `LoadImage` + 资源回调 | 已确认 |
| [src/musa/core/module.cpp:56-73] | `GetFunction`：`FindSymbolByName` + `SYMBOL_KIND_KERNEL` + `Function::Init` | 已确认 |
| [src/musa/core/module.cpp:187-233] | `LoadImage`：`call_once` + HAL `GetSymbols` + 建 `Function`/`VariableSymbol` 入 `m_SymbolMap` | 已确认 |
| [src/musa/core/module.cpp:235-256] | `FindSymbolByName`：惰性 `LoadImage`，失败 `DestroyModule`；未命中 `MUSA_ERROR_NOT_FOUND` | 已确认 |
| [src/musa/core/module.cpp:174-185] | 析构：先 `UntrackMemory`/reset 符号，再 `m_pHalLibrary->Destroy()` | 已确认 |
| [src/musa/core/context.cpp:631-677] | `GeneralLaunchKernel`：stream/context 校验 -> `CreateKernelNodeOnStream` -> capture 或 `CmdLaunchKernel` | 已确认 |
| [src/musa/core/command/dispatchCommand.cpp:78-300] | `Build`：依赖解析、隐式资源、bind kernel/state、`CmdDispatch` 参数 | 已确认 |
| [src/musa/core/command/dispatchCommand.cpp:302-347] | `Submit`：`End()`、`MUgdb/MUasan` 处理、`SubmitToQueue(cdm)` | 已确认 |
| [src/hal/m3d/kernel.cpp:56-126] | `Kernel::Init`：`GetKernelMetadata` + `CalculateMaxBlockSize` + 拷贝 metadata 到 Mthreads 结构 + `CreatePipeline` | 已确认 |
| [src/hal/m3d/cmdBuffer.cpp:347-380] | `CmdDispatch`：`GenerateComputeRuntimeData` -> `m_M3dCmdBuffer->CmdDispatch` | 构建/接口已确认，硬件副作用未知 |
| [src/hal/m3d/queue.cpp:182-355] | `Queue::Submit`：封装提交信息后 `m_M3dQueue->Submit` | 构建/接口已确认，硬件副作用未知 |

## 阅读方法

1. 先看参数检查和 early return：`muapiModuleLoadData` 的 NULL 检查、`muapiLaunchKernel` 的 `f` 检查、`GeneralLaunchKernel` 的 stream/ctx 校验。[src/driver/mu_module.cpp:122-146] [src/driver/mu_module.cpp:244-249] [src/musa/core/context.cpp:642-658]
2. 再看对象创建、登记、引用计数或 tracker 更新：`CreateModule`/`DestroyModule`、`LoadImage` 的 `TrackMemory`、DispatchCommand 的池化创建与 `ReleaseResources`。[src/musa/core/context.cpp:1280-1313] [src/musa/core/module.cpp:200-228] [src/musa/core/command/dispatchCommand.cpp:1347-1360]
3. 最后看 HAL 调用、异步入队、错误回滚和析构：`CmdDispatch`、`SubmitToQueue`、`DispatchCommand` 析构的 `Wait()`。[src/musa/core/command/dispatchCommand.cpp:57-64] [src/musa/core/command/dispatchCommand.cpp:302-347]

本页不把没有逐行读取的代码扩写成确定行为。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/driver/mu_module.cpp`（7 处，例：[src/driver/mu_module.cpp:87-120]）
- `src/musa/core/module.cpp`（6 处，例：[src/musa/core/module.cpp:20-54]）
- `src/musa/core/command/dispatchCommand.cpp`（5 处，例：[src/musa/core/command/dispatchCommand.cpp:78-300]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
