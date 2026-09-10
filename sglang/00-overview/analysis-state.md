# 分析状态

## 版本锚点

- 分支：`main`
- HEAD：`f1a512c51c73ab660cf41e1af3110c7c11e3b600`
- HEAD 提交：`[Config] msgspec.Struct for the config tier (#38753)`
- 本地 tag：HEAD 未指向 tag
- 工作树：`source-doc/` 为未跟踪目录；未修改生产源码

## 阶段状态

| 阶段 | 目标 | 状态 | 证据/说明 |
|---|---|---|---|
| 0 | 版本确认、仓库盘点、模块划分、总览层 | 已完成（首轮） | 版本锚点、仓库地图、M01-M18 注册表、总览文章和 README 导航已建立；后续源码变化仍需复核行号 |
| 1 | 模块实现层 | 进行中 | M03、M04、M05、M06、M07、M08、M09、M10 已有独立实现层文章；M01/M02/M11-M18 仍待补齐 |
| 2 | 真实 Demo 解剖 | 进行中 | 已建立 D01 注册和离线 Engine 深度文章；尚未运行 |
| 3 | 跨模块串联 | 进行中 | 已建立 system wiring、接口契约、运行轨迹、端到端流程，并补充调用链、共享数据、配置影响、错误边界、修改影响和性能路径；仍需与后续专题复核 |
| 4 | 开发实践层 | 进行中 | 已创建 `99-roadmap/` 导航、快速上手、阅读、调试、开发、测试、性能、风险、技术债务和后续步骤文档；仍需随模块证据更新 |
| 5 | 深度审计 | 进行中 | 已完成当前文档集合的链接、fence 和 whitespace 初检；实现深度、Demo 深度及源码行号仍需持续审计 |
| QA | 分级知识检验 | 已完成扩展版 | 已创建 100 道入门题、50 道中级题、50 道高级题和 50 道专家级题；新增题目覆盖运行时不变量、异步并发、KV ownership、数值/采样正确性、分布式拓扑、扩展边界、性能观测和系统级验证；仍需随源码专题演进复核引用 |

## 已确认的证据范围

- 顶层安装/build：`python/pyproject.toml`、`python/setup.py`、`rust/Cargo.toml`。
- 服务启动：`python/sglang/cli`、`python/sglang/launch_server.py`、`srt/entrypoints`。
- 请求、调度、执行、配置和 IPC：`source-doc/02-request-flow/` 对应源码已定向阅读。
- 模型加载和并行：`engine.py`、`scheduler.py`、`tp_worker.py`、`bootstrap.py`、`parallel_state.py`、`model_runner.py`、`model_loader/loader.py`、`auto_loader.py` 已定向阅读，并新增模型加载启动主链文章。
- 测试入口：`test/run_suite.py`、`test/registered`、`test/manual` 已盘点。

## 本批新增成果

- `source-doc/README.md` 已升级为知识库导航，保留旧文章兼容入口并加入总览、模块、Demo、跨模块和实践层状态。
- `source-doc/03-model-loading/01-模型加载与并行初始化.md` 已创建，覆盖 scheduler worker、rank/GPU、WORLD/并行组、ModelConfig、ModelRunner、loader、权重路由、PP 过滤、KV pool、attention backend、CUDA Graph 和启动 overlap。
- `source-doc/01-modules/M07-分布式并行.md` 已创建，覆盖 WORLD 初始化、GroupCoordinator、TP/PP/DCP、attention/MoE 组、collective、预热、alias 清理、错误边界和未验证范围。
- `source-doc/80-demos/demo-registry.md` 已创建，登记真实 checkout Demo；D01 离线 Engine 作为首篇深度解剖。
- `source-doc/01-modules/M03-tokenizer-request-state/README.md` 已创建，追踪请求状态、tokenize、tokenized object、ZMQ dispatch、增量响应、abort 和 cleanup。
- `source-doc/01-modules/M04-scheduler-batching/README.md` 已创建，追踪 normal/overlap loop、admission、PrefillAdder、ScheduleBatch、forward、result processing 和 decode retraction。
- `source-doc/01-modules/M05-model-execution/README.md` 已创建，追踪 ForwardBatch、TP worker、ModelRunner forward/sample、PP proxy、prefill-only 和 CUDA Graph 分支。
- `source-doc/01-modules/M08-kv-cache/README.md` 已创建，追踪 request-to-token pool、Radix Cache、page alignment、finished/unfinished cache 和 retraction ownership。
- `source-doc/01-modules/M09-attention-cuda-graph/README.md` 已创建，追踪 attention backend 合约、registry、metadata 生命周期、eager/prefill/decode graph 选择和回退。
- `source-doc/01-modules/M10-sampling-constraints/README.md` 已创建，追踪 SamplingParams、SamplingBatchInfo、penalty、grammar mask、sampler 和 overlap cleanup。

## 未验证事项

以下不能写成“运行成功”：

- 任意 GPU 模型服务、TP/PP/EP 多卡启动；
- checkpoint 下载和实际权重加载；
- CUDA Graph capture、NCCL/RCCL/HCCL 通信；
- 真实 Demo 的端到端输出；
- 完整项目测试、完整 pre-commit、Mintlify build。

## 仍待复核的文档证据

- `evidence-index.md` 中带 `...` 的范围需要在对应文章完成前换成精确行号。
- Llama/Qwen2 的 `_load_weights_v2` 当前只做了定位，需用精确函数范围补证。
- `initialize_model_parallel` 的 group 创建顺序及 `init_model_parallel_group` 内部同步细节仍需进一步展开。
- `ModelConfig.from_server_args` 的完整 HF config、架构和多模态配置来源尚未形成独立文章。

## 下一批工作

1. 复核新 README、M03、M04、M05、M07、M08、M09、M10 和 D01 文章的本地链接、fence、标题层级和源码符号。
2. 补 M09 Attention/CUDA Graph 和 M10 采样/约束输出的独立实现文章，并让 M07 追踪 group 消费调用点。
3. 继续补 M06 的具体参数 `weight_loader`、attention backend/KV layout、CUDA Graph capture 条件和专用 loader 分支。
4. 完善 `90-cross-module/`、复核 `99-roadmap/` 实践文档，创建第二个真实 Demo 或补充 D01 实际运行记录，最后进行实现深度、Demo 深度、链接、fence、标题和 `git diff --check` 审计。
