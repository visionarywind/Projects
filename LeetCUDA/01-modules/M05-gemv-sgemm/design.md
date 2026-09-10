# M05 设计

M05 以多个 kernel 版本展示优化阶梯：naive 每线程计算一个 C 元素；shared-memory sliced-K 复用 tile；thread tile 增加寄存器计算密度；vectorized load/store 减少指令；double buffer 重叠下一 tile 载入和当前计算。[kernels/sgemm/sgemm.cu:20-482]

GEMV 是相邻但独立的矩阵向量路径，不能把 SGEMM 的 layout 契约直接套用。
