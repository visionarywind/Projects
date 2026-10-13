# 跨模块接口契约

- 文档目的：用源码里的检查代码（空指针检查、状态检查、断言）说明 driver→core→HAL 各层边界的前置条件、后置条件与错误语义。
- 适用范围：`source/musa` 中初始化、上下文、内存、命令提交、Graph 与导出表六条跨层边界；不含 M3D 私有子模块内部。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）。
- 证据状态：静态分析，未验证（“已确认”只表示源码里存在该检查并按此调用，不表示硬件行为或异常语义已运行验证）。
- 最后更新：2026-10-07
- 前置阅读：[`system-wiring.md`](system-wiring.md)、[`../01-modules/module-registry.md`](../01-modules/module-registry.md)
- 后续阅读：[`change-impact-map.md`](change-impact-map.md)、[`error-boundaries.md`](error-boundaries.md)

## 结论摘要

结论：各层边界的契约由三层检查组成——Driver 层做“参数 + 当前上下文”检查并把失败翻译成 `MUresult`；Core 层做“对象状态 + 集合归属 + capture 状态”检查，失败时不产生半初始化对象；HAL 层以 `Hal::Result` 返回、由 `HalToMuResult` 统一翻译。真正的契约缺口不在检查缺失，而在“未在接口上表达”的假设：`IMemMgr::FindPool` 的裸指针可空性、`MUSA_ASSERT` 在 release 下被裁剪、`AccessorHint` 哨兵与静态导出表的所有权都只存在于实现细节里。
状态：已确认（静态检查代码）+ 部分推断（并发与运行期语义）。
证据：[src/driver/mu_context.cpp:121-133]、[src/musa/core/context.cpp:631-677]、[src/musa/core/internal_funcs.h:22-31]、[src/hal/halMemMgr.h:15-40]、[src/util/utils.h:24-29]。

## 0. 阅读约定

| 记号 | 含义 |
|---|---|
| 前置条件 | 调用方必须满足的条件，否则返回文中列出的错误码 |
| 下游保证 | 调用成功后下游一定建立的状态（对象已注册、状态已推进、输出参数已回填） |
| 错误语义 | 失败时是否产生副作用、是否写输出参数 |
| 并发约束 | 该边界依赖的锁、原子量或 once 语义 |
| 证据 | 支撑上述断言的最小源码区间 |

## 1. 分层契约总表

| 上游 | 下游 | 关键接口与前置条件 | 下游保证 / 错误语义 | 并发与生命周期约束 | 证据 |
|---|---|---|---|---|---|
| Tool/App | Driver API | `muapiInit` 要求 `flags == 0` | 成功则创建 Platform；否则 `MUSA_ERROR_INVALID_VALUE` 且无副作用 | 幂等，重复调用安全 | [src/driver/mu_context.cpp:121-133] |
| Tool/App | Driver API | `muapiCtxCreate_v2` 要求 `pctx` 非空、设备 ordinal 有效、`flags` 在 `MU_CTX_FLAGS_MASK` 内 | 成功则 `*pctx` 回填、context 入 TLS 栈并注册到 Platform | TLS 栈按线程隔离 | [src/driver/mu_context.cpp:156-184] |
| Tool/App | Driver API | `muapiCtxDestroy_v2` 要求句柄非空且未被 `ValidateContext` 判为已销毁，且不是 primary context | 成功则先从 Platform 摘除再销毁 | 销毁当前线程正在用的 context 会先出栈 | [src/driver/mu_context.cpp:188-218] |
| Tool/App | Driver API | `muapiStreamCreateWithPriority` 要求当前 context 存在、`phStream` 非空、`flags` 只能是默认或非阻塞 | 成功则回填 stream 句柄 | 依赖 TLS 当前上下文 | [src/driver/mu_stream.cpp:21-42] |
| Tool/App | Driver API | `muapiMemAlloc_v2` 要求 `dptr` 非空、`bytesize` 非 0、当前 context 有效 | 成功则 `*dptr = pMemory->GetDevicePointer()`；`bytesize == 0` 时把 `*dptr` 置 0 并返回错误 | 无 | [src/driver/mu_memory.cpp:271-304] |
| Tool/App | Driver API | `muapiMemFree_v2` 要求指针能反查到内存、类型在白名单内、`offset == 0`（必须是分配基址） | 成功则走释放；graph 内存还要求分配状态为 `Enqueued` | 释放路径会先同步 | [src/driver/mu_memory.cpp:722-760] |
| Driver API | Platform | `Platform::Init` 用 `std::call_once` 保证只初始化一次，`m_InitStatus` 最后才写入 | 并发调用者看到的状态与初始化完成之间有 happens-before | `m_InitStatus` 非原子，读写都在 once 内 | [src/musa/core/platform.cpp:85-139] |
| Driver API | Platform | `Platform::GetDevice` 要求 ordinal 在 `[0, m_Devices.size())` 且该设备 `LaterInit` 成功 | 返回设备或 `nullptr` | 进程内单例 | [src/musa/core/platform.cpp:141-149] |
| Driver API | Context | `Context::InfoStream` 把 NULL/`MU_STREAM_LEGACY` 解析为默认流、`MU_STREAM_PER_THREAD` 解析为按线程默认流 | 找不到时回落到其他设备 primary context 的 stream，仍找不到返回空 | 在 `CriticalBase` 读锁内查集合 | [src/musa/core/context.cpp:558-577] |
| Context | Memory | `Context::CreateMemory` 要求 `Memory::ValidateCreateInfo` 通过，且 context 内无禁止该分配的 capture 状态 | 成功则 `new Memory` 已 `Init`、已挂进 `CriticalBase`、已 `TrackMemory`、输出参数才回填 | 注册与摘除都持 `CriticalBase` 写锁 | [src/musa/core/context.cpp:1044-1078] |
| Context | Memory | `Memory::ValidateCreateInfo` 要求分配类型与 stream capture 状态不冲突 | 冲突时返回 `MUSA_ERROR_STREAM_CAPTURE_UNSUPPORTED` 并使该 stream capture 失效 | 遍历 stream 集合持读锁 | [src/musa/core/memory.cpp:15-60] |
| Context | Stream | `Context::CreateStream` 会把 priority 夹到 `[high, low]` 区间，`Stream::Init` 失败即 `delete` 且不注册 | 成功才 `AddStream` 并回调 `IssueStreamCreatedResourceCallback` | 集合修改持写锁 | [src/musa/core/context.cpp:1198-1217] |
| API/Core | Command | `Context::ResolveDependencyAndQueueCommand` 要求 stream 属于本 context 且命令尚未入队 | 成功则跨流依赖已录制、`QueueCommand` 已把状态推进到 `queued`；阻塞模式下还要 `Wait` 返回 | 按 stream 持 `CriticalBase` 读锁读集合，入队持 `m_SubmitMtx` | [src/musa/core/context.cpp:2012-2060] |
| API/Core | Command | `Stream::QueueCommand` 要求异步容量非 0（断言）且当前线程低于容量 | 成功则 `m_LastCommand` 推进、状态 `queued`、计数 +1 | `m_AsyncCount` 原子自旋 + `m_SubmitMtx` | [src/musa/core/stream.cpp:1056-1090] |
| API/Core | Stream capture | `Stream::BeginCapture` 要求当前不是 `ACTIVE`，`Stream::CaptureNode` 要求已有 `m_CaptureGraph` | 成功才置 `ACTIVE`/写图；失败返回 `MUSA_ERROR_ILLEGAL_STATE`/`MUSA_ERROR_INVALID_VALUE` 并把序列打成失效 | 状态变更与 Platform captured 集合同步 | [src/musa/core/stream.cpp:45-73] |
| API/Core | HAL | `HalToMuResult` 把 `Hal::Result` 映射为 `MUresult`，未列出的取值落到 `MUSA_ERROR_UNKNOWN` | 返回值即 API 最终错误码 | 无状态 | [src/musa/core/internal_funcs.h:22-31] |
| Core | HAL | `Hal::IMemMgr::Allocate` 要求 size 非 0、对齐不溢出、不超过设备显存总量 | 返回 `Hal::Result`，由调用方 `HalToMuResult` 翻译 | `MemMgr` 内部持 `m_Lock` | [src/hal/m3d/memMgr.cpp:85-105] |
| Core | HAL | `Hal::IMemMgr::FindPool`/`GetPool` 返回裸指针，接口未标注可空 | 实现可能解引用空结果，见下文“契约缺口” | 持 `m_Lock` | [src/hal/halMemMgr.h:15-40] |
| M10 工具 | Driver 导出表 | `muapiGetExportTable` 的判空（`FillAccessors` 空入参即置 stop）要求两个参数都非空 | 静态表族直接返回驱动侧对象；填充族写入调用方缓冲区，遇源端 `0` 停止 | 进程内静态对象，无锁 | [src/driver/mu_entry.cpp:2146-2161] |

## 2. 契约逐条展开

### 2.1 Driver API 层：先平台、再参数、再上下文

Driver 层入口的固定模式是 `InitPlatform()` → 参数空指针/取值检查 → `TlsCtxTop()` 上下文检查 → 调 core 对象 → 只在全部成功后写输出参数。

- `muapiInit` 只接受 `flags == 0`，其余直接 `MUSA_ERROR_INVALID_VALUE`，成功分支才调用 `Musa::CreatePlatform(nullptr)` [src/driver/mu_context.cpp:121-133]。
- `muapiCtxCreate_v2` 的检查顺序是 `pctx` 空 → 设备无效 → flags 掩码不合法 → `CreateContext`；`status == MUSA_SUCCESS` 才回填 `*pctx`、`TlsCtxPush`、`Platform::LockedAddCtx` [src/driver/mu_context.cpp:156-184]。
- `muapiStreamCreateWithPriority` 先确认当前上下文（`!pContext` → `MUSA_ERROR_INVALID_CONTEXT`），再确认输出指针、再确认 flags 白名单，最后才 `pContext->CreateStream` [src/driver/mu_stream.cpp:21-42]。
- `muapiMemAlloc_v2` 在 `bytesize == 0` 时会先把 `*dptr` 置 0 再返回错误，调用方可以依赖“失败时输出参数被清零”这一后置条件 [src/driver/mu_memory.cpp:271-304]。
- `muapiMemFree_v2` 的四重校验（反查存在、类型白名单、`offset == 0`、graph 分配状态）说明“释放必须针对分配基址且类型可释放”是显式契约而非约定 [src/driver/mu_memory.cpp:722-760]。
- 上下文读取本身也有契约：`TlsCtxTop` 在 TLS 数据已销毁时返回 `nullptr`，因此上层“无 context → `MUSA_ERROR_INVALID_CONTEXT`”分支必然可达 [src/driver/internal.h:232-234]。

### 2.2 Platform/Context 层：once、范围检查与集合归属

- `Platform::Init` 用 `std::call_once` 串行化初始化，并把 `m_InitStatus` 的写入放在 lambda 最后，注释明确要求“其他线程看到成功时初始化已经完成”；同时初始化会做可见设备过滤，空结果返回 `MUSA_ERROR_NO_DEVICE` [src/musa/core/platform.cpp:85-139]。
- `Platform::GetDevice` 先做 ordinal 范围检查，再要求该设备 `LaterInit()` 成功，两条都满足才返回设备指针 [src/musa/core/platform.cpp:141-149]；`Platform::Get` 是函数内静态单例，进程级唯一 [src/musa/core/platform.cpp:14-17]。
- 跨层共享的资源集合统一收在 `Context::CriticalBase`，它继承读写锁基类并提供 `AddStream`/`FindStream`/`AddGraph` 等成对的增删查接口，读写必须走同一把锁 [src/musa/core/context.h:185-215]。
- 因此“stream 句柄是否有效”的权威判定不是指针非空，而是“在本 context 的集合里能 `FindStream` 到”，`Context::InfoStream` 正是按这个契约实现，并保留跨设备 primary context 的回退 [src/musa/core/context.cpp:558-577]。

### 2.3 Context→Memory：先校验、再创建、后注册

- `Context::CreateMemory` 把 `Memory::ValidateCreateInfo` 作为硬前置条件，之后才 `std::make_shared<Memory>` 与 `Init`；注册阶段持 `CriticalBase` 写锁做 `MapToPeers`/`AddMemory`，全部成功才 `TrackMemory` 并回填 `*ppMemory`，失败时输出参数不被写入 [src/musa/core/context.cpp:1044-1078]。
- `Memory::ValidateCreateInfo` 的核心前置条件是 capture 状态：任一 stream 处于 capture ACTIVE 且分配会触发严格校验，或 stream 已 `MU_STREAM_CAPTURE_STATUS_INVALIDATED`，都返回 `MUSA_ERROR_STREAM_CAPTURE_UNSUPPORTED` 并主动使该 stream 的 capture 失效（这是“失败也会改状态”的例外，需要调用方知道） [src/musa/core/memory.cpp:15-60]。
- 反向契约 `Context::DestroyMemory` 只有两条后置动作：从 `CriticalBase` 摘除、从 tracker 反注册，且父 context 为空时跳过摘除但仍反注册 [src/musa/core/context.cpp:1080-1088]；tracker 侧的 `TrackMemory`/`UntrackMemory`/`FindRange` 构成地址→内存对象的唯一查询契约 [src/musa/core/internal_types.h:110-121]。
- HAL 侧对应前置条件是分配参数的合法性：`MemMgr` 拒绝 size 为 0、拒绝对齐加法溢出、拒绝超过设备显存总量，全部以 `Hal::Result` 返回而非断言 [src/hal/m3d/memMgr.cpp:85-105]；带 `SubAllocatable` 的请求才进内存池，否则直连 `Hal::CreateMemory` [src/musa/core/memory.cpp:626-640]。

### 2.4 Context/Stream→Command：状态机与依赖契约

- `Context::ResolveDependencyAndQueueCommand` 是命令进入队列的唯一关口：按目标流类型录制跨流依赖、检查屏障流未完成命令、合并调用方携带的当前依赖，然后才 `QueueCommand`；只有 `blocking || pfmSerialMode` 时才追加 `Wait` [src/musa/core/context.cpp:2012-2060]。
- `Stream::QueueCommand` 的前置条件是异步容量非 0，由 `MUSA_ASSERT(asyncCapacity != 0 && ...)` 断言；成功后置条件是状态 `Command::Status::queued`、`m_LastCommand` 更新、异步计数 +1 [src/musa/core/stream.cpp:1056-1090]。
- 命令状态枚举本身即跨模块契约：`created → queued → built → submitted → completed` 与 `error`，注释要求新增值只能加到底部，因为 MUPTI 侧复用该枚举 [src/musa/core/command/command.h:33-42]。
- capture 分支的前置条件独立成一套：`Stream::BeginCapture` 要求当前非 `ACTIVE`，否则 `MUSA_ERROR_ILLEGAL_STATE` [src/musa/core/stream.cpp:45-73]；`Stream::CaptureNode` 要求 `m_CaptureGraph` 存在，否则 `MUSA_ERROR_INVALID_VALUE`，且任何失败都会把 capture 序列置为失效 [src/musa/core/stream.cpp:673-694]。
- 上层 `Context::GeneralLaunchKernel` 因此把“句柄归属”和“capture 状态”分成两段检查：`ctx` 为空 → `MUSA_ERROR_INVALID_CONTEXT`；stream 句柄解析失败 → `MUSA_ERROR_STREAM_DETACHED` 或 `MUSA_ERROR_INVALID_HANDLE`；stream 的父 context 与调用 context 不一致且不在 capture/green 场景 → `MUSA_ERROR_INVALID_HANDLE`；最后按 capture 状态选择入图或真实下发 [src/musa/core/context.cpp:631-677]。
- `DispatchCommand::Submit` 在 release 下仍保证 HAL 侧的收尾顺序：先 `ResolveSubmitSignal`、结束采样，再 `pHalCmdBuffer->End()` 并检查返回值 [src/musa/core/command/dispatchCommand.cpp:302-315]。

### 2.5 Core→HAL：错误码翻译与断言边界

- 唯一的错误码桥是 `HalToMuResult`：`success`/`snapshot` → `MUSA_SUCCESS`，其余按枚举一一映射，未覆盖的取值退化为 `MUSA_ERROR_UNKNOWN`，所以新增 `Hal::Result` 必须同步补这张表 [src/musa/core/internal_funcs.h:22-31]。
- HAL 侧的错误契约以 `Hal::Result` 枚举表达，注释写明“现在只是与 `MUresult` 同名取值”，暗示两套枚举可能脱钩 [src/hal/halTypes.h:325-345]。
- `Stream::Init` 创建底层队列/命令池时把 `Hal::Result` 逐次翻译并短路，说明“HAL 对象创建失败必须向上收敛为 `MUresult`”是该边界的后置要求 [src/musa/core/stream.cpp:930-960]。
- 断言不构成契约：`MUSA_ASSERT` 在 `NDEBUG` 下展开为 `(void)(EXPR)`，因此 `MemMgr::Free` 中“池存在且键相等”的断言在 release 构建不提供任何保护 [src/hal/m3d/memMgr.cpp:149-161]、[src/util/utils.h:24-29]。
- 池键由 `MakeKey` 五字段位打包，且位移总和有 `static_assert` 兜底，意味着“新增 `MemoryPoolInfo` 字段必须同时改掩码与位移”是编译期契约 [src/hal/m3d/memMgr.h:96-104]。

### 2.6 契约缺口（接口未表达的假设）

| 缺口 | 现状 | 证据 | 后果 |
|---|---|---|---|
| `FindPool`/`GetPool` 可空性未标注 | `IMemMgr` 用裸指针返回，无空值约定、无错误码出口 | [src/hal/halMemMgr.h:15-40] | 调用方有理由认为必非空；实现里 `Find` miss 会返回空 |
| 断言即检查 | 释放路径的安全条件只写成 `MUSA_ASSERT` | [src/hal/m3d/memMgr.cpp:149-161] | release 构建失去这层检查 |
| accessor 哨兵语义 | 成员初值为 `AccessorHint`，是否可用由调用方自己比对 | [src/musa_shared_include/export_table.h:859-860]、[src/driver/muasan/tracepoints.h:11-14] | 漏填与不支持无法区分 |
| 导出表所有权 | 静态表族返回驱动侧对象，填充族写调用方缓冲区，接口未区分 | [src/driver/mu_entry.cpp:2146-2161] | 调用方误释放或重复填充会破坏驱动状态 |
| 填充中止条件 | 源端出现 `0` 即停止，剩余槽位保持哨兵值 | [src/driver/mu_entry.cpp:2146-2156] | “未填”被静默保留 |

## 3. 检查代码索引

| 检查类型 | 代表位置 | 覆盖边界 |
|---|---|---|
| 空指针检查 | `muapiCtxCreate_v2` 判 `pctx`/`pDevice` [src/driver/mu_context.cpp:156-184] | App→Driver |
| 空指针检查 | `FillAccessors` 判空且 `muapiGetExportTable` 判两个入参 [src/driver/mu_entry.cpp:2146-2161] | 工具→导出表 |
| 取值范围检查 | `muapiInit` 判 `flags != 0` [src/driver/mu_context.cpp:121-133] | App→Driver |
| 对象归属检查 | `Context::InfoStream` 在集合里查 stream [src/musa/core/context.cpp:558-577] | Driver→Context |
| 状态检查 | `Stream::BeginCapture` 判 capture 是否已 `ACTIVE` [src/musa/core/stream.cpp:45-73] | Stream 状态机 |
| 状态检查 | `Context::GeneralLaunchKernel` 按 capture 状态分支 [src/musa/core/context.cpp:631-677] | API→Stream/Graph |
| 断言 | `Stream::QueueCommand` 断言异步容量非 0 [src/musa/core/stream.cpp:1056-1090] | 提交热路径 |
| 断言（release 失效） | `MemMgr::Free` 断言池与键 [src/hal/m3d/memMgr.cpp:149-161] | Core→HAL |
| 编译期断言 | `MakeKey` 位移总和 `static_assert` [src/hal/m3d/memMgr.h:96-104] | 池键布局 |
| 错误码翻译 | `HalToMuResult` 的 switch [src/musa/core/internal_funcs.h:22-31] | Core↔HAL |

## 4. 状态说明

表中标“已确认”的行表示：源码中确有对应检查、且调用顺序与描述一致；标“部分确认”的行表示检查代码已确认，但其并发语义（锁的覆盖范围、`m_InitStatus` 之外的无锁读）与运行期行为仅靠静态阅读无法证明。全部内容均为静态分析，未验证。

## 相关文档

- [`system-wiring.md`](system-wiring.md)
- [`change-impact-map.md`](change-impact-map.md)
- [`error-boundaries.md`](error-boundaries.md)
- [`../01-modules/M04-context-lifecycle/interfaces.md`](../01-modules/M04-context-lifecycle/interfaces.md)
- [`../99-roadmap/risk-register.md`](../99-roadmap/risk-register.md)

## 源码证据摘要

- Driver 参数与上下文检查：[src/driver/mu_context.cpp:121-133]、[src/driver/mu_stream.cpp:21-42]、[src/driver/mu_memory.cpp:271-304]、[src/driver/mu_memory.cpp:722-760]
- Platform/Context 归属检查：[src/musa/core/platform.cpp:85-139]、[src/musa/core/context.cpp:558-577]、[src/musa/core/context.h:185-215]
- Memory 契约：[src/musa/core/context.cpp:1044-1078]、[src/musa/core/memory.cpp:15-60]、[src/hal/m3d/memMgr.cpp:85-105]
- 提交与状态机：[src/musa/core/context.cpp:2012-2060]、[src/musa/core/stream.cpp:1056-1090]、[src/musa/core/command/command.h:33-42]
- HAL 边界与断言：[src/musa/core/internal_funcs.h:22-31]、[src/hal/halTypes.h:325-345]、[src/util/utils.h:24-29]
- 导出表：[src/driver/mu_entry.cpp:2146-2161]、[src/musa_shared_include/export_table.h:859-860]

## 未解决问题

1. 锁的覆盖范围（`CriticalBase` 读写锁、`m_SubmitMtx`、`MemMgr::m_Lock`）只确认了使用点，未确认是否所有跨线程读写都持锁，属推断。
2. `Hal::Result` 到 `MUresult` 的映射只在 `HalToMuResult` 一处核对，HAL 私有子模块返回的错误码集合未逐一验证。
3. 所有错误码分支均为静态阅读结论，未构造非法输入实测，标注“静态分析，未验证”。

## 下一步阅读建议

按 [`change-impact-map.md`](change-impact-map.md) 选一个改动点，用本页的前置/后置条件设计最小回归；对 2.6 的四条契约缺口，先读 [`../99-roadmap/rca-splaytree-memmgr.md`](../99-roadmap/rca-splaytree-memmgr.md) 理解它们如何在真实缺陷中暴露。
