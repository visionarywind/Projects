# CoreWorker、任务提交与执行：实现

## 入口到副作用
Cython 转换 options；C++ 生成 TaskSpec，异步解析依赖、排队、申请 lease 和推送任务。

## 正常
Python/Cython → CoreWorker::SubmitTask → TaskManager::AddPendingTask → NormalTaskSubmitter → lease → PushNormalTask

## 分支与异常
- 依赖未就绪、idle worker、spillback、placement group、取消、generator backpressure。
- 依赖/ runtime env / worker 启动失败、不可调度、Raylet 失联。

## 清理
临时 put refs 在 binding 中移除；shutdown 回收 runtime。

## 数据与执行上下文
- TaskSpec、TaskID、ObjectRef、pending task、scheduling key、lease。
- driver/worker CoreWorker 的 IO service 与 RPC 回调并发。
