# 功能开发配方

## 新增请求字段

1. 在输入结构和 normalize/batch slicing 中定义语义；
2. 检查 tokenized IPC 的序列化；
3. 在 scheduler handler 构造 `Req` 并保留生命周期；
4. 检查 `ScheduleBatch`、`ForwardBatch` 和 worker 是否需要消费；
5. 更新 output/result processor、abort 和 cleanup；
6. 添加正常、失败、batch 对齐测试。

## 修改 admission

联查 `get_next_batch_to_run`、`PrefillAdder`、prefix match、KV allocator、chunked prefill、priority/preemption、metrics 和 retraction。至少验证“能加入”“预算不足等待”“无法恢复 abort”三类。

## 修改 batch 字段

确认是否被 overlap snapshot、ForwardBatch alias、attention metadata 或 CUDA Graph buffer 持有。优先使用显式 snapshot/restore，不要在共享生命周期未确认时原地改字段。

## 修改 sampling

同时检查 SamplingParams verify、SamplingBatchInfo merge/filter、grammar mask、logprob、delay sampling、finish reason 和 M03 输出清理。

## 修改配置

先确定值在哪个 resolution 阶段固定，再追踪 publish 到哪个进程、哪个 worker 和哪个 device tensor。不要把派生配置放到热路径反复判断。
