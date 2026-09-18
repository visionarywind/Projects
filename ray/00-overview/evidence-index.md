# 源码证据索引

- 版本：`source/ray` HEAD `cfe4725d23`（2026-09-17）。证据状态：静态代表路径索引；动态未验证。

| 主题 | 证据 |
|---|---|
| 项目定位与 Core 抽象 | `README.rst:17-47` |
| 公共导出 | `python/ray/__init__.py:80-130` |
| 初始化与地址解析 | `python/ray/_private/worker.py:1439-1505` |
| get/put | `python/ray/_private/worker.py:2881-3081` |
| RemoteFunction | `python/ray/remote_function.py:90-182,355-574` |
| Actor 代表入口 | `python/ray/actor.py:944,1079,1232,1717,1883` |
| Cython task binding | `python/ray/_raylet.pyx:3938-4032` |
| CoreWorker 进程初始化 | `src/ray/core_worker/core_worker_process.cc:231-285` |
| CoreWorker 生命周期 | `src/ray/core_worker/core_worker.cc:312-363,585-628` |
| C++ normal task 提交 | `src/ray/core_worker/core_worker.cc:2056-2135` |
| 依赖、lease 与 push | `src/ray/core_worker/task_submission/normal_task_submitter.cc:33-504` |
| Object Manager | `src/ray/object_manager/object_manager.cc:66-718` |
| Raylet NodeManager | `src/ray/raylet/node_manager.cc:199-373,575-609,1415+` |
| GCS server lifecycle/services | `src/ray/gcs/gcs_server.cc:81-106,292-497,916+` |
| Autoscaler v2 scheduler | `python/ray/autoscaler/v2/scheduler.py:905-2204` |
| Runtime env agent | `python/ray/_private/runtime_env/agent/runtime_env_agent.py:316-773` |
| Job manager | `python/ray/dashboard/modules/job/job_manager.py:57-703` |
| Dashboard head/state/reporter | `python/ray/dashboard/head.py:49-551`; `state_aggregator.py:61-697`; `reporter_agent.py:475-2212` |
| Ray Data API/executor | `python/ray/data/read_api.py:678-828`; `dataset.py:270-8709`; `streaming_executor.py:107-740` |
| Ray Train | `python/ray/train/base_trainer.py:128-936`; `data_parallel_trainer.py:29-603`; `TrainController:114-977` |
| Ray Tune | `python/ray/tune/tuner.py:43-425`; `tuner_internal.py:63-698`; `TuneController:72-2034` |
| RLlib | `rllib/algorithms/algorithm.py:211-4863`; `algorithm_config.py:109-6238`; `learner.py:112-1772` |
| Ray Serve | `python/ray/serve/api.py:82-166,902-1010`; `controller.py:134-1988`; `deployment_state.py:2952-5924` |
| DAG/CompiledDAG/channels | `python/ray/dag/dag_node.py:33-724`; `compiled_dag_node.py:813-3285`; `experimental/channel/common.py:61-688` |
| Demo | `release/hello_world_tests/hello_world.py:1-13` |
| 构建/贡献边界 | `BUILD.bazel`; `WORKSPACE`; `python/setup.py`; `src/ray/protobuf/BUILD.bazel`; `AGENTS.md:56-92` |
| Core internals 官方目录 | `doc/source/ray-core/internals.rst:5-23` |

## 使用规则

索引登记已定位的代表入口和实现范围；“+”表示只给出起始锚点，应在修改源码时重新收敛。旧文档和目标源码冲突时，以当前 checkout 为准。文件名未带完整目录的条目，应结合主题所属模块的 source-map 阅读。

## 相关文档
[分析状态](analysis-state.md) · [项目总览](project-overview.md) · [模块注册表](../01-modules/module-registry.md)

## 源码证据摘要
见上表；各模块还有独立 `source-map.md`、`line-level-analysis.md` 和 `testing.md`。

## 未解决问题
Actor/Java/C++ 公共 API 全链、特定 protobuf RPC、跨节点动态错误恢复和性能路径仍需专题或运行验证。

## 下一步阅读建议
按公共入口 → Cython → CoreWorker → Raylet/Object/GCS 顺序阅读 Core；AI library 则先读公共 API，再读 controller/executor/state machine。
