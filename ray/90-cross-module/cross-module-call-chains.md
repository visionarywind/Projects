# 跨模块调用链

- 对应源码版本：HEAD `cfe4725d23`（2026-09-17）。
- 证据状态：Python normal task、对象和调度代表路径已静态闭合；运行/性能未验证。

## 任务与对象主链

```text
ray.remote / .remote
→ RemoteFunction._remote:355-574
→ worker.core_worker.submit_task:537-556
→ _raylet.pyx submit_task:3938-4032
→ CoreWorker::SubmitTask:2056-2135
→ TaskManager::AddPendingTask
→ NormalTaskSubmitter::SubmitTask:33-504
→ dependency resolution / scheduling-key queue
→ RequestWorkerLease → Raylet local/cluster lease
→ PushNormalTask → worker execution
→ Object Manager/Object Store result
→ worker.get:2881-3029
→ worker.get_objects:2996-2998
→ value 或 RayError
```

M01/M02 提供请求和 pending task；M04 提供资源/worker lease；M03 提供对象内容传输；M05 提供节点、任务和控制元数据。箭头包含调用、RPC、异步投递或数据传递，不表示同步完成。

## 启动与控制面

```text
ray.init:1439-1505
→ CoreWorkerProcess initialization:231-285
→ Raylet/GCS registration
→ GcsServer Start/DoStart/RegisterRpcServices:292-497
→ node/resource/job/actor/task/pubsub state
```

M06 的 CLI/runtime env/autoscaler 影响启动和资源供给；M07 消费控制面状态与事件，不替代执行算法。

## AI library 组合

- D02：Tune trial → `ray.data.range`/Dataset executor → Core tasks/ObjectRefs → trial result。
- D03：Serve API → controller/deployment state → replica actor/router → handle response。
- Train/Tune/RLlib/Serve/Data 都复用 Core 的 task/actor/object/resource 边界，但各自 controller/state machine 独立。

## 相关文档

[项目入口](../README.md) · [分析状态](../00-overview/analysis-state.md) · [M01 调用链](../01-modules/M01-public-api/call-chains.md) · [D01](../80-demos/D01-hello-world/execution-trace.md)

## 源码证据摘要

`python/ray/remote_function.py:355-574`；`python/ray/_raylet.pyx:3938-4032`；`src/ray/core_worker/core_worker.cc:2056-2135`；`src/ray/core_worker/task_submission/normal_task_submitter.cc:33-504`；`src/ray/object_manager/object_manager.cc:66-718`；`src/ray/raylet/node_manager.cc:199-373,575-609`；`src/ray/gcs/gcs_server.cc:292-497`。

## 未解决问题

动态进程、网络、GPU、重试、跨节点故障和性能结论需要构建/运行日志；Actor/Java/C++ 全链仍需专题。

## 下一步阅读建议

先沿 D01 复核 normal task，再沿 D02/D03 进入 Data/Tune 和 Serve 的独立控制器。
