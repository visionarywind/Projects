# Module 与 Kernel 执行：例子

- 文档目的：module kernel：用法示例与边界/错误输入说明。
- 适用范围：M07 模块的《Module 与 Kernel 执行：例子》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


## 静态最小例子：加载并 launch

```text
MUmodule module;
muModuleLoad(&module, "kernel.mubin");                     // muapiModuleLoad -> muapiModuleLoadData
MUfunction kernel;
muModuleGetFunction(&kernel, module, "myKernel");          // Module::GetFunction -> Function::Init
unsigned int gridX = 16, blockX = 256;
void* args[] = { &inputPtr, &outputPtr };                  // 每个元素指向一个参数值
muLaunchKernel(kernel, gridX, 1, 1, blockX, 1, 1, 0, stream, args, nullptr);
// 生命周期结束前同步并释放
muModuleUnload(module);                                    // Synchronize + DestroyModule
```

- `args` 中每个指针会经 `Function::CreateState` 按非 implicit resourceMapping 顺序写入 `KernelState`。[src/musa/core/symbol.cpp:169-192]
- `extra` 路径必须提供 `MU_LAUNCH_PARAM_BUFFER_POINTER/SIZE` 并用 `MU_LAUNCH_PARAM_END` 结尾。[src/musa/core/symbol.cpp:193-214]

## 分支示例：eager 与 lazy

`moduleLoadingMode` 决定 `LoadFatBinary` 是否立即 `LoadImage()`；lazy 下第一次 `GetFunction` 才触发符号解析。[src/musa/core/module.cpp:44-46] [src/musa/core/module.cpp:235-256]

## 分支示例：动态 shared memory

launch 前用 `muFuncSetAttribute(MU_FUNC_ATTRIBUTE_MAX_DYNAMIC_SHARED_SIZE_BYTES, n)` 设定上限，否则默认上限为 `sharedMemPerBlock - 静态sharedMemorySize`；超过上限的 `sharedMemBytes` 会返回 `MUSA_ERROR_INVALID_VALUE`。[src/musa/core/symbol.cpp:225-259] [src/musa/core/symbol.cpp:444-448]

## 分支示例：cluster 与 PDL

`muapiLaunchKernelEx` 支持 `MU_LAUNCH_ATTRIBUTE_CLUSTER_DIMENSION` 与 `MU_LAUNCH_ATTRIBUTE_PROGRAMMATIC_STREAM_SERIALIZATION`；`UpdateParams` 校验 cluster 整除性与大小限制，`Build` 把 PDL 地址转为 `dispatch.pdlAddress/pdlValue`。[src/driver/mu_module.cpp:286-312] [src/musa/core/command/dispatchCommand.cpp:169-176]

## 说明

以上是基于源码阅读模型的静态例子，不是已运行的示例。可用 D01 `muInfo` 验证查询类路径，用 D02/D04 验证异步/Graph 路径。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/musa/core/symbol.cpp`（4 处，例：[src/musa/core/symbol.cpp:169-192]）
- `src/musa/core/module.cpp`（2 处，例：[src/musa/core/module.cpp:44-46]）
- `src/driver/mu_module.cpp`（1 处，例：[src/driver/mu_module.cpp:286-312]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
