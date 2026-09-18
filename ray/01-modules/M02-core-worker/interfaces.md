# CoreWorker、任务提交与执行：接口契约

## 输入
调用方须提供可验证、可序列化且符合生命周期的输入；入口证据：`python/ray/_raylet.pyx:3938-4032`、`src/ray/core_worker/core_worker.cc:2056-2135`、`src/ray/core_worker/task_submission/normal_task_submitter.cc:33-504`。

## 输出与副作用
- Python/Cython → CoreWorker::SubmitTask → TaskManager::AddPendingTask → NormalTaskSubmitter → lease → PushNormalTask
- 核心数据：TaskSpec、TaskID、ObjectRef、pending task、scheduling key、lease。

## 失败
依赖/ runtime env / worker 启动失败、不可调度、Raylet 失联。

跨模块修改需同步 binding/proto/日志/metrics/测试。
