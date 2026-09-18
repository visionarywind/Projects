# Raylet 调度与资源：示例与观测点

```python
# 伪代码，仅表达接口边界，未运行
handle = entry(config)
result = handle.run()
```

观察入口状态、正常链 `依赖满足 → RequestWorkerLease → 本地匹配或 spillback → worker 注册 → PushNormalTask`、错误 `不可调度、worker/Raylet 断连、placement group 删除。` 和清理 `DestroyWorker、disconnect/failure、node removal 释放资源。`。
