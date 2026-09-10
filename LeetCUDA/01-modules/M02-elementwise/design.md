# M02 设计

M02 采用同一数学算子对照 scalar 与 packed/vectorized kernel。二维输入在 launcher 中可按 row/block 映射；一维或较大 K 走扁平 grid。pack kernel 对完整向量使用重解释加载，尾部使用标量循环，避免越界。[kernels/elementwise/elementwise.cu:22-49,140-182]

## 不变式

每个逻辑元素只由一个线程写入；`c` 不由 kernel 分配；输入 dtype 必须匹配实例化版本。shape/device/stride 的完整契约由调用方和具体模块补充。
