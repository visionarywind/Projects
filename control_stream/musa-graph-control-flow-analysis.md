# MUSA Graph 控制流实现分析

## 1. 分析范围与结论摘要

本文分析 MUSA Driver 中 Graph 的控制流实现，源码位置为：

```text
shanfeng@10.20.34.9:/home/shanfeng/workspace/linux-ddk/musa
```

分析关注 Graph API、GraphExec、stream capture、child graph、conditional graph，以及 Graph 命令最终如何进入 HAL。这里的“控制流”包含两种不同机制：

1. **静态控制依赖**：Graph 节点之间的 DAG 边，以及节点在不同 engine 上的 wait/signal 关系。
2. **动态控制流**：IF、WHILE、SWITCH 根据运行时 conditional handle 选择或重复提交 child graph。

最重要的结论是：

> MUSA Graph 将静态依赖编译为设备侧 acquire/release 同步命令；而 IF、WHILE、SWITCH 并不是 GPU command buffer 内的原生分支指令，而是由 GraphCommand 在 Driver 提交路径中等待 semaphore、读取 host-mapped conditional handle，然后递归选择或重复执行 submission cluster。

整体执行形态如下：

```text
Graph API / Stream Capture
        │
        ▼
Graph：vertices + edges + parent list
        │
        │ muGraphInstantiate
        ▼
GraphExec::Init
  ├── CloneGraph
  ├── ResolveGraph：拓扑排序、环检测、递归解析子图
  ├── CreateExecResource
  └── PrepareAllSubmissions：预构建设备命令
        │
        ▼
submission clusters
  ├── cluster 0：根 Graph
  ├── child graph cluster
  └── conditional branch/body cluster
        │
        │ muGraphLaunch
        ▼
Stream::CmdLaunchGraph
        │
        ▼
GraphCommand::ExecuteImpl(0)
  ├── device submission → HAL command buffer
  ├── host submission → callback
  ├── child graph → 递归 ExecuteImpl
  └── conditional → 等待、读取条件、选择/循环执行子图
```

## 2. 远程工作树与证据边界

本文是基于远程仓库的只读源码分析。调研时远程工作树存在未提交改动，主要包括：

```text
M src/driver/CMakeLists.txt
M src/hal/m3d/m3d
M src/musa/core/command/command.cpp
M src/musa/core/command/dispatchCommand.cpp
M src/musa/core/stream.cpp
?? .codegraph/
?? src/musa/core/submitTrace.h
```

其中 `stream.cpp` 的已知改动主要是 submit trace 插桩；本文涉及的 `BeginCapture`、`EndCapture`、`CaptureNode` 和 `CmdLaunchGraph` 核心逻辑没有被当作插桩结论，而是按当前工作树源码核对。

本文引用的是稳定的源码相对路径和关键函数，不把远程工作树的临时改动当作仓库基线。没有在本次分析中修改远程源码、构建或运行测试。

## 3. 关键模块索引

| 模块 | 关键文件 | 作用 |
|---|---|---|
| Driver Graph API | `src/driver/mu_graph.cpp` | Graph 创建、节点 API、instantiate、launch、conditional handle API |
| Graph 数据模型 | `src/musa/core/graph.h`、`src/musa/core/graph.cpp` | 节点、边、入度/出度、root/leaf、资源和 clone |
| 通用节点对象 | `src/musa/core/node/graphNode.h` | parent graph、parent nodes、enable 状态、kick 和 GraphExec 关联 |
| Conditional node | `src/musa/core/node/graphConditionalNode.cpp` | 为 IF/WHILE/SWITCH 创建 child graph |
| GraphExec v1 | `src/musa/core/graph/graph1/graphExec.cpp` | 拓扑解析、submission 构建、同步命令和 command buffer |
| GraphCommand | `src/musa/core/command/graphCommand.cpp` | launch 后执行 submission、child graph 和动态控制流 |
| UniversalManager | `src/musa/core/graph/graph1/universalManager.cpp` | submission、callback、递归 graph、条件读取和 semaphore 协调 |
| Stream capture | `src/musa/core/stream.cpp` | capture 状态、frontier、捕获节点和 Graph launch 入队 |
| Context 工厂 | `src/musa/core/context.cpp` | GraphExec v1/v2 选择、conditional handle 分配 |
| GraphExec v2 | `src/musa/core/graph/graph2/graphExec.cpp` | 多内部 stream 的扁平化路径 |
| 控制流样例 | `tests/conditionalNode.cu` | IF 与 WHILE 的使用方式和 handle 更新样例 |
| Graph 性能背景 | `DriverPerfModel/13-Graph执行流程与DriverPerf埋点覆盖分析-20260817.md` | Graph launch、submission 与设备时间边界 |

## 4. Driver API 到 GraphCommand 的完整调用链

### 4.1 Instantiate

`muGraphInstantiate*` 位于 `src/driver/mu_graph.cpp`，主要路径是：

```text
muapiGraphInstantiate_v2
    → TlsCtxTop
    → Context::ValidateGraph
    → Context::CreateGraphExec
        → 根据条件选择 GraphExec 或 GraphExec2
        → GraphExec::Init
            → CloneGraph
            → ResolveGraph
            → CreateExecResource
            → PrepareAllSubmissions
```

源码证据：

- `src/driver/mu_graph.cpp:2260`
- `src/musa/core/context.cpp:1429`
- `src/musa/core/graph/graph1/graphExec.cpp` 中的 `GraphExec::Init`

`Context::CreateGraphExec()` 只有在以下条件同时满足时才选择 GraphExec2：

```text
graphUserQ 已开启
AND 设备支持 engine sync
AND Graph::GetPerfGraphExecVersion() == 2
```

否则使用 GraphExec v1。当前源码的性能版本选择逻辑是：含 kernel 的 Graph 倾向于 GraphExec v1；无 kernel 且存在多个 root 时才可能成为 GraphExec2 候选。

### 4.2 Launch

`muapiGraphLaunch()` 的路径是：

```text
muapiGraphLaunch
    → ValidateGraphExec
    → Stream::CmdLaunchGraph
        → 创建 GraphCommand
        → GraphExec::SetCommand
        → 设置 Graph ID
        → Context::ResolveDependencyAndQueueCommand
```

源码证据：

- `src/driver/mu_graph.cpp:2330`
- `src/musa/core/stream.cpp:308`

GraphExec v1 的 `CmdLaunchGraph()` 不会在 API 调用点立即遍历并重新生成所有设备命令，而是创建 `GraphCommand` 并加入 stream command 队列。真正的提交在线程后续执行 `GraphCommand::Submit()` 时发生。

### 4.3 GraphCommand 执行

```text
GraphCommand::Submit
    → MUpti::RegisterGraphTrace
    → GraphCommand::Execute
        → MarkGraphTraceBegin
        → ExecuteImpl(0)
```

`ExecuteImpl(0)` 负责执行根 submission cluster。遇到 child graph 或 conditional 时，通过 `UniversalManager::CmdGraph()` 调用递归 callback。

## 5. Graph 的逻辑数据模型

### 5.1 节点、边和度数

`Graph` 主要维护：

```cpp
std::vector<IGraphNode*> m_Vertices;
std::unordered_map<GraphNode*, std::list<GraphNode*>> m_Edges;
std::unordered_map<GraphNode*, size_t> m_NodeInDegree;
std::unordered_map<GraphNode*, size_t> m_NodeOutDegree;
std::vector<IGraphNode*> m_LeafNodes;
```

每个 `GraphNode` 另外维护：

```cpp
IGraph* m_ParentGraph;
std::vector<IGraphNode*> m_ParentNodes;
bool m_Enable;
Stream* m_LaunchStream;
MusaKick2 m_Kick;
```

添加节点时：

```text
Graph::AddGraphNode
    → AddNode(graphNode)
    → SetParentGraph(this)
    → 对每个 dependency 调用 AddEdge(parent, child)
```

`AddEdge()` 同时完成四件事：

1. 确保父、子节点在顶层节点列表中；
2. 更新邻接表 `m_Edges[parent]`；
3. 更新 parent 的 out-degree 和 child 的 in-degree；
4. 调用 `childNode->AddParent(parentNode)`。

因此，Graph 同时保留两种方向的数据：

```text
parent → children：用于拓扑遍历
child  → parents ：用于实例化时生成 wait values
```

### 5.2 Root 和 leaf

root 节点由入度为零的节点得到：

```text
root = { node | inDegree(node) == 0 }
```

leaf 节点由出度为零的节点得到：

```text
leaf = { node | outDegree(node) == 0 }
```

root 用于实例化时启动 Kahn 拓扑排序；leaf 用于 stream capture 结束时检查所有捕获分支是否已经汇合到当前 frontier。

## 6. Stream capture 如何形成控制依赖

### 6.1 BeginCapture

`Stream::BeginCapture()` 位于 `src/musa/core/stream.cpp:72`，主要动作：

```text
检查当前 stream 不能已经 active
    → 设置 Graph 的 capture device
    → 保存 m_CaptureGraph
    → 保存 capture mode
    → 设置 capture status = ACTIVE
    → 标记 origin stream
    → 记录开始 capture 的线程 ID
    → 注册 captured stream
    → 将显式初始依赖放入 m_LastCapturedNodes
```

关键字段：

| 字段 | 含义 |
|---|---|
| `m_CaptureGraph` | 当前捕获目标 Graph |
| `m_CaptureStatus` | active、invalidated 等状态 |
| `m_LastCapturedNodes` | 当前依赖 frontier |
| `m_IsOriginStream` | 是否为开始 capture 的 origin stream |
| `m_BeginThreadId` | 开始 capture 的线程 |
| `m_CapturedEvents` | capture 期间关联的 event |

### 6.2 CaptureNode 与 frontier

当 stream 处于 capture 状态时，普通操作不直接生成普通 command，而是进入 `CaptureNode()`：

```cpp
m_CaptureGraph->AddGraphNode(
    pGraphNode,
    m_LastCapturedNodes.data(),
    m_LastCapturedNodes.size());

SetLastCapturedNodes(&pGraphNode, 1);
```

默认串行 capture 可以表示为：

```text
last captured nodes
        │
        ▼
新节点的 parent list
        │
        ▼
新节点成为新的 last captured node
```

这使得连续 API 调用形成：

```text
A → B → C → D
```

`AddLastCapturedNodes()` 和 `SetLastCapturedNodes()` 分别支持追加或替换 frontier，用于表达 event、跨 stream capture 和显式边关系。跨 stream capture 的完整协调还依赖 stream/event 相关路径，不能只用单一 stream 的默认串行模型概括。

### 6.3 EndCapture

`Stream::EndCapture()` 位于 `src/musa/core/stream.cpp:102`，会检查：

- 是否在 origin stream 上结束；
- 非 relaxed 模式下是否由开始 capture 的线程结束；
- capture 是否已经 invalidated；
- capture 是否处于 active 状态；
- Graph 中的每个 leaf 是否都存在于当前 `m_LastCapturedNodes`。

如果 leaf 没有 join 到当前 frontier，返回：

```text
MUSA_ERROR_STREAM_CAPTURE_UNJOINED
```

成功结束后返回捕获 Graph，清理 capture 状态，并恢复捕获 event 的普通状态。

## 7. GraphExec：从 DAG 到 submission cluster

### 7.1 初始化阶段

GraphExec v1 的初始化顺序为：

```text
CloneGraph
    → ResolveGraph
        → Kahn 拓扑排序
        → 每个节点转换为 kick/submission
        → child graph 递归解析
        → conditional branch 递归解析
    → CreateExecResource
    → PrepareAllSubmissions
        → 分配同步内存、command buffer、timestamp 等资源
        → 预写 device command
```

实例化使用 clone 的重要原因是：GraphExec 可以固化一份执行版本，同时保留原始 Graph 与 clone node 的映射，后续通过 GraphExec 参数更新 API 修改 clone。

### 7.2 Kahn 拓扑排序和环检测

`ResolveGraphImpl()` 使用如下算法：

```text
把所有 root 节点放入队列
while 队列非空：
    取出一个节点
    转换为 kick/submission
    遍历其 children
    对 child 的临时入度减一
    入度为零则加入队列

如果出队节点数量 != Graph 节点总数：
    返回 MUSA_ERROR_INVALID_VALUE
```

因此，逻辑 Graph 必须是 DAG。WHILE 的重复执行并不在 Graph 的静态边中创建回边；它是运行时对已经实例化的 body cluster 进行重复调用，所以不会破坏静态 DAG。

### 7.3 Child graph 和 conditional cluster

普通 child graph：

```text
Graph child node
    → ResolveGraphImpl(child Graph, &index)
    → 保存 ChildGraphParameter.childGraphSubmissionsIndex = index
    → submission type = MUSA_SUBMISSION_GRAPH
```

Conditional node：

```text
conditional node
    → 读取 ConditioalParameter
    → 对每个 child graph 调用 ResolveGraphImpl
    → 保存各 child graph 的 cluster index
    → submission type = MUSA_SUBMISSION_CONDITIONAL
```

结果可以抽象为：

```text
m_Submissions[0] = 根 Graph 的 submissions
m_Submissions[1] = child graph A
m_Submissions[2] = IF true branch
m_Submissions[3] = IF false branch
m_Submissions[4] = WHILE body
...
```

父 cluster 不内联子图全部命令，只保留一个引用。这样做便于运行时选择分支，也使 WHILE 可以多次调用同一个 body cluster。

## 8. Submission 与设备同步

### 8.1 Submission 分组

`NeedNewSubmission()` 在以下场景拆分 submission：

- 当前 submission 为空；
- 前一个 submission 是 child graph；
- 前一个 submission 是 conditional；
- submission type 发生变化；
- kernel access policy window 不一致；
- external semaphore signal/wait 需要独立边界。

所以：

```text
一个 Graph
    ≠ 一个 submission
    ≠ 一个 kernel launch
```

多个普通 device node 可以位于同一 `MusaSubmission`；child graph、conditional、host 和 host-device 操作会形成更明显的边界。

### 8.2 Kick 的 wait/signal 值

实例化时，每个 device kick 按 engine/kick type 维护 signal value。子节点遍历父节点：

```text
parent kick.signalValue
    → child kick.waitValues[parent kick type]
```

同一种 engine 的多个父依赖取最大值：

```cpp
kick.waitValues[kickType] = std::max(
    kick.waitValues[kickType],
    parentKick.signalValue);
```

### 8.3 CmdAcquire 与 CmdRelease

`GraphExec::CmdAcquire()` 位于 `src/musa/core/graph/graph1/graphExec.cpp:1957`：

```text
对每个有 waitValue 的父 engine：
    取对应 m_SyncMems[idx] 的 device VA
    生成 Hal::AcquireParameter
    CmdAcquire
```

`GraphExec::CmdRelease()` 位于同一文件约 1970 行：

```text
取当前 kick 对应 engine 的 sync memory
写入当前 signalValue
CmdRelease
```

硬件执行关系为：

```text
父节点
  → CmdRelease(sync memory X, value N)

子节点
  → CmdAcquire(sync memory X, value N)
  → 开始执行
```

这是 DAG 边最终进入 HAL 的路径。拓扑排序只负责保证构造过程合法，真正的跨 engine 执行等待由 acquire/release 完成。

### 8.4 Submission 间 private semaphore

除 engine sync memory 外，UniversalManager 使用 GraphCommand 私有 semaphore 串行连接 submission：

```text
submission A
    → signal privateSemaphore = N

submission B
    → wait privateSemaphore = N
    → signal privateSemaphore = N + 1
```

`UniversalManager::Submit()` 中：

- `WaitWithinCmd` 使用 `m_PrivateSemaphore` 当前值；
- `SignalWithinCmd` 增加 `m_PrivateSemaphoreValue[0]` 并 signal；
- `WaitBetweenCmd` 使用 GraphCommand 外部 wait semaphore；
- `SignalBetweenCmd` 使用 GraphCommand 外部 signal timeline semaphore。

根 cluster 的第一个 submission 通常使用 `WaitBetweenCmd`，根 cluster 的最后一个 submission 使用 `SignalBetweenCmd`，内部 submission 使用 `WaitWithinCmd`/`SignalWithinCmd`。

## 9. Command buffer 写入与节点禁用

`PrepareAllSubmissions()` 在实例化阶段预构建设备 submission。`WriteSubmission()` 按 kick 类型选择 HAL command buffer，并为每个 engine 写入必要的等待和节点命令。

节点类型大致映射为：

| Graph node | command 写入 |
|---|---|
| Kernel | dispatch command |
| Memcpy | memcpy command |
| Memset | memset command |
| Memory atomic | atomic command |
| Event record/wait | event command |
| Empty | 空操作/同步边界 |
| Disabled node | NOP |

禁用节点不会从 DAG 删除，而是保留拓扑位置并写入 NOP。这避免重新计算后续节点的依赖布局，也保留 GraphExec 的同步位置。

## 10. Child graph 的运行时执行

`GraphCommand::ExecuteImpl()` 遇到 `MUSA_SUBMISSION_GRAPH` 时：

```cpp
auto& childParams =
    std::get<ChildGraphParameter>(submission.kicks[0].nodeParams);

auto recursiveCall = std::bind(
    &GraphCommand::ExecuteImpl,
    this,
    childParams.childGraphSubmissionsIndex);

status = universalMgr->CmdGraph(
    recursiveCall,
    this,
    waitMode,
    signalMode);
```

`UniversalManager::CmdGraph()` 的实际结构是：

```text
根据 waitMode 等待前序 semaphore
    → 直接调用 recursiveCall()
    → 如果 signalMode 为 SignalBetweenCmd：
          等待内部 private semaphore
          HostSignalFinish()
```

所以 child graph 不是单个硬件 branch packet，也不是把所有 child 节点内联到父 command buffer。它是独立 cluster，由父 GraphCommand 通过 cluster index 递归执行。

## 11. Conditional handle 的内存与数据流

### 11.1 Handle 创建

`Context::CreateConditionalHandle()` 位于 `src/musa/core/context.cpp:2489`：

```cpp
static constexpr uint32_t ConditionalHandleSize = 64;

AllocateInternalMem(
    ConditionalHandleSize,
    0,
    &pMemory,
    Hal::InternalMemoryPoolType::HostMapped);

*pHandle = pMemory->GetDevicePointer();
*reinterpret_cast<uint32_t*>(pMemory->GetHostPointer()) = defaultValue;
```

可以抽象为：

```text
64-byte host-mapped internal memory
    ├── device pointer：作为 MUgraphConditionalHandle
    └── host pointer：Driver 读取 uint32_t 条件
```

Context 保存 handle 到 `ConditionalHandleInternal` 的映射，其中包含：

```text
pMemory
 defaultValue
 flags
```

GraphResource 保存该 handle 的资源关系，在 Graph 销毁时负责释放相关资源。

### 11.2 Conditional 执行前的读取

Conditional submission 执行时：

```text
ResetConditionalHandle(handle)
    → GetContionalValue(handle)
        → 等待前序 Graph submission 完成
        → 查询 handle 对应 Memory
        → 获取 host pointer
        → 读取 uint32_t
    → 根据值选择 child cluster
```

`GetContionalValue()` 在首个 Graph submission 使用 `HostWaitSemaphores()`，内部 submission 使用 private semaphore wait。条件读取发生在前序 GPU 工作完成之后，而不是提交前盲读。

### 11.3 GPU 更新 handle

用户可以将 handle 的 device pointer 作为 kernel 参数。`tests/conditionalNode.cu` 展示了这种模式：

```text
kernel 更新 handle 指向的值
    → kernel 所属 submission 完成
    → Driver 等待 semaphore
    → Driver 从 host-mapped pointer 读取值
```

这使得前序 kernel 能决定后续 IF/WHILE/SWITCH 的路径。

## 12. IF、WHILE、SWITCH 的实现

### 12.1 IF

源码：`src/musa/core/command/graphCommand.cpp` 的 `MU_GRAPH_COND_TYPE_IF` 分支。

约束：`conditionalParams.size` 必须为 1 或 2。

执行规则：

```text
conditional value != 0
    → 执行 childSubmissionIndices[0]

conditional value == 0 且 size == 2
    → 执行 childSubmissionIndices[1]

conditional value == 0 且 size == 1
    → 执行 emptyCall
```

文字时序：

```text
前序节点
    → 更新 conditional handle
    → semaphore signal

Driver
    → 等待前序完成
    → 读取 handle
    → value != 0：递归提交 true graph
    → value == 0：递归提交 false graph 或空 callback

后继节点
    → 等待 conditional 分支的完成边界
```

`tests/conditionalNode.cu:32-150` 验证了 IF 的两个分支：条件为 0 时执行写入 102 的分支，条件为 1 时执行写入 101 的分支。

### 12.2 WHILE

约束：`conditionalParams.size` 必须为 1，唯一 child graph 是循环体。

当前源码结构：

```cpp
do {
    ExecuteImpl(bodyCluster);
    waitMode = WaitMode::WaitWithinCmd;
    GetContionalValue(
        handle,
        this,
        &conditionalValue,
        waitMode);
} while (conditionalValue != 0);
```

运行模型：

```text
初始化 handle
    → 执行 body cluster 第 0 次
    → 等待 body 完成
    → 读取 handle
    → 非零：再次执行 body cluster
    → 零：退出循环
```

WHILE 的回边是 Driver 的递归调用，不是 Graph DAG 中的静态回边。这样 Graph 仍然可以在实例化时使用 Kahn 算法验证为 DAG。

#### WHILE 的语义风险

从当前源码表面看，WHILE 是后判断形式：即使进入 conditional node 时初始值为 0，body 也可能先执行一次。

但本结论仍需结合正式 API 语义和专门测试确认，原因包括：

- 前序初始化 kernel 通常会先设置 handle；
- API 可能规定 body 首次执行由构图协议保证；
- 当前已有样例使用正数 loop count，未覆盖初始值为 0 的边界。

`tests/conditionalNode.cu:153-247` 的 WHILE 样例为：

```text
initHandleNode
    → WHILE body
        → addValueNode
        → decHandleNode
    → device-to-host memcpy
```

### 12.3 SWITCH

SWITCH 把 conditional value 当作 child graph 下标：

```cpp
if (conditionalValue < conditionalParams.size) {
    index = childSubmissionIndices[conditionalValue];
    ExecuteImpl(index);
} else {
    emptyCall();
}
```

因此：

```text
value = 0 → child 0
value = 1 → child 1
...
value >= size → 空分支
```

当前读取到的执行代码没有独立的 default child graph；越界行为是 empty callback。

## 13. GraphCommand 的控制流时序

### 13.1 IF 时序

```text
GraphCommand::ExecuteImpl(root)
    │
    ├── 提交前序 device submission
    │       └── GPU 执行并更新 conditional handle
    │
    ├── 遇到 CONDITIONAL
    │       ├── ResetConditionalHandle
    │       ├── HostWaitSemaphores / private semaphore Wait
    │       ├── 读取 host-mapped conditional value
    │       └── 选择 branch cluster
    │
    ├── CmdGraph(branch callback)
    │       ├── ExecuteImpl(branch cluster)
    │       └── 按需要 signal parent boundary
    │
    └── 继续 root cluster 后继 submission
```

### 13.2 WHILE 时序

```text
GraphCommand::ExecuteImpl(root)
    │
    ├── 初始化条件
    │
    ├── CmdGraph(body)
    │       └── ExecuteImpl(body cluster)
    │
    ├── WaitWithinCmd
    ├── GetContionalValue
    │
    ├── value != 0 ──┐
    │                 │
    │                 └── 再次 CmdGraph(body)
    │
    └── value == 0 → 退出并继续后继
```

如果 WHILE 是根 Graph 的最后一个节点，源码还会通过 empty callback 补发 `SignalBetweenCmd`，因为循环次数在开始时并不确定，不能在静态解析阶段简单确定最后一次 body submission 的外部 signal 边界。

## 14. GraphExec2 的适用边界

`GraphExec2::GraphFlatten()` 同样使用拓扑排序和 wait/signal 值，但其主要逻辑是：

- 将节点分配到多个内部 stream；
- 对 CE 节点建立内部 stream；
- 将 parent node 的 signal value 转换为 child node 的 wait value；
- 对普通 `MU_GRAPH_NODE_TYPE_GRAPH` 进行递归 flatten。

当前已读的 `GraphFlatten()` 只显式处理：

```text
KERNEL
GRAPH child graph
default
```

没有看到与 GraphExec v1 相同的 conditional IF/WHILE/SWITCH 解释执行逻辑。再结合 `Context::CreateGraphExec()` 的选择条件，可以得出较稳妥的边界判断：

> 本文确认的 conditional 控制流机制属于 GraphExec v1；不能仅凭 GraphExec2 的普通 child graph flatten 代码，断言 GraphExec2 完整支持 conditional graph。

这部分应通过 GraphExec2 专项测试继续确认，尤其是无 kernel、多 root、user queue 和 conditional node 同时出现的组合。

## 15. 性能与实现特征

### 15.1 实例化前移命令构建

普通 device submission 的 command buffer 在 Graph instantiate 时预构建，launch 时主要复用这些命令。这样降低重复构建 kernel/memcpy command 的开销，但会增加：

- Graph instantiate 时间；
- command buffer 和同步内存资源占用；
- 参数更新后的 dirty submission 重建成本。

### 15.2 动态控制流的 host 往返

conditional graph 的每次判断至少包含：

```text
前序 GPU submission
    → semaphore signal
    → host wait
    → host 读取 mapped memory
    → Driver 分支判断
    → 提交 child cluster
```

WHILE 每次循环还会重复这一流程。因此小粒度循环可能受到 host wait、线程调度和重新提交成本影响。

### 15.3 静态 DAG 并行性受 submission 边界影响

无依赖节点理论上可以并行，但以下因素会收缩并行空间：

- 不同 node type 被拆到不同 submission；
- child graph/conditional 强制形成边界；
- private semaphore 串行连接内部 submission；
- host-device 节点需要进入 Driver 逻辑；
- external semaphore 节点要求独立 submission。

因此性能分析需要分别观察：

```text
Graph instantiate
Graph launch enqueue
GraphCommand host submission
HAL queue submit
Node device execution
Conditional host wait / branch overhead
```

不能将 `muGraphLaunch()` API 时间直接等同为所有 Graph node 的设备执行时间。

## 16. 当前实现边界与待验证项

### 已由源码确认

- Graph 是显式 DAG；
- GraphExec v1 使用 Kahn 拓扑排序和环检测；
- child graph/conditional branch 被递归解析为独立 submission cluster；
- DAG 依赖通过 engine sync memory 的 acquire/release 落到 HAL；
- conditional handle 是 host-mapped internal memory，GPU 可写、Driver 可读；
- IF 按非零/零选择 child graph；
- SWITCH 将值作为 child index，越界走 empty callback；
- WHILE 通过 Driver 重复递归执行 body cluster；
- stream capture 使用 `m_LastCapturedNodes` 维护 frontier；
- disabled node 通过 NOP 保留拓扑位置。

### 仍需专项验证

1. WHILE 初始条件为 0 时，body 是否按正式 API 语义必须执行一次；
2. `GetContionalValue()` 读取 host-mapped memory 时的底层 cache/coherence 保证；
3. GraphExec2 对 conditional node 的完整支持范围；
4. nested IF/WHILE/SWITCH 中 conditional handle reset 的边界行为；
5. conditional branch 中 host、host-device、external semaphore 节点的组合行为；
6. `CmdGraph()` 等待失败后递归调用结果对错误状态的覆盖关系；
7. conditional graph 作为最后节点时 empty callback 对外部 signal 的完整性；
8. 跨 stream capture 与 event frontier 更新的全部路径。

## 17. 源码和测试证据索引

### 关键实现

```text
src/driver/mu_graph.cpp
  muapiGraphInstantiate_v2
  muapiGraphLaunch
  muapiGraphConditionalHandleCreate

src/musa/core/context.cpp
  Context::CreateGraphExec
  Context::CreateConditionalHandle
  Context::CreateConditionalNode

src/musa/core/graph.cpp
  Graph::AddNode
  Graph::AddEdge
  Graph::AddGraphNode
  Graph::GetRootNodes
  Graph::GenLeafNodes
  Graph::GetInternalGraphsImpl
  Graph::GetPerfGraphExecVersion

src/musa/core/graph/graph1/graphExec.cpp
  GraphExec::Init
  GraphExec::ResolveGraph
  GraphExec::PrepareAllSubmissions
  GraphExec::NeedNewSubmission
  GraphExec::WriteSubmission
  GraphExec::CmdAcquire
  GraphExec::CmdRelease

src/musa/core/command/graphCommand.cpp
  GraphCommand::Submit
  GraphCommand::Execute
  GraphCommand::ExecuteImpl
  GraphCommand::ResetConditionalHandle

src/musa/core/graph/graph1/universalManager.cpp
  UniversalManager::CmdGraph
  UniversalManager::Submit
  UniversalManager::GetContionalValue

src/musa/core/stream.cpp
  Stream::BeginCapture
  Stream::EndCapture
  Stream::CmdLaunchGraph
  Stream::CaptureNode
  Stream::GetCaptureInfo
  Stream::AddLastCapturedNodes
  Stream::SetLastCapturedNodes
```

### 测试样例

```text
tests/conditionalNode.cu
  ifElseGraph / ifElseConditional
  whileGraph / whileConditioanl
```

其中 IF 样例覆盖 true/false branch；WHILE 样例展示前序 kernel 初始化 handle、body 更新数据并递减 handle 的模式，但没有单独证明初始值为 0 的语义。

## 18. 总结

MUSA Graph 的控制流实现可以归纳为两条链：

```text
静态链：
Graph DAG
  → Kahn 拓扑排序
  → MusaKick / MusaSubmission
  → waitValues / signalValue
  → HAL CmdAcquire / CmdRelease

动态链：
conditional handle
  → 前序 semaphore wait
  → CPU 读取 host-mapped 值
  → IF/SWITCH 选择 child cluster
  → WHILE 重复递归执行 body cluster
```

这种设计保留了 Graph instantiate 对设备命令的预构建优势，同时用 Driver 运行时逻辑实现动态分支和循环。它的主要代价是 conditional graph 需要 host 参与判断，尤其是 WHILE 会在每轮循环后等待、读取并重新提交 body，因此控制流粒度和 host 调度开销会直接影响实际性能。
