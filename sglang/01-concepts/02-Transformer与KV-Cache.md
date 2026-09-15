# 03 Transformer 与 KV Cache：为什么缓存能让服务变快

- 文档目的：解释 01-concepts/02-Transformer与KV-Cache.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/sglang HEAD 78be4b50af（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-concepts/02-Transformer与KV-Cache.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## 1. 直观解释

Transformer 的自回归生成要求新 token 关注所有历史 token。注意力计算需要历史 token 的 Key/Value（简称 KV）。如果每生成一个新 token 都重新计算历史 token 的 K/V，计算量会越来越大。

KV cache 的做法是：历史位置的 K/V 第一次算好后保存起来，下一次只计算新 token 的 K/V，然后把它与缓存拼起来参与注意力。

```text
没有 KV cache:
[A B C D E] 每轮都重新算 A..E

有 KV cache:
[A B C D] 保存 KV
生成 E 时只算 E 的 KV，读取 A..D 的 KV
```

## 2. 一个简化的注意力公式

单个注意力头可以写成：

```text
Q = x Wq
K = x Wk
V = x Wv
Attention(Q,K,V) = softmax(Q K^T / sqrt(d)) V
```

prefill 对 prompt 的所有位置计算 Q/K/V，并把 K/V 写入 cache。decode 时，只有最新 token 产生新的 Q/K/V：

```text
Q_new 与 K_history + K_new 做点积
结果乘以 V_history + V_new
```

真实 SGLang 还要处理多头/分组查询头、MLA、滑动窗口、稀疏注意力、上下文并行等变体，所以 cache 的布局由 attention backend 和模型配置共同决定。

## 3. 为什么是 Radix Tree，而不是简单字典

如果缓存只用 `hash(prompt) -> KV`，只能命中完整 prompt。服务中常见的是共享**前缀**：

```text
请求 1: [system, user, question-1]
请求 2: [system, user, question-2]
```

两者共享 `[system, user]`。Radix tree（压缩前缀树）把 token 序列的公共前缀放在同一条路径上：

```text
root
 └── [system, user]
       ├── [question-1]
       └── [question-2]
```

命中时只需沿树比较 token，得到最长公共前缀；未命中的后缀再执行 prefill。

## 4. SGLang 中的三层关系

初学者容易把三个对象混在一起：

1. **GPU KV pool**：真正存放 K/V tensor 的大块显存。
2. **ReqToTokenPool**：把“某请求的第 n 个 token”映射到 pool 中的物理槽位。
3. **RadixCache**：用 token 序列组织哪些 prefix 已经可复用，并保存对应的值/索引。

RadixCache 不一定直接保存整个 K/V tensor；它主要管理前缀路径和 token 索引，实际数据由 memory pool/allocator 维护。

## 5. 逐段看 `RadixCache`

源文件：`python/sglang/srt/mem_cache/radix_cache.py`。

### 5.1 节点和树

`TreeNode`（约第 259 行）是树节点。节点通常包含：

- `key`：该节点上的 token 序列；
- `value`：对应的 KV pool 索引；
- `children`：按 token/hash 找到下一节点；
- 父节点和访问/锁定信息。

树使用压缩边：一条边可以代表多个 token，而不是每个 token 创建一个节点。这样公共前缀长时节点数量更少。

### 5.2 `match_prefix`

`RadixCache.match_prefix` 约在第 400 行。逻辑可概括为：

1. 从根节点开始；
2. 用请求的 token 序列和当前节点 key 比较；
3. 如果整段匹配，累计命中的 token 数并进入子节点；
4. 如果只匹配一部分，说明在压缩边中间结束；
5. 返回命中的 token 数和对应的 cache indices。

关键点是返回“最长匹配前缀”，而不是布尔值。Scheduler 需要知道剩余多少 token 才能决定 prefill 工作量。

### 5.3 `insert`

`insert` 约在第 460 行。当新请求计算出一段 KV 后，系统把 token 序列和物理索引插入树中：

- 完全相同的路径可以复用；
- 新路径作为子节点插入；
- 如果新序列在一条已有边中间分叉，需要 split 节点；
- 节点的访问/引用状态决定它能否被淘汰。

### 5.4 `cache_finished_req`

`cache_finished_req` 约在第 482 行。请求结束时，已经完整计算的 token 仍可能对未来请求有价值，所以结束不等于立即释放所有 KV。此函数将请求的可缓存路径并入 prefix tree，同时处理是否插入、锁定和索引回收。

### 5.5 `evict`

`evict` 约在第 616 行。显存不足时，allocator 会要求 cache 释放 token。典型策略是从可淘汰的叶子开始，结合 LRU/引用计数等信息回收：

- 正在被请求使用的节点不能淘汰；
- 公共前缀可能被多个请求引用；
- 淘汰树节点还必须释放对应的 KV pool 槽位。

因此 cache eviction 不是简单的 `dict.pop()`，而是树元数据和 GPU 内存的联合操作。

## 6. 一个可计算的收益例子

假设 system prompt 有 2,000 tokens，每个请求自身有 500 tokens，100 个请求共享 system prompt：

- 不做前缀缓存：prefill 约计算 `100 × 2,500 = 250,000` token 位置；
- 共享前缀：system prompt 只需计算一次，约计算 `2,000 + 100 × 500 = 52,000` 个新增位置（忽略边界与批处理差异）。

实际收益还受 cache 容量、命中率、attention backend 和请求到达顺序影响，但这说明了 RadixAttention 的核心价值。

## 7. 阅读源码时要同时追踪什么

只看 `radix_cache.py` 不足以理解完整行为，还应一起看：

- `srt/managers/schedule_policy.py`：请求何时进行 prefix match；
- `srt/managers/schedule_batch.py:Req`：请求如何记录 prefix/cache 状态；
- `srt/mem_cache/memory_pool.py`：逻辑 token 如何映射到物理 KV；
- `srt/mem_cache/allocator/`：显存不足时如何分配和淘汰；
- `srt/model_executor/forward_batch_info.py`：模型如何消费 cache indices。

**架构结论：** Radix tree 负责“哪些前缀可复用”，pool/allocator 负责“这些前缀的 K/V 放在哪里”，attention backend 负责“如何以正确布局读取它们”。

## 相关文档
- [项目入口](../README.md)
- [分析状态](../00-overview/analysis-state.md)
- [源码证据索引](../00-overview/evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
