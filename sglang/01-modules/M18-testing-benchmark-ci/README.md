# M18 测试、Benchmark 与 CI

- 文档目的：解释 01-modules/M18-testing-benchmark-ci/README.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/sglang HEAD 78be4b50af（2026-09-15 只读确认）。
- 证据状态：suite 发现、过滤、分片和主要测试入口已静态确认；本次未运行完整 suite、GPU 服务、模型下载或 CI job。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M18-testing-benchmark-ci/README.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


- 源码证据：`/home/mtuser/workspace/repos/Github/sglang` 当前 checkout。
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

`test/run_suite.py` 的 `run_a_suite` 先递归发现 `test/registered/**/*.py`，排除 `conftest.py`/`__init__.py`，再用 AST registration 收集测试并执行严格 sanity check；`validate_all_suites` 检查 backend 与 suite 归属，`filter_tests` 按 backend、suite、nightly 和 disabled 状态分组。启用自动分片时，`load_live_est` 读取历史模型覆盖 `est_time`，`auto_partition` 使用按耗时降序的贪心 LPT，并以文件名打破平局保证确定性。[`test/run_suite.py:229-282`][`test/run_suite.py:324-364`][`python/sglang/test/ci/ci_register.py:1-287`]

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

## 文档元数据（规范补充）

- 文档目的：说明 `01-modules/M18-testing-benchmark-ci/README.md` 的源码分析范围、结论和维护入口。
- 适用范围：当前项目对应模块/入口的静态源码与测试分析。
- 对应源码版本：以本项目 `00-overview/analysis-state.md` 或同页版本字段为准。
- 证据状态：静态源码证据；未执行的构建、测试、GPU、网络或多进程行为保持“未验证”。
- 最后更新：2026-09-15
- 前置阅读：本项目根 README 与 `00-overview/analysis-state.md`。
- 后续阅读：本模块/示例的实现、测试和风险页面。

## 深度审计

| 分析对象 | 入口落地 | 正常路径 | 分支 | 异常 | 清理 | 数据生命周期 | 执行上下文 | 行级证据 | Demo 映射 | 状态/缺口 |
|---|---|---|---|---|---|---|---|---|---|---|
| `01-modules/M18-testing-benchmark-ci/README.md` | 已完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 已映射或不适用 | 部分完成：动态行为、边界或专用变体仍需验证 |

## 相关文档
- [项目入口](../../README.md)
- [分析状态](../../00-overview/analysis-state.md)
- [源码证据索引](../../00-overview/evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](../../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
