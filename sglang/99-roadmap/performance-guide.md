# 性能指南

## 先分阶段测量

- 请求侧：tokenize、IPC、队列等待；
- prefill：输入 token、prefix hit、chunk 数和 KV 分配；
- decode：batch size、step 时间、retraction；
- worker：forward、collective、attention、sampling；
- 输出侧：copy-to-CPU、detokenize、stream/event 通知。

## 解释指标时的约束

吞吐和延迟受模型、硬件、并行拓扑、KV 容量、workload 和 backend 共同影响。静态源码只能指出关键路径，不能替代 benchmark。

## 调优顺序

1. 确认 workload 和资源是否稳定；
2. 看 prefix/KV 和 admission 是否成为瓶颈；
3. 比较 decode batch 与 prefill budget；
4. 再检查 overlap、CUDA Graph 和 attention backend；
5. 最后分析输出/IPC 和多卡通信。

**未验证**：本批未运行 benchmark、nsys、GPU profiler 或多卡性能测试。
