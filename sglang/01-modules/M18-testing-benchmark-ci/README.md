# M18 测试、Benchmark 与 CI

- 源码证据：`/home/mtuser/workspace/repos/Github/sglang` 当前 checkout。
- 证据状态：suite 发现、过滤、分片和主要测试入口已静态确认；本次未运行完整 suite、GPU 服务、模型下载或 CI job。
- 关联：[M01 启动](../M01-cli-service-startup/README.md)、[M04 Scheduler](../M04-scheduler-batching/README.md)、[M16 Kernel/后端](../M16-kernel-device-backend/README.md)。

## 1. 测试系统边界

SGLang 的测试不是一个单一 pytest 命令：registered tests、manual tests、benchmark 和 CI suite 有不同的发现、硬件、模型和进程要求。`test/run_suite.py` 是 suite 调度入口，负责后端有效性、测试过滤、历史耗时估计、suite 执行和结果汇总；CI registration 决定哪些测试进入 PR/nightly/weekly 等任务。

```text
suite config / CI registration
  -> _valid_suites_by_backend
  -> validate_all_suites / filter_tests
  -> partition or run_a_suite
  -> server fixture / test process
  -> result, timing, failure summary
```

## 2. 发现与调度契约

`test/run_suite.py` 的 `run_a_suite` 先递归发现 `test/registered/**/*.py`，排除 `conftest.py`/`__init__.py`，再用 AST registration 收集测试并执行严格 sanity check；`validate_all_suites` 检查 backend 与 suite 归属，`filter_tests` 按 backend、suite、nightly 和 disabled 状态分组。启用自动分片时，`load_live_est` 读取历史模型覆盖 `est_time`，`auto_partition` 使用按耗时降序的贪心 LPT，并以文件名打破平局保证确定性。[`test/run_suite.py:229-282`][`test/run_suite.py:324-419`][`python/sglang/test/ci/ci_register.py:340-407`]

registered 测试通常适合稳定、可纳入 CI 的行为；manual 测试往往需要特定模型、GPU、网络或人工环境；benchmark 衡量吞吐、延迟、显存或 kernel 性能，不能直接作为 correctness 测试。

## 3. 代表测试链

`test/registered/core/test_srt_endpoint.py` 的 `TestSRTEndpoint.setUpClass` 启动服务 fixture，测试通过 HTTP/协议入口覆盖 logprob、chunked prefill、grammar、custom logit processor、cache token 和 server info，`tearDownClass` 负责清理；其中 `TestTokenizeDetokenize` 与 `TestRustServerEndpoint` 覆盖不同入口。文件中出现 `SERVER_ENV = {"SGLANG_USE_PICKLE_IPC": "0"}` 体现测试可显式固定 IPC 变体，但不表示本文会话已经运行。

```text
registered test
  -> server fixture / model process
  -> HTTP or Rust endpoint
  -> tokenizer/scheduler/model result
  -> assertion
  -> teardown and process cleanup
```

失败排查应先确认 suite 是否被选中、backend/hardware 是否满足、模型和 server 是否 ready，再区分协议错误、数值错误、超时、子进程退出和资源泄漏。benchmark 还应记录 warmup、并发、输入输出长度、设备和版本，否则不同结果不可直接比较。

## 4. 变体矩阵

| 维度 | 典型覆盖 | 主要风险 |
|---|---|---|
| backend/device | CUDA、CPU、其他设备后端 | kernel、dtype、extension 能力不同 |
| API | native、OpenAI、Rust server、gRPC | schema、SSE、错误和取消不一致 |
| execution | eager、CUDA Graph、speculative、chunked prefill | shape、KV、回滚与状态生命周期 |
| model/input | text、multimodal、MoE、quant、LoRA | placeholder、权重布局、adapter 组合 |
| deployment | 单进程、多进程、分离部署、gateway | IPC、发现、超时、清理 |

## 5. 小例子、命令和断点

例：修改 sampling penalty 后，先定位 registered sampling/API 测试，再用最小 suite 验证普通 eager 路径；随后才比较 `benchmark/` 中对应吞吐。若改动 IPC payload，优先看 tokenizer manager、endpoint fixture 和 `SGLANG_USE_PICKLE_IPC` 变体，不应直接把完整 nightly 当作第一步。

断点：`test/run_suite.py: _valid_suites_by_backend`、`validate_all_suites`、`filter_tests`、`load_live_est`、`run_a_suite`、`main`；以及测试的 `setUpClass`/`tearDownClass`、请求 helper、断言和 server cleanup。

可参考 upstream checkout 中项目自身的 registered/manual/benchmark 命令；本 docs-only workspace 没有统一 build/test target。GPU、多卡、模型下载、完整 CI 和外部服务命令均应在满足环境后单独执行并记录结果。

## 6. 修改影响与证据等级

新增功能至少需要：单元/逻辑测试、协议或 endpoint 测试（若改变边界）、对应设备/变体测试（若改变 kernel 或 graph）以及必要的 benchmark。测试文件存在只证明代码入口存在；suite 被注册只证明调度配置存在；只有实际日志和结果才能证明某次运行成功。修改 suite filter、环境变量或 partition 逻辑会影响 CI 覆盖率，可能造成“绿色但未执行目标测试”，应同步更新注册、估时和报告。
