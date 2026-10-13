# Graph 执行：数据结构与所有权

- 文档目的：graph：核心数据结构、字段含义与持有关系。
- 适用范围：M08 模块的《Graph 执行：数据结构与所有权》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


## 结构关系

```text
公共句柄 MUgraph / MUgraphNode / MUgraphExec
 -> Musa::Graph / Musa::GraphNode 子类 / Musa::GraphExec(1) 或 Musa::GraphExec2
 -> Graph 持有顶点表、边表、入度/出度表与共享的 GraphResource
 -> GraphExec::m_Submissions 是 submission -> kick 的两级数组
 -> GraphCommand 提交时拷贝 submissions 快照并共享 timeline semaphore
```

## 核心数据结构

| 结构 | 位置 | 关键成员 |
|---|---|---|
| `Graph` | [src/musa/core/graph.h:23-107] | `m_Vertices`、`m_Edges`、`m_NodeInDegree`/`m_NodeOutDegree`、`m_LeafNodes`、`m_MapCloneByOriginal`、`m_Resource`、`m_ConditionalHandleBindings` |
| `GraphResource` | [src/musa/core/graph.h:13-21] | `m_VirtMemories` 虚拟内存列表与 `m_ConditioanlHandles` 绑定表 |
| `GraphNode` | [src/musa/core/node/graphNode.h:29-100] | `m_Type`、`m_ParentGraph`、`m_ParentNodes`、`m_Enable`、`m_LaunchStream`、`m_Kick`（v2 用） |
| `GraphChildGraphNode` | [src/musa/core/node/graphNode.h:102-116] | 持有子图 `m_ChildGraph`，析构时 `DestroyGraph` |
| `GraphEventRecordNode` | [src/musa/core/node/graphNode.h:118-143] | 持有 `Event* m_recordEvent` |
| `GraphEventWaitNode` | [src/musa/core/node/graphNode.h:145-170] | 持有 `Event* m_waitEvent` |
| `GraphEmptyNode` | [src/musa/core/node/graphNode.h:172-183] | 无参数的 no-op 节点 |
| `KernelResource` | [src/musa/core/node/graphKernelNode.h:35-70] | hal kernel state、spill/print/cluster 等 buffer、printf 格式、`m_ForceSingleCore` |
| `AllocParameter` | [src/musa/core/node/graphMemoryAllocNode.h:7-11] | `virtMemory` 为 `shared_ptr<IMemory>`，随节点复制 |
| `MusaKick` | [src/musa/core/graph/graph1/graphExec.h:66-85] | `node`、`nodeParams` variant、`waitValues[KICK_TYPE_COUNT]`、`signalValue`、`kickType`、`mpxMask`、`flags.isEnable` |
| `MusaSubmission` | [src/musa/core/graph/graph1/graphExec.h:102-113] | `type`、`kicks`、路径计数、`engineMask`、`isDirty`、`pHalCmdBuffers` |
| `NodeParams` 宏 | [src/musa/core/graph/graph1/graphExec.h:61-64] | 12 类参数的 `std::variant`，含 `ConditioalParameter` |
| `GraphExec`（v1） | [src/musa/core/graph/graph1/graphExec.h:139-281] | `m_Graph` 克隆图、`m_Submissions`、timeline semaphore、cmd pool、时间戳内存 |
| `GraphExec2`（v2） | [src/musa/core/graph/graph2/graphExec.h:36-145] | `m_BfsNodes`、`m_LaunchStreams`、`m_CurMaxValues`、trace 资源表 |
| `GraphCommand` | [src/musa/core/command/graphCommand.h:17-62] | submissions 快照、`m_PrivateSemaphore`、`m_PtiCtxs`、printf 记录表 |
| `ConditioalParameter` | [src/musa/musaStream.h:117] | conditional 节点的类型、child 图数组与 `childSubmissionIndices` |
| `ConditionalHandleInternal` | [src/musa/core/context.h:32-36] | 宿主可见的 `pMemory`、`defaultValue`、`flags` |
| Stream capture 字段 | [src/musa/core/stream.h:249-255] | `m_CaptureGraph`、`m_CaptureStatus`、`m_LastCapturedNodes`、`m_PrimaryCaptureStream`、`m_BeginThreadId` |

## 所有权要点

- `Graph` 析构对每个顶点 `delete reinterpret_cast<GraphNode*>(node)`，节点对象归 Graph 所有：[src/musa/core/graph.cpp:58-63]
- `GraphResource::~GraphResource()` 逐个 `ReleaseGraphOwnership()`，返回真才 `DestroyPhysMemories` 并归还内存池；随后释放 conditional handle 的内部内存并移出注册表：[src/musa/core/graph.cpp:22-41]
- `Graph::CloneGraph` 把 `m_Resource` 整个共享给克隆图（同一 `shared_ptr`），因此原图先销毁不会连带释放仍被克隆图使用的虚拟内存：[src/musa/core/graph.cpp:126-132]
- alloc 节点在 `SetParentGraph` 时把虚拟内存登记进 `GraphResource`：[src/musa/core/node/graphMemoryAllocNode.cpp:51-55]
- `GraphChildGraphNode` 析构时销毁子图，克隆构造函数会 `CloneGraph` 出一份新的：[src/musa/core/node/graphNode.cpp:40-51]
- `GraphConditionalNode` 析构时逐个 `DestroyGraph` 其 body 图：[src/musa/core/node/graphConditionalNode.cpp:24-31]
- `GraphExec::Init` 用 `CloneGraph` 生成 `m_Graph`，GraphExec 不直接持有调用方的图：[src/musa/core/graph/graph1/graphExec.cpp:709-713]
- `GraphExec` 析构先对最新 `GraphCommand` 执行 `Wait`，再 `delete m_Graph` 并释放时间戳/spill 内存：[src/musa/core/graph/graph1/graphExec.cpp:37-57]
- v1 `GraphCommand` 构造时按值拷贝 submissions、共享 `GraphExec` 的 timeline semaphore：[src/musa/core/command/graphCommand.cpp:20-30]
- `GraphExec2::ReleaseStream` 逐个 `Synchronize` 后销毁内部 launch stream：[src/musa/core/graph/graph2/graphExec.cpp:44-51]

## 登记与查找

- `Context::CriticalBase` 用 `m_Graphs`/`m_GraphExecs` 集合登记对象，`ValidateGraph`/`DestroyGraph` 先查本 context，再遍历各设备 primary context 与设备上其他 context：[src/musa/core/context.cpp:1588-1616]
- `ValidateGraphExec`/`DestroyGraphExec` 用同样三段式查找；销毁成功后 `LockedWait` 再 `delete`：[src/musa/core/context.cpp:1690-1738]
- conditional handle 的 64 字节宿主内存在 `CreateConditionalHandle` 里 `AllocateInternalMem` 分配，并同时登记到 context 表与 `GraphResource`：[src/musa/core/context.cpp:2579-2597]
- v2 的内部 stream 只在 `DestroyGraphExec` 分支里 `ReleaseStream`，因此 graph exec 的销毁顺序必须是「同步内部 stream -> 摘登记 -> 等待 -> delete」：[src/musa/core/context.cpp:1698-1704]

## 未决点

节点的父集合是无序 `GetParentNodes` 返回的 vector，元素顺序由加边顺序决定；`GraphExec::Update` 按下标比较父节点类型，因此同一批父节点以不同顺序加入会被判定为拓扑变化（推断，来自 [src/musa/core/graph/graph1/graphExec.cpp:677-693]，未运行验证）。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/musa/core/node/graphNode.h`（5 处，例：[src/musa/core/node/graphNode.h:29-100]）
- `src/musa/core/graph/graph1/graphExec.h`（4 处，例：[src/musa/core/graph/graph1/graphExec.h:66-85]）
- `src/musa/core/context.cpp`（4 处，例：[src/musa/core/context.cpp:1588-1616]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
