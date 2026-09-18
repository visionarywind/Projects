# CoreWorker、任务提交与执行：调用链

## 主链
```text
Python/Cython → CoreWorker::SubmitTask → TaskManager::AddPendingTask → NormalTaskSubmitter → lease → PushNormalTask
```

## 实现落点
Cython 转换 options；C++ 生成 TaskSpec，异步解析依赖、排队、申请 lease 和推送任务。

## 失败链
```text
依赖/输入失败 → 依赖/ runtime env / worker 启动失败、不可调度、Raylet 失联。 → 重试、失败传播或清理
```
