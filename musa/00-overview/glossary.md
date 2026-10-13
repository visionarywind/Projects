# 术语表

- 文档目的：给出本知识库中高频术语的单一定义，并指向其源码定义处，避免同名不同义（如两个 `MemoryPool`）被混用。
- 适用范围：`source/musa` 快照（5.3.0 develop）内出现的类型、句柄、状态与构建概念。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：定义处均按行号核对，属“已确认（静态）”；语义解释为静态阅读结论，未运行验证。
- 最后更新：2026-10-07
- 前置阅读：[`../01-modules/module-registry.md`](../01-modules/module-registry.md)、[`analysis-state.md`](analysis-state.md)
- 后续阅读：[`../90-cross-module/interface-contracts.md`](../90-cross-module/interface-contracts.md)

## 结论摘要

术语按四组组织：公共 API/ABI、运行时对象、HAL 与实现层、构建与工具。最容易踩坑的三组同名项是 `MemoryPool`（core 与 halM3d 各有一个类）、`ExportTable`（静态表族与填充表族两种分发）、以及 `Hal::Result` 与 `MUresult` 两套错误码（仅靠 `HalToMuResult` 桥接）。

## 1. 公共 API 与 ABI

| 术语 | 含义 | 定义处 |
|---|---|---|
| `MUresult` | 所有 MUSA API 的返回类型，等价于 `musaError_enum` 的别名 | [src/musa_shared_include/musa.h:2625-2637] |
| `MUSA_SUCCESS` | 唯一成功取值 0；`MUSA_ERROR_INVALID_VALUE` 为 1，参数越界的默认错误 | [src/musa_shared_include/musa.h:2631-2637] |
| `MUcontext` | 不透明上下文句柄，源码即 `struct MUctx_st*` | [src/musa_shared_include/musa.h:231-241] |
| `MUgraph` | 不透明图句柄，`MUgraphNode`、`MUgraphExec` 是它的节点与可执行副本 | [src/musa_shared_include/musa.h:249-251] |
| `MU_STREAM_LEGACY` | 伪流句柄 `0x1`，传给 API 表示“老式全局同步流” | [src/musa_shared_include/musa.h:404-418] |
| `MU_STREAM_PER_THREAD` | 伪流句柄 `0x2`，解析为按线程懒创建的默认流 | [src/musa_shared_include/musa.h:410-418] |
| `MUstreamCaptureStatus` | capture 状态三态：`MU_STREAM_CAPTURE_STATUS_NONE`/`MU_STREAM_CAPTURE_STATUS_ACTIVE`/`MU_STREAM_CAPTURE_STATUS_INVALIDATED` | [src/musa_shared_include/musa.h:2474-2477] |
| `MUgraphNodeType` | 公共图节点类型枚举，尾部追加约束使 core 与 MUPTI 共用 | [src/musa_shared_include/musa.h:1964-2000] |
| `muGetExportTable` | 导出表入口：按 uuid 拿到某一方的接口表，实现体在 driver | [src/musa_shared_include/musa.h:27753-27754] |
| `ExportTable` | 每个消费方一份的函数指针集合，用 uuid 区分 | [src/musa_shared_include/export_table.h:1944-1964] |
| `Client` | 导出表消费方枚举：MUpti/Runtime/Driver/MUgdb/MUasan/Tools/… | [src/musa_shared_include/export_table.h:1954-1964] |
| `AccessorHint` | 未填充槽位的哨兵值 `0xDEADBEEF`，代表“没这个 hook”而非可调用 | [src/musa_shared_include/export_table.h:859-863] |
| `ADDMEMBER` | 导出表成员宏：成员默认填哨兵，由 `FillAccessors` 覆写 | [src/musa_shared_include/export_table.h:859-865] |
| v1 / v2 | 兼容 API 版本；常见 v1 只是把 v2 的 `size_t` 收窄为 `unsigned int` 再转发 | [src/driver/mu_memory.cpp:306-308] |
| _ptsz 后缀 | per-thread default stream 变体，以 `muapiLaunchKernel_ptsz` 为例：参数收窄 + 隐式用按线程默认流 | [src/driver/mu_module.cpp:542-546] |

## 2. 运行时对象

| 术语 | 含义 | 定义处 |
|---|---|---|
| `Platform` | 进程级单例，持有 HAL platform、可见 Device 与全局 tracker | [src/musa/core/platform.cpp:14-17] |
| `Context` | 资源归属与当前执行语义边界，持有 stream/graph/module/memory 集合 | [src/musa/core/context.h:43-54] |
| `CriticalBase` | Context 内那组带读写锁的资源集合与成对增删查接口 | [src/musa/core/context.h:185-215] |
| `MemoryTracker` | 按地址区间查找 Memory 与 offset 的全局索引 | [src/musa/core/internal_types.h:110-121] |
| `Stream` | 命令顺序、队列、同步与 capture 的抽象，实现 `IStream` | [src/musa/core/stream.h:28-34] |
| `GetPerThreadDefaultStream` | 按线程懒创建并缓存默认流的入口，不随线程退出销毁 | [src/musa/core/context.cpp:1238-1261] |
| `GetPrimaryCtx` | Device 上的默认 Context，`muDevicePrimaryCtx*` 系列操作它 | [src/musa/core/device.h:210-211] |
| capture | 把 stream 上的节点收集进图对象而不立即提交，有 ACTIVE/INVALIDATED 两段式状态 | [src/musa/core/stream.cpp:45-73] |
| `Command` | 一次可排队的 kernel/copy/memset/graph 操作基类 | [src/musa/core/command/command.h:33-42] |
| `Command::Status` | 命令生命周期：`error/completed/submitted/built/queued/created`，只在尾部追加 | [src/musa/core/command/command.h:35-42] |
| `GraphNode` | 图节点基类，所有节点类型由 `MUgraphNodeType` 决定 | [src/musa/core/node/graphNode.h:29-59] |
| `MemoryPool`（core 侧） | 位于 `src/musa/core` 的池对象，按 type 分配后走 HAL 创建 | [src/musa/core/memoryPool.h:101-108] |
| `MemoryPool`（halM3d 侧） | 位于 `src/hal/m3d` 的池实现，被 `MemMgr::FindPool` 按池键索引 | [src/hal/m3d/memMgr.h:16-24] |
| `MakeKey` | 五字段位打包的池键生成器，位移总和受 `static_assert` 约束 | [src/hal/m3d/memMgr.h:96-104] |

## 3. HAL 与实现层

| 术语 | 含义 | 定义处 |
|---|---|---|
| `Hal` | core 与具体设备实现之间的 C++ 接口层命名空间 | [src/hal/halTypes.h:16-20] |
| `Hal::Result` | HAL 错误码枚举，与 `MUresult` 同名取值但相互独立 | [src/hal/halTypes.h:327-345] |
| `HalToMuResult` | 唯一的错误码翻译点，未覆盖取值落 `MUSA_ERROR_UNKNOWN` | [src/musa/core/internal_funcs.h:22-31] |
| `IMemMgr` | core 可见的内存管理器接口，返回裸指针池对象 | [src/hal/halMemMgr.h:15-40] |
| `MemMgr` | M3D 设备侧的 `IMemMgr` 实现，按池键缓存 `MemoryPool` | [src/hal/m3d/memMgr.h:16-24] |
| `KernelState` | kernel 资源映射与更新状态对象，由 create info 构造 | [src/hal/halKernel.h:144-165] |
| `M3D` | `src/hal/m3d` 设备后端子模块，产物是静态库 `halM3d` | [src/hal/m3d/CMakeLists.txt:56-59] |
| `SplayTree` | 伸展树容器，池索引与地址索引的底层结构，含已确认的分支缺陷 | [src/util/utilSplayTree.h:59-66] |

## 4. 构建、工具与知识库约定

| 术语 | 含义 | 定义处 |
|---|---|---|
| loader | 单独构建的符号解析与转发层，表类型 `DriverFunctions` 由清单生成，不链接 util 目标 | [src/loader/loader.cpp:118-124] |
| `MUSA_LOADER_API` | 清单条目宏：既参与导出符号抽取，也参与 loader 表生成 | [src/loader/loader.cpp:118-124] |
| `MUSA_LOADER_API_MANIFEST` | CMake 里被正则抽取的清单路径，缺失即配置失败 | [cmake/export_symbols.cmake:16-35] |
| `CSV_UNSPPORTED` | CMake 分支里的拼写（README 写作 `CSV_UNSUPPORTED`），两者不一致 | [CMakeLists.txt:134-136] |
| `muInfo` | M10 工具目标与 D01 主 Demo，入口序列是 init → 查设备 → primary ctx → 打印属性 | [src/tools/muInfo.cpp:372-388] |
| 模块 ID（M01–M13） | 知识库内部稳定编号，对应顶层子目录组织，不与目录名一一绑定 | [src/CMakeLists.txt:7-15] |
| evidence status | 文档证据分级：已确认 / 推断 / 未知 / 未验证，见 [`decision-log.md`](decision-log.md) 第 5 节 | — |
| 引用格式 | `[path:start-end]` 指向源码最小区间，行号随源码更新会漂移，需定期重跑校验 | — |
| 静态分析，未验证 | 构建、运行、硬件相关结论的统一标注，表示未执行 | — |

## 5. 同名项消歧

| 同名 | 区别 |
|---|---|
| `MemoryPool` | core 侧是池对象与类型枚举；halM3d 侧是被池键索引的实现类，二者经 `IMemMgr` 相连 |
| `ExportTable` | 静态表族直接返回驱动侧对象；填充表族写入调用方缓冲区并在遇到源端 `0` 时停止 |
| `Find` / `Get` | `Find` miss 返回 `nullptr`；`Get` 返回“相等或最近”节点，从不返回空 |
| `Hal::Result` / `MUresult` | 两套独立枚举，只在 `HalToMuResult` 处翻译一次 |
| default stream | 全局 legacy 默认流（`MU_STREAM_LEGACY`）与按线程默认流（`MU_STREAM_PER_THREAD`）是两条路径 |

## 相关文档

- [`../01-modules/module-registry.md`](../01-modules/module-registry.md)
- [`decision-log.md`](decision-log.md)
- [`analysis-state.md`](analysis-state.md)
- [`../90-cross-module/interface-contracts.md`](../90-cross-module/interface-contracts.md)
- [`../99-roadmap/risk-register.md`](../99-roadmap/risk-register.md)

## 源码证据摘要

- 公共 ABI：[src/musa_shared_include/musa.h:249-251]、[src/musa_shared_include/musa.h:2625-2637]、[src/musa_shared_include/export_table.h:859-865]
- 运行时对象：[src/musa/core/context.h:43-54]、[src/musa/core/command/command.h:33-42]、[src/musa/core/internal_types.h:110-121]
- HAL 层：[src/hal/halMemMgr.h:15-40]、[src/hal/m3d/memMgr.h:16-24]、[src/musa/core/internal_funcs.h:22-31]
- 构建与工具：[cmake/export_symbols.cmake:16-35]、[src/loader/loader.cpp:118-124]、[CMakeLists.txt:134-136]

## 未解决问题

1. 部分术语（如 capture 的 green-list、`MergeLevel`）只在本表出现定义，未展开成文档，属覆盖缺口。
2. `ExportTable` 各消费方是否都按 `AccessorHint` 做哨兵判断，跨仓库部分未核对。
3. 所有定义处行号随源码更新会漂移，需在版本变更后重跑引用与内容校验。

## 下一步阅读建议

若需要术语背后的调用关系，先读 [`../90-cross-module/interface-contracts.md`](../90-cross-module/interface-contracts.md)；若关心同名项引发的真实缺陷，直接看 [`../99-roadmap/rca-splaytree-memmgr.md`](../99-roadmap/rca-splaytree-memmgr.md)。
