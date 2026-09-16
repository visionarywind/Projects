# 全局错误模型

## 分层

1. Driver API 检查空指针、ordinal、当前 context、句柄类型并返回 `MUresult`。[src/driver/mu_device.cpp:14-45] [src/driver/mu_memory.cpp:271-320]
2. Core 对象方法返回 `MUresult`，失败时通常不把对象加入 Context/Platform tracker。[src/musa/core/context.cpp:1037-1089]
3. HAL result 通过 `HalToMuResult` 转换；该转换在 memory、queue、semaphore 初始化处普遍出现。[src/musa/core/memory.cpp:510-513] [src/musa/core/stream.cpp:950-1005]
4. 异步错误通过 stream/queue 查询和最后错误传播，不能仅依据 enqueue 的返回值判断执行完成。[src/musa/core/stream.cpp:1113-1146]

## 典型失败路径

```text
非法参数/无当前 context -> API 立即返回
HAL Create/Allocate 失败 -> Core 初始化失败 -> 不登记或回滚
enqueue 成功、GPU 执行失败 -> Query/Synchronize 时暴露错误
销毁前仍有命令 -> Synchronize/Destroy 可能失败或等待
```

## 排障原则

记录 API 返回值、当前 device/context、stream、correlation/submission id；对异步问题同时检查 `Query`、`Synchronize` 和 engine robust info。不要把未在目标环境执行的命令输出写成事实。
