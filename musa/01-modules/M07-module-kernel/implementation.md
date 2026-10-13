# Module 与 Kernel 执行：实现说明

- 文档目的：module kernel：核心函数/流程的逐段实现解读。
- 适用范围：M07 模块的《Module 与 Kernel 执行：实现说明》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


## 正常实现路径

```text
module load -> function -> launch params -> DispatchCommand -> HAL/M3D
```

源码证据：[src/driver/mu_module.cpp:87-146] [src/driver/mu_module.cpp:157-178] [src/driver/mu_module.cpp:231-268] [src/musa/core/command/dispatchCommand.cpp:78-300]

## 模块加载实现

- `muapiModuleLoad` 以 binary+ate 打开文件，`tellg` 取 size、`seekg` 回开头、`read` 进 `unique_ptr<char[]>` 并补 `'\0'`；任一失败返回 `MUSA_ERROR_FILE_NOT_FOUND`。[src/driver/mu_module.cpp:96-111]
- `muapiModuleLoadData` 校验 `module`/`image` 非空、current context 存在，然后 `CreateModule` -> `LoadFatBinary`。[src/driver/mu_module.cpp:122-146]
- `LoadFatBinary` 组装 `Hal::LibraryCreateInfo{sourceType=librarySourceTypeMusaFatBin, sourceLength=0(由 fatbin 决定), lazyLoad=按 moduleLoadingMode}`，调 `Hal().CreateLibrary`；eager 模式直接 `LoadImage()`。[src/musa/core/module.cpp:20-46]
- fatbin 解析符号发生在 HAL 侧：`Hal::M3d::Library::Init` 用 `pSource` 建 ShaderLibrary，`GetSymbols` 惰性枚举 `LibrarySymbol{type, pName, pMemory}`（prealloc device memory view）。[src/hal/m3d/library.cpp:86-110] [src/hal/m3d/library.cpp:24-68]
- eager/lazy 模式来自 `PlatformSettingsLoader`（`moduleLoadingMode`，默认 DEFAULT）。[src/musa/core/platformSettings.cpp:71-76] [src/musa/core/platformSettings.cpp:385-387]

## 函数符号解析实现

- `Module::GetFunction` 走 `FindSymbolByName`：非 eager 先 `LoadImage()`（失败 `DestroyModule`）；`m_SymbolMap` 未命中返回 `MUSA_ERROR_NOT_FOUND`。[src/musa/core/module.cpp:56-73] [src/musa/core/module.cpp:235-256]
- 命中后校验 `SYMBOL_KIND_KERNEL`，并 `Function::Init()` 懒创建 HAL kernel（`Hal().CreateKernel`，gdb/asan 开关来自 `MUgdb/MUasan`）。[src/musa/core/module.cpp:60-69] [src/musa/core/symbol.cpp:434-452]
- `Function::Init` 同时算出 `m_MaxDynamicSharedMemSize = sharedMemPerBlock - 静态sharedMemorySize`，供 launch 时校验动态 shared memory。[src/musa/core/symbol.cpp:434-449]
- `muapiModuleGetFunction` 成功后回填 `*hfunc = pFunction`。[src/driver/mu_module.cpp:157-173]

## Launch 参数组装实现

`muapiLaunchKernel` 把 gridDim/blockDim/sharedMemBytes/kernelParams/extra 填入 `MUSA_KERNEL_NODE_PARAMS`，再交给 `Context::GeneralLaunchKernel`（带 `launchBlocking`）。[src/driver/mu_module.cpp:251-263] [src/musa_shared_include/musa.h:1820-1835]

`GraphKernelNode::UpdateParams` 在创建阶段完成：

- grid 各维 ≤ `maxGridSize` 且 >0；block 各维 ≤ `maxThreadsDim` 且 >0；乘积 ≤ `maxThreadsPerBlock`；乘积 ≤ 函数 `maxBlockSize`（否则 `MUSA_ERROR_LAUNCH_OUT_OF_RESOURCES`）。[src/musa/core/node/graphKernelNode.cpp:66-84]
- TCE SQMMA/CONV kernel 的 block size 必须是 `simdNumPerUsc * warpSize` 的倍数。[src/musa/core/node/graphKernelNode.cpp:86-94]
- cluster 尺寸默认取 metadata 的 `blockClusterSizeX`/`blockClusterSizeZ` 分量，再校验 grid % cluster == 0。[src/musa/core/node/graphKernelNode.cpp:96-111]
- 调 `Function::CreateState(sharedMemBytes, kernelParams, extra, blockSize, &state)`。[src/musa/core/node/graphKernelNode.cpp:113-116]

`Function::CreateState` 是参数布局核心：

- `Hal::KernelState::Create` 后按 resourceMapping 处理三类输入：`kernelParams`（数组逐项，跳过 implicit，指向 null 则 `MUSA_ERROR_INVALID_VALUE`）、`extra`（`MU_LAUNCH_PARAM_BUFFER_POINTER/SIZE/END` 解析，要求 bufferPtr 非空且 size>0）、两者都空（要求全部 mapping 都是 implicit）。[src/musa/core/symbol.cpp:169-223]
- shared memory 校验：`sharedMemorySize > m_MaxDynamicSharedMemSize` 返回 `MUSA_ERROR_INVALID_VALUE`；major==3 时按 attrRegCount/warp 数做 PINGHU 补偿并保证至少 1 字节。[src/musa/core/symbol.cpp:225-259]

## 从 GeneralLaunchKernel 到 HAL 提交

- `GeneralLaunchKernel` 校验 ctx/stream，`CreateKernelNodeOnStream` 建 `GraphKernelNode`，capture 激活走 `Stream::CaptureNode`，否则 `Stream::CmdLaunchKernel`。[src/musa/core/context.cpp:631-677]
- `CmdLaunchKernel` 经对象池建 `DispatchCommand`，`ResolveDependencyAndQueueCommand` 入队。[src/musa/core/stream.cpp:1568-1582]
- `DispatchCommand::Build` 完成所有 HAL cmd：bind kernel/state、设置 policy window、按 `UpdateImplicitResources` 写 KernelState，最后 `CmdDispatch`（workgroupCount=grid、workgroupSize=block、blockClusterSize、constBufferGpuVa）。[src/musa/core/command/dispatchCommand.cpp:154-249]
- `Submit` 收尾：`ResolveSubmitSignal`、`End()`、`MUgdb/MUasan::KernelLoadingProcess`、`SubmitToQueue(cdm)`。[src/musa/core/command/dispatchCommand.cpp:302-347]
- M3D `CmdDispatch` 侧：`GenerateComputeRuntimeData` 用 `KernelState::GetResourceBuffer` 填 const buffer（immediate + kernel args + struct 特殊处理），然后 `m_M3dCmdBuffer->CmdDispatch(dispatchArgs)`，`dynamicSharedSize` 来自 KernelState。[src/hal/m3d/cmdBuffer.cpp:242-345] [src/hal/m3d/cmdBuffer.cpp:347-380]

## 分支矩阵

| 分支 | 触发 | 结果 | 当前证据 |
|---|---|---|---|
| 文件不存在/读失败 | fname 打不开、read 失败 | `MUSA_ERROR_FILE_NOT_FOUND` | 已确认于 muapiModuleLoad |
| 参数/句柄无效 | NULL、无 current context | `INVALID_VALUE/INVALID_CONTEXT/INVALID_HANDLE` | 已确认于 API 层 |
| 函数符号不是 kernel | name 指向 global/缺失 | `MUSA_ERROR_NOT_FOUND` | 已确认于 Module::GetFunction |
| launch 尺寸越界 | grid/block 超限 | `MUSA_ERROR_INVALID_VALUE` / `MUSA_ERROR_LAUNCH_OUT_OF_RESOURCES` | 已确认于 UpdateParams |
| cluster 不合法 | grid 不能整除 cluster | `MUSA_ERROR_INVALID_CLUSTER_SIZE` | 已确认于 UpdateParams |
| 动态 shared 超限 | sharedMemBytes 超过属性上限 | `MUSA_ERROR_INVALID_VALUE` | 已确认于 CreateState |
| capture 模式 | stream 正在 capture | 创建 GraphNode 入 capture 图 | 已确认于 GeneralLaunchKernel |
| DP-CE kernel 入图 | 函数 hasDpCe 且走 graph | `MUSA_ERROR_NOT_SUPPORTED` | 已确认于 GraphKernelNode::PrepareDeviceResource |
| 加载失败回滚 | LoadFatBinary 失败 | DestroyModule | 已确认于 muapiModuleLoadData |

## 尚未证明

该模块是否在所有错误分支都释放中间对象、是否所有底层错误都有稳定映射，必须结合完整函数体和目标机测试确认。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/driver/mu_module.cpp`（7 处，例：[src/driver/mu_module.cpp:87-146]）
- `src/musa/core/module.cpp`（4 处，例：[src/musa/core/module.cpp:20-46]）
- `src/musa/core/symbol.cpp`（4 处，例：[src/musa/core/symbol.cpp:434-452]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
