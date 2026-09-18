# CoreWorker、任务提交与执行：示例与观测点

```python
# 伪代码，仅表达接口边界，未运行
handle = entry(config)
result = handle.run()
```

观察入口状态、正常链 `Python/Cython → CoreWorker::SubmitTask → TaskManager::AddPendingTask → NormalTaskSubmitter → lease → PushNormalTask`、错误 `依赖/ runtime env / worker 启动失败、不可调度、Raylet 失联。` 和清理 `临时 put refs 在 binding 中移除；shutdown 回收 runtime。`。
