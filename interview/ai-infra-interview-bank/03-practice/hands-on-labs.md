# AI Infra 实践实验

1. 设备与拓扑：记录设备枚举、PCIe/NVLink/RDMA 拓扑、带宽和错误计数。[待验证]
2. 显存与 IO：固定 workload 比较分配、缓存、NUMA、对象存储和本地缓存。[待验证]
3. 容器与调度：用最小 Pod 验证 device plugin、资源请求、队列和抢占恢复。[待验证]
4. 分布式训练：小 world 验证 collective、checkpoint、rank 失败和恢复。[待验证]
5. 推理容量：固定模型和输入分布测 batching、KV、TTFT/TPOT、吞吐和 P99。[待验证]
6. 性能治理：保存 profiler、配置、版本、统计区间和成本，建立回归基线。[待验证]

