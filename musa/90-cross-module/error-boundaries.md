# 跨模块错误边界

```text
API参数错误 -> Driver API
对象/所有权错误 -> Platform/Context/Memory
底层资源错误 -> HALToMuResult边界
异步执行错误 -> Stream Query/Synchronize/robust info
工具输出错误 -> checkMuErrors/CLI
```

每一层都应保留原始上下文（device、context、stream、correlation id）。异步 enqueue 返回成功不等于 GPU 工作成功；这是由 `AsyncSubmit` 与 `GetEngineLastError` 的分离可见的。[src/musa/core/stream.cpp:1113-1146] [src/musa/core/stream.cpp:1154-1220]
