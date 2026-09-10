# M09 测试

先按单架构构建并执行最小 correctness/smoke，再执行选定 benchmark；每个显式 `malloc`/`cudaMalloc` 都要确认释放和错误路径。测试动态 shared-memory feasibility、架构宏、TMA/MMA 可用性和 reference max error。当前未执行。

证据：`[kernels/interview/notes-v2.cu:82-97,100-499]`。
