# CoreWorker、任务提交与执行：执行流

## 正常流
```text
入口 → 配置/参数归一化 → 内部状态创建 → Python/Cython → CoreWorker::SubmitTask → TaskManager::AddPendingTask → NormalTaskSubmitter → lease → PushNormalTask → 结果/事件
```

## 分支
依赖未就绪、idle worker、spillback、placement group、取消、generator backpressure。

## 异常
依赖/ runtime env / worker 启动失败、不可调度、Raylet 失联。

## 终止
临时 put refs 在 binding 中移除；shutdown 回收 runtime。
