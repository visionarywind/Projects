# 下一步

## P0

- 在目标源码版本补齐 M09 `IPlatform/IDevice/IQueue/IMemory` 到 M3D 具体函数。
- 展开 `DispatchCommand`、`MemcpyCommand` 的 `Submit`、错误和资源释放。
- 完成 D01 目标机运行记录（若获得授权）。

## P1

- 读取 graph/mempool/error/debug 测试并建立 D02/D04 实际轨迹。
- 为 export/accessor 建立自动符号快照。
- 记录多 stream、peer、异步分配的性能基线。

## 完成标准

每个主模块至少有入口、正常路径、分支、异常、清理、所有权、并发、行级证据和测试映射；任何未执行的命令仍标记为未验证。
