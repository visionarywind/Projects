# 分析状态

## 版本锚点

- 分支：`main`
- HEAD：`f1a512c51c73ab660cf41e1af3110c7c11e3b600`
- HEAD 提交：`[Config] msgspec.Struct for the config tier (#38753)`
- 本地 tag：HEAD 未指向 tag
- 工作树：`sglang/` 为知识库目录；本轮只修改文档，没有修改生产源码

## 阶段状态

| 阶段 | 目标 | 状态 | 证据/说明 |
|---|---|---|---|
| 0 | 版本确认、仓库盘点、模块划分、总览层 | 已完成（首轮） | 版本锚点、仓库地图、M01-M18 注册表、总览文章和 README 导航已建立；后续源码变化仍需复核行号 |
| 1 | 模块实现层 | 已完成静态首轮，代表路径深审已完成 | M01-M18 实现层文章已建立；M06-M10 已补充 loader/weight mapping、group 拓扑、allocator/eviction、capture/fallback 和 sampler/grammar 代表路径；全部硬件行为仍需运行验证 |
| 2 | 真实 Demo 解剖 | 进行中 | 已建立 D01 注册和离线 Engine 深度文章，并登记 D02 协议/结构化输出、D03 speculative 静态链路；尚未运行 |
| 3 | 跨模块串联 | 进行中 | 已建立 system wiring、接口契约、运行轨迹、端到端流程，并补充调用链、共享数据、配置影响、错误边界、修改影响和性能路径；仍需与后续专题复核 |
| 4 | 开发实践层 | 进行中 | 已创建 `99-roadmap/` 导航、快速上手、阅读、调试、开发、测试、性能、风险、技术债务和后续步骤文档；仍需随模块证据更新 |
| 5 | 深度审计 | 静态审计完成，运行验证待执行 | 已完成当前文档集合的链接、fence、标题层级、源码引用覆盖和 whitespace 初检；M06-M10 的代表性 loader、allocator/eviction、prefill/decode capture 和 sampler mask 路径已补证；真实 GPU/网络/多进程行为仍需单独验证 |
| QA | 分级知识检验 | 已完成扩展版 | 已创建 100 道入门题、50 道中级题、50 道高级题和 50 道专家级题；新增题目覆盖运行时不变量、异步并发、KV ownership、数值/采样正确性、分布式拓扑、扩展边界、性能观测和系统级验证；仍需随源码专题演进复核引用 |

## 已确认的证据范围

- 顶层安装/build：`python/pyproject.toml`、`python/setup.py`、`rust/Cargo.toml`。
- 服务启动：`python/sglang/cli`、`python/sglang/launch_server.py`、`srt/entrypoints`。
- 请求、调度、执行、配置和 IPC：`sglang/02-request-flow/` 对应源码已定向阅读。
- 模型加载和并行：`engine.py`、`scheduler.py`、`tp_worker.py`、`bootstrap.py`、`parallel_state.py`、`model_runner.py`、`model_loader/loader.py`、`auto_loader.py` 已定向阅读，并新增模型加载启动主链文章。
- 测试入口：`test/run_suite.py`、`test/registered`、`test/manual` 已盘点。

- `sglang/01-modules/M01-cli-service-startup/README.md` 已创建，覆盖顶层 CLI 延迟解析、serve backend registry、自动探测、配置 resolution、部署分支和子进程清理。
- `sglang/01-modules/M02-http-api-protocol/README.md` 已创建，覆盖 FastAPI lifespan、native/OpenAI 协议、请求转换、SSE、错误 envelope、disconnect/abort 和管理接口。
- `sglang/README.md` 已升级为知识库导航，保留旧文章兼容入口并加入总览、模块、Demo、跨模块和实践层状态。
- `sglang/03-model-loading/01-模型加载与并行初始化.md` 已创建，覆盖 scheduler worker、rank/GPU、WORLD/并行组、ModelConfig、ModelRunner、loader、权重路由、PP 过滤、KV pool、attention backend、CUDA Graph 和启动 overlap。
- `sglang/01-modules/M07-分布式并行.md` 已创建，覆盖 WORLD 初始化、GroupCoordinator、TP/PP/DCP、attention/MoE 组、collective、预热、alias 清理、错误边界和未验证范围。
- `sglang/80-demos/demo-registry.md` 已创建，登记真实 checkout Demo；D01 离线 Engine 作为首篇深度解剖。
- `sglang/01-modules/M03-tokenizer-request-state/README.md` 已创建，追踪请求状态、tokenize、tokenized object、ZMQ dispatch、增量响应、abort 和 cleanup。
- `sglang/01-modules/M04-scheduler-batching/README.md` 已创建，追踪 normal/overlap loop、admission、PrefillAdder、ScheduleBatch、forward、result processing 和 decode retraction。
- `sglang/01-modules/M05-model-execution/README.md` 已创建，追踪 ForwardBatch、TP worker、ModelRunner forward/sample、PP proxy、prefill-only 和 CUDA Graph 分支。
- `sglang/01-modules/M08-kv-cache/README.md` 已创建，追踪 request-to-token pool、Radix Cache、page alignment、finished/unfinished cache 和 retraction ownership。
- `sglang/01-modules/M09-attention-cuda-graph/README.md` 已创建，追踪 attention backend 合约、registry、metadata 生命周期、eager/prefill/decode graph 选择和回退。
- `sglang/01-modules/M15-ipc-control-plane/README.md` 已创建，覆盖 PortArgs、ZMQ channels、startup pipe、请求/输出对象、msgpack/pickle、abort、异常传播和 graceful shutdown。
- `sglang/01-modules/M11-speculative-decoding/README.md`、`M12-multimodal-runtime/README.md`、`M13-disaggregation-hicache/README.md`、`M14-moe-quantization-lora/README.md`、`M15-ipc-control-plane/README.md`、`M16-kernel-device-backend/README.md`、`M17-rust-router-gateway/README.md`、`M18-testing-benchmark-ci/README.md` 已建立，覆盖各模块的入口、生命周期、调用链、边界、测试和修改影响；`M15-ipc/README.md` 保留为兼容入口；真实硬件/网络行为仍未验证。

## 未验证事项

以下不能写成“运行成功”：

- 任意 GPU 模型服务、TP/PP/EP 多卡启动；
- checkpoint 下载和实际权重加载；
- CUDA Graph capture、NCCL/RCCL/HCCL 通信；
- 真实 Demo 的端到端输出；
- 完整项目测试、完整 pre-commit、Mintlify build。

## 仍待复核的文档证据

- M06 的代表 loader、权重映射、`get_model_loader` 优先级和 `ModelConfig.from_server_args` 代表字段投影已补证；并进一步补充 ShardedState、Presharded、BitsAndBytes、GGUF、Remote/RemoteInstance、ModelOpt 和 Run:ai Streamer 的专用边界；M07 的标准 group 拓扑、collective 和 alias 清理已补证；M08 的 request row、token/page allocator、eviction policy 和 ownership 代表路径已补证；M09 的 backend registry、Hybrid/Native 代表实现、metadata 和 capture/fallback 已补证；M10 的 sampler backend、grammar cache/future/同步/失败传播已补证；其他专用变体仍需逐一覆盖。

## 下一批工作

1. 复核 checkout 更新后的源码行号，并把 evidence index 中仍剩余的 `...` 范围替换为精确范围。
2. 按需深化 M06-M10 的非代表性专用 backend/算法分支；当前代表路径已补充 loader dispatch、allocator/eviction policy、prefill graph capture 和 sampler mask 生命周期，不把代表路径误标为全覆盖。
3. 为 M12-M18 的扩展路径继续补精确函数范围和测试映射；保持 GPU、网络、多卡和端到端运行状态为未验证，直到有实际结果。
4. 在具备依赖和硬件的环境中分层执行单元、协议、Rust、GPU/多卡和 Demo 验证，并记录真实日志。
