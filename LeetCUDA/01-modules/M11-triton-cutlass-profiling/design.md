# M11 设计

M11 有意把 Triton、CUTLASS/CuTe 和 Nsight 材料放在比较/工具边界，而不是并入普通 kernel runtime。第三方 submodule 只作为 include/build 依赖登记，不复制其实现。

证据：`.gitmodules:1-5` 和目录文件清单。
