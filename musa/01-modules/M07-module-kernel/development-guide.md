# Module 与 Kernel 执行：开发指南

- 文档目的：module kernel：常见修改场景的落地入口、注意点与验证方式。
- 适用范围：M07 模块的《Module 与 Kernel 执行：开发指南》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


## 修改前

1. 锁定源码提交并更新证据行号：本模块证据锚点 `muapiModuleLoad`/`muapiLaunchKernel` 在 `[src/driver/mu_module.cpp:87-146] [src/driver/mu_module.cpp:231-268]`，`GeneralLaunchKernel` 在 [src/musa/core/context.cpp:631-677]。
2. 判断改动属于 ABI（M02 约束）、Core 生命周期、HAL 适配还是观测层。
3. 画出调用链和资源所有权，再改代码。launch 链路要同时考虑 graph/capture 与 stream 两条路径。[src/musa/core/context.cpp:666-673]

## 修改中

- 保持现有 v1/v2 wrapper 风格：`muapiModuleGetGlobal_v2` 与 `muapiModuleGetGlobal` 的互转是现成模板。[src/driver/mu_module.cpp:180-205]
- 新增函数入口先做与现有入口一致的校验顺序：`InitPlatform` -> 句柄/参数检查 -> context 解析 -> 核心调用。[src/driver/mu_module.cpp:87-120]
- 新增 launch 属性先在 `muapiLaunchKernelEx` 的 attr switch 中明确支持或返回 `MUSA_ERROR_NOT_SUPPORTED`。[src/driver/mu_module.cpp:286-312]
- 所有新建异步资源明确完成条件和持有者；`KernelState`/buffer 的释放时机参照 `ReleaseResources` 与析构的 `Wait()`。[src/musa/core/command/dispatchCommand.cpp:57-64] [src/musa/core/command/dispatchCommand.cpp:1277-1292]
- HAL 错误通过既有转换边界返回（`HalToMuResult`/`M3dToHalResult`），不要在 driver 层直接发明映射。[src/musa/core/module.cpp:39-42]
- 新增隐式资源类型需同时改 HAL 枚举、`kernel.h` static_assert、`UpdateImplicitResources` 的分支处理。[src/hal/halKernel.h:70-104] [src/musa/core/command/dispatchCommand.cpp:782-1033]
- 不在没有硬件证据时改变同步语义（`launchBlocking`、PDL、capture 判定）。

## 修改后

- 运行 `python3 verify_kb.py` 与 `python3 verify_content.py` 校验引用与行号一致性。
- 先做静态 diff/格式检查，再在授权目标机做最小构建和专项测试；安装脚本只在明确授权时执行。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/driver/mu_module.cpp`（5 处，例：[src/driver/mu_module.cpp:87-146]）
- `src/musa/core/command/dispatchCommand.cpp`（3 处，例：[src/musa/core/command/dispatchCommand.cpp:57-64]）
- `src/musa/core/context.cpp`（2 处，例：[src/musa/core/context.cpp:631-677]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
