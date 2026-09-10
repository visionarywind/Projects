# M07 性能

主要观察片上复用、global/shared traffic、Tensor Core utilization、online softmax 开销、动态 shared memory 和同步。固定 shape、GPU、编译宏、warmup/iters 后比较；不要混入扩展编译时间。使用 Nsight 前先通过 correctness 和 sanitizer。

历史 benchmark 仅作参考，未在本机验证。
