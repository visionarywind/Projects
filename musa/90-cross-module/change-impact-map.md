# 变更影响地图

- 文档目的：给出“改一个核心接口或数据结构会波及哪些模块”的证据化地图，用真实调用点引用替代目录级猜测。
- 适用范围：`source/musa` 中 Context 创建、ExportTable、Memory/MemoryPool、Stream/Command、Graph node、错误映射与构建系统七类改动。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）。
- 证据状态：静态分析，未验证（调用点全部来自静态检索，未在授权硬件上执行回归）。
- 最后更新：2026-10-07
- 前置阅读：[`../01-modules/module-registry.md`](../01-modules/module-registry.md)、[`system-wiring.md`](system-wiring.md)
- 后续阅读：[`interface-contracts.md`](interface-contracts.md)、[`../99-roadmap/risk-register.md`](../99-roadmap/risk-register.md)

## 结论摘要

结论：MUSA Driver 的改动影响面不由目录决定，而由四条传导链决定：(1) 上下文/句柄解析链——driver 层用 `TlsCtxTop` 取当前上下文再解 `MUstream` 等句柄；(2) 提交链——所有 `Cmd*` 操作汇入 `ResolveDependencyAndQueueCommand` 再进 `Stream::QueueCommand`；(3) 类型与状态定义链——`MUgraphNodeType`、`MemoryCreateInfo`、`ExportTable` accessor、`Command::Status` 等枚举被 driver/core/HAL/工具四方共用；(4) 构建清单链——`aux_source_directory` glob 与 `MUSA_LOADER_API` manifest 决定新文件/新 API 是否真的被编译与导出。改动落在定义点时必须同时改四段，落在调用点时只需改该段并回归其上游。
状态：已确认（静态调用点）。
证据：[src/driver/internal.h:232-234]、[src/musa/core/context.cpp:2012-2060]、[src/musa_shared_include/musa.h:1964-2000]、[src/loader/loader_api_list.inc:2-568]。

## 0. 使用方法

1. 先判断改动属于“定义点”还是“调用点”：定义点（枚举、结构体、导出表、构建清单）需要全链同步修改，调用点（某个 `Cmd*` 或某个 API 分支）通常只影响本段及错误码。
2. 沿下表“主要波及”列给出的符号做静态检索（`rg -n <symbol> src`），确认新增/删除的调用点。
3. 对每条改动，先在本页勾选回归重点，再按 [`interface-contracts.md`](interface-contracts.md) 的前置/后置条件设计用例；所有结论在授权硬件验证前一律标注“静态分析，未验证”。

## 1. 影响面总表

| 改动点 | 定义/入口证据 | 主要波及（静态调用点方向） | 回归重点 |
|---|---|---|---|
| Context 创建/销毁 | `muapiCtxCreate_v2` [src/driver/mu_context.cpp:156-184] | Platform 注册、TLS 栈、全部依赖当前上下文的资源 API | pctx/flags/设备校验、primary context 保护、销毁后的句柄 |
| 当前上下文解析 | `TlsCtxTop` [src/driver/internal.h:232-234] | `src/driver` 各资源入口（`mu_memory`、`mu_graph`、`mu_stream` 等） | 无 context 时必须返回 `MUSA_ERROR_INVALID_CONTEXT` |
| Stream 句柄解析 | `Context::InfoStream` [src/musa/core/context.cpp:558-577] | `GeneralLaunchKernel` 等所有按句柄取 stream 的 core 入口 | NULL/`MU_STREAM_LEGACY`/`MU_STREAM_PER_THREAD` 哨兵语义 |
| Memory 创建与校验 | `Context::CreateMemory` [src/musa/core/context.cpp:1044-1078] | Context 资源集合、MemoryTracker、peer 映射、HAL 分配 | capture 期分配、peer access、tracker 注册与反注册 |
| 内存类型校验 | `Memory::ValidateCreateInfo` [src/musa/core/memory.cpp:15-60] | 每个 memory 类型分支、stream capture 状态 | 非法类型、capture ACTIVE/INVALIDATED 冲突 |
| 内存池复合键 | `MakeKey` [src/hal/m3d/memMgr.h:96-104] | MemMgr 池查找、自动建池、`MemoryPoolInfo` 字段 | 位宽静态断言、键冲突导致错池 |
| 命令依赖与入队 | `Context::ResolveDependencyAndQueueCommand` [src/musa/core/context.cpp:2012-2060] | 全部 `Stream::Cmd*`、默认流/屏障流依赖规则 | 依赖方向、阻塞等待、profiler 串行分支 |
| 命令入队实现 | `Stream::QueueCommand` [src/musa/core/stream.cpp:1056-1090] | 异步容量自旋、提交锁、命令状态机 | 容量断言、状态从 queued 起步的下游假设 |
| Kernel 启动 | `Context::GeneralLaunchKernel` [src/musa/core/context.cpp:631-677] | node 创建、capture 分支、`CmdLaunchKernel` | capture 三分支、stream 归属校验、detached stream |
| Graph 节点类型 | `MUgraphNodeType` [src/musa_shared_include/musa.h:1964-2000] | 节点类、工厂、`ToString`、MUPTI 枚举 | 枚举只允许尾部追加（工具侧共用） |
| 导出表 accessor | `AccessorHint` [src/musa_shared_include/export_table.h:859-860] | 8 组导出表、填充循环、M10 消费方 | 哨兵值一致性、漏填与“不支持”不可区分 |
| 导出表分发 | `FillAccessors` [src/driver/mu_entry.cpp:2146-2161] | `muapiGetExportTable` 的 9 个 `Client` 分支 | 静态表 vs 填充表两种所有权语义 |
| Loader 导出清单 | `MUSA_LOADER_API` [src/loader/loader_api_list.inc:2-568] | 函数指针表、符号解析、`extern "C"` 转发、ELF/PE 导出 | 漏一条即静默少一个导出符号 |
| HAL 错误映射 | `HalToMuResult` [src/musa/core/internal_funcs.h:22-31] | core 各处 HAL 调用的 `MUresult` 转换 | 新增 `Hal::Result` 是否有对应 `MUresult` |
| Splay 树查找 | `Search` [src/util/utilSplayTree.h:216-226] | MemMgr 池查找（`Find`/`Get` 两条路径） | 已知缺陷：分支方向反转，见风险登记 |
| 构建源码清单 | `aux_source_directory` [src/util/CMakeLists.txt:1-4] | util、halM3d、tools 三处 glob 目标 | 新文件是否被纳入、无用文件被误编译 |

## 2. 分主线展开

### 2.1 上下文与句柄主线

- 入口 `muapiCtxCreate_v2` 依次校验 `pctx`、`Platform::GetDevice(dev)`、`flags` 掩码，成功后 `CreateContext`、`TlsCtxPush` 并把 context 注册进 Platform；任何一步失败都只返回 `MUresult`，不产生半初始化对象 [src/driver/mu_context.cpp:156-184]。
- 销毁侧 `muapiCtxDestroy_v2` 先 `nullptr` 检查，再 `ValidateContext` 拒绝已销毁 context，且不允许销毁 primary context，最后才从 Platform 摘除并 `DestroyContext` [src/driver/mu_context.cpp:188-218]。
- 当前上下文读取集中在 `TlsCtxTop`，它在 TLS 数据已销毁时返回 `nullptr` 而不是崩溃 [src/driver/internal.h:232-234]。静态计数显示 `TlsCtxTop()` 在 `src/driver` 出现 898 次（`rg -o` 计数，未逐处核对语义），其中 `mu_memory.cpp` 91 次、`mu_graph.cpp` 89 次、`mu_stream.cpp` 29 次——因此“改上下文获取方式”是全局改动。
- stream 句柄解析集中在 `Context::InfoStream`：NULL 或 `MU_STREAM_LEGACY` 回落默认流，`MU_STREAM_PER_THREAD` 走按线程缓存的默认流，其余句柄先在本 context 的 `CriticalBase` 集合里 `FindStream`，失败后还会跨设备查找 primary context 的 stream [src/musa/core/context.cpp:558-577]。
- 按线程默认流由 `Context::GetPerThreadDefaultStream` 用 `static thread_local` 映射缓存，并在找到但已被 context 摘除时重建 [src/musa/core/context.cpp:1238-1261]；两个哨兵常量定义在公共头 [src/musa_shared_include/musa.h:406-418]。
- 影响：改哨兵语义会同时改变 `InfoStream`、`GeneralLaunchKernel` 的句柄判断和所有使用 `MU_STREAM_PER_THREAD` 的 API。

### 2.2 ExportTable 主线

- `ADDMEMBER` 宏把每个 accessor 成员初始化为 `AccessorHint = 0xDEADBEEF`，全文件共 652 处（静态计数），这是“未填充”占位而非可调用标记 [src/musa_shared_include/export_table.h:859-860]。
- 公共头里定义了 8 个 `struct ExportTable`（静态计数），消费方通过 `enum class Client` 的 magic number 选择其中之一 [src/musa_shared_include/export_table.h:1954-1963]。
- 分发存在两种互不兼容的所有权语义：`Client::MUpti`/`Runtime`/`MUgdb`/`MUasan`/`Photon` 直接返回驱动侧静态表指针 [src/driver/mu_entry.cpp:2164-2171]；`Client::Driver`/`Tools`/`Injection`/`ToolsMuapi` 则把函数地址写进调用方提供的缓冲区，`FillAccessors` 遇到源端 `0` 即停止，造成后续槽位静默不填 [src/driver/mu_entry.cpp:2146-2161]。
- 未知 `Client` 走 default 分支静默返回 `muptiDriverTable`，源码自带 `FIXME`，调用方无法区分“不支持”与“填错” [src/driver/mu_entry.cpp:2300-2304]。
- 消费方必须自己比对哨兵：`IsPfnValid` 同时排除 `AccessorHint` 与 `nullptr` 才认为 hook 可用 [src/driver/muasan/tracepoints.h:11-14]。
- 新增一个导出 API 的连锁改动：公共头原型 [src/musa_shared_include/musa.h:27753] → accessor 结构体成员 → `mu_entry.cpp` 源数组与 `FillAccessors` 调用 → loader manifest [src/loader/loader_api_list.inc:2-568] → 导出符号脚本按 manifest 正则抽取行生成 ELF/PE 导出表 [cmake/export_symbols.cmake:21-33]。漏改 manifest 不会报错，只会少一个符号。
- loader 侧同一份清单被包含三次：声明函数指针表、逐个解析符号、生成 `extern "C"` 转发包装 [src/loader/loader.cpp:118-124]、[src/loader/loader.cpp:146-154]、[src/loader/loader.cpp:209-219]。

### 2.3 Memory 与内存池主线

- `Context::CreateMemory` 的顺序是 `ValidateCreateInfo` → `new Memory` → `Init` → 持写锁做 `MapToPeers`/`AddMemory` → 注册进 `MemoryTracker` → 回填 `*ppMemory`；任一步失败都不写输出参数 [src/musa/core/context.cpp:1044-1078]。
- `Memory::ValidateCreateInfo` 除按 `createInfo.type` 分派外，还会遍历 context 内所有 stream：任一 stream 处于 capture ACTIVE 且本分配会触发 `MU_STREAM_CAPTURE_STATUS_INVALIDATED` 时直接返回 `MUSA_ERROR_STREAM_CAPTURE_UNSUPPORTED` [src/musa/core/memory.cpp:15-60]。
- HAL 分配分两条路：带 `SubAllocatable` 的走 `GetMemMgr()->Allocate` 进内存池，否则直接 `Hal::CreateMemory` [src/musa/core/memory.cpp:626-640]。
- 池选择用五字段位打包键 `MakeKey`（property/viewCapability/type/heap/numaId），其位移总和受 `static_assert` 约束，新增字段必须同时调整掩码与位移 [src/hal/m3d/memMgr.h:96-104]。
- 自动选池在 `MemMgr::Allocate` 内用 `Get` 拿“最近节点”再逐字段比对兜底，不匹配就新建池 [src/hal/m3d/memMgr.cpp:106-133]；释放侧 `MemMgr::Free` 用 `Get` + `MUSA_ASSERT`，而 `MUSA_ASSERT` 在 `NDEBUG` 下退化为空表达式，release 构建没有这层保护 [src/hal/m3d/memMgr.cpp:149-161]、[src/util/utils.h:24-29]。
- 释放路径 `muapiMemFree_v2` 用 tracker 反查内存，依次校验指针存在、类型白名单、必须是分配基址（`offset == 0`）、graph 分配状态，任何一项不满足都返回 `MUSA_ERROR_INVALID_VALUE` 或 `MUSA_ERROR_INVALID_CONTEXT` [src/driver/mu_memory.cpp:722-760]。
- 反注册由 `Context::DestroyMemory` 完成：先从 `CriticalBase` 摘除，再 `UntrackMemory` [src/musa/core/context.cpp:1080-1088]，tracker 接口本身提供 `TrackMemory`/`UntrackMemory`/`FindRange` 三个入口 [src/musa/core/internal_types.h:110-121]。
- 析构 `Memory::~Memory` 根据是否挂在 pool 上选择 `GetMemMgr()->Free` 或 pool 自己的 `Hal()->Free`，改析构顺序会同时改变这两条路径 [src/musa/core/memory.cpp:485-500]。
- 影响：改 `MemoryCreateInfo` 字段会同时波及 `ValidateCreateInfo` 的 switch、`Context::CreateMemory` 的注册逻辑、`muapiMemFree_v2` 的类型白名单、`MakeKey` 的位打包以及 graph 内存节点。

### 2.4 Stream/Command 提交主线

- 所有命令最终汇入 `Context::ResolveDependencyAndQueueCommand`：它按“目标是不是默认流/屏障流/普通流”三种情形录制跨流依赖，检查屏障流最后一条命令的状态是否仍大于 `completed`，合并当前依赖后调用 `QueueCommand`，仅在阻塞或 profiler 串行模式下再 `Wait` [src/musa/core/context.cpp:2012-2060]。
- `Stream::QueueCommand` 是入队的唯一实现：先断言异步容量非 0，再自旋等待 `m_AsyncCount` 降到容量以下，在 `m_SubmitMtx` 内挂前置依赖、推进 `m_LastCommand` 并把状态置为 `Command::Status::queued` [src/musa/core/stream.cpp:1056-1090]。
- 命令状态是外部可观测契约：`created → queued → built → submitted → completed`，另有 `error = -1`，枚举新增值只能尾部追加，因为 MUPTI 侧共用该枚举 [src/musa/core/command/command.h:33-42]；`Command::Type` 同样注明“新类型必须加到底部” [src/musa/core/command/command.h:55-70]。
- capture 与提交是同一入口的两条分支：`Context::GeneralLaunchKernel` 先校验 ctx 与 stream 归属，再按 capture 状态选择 `CaptureNode` 或 `CmdLaunchKernel`，命中 `MU_STREAM_CAPTURE_STATUS_INVALIDATED` 直接返回错误 [src/musa/core/context.cpp:631-677]。
- `Stream::CaptureNode` 在没有 `m_CaptureGraph` 时返回 `MUSA_ERROR_INVALID_VALUE`，任何失败都会把 capture 状态打成 `MU_STREAM_CAPTURE_STATUS_INVALIDATED`，使同一 capture 序列后续全部失败 [src/musa/core/stream.cpp:673-694]。
- `Stream::BeginCapture` 在已是 `ACTIVE` 时返回 `MUSA_ERROR_ILLEGAL_STATE`，成功才置 `ACTIVE` 并把 stream 注册进 Platform 的 captured 集合 [src/musa/core/stream.cpp:45-73]。
- 影响：改依赖规则或状态机要同时回归默认流语义、屏障流、capture 序列、异步容量与 `blocking` 参数；静态计数显示 `src/musa/core` 内有 359 处 `Cmd*(` 调用（`rg -o` 计数）。

### 2.5 Graph 节点主线

- 节点基类 `GraphNode` 持有父 context、类型、父图、correlation id、启用标志、launch stream 与所属 `GraphExec`，新增节点类型必须先在该类的状态模型中落位 [src/musa/core/node/graphNode.h:29-59]。
- 节点工厂在 `Context`：`CreateKernelNode` 只是 `CreateKernelNodeOnStream` 的薄包装，后者用 `GraphKernelNode::Create` 创建节点、判空（失败返回 `MUSA_ERROR_OUT_OF_MEMORY`）并 `UpdateParams` [src/musa/core/context.cpp:2230-2259]。
- 节点注册进图时 `Graph::AddGraphNode` 会针对条件节点做特殊绑定校验，绑定缺失或重复都返回 `MUSA_ERROR_INVALID_VALUE` [src/musa/core/graph.cpp:245-261]。
- 类型枚举 `MUgraphNodeType` 覆盖 kernel/memcpy/memset/host/child graph/event/semaphore/mem alloc/mem free/conditional/atomic 等 18 个值，是公共 ABI，只能尾部追加 [src/musa_shared_include/musa.h:1964-2000]；core 侧 `ToString(MUgraphNodeType)` 另有一份手工 switch，加类型时同样要补 [src/musa/core/logging.h:580-592]。
- 实例化后的参数更新走 `GraphExec::SetKernelNodeParams` 等一组按节点类型区分的 setter，改参数布局要同步改这些 setter 与节点自身的 `UpdateParams` [src/musa/core/graph/graph1/graphExec.cpp:73-120]。
- capture 期的内存节点 `GraphMemoryAllocNode::Init` 从 graph 专属池取虚拟内存，池不存在直接返回 `MUSA_ERROR_OUT_OF_MEMORY`，未给访问描述时默认对所有可见设备开放访问 [src/musa/core/node/graphMemoryAllocNode.cpp:15-48]。
- 影响：新增一种 graph node 至少要动公共枚举、节点类、`Context::Create*Node` 工厂、capture/提交分支、`GraphExec` 的 setter、日志 `ToString` 与 MUPTI 消费方。

### 2.6 错误映射主线

- core 与 HAL 的错误码是两套枚举：HAL 侧 `Hal::Result` 注释明确“现在只是与 `MUresult` 同名取值” [src/hal/halTypes.h:325-345]，core 侧 `MUresult` 以 `MUSA_SUCCESS = 0` 开头、`MUSA_ERROR_UNKNOWN = 999` 结尾 [src/musa_shared_include/musa.h:2625-2637]。
- 唯一的桥是 `HalToMuResult` 的 switch；静态计数显示它在 `src/musa/core` 被调用 198 次（`rg -o` 计数），新增 `Hal::Result` 而不改该 switch 会落到默认的 `MUSA_ERROR_UNKNOWN` [src/musa/core/internal_funcs.h:22-31]。
- 影响：改任何 HAL 返回码都会改变上层 API 可见的 `MUresult`，进而改变 Driver API 的错误兼容性；这类改动必须同时核对公共头中的错误码文档。

### 2.7 构建与安装主线

- `src/util`、`src/hal/m3d`、`src/tools` 都用 glob 收集源文件：`aux_source_directory` 直接把目录下所有 `.cpp` 纳入静态库 [src/util/CMakeLists.txt:1-4]、[src/hal/m3d/CMakeLists.txt:56-59]，tools 侧 `GLOB_RECURSE` 拿到的每个 `.cpp` 都会生成一个可执行目标并安装到 `CMAKE_INSTALL_BINDIR` [src/tools/CMakeLists.txt:10-18]。
- CSV 开关存在拼写差异：README 写 `CSV_UNSUPPORTED`，CMake 判断的是 `CSV_UNSPPORTED`，且其分支会调用不在快照内的 `tool/code_reason.py` [README.md:34-35]、[CMakeLists.txt:134-136]。
- 安装脚本按架构/发行版推导 `LIB_INSTALL_DIR`（如 `/usr/lib/x86_64-linux-gnu`、`/usr/lib64`），属于写系统目录的操作，未授权不得执行 [install.sh:51-70]。
- 影响：新增源文件不改任何清单也会被 glob 编进库（误纳入），新增导出 API 不改 loader manifest 则不会被转发（漏导出），两者都不产生编译错误。

## 3. 回归检查清单（静态建议，未执行）

| 改动类型 | 静态检查 | 运行期检查（未验证） |
|---|---|---|
| 新增/修改 Driver API | 核对 `musa.h` 原型、accessor 结构、`mu_entry.cpp` 源数组、loader manifest 四处是否同步 | 导出符号表 diff、`muGetExportTable` 取表后逐字段比对哨兵 |
| 改 Context/句柄语义 | 检索 `TlsCtxTop`、`InfoStream` 的全部调用点 | context 创建/销毁/primary retain 往返、销毁后句柄再用 |
| 改 Memory 类型或字段 | 检索 `ValidateCreateInfo`、`CreateMemory`、`muapiMemFree_v2`、`MakeKey` | 分配-释放-再分配、capture 期分配、peer 访问 |
| 改 Stream 提交或状态 | 检索 `ResolveDependencyAndQueueCommand`、`QueueCommand` 与 `Command::Status` 读取方 | 默认流/非阻塞流依赖、异步容量打满、query/synchronize |
| 改 Graph 节点 | 检索 `MUgraphNodeType` 的所有 switch 与 `Create*Node` 工厂 | capture→instantiate→launch→update 全链 |
| 改 HAL 错误码 | 核对 `HalToMuResult` 的 switch 是否覆盖新枚举 | 上层 API 错误码是否仍与公共头文档一致 |
| 改构建清单 | 检查三处 glob、`export_symbols.cmake` 正则、`MUSA_BUILD_UT` 分支 | 干净构建、安装产物与导出符号清单 |

## 4. 统计口径

本文出现的计数（898 处 `TlsCtxTop()`、652 处 `ADDMEMBER`、8 个 `struct ExportTable`、567 条 `MUSA_LOADER_API`、359 处 `Cmd*(`、198 次 `HalToMuResult`）均来自 `rg -o` 对 `source/musa` 的静态计数，口径是“出现次数”，未逐处核对语义，也未在运行期验证；修改源码后需重算。

## 相关文档

- [`system-wiring.md`](system-wiring.md)
- [`interface-contracts.md`](interface-contracts.md)
- [`cross-module-call-chains.md`](cross-module-call-chains.md)
- [`../01-modules/module-registry.md`](../01-modules/module-registry.md)
- [`../99-roadmap/risk-register.md`](../99-roadmap/risk-register.md)

## 源码证据摘要

- 上下文与句柄：[src/driver/mu_context.cpp:156-184]、[src/driver/internal.h:232-234]、[src/musa/core/context.cpp:558-577]
- 内存与池：[src/musa/core/context.cpp:1044-1078]、[src/musa/core/memory.cpp:15-60]、[src/hal/m3d/memMgr.h:96-104]、[src/driver/mu_memory.cpp:722-760]
- 提交链：[src/musa/core/context.cpp:2012-2060]、[src/musa/core/stream.cpp:1056-1090]、[src/musa/core/command/command.h:33-42]
- Graph：[src/musa_shared_include/musa.h:1964-2000]、[src/musa/core/node/graphNode.h:29-59]、[src/musa/core/graph.cpp:245-261]
- 导出表：[src/musa_shared_include/export_table.h:859-860]、[src/driver/mu_entry.cpp:2146-2161]、[src/loader/loader_api_list.inc:2-568]
- 构建：[src/util/CMakeLists.txt:1-4]、[src/tools/CMakeLists.txt:10-18]、[CMakeLists.txt:134-136]

## 未解决问题

1. 每条波及链只做了静态调用点检索，未在授权硬件上执行对应回归，运行期影响未知。
2. `src/hal/m3d/m3d` 私有子模块内部的调用点不在本快照，HAL 下游影响面无法闭合。
3. MUPTI/MUASAN/GDB 侧对 `Command::Status`、`MUgraphNodeType` 的消费方只核对到本仓库内的读取点，跨进程/跨版本兼容性未知。

## 下一步阅读建议

先读 [`interface-contracts.md`](interface-contracts.md) 拿到每条链的前置/后置条件，再按 [`../99-roadmap/risk-register.md`](../99-roadmap/risk-register.md) 中的已知缺陷（`Search` 分支反转、`FindPool` 判空缺失）确认改动是否踩中未修复区域。
