# M03 设计

M03 的共同设计是把跨线程聚合和数值稳定性分开处理：warp/block reduction 聚合局部结果，softmax 先获得 row max 再累加指数，online 变体流式维护统计量，normalization 使用适当 accumulator。具体 axis、dtype 和布局必须以每个 `.cu` 为准。

证据：`[README.md:279-338]`、`[kernels/interview/notes-v2.cu:11-15]`。
