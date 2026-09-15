# M02 测试

- 对应源码版本：`main` / `4513b31`。
- 证据状态：测试矩阵为开发建议；本环境未执行 GPU 测试。
- 最后更新：2026-09-10

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
