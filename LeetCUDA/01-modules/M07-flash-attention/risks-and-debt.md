# M07 风险与债务

- source/flag 列表按硬件和 GPU 名称分支，支持矩阵不是统一配置。
- 多种 `[B,H,N,D]`/转置/swizzle layout 易发生 binding 与 kernel 不匹配。
- online softmax、低精度 accumulator 和 tile 边界会产生数值差异。
- 动态 shared memory、barrier 和多 stage 使 sanitizer/架构验证必不可少。
- 官方实现和学习实现的性能不能直接以单次结果比较。
