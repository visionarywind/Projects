# 模块注册表

本表给每个重要职责分配稳定 ID。ID 描述职责边界，不等于 Python 目录；一个模块可以跨多个目录，一个目录也可能包含多个模块。状态只表示本知识库文档完成度，不表示生产代码成熟度。

| ID | 模块 | 主要源码范围 | 核心入口/接口 | 状态 |
|---|---|---|---|---|
| M01 | CLI 与服务启动 | `python/sglang/cli`、`launch_server.py` | `main`、`serve`、`run_server` | 已完成首轮：已有实现层文章 |
| M02 | HTTP/API 与协议 | `python/sglang/srt/entrypoints` | `launch_server`、各 route、协议模型 | 已完成首轮：已有实现层文章 |
| M03 | Tokenizer 与请求状态 | `managers/tokenizer_manager.py`、tokenizer | `TokenizerManager`、`ReqState` | 已有实现层文章 |
| M04 | Scheduler 与连续批处理 | `managers/scheduler.py`、`schedule_batch.py`、`scheduler_components` | `Scheduler`、`ScheduleBatch`、`Req` | 已有实现层文章 |
| M05 | 模型执行 | `model_executor`、`managers/tp_worker.py` | `ModelRunner`、`ForwardBatch`、`TpModelWorker` | 已有实现层文章 |
| M06 | 模型加载与权重 | `model_loader`、`models/*` | `get_model_loader`、`load_weights` | 已有实现层文章；代表 loader、权重映射和 barrier 已补证，专用优先级仍需逐一复核 |
| M07 | 分布式并行 | `distributed`、`layers/dp_attention.py` | `init_torch_distributed`、`initialize_model_parallel` | 已有实现层文章；标准 group 拓扑、collective 和清理已补证，专用消费点仍需复核 |
| M08 | KV Cache 与 Radix Cache | `mem_cache` | `ReqToTokenPool`、`RadixCache`、prefix match | 已有实现层文章；request row、eviction 和 ownership 代表路径已补证，allocator 变体仍需复核 |
| M09 | Attention 与 CUDA Graph | `model_executor`、`layers/attention`、`compilation` | backend builders、graph capture | 已有实现层文章；backend 选择、metadata、capture/fallback 代表路径已补证，专用 kernel 仍需复核 |
| M10 | 采样与约束输出 | `sampling`、`constrained`、`parser` | sampler、grammar backend、parser | 已有实现层文章；batch、grammar、sampler 分支和 logprob 代表路径已补证，全部 kernel/cache 仍需复核 |
| M11 | Speculative decoding | `speculative`、相关 scheduler/attention | draft worker、accept/reject | 已有实现层文章 |
| M12 | 多模态 runtime | `srt/multimodal`、`multimodal_gen` | `MultimodalInputs`、generation runtime | 已有实现层文章 |
| M13 | 分离部署与 HiCache | `disaggregation`、`mem_cache/storage` | prefill/decode、transfer、HiCache | 已有实现层文章 |
| M14 | MoE、量化与 LoRA | `layers/quantization`、`models`、`lora`、`eplb` | quant method、expert groups、LoRA manager | 已有实现层文章 |
| M15 | 多进程与 IPC | `entrypoints/engine.py`、manager IPC 组件 | ZMQ channels、startup pipe | 已有实现层文章 |
| M16 | Kernel 与设备后端 | `kernels`、`platforms`、`hardware_backend` | device dispatch、JIT/AOT ops | 已有实现层文章；设备行为未验证 |
| M17 | Rust 扩展、Router、Gateway | `rust`、`experimental/sgl-router`、`sgl-model-gateway` | PyO3 modules、独立服务 | 已有实现层文章；跨语言/网络行为未验证 |
| M18 | 测试、Benchmark 与 CI | `test`、`benchmark`、`.github/workflows` | registration、suite runner | 已有实现层文章；实际 suite 未运行 |

## 模块文章的最低完成标准

每个模块最终至少回答：

1. 入口从哪里来，出口到哪里去；
2. 初始化、正常运行、异常和清理流程是什么；
3. 真实对象、字段和接口契约是什么；
4. 至少一条从入口到状态改变/资源访问/外部效果的源码调用链；
5. 一个小例子和一个调试断点；
6. 对应测试在哪里，哪些测试尚未执行；
7. 修改它会影响哪些模块和跨进程/设备边界。

“待补”表示还没有达到这个标准，不表示源码没有该功能。
