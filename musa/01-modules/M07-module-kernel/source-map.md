# Module 与 Kernel 执行：源码地图

- 文档目的：module kernel：源码文件/目录到文档章节的映射。
- 适用范围：M07 模块的《Module 与 Kernel 执行：源码地图》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


| 层 | 路径 | 关注点 |
|---|---|---|
| Driver 入口 | `src/driver/mu_module.cpp` | `muapiModuleLoad`、`muapiModuleLoadData`、`muapiModuleGetFunction`、`muapiLaunchKernel`、`muapiModuleUnload` |
| Core | `src/musa/core/module.cpp/h`、`library.cpp/h` | Module/Function/Kernel/Library 对象、符号表、加载模式 |
| Command | `src/musa/core/command/dispatchCommand.cpp/h` | `DispatchCommand` 构建 cmd buffer 与提交 |
| Graph/Stream | `src/musa/core/context.cpp`、`stream.cpp`、`node/graphKernelNode.cpp` | `GeneralLaunchKernel`、`CreateKernelNodeOnStream`、`CmdLaunchKernel` |
| HAL/M3D | `src/hal/m3d/kernel.cpp/h`、`library.cpp`、`cmdBuffer.cpp`、`queue.cpp` | HAL kernel metadata、pipeline、`CmdDispatch`、`Submit` |
| 观测 | `src/driver/mupti`、`mugdb`、`muasan` | trace/accessor/debug 接入点 |

## 证据锚点

[src/driver/mu_module.cpp:87-146] [src/driver/mu_module.cpp:231-268] [src/musa/core/module.cpp:20-54] [src/musa/core/context.cpp:631-677] [src/musa/core/command/dispatchCommand.cpp:78-300]

## 搜索建议

```bash
grep -R "module" src/driver src/musa/core src/hal
```

上述命令是阅读建议，不代表已在本地或远端执行。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/driver/mu_module.cpp`（2 处，例：[src/driver/mu_module.cpp:87-146]）
- `src/musa/core/module.cpp`（1 处，例：[src/musa/core/module.cpp:20-54]）
- `src/musa/core/context.cpp`（1 处，例：[src/musa/core/context.cpp:631-677]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
