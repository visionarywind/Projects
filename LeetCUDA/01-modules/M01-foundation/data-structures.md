# M01 数据结构与生命周期

公共层主要处理 warp/block 索引、向量类型、shared-memory tile、barrier/TMA descriptor 和 accumulator 等 device-side 对象；具体所有权由调用 phase 管理。Interview 测试中的显式分配和释放路径见 `[kernels/interview/notes-v2.cu:100-499]`。
