# HAL/M3D 适配：接口契约

- 文档目的：hal m3d：接口签名、前置条件、错误码与跨层契约。
- 适用范围：M09 模块的《HAL/M3D 适配：接口契约》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


## 构建目标 `halM3d`

`src/hal/m3d/CMakeLists.txt` 定义一个 STATIC 库目标，并拉起私有子模块 `m3d`：

| 行 | 内容 | 后果 |
|---|---|---|
| [src/hal/m3d/CMakeLists.txt:1-12] | `M3D_CLIENT_MUSA 1`、`M3D_CLIENT_INTERFACE_MAJOR_VERSION 20`、`M3D_BUILD_MT_DEVICE 1`、`SCPC_CLIENT_INTERFACE_MAJOR_VERSION 2`、`SCPC_BUILD_MT_COMPILER 1`、`M3D_BUILD_RAYTRACING 1`、`M3D_BUILD_CMD_PFM 1`、`M3D_BUILD_GPU_PROFILER 1` | 编译期特征开关，进入 m3d 子模块 |
| [src/hal/m3d/CMakeLists.txt:13-21] | WIN32 → `M3D_KMT_BUILD 1`；UNIX → `M3D_LINUX_BUILD 1`；`MUSA_WSL2_BUILD` → `M3D_WSL2_BUILD 1` + `M3D_KMT_BUILD 1` | 平台分支影响后续定义与子模块 |
| [src/hal/m3d/CMakeLists.txt:22-23] | `set(ENV{M3D_LIB_ROOT_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/m3d)` 后 `add_subdirectory(m3d)` | 私有子模块是独立构建单元，本目录只声明边界 |
| [src/hal/m3d/CMakeLists.txt:25-35] | `M3D_INC_DIRS` 含 `m3d/inc/core`、`m3d/inc/util`、`m3d/inc/scpc`、`m3d/src/core/hw/scpc/mthreads` 等 | 决定本目录代码可见的头文件域 |
| [src/hal/m3d/CMakeLists.txt:36-40] | `M3D_LIBS` = `m3d;scpc;util` | 三个私有库，均由子模块提供 |
| [src/hal/m3d/CMakeLists.txt:41-54] | 与 1-21 同步的 `add_definitions`（含 `-DM3D_LINUX_BUILD=1` 等） | 编译宏与 `set()` 双写，易漏同步 |
| [src/hal/m3d/CMakeLists.txt:56-57] | `aux_source_directory` 收集目录全部 `.cpp`；`add_library(halM3d STATIC ${HAL_M3D_SRC})` | 目录内任何新增 `.cpp` 会被自动编入 |
| [src/hal/m3d/CMakeLists.txt:58-59] | `target_include_directories(halM3d PRIVATE ${M3D_INC_DIRS})`；`target_link_libraries(halM3d PRIVATE ${M3D_LIBS} $<$<BOOL:${UNIX}>:dl;pthread;rt>)` | 头文件与依赖均为 PRIVATE，消费方通过全局 include 根访问 |

## HAL 接口族契约

| 接口族 | 头文件 | 职责 | 关键成员 | 契约位置 |
|---|---|---|---|---|
| `Hal::IPlatform` | `src/hal/halPlatform.h` | 设备枚举、P2P 能力矩阵、平台内存属性、设置 | `GetDeviceCount`、`GetDevices`、`GetDeviceP2pCapability`、`GetMemProperties`、`CreateMemory` | [src/hal/halPlatform.h:91-152] |
| `Hal::IDevice` | `src/hal/halDevice.h` | 创建所有 HAL 对象、设备属性、heap 信息、暴露 MemMgr | `CreateQueue`、`CreateCmdBuffer`、`CreateMemory`、`Finalize`、`GetHeapInfos`、`GetMemMgr` | [src/hal/halDevice.h:279-372] |
| `Hal::IQueue` | `src/hal/halQueue.h` | 提交命令缓冲、Paging、同步、鲁棒信息、优先级 | `Submit`、`Paging`、`WaitIdle`、`QueryRobustInfo`、`SetPriority`、`IsUserQueue` | [src/hal/halQueue.h:270-336] |
| `Hal::IMemory` | `src/hal/halMemory.h` | 尺寸/地址/属性查询、Map/Unmap、peer/external 打开 | `GetSize`、`GetDeviceVirtualAddress`、`Map`、`Unmap`、`GetHeap`、`GetProperties` | [src/hal/halMemory.h:282-407] |
| `Hal::IMemMgr` | `src/hal/halMemMgr.h` | 池注册/查找、分配/释放、内部池、用户池 | `FindPool`、`GetPool`、`GetInternalPool`、`CreateUserPool`、`Allocate`、`Free` | [src/hal/halMemMgr.h:15-40] |
| `Hal::IMemoryPool` | `src/hal/halMemoryPool.h` | 子分配、chunk 分配、释放、trim、水线统计 | `SubAllocate`、`ChunkAllocate`、`FullAllocate`、`Free`、`TrimPool` | [src/hal/halMemoryPool.h:62-103] |
| `Hal::ICmdPool` | `src/hal/halCmdPool.h` | 命令池 Reset | `ICmdPool::Reset` | [src/hal/halCmdPool.h:16-32] |
| `Hal::ICmdBuffer` | `src/hal/halCmdBuffer.h` | 命令录制状态机与录制命令 | `GetState`、`Begin`、`End`、`Reset`、`CmdBindKernel` | [src/hal/halCmdBuffer.h:354-416] |

## 核心结构体与枚举

| 类型 | 头文件 | 要点 | 位置 |
|---|---|---|---|
| `MemoryPoolInfo` | `halMemoryPool.h` | 五字段：`type`、`heap`、`property`、`viewCapability`、`numaId`；是池查找键的来源 | [src/hal/halMemoryPool.h:8-14] |
| `MemoryPoolCreateInfo` | `halMemoryPool.h` | `info`、`usageFlags`、`internalType`、`minEnlargeChunkSize`、`reuseCountLimit`、`reuseSizeLimit`、`size`、`maxSize`、`alignment` | [src/hal/halMemoryPool.h:25-35] |
| `MemoryAllocInfo` | `halMemory.h` | `type`、`heap`、`property`、`viewCapability`、`size`、`alignment`、`numaId` | [src/hal/halMemory.h:90-100] |
| `MemoryHeap` | `halMemory.h` | `general`/`largePage`/`texture`/`pds`/`usc` 等 | [src/hal/halMemory.h:63-76] |
| `MemoryProperty` | `halTypes.h` | 位掩码枚举，`Virtual`/`Physical`/`HostVisible`/`DeviceVisible` 等 | [src/hal/halTypes.h:132-202] |
| `MemoryViewCapability` | `halTypes.h` | `Shareable`/`PeerAccessible`/`IpcExportable`/`Exportable` 等 | [src/hal/halTypes.h:207-233] |
| `Result` | `halTypes.h` | 与 `MUresult` 同序的错误枚举 | [src/hal/halTypes.h:327-446] |
| `InternalMemoryPoolType` | `halTypes.h` | `Default`/`HostMapped`/`Host`/`Profile`/`Shader`/`Count` | [src/hal/halTypes.h:460-467] |
| `QueueFamily` | `halQueue.h` | `name[16]`、`capability`、`timestampQueryInfo`、`count` | [src/hal/halQueue.h:65-75] |
| `QueueSubmitInfo` | `halQueue.h` | cmdBuffers、wait/signal 信号量数组、fence、submissionId | [src/hal/halQueue.h:93-147] |

## 结果转换边界

底层 `::M3d::Result`、`Util::Result`、硬件异常统一转成 `Hal::Result`：

| 转换函数 | 位置 | 说明 |
|---|---|---|
| `M3dToHalResult` | [src/hal/m3d/common.h:175-177] | 转发 `ResultCast` |
| `ResultCast` | [src/hal/m3d/common.h:44-173] | `Util::Result` → `Hal::Result` 全表映射 |
| `ScpcToHalResult` | [src/hal/m3d/common.h:179-181] | SCPC 编译结果转换 |
| `ExceptionToHalResult` | [src/hal/m3d/common.h:183-311] | `::M3d::HardWareExceptionType` → `Hal::Result` |

`Hal::Result` 注释明确「现在错误码与 MUresult 相同」（[src/hal/halTypes.h:327-329]），转换表只做语义等价，不做额外归一化。未映射的 `Util::Result` 值统一落入 `errorUnknown`/`errorInvalidValue`。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/hal/m3d/CMakeLists.txt`（8 处，例：[src/hal/m3d/CMakeLists.txt:1-12]）
- `src/hal/halTypes.h`（5 处，例：[src/hal/halTypes.h:132-202]）
- `src/hal/m3d/common.h`（4 处，例：[src/hal/m3d/common.h:175-177]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
