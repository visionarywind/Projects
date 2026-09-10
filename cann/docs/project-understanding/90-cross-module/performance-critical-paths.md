# 跨模块：性能关键路径

- 证据状态：路径和潜在瓶颈已确认/推断；无设备性能数据

## 路径

```text
模型编译：Parser → Graph/IR → Pass/Engine → memory/stream planning → serialization
模型执行：Input copy/bind → Runtime enqueue → Driver queue/HDC → device compute → sync/callback
普通内存：policy parse → DevMemAlloc → halMemAlloc → backing
SOMA 异步内存：Segment split/cache → HAL async config → SomaMemMng → Driver VMM/ioctl → device-side completion
```

GE 文档确认图优化、流分配、内存复用和 Sink/SuperKernel 能力 `[ge/docs/zh/design/architecture.md:129-189]`；V2 Execute 的 IO 指定、资源占用和 ExecuteGraph 顺序见 `[ge/runtime/v2/core/model_v2_executor.cc:260-305]`。SOMA 的本地 Segment 分配和 AICPU/Driver 配置路径见 `[runtime/src/runtime/feature/soma/stream_mem_pool.cc:150-195]` `[runtime/src/runtime/api/impl/api_impl_soma.cc:64-109]`。

## 可能瓶颈

- 主机逐任务下发和频繁同步。
- Host-device 拷贝、未对齐 buffer 或不必要的内存分配。
- KernelMemoryPool first-fit、固定 2 MiB backing 和缺少显式邻接合并带来的碎片；需压力测试确认 `[runtime/src/runtime/core/src/pool/memory_pool.cc:15-106]` `[runtime/src/runtime/core/src/pool/memory_list.cc:14-122]`。
- SOMA FREE Segment 使用按大小升序的 `std::set` 和 `lower_bound` 实现 Best-Fit；Segment 分裂/缓存、AICPU `SomaMemMng` 提交、HAL/Driver VMM 往返仍可能成为开销。`TryToReuse` 当前 no-op，不能把 cached 依赖重用策略写成已获得收益 `[runtime/src/runtime/feature/soma/stream_mem_pool.hpp:67-75]` `[runtime/src/runtime/feature/soma/stream_mem_pool.cc:345-361]`。
- Stream/Event 资源竞争和 Driver queue/HDC 往返。
- 编译 Pass、算子编译和模型加载时间。
- DFX/profiling/dump 开销。

## 测量原则

分别测量编译、Load、首轮执行、稳态执行、同步、数据传输、普通内存分配、SOMA create/alloc/free/trim 和卸载；同时记录 batch/shape、Stream 数、设备型号、频率、配置和 profiling 开销。没有真实 NPU 时只能做静态复杂度和代码路径分析。

## 优化边界

先证明瓶颈属于 GE、Runtime、Driver 还是设备 Kernel，再修改对应层；不能只优化 API 包装而忽略同步、队列和内存 backing 行为。任何声称 SOMA 重用或隐式 trim 带来收益的结论，都必须先确认目标源码实现和设备测试结果。
