# 证据索引

## 版本与构建

- `[python/pyproject.toml:1-24]`：Python build backend、包名、Python 版本和依赖起点。
- `[python/setup.py:1-24]`：Rust 扩展发现规则和 `SGLANG_BUILD_RUST_EXTS`。
- `[rust/Cargo.toml:1-16]`：Rust workspace、成员和 extension manifest。

## 启动与进程

- `[python/sglang/cli/main.py]`：顶层 CLI（当前行号需随文章生成时复核）。
- `[python/sglang/cli/serve.py]`：serve backend registry 和参数规范化。
- `[python/sglang/launch_server.py:16-58]`：部署模式分支。
- `[python/sglang/srt/entrypoints/engine.py:847-1051]`：scheduler/detokenizer subprocess 启动。
- `[python/sglang/srt/managers/scheduler.py:5676-5840]`：scheduler process 配置、publish、构造和异常清理。

## 请求与执行

- `[python/sglang/srt/entrypoints/http_server.py:1-80]`：HTTP server 依赖和协议入口。
- `[python/sglang/srt/managers/tokenizer_manager.py]`：请求状态、tokenize 和 IPC。
- `[python/sglang/srt/managers/scheduler.py:425-...]`：Scheduler 生命周期和调度循环。
- `[python/sglang/srt/managers/schedule_batch.py:926-...]`：`Req`；`:2185-...`：`ScheduleBatch`。
- `[python/sglang/srt/model_executor/model_runner.py:299-...]`：ModelRunner。

## 配置与并行

- `[python/sglang/srt/distributed/bootstrap.py:70-172]`：backend、WORLD 和并行组启动。
- `[python/sglang/srt/distributed/bootstrap.py:250-308]`：rank/world 计算和 `_init_parallel_groups`。
- `[python/sglang/srt/distributed/parallel_state.py:2206-2296]`：基础 process group；`:2298-2682`：model parallel groups。
- `[python/sglang/srt/distributed/parallel_state.py:290-545]`：`GroupCoordinator` 的 device/CPU group 与 communicator。
- `[python/sglang/srt/distributed/parallel_state.py:1281-1353]`：`all_gather` collective 路径。
- `[python/sglang/srt/distributed/parallel_state.py:2684-2730]`：自定义 group 的 all-gather、去重和统一创建。
- `[python/sglang/srt/distributed/parallel_state.py:2929-3018]`：并行组和 distributed 环境清理。
- `[python/sglang/srt/distributed/utils.py:95-135]`：PP layer partition。
- `[python/sglang/srt/configs/model_config.py]`：ModelConfig 来源和架构配置（待形成专门文章）。

## 模型加载

- `[python/sglang/srt/model_executor/model_runner.py:650-...]`：initialize/load_model 生命周期。
- `[python/sglang/srt/model_executor/model_runner.py:1010-1100]`：attention backend/CUDA Graph 初始化。
- `[python/sglang/srt/model_executor/model_runner_components/load_model_utils.py:256-368]`：memory saver、loader 选择和 barrier。
- `[python/sglang/srt/model_loader/loader.py:274-...]`：架构实例化；`:4282-...`：loader dispatch。
- `[python/sglang/srt/model_loader/auto_loader.py:156-...]`：PP filter、stacked dispatch、AutoWeightsLoader。
- `[python/sglang/srt/models/llama.py:...]`、`qwen2.py`：具体模型权重路由。

## 测试

- `[test/run_suite.py:1-16]`：hardware mapping 和 suite runner 入口。
- `test/registered/model_loading/`：模型加载专项测试目录。
- `test/registered/unit/model_loader/`：loader 单元测试目录。

带 `...` 的范围表示已定位到函数/文件但尚未在索引中锁定最终行号；后续文章完成前必须复核，不能把省略号当作精确引用。
