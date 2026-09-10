# M03 Runtime 测试

- 证据状态：测试能力和入口已确认；结果未执行

## 分层测试

- C API 参数、句柄魔数和错误宏 UT。
- Device/Context/Stream/Event/Memory 生命周期 UT。
- KernelMemoryPool 的 0 字节、>2 MiB、read-only 分池、碎片和空闲池回收测试。
- SOMA 的 pool create/destroy、Segment split、重复 free、越界、BUSY/CACHED/FREE 状态、trim 和 AICPU 配置失败测试。
- scheduler、queue、异步同步和超时测试。
- DFX 日志、trace、profiling、dump 和错误上下文测试。
- 与 ACL、GE、Driver、固件和真实 NPU 的集成测试。

## 回归重点

1. `rtSetDevice` 的全局锁、Primary Context retain 和多线程初始化/Finalize。
2. 无效 handle、重复释放和跨 Context 使用。
3. 异步任务完成前释放 buffer/stream/event；对比普通 free 与 `rtFreeWithDevSync`。
4. SOMA `TryToReuse` 当前 no-op、显式 trim 和隐式 trim no-op 的行为回归，避免把辅助策略误当成已启用功能。
5. AICPU `SomaMemMng` launch 失败、本地 free 回滚和 Driver V3 free ioctl“不回滚”窗口。
6. SoC 缓存与多设备切换。
7. 资源限制 API 的设置、重置、查询一致性。

## 验证限制

构建/测试依赖和命令见 Runtime 仓 `AGENTS.md` 与 README；本知识库当前未执行这些命令，不把文档命令写成已通过的结果。没有匹配 NPU、固件和 Toolkit 时，SOMA 设备侧完成、物理 trim、busy destroy 和性能收益均为“未验证”。
