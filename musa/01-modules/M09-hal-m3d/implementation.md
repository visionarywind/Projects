# HAL/M3D 适配：实现说明

## 正常实现路径

```text
HAL interface -> M3D device/queue/memory/cmd buffer
```

源码证据：[src/hal/m3d/CMakeLists.txt:1-59] [src/musa/core/stream.cpp:927-1005]

## 关键实现观察

- 入口与内部对象之间存在显式转换/查找，而不是把 C 句柄当作任意指针直接使用。
- 失败通常沿 `MUresult` 向上传递；HAL 结果在 Core 边界转换。
- 资源登记、追踪和析构是实现正确性的组成部分，不应只阅读成功分支。

## 分支矩阵

| 分支 | 触发 | 结果 | 当前证据 |
|---|---|---|---|
| 参数/句柄无效 | NULL、越界、无 current context | 立即错误 | 已确认于 API 层 |
| 初始化失败 | HAL/对象 `Init` 返回错误 | 不完成登记或回滚 | 已确认于部分路径 |
| 异步模式 | stream/setting 开启 | 入队后返回 | 已确认于 Stream/Memory |
| capture 模式 | stream 正在 capture | 创建 GraphNode | 已确认于 Stream/Context |
| 跨设备/特殊类型 | peer、managed、IPC、virtual | 选择专门 HAL 路径 | 部分确认 |

## 尚未证明

该模块是否在所有错误分支都释放中间对象、是否所有底层错误都有稳定映射，必须结合完整函数体和目标机测试确认。
