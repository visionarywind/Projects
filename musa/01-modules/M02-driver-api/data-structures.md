# Driver API 导出层：数据结构与所有权

- 文档目的：driver api：核心数据结构、字段含义与持有关系。
- 适用范围：M02 模块的《Driver API 导出层：数据结构与所有权》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


## 结构关系

```text
Public handle (MUcontext/MUdevice/MUstream/...) -> C++ 对象 -> optional shared_ptr -> HAL 对象
       转换路径：
       Musa::IntrusiveCast<T>(handle)   句柄即对象指针时用  [src/driver/mu_context.cpp:24]
       Musa::ICast<T>(handle)           按接口 downcast      [src/driver/mu_context.cpp:50]
       reinterpret_cast<T*>(handle)     纯类型抹除的对象      [src/driver/mu_mempool.cpp:132]
```

- `MUcontext`/`MUstream` 等句柄在 Driver 层大多即 C++ 对象指针：`*pctx = pContext; *phStream = pStream;` 直接写回接口指针。[src/driver/mu_context.cpp:172-175] [src/driver/mu_stream.cpp:33-37]
- `MUdevice` 是序号而非指针：`*device = pDevice->GetId();`；`Platform::GetDevice`/`GetIDeviceView` 用序号取回设备对象。[src/driver/mu_device.cpp:14-37] [src/musa/core/platform.cpp:141-149]
- `MUmemoryPool` 以 `reinterpret_cast<Musa::MemoryPool*>` 转换，校验走 `Platform::ValidateMemoryPool`，失败路径 `delete pMemoryPool`。[src/driver/mu_mempool.cpp:108-117]

## 导出的访问表

`muapiGetExportTable` 中所有 accessor 结构以 `ADDMEMBER(type, name)` 声明，未填槽位初始为 `AccessorHint(0xDEADBEEF)`；`FillAccessors` 只回填值为 `AccessorHint` 的槽位。[src/musa_shared_include/export_table.h:859-860] [src/driver/mu_entry.cpp:2146-2156]

`Driver::ExportTable` 聚合 17 个分组：general、device、deprecated、gfxInterop、graph、context、event、memory、stream、module、occupancy、texture、external、library、log、profiler、greenCtx，末尾 `reserved`。[src/musa_shared_include/export_table.h:1640-1659]

## 所有权与登记

- `Context`：创建成功后在 `muapiCtxCreate_v2` 内完成 `TlsCtxPush` 与 `Platform::LockedAddCtx`（登记进 `ctxs()` 集合并分配 SeqID）；销毁前 `TlsCtxPop`、`LockedRemoveCtx`，再 `DestroyContext`。[src/driver/mu_context.cpp:169-175] [src/driver/mu_context.cpp:199-212] [src/musa/core/platform.cpp:430-454]
- `Stream`：常规 stream 由 Context 持有；detached stream 由 Driver 直接 `delete detachedStream`，若删除后该 parent ctx 无 stream 引用则连 parent ctx 一并删除。[src/driver/mu_stream.cpp:177-194]
- `Module`：`CreateModule` 成功但 `LoadFatBinary` 失败时回滚 `DestroyModule`，不对外暴露半初始化模块。[src/driver/mu_module.cpp:132-141]
- `Memory`：通过 `MemoryTracker` 的 `FindRange` 按设备地址查回 `IMemory*`，virtual memory 再经 `GetPhysMemory` 落到物理内存（`GetMemoryByDevicePointer` 是另一条地址反查路径）。[src/driver/mu_memory.cpp:117-126] [src/driver/mu_memory.cpp:728-738]
- `Command`：以 `std::shared_ptr<Command>` 在 `QueueCommand` 中 `m_CommandList.push_back(std::move(command))` 转移，保证异步完成前对象存活；profiler 侧从 `GetInflightList`/`GetWaitingList` 读取同一批对象。[src/musa/core/stream.cpp:1052-1113] [src/driver/mu_entry.cpp:586-650]
- profiler 备份：`g_memory_backups` 以 `MUcontext` 为键、`MemoryBackupEntry` 持有 `std::mutex` 与 `backups` 映射，备份/恢复走 `SaveRestoreDeviceMemory`。[src/driver/mu_entry.cpp:38-59] [src/driver/mu_entry.cpp:981-1015]

具体模块的字段布局以对应头文件为准；本文不臆测未读字段。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/driver/mu_context.cpp`（5 处，例：[src/driver/mu_context.cpp:24]）
- `src/driver/mu_entry.cpp`（4 处，例：[src/driver/mu_entry.cpp:2146-2156]）
- `src/driver/mu_mempool.cpp`（2 处，例：[src/driver/mu_mempool.cpp:132]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
