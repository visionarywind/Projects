# M13 分离部署与 HiCache

- 源码证据：`/home/mtuser/workspace/repos/Github/sglang` 当前 checkout。
- 证据状态：连接、poll、staging、HiCache 状态机已静态确认；真实跨机传输、RDMA/NIXL/Mooncake/Mori 未验证。
- 关联：[M08 KV Cache](../M08-kv-cache/README.md)、[M15 IPC](../M15-ipc-control-plane/README.md)。

## 1. 两条相关但不同的路径

PD disaggregation 将 prefill 与 decode 放到不同 runtime/实例，通过 KV transfer 连接；HiCache 则在本地 GPU KV 之外增加 host/storage 层并执行 prefetch/restore/eviction。两者可以组合，但不能写成同一个 cache。

```text
prefill scheduler
  -> BaseKVSender.init/send
  -> bootstrap metadata / KV chunks
  -> decode BaseKVReceiver.init/send_metadata/poll
  -> local KV commit

decode request
  -> radix/L1 match
  -> HiCache prefetch or storage load
  -> staging buffer
  -> scatter into KV pool
  -> decode forward
```

## 2. KV transfer 契约

`python/sglang/srt/disaggregation/base/conn.py:17-258` 定义 `StateType`、`KVTransferMetric`、`KVArgs`、`KVPoll` 以及 `BaseKVManager`、`BaseKVSender`、`BaseKVReceiver`、`BaseKVBootstrapServer`。sender/receiver 的公共方法是 `init`、`poll`、`clear`、`abort`；receiver 还通过 `send_metadata` 建立目标布局。

`common/conn.py:95-1977` 的 `PrefillServerInfo`、`PrefillRankInfo`、`CommonKVManager`、`CommonKVSender`、`CommonKVReceiver` 和 bootstrap server 负责拓扑注册、rank 映射、心跳、超时、abort acknowledgement 和 endpoint cleanup。`FakeKVSender/Receiver`（`fake/conn.py:22-141`）可作为逻辑测试替身；NIXL/Mooncake/Mori/Ascend 是 backend 实现，不是公共契约。

## 3. staging 与 HiCache

`common/staging_buffer.py:118-277` 的 `StagingBuffer` 和 `StagingAllocator` 管理指针、offset、alloc id、水位线与释放；`:463-724` 提供 gather/scatter 和 layout 计算。`staging_handler.py:40-657` 的 `DecodeStagingContext`、`PrefillStagingContext`、`DecodeStagingHandler`、`PrefillStagingStrategy` 管理 room、chunk、watermark 和 transfer。

`decode_hicache_mixin.py:26-315` 将 prefix match 分成 L1/local decode prefix 与需要 restore 的长度；prefetch 完成后才把结果提交给 request。restore 失败、超时或请求 abort 时必须调用 `_clean_hicache_prefetch_resources`，不能让 staging room 永久占用。

## 4. 代表调用链和状态

```text
ScheduleBatch disaggregation mixin
  -> build decode/prefill transfer request
  -> CommonKVSender.send / CommonKVReceiver.poll
  -> transfer/staging completion
  -> DecodeHiCacheTransferMixin._process_hicache_local_restores
  -> _commit_hicache_local_restore_to_req
  -> M08 request-to-token/KV state
```

正常状态是 `registered → metadata/transfer → poll DONE → commit`；异常状态是 `poll FAILED/TIMEOUT → record_failure → abort/clear`。KV event publisher、storage backend 和 ZMQ control channel 只传递元数据/控制信号；实际 payload 可能走专用 transfer engine。

## 5. 小例子、测试和边界

例：decode 只在 GPU cache 命中 128 tokens，而 host storage 有后续 256 tokens；系统预分配 restore 空间，staging gather/load 后 scatter 回 KV pool，完成后 request 才能继续 decode。

断点：`CommonKVSender.send`、`CommonKVReceiver.poll`、`StagingAllocator.assign/free`、`DecodeHiCachePreallocMixin._start_hicache_prefetch` 和 `_commit_hicache_local_restore_to_req`。

测试/脚本：`test/manual/test_kv_events.py`、`test/manual/test_mori_transfer_engine_e2e.py`、`test/manual/test_weight_cache_e2e.py`、`benchmark/hicache/`、`mem_cache/storage/*/test*`。本次未运行真实 network/storage/RDMA/多节点测试。

## 6. 修改影响

修改 chunk layout 影响 M08 KV layout、M07 DCP/PP、M15 IPC 和 backend transfer；修改 abort acknowledgement 影响 scheduler request lifetime；修改 HiCache prefix accounting 影响 admission 和 decode correctness。**已确认**公共状态机；**未验证**硬件传输性能和故障恢复的端到端行为。
