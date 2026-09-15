# M02 开发指南

- 对应源码版本：`main` / `4513b31`。
- 证据状态：基于 elementwise 代表实现；CUDA 命令未在本机验证。
- 最后更新：2026-09-10

## 新增一个 elementwise 变体

1. 从 `elementwise.cu:22-127` 选择标量、x2/x4/x8 或 pack 的线程映射模式。
2. 增加具体 `__global__` kernel，并明确完整 pack 和尾部条件。[kernels/elementwise/elementwise.cu:32-49]
3. 在 `TORCH_BINDING_ELEM_ADD` 后增加实例化，令 wrapper 的 `n_elements` 与 kernel 的 `idx` 步长一致。[kernels/elementwise/elementwise.cu:184-189]
4. 在 `PYBIND11_MODULE` 中注册 Python 名称。[kernels/elementwise/elementwise.cu:191-198]
5. 在 `elementwise.py` 里用连续、非 pack 对齐和参考 `torch.add` 的小输入调用新函数，再加入 benchmark。[kernels/elementwise/elementwise.py:69-95]
6. 更新 M02 source map、接口契约、风险和证据索引。

## 修改现有行为

- 若改变输入 shape/layout 契约，先修改 wrapper 的显式检查，再修改 kernel 的 offset；不能只在 Python 侧调用 `.contiguous()` 后把约束隐去。
- 若改变 pack 宽度，检查 `block = 256/n_elements`、`idx` 乘数、128-bit 对齐和 tail loop 四者是否一致。
- 保留 scalar 版本作为 correctness 基线；不要用性能输出来代替数值对拍。

## 修复 bug 的最小验证矩阵

| 用例 | 目的 |
|---|---|
| `N=0`、`N=1` | empty/单元素边界 |
| `N=pack-1, pack, pack+1` | tail 分支 |
| 二维 `K/n_elements <= 1024` 与 `>1024` | 两种 launcher 映射 |
| `a,b,c` shape 不一致 | 确认入口拒绝或记录当前错误 |
| `.transpose()` 非 contiguous view | 验证 stride 契约 |
| wrong dtype/device | 验证错误边界 |
| `torch.add` 对拍 | 数值正确性 |

这些是开发建议和未执行测试，不是当前仓库已有的完整测试结果。当前脚本只展示规则二维尺寸并打印样本。[kernels/elementwise/elementwise.py:69-95]

## 调试顺序

1. 在 `load()` 处确认实际编译源文件和编译错误。[kernels/elementwise/elementwise.py:8-24]
2. 在生成 wrapper 入口观察 `dtype`、`ndim`、`K`、`N` 和 `block/grid`。[kernels/elementwise/elementwise.cu:140-180]
3. 用 `CUDA_LAUNCH_BLOCKING=1` 运行小尺寸 tail case，区分 launch 错误和异步读写错误。
4. 用 compute-sanitizer memcheck 检查 packed pointer 的越界和非对齐访问；该命令需要实际 CUDA 环境，当前未运行。
5. 最后再用同步计时比较性能，避免把错误 kernel 的快结果当作优化结果。

## 回滚

新变体若无法通过 scalar 对拍，应撤回其 PyBind 实例化和 Python benchmark 调用，保留原有导出；不要删除 scalar 基线，因为它承担最小可读性和回归参考作用。

## 相关文档

- [implementation.md](implementation.md)
- [testing.md](testing.md)
- [risks-and-debt.md](risks-and-debt.md)

## 源码证据摘要

- `[kernels/elementwise/elementwise.cu:140-198]`：新增变体需要同步的 wrapper/export 位置。
- `[kernels/elementwise/elementwise.py:69-95]`：现有 Python 调用矩阵。

## 未解决问题

仓库没有针对 M02 的独立 pytest/单元测试；需要决定是扩展现有脚本还是建立小型 CUDA correctness harness。

## 下一步阅读建议

先完成 `testing.md` 的边界矩阵，再阅读 M08 的 ABI 和 tensor ownership 说明。
