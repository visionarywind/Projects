# AI Infra 故障排查案例

- 设备初始化失败：区分驱动、runtime、设备映射、权限和容器挂载。
- GPU OOM 或碎片：记录 allocated/reserved、生命周期、batch、缓存和泄漏。
- RDMA/NCCL 慢或 hang：核对拓扑、collective 顺序、网络计数和 rank 日志。
- Kubernetes 调度成功但任务失败：核对镜像、device plugin、节点 taint、驱动和 runtime。
- 训练恢复不一致：核对 checkpoint manifest、数据位置、RNG、optimizer 和版本。
- 推理 P99 回归：拆分排队、prefill/decode、KV、冷启动、路由和后端 kernel。

