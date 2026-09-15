# 证据索引

- 文档目的：解释 00-overview/evidence-index.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/sglang HEAD 78be4b50af（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](analysis-state.md)。
## 结论摘要

本页聚焦 00-overview/evidence-index.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## 版本与构建

- `[python/pyproject.toml:1-24]`：Python build backend、包名、Python 版本和依赖起点。
- `[python/setup.py:1-24]`：Rust 扩展发现规则和 `SGLANG_BUILD_RUST_EXTS`。
- `[rust/Cargo.toml:1-16]`：Rust workspace、成员和 extension manifest。

## 启动与进程

- `[python/sglang/cli/main.py:11-45]`：顶层子命令和 `parse_known_args`。
- `[python/sglang/cli/serve.py:22-130]`：backend selector、位置 model path、registry dispatch 和清理。
- `[python/sglang/cli/serve_backends.py:22-211]`：ServeBackend 契约、entry point 发现、自动探测和错误语义。
- `[python/sglang/launch_server.py:16-58]`：解析后的部署模式分支。
- `[python/sglang/srt/entrypoints/engine.py:847-1051]`：scheduler/detokenizer subprocess 启动。
- `[python/sglang/srt/managers/scheduler_components/ipc_channels.py:16-89]`：rank-zero scheduler 的请求/RPC/Tokenizer/Detokenizer/metrics channels 和 skip-tokenizer 分支。
- `[python/sglang/srt/managers/detokenizer_manager.py:102-187]`：Detokenizer 初始化、socket 和 event loop。
- `[python/sglang/srt/managers/detokenizer_manager.py:1-437]`：detokenizer 子进程启动、异常通知和 socket cleanup。
- `[python/sglang/srt/managers/io_struct.py:83-128]`、`[python/sglang/srt/managers/io_struct.py:972-1091]`：BaseReq、PickleWrapper、tokenized request 和 batch request。
- `[python/sglang/srt/managers/io_struct.py:1531-1622]`、`[python/sglang/srt/managers/io_struct.py:2042-2054]`、`[python/sglang/srt/managers/io_struct.py:1-2149]`：BatchStrOutput、AbortReq 和 ShutdownReq。
- `[python/sglang/srt/managers/io_struct.py:1-2149]`：msgpack/pickle IPC 编码、解码和 socket helpers。
- `[python/sglang/srt/managers/tokenizer_manager.py:1-2816]`、`[python/sglang/srt/managers/tokenizer_manager.py:1-2816]`：graceful shutdown、ShutdownReq 和请求失败清理。

## 请求与执行

- `[python/sglang/srt/entrypoints/http_server.py:197-477]`：global state、lifespan、ORJSON request/route 和 app 初始化。
- `[python/sglang/srt/entrypoints/http_server.py:643-704]`：JSON content-type validation 和 health/generate health。
- `[python/sglang/srt/entrypoints/http_server.py:905-980]`：native generate、streaming、disconnect/abort、embedding/classify。
- `[python/sglang/srt/entrypoints/http_server.py:1729-1807]`：OpenAI completion/chat/embedding/tokenize/detokenize routes。
- `[python/sglang/srt/entrypoints/openai/serving_base.py:27-211]`：协议公共 validate/convert/stream/error 模板。
- `[python/sglang/srt/entrypoints/openai/serving_completions.py:48-220]`：completion 字段转换和 streaming。
- `[python/sglang/srt/managers/schedule_batch.py:926-2182]`：`Req`；`[python/sglang/srt/managers/schedule_batch.py:2184-2845]`：`ScheduleBatch`。
- `[python/sglang/srt/model_executor/model_runner.py:299-2242]`：ModelRunner。

## 配置与并行

- `[python/sglang/srt/distributed/bootstrap.py:70-172]`：backend、WORLD 和并行组启动。
- `[python/sglang/srt/distributed/bootstrap.py:250-308]`：rank/world 计算和 `_init_parallel_groups`。
- `[python/sglang/srt/distributed/parallel_state.py:2206-2296]`：基础 process group；`:2298-2682`：model parallel groups。
- `[python/sglang/srt/distributed/parallel_state.py:290-545]`：`GroupCoordinator` 的 device/CPU group 与 communicator。
- `[python/sglang/srt/distributed/parallel_state.py:1281-1353]`：`all_gather` collective 路径。
- `[python/sglang/srt/layers/communicator.py:247-292]`：attention TP hidden-state gather 与 `is_pre_gathered` 防重复逻辑。
- `[python/sglang/srt/layers/dp_attention.py:568-602]`：DP attention 的 all-gather/reduce-scatter 路径。
- `[python/sglang/srt/layers/logits_processor.py:1016-1023]`：TP vocab-parallel logits all-to-all 与 row 重组。
- `[python/sglang/srt/layers/moe/token_dispatcher/standard.py:145-224]`：MoE token dispatcher 的 TP all-gatherv 和 local expert mapping。
- `[python/sglang/srt/distributed/parallel_state.py:1-2417]`：自定义 group 的 all-gather、去重和统一创建。
- `[python/sglang/srt/distributed/parallel_state.py:1-2417]`：并行组和 distributed 环境清理。
- `[python/sglang/srt/distributed/utils.py:95-135]`：PP layer partition。
- `[python/sglang/srt/configs/model_config.py]`：ModelConfig 来源和架构配置（待形成专门文章）。

## 模型加载

- `[python/sglang/srt/model_executor/model_runner.py:650-691,1146-1280]`：`initialize`/`load_model` 生命周期。
- `[python/sglang/srt/model_executor/model_runner.py:1010-1100]`：attention backend/CUDA Graph 初始化。
- `[python/sglang/srt/model_executor/model_runner_components/load_model_utils.py:256-368]`：memory saver、loader 选择和 barrier。
- `[python/sglang/srt/model_loader/loader.py:274-336]`：架构实例化、post-load hook 和 `BaseModelLoader` 契约；`[python/sglang/srt/model_loader/loader.py:361-3368]`：loader 实现与 `get_model_loader` 优先级。
- `[python/sglang/srt/model_loader/auto_loader.py:62-226]`：stacked dispatch、PP filter、remap registry 和通用 walker；`[python/sglang/srt/models/utils.py:125-281]`：`AutoWeightsLoader` 递归委托和 parameter `weight_loader`。
- `[python/sglang/srt/models/llama.py:742-778]`、`[python/sglang/srt/models/qwen2.py:1-660]`：Llama/Qwen2 v2 权重入口。
- `[python/sglang/srt/configs/model_config.py:692-809]`：`from_server_args` 字段投影和 draft architecture 改写。

- `[python/sglang/srt/mem_cache/allocator.py:41-223]`、`[python/sglang/srt/mem_cache/allocator.py:27-96]`、`[python/sglang/srt/mem_cache/allocator.py:116-337]`：allocator 抽象、普通 token allocator 和 paged allocation/free。
- `[python/sglang/srt/mem_cache/evict_policy.py:9-65]`、`[python/sglang/srt/mem_cache/radix_cache.py:616-680]`：eviction strategy、heap eviction 和 lock/protected accounting。
- `[python/sglang/srt/mem_cache/memory_pool.py:129-194]`：`ReqToTokenPool` request row 分配、复用和清空。
- `[python/sglang/srt/mem_cache/allocator.py:35-170,362-430]`：普通/paged token allocator、free group、release/sort 和 page-aligned indices。
- `[python/sglang/srt/mem_cache/memory_pool.py:196-384,703-930,1517-1600]`：Mamba、KVCache、MHA/MLA/NSA 物理 buffer 和 tensor lifecycle。
- `[python/sglang/srt/mem_cache/radix_cache.py:269-318,438-630]`：RadixCache 初始化、finished/unfinished ownership、lock_ref 和 eviction。
- `[python/sglang/srt/model_executor/model_runner_kv_cache_mixin.py:199-270,614-720,751-840]`：memory pool 配置、容量投影和 allocator/backend 选择。
- `[python/sglang/srt/managers/scheduler.py:785-870,2577-2735,2869-2920,3402-3420]`：scheduler wiring、admission/retraction、KV 差值观测和 flush。
- `[python/sglang/srt/model_executor/cuda_graph_runner.py:547-721,757-760]`：capture batch、固定 `DecodeInputBuffers`、global graph pool 和 eager fallback。
- `[python/sglang/srt/mem_cache/memory_pool.py:703-742]`、`[python/sglang/srt/mem_cache/utils.py:332-358]`：KV physical buffer 与 Mooncake/custom memory pool 初始化边界。

## 多模态、分离部署与扩展

- `[python/sglang/srt/multimodal/processors/base_processor.py:70-237,1214-1515,1678-1707,1773-2007]`：多模态 processor、媒体加载/组合、CUDA IPC wrapper 和 placeholder/feature 处理。
- `[python/sglang/srt/multimodal/processors/executor.py:15-54]`、`[python/sglang/srt/multimodal/encoder_preprocessing.py:16-150]`：异步预处理 executor 与按 rank materialize。
- `[python/sglang/srt/disaggregation/base/conn.py:17-194]`、`common/conn.py:95-1980`：KV sender/receiver、bootstrap、poll、timeout、abort 和 cleanup 契约。
- `[python/sglang/srt/disaggregation/common/staging_buffer.py:118-768]`、`staging_handler.py:40-931`、`decode_hicache_mixin.py:26-315`：staging allocator、gather/scatter、HiCache restore 状态机。
- `[python/sglang/srt/layers/quantization/base_config.py:24-264]`、`lora/lora_manager.py:65-1078`、`eplb/expert_location.py:63-343`、`eplb/eplb_manager.py:29-352`：量化、LoRA 生命周期和 EPLB expert location。
- `[rust/Cargo.toml:1-50]`、`[rust/sglang-server/src/api_server.rs:1-15]`、`[rust/sglang-server/src/tokenizer_manager/wiring.rs:1-77]`：Rust workspace、server API 模块和 flume 请求/abort/shutdown wiring。
- `[experimental/sgl-router/src/server/app.rs:14-82]`、`[experimental/sgl-router/src/proxy/mod.rs:19-257]`、`[experimental/sgl-router/src/policies/mod.rs:32-90]`、`[experimental/sgl-router/src/workers/registry.rs:11-180]`：router ingress/metrics、breaker-gated proxy、token/prefix routing 数据和 worker registry。
- `[sgl-model-gateway/src/core/worker_registry.rs:173-269]`、`[sgl-model-gateway/src/core/retry.rs:9-100]`、`[sgl-model-gateway/src/service_discovery.rs:33-229]`、`[sgl-model-gateway/src/core/worker.rs:650-1185]`：gateway worker 索引、重试、服务发现和 worker 生命周期。

## Kernel、平台与测试

- `[python/sglang/srt/platforms/__init__.py:33-125]`、`[python/sglang/srt/platforms/interface.py:26-133]`、`[python/sglang/srt/platforms/device_mixin.py:96-244]`：平台发现、SRTPlatform 工厂/能力钩子和 DeviceMixin 活跃/规划接口。
- `[python/sglang/kernels/spec.py:29-267]`、`[python/sglang/kernels/registry.py:17-79]`、`[python/sglang/kernels/selector.py:33-104]`、`[python/sglang/kernels/fused_op.py:334-665]`：KernelSpec 能力元数据、注册表、懒加载选择、BaseFusedOp 后端/平台 dispatch、torch.compile 切换和 native fallback。
- `[test/run_suite.py:229-364]`、`[python/sglang/test/ci/ci_register.py:32-59,340-407]`：suite 校验、过滤、历史估时、LPT 分片、AST registration 收集和 suite CLI。
- `[test/registered/core/test_srt_endpoint.py]`：server fixture、协议/采样/grammar/cache/Rust endpoint 测试入口。
- `[benchmark/hicache/]`、`[benchmark/lora/]`、`[benchmark/kernels/]`、`[benchmark/bench_adaptive_speculative.py]`：专项性能入口；文件存在不等于已运行。

## 相关文档
- [项目入口](../README.md)
- [分析状态](analysis-state.md)
- [源码证据索引](evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
