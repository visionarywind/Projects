# 性能指南

## 观察对象

- GPU utilization 与 kernel timeline
- TP/DP/PP/CP collective 时间
- pipeline bubble 和 stage imbalance
- dataloader stalls
- activation recompute/offload 成本
- checkpoint I/O 时间
- inference KV cache occupancy 和 request latency

## 源码入口

- M01：attention/MLP/backend spec
- M02：schedule/P2P/overlap/group
- M03：CUDA graph、profiling、logging
- M04：data loader 和 batch 搬运
- M05：grad reduce、distributed optimizer、checkpoint
- M06：dynamic batching、KV allocator、sampling

本文只提供性能排查地图；具体优化结论必须来自 profiler/benchmark。