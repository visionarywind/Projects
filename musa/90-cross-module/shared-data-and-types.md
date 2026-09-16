# 共享数据与类型

| 类型/数据 | 生产者 | 消费者 | 生命周期/风险 |
|---|---|---|---|
| `MUdevice` | Platform/Device | API/Context | ordinal 与可见设备映射 |
| `MUcontext` | Device | TLS/API/资源 | current context 栈 |
| `MUdeviceptr` | Memory/HAL | pointer/copy/kernel | tracker 区间和 offset |
| `MemoryCreateInfo` | Driver/API | Memory/HAL | type/property/view 决定分支 |
| `Command` | Stream/Context | submit/wait/MUPTI | shared_ptr 保活 |
| `GraphNode` | Context/capture | GraphExec/Stream | 拓扑、依赖、资源引用 |
| correlation/submission id | ThreadInfo/Stream | profiler/debugger | 异步排障关键 |

来源：[src/musa/core/context.cpp:1037-1081] [src/musa/core/stream.cpp:1048-1220] [src/driver/mu_entry.cpp:121-198]。
