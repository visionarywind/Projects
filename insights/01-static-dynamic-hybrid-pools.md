# 静态、动态与混合显存池方案

## 1. 方案总览

| 方案 | 物理 backing | 逻辑分配 | 优点 | 主要代价 | 适用场景 |
|---|---|---|---|---|---|
| 全静态 per-request 预留 | 每请求按最大长度预留 | 简单 | 地址稳定、实现简单 | 输出短时浪费极大，batch 受限 | 小规模、长度上界稳定、低并发 |
| 启动静态大池 + 动态 slot/page | 启动创建大 KV tensors | 请求按 token/page 领取 | 地址稳定，适合 CUDA Graph，降低运行期 malloc | 容量上限固定，需要 eviction/retraction | vLLM/SGLang 风格在线 serving |
| 运行期动态物理分配 | 按需 malloc/free 或扩展 | 按请求增长 | 表面利用率高 | allocator 碎片、同步、尾延迟、不利 graph | 原型、小 batch、对稳定性要求低 |
| 虚拟连续 + 物理动态映射 | 虚拟地址预留，物理页按需 map | 连续虚拟 KV | kernel 看到连续布局，物理可动态 | 依赖 CUDA VMM，map/unmap 策略复杂 | vAttention 类设计 |
| 异构/分层 cache | GPU + CPU/DRAM/storage/network | placement/restore/recompute 动态 | 扩展上下文和并发容量 | 传输瓶颈、一致性、调度复杂 | 长上下文、P/D 分离、企业缓存层 |

## 2. 启动静态大池 + 运行期动态 slot/page

这是当前主流 serving 系统最常见的折中：

1. 启动或 profile 后确定 `max_total_tokens`、`page_size`、dtype、层数和 head layout；
2. 一次性创建物理 K/V tensors 或平台相关 backing；
3. allocator 维护 free slot/page index；
4. 每个请求在 prefill/decode 期间动态领取逻辑位置；
5. 请求结束、prefix cache 插入、eviction、retraction 或 flush 时归还 ownership。

[已确认] SGLang 文档中的 `ReqToTokenPool`、`TokenToKVPoolAllocator/PagedTokenToKVPoolAllocator` 和物理 `KVCache` 正是这种分层建模。来源：`../sglang/90-cross-module/pooling-and-resource-management.md`。

### 2.1 为什么它像“动态池”

从请求视角看，请求没有固定 KV 长度；它随 token 生成动态增长，prefix 命中部分不重新分配，缺口部分再领取 slot/page。

### 2.2 为什么它又像“静态池”

从 GPU backing 视角看，物理 K/V tensor 的地址和容量在启动阶段确定；attention kernel 和 CUDA Graph 可基于稳定地址读写局部 view。

## 3. PagedAttention 风格分页池

[已确认] PagedAttention/vLLM 论文将 KV cache 管理类比为虚拟内存分页，目标是减少动态 KV cache 的碎片和冗余复制，并报告在论文评测中达到接近零 KV cache waste 与 2–4x 吞吐提升。来源：[Efficient Memory Management for Large Language Model Serving with PagedAttention](https://arxiv.org/abs/2309.06180)。

工程理解：

```text
logical sequence blocks  -> block table -> physical KV blocks
request A: [b1, b2, b3]
request B: [b1, b2, b9]  # prefix blocks 可共享/引用
```

优势：
- 减少变长序列导致的外部碎片；
- 支持 prefix 或 beam/search 场景中的 block 共享；
- admission 可以用 free block 数做容量约束。

代价：
- block/page 内仍可能有内部碎片；
- attention kernel 需要处理非连续物理块或 block table；
- eviction、引用计数和 partial block 边界更复杂。

## 4. vAttention 风格虚拟地址方案

[已确认] vAttention 论文主张用 CUDA virtual memory API 分离虚拟地址和物理显存，使 KV cache 在虚拟地址上保持连续，同时动态管理物理 backing；论文称这样可减轻 PagedAttention 非连续虚拟布局带来的 kernel 编程复杂度和开销。来源：[vAttention: Dynamic Memory Management for Serving LLMs without PagedAttention](https://arxiv.org/abs/2405.04437)。

适用判断：
- 如果 attention kernel 生态高度依赖连续 KV layout，虚拟连续可能降低适配成本；
- 如果平台 CUDA VMM 支持、map/unmap 开销和 page granularity 不合适，收益可能被管理开销抵消；
- 它解决的是“虚拟布局连续性 + 物理动态性”的矛盾，不自动解决输出长度预测、prefix eviction 或跨节点 transfer。

## 5. 异构内存与 offload 池

[已确认] NEO 将部分 attention 计算和 KV cache 状态迁到 CPU，以减轻 GPU 显存压力，并通过 GPU/CPU 非对称流水和负载感知调度缓解瓶颈；论文报告不同 GPU/CPU 组合下收益差异显著。来源：[NEO](https://arxiv.org/abs/2411.01142)。

[已确认] KVPR 把 CPU offload 和 GPU partial recomputation 结合，用额外计算换取更好的 PCIe 传输重叠。来源：[KVPR](https://arxiv.org/abs/2411.17089)。

设计要点：
- offload pool 必须定义放置粒度：token、page、layer、head、request 还是 prefix segment；
- 必须定义恢复时机：decode 前同步、layer-ahead 预取、按 attention 稀疏度选择，还是 miss 后阻塞；
- 必须观测 transfer time、GPU idle、CPU 利用率、P99 TPOT 和 recompute 开销。

## 6. 静态/动态划分决策矩阵

| 约束 | 推荐倾向 | 原因 |
|---|---|---|
| 需要 CUDA Graph decode replay | 固定物理 backing + 静态 input buffers | graph capture 依赖稳定地址和 shape |
| 输出长度分布波动大 | 动态 slot/page + 保守 reservation | 避免最大长度预留浪费 |
| 长上下文但 GPU 显存不足 | 分层 offload 或 recompute | 用带宽/计算换容量 |
| attention kernel 不支持 paged layout | 虚拟连续或改 kernel | 避免 block table 适配成本 |
| 多租户强隔离 | 配额化 pool + admission | 防止单租户吃光 KV |
| P/D 分离部署 | KV transfer pool + lifecycle protocol | prefill 产出的 KV 需要跨实例恢复 |

## 7. 失效模式

| 失效模式 | 典型症状 | 根因方向 |
|---|---|---|
| 过度静态预留 | 并发低、显存空洞大 | 按 max context/reservation 分配过多 |
| 运行期动态 malloc 抖动 | P99 上升、偶发 OOM | allocator 碎片/同步/回收不及时 |
| page 内部碎片 | free block 不少但可用 token 偏低 | page_size 太大或 partial page 多 |
| offload 恢复阻塞 | TPOT 尾延迟尖峰 | PCIe/NVLink/CPU 或调度预测不足 |
| prefix cache 锁泄漏 | 可驱逐容量持续下降 | lock/ref 未配对或 abort 路径漏清理 |
