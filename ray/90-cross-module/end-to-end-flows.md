# 端到端流程

- 对应源码版本：HEAD `cfe4725d23`（2026-09-17）。
- 证据状态：D01 normal task、D02 Data/Tune、D03 Serve 的代表静态路径已建立；运行/性能未验证。

## D01 任务流程

1. 启动：`ray.init` 连接/启动 runtime，driver 建立 CoreWorker。[`worker.py:1439-1505`]
2. 声明：`@ray.remote` 构造 RemoteFunction。[`remote_function.py:90-182`]
3. 提交：`_remote` → `_raylet.pyx` → `CoreWorker::SubmitTask` → pending task。[`remote_function.py:355-574`; `_raylet.pyx:3938-4032`; `core_worker.cc:2056-2135`]
4. 调度：NormalTaskSubmitter 解析依赖、创建 scheduling key、申请 worker lease；Raylet 进行本地匹配或 spillback。[`normal_task_submitter.cc:33-504`; `node_manager.cc:199-373`]
5. 执行：worker 收到 `PushNormalTask`，写入返回对象；动态执行未验证。
6. 获取：`ray.get` → `get_objects` → 反序列化 value/error。[`worker.py:2881-3029`]
7. 清理：显式/隐式 shutdown、引用和进程回收；具体时序未验证。

## D02 Data/Tune 流程

1. `ray.init(num_cpus=4)` 建立资源环境。
2. Tune 创建带 CPU 资源的 Trial，并限制 `max_concurrent_trials=3`。
3. Trial 执行 `ray.data.range(10).show()`；Dataset lazy plan 在 show 时进入 Data executor，产生 blocks/tasks。
4. Trial result 返回 Tune controller；Data blocks 与 ObjectRefs 按 executor/trial 生命周期释放。
5. 原脚本注释说明并发 4 可能让 Data 缺少资源，但该现象未运行验证。[`doc/source/data/doc_code/key_concepts.py:4-24`]

## D03 Serve 流程

1. `MyFirstDeployment.bind("Hello world!")` 创建 application DAG。
2. `serve.run` 通过 controller client 提交 deployment；controller 更新 DeploymentState。
3. replica actor 启动并持有 `msg`；router 将 `DeploymentHandle.remote()` 请求派发到 replica。
4. `.result()` 等待 response，返回字符串；Serve controller/router/replica 的真实进程时序未验证。[`doc/source/serve/doc_code/key_concepts.py:2-20`]

## 统一错误边界

- driver/API：参数、连接和序列化错误。
- Core/Raylet：依赖、资源、lease、worker 和调度错误。
- Object：存储满、对象丢失、传输/owner 错误。
- Library controller：Trial、Dataset、deployment、replica 和用户函数错误。
- cleanup：每层应处理取消、失败和正常结束；本仓库没有动态日志证据。

## 相关文档

[项目入口](../README.md) · [分析状态](../00-overview/analysis-state.md) · [跨模块调用链](cross-module-call-chains.md) · [Demo 注册表](../80-demos/demo-registry.md)

## 源码证据摘要

`python/ray/_private/worker.py:1439-1505,2881-3029`; `python/ray/remote_function.py:90-182,355-574`; `python/ray/_raylet.pyx:3938-4032`; `src/ray/core_worker/core_worker.cc:2056-2135`; `src/ray/core_worker/task_submission/normal_task_submitter.cc:33-504`; D02/D03 官方 doc_code 脚本。

## 未解决问题

动态进程、网络、GPU、重试、跨节点数据和性能结论需要真实构建/运行日志。

## 下一步阅读建议

按 D01→D02→D03 顺序阅读；先验证 Core 任务，再验证 Data/Tune 资源交互和 Serve replica 生命周期。
