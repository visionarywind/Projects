# Graph 执行：设计分析

- 文档目的：说明 M08 Graph 模块的职责边界、内部分层、设计取舍与不变量
- 适用范围：`src/driver/mu_graph.cpp`、`src/musa/core/graph.cpp`/`graph.h`、`src/musa/core/graph/{graph1,graph2}`、`src/musa/core/node/graph*`、`src/musa/core/stream.cpp` 的 capture/launch 段
- 对应源码版本：`5.3.0 develop`（顶层 `CMakeLists.txt` 的 `version_string`/`version_status`）[CMakeLists.txt:33-36]
- 证据状态：静态源码分析；未编译、未在目标机运行，硬件与并发行为未验证
- 最后更新：2026-10-07
- 前置阅读：`README.md`、`source-map.md`、`call-chains.md`
- 后续阅读：`implementation.md`、`interfaces.md`、`risks-and-debt.md`

## 结论摘要

Graph 模块的设计主线是「**描述 → 克隆 → 打平 → 提交**」四段式：调用方只用公共句柄描述依赖图，`Context::CreateGraphExec` 选择 v1/v2 实现，`GraphExec::Init` / `GraphExec2::Init` 先 `CloneGraph` 再把拓扑翻译成可提交结构，真正的设备副作用推迟到 `Stream::CmdLaunchGraph` 之后。[src/musa/core/context.cpp:1420-1450] [src/musa/core/graph/graph1/graphExec.cpp:709-724] [src/musa/core/graph/graph2/graphExec.cpp:362-371]

这套设计的收益是：公共 ABI 不感知硬件，图描述与提交时机解耦，调用方可以在 launch 前反复 update；代价是句柄、登记表、克隆图、submission、command 五层对象同时存在，错误和生命周期要跨层传播。

## 模块职责与边界

- **负责**：图的创建与销毁、18 种节点类型的入图与依赖拓扑、capture 建图、实例化（instantiate）、launch、update、节点参数就地更新、图内存池属性查询。
- **不负责**：公共 ABI 契约与导出表（M02）、stream 依赖合并与命令排队本身（M06）、HAL/M3D 的最终提交（M09）、MUpti 回调实现（M10）。
- 节点类型全集由 `MUgraphNodeType` 定义，`MU_GRAPH_NODE_TYPE_KERNEL` 到 `MU_GRAPH_NODE_TYPE_MEM_TRANSFER` 共 18 个取值：[src/musa_shared_include/musa.h:1964-2000]

## 内部分层与主要抽象

| 层 | 抽象 | 职责 | 证据 |
|---|---|---|---|
| 1 公共声明 | `muGraph*` | 头文件声明与 `_v2` 宏别名 | [src/musa_shared_include/musa.h:105-112] |
| 2 观测包装 | `muGraphCreate` | ApiTrace、MUpti 回调、`skipDriverImpl` 后转发 | [src/driver/mu_wrappers_generated.cpp:6034-6073] |
| 3 driver 实现 | `muapiGraphCreate` | 句柄/参数/context 校验 | [src/driver/mu_graph.cpp:1188-1208] |
| 4 Core 对象 | `Graph`、`GraphNode` 子类、`GraphExec`/`GraphExec2` | 拓扑、参数、克隆、submission | [src/musa/core/graph.h:23-107] |
| 5 提交 | `Stream::CmdLaunchGraph`、`GraphCommand` | 命令入队与依赖解析 | [src/musa/core/stream.cpp:280-416] |
| 6 硬件 | `UniversalManager`、HAL cmd buffer | 真正下发 | [src/musa/core/graph/graph1/universalManager.cpp:73-104] |

第 2 层不是简单的转发：只有在 MUpti 工具回调开启时才可能被 `skipDriverImpl` 拦下，否则两条分支都落到同一个 `muapi*` 实现。[src/driver/mu_wrappers_generated.cpp:6058-6072]

## 控制流与数据流

- **控制流**：`muapi*` 校验 -> `Context` 创建/登记 -> `Graph` 拓扑操作 -> `GraphExec::Init` 翻译 -> `Stream` 入队 -> `GraphCommand` 执行。
- **数据流**：调用方的 `MUSA_*_NODE_PARAMS` 被复制进 `GraphNode` 子类；`CloneGraph` 再复制一遍成执行图，因此 update 参数不会污染调用方的原图。
- **两条建图入口是同构的**：手动 `muapiGraphAddNode` 与 capture 路径最终都落到 `Graph::AddGraphNode`。[src/driver/mu_graph.cpp:1012-1069] [src/musa/core/graph.cpp:245-272]
- capture 用 `m_LastCapturedNodes` 作为新节点默认依赖，从而把「上一条命令」隐式变成边。[src/musa/core/stream.cpp:673-694]

## 生命周期与状态机

```text
Graph:      CreateGraph -> 登记 -> (AddNode/Clone) -> DestroyGraph: 摘登记 -> LockedWait -> delete
GraphExec:  CreateGraphExec -> Init(Clone+Resolve/Flatten) -> Launch/Update -> DestroyGraphExec: (v2) ReleaseStream -> 摘登记 -> LockedWait -> delete
Stream:     BeginCapture -> ACTIVE -> (CaptureNode)* -> EndCapture -> 回到 NONE；Query/Synchronize 会 INVALIDATED
```

销毁必须「先摘登记、再等待、后 delete」，v2 还要先同步并销毁内部 launch stream：[src/musa/core/context.cpp:1690-1738] [src/musa/core/graph/graph2/graphExec.cpp:44-51]

## 并发与同步模型

- 对象登记放在 `Context::CriticalBase` 的读/写锁下，`ValidateGraph` 用读锁，`DestroyGraph` 用写锁：[src/musa/core/context.cpp:1588-1616] [src/musa/core/context.cpp:1618-1659]
- 校验不是「必须属于当前 context」，而是先查本 context，再遍历每个设备的 primary context 和该设备上的其他 context，因此跨 context 使用同一张图在 API 层被放行：[src/musa/core/context.cpp:1602-1616]
- capture 有线程约束：非 RELAXED 模式下 `EndCapture` 必须与 `BeginCapture` 同线程，否则返回 `MUSA_ERROR_STREAM_CAPTURE_WRONG_THREAD`。[src/musa/core/stream.cpp:75-139]
- 图的执行本身是异步的：v1 由单个 `GraphCommand` 承载全部 submission，v2 把节点拆到多条内部 launch stream，最后 join 回调用 stream。[src/musa/core/stream.cpp:281-292] [src/musa/core/stream.cpp:396-414]

## 资源所有权

- 节点对象归 `Graph` 所有：析构时逐个 `delete reinterpret_cast<GraphNode*>(node)`。[src/musa/core/graph.cpp:58-63]
- 虚拟内存与 conditional handle 归 `GraphResource` 所有，`~GraphResource()` 释放物理内存并移出注册表。[src/musa/core/graph.cpp:22-41]
- `CloneGraph` 把 `m_Resource` 以同一个 `shared_ptr` 交给克隆图，原图与执行图共享资源而非各自持有。[src/musa/core/graph.cpp:126-132]
- GraphExec 不持有调用方的图，只持有克隆图；调用方在 instantiate 之后销毁原图不会连带销毁执行图。[src/musa/core/graph/graph1/graphExec.cpp:709-713]

## 错误处理

- 统一返回 `MUresult`，没有异常；driver 层失败时不写输出句柄，部分 add 入口用 `do { ... } while(0)` + `break` 提前退出。
- node 创建成功但入图失败时，由调用方 `delete pGraphNode` 回滚，避免半连接对象。[src/driver/mu_graph.cpp:827-830]
- 依赖合法性由 `Graph::ValidateDependencies` 统一检查空指针、非本图节点和重复依赖：[src/musa/core/graph.cpp:494-511]
- 拓扑含环时 `ResolveGraphImpl` 以 `outQueueNumber != nodes.size()` 判定并清空 submissions：[src/musa/core/graph/graph1/graphExec.cpp:1291-1305]

## 配置影响

| 配置 | 位置 | 影响 |
|---|---|---|
| `graphUserQ` | `Platform::Get().GetSettings()` | 与 `supportEngineSync`、`GetPerfGraphExecVersion()==2` 三者同时成立才创建 `GraphExec2` [src/musa/core/context.cpp:1428-1434] |
| `supportEngineSync` | `Hal().GetProperties().ipProperties` | 同上，硬件不支持则强制回退 v1 [src/musa/core/context.cpp:1429-1434] |
| `GetPerfGraphExecVersion` | `Graph` 自身拓扑 | 存在 kernel 节点降为 1；无 kernel 且根节点数 >1 才保持 2 [src/musa/core/graph.cpp:477-494] |

## 扩展点

1. **新增节点类型**：`MUgraphNodeType` 枚举 -> `Graph` 子类 -> `Context::CreateKernelNode` -> `muapiGraphAddNode` 的 switch case -> v2 `CmdLaunchGraph` 的 switch case -> `NodeParams` variant。少改任何一环都会落到 `MUSA_ERROR_NOT_SUPPORTED` 或空分支。[src/driver/mu_graph.cpp:1062-1065] [src/musa/core/stream.cpp:385-393]
2. **新增 API**：`muapi.h` 声明 + `mu_wrappers_generated.cpp` 包装 + `DriverEntryPointTable` 登记 + loader manifest 条目，四处缺一不可。[src/driver/internal.cpp:497] [src/loader/loader_api_list.inc:426]
3. **edge 语义**：`_v2` 的 `dependencyData` 参数已预留，当前实现忽略它，只支持 force dependency。[src/driver/mu_graph.cpp:769-777]

## 设计取舍

| 取舍 | 好处 | 代价 | 证据状态 |
|---|---|---|---|
| 描述与执行分离（clone 后再 resolve） | 原图可反复 update，执行图拓扑稳定 | 双份图对象、`m_MapCloneByOriginal` 维护成本 | 已确认 |
| v1/v2 双实现按配置与拓扑自动选择 | 无需调用方选择，可随硬件升级 | 两条 launch 路径语义不完全一致 | 已确认 |
| capture 与手动建图共用 `AddGraphNode` | 一套拓扑与校验逻辑 | capture 的隐式依赖规则耦合进 stream 状态机 | 已确认 |
| 分层而非 API 直达硬件 | 可替换 HAL、集中管理对象 | 调用链长、排查成本高 | 已确认 |
| 登记表 + 跨 context 查找 | 兼容多 context 使用同一图 | 查找为 O(设备数 × context 数)，且 GraphExec 的第三段查找有死分支 | 已确认（见 `risks-and-debt.md`） |
| `_v2` 前向兼容宏别名 | 旧调用方无感 | 符号表同时存在多代名字 | 已确认 |

## 不变量

1. 公共句柄在使用前必须能映射到有效对象：`muapi*` 入口统一 `ICast` 后再 `ValidateGraph`/`ValidateGraphExec`。[src/driver/mu_graph.cpp:2304-2324]
2. Core 登记成功前不得暴露半初始化对象：`CreateGraphExec` 在 `Init` 失败时 `delete graphExec` 且不登记。[src/musa/core/context.cpp:1439-1445]
3. 含未绑定 conditional handle 的图不能实例化：`HasUnboundConditionalHandle` 命中直接 `MUSA_ERROR_INVALID_VALUE`。[src/musa/core/context.cpp:1423-1427]
4. 异步资源必须活到对应 Command 完成：`GraphExec` 析构对未完成的 `GraphCommand` 执行 `Wait`。[src/musa/core/graph/graph1/graphExec.cpp:37-41]
5. 销毁先处理未完成工作再释放 HAL 对象：`DestroyGraphExec` 的顺序是 `ReleaseStream` -> 摘登记 -> `LockedWait` -> `delete`。[src/musa/core/context.cpp:1701-1737]

第 4 项只覆盖了 `GraphExec` 自己记录的最后一个 command；v2 的多 stream 路径靠 `ReleaseStream` 内的 `Synchronize` 收敛，二者合起来是否覆盖全部 inflight 资源属于推断，未运行验证。

## 与其他模块的稳定契约

- **M02（ABI/导出）**：`muGraph*` 的函数签名与导出顺序是硬约束；本模块只能新增符号，不能改已有签名。证据为导出表成员与 entry point 登记。[src/musa_shared_include/export_table.h:1280] [src/driver/internal.cpp:497]
- **M06（stream/command）**：`Context::ResolveDependencyAndQueueCommand` 是唯一的入队通道，graph 不自己维护队列。[src/musa/core/stream.cpp:289]
- **M09（HAL）**：v1 通过 `UniversalManager` 下发，v2 复用普通 `CmdLaunchKernel`/`CmdCopyMemory` 等命令，两条路径的 HAL 边界不同。[src/musa/core/graph/graph1/universalManager.cpp:73-104] [src/musa/core/stream.cpp:331-383]
- **M10（观测）**：包装层预留 `toolsIssueCallback` 与 `skipDriverImpl`，观测方可以拦截而不改本模块。[src/driver/mu_wrappers_generated.cpp:6038-6072]

## 未解决问题

1. 头文件写明「含 conditional 节点的图不能被克隆」，而 `GraphExec::Init` 无条件 `CloneGraph`；二者如何协调，静态分析无法判定（推断，未验证）。[src/musa_shared_include/musa.h:1976-1994] [src/musa/core/graph/graph1/graphExec.cpp:709-713]
2. `muapiGraphRemoveDependencies` 之后的度数与父表一致性问题见 `risks-and-debt.md`。
3. v2 路径下 `MEM_ALLOC`/`MEM_FREE`/`CONDITIONAL` 节点的行为需要目标机验证。

## 相关文档

- `call-chains.md`、`implementation.md`、`execution-flows.md`：链路与分支细节
- `interfaces.md`、`data-structures.md`：契约与对象结构
- `risks-and-debt.md`、`testing.md`、`development-guide.md`：风险与改动方法

## 源码证据摘要

- 入口分层：`musa.h` 别名 -> `mu_wrappers_generated.cpp` 包装 -> `mu_graph.cpp` 实现 -> `context.cpp` 创建登记 -> `graph.cpp` 拓扑 -> `stream.cpp` 提交
- v1/v2 分叉：`Context::CreateGraphExec` + `Graph::GetPerfGraphExecVersion`
- 销毁顺序：`Context::DestroyGraph` / `Context::DestroyGraphExec`

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
