# M05 风险与技术债

| 风险 | 证据 | 说明 |
|---|---|---|
| 延迟回收 | `[src/cui/cuistream.c:1926-1954]` | marker 未完成时 detached 对象不能复用 |
| 锁序耦合 | `[src/api/apistream.c:83-109]` | context/pool/QMD semaphore 跨锁操作 |
| 多 backend | `[src/cui/channel.c:78-101]` | RM/MRM/WDDM/amodel 行为不同 |
| QMD 轮换容量 | `[src/cui/cuistream.c:1973-1986]` | 并发 launch 对槽位深度有隐含约束 |
| Marker 异构完成源 | tracking/QMD/CPU/UVM/cross-context entry 统一聚合 | 等待、flush、RM interrupt 和 firmware 完成值之间的时序难以单靠 host 源码验证 |
| GPFIFO/pushbuffer 游标耦合 | `gpuGet` 仅在 tracking semaphore 完成后推进 pushbuffer get | tracking 丢失或 DMAL 不推进会表现为长期占用/回收延迟 |

**深度状态**：stream/channel 创建、锁、QMD、pushbuffer/GPFIFO、marker 和 detach/reclaim 已完成 host 侧静态主线；DMAL doorbell/RM/WDDM 末端及 firmware 完成时序仍未完成。

