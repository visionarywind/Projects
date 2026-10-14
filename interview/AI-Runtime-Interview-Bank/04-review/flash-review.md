# 快速复习清单

## 一分钟必须说清

- API 返回、命令入队、设备完成、host 可读的差异；
- segment/block/free/deferred、allocated/reserved 的差异；
- 同 Stream 顺序与跨 Stream event 依赖；
- buffer 的 owner、借用者和安全复用条件；
- AllReduce 与 ReduceScatter/AllGather 的语义；
- prefill/decode、TTFT/TPOT 和 KV bytes/token；
- baseline、指标、控制变量和 oracle。

## 五分钟必须展开

- Framework→Runtime→UMD/Driver→Device 链路；
- allocator 状态机和 OOM 顺序；
- 多 Stream producer/consumer 时序；
- overlap pipeline 的 chunk/buffer/event；
- 现象→假设→证据→止损→修复→回归；
- 个人项目的职责、关键取舍和真实结果。

## 专家追问检查

- 约束和假设是什么？
- 方案的上限和失效边界是什么？
- 如果出现部分失败/取消/超时怎么办？
- 如何避免 double free、UAF、collective hang 和 cache lock 泄漏？
- 如何同时测容量、吞吐、P99、成本和正确性？
- 迁移、灰度、回滚和兼容策略是什么？
- 哪些结论是个人事实、源码事实、推断、建议或待验证？

## 个人经历关键词

```text
13 年系统软件经验
MindSpore framework/runtime
UMD/device software
memory pool / allocator
multi-stream design
host networking / topology
retrieval engine / cache / low latency
Java server engineering
```

关键词必须后接证据，不要单独罗列。
