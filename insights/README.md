# KV Cache 池化技术专题

- 主题：大模型推理中的 KV cache 池化、动态显存池、静态显存池划分与跨层资源管理。
- 适用读者：准备推理系统、SGLang/vLLM、长上下文、GPU 显存管理和系统设计面试的工程师。
- 证据基线：本仓库 SGLang 知识库锚定 `source/sglang` HEAD `78be4b50af`；外部论文以 arXiv 页面为索引。未执行 GPU、模型、RDMA、压测或端到端服务。
- 状态约定：`[已确认]` 来自当前仓库文档/源码引用；`[推断]` 为基于证据的工程推导；`[待验证]` 需要实验或线上指标；`[建议]` 为设计建议；`[存在争议]` 表示不同系统或论文方案取舍不同。

## 阅读顺序

1. [概念与分类轴](00-concepts-and-taxonomy.md)：先把“静态/动态池”这组词拆开，避免把 CUDA allocator、KV slot allocator 和 prefix cache 混为一谈。
2. [静态、动态与混合池方案](01-static-dynamic-hybrid-pools.md)：比较启动预留、按需分配、分页/虚拟化和异构内存方案。
3. [KV pool 生命周期与 ownership](02-kv-pool-lifecycle-and-ownership.md)：结合 SGLang 三层池模型追踪分配、命中、淘汰、retraction 和 flush。
4. [arXiv 论文阅读笔记](03-arxiv-reading-notes.md)：梳理 PagedAttention、vAttention、NEO、P/D-Serve、eLLM 等论文能直接支持的结论。
5. [设计决策与验证路线](04-design-decision-and-validation.md)：给出容量公式、指标、故障注入和面试复盘问题。

## 专题核心结论

- [已确认] SGLang 文档中的池化模型至少分三层：request row 映射池、token/page 索引 allocator、物理 KV/Mamba tensor pool；RadixCache 保存的是 KV index 引用和锁状态，不是另一份 KV 数据副本。来源：`../../../sglang/90-cross-module/pooling-and-resource-management.md`。
- [推断] “静态显存池”和“动态显存池”不是二选一：生产推理系统常见的是**启动时预留物理 backing + 请求期间动态分配逻辑 slot/page + 按策略动态移动/淘汰 cache**的混合方案。
- [已确认] PagedAttention 把 KV cache 管理类比为分页，目标是降低动态 KV 带来的碎片和浪费；vAttention 则尝试用 CUDA virtual memory 保持虚拟连续、物理动态映射。来源见 [论文笔记](03-arxiv-reading-notes.md)。
- [待验证] 本专题没有复现任何论文吞吐提升、显存节省、D2D 传输优化或 SGLang 真实 GPU 行为；相关数字只作为论文报告结果记录。

## 面试/设计复盘主线

```mermaid
flowchart LR
    REQ[请求与输出长度不确定] --> BUDGET[容量预算]
    BUDGET --> ROW[request row pool]
    BUDGET --> SLOT[token/page allocator]
    SLOT --> PHY[physical KV tensors]
    ROW --> RAD[Radix/prefix cache]
    RAD --> EVICT[eviction / reuse]
    PHY --> ATT[attention kernels / CUDA Graph]
    PHY --> TIER[CPU/DRAM/storage/network tiers]
    TIER --> RESTORE[restore / transfer / recompute]
```

图中 `ROW/SLOT/PHY/RAD` 分别代表不同 owner 和生命周期；`TIER` 是异构/分离部署扩展，不等同于基础 GPU allocator。
