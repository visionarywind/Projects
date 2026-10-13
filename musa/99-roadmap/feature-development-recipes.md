# 功能开发配方

- 文档目的：给出在本代码库新增 API、新增 graph 节点、新增 mempool 属性、修改异步行为、补导出 accessor 的可执行配方，每一步落到真实代码模式。
- 适用范围：`source/musa` 快照（5.3.0 develop）的驱动/核心/HAL 分层。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）。
- 证据状态：已确认（全部引用为静态源码核对）；构建、运行、回归结果均未执行，标注为**静态分析，未验证**。
- 最后更新：2026-10-07
- 前置阅读：[`../00-overview/architecture.md`](../00-overview/architecture.md)、[`../01-modules/module-registry.md`](../01-modules/module-registry.md)
- 后续阅读：[`testing-recipes.md`](testing-recipes.md)、[`technical-debt.md`](technical-debt.md)

## 结论摘要

本代码库可扩展的入口有四类：Driver API（实现 + 分发表 + loader 清单）、graph 节点类型（枚举 + 节点类 + Context 工厂 + `muapiGraphAddNode` 分发 switch）、mempool 属性（枚举 + `MemoryPool::SetAttribute` switch）、导出 accessor（`export_table.h` + `FillAccessors`）。新增功能必须成套改完对应一类的所有落点；漏改任何一环的典型症状是"编译通过但按名取不到函数指针"或"运行时返回 `MUSA_ERROR_NOT_SUPPORTED`"。

---

## 配方 A：新增一个 Driver API

### A1. 选择实现文件

驱动层实现集中在 `src/driver/mu_*.cpp`，该目录由 `aux_source_directory` 整目录收集，新增 `.cpp` 会自动进入编译，无需改清单 [src/driver/CMakeLists.txt:3-9]。

建议按域归档：内存进 `mu_memory.cpp`，现有 `muapiMemAlloc_v2` 可作范例 [src/driver/mu_memory.cpp:271-304]；图进 `mu_graph.cpp`，流进 `mu_stream.cpp`。

### A2. 声明两处原型

1. 驱动内部原型写在 `src/driver/muapi.h`，`muapiMemAlloc_v2` 与 `muapiMemcpyAsync` 的声明就在同一批相邻行 [src/driver/muapi.h:291-356]。
2. 应用可见原型写在 `src/musa_shared_include/musa.h`，例如 `muMemAlloc` [src/musa_shared_include/musa.h:9650-9660]；ABI 版本宏 `MUSA_VERSION` 在文件头 [src/musa_shared_include/musa.h:210-215]，新增 API 必须明确它属于哪个版本号，分发表按版本号选择实现。

### A3. 实现体的固定形态

驱动入口统一是"`InitPlatform()` 前置 → 参数空值/越界校验 → `TlsCtxTop()` 取当前 context → 调 Core 对象 → 返回 `MUresult`"。`muapiMemAlloc_v2` 把这三段写在同一个函数里：参数校验、TLS context 缺失返回 `MUSA_ERROR_INVALID_CONTEXT`、`pContext->CreateMemory` [src/driver/mu_memory.cpp:271-304]。两个前置工具的定义分别是 `TlsCtxTop` [src/driver/internal.h:232-232] 与 `InitPlatform` [src/driver/internal.h:307-320]。

### A4. 兼容包装（`muapi` 实现 → 应用侧 `mu*`）

应用实际调用的 `mu*` 函数体在生成文件 `src/driver/mu_wrappers_generated.cpp`：每个包装构造 `ApiTrace` 记录调用、组装 tools 回调参数、在未被拦截时转发给 `muapi*` 实现 [src/driver/mu_wrappers_generated.cpp:14-46]。`muMemcpyAsync` 的真实转发语句是 `status = muapiMemcpyAsync(dst, src, ByteCount, hStream)` [src/driver/mu_wrappers_generated.cpp:12585-12592]。

该文件与 `generated_musa_meta.h` 头部都写明"由生成器生成，改动会在下次 clean build 丢失" [src/musa_shared_include/generated_musa_meta.h:1-1]，但生成脚本不在本快照，**无法在本仓库内重新生成**（见 [`technical-debt.md`](technical-debt.md)）。所以新增 API 的包装体只能由持有生成脚本的一方产出。

### A5. `muapiGetProcAddress` 分发表注册

应用和工具按名字取函数指针时走 `muapiGetProcAddress_v2`：静态实例 `s_EntryPointTable` 查名，再用 `FindBestMatch` 依据请求版本挑实现 [src/driver/mu_entry.cpp:1226-1250]。表项在 `DriverEntryPointTable` 构造函数里逐条 `addEntryPoints` 注册，形如 `addEntryPoints("muStreamCreate", {{40300, ...}})`，每条给出"引入版本号 → 默认函数 → `_ptsz` 函数"三元组 [src/driver/internal.cpp:175-190]。不注册时 `Find` 返回空，`muapiGetProcAddress` 直接给出"未找到"结果。

### A6. loader 导出清单与导出符号

1. `src/loader/loader_api_list.inc` 用 `MUSA_LOADER_API(名字, 参数, 实参)` 宏逐行登记导出函数，文件首条就是 `muStreamCreate` [src/loader/loader_api_list.inc:1-6]；同一份清单被 loader 以三种方式展开成函数指针表、符号解析和 `extern "C"` 转发。
2. CMake 侧由 `MUSA_LOADER_API_MANIFEST` 读取该清单生成平台导出文件（Windows `.def` / UNIX `.ver`）[cmake/export_symbols.cmake:14-25]。重复条目会在配置期报错，但**漏登记只会表现为运行期少一个导出符号**，没有自动 diff 校验。

### A7. 收尾清单与验收

- [ ] `muapi.h` 与 `musa.h` 原型成对出现；
- [ ] `internal.cpp` 分发表加 `addEntryPoints`；
- [ ] `loader_api_list.inc` 加一行；
- [ ] 兼容包装（依赖快照外的生成脚本）；
- [ ] 错误码语义与同类 API 一致（参考 [`../90-cross-module/error-boundaries.md`](../90-cross-module/error-boundaries.md)）；
- [ ] 静态自检：`rg -n "你的API名" src/` 应同时命中声明、实现、分发表、清单四处。

验收状态：**静态分析，未验证**——本快照未执行构建与运行，没有可对照的运行结果。

---

## 配方 B：新增一个 graph 节点类型

### B1. 扩节点类型枚举

MUgraphNodeType 枚举从 `MU_GRAPH_NODE_TYPE_KERNEL = 0` 排到 `MU_GRAPH_NODE_TYPE_CONDITIONAL = 13`，新增类型续在尾部并保留既有数值，已发布的枚举值不能改 [src/musa_shared_include/musa.h:1963-1982]。

### B2. 新增节点类

节点类放 `src/musa/core/node/`，每个类型一对 `graphXxxNode.{h,cpp}`（现有 `graphKernelNode.cpp` 等 15 对）。基类 `GraphNode` 持有 `m_Type` 并由 `GetType` 暴露，构造函数要求调用方传入类型 [src/musa/core/node/graphNode.h:30-50]。父子关系、`m_CorId`、`m_GraphExec` 等公共字段已由基类承担，新类不要重复定义。

### B3. Context 工厂方法

`Context` 为每种节点提供 CreateXxxNode 风格的工厂：kernel 路径 `CreateKernelNode` 只做转发，真正构造发生在 `CreateKernelNodeOnStream`，内部 `GraphKernelNode::Create` + `UpdateParams()`，失败路径 `delete graphNode` 并返回错误码 [src/musa/core/context.cpp:2230-2267]；具体构造函数在 [src/musa/core/node/graphKernelNode.cpp:12-30]。新节点必须沿用"创建失败释放半成品对象"的写法，否则句柄泄漏。

### B4. Driver 层的两处 switch

1. 专用 API 形如 `muapiGraphAddKernelNode`，校验顺序固定为空值 → TLS context → `ValidateGraph` → `ValidateDependencies` → 工厂 → `AddGraphNode`，挂图失败时 `delete pGraphNode` [src/driver/mu_graph.cpp:718-767]。
2. 通用入口 `muapiGraphAddNode` 按 `nodeParams->type` 分发到上面的专用 API，新增类型必须补 `case`，否则落到 `default` 返回 `MUSA_ERROR_NOT_SUPPORTED` [src/driver/mu_graph.cpp:1012-1069]。
3. 反向读取参数的 `muapiGraphNodeGetParams` 还有第三个 switch，同样按类型回填 `nodeParams`，漏补时该节点的 `GetParams` 会走 `default` 失败 [src/driver/mu_graph.cpp:3304-3363]。

### B5. 挂图与依赖

`Graph::AddGraphNode` 负责把节点登记进图：先做 conditional 句柄绑定的特判，再 `AddNode` 记入顶点、`SetParentGraph`、对每个依赖调 `AddEdge` [src/musa/core/graph.cpp:245-271]；顶点登记本身只做 `m_Vertices` 追加与入度/出度初始化 [src/musa/core/graph.cpp:65-70]。依赖校验在 driver 层由 `ValidateDependencies` 完成，不要在 `AddGraphNode` 里重复实现。

### B6. 执行落点

图执行命令 `GraphCommand::Submit` 直接调用 `Execute()` 把节点变成实际队列提交 [src/musa/core/command/graphCommand.cpp:69-79]，最终统一经 `UniversalManager::Submit` 组装 `Hal::QueueSubmitInfo` 并调用 HAL 队列 `Submit` [src/musa/core/graph/graph1/universalManager.cpp:470-500]。新增节点类型还必须在其可执行分支里给出对应实现，否则会在执行期才发现不支持。**该分支在私有 M3D 子模块之外，行为未验证。**

---

## 配方 C：新增一个 mempool 属性

1. **枚举**：`MUmemPool_attribute` 中新增成员，紧跟既有 `MU_MEMPOOL_ATTR_RELEASE_THRESHOLD` 等值之后，保持数值单调 [src/musa_shared_include/musa.h:4831-4870]。
2. **Driver 入口**：`muapiMemPoolSetAttribute` 已做统一前置——`InitPlatform`、`value` 判空、`ValidateMemoryPool`、拒绝已导入池，之后才转发 `pMemoryPool->SetAttribute` [src/driver/mu_mempool.cpp:139-158]。新增属性不需要改这里。
3. **核心实现**：`MemoryPool::SetAttribute` 用 `switch (attr)` 落到成员字段或 `m_pHalPool`，`default` 返回 `MUSA_ERROR_INVALID_VALUE`；布尔类属性按 `attribVal` 取反写入，数值类属性直接透传 `uint64_t` [src/musa/core/memoryPool.cpp:114-167]。
4. **对称读取**：`MemoryPool::GetAttribute` 必须同步补 `case`，并注意它用 `MU_MEMPOOL_ATTR_ALLOCATION_TYPE` 到 `MU_MEMPOOL_ATTR_MAX_POOL_SIZE` 的区间判定"创建期属性"，导入池只允许读创建期属性 [src/musa/core/memoryPool.cpp:170-205]。新增属性若属于创建期，要把区间上界一起调整。
5. **值类型**：每个属性的取值类型（`int` / `muuint64_t`）写在枚举注释里，`SetAttribute` 里用 `static_cast<int*>` 或 `static_cast<uint64_t*>` 解引用，类型写错就是未定义行为。

---

## 配方 D：修改异步行为（stream / command）

不能只改 API 返回点，必须沿这条链逐层核对：

1. `Context::ResolveDependencyAndQueueCommand`：在这里建立默认流、barrier 流与当前流的依赖，然后才 `QueueCommand`，阻塞模式下还会 `commandRef->Wait()` [src/musa/core/context.cpp:2012-2076]。
2. `Stream::QueueCommand`：`m_AsyncCount` 满容量时自旋等待，随后持 `m_SubmitMtx` 记录前驱命令、置 `queued` 状态、压入 `m_CommandList` 并唤醒提交线程 [src/musa/core/stream.cpp:1056-1104]。
3. `Stream::AsyncSubmit`：提交线程从 `m_CommandList` 取命令、按 `CanMergeTo` 合并、批量 `Submit` 并写入 `m_InflightList` [src/musa/core/stream.cpp:1159-1310]。
4. 合并条件本身在命令类里，例如 `DispatchCommand::CanMergeTo` 会因"带 copy job 的 kernel"或"使用 spill 内存"拒绝合并 [src/musa/core/command/dispatchCommand.cpp:66-80]。

修改验收至少覆盖：单流顺序、跨流 event、默认流阻塞语义、PFM 串行分支，以及 MUPTI/GDB 侧的时间戳与 correlation 是否仍对得上。**以上均为静态分析，未验证。**

---

## 配方 E：新增或修改导出 accessor

1. accessor 结构体定义在 `src/musa_shared_include/export_table.h`，未填充成员统一用 `AccessorHint = 0xDEADBEEF` 占位，通过 `ADDMEMBER` 宏声明 [src/musa_shared_include/export_table.h:859-866]。**占位值是"尚未填充"，不是"可调用"。**
2. 填充方在 `muapiGetExportTable` 内：先按 `Client` 枚举选表，再用 lambda `FillAccessors` 把 `uintptr_t` 数组逐槽写入 [src/driver/mu_entry.cpp:2146-2195]。
3. 未知 `Client` 走 `default` 分支，静默返回 MUPTI 驱动表，源码自带 FIXME，说明这是兼容性妥协而非设计 [src/driver/mu_entry.cpp:2296-2303]。
4. 新增 accessor 必须同时改 `export_table.h` 与对应 `FillAccessors` 调用，只改一边会留下 `0xDEADBEEF` 悬空槽位，调用方无法区分"不支持"与"漏填"。

---

## 配方 F：新增构建目标或源文件

| 位置 | 收集方式 | 新增文件的后果 | 证据 |
|---|---|---|---|
| `src/driver` | `aux_source_directory` | 新 `.cpp` 自动编译进 `libmusa` | [src/driver/CMakeLists.txt:3-9] |
| `src/util` | `aux_source_directory` | 新 `.cpp` 自动进 `util` 静态库 | [src/util/CMakeLists.txt:1-4] |
| `src/tools` | `GLOB_RECURSE` | 每个 `.cpp` 生成一个同名可执行文件并链接 driver 动态库 | [src/tools/CMakeLists.txt:10-15] |

三处都是"隐式纳入"：好处是加文件不用改清单，代价是误放进来的 `.cpp` 也会被编译，且 CMake 不会因新增文件自动重跑（需要 `cmake` 重新配置或依赖 `CONFIGURE_DEPENDS`，本仓库未使用）。工具目录尤其要注意——放进去的任何 `.cpp` 都会变成独立可执行目标。

---

## 相关文档

- [`risk-register.md`](risk-register.md)：这些扩展点上已知的风险。
- [`technical-debt.md`](technical-debt.md)：生成脚本、隐式 glob、无符号快照等结构性欠账。
- [`testing-recipes.md`](testing-recipes.md)：改完之后在哪个层验证。
- [`../90-cross-module/change-impact-map.md`](../90-cross-module/change-impact-map.md)：改动的跨模块影响面。

## 源码证据摘要

- 分发表：[src/driver/internal.cpp:175-190]、[src/driver/mu_entry.cpp:1226-1250]
- 导出清单：[src/loader/loader_api_list.inc:1-6]、[cmake/export_symbols.cmake:14-25]
- graph 节点：[src/musa_shared_include/musa.h:1963-1982]、[src/driver/mu_graph.cpp:1012-1069]、[src/musa/core/context.cpp:2230-2267]、[src/musa/core/graph.cpp:245-271]
- mempool 属性：[src/musa_shared_include/musa.h:4831-4870]、[src/driver/mu_mempool.cpp:139-158]、[src/musa/core/memoryPool.cpp:114-167]
- 异步链：[src/musa/core/context.cpp:2012-2076]、[src/musa/core/stream.cpp:1056-1104]、[src/musa/core/stream.cpp:1159-1310]
- accessor：[src/musa_shared_include/export_table.h:859-866]、[src/driver/mu_entry.cpp:2146-2195]

## 未解决问题

1. `mu_wrappers_generated.cpp` 与 `generated_*_meta.h` 的生成脚本不在快照，配方 A4 无法在本仓库内闭环。
2. 新增 graph 节点在 `GraphExec`（graph1/graph2 两套执行器）中的执行分支未逐个展开。
3. 所有配方只做了静态核对，未执行编译与运行。

## 下一步阅读建议

先按配方 A 走一遍静态自检（四处 `rg` 命中），再读 [`../80-demos/D01-muinfo/modification-exercises.md`](../80-demos/D01-muinfo/modification-exercises.md) 做一次最小改动练习。
