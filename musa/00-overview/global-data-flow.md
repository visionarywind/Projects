# 全局数据流

- 文档目的：跨模块数据流的走向、类型与关键节点证据。
- 适用范围：00-overview《全局数据流》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`project-overview.md`](project-overview.md)、[`../README.md`](../README.md)
- 后续阅读：[`architecture.md`](architecture.md)、[`../99-roadmap/reading-guide.md`](../99-roadmap/reading-guide.md)


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

## 相关文档

- [`project-overview.md`](project-overview.md)
- [`architecture.md`](architecture.md)
- [`../README.md`](../README.md)
- [`../01-modules/module-registry.md`](../01-modules/module-registry.md)
- [`../99-roadmap/reading-guide.md`](../99-roadmap/reading-guide.md)

## 源码证据摘要

- `src/tools/muInfo.cpp`（2 处，例：[src/tools/muInfo.cpp:120-267]）
- `src/driver/mu_memory.cpp`（2 处，例：[src/driver/mu_memory.cpp:105-125]）
- `src/musa/core/context.cpp`（2 处，例：[src/musa/core/context.cpp:1037-1100]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`../99-roadmap/risk-register.md`](../99-roadmap/risk-register.md)。

## 下一步阅读建议

阅读 [`architecture.md`](architecture.md)；阅读 [`../99-roadmap/reading-guide.md`](../99-roadmap/reading-guide.md)。
