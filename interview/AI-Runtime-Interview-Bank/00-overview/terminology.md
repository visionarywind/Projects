# 术语与易混概念

| 概念 | 准确定义/面试口径 | 不要混淆 |
|---|---|---|
| API 返回 | 调用方完成一次 host API 调用，命令可能只是入队 | 设备工作完成、数据可读 |
| Stream 顺序 | 同一执行队列中的先后关系 | 不同 Stream 自动有序 |
| Event | 可记录某个队列位置并被其他队列等待的依赖标记 | 全局同步、内存屏障的万能替代 |
| allocated | 当前被上层对象/分配记录占用的内存 | 物理显存已用、allocator reserved |
| reserved/cached | allocator 为复用而保留的内存 | 可立即满足任意大小分配 |
| active/busy | 仍有合法使用者或异步操作未完成 | 仅仅有 Python/C++ 指针 |
| 外部碎片 | 空闲总量存在但分散，无法满足请求 | page 内未使用空间 |
| 内部碎片 | 已分配块/page 内未使用的空间 | allocator 的 free 总量 |
| borrowed tensor | binding 使用调用方 Tensor 的 storage | binding 拥有 Tensor/allocator |
| fence/completion | 证明某段工作达到完成条件的机制 | host 函数返回 |
| backpressure | 下游处理不过来，上游被限制/排队 | 无限增大队列 |
| AllReduce | 所有 rank 聚合并得到相同结果 | AllGather/ReduceScatter 的语义 |
| collective hang | 参与者顺序、连接或完成条件无法满足 | 单个 kernel 慢 |
| TTFT | 请求到首 token 的时间 | 单个 attention kernel latency |
| TPOT | 生成阶段每个 token 的时间口径 | 总请求 latency |
| KV logical index | 请求 token 到物理 KV 位置的映射 | 物理 K/V backing tensor |
| prefix hit | 请求前缀复用了已有 KV | cache 总大小或命中请求数 |
| `[推断]` | 由证据合理推出但未被直接确认 | `[已确认]` |
| `[待验证]` | 需实验/目标版本源码/硬件确认 | 已发生的个人项目事实 |

## 回答时的强制辨析

- “异步”至少说明谁异步、完成点在哪里、如何观察；
- “内存够”至少说明总量、可用块、碎片和延迟释放；
- “有网络经验”至少说明协议、设备、数据路径和指标；
- “做过框架”至少说明模块、输入输出、依赖和本人职责；
- “性能提升”必须给 baseline、指标、控制变量和数据来源。
