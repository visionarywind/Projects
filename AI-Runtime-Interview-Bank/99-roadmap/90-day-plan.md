# 90 天进阶路线

## 目标岗位

主攻 AI Framework/Device Runtime、Memory/Stream/性能；辅攻集合通信/RDMA；用 LLM Inference/KV Cache 扩展岗位面。

## 1–30 天：把已有经历讲深

- 完成首批 B001/B002/I001/A001/A002；
- 填完 MindSpore、UMD、多 Stream、Host 网络三个事实卡；
- 每天画一张时序/状态/故障图；
- 写 Toy allocator，做碎片和 deferred reclaim；
- 准备 1 分钟自我介绍和 3 个项目案例；
- 补 C++ RAII、原子、锁、Linux 调试和内存模型。

## 31–60 天：补岗位硬技能

- 阅读目标版本 PyTorch allocator、CUDA stream/event/memory pool 官方资料；
- 补 MindSpore 目标版本的 Framework/Runtime/Device Adapter 调用链；
- 学习 NCCL/HCCL/MPI collective、RDMA QP/CQ/MR、NUMA/NIC affinity；
- 做 AllReduce 带宽模型和 stream overlap 模拟；
- 练 OOM、UAF、collective hang、异步错误四类故障题；
- 若有环境且获授权，运行低成本 correctness/sanitizer/profile 实验并记录版本。

## 61–90 天：扩展 LLM Serving 和岗位化投递

- 学习 Attention/KV Cache、PagedAttention、continuous batching、prefix cache；
- 做 KV bytes/token、page waste、admission 和 retraction 设计；
- 形成 Runtime、通信、推理三版简历摘要；
- 针对字节 Agent/算力 Infra、腾讯训练框架/云 AI/存储岗位做 JD 映射；
- 每周至少两次完整模拟面试；
- 对所有个人指标做脱敏和证据审查，绝不编造。

## 90 天过关判定

| 项目 | 过关标准 |
|---|---|
| 自我介绍 | 1 分钟说清主线，2 分钟能落到一个真实项目 |
| Runtime | 5 分钟讲清调用链、异步边界、错误和清理 |
| Memory | 能手算碎片并设计 deferred allocator |
| Stream | 能画跨 Stream 依赖和安全复用证明 |
| Network | 能解释 collective、拓扑、overlap 和 hang 排查 |
| Inference | 能做 KV 容量估算和分页池设计 |
| 项目 | 3 个案例均能说明本人职责、取舍、指标、失败和复盘 |
| 证据 | 明确区分已做、参与、了解、推断和待验证 |
