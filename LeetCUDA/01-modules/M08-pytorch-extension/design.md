# M08 设计

M08 把 PyTorch tensor 作为跨语言边界：Python 负责实验编排，C++/PyBind 负责导出与检查，CUDA kernel 负责并行实现。仓库同时保留即时 `load()` 和 `CUDAExtension` wheel，以便学习不同构建生命周期，而非形成统一 runtime。

证据：`[kernels/elementwise/elementwise.py:9-24]`、`[kernels/hgemm/setup.py:42-95]`。
