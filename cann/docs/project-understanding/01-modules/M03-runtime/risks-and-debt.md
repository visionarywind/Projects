# M03 Runtime 风险与技术债务

- 证据状态：源码支撑的风险评估；不是已验证缺陷

## 高风险

- Runtime 与 ACL/GE/Driver/固件版本必须配套；单仓替换可能产生 ABI 和行为不一致 `[runtime/README.md:16-19]`。
- SOMA 的 `TryToReuse` 当前直接返回 `nullptr`，而依赖策略辅助函数仍存在；cached segment、统计和预期性能之间可能出现实现/文档偏差 `[runtime/src/runtime/feature/soma/stream_mem_pool.cc:140-195,246-334]`。
- `MemPoolTrimImplicit` 当前是 no-op，却保留了临时 workaround 注释；不能把调用点或设计意图当作真实回收机制 `[runtime/src/runtime/feature/soma/soma.cc:328-333]`。
- AICPU 异步配置、Runtime Segment 状态和 Driver VMM segment 的失败回滚并非完全对称；Driver V3 free ioctl 失败路径明确“不回滚” `[runtime/src/runtime/feature/soma/soma.cc:235-325]` `[driver/src/ascend_hal/svm/v3/api/master/svm_soma.c:862-901]`。
- 全局状态锁、初始化引用计数和异步任务交叠时，错误顺序可能泄漏资源或造成并发问题 `[runtime/src/acl/aclrt_impl/acl_rt_impl_base.cpp:25-151]`。

## 中风险

- KernelMemoryPool 使用固定 2 MiB backing、链表 first-fit 和释放尾插；源码未显示相邻空闲块合并，长期碎片行为需要压力测试 `[runtime/src/runtime/core/src/pool/memory_pool_manager.cc:69-201]` `[runtime/src/runtime/core/src/pool/memory_list.cc:14-122]`。
- `Api::Instance()` 隐藏深层实现，ACL Runtime 符号映射、Hook 和实现文件分散，打包/链接边界需要持续核对。
- Context teardown 失败时会恢复 owner Stream 以便重试；Stream、Model、SOMA callback 和设备 pending task 的组合状态尚未有完整故障矩阵 `[runtime/src/runtime/core/src/context/context.cc:759-785,813-927]`。
- 普通 `rtMalloc` 的 run mode、huge/normal/P2P/1G huge flags 和 SoC feature 分支多，跨产品实际行为尚未由设备矩阵覆盖 `[runtime/src/runtime/driver/npu_driver_mem.cc:1026-1114,1302-1326]`。

## 内存实现状态边界

- 已确认：KernelMemoryPool 为 Runtime 内部 2 MiB/First-Fit；SOMA FREE Segment 为按大小 Best-Fit；ordinary `rtMalloc` 可能进入 Driver V2/V3 cache。
- 源码显示未启用：SOMA `TryToReuse`；当前隐式 trim 仅返回成功。
- 未验证：Driver 实际产品选择、设备侧完成、物理 backing 算法、cache 命中率和性能收益。
- 技术债：为三条路径分别建立故障注入、统计一致性和跨 Context/Stream 测试，避免用一个“memory pool”指标覆盖不同实现。

