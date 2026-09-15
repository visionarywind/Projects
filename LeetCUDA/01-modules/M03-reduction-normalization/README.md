# M03 归约与归一化

- 文档目的：说明 reduce、softmax、LayerNorm/RMSNorm 的学习边界。
- 适用范围：`kernels/reduce/`, `softmax/`, `layer-norm/`, `rms-norm/` 与 interview base。
- 对应源码版本：`4513b31`。
- 证据状态：目录/接口模式已确认，深层 kernel 为部分分析。
- 最后更新：2026-09-10
- 前置阅读：[M01](../M01-foundation/README.md)
- 后续阅读：[M07](../M07-flash-attention/README.md)

## 结论摘要

M03 处理需要跨线程协作的 reduction 与数值稳定问题：warp/block reduce 负责聚合，safe softmax 先减 row max，online softmax 流式维护统计量，Norm 组合均值/平方均值与仿射参数。它是 FlashAttention 的算法前置知识，但普通目录之间没有统一运行时依赖。

## 设计要点

- reduction 的正确性取决于 warp shuffle/shared memory 和同步范围。
- softmax 的 max/sum 顺序决定溢出和误差；F16 输入可能使用 F32 accumulator。
- row/feature 维度、输入 layout 和尾部处理是主要契约。

## 入口与测试

各目录 Python 脚本是即时扩展和 benchmark 入口；README 中的输出是历史实测。新增实现应以 PyTorch `torch.sum`/`softmax`/`layer_norm`/`rms_norm` 作为参考，固定随机种子并分别测试极值、短行和大行。

## 相关文档

- [design.md](design.md)
- [data-structures.md](data-structures.md)
- [call-chains.md](call-chains.md)
- [testing.md](testing.md)
- [risks-and-debt.md](risks-and-debt.md)

## 源码证据摘要

- `[README.md:279-280]`：reduce、softmax、norm 学习范围。
- `[kernels/interview/notes-v2.cu:11-15]`：基础阶段和 softmax/norm 递进。

## 未解决问题

每个算子支持的 dtype、axis、NaN/Inf 语义和误差阈值需逐文件核验。
