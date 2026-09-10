# M08 KV Cache 与 Radix Cache

- 文档目的：解释 scheduler 如何为请求分配 request row 和 KV slots，并如何通过 Radix Cache 复用前缀、缓存未完成请求和回收完成请求。
- 适用范围：`ReqToTokenPool`、token-to-KV allocator、`RadixCache`、prefix matching、page alignment、finished/unfinished cache。
- 对应源码版本：`f1a512c51c73ab660cf41e1af3110c7c11e3b600`
- 证据状态：部分完成
- 最后更新：2026-09-10
- 前置阅读：[M04 Scheduler 与连续批处理](../M04-scheduler-batching/README.md)、[Transformer 与 KV Cache](../../01-concepts/02-Transformer与KV-Cache.md)
- 后续阅读：[M05 模型执行](../M05-model-execution/README.md)、[系统 wiring](../../90-cross-module/system-wiring.md)

## 1. 两张表先分清

**通俗解释**：KV cache 不是一张“请求字符串到结果”的字典。SGLang 至少维护两类映射：

1. `ReqToTokenPool`：某个 scheduler 请求 row 的第 n 个 token 使用哪个 KV slot；
2. `RadixCache`：某段 token 前缀对应哪些 KV indices，以便后来的请求复用。

小例子：请求 A 的 token `[10, 20, 30]` 写入 KV slots `[501, 502, 503]`。请求 B 以 `[10, 20]` 开头时，Radix Cache 先命中 `[501, 502]`，B 只需扩展后续 token。

## 2. `ReqToTokenPool`

构造时分配 `(size + 1, max_context_len)` 的 int32 device tensor，并把 row 0 保留为 padding row；free slots 从 1 开始。[`python/sglang/srt/mem_cache/memory_pool.py:258-289`]

padding row 的意义不是多余容量：CUDA Graph padded batch 可能把默认 request index 设为 0，安全的零行可以承接 dummy read/write，而不误写真实请求。

`alloc(reqs)` 对已有 KV 的 chunked request 复用原 row，对新请求从 free slots 取 row，并把 `req.kv.req_pool_idx` 绑定到该 row。[`python/sglang/srt/mem_cache/memory_pool.py:297-313`]

释放时 `free(req)` 先断言请求持有 KV，再释放 row 并清除 `req_pool_idx`；row generation 用于区分重复利用后的生命周期。[`python/sglang/srt/mem_cache/memory_pool.py:332-347`]

## 3. 从 prefix match 到 admission

`PrefillAdder` 使用 token budget、page size 和 tree cache 做 admission；prefix match 的结果最终需要写入 request row，新的 token 则向 allocator 申请 KV slots。M04 负责什么时候调用，M08 负责映射和资源实际归属。

**已确认**：请求的 `prefix_indices` 表示已拥有/命中的 KV indices，`extend_range` 表示仍需写入的区间；因此 `ScheduleBatch.prepare_for_extend` 只把未命中的 fill ids 放进 input，并让 allocator 为扩展部分提供 `out_cache_loc`。[`python/sglang/srt/managers/schedule_batch.py:2559-2605`]

## 4. Radix Cache 插入

`RadixCache.insert` 在 disable 时直接返回；否则先将 key 转为适合 bigram/Eagle 的视图、按 page 对齐，并截断 value 使其与 key 长度一致。没有 value 时，debug/test fallback 使用 token ids 作为 value。[`python/sglang/srt/mem_cache/radix_cache.py:460-480`]

这里的 page alignment 解释了一个常见现象：token 前缀命中长度不一定等于任意字符/任意 token 边界，未对齐尾部可能不能作为 radix node 的完整缓存页。

## 5. 完成请求如何进入 cache

`cache_finished_req` 首先根据配置决定是否真的插入 finished request；disable cache 时，它只释放不是 protected prefix 的 KV segment。[`python/sglang/srt/mem_cache/radix_cache.py:482-499`]

正常插入路径：

```text
origin_input_ids + output_ids
  -> 截到 kv_len_to_handle
  -> 读取 req row 的 kv indices
  -> 构造 page-aligned RadixKey
  -> insert(key, values)
  -> 释放重复/未插入范围和未对齐 tail
  -> dec_lock_ref(last_node)
```

对应代码先组合 token ids，读取 row，构造带 `extra_key/cache_salt` 的 key，并复制 values 为 int64；插入结果的 `prefix_len` 决定哪些 slots 被 radix cache 接管。[`python/sglang/srt/mem_cache/radix_cache.py:500-538`]

这不是简单的“把请求放进字典”：allocator 释放重复部分，radix node 取得自己的引用，request row 的 cache lock 也要解除。

## 6. 未完成请求和 chunked prefill

`cache_unfinished_req` 使用 `req.get_fill_ids()` 构造 key，支持 chunked 标记；插入后重新 match prefix，把新的 indices 写回 request row，并更新 `cache_protected_len` 和 node lock。[`python/sglang/srt/mem_cache/radix_cache.py:539-599`]

`cache_protected_len` 与 `len(req.prefix_indices)` 可能不同：page size 大于 1 时，partial page 可能属于请求当前工作集，但尚未成为 tree 中完整节点。若忽略这个差异，下一轮或完成清理可能泄漏 partial KV。

## 7. 资源不足和 retraction

M04 在 decode 前调用 allocator capacity 检查；不足时 `ScheduleBatch.retract_decode` 尝试释放请求、做 host backup 并重新入队，无法恢复的请求设置 abort。[`python/sglang/srt/managers/schedule_batch.py:3076-3159`]

因此 M08 的资源状态跨越三个对象：

- request row 是否仍存在；
- token-to-KV slots 是否仍由请求、prefix cache 或 host backup 拥有；
- radix node lock/reference 是否仍保持。

只释放 GPU tensor 而不更新 row、prefix node 或 request metadata，会使下一轮读到悬挂映射。

## 8. 调试断点

| 断点 | 观察内容 |
|---|---|
| `ReqToTokenPool.__init__:266-289` | row 0、free slots、device tensor shape |
| `ReqToTokenPool.alloc:297-313` | 新 row 与 chunked row 是否正确绑定 |
| `RadixCache.insert:460-480` | page alignment、key/value 长度 |
| `cache_finished_req:482-538` | protected range、duplicate slots、lock release |
| `cache_unfinished_req:539-599` | chunked cache、rematch、protected length |
| `ScheduleBatch.retract_decode:3076-3159` | release、backup、requeue、abort |

## 9. 验证边界

**已定位但未执行**：`test/registered/unit/mem_cache/`、scheduler retraction 和 chunked prefill 测试可作为验证入口；本批没有运行 GPU、KV allocator、Radix eviction 或真实 prefix hit 测试。

## 10. 修改影响

修改 page size、prefix key、allocator capacity 或 free logic 时，必须联查 `PrefillAdder`、`ScheduleBatch.prepare_for_extend/decode`、`Req` 的 `prefix_indices/cache_protected_len`、retraction、finished/unfinished cache 和 M05 的 `out_cache_loc`。不要只改 radix tree 的插入函数。

## 相关文档

- [M04 Scheduler 与连续批处理](../M04-scheduler-batching/README.md)
- [M05 模型执行](../M05-model-execution/README.md)
- [M07 分布式并行](../M07-分布式并行.md)
- [跨模块共享数据](../../90-cross-module/shared-data-and-types.md)

## 源码证据摘要

- [`python/sglang/srt/mem_cache/memory_pool.py:258-347`](../../../python/sglang/srt/mem_cache/memory_pool.py)
- [`python/sglang/srt/mem_cache/radix_cache.py:460-599`](../../../python/sglang/srt/mem_cache/radix_cache.py)
- [`python/sglang/srt/managers/schedule_batch.py:2559-2605`](../../../python/sglang/srt/managers/schedule_batch.py)
- [`python/sglang/srt/managers/schedule_batch.py:3076-3159`](../../../python/sglang/srt/managers/schedule_batch.py)
- [`python/sglang/srt/managers/schedule_policy.py:1266-1359`](../../../python/sglang/srt/managers/schedule_policy.py)

## 深度审计

| 对象 | 入口 | 正常 | 分支 | 异常 | 清理 | 生命周期 | 执行上下文 | 行级证据 | Demo | 状态 |
|---|---|---|---|---|---|---|---|---|---|---|
| request row pool | 已完成 | 已完成 | chunk reuse 已覆盖 | 部分完成 | 已完成 | 已完成 | device/scheduler | 已完成 | D01 静态映射 | 部分：allocator 细节待补 |
| Radix insert/cache | 已完成 | 已完成 | disable/page/bigram 已覆盖 | 部分完成 | 已完成 | 已完成 | scheduler/device | 已完成 | D01 静态映射 | 部分：eviction 待补 |
| retraction ownership | 已完成 | 已完成 | restore/abort 已覆盖 | 已完成 | 部分完成 | 部分完成 | scheduler/host/device | 已完成 | D01 未实际覆盖 | 需测试验证 |

## 未解决问题

- token-to-KV allocator 各实现的具体 slot/page 算法尚未独立展开；
- eviction、priority、HiCache/storage 和跨设备 KV transfer 尚未覆盖；
- 真实 prefix hit、显存压力和 retraction 未验证。
