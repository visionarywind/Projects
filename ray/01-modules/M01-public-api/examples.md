# 公共 API 与句柄：示例与观测点

```python
# 伪代码，仅表达接口边界，未运行
handle = entry(config)
result = handle.run()
```

观察入口状态、正常链 `@ray.remote → RemoteFunction._remote → _raylet submit_task → CoreWorker → ObjectRef → ray.get`、错误 `序列化失败、RayTaskError、ObjectLostError、GetTimeoutError。` 和清理 `显式 shutdown 清理 driver 状态；动态未验证。`。
