# Module 与 Kernel 执行：风险与技术债

- 文档目的：module kernel：风险、技术债及源码证据与修复/验证建议。
- 适用范围：M07 模块的《Module 与 Kernel 执行：风险与技术债》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


## 风险表

| 风险 | 影响 | 证据/状态 | 建议 |
|---|---|---|---|
| 参数布局 ABI（kernelParams/extra） | 调用方与编译器 layout 不一致会静默错位 | `CreateState` 按 `KernelResourceMapping` 顺序拷参，未校验总字节数；extra 要求调用方自带完整 buffer。[src/musa/core/symbol.cpp:169-223] | 保留 `GetParamCount/GetParamInfo` 供调用方查询；对 extra buffer 增加长度/对齐校验。[src/musa/core/symbol.cpp:73-84] |
| struct 参数按硬件代次分流 | major<4 走 embedded/internal 分配，major>=4 返回 `errorInvalidImage` | `GenerateComputeRuntimeData` 的 `Struct` 分支。[src/hal/m3d/cmdBuffer.cpp:288-324] | 补新代次 struct 支持或明确报错路径 |
| 动态 shared memory 上限 | 超过属性上限直接失败；major==3 有特殊补偿逻辑 | `sharedMemorySize > m_MaxDynamicSharedMemSize` 判 INVALID；PINGHU attr 补偿与至少 1 字节规则。[src/musa/core/symbol.cpp:225-259] | 针对 major==3 补偿路径做专项测试 |
| 隐式资源类型动态分派 | 未识别的 implicit 类型仅 `tprintf(LOG_ERR)`，不失败 | `UpdateImplicitResources` 的 `default` 分支。[src/musa/core/command/dispatchCommand.cpp:1030-1032] | 新枚举加 static_assert 校验（`kernel.h` 已对数值一致性做 static_assert） |
| module 生命周期/回滚 | lazy 加载失败在 `FindSymbolByName` 内 `DestroyModule`，可能与持有引用冲突 | [src/musa/core/module.cpp:238-244] | 确认所有 `DestroyModule` 路径无 use-after-free；依赖 `ValidateModule` |
| launch 参数校验遗漏 | 非法 grid/block 进入 HAL | `UpdateParams` 已查尺寸，但 cooperative 能力、TCE 限制等有 TODO | [src/driver/mu_module.cpp:336-339] [src/musa/core/node/graphKernelNode.cpp:66-111] | 补 cooperative 校验 |
| DP-CE/GPU copy-job 依赖 engine-sync | 无 engine-sync 硬件直接 `MUSA_ERROR_NOT_SUPPORTED`；graph 内禁止 | `SetupGpuCopyJobResources` 前置检查与 `PrepareDeviceResource` 拒绝。[src/musa/core/command/dispatchCommand.cpp:1062-1072] [src/musa/core/node/graphKernelNode.cpp:133-139] | 平台能力矩阵化 |
| HAL/M3D 私有实现不可见 | 无法证明硬件副作用 | `CmdDispatch` 之后行为在 M3D 内部。[src/hal/m3d/cmdBuffer.cpp:347-380] | 目标机运行验证 |

## 技术债清单

- `muapiModuleLoadDataEx` 返回 `MUSA_ERROR_NOT_SUPPORTED`，JIT options 未实现。[src/driver/mu_module.cpp:148-151]
- `Library::GetManaged`/`GetUnifiedFunction` 为 TODO 占位，返回 `MUSA_ERROR_NOT_SUPPORTED`。[src/musa/core/library.cpp:172-194]
- `muapiLaunchCooperativeKernel` 直接转发 `muapiLaunchKernel`，能力校验为 TODO。[src/driver/mu_module.cpp:336-339]
- `KernelMetadata::maxThreadsPerBlock` 硬编码 0（TODO），依赖 `CalculateMaxBlockSize` 兜底。[src/hal/m3d/kernel.cpp:107]
- `MUSA_ERROR_NO_BINARY_FOR_GPU` 只覆盖 arch 未编译路径，未覆盖 fatbin 不匹配场景的详细诊断。[src/musa/core/library.cpp:229-246]

## 说明

本模块具体风险按其入口和资源类型排序；未执行的测试不标为通过。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/musa/core/symbol.cpp`（3 处，例：[src/musa/core/symbol.cpp:169-223]）
- `src/driver/mu_module.cpp`（3 处，例：[src/driver/mu_module.cpp:336-339]）
- `src/hal/m3d/cmdBuffer.cpp`（2 处，例：[src/hal/m3d/cmdBuffer.cpp:288-324]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 上表风险的运行期复现与修复验证均未执行。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
