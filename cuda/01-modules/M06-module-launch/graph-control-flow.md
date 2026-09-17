# CUDA Graph 控制流实现

- 文档目的：单独说明 CUDA Graph 如何把 API 操作转换为可验证、可调度、可重复执行的控制流计划，以及该计划如何在 launch 时驱动 host/device execution。
- 适用范围：`source/cuda/src/api`、`source/cuda/src/cui/cuigraph.c` 和相关 stream/marker/UVM 代码的 host-side 静态分析。
- 证据状态：Graph 节点、依赖、拓扑排序、launch mode、QMD chaining、pushbuffer 和 scheduler backing 的控制流为静态确认；device scheduler kernel、firmware、GPU 实际执行顺序和性能未验证。
- 对应源码版本：见 [`00-overview/analysis-state.md`](../../00-overview/analysis-state.md)。
- 前置阅读：[M06 Module/Launch](README.md)、[Graph 资源生命周期](graph-resource-lifecycle.md)。

## 1. 核心结论：Graph 是控制流计划

CUDA Graph 的实现不是简单地缓存一组 kernel 参数，而是把控制流拆成三个阶段：

```text
Capture
  = 将 API 操作记录为节点和依赖边

Instantiate
  = 验证、展开、规范化并编译执行计划

Launch
  = 将已编译计划接入 API stream，按依赖驱动节点提交
```

控制流的核心数据结构可以抽象为：

```text
CUIgraph
  ├─ CUIgraphNode 链表
  ├─ predecessor / successor 集合
  ├─ parentConditional / conditional body
  ├─ node->ctx
  ├─ schedulingGroup
  ├─ launchMode
  └─ graphExec per-context data
       ├─ QMD / constant-bank
       ├─ internal stream / completion marker
       ├─ scheduler host/device backing
       └─ Context lock set
```

节点之间的边决定“何时允许执行”，节点的 `launchMode` 决定“由哪一种执行机制驱动”。这两个概念必须区分：依赖边表达逻辑控制流，launch mode 表达该控制流的物理实现。

## 2. Capture：从 API 副作用转为节点

### 2.1 Begin capture

```text
cuStreamBeginCapture
  → API 检查 mode、stream 和 null-stream 约束
  → Context lock
  → 创建 CUIgraph
  → 保存 origin stream、capture mode、TLS/event state
  → 标记 stream 为 capturing
```

capture graph 在这一阶段主要是 host-side DAG。它保存的是未来执行所需的操作描述，而不是已编码的 GPFIFO entry。

### 2.2 Capture 中的 kernel 操作

```text
cuLaunchKernel
  → cuapiLaunchKernelCommon
  → cuiValidateLaunch
  → cuiStreamIsCapturing(stream)
  → graph lock
  → 读取 stream.capture.nextPushDeps
  → 创建 kernel node
  → 保存 function、参数、grid/block、shared memory 和 stream
  → 添加 predecessor/successor edges
  → 更新 capture event/dependency state
```

因此 capture 分支不会进入普通 launch 的：

```text
streamBeginPush
  → streamEndPush
  → GPFIFO publish
```

如果 capture 中的 API 不支持、Context 不一致或依赖关系无效，Graph 会被标记 invalidated；`cuStreamEndCapture` 会返回错误并销毁失败 graph，而不是产生部分可执行 graph。

### 2.3 End capture

```text
cuStreamEndCapture
  → 必须从 origin stream 结束
  → 检查 capture invalidation
  → 检查多 stream 是否 joined
  → 检查 end-capture dependency 可达性
  → 成功：返回 external graph handle
  → 失败：销毁 graph，返回 NULL/error
```

`origin stream` 是控制流入口的一部分；它使 driver 能判断 capture 是否完整闭合，而不是仅根据节点数量判断成功。

## 3. Graph 节点如何表达控制流

### 3.1 显式依赖边

对普通节点，边表示：

```text
A → B
```

含义是 B 的执行资格依赖 A 的 completion。一个节点可以有多个 predecessor 和 successor，因此可以表达：

```text
串行：A → B → C
分叉：A → B，A → C
汇聚：B → D，C → D
```

这些边首先是 host graph 的逻辑关系，instantiate 后才决定采用 marker wait、pushbuffer 顺序、QMD chaining 或 device scheduler。

### 3.2 Nested graph flatten

`cuiGraphFlatten` 递归处理 child graph 和 conditional body：

```text
outer node
  → recursively flatten child graph
  → reparent child nodes to root graph
  → copy clone map
  → connect child sources to outer predecessors
  → connect child sinks to outer successors
  → remove ordinary child-graph wrapper
```

对于普通 child graph，入口边连接到 child source，child sink 连接到外层 successor，从而把嵌套控制流变为单一 DAG。对于 conditional body，则保留 `parentConditional` 关系，供后续条件控制和 device scheduler 使用。

### 3.3 Memset lowering

instantiate 前，memset 节点会根据 HAL 能力判断是否应转换成 kernel 节点：

```text
memset node
  → cuiMemsetPrepare
  → inline / copy-engine path
      或
  → hal.memsetKernel
  → 创建一个或多个 kernel nodes
  → 复制原节点的 predecessors/successors
  → 保存 original node mapping
  → 删除原 memset node
```

这是一种控制流规范化：上层仍看到 memset 语义，但执行计划可以统一使用 kernel scheduling 和 QMD 机制。

## 4. Instantiate：把 DAG 编译成执行计划

### 4.1 总体控制流

```text
cuiGraphInstantiate
  → cycle detection
  → conditional validation
  → clone graphExec
  → create ctxData hash
  → flatten child/conditional graphs
  → convert eligible memset nodes
  → assign/register node contexts
  → cuiGraphSetupScheduling
  → allocate per-context execution resources
  → acquire Context locks
  → instantiate nodes under locks
  → register QMD semaphore pools
```

实例化的产物是 `graphExec`，不是原始 capture graph 的可变别名。原 graph 负责描述 API 录制结果，graph exec 负责保存可反复 launch 的执行资源和调度决策。

### 4.2 Context 归属是控制流的一部分

节点必须先确定执行 Context：

```text
kernel node
  → function->mod->ctx

memcpy node
  → source/destination owning Context
  → 必要时选择 pull Context

host/empty node
  → current Context

conditional node
  → body node Context
```

之后通过 `cuiGraphRegisterCtx` 把 Context 放进 `graphExec->ctxData`。这决定了：

- 哪个 Context 持有节点资源；
- launch 时哪些 Context 必须加锁；
- 是否能直接使用 QMD chaining；
- 是否必须跨 stream/Context 建立 marker 依赖。

### 4.3 拓扑排序

`cuiGraphSetupScheduling` 使用依赖计数进行拓扑处理：

```text
初始化每个 node 的 unsortedDependencies
  → 找到依赖数为 0 的 roots
  → 放入 sorted list
  → 处理 node
  → 减少 successor 的依赖计数
  → 新的 0-dependency successor 入队
  → 重复直到所有节点排序
```

conditional body 还会获得指向 parent conditional 的 fake dependency，用于保证 body 不会在条件控制节点被处理前进入错误的执行阶段。最终 `graph->nodesHead/tail` 被替换为拓扑顺序。

拓扑排序只决定合法的逻辑先后；它不等于所有节点都被串行执行。多个 root 或多个可运行 successor 仍可以在执行计划中并行。

## 5. Launch mode：逻辑边到物理控制流

### 5.1 Pushbuffer launch

节点或边必须使用 pushbuffer 的典型原因：

```text
节点不是 kernel
使用 CNP，grid QMD completion 不足以代表完整完成
前后节点属于不同 Context
禁用 QMD chaining
设备不支持 QMD chaining
复杂分叉/汇聚或 scheduler 边
```

`cuiGraphEdgeMustUsePushbuf` 对这些条件进行判断。pushbuffer 路径由 host-side driver 按拓扑和 marker 组织 packet，并在 channel 上提交。

### 5.2 QMD chaining

当相邻节点满足较强约束时，后继可使用 QMD chaining：

```text
前后节点都是 kernel
同一 Context
前驱没有要求 pushbuffer
设备支持 QMD chaining
只有一个合适的前驱/后继关系
```

此时控制流主要由设备读取下一个 QMD 实现，而不是每个节点都由 host 单独发布 pushbuffer。QMD chaining 减少 host 提交开销，但受 completion 语义和硬件能力限制。

### 5.3 Device scheduler launch

以下情况可能选择 scheduler：

- conditional body 的入口节点；
- 存在复杂依赖或多分支；
- 需要由设备判断 successor 是否可运行；
- 节点不能安全地通过简单 QMD chain 串起来。

instantiate 会生成 scheduler node、device node index 和依赖索引。`cuiGraphCreateSchedulerNode` 使用 Context 中的 graph scheduler function，之后在 per-context execution resources 中建立 host/device backing。

设备 scheduler 的抽象控制流是：

```text
scheduler backing
  → predecessor counters / successor indexes
  → device observes node completion
  → decrements or evaluates dependency state
  → selects runnable device node
  → launches next QMD/kernel
```

这段 device-side 执行只能由当前源码确认其 host-side backing、索引和 kernel 参数准备；实际 GPU 调度行为仍未验证。

### 5.4 Launch mode 选择算法

`cuiGraphSetupScheduling` 先把节点分配到 scheduling group，再为尚未确定的节点选择模式：

```text
pushbuffer root
  → 标记 scheduling group root

conditional body root
  → scheduler launch

单一 kernel-to-kernel 依赖且条件满足
  → QMD chaining

其余需要设备判断的后继
  → scheduler launch

scheduler 需要从 kernel 后继续接管
  → 插入 scheduler node after
```

同时，非 sink kernel 可能禁用 semaphore release，避免每一个内部节点都产生多余的 release；真正需要将完成传播给下游的边才保留相应同步动作。

## 6. Scheduling group 和控制流压缩

scheduling group 把可以连续处理的节点组合起来：

```text
root node
  → group.next
  → group.next
  → ...
```

组内 kernel 可由 `cuiGraphLaunchKernelGroup` 一次完成：

```text
设置每个 function 的 gridID/currentLaunchData
  → 建立 group 级 memory tracking
  → streamBeginPush
  → 写入多个 kernel 的初始化状态
  → QMD/constant-bank/launch packet
  → streamEndPush
```

组并不改变原始逻辑依赖；它只是把多个合法连续节点压缩到同一 host push sequence。若依赖边要求独立 completion、跨 Context 或 device scheduler，节点不会被错误合并。

QMD cache 还会限制连续 serial kernel 的缓存数量；达到 cache size 后拆分 scheduling group，避免执行计划假定超过硬件缓存容量。

## 7. Launch：控制流如何真正被驱动

### 7.1 Graph launch 入口

```text
cuGraphLaunch
  → validate graphExec and API stream
  → collect/lock graph Contexts
  → wait previous graph completion where needed
  → initialize scheduler backing on first launch
  → acquire per-launch parameter bank
  → cuiUvmDagSetRunning
  → process nodes in instantiate order
```

如果 API launch stream 的 Context 不在 graph 静态 Context 集合中，launch 会临时增加一把 Context lock；这只影响 host-side 并发保护，不改变图的逻辑依赖。

### 7.2 根节点

root node 没有图内 predecessor，但仍可能依赖 API stream 之前的工作：

```text
API stream marker
  → root internal stream wait
  → root node push/scheduler launch
```

因此 Graph launch 不是脱离 API stream 的独立队列；它必须把 graph 的第一批节点接到用户提交顺序上。

### 7.3 中间节点

中间节点根据 launch mode 执行：

```text
pushbuffer node
  → host waits predecessor marker as needed
  → emit node commands

QMD-chained node
  → predecessor QMD completion selects next QMD

scheduler node
  → device-visible dependency state
  → scheduler selects runnable successor
```

跨 internal stream 的依赖通过 `cuiStreamWaitMarker` 或等价 marker 机制表达。这样，逻辑边最终对应一种实际的 completion/wait 机制。

### 7.4 Sink 节点和 Graph completion

sink 节点完成后，Graph 将各 Context/internal stream 的完成状态汇聚到 graph completion marker：

```text
leaf node completion
  → per-node/per-stream marker
  → per-context aggregation
  → graph completion marker
  → API stream observes graph completion
```

completion marker 是 Graph 与后续 API 操作、Graph destroy、资源 reclaim 之间的边界。提交成功仅表示 host-side push/GPFIFO 已建立，不表示 sink 已由 GPU 执行完成。

## 8. Conditional 控制流

conditional graph 不能仅用普通 DAG 边表达，因为 body 是否执行取决于运行时条件。实现上可观察到以下 host-side准备：

```text
validateConditionals
  → preserve parentConditional/body relationship
  → assign body Context
  → add fake ordering dependencies
  → mark conditional/device nodes
  → allocate device-node indexes and dependency arrays
  → insert scheduler node where required
```

运行时意图是：

```text
conditional node
  → evaluate condition / scheduler state
  → execute body roots if enabled
  → propagate body sinks to successor
```

源码能够证明 conditional body 的结构、索引和 scheduler 资源准备；条件值如何由设备读取、设备 scheduler 的具体分支指令和 body 的最终执行行为属于设备侧未验证边界。

## 9. Update 为什么不能任意改变控制流

`cuGraphExecUpdate` 使用 per-graph-node ID 对齐新旧 graph，并检查：

- topology；
- node type；
- function/context；
- memcpy/memset 形状和 memory type；
- host callback 等节点配置；
- 资源形状是否仍兼容。

参数更新可以保留原来的执行计划；如果更新改变了拓扑、节点类型或 QMD/constant-bank/scheduler 需求，就不能只改参数，必须返回 update failure。其原因是 graph exec 已经编译了控制流到 launch mode、资源数量和依赖索引。

## 10. 错误和控制流完整性

```text
capture unsupported/invalid
  → invalidate graph
  → end capture returns error
  → destroy partial graph

cycle detected
  → instantiate stops before execution plan

flatten/memset/context registration failure
  → destroy partial clone/resources

scheduling failure
  → no valid graph exec

scheduler/QMD registration failure
  → instantiate cleanup path

node launch failure
  → rollback UVM running state
  → release temporary lock/stream state
  → do not report graph completion
```

两个重要的静态风险仍待运行验证：

1. QMD semaphore pool 注册失败发生在 `graphExec->ctxLocks` 保存之后，统一错误路径中的 lock-array 生命周期需要注入失败验证。
2. Graph launch 替换 API stream 为临时 stream 后，节点中途失败时的恢复路径需要专门验证。

## 11. 控制流、数据流和资源流的区别

| 流程 | 表示什么 | 主要载体 |
|---|---|---|
| 控制流 | 节点何时可运行、走哪条边 | dependency edges、conditional、launch mode |
| 数据流 | kernel/memcpy 读写哪些资源 | memobj、memcpy operands、memory tracking |
| 提交流 | 命令如何送达设备 | pushbuffer、QMD、GPFIFO、DMAL |
| 完成流 | 何时可认为节点/图结束 | semaphore、marker、UVM DAG |
| 所有权流 | 何时可释放资源 | Context/graph ownership、tracking、destroy |

Graph 的实现价值在于将控制流和提交流分离：同一逻辑 DAG 可以根据 Context、CNP、硬件能力和分支复杂度选择不同的提交实现。

## 12. 证据边界和验证计划

### 静态确认

- capture 创建 graph node 和 dependency edges；
- end capture 的 origin/invalidation/unjoined 检查；
- clone/flatten 和 child graph 边重连；
- memset 到 kernel node 的 lowering；
- 拓扑排序、scheduling group 和 launch mode 选择；
- QMD chaining、pushbuffer 和 scheduler backing 的 host-side 准备；
- marker、UVM DAG 和 graph destroy 的 host-side 关系。

### 未验证

- device scheduler kernel 的实际分支与调度顺序；
- conditional body 的 GPU 运行结果；
- QMD chaining 在目标架构上的实际硬件行为；
- Graph completion latency 和错误注入；
- graph 专用 DVS/GPU 测试。

建议的专用验证用例：

```text
A→B 串行
A→B/C 分叉
B/C→D 汇聚
多 root / 多 sink
nested child graph
conditional body
memset lowering
CNP kernel edge
跨 Context graph
update topology failure
destroy-after-launch
```

## 13. 相关文档

- [M06 Module/Launch](README.md)
- [Graph 资源生命周期](graph-resource-lifecycle.md)
- [Kernel Launch 端到端调用链](../../90-cross-module/end-to-end-kernel-launch.md)
- [具体架构设计](../../90-cross-module/architecture-design.md)
- [源码证据索引](../../00-overview/evidence-index.md)
