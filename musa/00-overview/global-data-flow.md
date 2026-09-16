# 全局数据流

## 控制流与数据流

```text
API 参数/句柄
  -> 参数归一化(v1/v2、默认 stream、copy descriptor)
  -> C++ 对象查找/创建
  -> Memory/Module/Node 参数对象
  -> Command + dependency
  -> Stream engine resource
  -> HAL queue/cmd buffer/memory
  -> MUresult、输出参数、异步完成
```

### 设备查询

`muInfo` 先获取 count，再按 ordinal 获取 device，随后查询 name、total memory、attribute 和 UUID 等字段，最后格式化为 stdout。[src/tools/muInfo.cpp:120-267] [src/tools/muInfo.cpp:302-369]

### 内存

设备指针不是裸的“可随意释放地址”：API 通过 Platform 的 MemoryTracker 查找区间和偏移，再由 Context/Memory 决定类型与释放方式。[src/driver/mu_memory.cpp:105-125] [src/musa/core/context.cpp:1037-1100]

### 异步操作

异步 API 把 host 参数转换成内部参数，然后选择 stream，创建 graph node 或 Command，并根据 `memBlocking`/`launchBlocking` 决定 API 是否等待。[src/driver/mu_memory.cpp:1004-1022] [src/musa/core/context.cpp:696-890]

## 重要数据实体

| 实体 | 创建者 | 主要消费者 | 生命周期索引 |
|---|---|---|---|
| `MUdevice` | Platform/Device | device API、Context | Platform |
| `MUcontext` | Device | driver API、资源对象 | Device/Platform |
| `MUdeviceptr` | Memory/HAL | copy/kernel/pointer API | MemoryTracker + Context |
| `Command` | Stream/Context | submit/wait/profiler | Stream lists |
| `GraphNode` | Context/Graph | capture/graph exec | Graph/Context |
