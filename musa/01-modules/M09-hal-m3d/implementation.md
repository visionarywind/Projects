# HAL/M3D 适配：实现说明

- 文档目的：hal m3d：核心函数/流程的逐段实现解读。
- 适用范围：M09 模块的《HAL/M3D 适配：实现说明》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


## 正常实现路径

```text
src/musa/core 持有 Hal::IBase 派生句柄
  -> 调用 src/hal/hal*.h 声明的纯虚方法
     -> src/hal/m3d/*.cpp 的 Hal::M3d::* 实现
        -> OwnPtr 包装的 IM3d::* 私有对象
           -> src/hal/m3d/m3d 子模块（边界之外，未读取）
```

平台这一层的入口是 `Hal::CreatePlatform`：`new M3d::Platform()` 后调 `Init`，失败即 `delete` 并返回错误 [src/hal/m3d/lib.cpp:6-13]。Core 侧用 `HalToMuResult` 把 `Hal::Result` 转回 `MUresult` [src/musa/core/internal_funcs.h:22-27]；`Hal::Result` 的注释说明现在这些错误码与 `MUresult` 相同 [src/hal/halTypes.h:327-331]。

## 关键实现观察

- **销毁统一走 `IBase::Destroy`**：`Hal::M3d::base.cpp` 把它实现成 `delete this`，因此所有 HAL 对象都必须由 `Destroy()` 而非 `delete` 释放 [src/hal/m3d/base.cpp:5-7]；Core 的 `HalPtr` 析构正是调用它 [src/musa/core/halPtr.h:26]。
- **IM3d 对象用 `OwnPtr` 托管**：`Reserve` 分配原始内存，`Release` 先 `Destroy()` 再 `deallocate`，类模板以 `IM3d::IDestroyable` 为约束 [src/hal/m3d/ownPtr.h:9-47]。
- **内存池有三类归属**：`m_InternalPoolRefs`（内部）、`m_PoolRefs`（自动注册）、`m_UserPools`（用户托管）三个容器由 `usageFlags` 决定写入哪一个 [src/hal/m3d/memMgr.h:122-127]。
- **引擎类型靠家族名字符串匹配**：`Queue::Init`、`CmdBuffer::Init`、`CmdPool::Init` 各自用 `std::string_view(name)` 比对 `"CDM"/"TCE"/...`，未命中落到 `ShouldNotReachHere` 分支 [src/hal/m3d/queue.cpp:103-151]。
- **错误在边界处归一**：`M3dToHalResult`、`ScpcToHalResult` 最终都转发 `ResultCast` 全表映射 [src/hal/m3d/common.h:44-181]。
- **目录即源文件集合**：`aux_source_directory` 收集 `src/hal/m3d` 下全部 `.cpp`，新增 `.cpp` 无需改清单 [src/hal/m3d/CMakeLists.txt:56-57]。

## 分支矩阵

| 分支 | 触发 | 结果 | 当前证据 |
|---|---|---|---|
| 分配尺寸非法 | `size == 0` | `errorInvalidValue`，不落池 | [src/hal/m3d/memMgr.cpp:85-87] |
| 分配溢出 | `size + alignment - 1` 回绕 | `errorOutOfMemory` | [src/hal/m3d/memMgr.cpp:88-91] |
| 超设备总量 | deviceLocal 且超过 `totalGlobalMem` | `errorOutOfMemory` | [src/hal/m3d/memMgr.cpp:92-95] |
| 指定池键不符 | `MakeKey(pPool->GetInfo()) != MakeKey(allocInfo)` | `errorInvalidValue` | [src/hal/m3d/memMgr.cpp:103-105] |
| 自动池未命中 | `Get` 结果为空或五字段不符 | 走 `CreatePoolNoLock` 建池 | [src/hal/m3d/memMgr.cpp:117-133] |
| 池对象分配失败 | `new MemoryPool` 返回空 | `errorOutOfMemory` | [src/hal/m3d/memMgr.cpp:194-196] |
| 池 `Init` 失败 | `pPool->Init` 非 success | `delete pPool` 后返回错误 | [src/hal/m3d/memMgr.cpp:178-193] |
| 释放段未登记 | `m_SegmentTracker` 查不到区间 | `LOG_WARN` 后直接返回，无错误码 | [src/hal/m3d/memoryPool.cpp:265-269] |
| 命令状态违规 | 非 `cmdBufferStateInitial` 调 `Begin` | `errorIllegalState` | [src/hal/m3d/cmdBuffer.cpp:146-148] |
| user queue 带 fence | `IsUserQueue() && submitInfo.pFence` | `errorInvalidValue`，整次提交拒绝 | [src/hal/m3d/queue.cpp:185-188] |
| 接口未实现 | `CreateEvent` 直接调用 `HAL_M3D_NOT_IMPLEMENTED()` | debug 下 `assert(false)`，仍返回 success | [src/hal/m3d/device.cpp:167-170] |
| 家族名未知 | 名字不匹配任何已知 family | `ShouldNotReachHere()` | [src/hal/m3d/queue.cpp:145-151] |

## 失败回滚的形状

创建类接口（`CreateQueue`/`CreateCmdBuffer`/`CreateMemory` 等）遵循同一模板：`new` → `Init` → 成功写输出参数、失败 `delete` [src/hal/m3d/device.cpp:77-92]。`MemMgr::Allocate` 则分三段推进：校验、选池、落池，每段都以 `res == Result::success` 作为继续的前提 [src/hal/m3d/memMgr.cpp:81-147]。

## 尚未证明

- 每个 `Create*` 失败分支是否真的把已构造的成员全部回滚（只读了模板分支，`Image`/`PerfExperiment` 等未逐个核对）。
- `Queue::Submit` 内 wait 装配成功、后续 `Submit` 失败时，已单独 `WaitQueueSemaphore` 的信号量是否会重复等待，源码未见补偿逻辑。
- `src/hal/m3d/m3d` 子模块内部如何消费这些入参，本模块不可读，属未验证范围。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/hal/m3d/memMgr.cpp`（8 处，例：[src/hal/m3d/memMgr.cpp:85-87]）
- `src/hal/m3d/queue.cpp`（3 处，例：[src/hal/m3d/queue.cpp:103-151]）
- `src/hal/m3d/device.cpp`（2 处，例：[src/hal/m3d/device.cpp:167-170]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
