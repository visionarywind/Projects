# HAL/M3D 适配：行级分析

- 文档目的：hal m3d：关键代码位置的行级证据与解读。
- 适用范围：M09 模块的《HAL/M3D 适配：行级分析》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


本页只登记我逐行读到的片段；没有逐行读取的函数一律不写成确定行为。

## 证据表

| 行号 | 观察 | 结论状态 |
|---|---|---|
| [src/hal/halPlatform.h:91-152] | `Hal::IPlatform` 是纯虚类：设备枚举、P2P 能力、平台内存属性、`CreateMemory`、设置 | 已确认（仅声明） |
| [src/hal/halMemMgr.h:15-40] | `Hal::IMemMgr` 共 9 个纯虚方法，`Allocate`/`Free` 是其中两条；析构为 protected | 已确认（仅声明） |
| [src/hal/m3d/memMgr.h:98-104] | `MakeKey` 把 property/capability/type/heap/numaId 按位移拼成 `DevSize` 键 | 已确认 |
| [src/hal/m3d/memMgr.cpp:85-96] | 分配入口三条规模校验：`size==0`、`size+alignment-1` 回绕、deviceLocal 超 `totalGlobalMem` | 已确认 |
| [src/hal/m3d/memMgr.cpp:173-198] | `CreatePoolNoLock`：`new MemoryPool` → `Init` 成功才登记，失败 `delete`，分配失败返回 `errorOutOfMemory` | 已确认 |
| [src/hal/m3d/memoryPool.cpp:85-98] | `FullAllocate` 先 `SubAllocate`，仅在 `errorNotFound` 时 `ChunkAllocate` 后再试一次 | 已确认 |
| [src/hal/m3d/memoryPool.cpp:240-260] | 释放时按 `isLeftMost`/`isRightMost` 与 `!busy` 判断，逐侧合并并 `FreeListRemove` | 已确认 |
| [src/hal/m3d/memoryPool.cpp:502-532] | `TrimPool` 只释放 `isLeftMost && isRightMost` 的整块空闲 chunk，直到 `m_TotalSize <= value` | 已确认 |
| [src/hal/m3d/cmdBuffer.cpp:145-197] | `CmdBuffer::Begin` 要求 `cmdBufferStateInitial`，否则 `errorIllegalState` | 已确认 |
| [src/hal/m3d/queue.cpp:182-353] | `Queue::Submit` 从入参装配 wait/signal 数组，到 `m_M3dQueue->Submit` 结束 | 已确认 |
| [src/hal/m3d/device.cpp:366-388] | `Device::Init` 顺序：`CommitSettingsAndInit` → `InitProps` → `ConstructHeapInfos` → `ConstructQueueFamilies` | 已确认 |
| [src/hal/m3d/platform.cpp:128-195] | `CreateDevices`：`EnumerateDevices` → 逐个 `new Device` + `Init` → 建 P2P 拓扑矩阵 | 已确认 |
| [src/hal/m3d/kernel.cpp:56-126] | `Kernel::Init` 从 `Library` 取 metadata、填 `KernelMetadata`、校验寄存器上限，末尾才 `CreatePipeline` | 已确认 |
| [src/hal/m3d/memory.cpp:127-177] | `Memory::Init` 按 `memoryTypeAlloc`/view 类型分派到 8 个 `Init*` 私有函数 | 已确认 |
| [src/hal/m3d/CMakeLists.txt:41-54] | `add_definitions(-DM3D_LINUX_BUILD=1)` 与前面的 `set(M3D_LINUX_BUILD 1)` 是同一宏的双写 | 已确认 |
| `src/hal/m3d/m3d` 子模块内部 | 未读取，只确认 `add_subdirectory` 边界 | 未验证 |

## 深挖顺序（行号锚点）

1. 先读参数与 early return：分配规模三连判断 [src/hal/m3d/memMgr.cpp:85-96]、状态机检查 [src/hal/m3d/cmdBuffer.cpp:146-148]。
2. 再读对象创建与登记：池登记分支 [src/hal/m3d/memMgr.cpp:184-190]、设备数组登记 [src/hal/m3d/platform.cpp:181-189]。
3. 最后读向下调用与回滚：chunk 创建 [src/hal/m3d/memoryPool.cpp:184-202]、创建失败 `delete` [src/hal/m3d/device.cpp:77-92]。

## 未覆盖的行

- `src/hal/m3d/cmdBuffer.cpp` 中 60 KB 的录制命令族（`CmdCopy*`、`CmdBuildAccelStruct` 等）只看了状态机部分，逐条命令的参数转换未读。
- `Device::Finalize` 的 settings 写入段、`Queue::QueryRobustInfo` 的 switch 分支未逐行核对。
- `src/hal/m3d/m3d` 私有子模块内容不在本模块范围内，本页不给出其行级结论。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/hal/m3d/memMgr.cpp`（4 处，例：[src/hal/m3d/memMgr.cpp:85-96]）
- `src/hal/m3d/memoryPool.cpp`（4 处，例：[src/hal/m3d/memoryPool.cpp:85-98]）
- `src/hal/m3d/cmdBuffer.cpp`（2 处，例：[src/hal/m3d/cmdBuffer.cpp:145-197]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
