# M03 Runtime 开发指南

## 修改前

- 找到公开 C API、对应 `Api` 方法和内部对象的完整链路。
- 确定错误码是否需要扩展映射，保留底层错误上下文。
- 明确资源的创建线程、Context 归属、异步完成和销毁顺序。
- 判断变更属于 KernelMemoryPool、SOMA 还是普通 device/Host memory；三条路径不能用同一套生命周期假设。
- 对 SOMA 变更核对 `SegmentManager` 状态、PoolRegistry ownership、AICPU 参数和 HAL/SDK-driver ABI；特别确认 `TryToReuse` 与 `MemPoolTrimImplicit` 的当前实现状态。
- 检查 CMake 目标、安装库和 ACL 导出表是否需要同步修改。

## 修改中

- 复用现有验证和错误宏，不在单个 API 中引入另一套返回规则。
- 不直接暴露内部对象布局；保持 C ABI 稳定。
- 所有锁都要检查异常路径、锁顺序和潜在回调重入。
- 变更调度或内存时，同时考虑 DFX、profiling、dump 和 Driver 协议。
- 不把 Segment 的本地状态变化写成设备侧已完成；区分提交成功、callback/event 完成和同步完成。
- 若启用或恢复 SOMA 重用/隐式 trim，必须补足策略选择、失败回滚、统计一致性和跨 Stream/Event 测试，不能只修改辅助函数。

## 修改后

先跑无设备 UT，再在匹配 Toolkit/Driver/Firmware/NPU 上跑集成和压力测试；记录版本、配置、设备和失败底层错误码。对内存变更至少覆盖重复 free、越界、异步提前释放、trim 失败、Context/Stream 销毁失败以及普通 free 与同步 free 的差异。
