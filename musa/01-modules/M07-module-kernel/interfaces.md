# Module 与 Kernel 执行：接口契约

- 文档目的：module kernel：接口签名、前置条件、错误码与跨层契约。
- 适用范围：M07 模块的《Module 与 Kernel 执行：接口契约》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


## Driver 层接口

| 接口 | 输入 | 输出/副作用 | 前置条件 | 错误 |
|---|---|---|---|---|
| `muapiModuleLoad` | `MUmodule*`, fname | 写 module 句柄 | current context 存在、文件可读 | `MUSA_ERROR_FILE_NOT_FOUND`/`MUSA_ERROR_INVALID_VALUE`/`MUSA_ERROR_INVALID_CONTEXT`。[src/driver/mu_module.cpp:87-120] |
| `muapiModuleLoadData` | `MUmodule*`, image | 写 module 句柄 | current context 存在、image 非空 | `MUSA_ERROR_INVALID_VALUE`/`MUSA_ERROR_INVALID_CONTEXT`/`MUSA_ERROR_INVALID_IMAGE`。[src/driver/mu_module.cpp:122-146] |
| `muapiModuleGetFunction` | `MUfunction*`, hmod, name | 写 function 句柄 | 模块已加载、名字是 kernel 符号 | `MUSA_ERROR_INVALID_HANDLE`/`MUSA_ERROR_INVALID_VALUE`/`MUSA_ERROR_NOT_FOUND`。[src/driver/mu_module.cpp:157-178] |
| `muapiLaunchKernel` | f, grid/block/sharedMem, stream, params, extra | 入队 kernel 任务 | current context、f 有效 | `MUSA_ERROR_INVALID_VALUE`/`MUSA_ERROR_INVALID_HANDLE`/launch 尺寸类错误。[src/driver/mu_module.cpp:231-268] |
| `muapiLaunchKernelEx` | `MUlaunchConfig*`, f, params, extra | 入队 kernel 任务 | config 非空、attr 受支持 | `MUSA_ERROR_NOT_SUPPORTED`（未支持 attr）。[src/driver/mu_module.cpp:270-334] |
| `muapiModuleUnload` | hmod | 同步后销毁模块 | 模块有效 | `MUSA_ERROR_INVALID_HANDLE`/`MUSA_ERROR_INVALID_CONTEXT`。[src/driver/mu_module.cpp:341-361] |

## 本模块 -> Core

| 接口 | 输入 | 输出/副作用 | 前置条件 | 错误 |
|---|---|---|---|---|
| `Context::CreateModule` | Context | 建 `Module` 并 `RecordModule` | current context | 无（new）。[src/musa/core/context.cpp:1280-1286] |
| `Module::LoadFatBinary` | fatMubin, fromInternal | 建 `m_pHalLibrary`，eager 时 `LoadImage` | 父设备 HAL 就绪 | `MUSA_ERROR_INVALID_IMAGE`。[src/musa/core/module.cpp:20-54] |
| `Module::GetFunction` | name | 写 `IFunction*` | 符号存在且为 kernel | `MUSA_ERROR_NOT_FOUND`。[src/musa/core/module.cpp:56-73] |
| `Module::FindSymbolByName` | name | 写 `SymbolBase_t*` | 已加载（或可惰性加载） | `MUSA_ERROR_NOT_FOUND`/`MUSA_ERROR_INVALID_IMAGE`。[src/musa/core/module.cpp:235-256] |
| `Function::CreateState` | sharedMemBytes, kernelParams, extra, blockSize | 写 `Hal::KernelState*` | 函数已 `Init()` | `MUSA_ERROR_INVALID_VALUE`。[src/musa/core/symbol.cpp:169-266] |
| `Library::LoadToContext` | pContext | 建 context 内 Module 并登记 | device code 已按 arch 编译 | `MUSA_ERROR_NO_BINARY_FOR_GPU`。[src/musa/core/library.cpp:229-269] |

## 本模块 -> HAL

| 接口 | 输入 | 输出/副作用 | 前置条件 | 错误 |
|---|---|---|---|---|
| `Hal::IDevice::CreateLibrary` | `LibraryCreateInfo` | `Hal::ILibrary*` | HAL device 有效 | `HalToMuResult` 转换。[src/musa/core/module.cpp:39-42] |
| `Hal::IDevice::CreateKernel` | `KernelCreateInfo` | `Hal::IKernel*` | library/entry 有效 | `HalToMuResult` 转换。[src/musa/core/symbol.cpp:434-452] |
| `Hal::KernelState::UpdateResourceState` | `KernelResourceStateUpdateInfo[]` | 写 resource buffer | state 已创建 | 本身返回 void；前置 `KernelState::Create` 结果经 `HalToMuResult` 转换。[src/musa/core/symbol.cpp:172-191] |
| `CmdBuffer::CmdBindKernel/CmdBindKernelState/CmdDispatch` | Bind/Dispatch 参数 | 记录到 cmd buffer | 处于 recording 状态 | 取不到 cmd buffer 时 `MUSA_ERROR_OUT_OF_MEMORY`。[src/musa/core/command/dispatchCommand.cpp:96-249] |
| `Queue::Submit` | `QueueSubmitInfo` | 硬件队列提交 | cmd buffer 已 `End()` | `HalToMuResult` 转换。[src/musa/core/command/dispatchCommand.cpp:328-344] |

公共 ABI 兼容性是 M02 的硬约束；其他模块应避免直接改变句柄布局和导出表顺序。此处的“应”是开发约束，不是源码事实。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/driver/mu_module.cpp`（6 处，例：[src/driver/mu_module.cpp:87-120]）
- `src/musa/core/module.cpp`（4 处，例：[src/musa/core/module.cpp:20-54]）
- `src/musa/core/symbol.cpp`（3 处，例：[src/musa/core/symbol.cpp:169-266]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
