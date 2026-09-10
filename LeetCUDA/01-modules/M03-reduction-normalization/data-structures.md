# M03 数据结构

典型数据是按 row/feature 组织的输入、每行的局部 max/sum 或均值/方差，以及输出 tensor。中间统计可能位于 registers/shared memory；并行归约必须匹配同步范围。仓库没有统一结构体或全局 tensor schema，具体布局以各目录 binding/kernel 为准。

已确认的总体阶段见 `[kernels/interview/notes-v2.cu:11-15]`；逐算子布局仍属待核验内容。
