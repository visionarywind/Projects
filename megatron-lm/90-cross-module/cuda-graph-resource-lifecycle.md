# CUDA Graph 与显存池资源生命周期

- 文档目的：解释 Megatron-LM 的局部、全迭代和优化器 CUDA Graph 如何持有 stream、静态 tensor、graph memory pool、RNG 与通信资源。
- 适用范围：源码提交 `3703d4e33a3a2b2d11ebcc8e41f45af7ce7d1eda`。
- 对应源码版本：source/megatron-lm HEAD 3703d4e33（2026-09-15 只读确认）。
- 证据状态：Python/PyTorch 调用链静态确认；CUDA allocator、NCCL、GPU replay 和峰值显存未运行验证。
- 最后更新：2026-09-15
- 前置阅读：[池化与资源管理](pooling-and-resource-management.md)
- 后续阅读：[M03 训练运行时](../01-modules/M03-training-runtime/README.md)
## 结论摘要

Megatron 存在三类 CUDA Graph：Transformer module 级 runner、full-iteration wrapper、optimizer wrapper。它们都要求 replay 地址稳定，但 memory-pool 策略不同：module runner 可使用全局 graph mempool 并复用边界 tensor；full-iteration/optimizer 可选择共享一个 process-wide `graph_pool_handle` 和捕获 stream，或者每次取得新 pool。Graph pool 只约束捕获分配，不等于 DDP contiguous bucket 或 NCCL `MemPool`。

## Full-iteration 与 optimizer graph

`get_shared_graph_pool()` 和 `get_shared_capture_stream()` 是进程级 lazy singleton；`get_graph_pool(false)` 则每次返回新的 pool handle。`StaticBufferLoader` 为 training/validation 及每个 microbatch 保留独立 CUDA tensor，并用私有 stream 完成 copy，当前 stream 在返回前等待它（[source/megatron-lm/megatron/core/full_cuda_graph.py:14-52,97-135]）。

达到 warmup 次数后，`FullCudaGraphWrapper` 注册 RNG state，执行 distributed barrier 和 device synchronize，在指定 stream/pool 中 capture；后续只 `replay()`。`reset_cuda_graph()` 删除 graph 引用、结果和 iteration 状态并执行 `gc.collect()`，但静态 buffer、共享 pool 和共享 stream 是 class/module global，不在这里释放（[source/megatron-lm/megatron/core/full_cuda_graph.py:138-267]）。optimizer wrapper 复用相同 pool/stream 选择，析构只清 graph/result，不清共享 singleton（[source/megatron-lm/megatron/core/optimizer/optimizer_cuda_graph.py:14-68]）。

## Module runner 的 buffer 所有权

module runner 使用 `CudagraphBufferMetadata` 标记 graph input/output、saved-for-backward、reuse count 和 forward/backward shared buffer。`alloc_tensor_from_graph_mempool()` 通过 PyTorch internal begin/end pool API，把新 tensor 明确分配到 `CudaGraphManager.global_mempool`（[source/megatron-lm/megatron/core/transformer/cuda_graphs.py:280-347]）。

创建 graph 前先按执行顺序记录 runner，目的是让多个 graph 安全共享 pool。输入重复出现时可预先分配 shared buffer、递减 capture reuse count，并在允许时跳过 replay copy；saved-for-backward 或逃逸到 eager 的 differentiable output 必须保持强引用，避免 allocator 在 backward capture 前复用同一地址（[source/megatron-lm/megatron/core/transformer/cuda_graphs.py:613-774,1157-1176,1275-1327]）。replay 会拒绝 shape、dtype、device 或非 tensor value 不匹配的参数（[source/megatron-lm/megatron/core/transformer/cuda_graphs.py:1739-1807]）。

`delete_cuda_graphs()` 清 runner graph/pool 引用、record、GTP stream registry，随后 `gc.collect()`、`torch.cuda.empty_cache()` 并清空 global mempool；这才是 module graph 的集中回收入口（[source/megatron-lm/megatron/core/transformer/cuda_graphs.py:777-808]）。

## NCCL MemPool 不是 graph pool

`create_nccl_mem_pool()` 用 `ncclMemAlloc/ncclMemFree` 的 pluggable allocator 创建 `torch.cuda.MemPool`；对称内存参数按 PyTorch 版本降级。注册粒度是整个 pool，不是单 tensor，因此 context manager 进入时若 snapshot 非空会先从通信组注销，退出时重新注册包含新增 allocation 的 pool（[source/megatron-lm/megatron/core/nccl_allocator.py:111-191,194-274]）。`MultiGroupMemPoolAllocator` 对多个 process group 执行同一协议（[source/megatron-lm/megatron/core/nccl_allocator.py:276-364]）。

## 资源所有权与风险

| 资源 | Owner | 释放/复位 | 风险 |
|---|---|---|---|
| Graph capture pool handle | module global / `CudaGraphManager` | module graph 用 `delete_cuda_graphs`；full graph singleton 无显式 reset | 多 pool/stream 增加 `memory_reserved` |
| static input tensor | `StaticBufferLoader.static_buffers` | 当前无集中 clear | stage/microbatch shape 变化后长期驻留或 mismatch |
| graph boundary buffer | runner metadata/strong ref | delete graphs、reuse count 或 weakref 规则 | saved-for-backward storage 被提前复用 |
| capture/replay stream | global pool或 wrapper | 进程生命周期/runner registry | stream alias、side-stream 未 join |
| NCCL MemPool | allocator/context manager | pool owner + group deregister/register | 重复注册、某 group 部分失败 |

## 深度审计

| 分析对象 | 入口落地 | 正常路径 | 分支 | 异常 | 清理 | 数据生命周期 | 执行上下文 | 行级证据 | Demo 映射 | 状态/缺口 |
|---|---|---|---|---|---|---|---|---|---|---|
| module/full/optimizer CUDA Graph | 已完成 | 已完成 | 已完成 | 部分完成 | 部分完成 | 已完成 | capture/replay/side stream 已确认 | 已完成 | D01 未启用 Graph | 部分完成：静态 buffer reset、异常 capture、GPU 验证 |
| NCCL MemPool | 已完成 | 已完成 | 已完成 | 部分完成 | 已完成 | 已完成 | process group/context manager | 已完成 | D01 未覆盖 | 部分完成：多组失败与真实注册待验证 |

## 相关文档
- [项目入口](../README.md)
- [分析状态](../00-overview/analysis-state.md)
- [源码证据索引](../00-overview/evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题

- Full graph reset 后 class-level static buffers、共享 pool/stream 何时释放，需要进程级显存快照验证。
- capture 抛异常时 `_IS_GRAPH_CAPTURING`、GC freeze、通信 side stream 和 pool 引用是否全部复位，需要 fault injection。
- Graph pool、NCCL pool、DDP buffer 和 activation offload 同时启用时的峰值与 stream ordering 尚未验证。

## 下一步阅读建议
先阅读 [分析状态](../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
