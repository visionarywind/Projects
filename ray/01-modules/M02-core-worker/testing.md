# CoreWorker、任务提交与执行：测试边界

|路径|覆盖目标|
|---|---|
|正常|Python/Cython → CoreWorker::SubmitTask → TaskManager::AddPendingTask → NormalTaskSubmitter → lease → PushNormalTask|
|分支|依赖未就绪、idle worker、spillback、placement group、取消、generator backpressure。|
|异常|依赖/ runtime env / worker 启动失败、不可调度、Raylet 失联。|
|清理|临时 put refs 在 binding 中移除；shutdown 回收 runtime。|

测试位置：src/ray/core_worker/。当前只定位源码/测试目录，未启动上游构建、集群或硬件，结果为未验证。
