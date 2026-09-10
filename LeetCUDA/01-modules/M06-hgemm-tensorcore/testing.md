# M06 测试

先对 torch.matmul/cuBLAS 做小尺寸 correctness，再扩大矩阵。覆盖 M/N/K 非 tile 倍数、NN/TN、F16 input/F32 accumulator、各 stages、swizzle 变体和 `sm_80/sm_89/sm_90a`。检查编译目标、动态 shared memory、对齐和输出 stride；所有当前命令未执行。
