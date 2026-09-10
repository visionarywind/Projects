# M09 设计

M09 将约 30 个从基础到高级的 CUDA kernel 放入一个编译单元，以 phase 和 CLI 组织学习、correctness 与 benchmark；架构脚本负责按 SM 选择宏、库和输出名。该整合便利学习，但使编译单元和公共 header 变更影响半径较大。

证据：`[kernels/interview/notes-v2.cu:1-20,33-53]`；`[kernels/interview/build.sh:45-81]`。
