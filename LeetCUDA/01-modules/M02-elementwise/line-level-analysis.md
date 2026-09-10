# M02 行级分析

1. kernel 计算线程/元素索引并做边界判断。[kernels/elementwise/elementwise.cu:22-49]
2. packed kernel 对完整 pack 使用 `float4`/half pack，尾部回退标量循环。[kernels/elementwise/elementwise.cu:32-49,106-127]
3. binding 先做 dtype 检查，再依据 ndim/K 计算 block/grid。[kernels/elementwise/elementwise.cu:140-182]
4. Python benchmark warmup、同步并测量均值。[kernels/elementwise/elementwise.py:27-66]

未在源码中确认统一的异步错误检查。
