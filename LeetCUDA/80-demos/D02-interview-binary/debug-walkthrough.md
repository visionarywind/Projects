# D02-S04 调试 walkthrough

## 构建阶段

```bash
bash -x ./build.sh --arch sm_89
```

（未验证）观察 nvcc 路径、gencode、宏、include、链接库和输出名；若失败，先区分找不到 nvcc、头文件、库、架构选项和 CUDA 编译错误。

## 运行阶段

```bash
./notes_v2_sm89.bin --help
CUDA_LAUNCH_BLOCKING=1 ./notes_v2_sm89.bin --test
compute-sanitizer --tool memcheck ./notes_v2_sm89.bin --test
```

具体 `--test` 语义需以 binary help/源码 CLI 为准，以上均未验证。

## 断点和源码检查点

1. `check()`：第一处 CUDA error 归因；
2. `check_smem_feasible()`：判断配置跳过还是 launch；
3. `test_block_reduce()`：完整 allocation/transfer/sync/free；
4. `launch_timed_hgemm_mma()`：grid、dynamic SMEM、warmup、events；
5. FlashAttention benchmark：stream/event、D2H、max error；
6. TMA map 创建/销毁：descriptor 与 resource 生命周期。

## 性能诊断

先用 correctness 和 sanitizer 排除错误，再用 Nsight Systems 查看 stream/event 时间线，最后用 Nsight Compute 检查 Tensor Core、memory、occupancy 和 register pressure。当前未运行任何 profiler。

## 常见误读

- warmup 后有同步不等于 measured timing 包含/排除了所有初始化；
- event elapsed time 只覆盖其 record 区间；
- `check_smem_feasible` 跳过配置不代表 kernel 错；
- 一个架构 binary 编译成功不代表另一架构可执行。
