# M02 逐元素算子

- 文档目的：说明标量/向量化 elementwise 与激活算子的通用模式。
- 适用范围：`kernels/elementwise/` 及 ReLU/GELU/ELU/Swish/Hard* 目录。
- 对应源码版本：`4513b31`。
- 证据状态：elementwise 完整代表分析；其他激活按同一模式归纳。
- 最后更新：2026-09-10
- 前置阅读：[架构](../../00-overview/architecture.md)
- 后续阅读：[M08](../M08-pytorch-extension/README.md)

## 结论摘要

M02 用同一个算子展示 scalar、`float4`/`half2`/8 元素 pack 的吞吐差异。Python 脚本用 `load()` 编译 `.cu`，复用输出 tensor，warmup 后同步计时；C++ launcher 根据 ndim/二维 K 和 pack 宽度选择 block/grid。尾部不足一个 vector 的元素走 scalar fallback。

## 关键调用链

```text
 elementwise.py:load
   -> elementwise_add_f32/f16 binding
     -> elementwise_add_*_kernel<<<grid,block>>>
       -> global load + add + global store
     -> Python run_benchmark
       -> torch.cuda.synchronize / output sample / time
```

## 关键证据

- kernel 与尾部处理：`elementwise.cu:22-127`。
- launcher 的 shape 分支和绑定宏：`elementwise.cu:140-197`。
- Python build/benchmark：`elementwise.py:9-24,27-94`。

## 数据和接口契约

输入/输出由 Python 传入，支持 README 列出的 f32/f16 版本；绑定只显式检查 dtype，未统一检查 device、shape 相等或 contiguous，调用者必须保证。`c` 是调用者提供的写回 tensor，不由 kernel 分配。

## 风险与测试

重点测试奇数/非 pack 对齐长度、二维 K 大于 1024、非 contiguous、错误 dtype 和输出复用；当前 README 示例主要是规则二维尺寸。向量化指针重解释要求对齐和合法尾部访问。

## 相关文档

- [design.md](design.md)
- [interfaces.md](interfaces.md)
- [line-level-analysis.md](line-level-analysis.md)
- [testing.md](testing.md)
- [risks-and-debt.md](risks-and-debt.md)

## 源码证据摘要

`[kernels/elementwise/elementwise.cu:32-49]`、`[kernels/elementwise/elementwise.cu:106-127]`、`[kernels/elementwise/elementwise.py:47-63]`。

## 未解决问题

激活目录的 dtype/边界契约需要逐目录核对，不能仅凭 elementwise 推广。

## 深度审计

| 分析对象 | 入口落地 | 正常路径 | 分支 | 异常 | 清理 | 数据生命周期 | 执行上下文 | 行级证据 | Demo 映射 | 状态/缺口 |
|---|---|---|---|---|---|---|---|---|---|---|
| `01-modules/M02-elementwise/README.md` | 已完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 已映射或不适用 | 部分完成：动态行为、边界或专用变体仍需验证 |

## 下一步阅读建议
先阅读 [分析状态](../../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
