# M09 设计

M09 分离 Khronos ICD loader 与 NVIDIA vendor implementation：loader 管理动态库、platform list 和 ABI dispatch；`src/cl` 管理 OpenCL 对象和异步 DAG；底层复用 CUDA CUI 的 context/memory/stream 机制。

图形互操作通过 feature/平台条件暴露，入口存在不等于外部 GL/D3D resource 生命周期已经验证。
