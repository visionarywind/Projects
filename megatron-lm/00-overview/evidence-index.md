# 源码证据索引

- 文档目的：集中登记总览和跨文档反复使用的最小证据。
- 适用范围：目标提交 `3703d4e33a3a2b2d11ebcc8e41f45af7ce7d1eda`。
- 证据状态：已确认（行号按当前工作树源码核对）；动态行为未验证。
- 最后更新：2026-09-15

| 结论 | 证据 | 支持文档 |
|---|---|---|
| 仓库由 LM 参考程序与 Core 组成 | [README.md:15-21] | project-overview, architecture |
| 顶层结构包含 core/training/tests/examples/tools/docs | [README.md:65-87] | project-overview |
| GPT 入口导入 builder、dataset、training | [pretrain_gpt.py:33-80] | architecture, M03 |
| `get_batch` 负责 TP/CP/packing batch 边界 | [pretrain_gpt.py:111+] | M04, cross-module |
| builder 根据 spec/TE/MoE/异构配置选择实现 | [gpt_builders.py:24-110] | M01 |
| provider 可切换 ModelOpt builder | [model_provider.py:19-58] | M01, cross-module |
| 初始化检查 CUDA 并设置 distributed/model parallel | [megatron/training/initialize.py:48-176] | M02, M03 |
| process_state 存储多维 group | [megatron/core/parallel_state.py:28-165] | M02 |
| group wrapper 调用 torch.new_group | [megatron/core/parallel_state.py:232-260] | M02 |
| `initialize_model_parallel` 创建 TP/PP/DP/CP/EP/GTP groups | [megatron/core/parallel_state.py:600-1029] | M02 |
| schedule 按 PP/VP 选择实现 | [megatron/core/pipeline_parallel/schedules.py:53-168] | M02, M03 |
| 无 pipeline 调度逐 microbatch 执行并 finalize grads | [schedules.py:723-928] | M02, M03 |
| GPTModel 构造 embedding/decoder/output | [megatron/core/models/gpt/gpt_model.py:98-242] | M01 |
| GPTModel.forward 串联 preprocess、decoder、postprocess | [megatron/core/models/gpt/gpt_model.py:567-674] | M01, M06 |
| TransformerBlock.forward 逐层执行并处理 recompute/offload/norm | [megatron/core/transformer/transformer_block.py:506-780] | M01, M02 |
| training.pretrain 编排初始化、setup、data、train | [megatron/training/training.py:1530+] | M03 |
| train_step 调用 schedule、optimizer.step 和 scheduler | [megatron/training/training.py:3092+] | M03, M05 |
| D01 初始化、数据、训练和 checkpoint 完整存在 | [examples/run_simple_mcore_train_loop.py:28-283] | D01 |
| GPTDataset 产生 shifted tokens/labels/masks | [megatron/core/datasets/gpt_dataset.py:264+] | M04 |
| optimizer 有 TE/Apex/Torch fallback | [megatron/core/optimizer/__init__.py:13-35] | M05 |
| save/load checkpoint 位于 training checkpointing | [megatron/training/checkpointing.py:611+,2469+] | M05 |
| StaticInferenceEngine 优先包装 DynamicInferenceEngine | [megatron/core/inference/engines/static_engine.py:35-132] | M06 |
| DynamicInferenceEngine 管理 dynamic requests/KV context | [megatron/core/inference/engines/dynamic_engine.py:292-427] | M06 |
| text generation server 构造 inference engine 并启动服务 | [tools/run_text_generation_server.py:51-192] | M06 |
| unit test 使用 distributed runner | [skills/mcore-testing/SKILL.md:117-155] | M03, testing-recipes |
| format script 运行 black/isort/pylint/ruff/mypy | [tools/autoformat.sh:37-42] | build-and-deploy |
| docs 使用 Sphinx | [docs/documentation.md:21-60] | build-and-deploy |
| 参数/梯度 contiguous bucket buffer | `source/megatron-lm/megatron/core/distributed/param_and_grad_buffer.py:1051-1270,1630-1701` | param/grad storage、bucket view、offload/reload |
| DDP buffer reset/offload | `source/megatron-lm/megatron/core/distributed/distributed_data_parallel.py:697-775` | overlap buffer、grad buffer 生命周期 |
| 推理 KV block pool | `source/megatron-lm/megatron/core/inference/contexts/kv_block_allocator.py:17-258` | block id、prefix ref/LRU、释放与淘汰 |
| 动态上下文释放 | `source/megatron-lm/megatron/core/inference/contexts/dynamic_context.py:3717-3745,4213-4237` | request/KV/Mamba 状态清理 |
| 细粒度 activation offload | `source/megatron-lm/megatron/core/pipeline_parallel/fine_grained_activation_offload.py:60-1138` | CPU pinned storage、stream/event 和 group 生命周期 |
| full-iteration/optimizer Graph pool | `source/megatron-lm/megatron/core/full_cuda_graph.py:14-267`; `source/megatron-lm/megatron/core/optimizer/optimizer_cuda_graph.py:14-68` | shared pool/stream、static input buffers、capture/replay/reset |
| module CUDA Graph buffer ownership | `source/megatron-lm/megatron/core/transformer/cuda_graphs.py:280-347,613-808,1157-1327,1739-1807` | global graph mempool、reuse metadata、saved-for-backward、argument matching、delete |
| NCCL pluggable MemPool | `source/megatron-lm/megatron/core/nccl_allocator.py:111-364` | allocator creation、pool snapshot、group register/deregister、multi-group context |

## 引用规则

正文使用 `[相对路径:起始行-结束行]`；符号名称不替代行号。源码变化后应重新核对并更新本索引。

## 相关文档

- [分析状态](analysis-state.md)
- [D01 执行轨迹](../80-demos/D01-simple-mcore-training/execution-trace.md)

## 未解决问题

动态调用（TE kernel、torch autograd C++ engine、NCCL、推理服务网络行为）只有静态入口证据，真实目标需运行或 profiler 确认。
