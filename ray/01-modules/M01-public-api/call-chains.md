# M01 调用链

## 远程函数

```text
hello_world.remote()
→ RemoteFunction.remote proxy [remote_function.py:172-182]
→ RemoteFunction._remote [355-574]
→ global_worker.core_worker.submit_task [537-556]
→ `_raylet.pyx:3938-4032`：参数/资源/调度转换与 CTaskOptions
→ `CoreWorker::SubmitTask` [core_worker.cc:2056-2135]
→ `TaskManager::AddPendingTask` + io_service async post
→ `NormalTaskSubmitter::SubmitTask` [normal_task_submitter.cc:33-504]
→ dependency resolution / scheduling-key queue / RequestWorkerLease
→ Raylet grant/redirect/reject
→ PushNormalTask → Worker 执行
→ M03 保存结果对象
```

该链是源码静态调用和异步消息边界的组合；没有真实集群运行证据，因此不代表具体节点选择或时序已验证。

## 获取结果

```text
ray.get(ref)
→ worker.get [2881-3029]
→ worker.get_objects [2996-2998]
→ CoreWorker/Object Manager
→ deserialize value 或 RayError
→ RayTaskError.as_instanceof_cause / return value
```

## 初始化

```text
ray.init
→ worker.init [1439-1505+]
→ 地址解析：显式地址/RAY_ADDRESS/本地发现
→ 连接已有集群或启动本地 runtime
→ CoreWorkerProcess 初始化 [core_worker_process.cc:231-285]
```

## 分支边界

- client mode、generator return、placement group 和 labels 在 `RemoteFunction._remote`/binding 中改变 options 或返回句柄。
- 依赖失败、runtime env failure、lease rejection、worker/Raylet failure 在 M02/M04 进入失败或重试路径。
- `ray.get` 对单 ref、列表、timeout、`RayTaskError`、`ObjectLostError` 分支处理。

## 相关文档

[README](README.md) · [跨模块调用链](../../90-cross-module/cross-module-call-chains.md) · [M02](../M02-core-worker/call-chains.md)

## 源码证据摘要

`python/ray/remote_function.py:355-574`；`python/ray/_raylet.pyx:3938-4032`；`src/ray/core_worker/core_worker.cc:2056-2135`；`src/ray/core_worker/task_submission/normal_task_submitter.cc:33-504`；`python/ray/_private/worker.py:2881-3029`。

## 未解决问题

Actor、Java/C++ API、跨节点故障和动态性能需要专项源码/运行验证；Python normal task 代表链已静态闭合。

## 下一步阅读建议

进入 M02 读 pending task/lease，再进入 M04/M03 读资源和对象数据路径。
