# M09 风险与债务

- 固定 `/usr/local/cuda/bin/nvcc` 降低可移植性。
- 多 SM 宏、库和输出名存在环境矩阵风险。
- 单一编译单元增量编译慢、公共 header 改动影响广。
- 显式资源管理路径多，新增测试可能遗漏 free/error cleanup。
- benchmark globals 与 CLI 关系需持续维护。

证据：`[kernels/interview/build.sh:13-43]`、`[kernels/interview/notes-v2.cu:33-97]`。
