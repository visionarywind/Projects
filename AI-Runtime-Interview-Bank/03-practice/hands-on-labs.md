# 实验与代码练习

> 下列练习默认只给方案，未在当前环境运行的结果均为 `[待验证]`。先 correctness，再性能；先单进程/小规模，再考虑多卡、RDMA 和真实模型。

## Lab 1：多 Stream 生命周期最小实验

- 输入：两个 stream、一个 device tensor、一个生产 kernel、一个消费 kernel；可选 event。
- 步骤：
  1. 在 Stream A 写入 sentinel；
  2. 记录 event；
  3. Stream B 等待 event 后读取；
  4. 对照组取消 wait，重复多次；
  5. 分别在同步前后尝试释放/复用 buffer。
- oracle：有明确 happens-before 时输出稳定正确；无依赖的对照组不能被当作必然错误，但若出现错误说明存在竞态窗口；释放/复用必须晚于所有合法使用。
- 观测：event 时间、stream 状态、错误暴露点、输出 checksum、重复运行一致性。
- 环境：需要兼容 CUDA/PyTorch；当前未确认，状态 `[待验证]`。

## Lab 2：Toy caching allocator

实现 CPU 版或 GPU 版最小 allocator：

- size class/free list；
- split/merge；
- allocated/reserved/free/deferred 计数；
- event 模拟的延迟回收；
- debug owner、double-free 和 UAF 检查。

### 测试输入

- 分配 `[4, 8, 16, 8, 4]` 后交错释放；
- 产生大块空闲但无法满足连续分配；
- 同一块在 event 完成前释放和再次分配；
- 重复 free、错误 owner、随机 allocate/free。

### oracle

```text
live + free + deferred == reserved
live 集合与 free 集合互斥
未完成 event 的 block 不得进入可复用 free list
每个 allocation 恰好对应一次合法 release
```

## Lab 3：显存 OOM 诊断表

构造一组记录：总显存、allocated、reserved、最大空闲块、deferred release、workspace、通信 buffer、请求大小。要求先判断：

1. 物理容量不足；
2. allocator 保留；
3. 外部碎片；
4. 异步操作未完成；
5. 非 allocator 的 workspace/通信占用。

不能只根据 `free` 总量下结论。

## Lab 4：AllReduce 带宽估算

对 ring AllReduce 写出消息传输量、步骤数和近似时间模型。明确变量：rank 数 `P`、消息大小 `N`、链路带宽 `B`、每步启动时延 `α`。再比较 tree/hierarchical 的适用场景。

实验 oracle：模型数量级与固定环境 benchmark 的趋势一致；未运行状态 `[待验证]`，不能填写虚构 TFLOPS/GB/s。

## Lab 5：KV Cache 容量手算

一阶公式：

```text
bytes_per_token = layers × 2(K/V) × kv_heads × head_dim × dtype_bytes
max_tokens ≈ floor((HBM - weights - workspace - graph - comm - margin) / bytes_per_token)
```

再计算 `page_size`、partial page、prefix hit 和 page waste 对有效容量的影响。不要把手算结果写成某个真实模型的实测结果。

## 工具顺序

1. reference 对拍和边界输入；
2. debug assertion、日志和资源计数；
3. compute-sanitizer/ASan/TSan（需要对应环境）；
4. Nsight Systems/Compute、perf、火焰图；
5. 只有获得授权且具备环境后，才做 NCCL/RDMA、多进程、多卡和真实模型压测。
