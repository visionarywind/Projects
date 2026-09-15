# M02 测试

- 文档目的：解释 01-modules/M02-elementwise/testing.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：`main` / `4513b31`。
- 证据状态：测试矩阵为开发建议；本环境未执行 GPU 测试。
- 最后更新：2026-09-10
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M02-elementwise/testing.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。

## Correctness reference

固定验证每个 f32/f16 binding 与 `torch.add(a, b)` 对拍，并在比较前同步 GPU。scalar 版本既是导出变体也是 packed 版本的最小 correctness 基线；不要用 benchmark 时间代替数值验证。[kernels/elementwise/elementwise.py:27-66,69-95]

## 最小矩阵

| 维度 | 用例 | 目的 |
|---|---|---|
| 元素数 | `N=0,1,pack-1,pack,pack+1` | empty、首元素和 scalar tail |
| 二维 dispatch | `K/n_elements <= 1024`、`>1024` | 一行一个 block 与扁平 grid |
| 形状 | 不同 `S/K`、`a/b/c` 不一致 | 记录入口拒绝或当前未定义行为 |
| layout | contiguous、`.transpose()` view、带 stride view | 检查 raw pointer 假设 |
| dtype/device | FP32、FP16、CPU tensor、跨 device | 入口错误边界 |
| 数值 | 零、负数、极大值、NaN/Inf | 加法语义和 fast-math 影响 |
| pack | 地址可对齐与非 pack 倍数 | 128-bit/vector tail |

## 运行观测

脚本当前只使用规则二维尺寸 `[1024,2048,4096]` 的组合，预先创建 contiguous CUDA tensor，warmup 后 `torch.cuda.synchronize()`，再使用 wall-clock 计时并打印样本；这些调用是 benchmark 示例，不是独立 pytest harness。[kernels/elementwise/elementwise.py:27-95]

## 调试矩阵

1. 先运行 scalar/reference 对拍。
2. 对 tail 用 `CUDA_LAUNCH_BLOCKING=1`，区分 launch 与异步访问错误。
3. 对 packed 变体用 `compute-sanitizer --tool memcheck` 检查越界/未对齐访问；该命令当前未执行。
4. 只有 correctness 和边界通过后再比较同步计时。

## 当前状态

当前 GPU 测试、sanitizer、benchmark 和非连续输入实验均未执行；不能据此声称任何变体已在当前环境验证。

## 相关文档

- [implementation.md](implementation.md)
- [development-guide.md](development-guide.md)
- [risks-and-debt.md](risks-and-debt.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](../../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
