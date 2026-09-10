# M08 运行时边界

普通脚本在 import/执行时调用 `load()`，编译和 module cache 可能成为首次运行副作用；wheel 路径在安装时构建 CUDAExtension；standalone interview 不经过 PyTorch binding。CUDA kernel 通常异步，Python benchmark 用 synchronize 建立计时边界，但全仓库 current stream policy 未统一确认。

证据：`[kernels/elementwise/elementwise.py:9-24,27-66]`；`[kernels/hgemm/setup.py:42-95]`。
