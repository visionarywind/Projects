# M08 开发指南

新增导出函数时同步修改 CUDA/C++ 导出、Python 调用、README 示例和 correctness。binding 不应把裸 data pointer 保存到异步调用生命周期之外；如果需要 stream/contiguous 约束，应显式检查或建立 contiguous copy，并说明所有权和性能代价。

证据规则见 [`../../00-overview/global-error-model.md`](../../00-overview/global-error-model.md)。
