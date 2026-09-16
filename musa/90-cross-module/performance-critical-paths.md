# 性能关键路径

## 已能从源码定位的热点

1. `Platform::Init` 的设备发现和 `call_once` 初始化。[src/musa/core/platform.cpp:84-140]
2. `Context::ResolveDependencyAndQueueCommand` 的平台串行锁、依赖扫描和队列操作。[src/musa/core/context.cpp:1984-2038]
3. Stream 的 `QueueCommand`、合并列表和异步提交线程。[src/musa/core/stream.cpp:1048-1220]
4. MemoryTracker 地址区间查找、peer mapping 和多设备同步。[src/driver/mu_memory.cpp:105-125] [src/musa/core/memory.cpp:117-145]

## 测量建议

记录 API、queued、submitted、begin/end、completed 时间戳，并区分 CPU 等待、队列等待和 GPU 执行。`MUSA_TRACE_SUBMIT_FILE` 提供 submit trace 写文件入口，但实际格式和开销需目标机验证。[src/musa/core/stream.cpp:46-63]

不能凭源码行数声称性能提升；需基准、配置、设备型号和统计方法共同记录。
