# 术语表

| 术语 | 含义 |
|---|---|
| Driver API | 对外 C ABI，源码中主要是 `muapi*` 实现及 `mu*` wrapper |
| Platform | 进程级单例，持有 HAL platform、可见 Device 和全局 tracker |
| Context | 资源归属与当前执行语义边界 |
| MemoryTracker | 按地址区间查找 Memory 和 offset 的全局索引 |
| Stream | 命令顺序、队列、同步和 capture 的抽象 |
| Command | 一次可排队的 kernel/copy/memset/graph 操作 |
| HAL | core 与具体 M3D 实现之间的 C++ 接口 |
| M3D | `src/hal/m3d/m3d` 子模块及其设备实现 |
| v1/v2 | 兼容 API 版本；常见 v1 包装 v2 并扩展整数宽度 |
| primary context | Device 持有的默认 Context，`muDevicePrimaryCtx*` 操作它 |
| per-thread default stream | 按线程懒创建并缓存的默认 stream |
| capture | 将 stream 操作收集为 GraphNode，而不是立即提交 |
| evidence status | 文档中的“已确认/推断/未知/未验证”证据标记 |
