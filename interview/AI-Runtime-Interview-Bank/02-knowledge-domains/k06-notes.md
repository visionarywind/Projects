# K06 LLM 推理与 KV Cache

## 学习目标

区分 prefill 与 decode 两阶段的瓶颈特征及 TTFT/TPOT 的衡量口径，会用公式估算单请求 KV 占用与并发容量，理解分页池的 block table 映射、前缀共享与 continuous batching 的每步调度，并能设计 KV 全局容量与准入策略。

## 核心机制

- 两阶段特征：prefill 一次并行处理整段 prompt、计算受限，决定 TTFT；decode 每步生成一个 token、访存受限，决定 TPOT。端到端约等于 TTFT + (输出 token 数 − 1) × TPOT。
- KV 容量公式：每 token 字节 = 2（K,V）× 层数 L × KV 头数 H_kv × 每头维度 D × 每元素字节 b；单请求占用再乘序列长度。
- GQA/MQA 与量化：GQA/MQA 通过减少 H_kv 成倍降低 KV；量化把 b 从 2 降到 1，占用减半，需在精度与带宽间权衡。
- 分页池：KV 按固定 block 管理，请求的逻辑块经 block table 映射到不连续物理块，占用按块取整，末块可能浪费。
- continuous batching：每步回收完成请求、为运行请求预留 1 个 token、按 token 预算与 KV 预算从等待队列取请求，组装 decode 与 chunked prefill 的混合 batch。
- 容量与复用：全局块池是唯一容量源，用分位数而非均值估算并发；前缀共享按引用计数复用公共块，冷块可换出到 host 或下层存储。

## 覆盖题目

| ID | 难度 | 标题 | 题型 |
|---|---|---|---|
| B013 | 入门级 | prefill 与 decode 阶段有什么区别？TTFT 和 TPOT 分别衡量什么？ | 概念辨析 |
| I013 | 中等级 | 单个请求的 KV cache 占用如何计算？ | 实现设计 |
| I014 | 中等级 | continuous batching 下调度器如何决定一个 batch 的组成？ | 实现设计 |
| A015 | 高级 | Paged KV Cache allocator 如何设计以支撑高并发服务？ | 系统设计 |
| A016 | 高级 | continuous batching 的调度如何兼顾吞吐与 P99？ | 系统设计 |
| E012 | 专家级 | 推理调度的公平、吞吐和 P99 如何联合设计？ | 治理设计 |
| E013 | 专家级 | KV Cache 全局容量与 admission 如何设计？ | 治理设计 |

## 常见坑

- 计算每 token 占用时忘记乘 2（K 和 V 两份），或用 query 头数代替 KV 头数。
- 只用 prompt 长度估算，漏掉生成长度，低估单请求峰值。
- 把 continuous batching 当成固定 batch 大小，或对 block size 只看浪费不看 kernel 与映射开销。
- 不加 token 预算，让长 prefill 一次性占满批次，阻塞 decode 拖高 TTFT 与 TPOT。
- 用均值估算并发上限，或只看剩余块就准入，长尾请求把块池打穿。

## 个人经历映射

- [P-已确认]：13 年系统软件经验及用户自述的 MindSpore、UMD、显存池、多 Stream、Host 网络、检索引擎、Java 经历类别。
- [P-待补充]：具体模块、提交、故障、指标、规模、本人职责和版本；不得用通用答案替代这些事实。
