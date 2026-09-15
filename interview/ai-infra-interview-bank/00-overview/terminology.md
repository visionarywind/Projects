# AI Infra 术语表

| 术语 | 含义 | 易混点 |
|---|---|---|
| device plugin | 向调度器暴露加速器资源的节点组件 | 不等同于驱动或 runtime |
| NUMA | CPU、内存和 PCIe 的局部性拓扑 | 不只影响 CPU 线程 |
| RDMA | 绕过部分内核路径的远程内存访问 | 仍需要拓扑、权限和拥塞治理 |
| collective | 多进程协作通信操作 | 调用成功不等于全局进度 |
| gang scheduling | 作业所需资源成组调度 | 与单 Pod 排队不同 |
| preemption | 回收资源让高优先级任务运行 | 需要 checkpoint/恢复语义 |
| KV Cache | 推理保存历史 key/value 的缓存 | 容量和碎片决定并发 |
| SLO | 服务级目标，如吞吐、P99、可用性 | 不等同于单次 benchmark |

