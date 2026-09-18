# GCS、RPC、PubSub 控制面：示例与观测点

```python
# 伪代码，仅表达接口边界，未运行
handle = entry(config)
result = handle.run()
```

观察入口状态、正常链 `GcsServer Start/DoStart → managers/services → RPC/KV/PubSub → node/actor/task state`、错误 `storage/RPC failure、heartbeat timeout、订阅断开。` 和清理 `Stop 关闭 RPC、订阅和 managers；不等同于内容立即删除。`。
