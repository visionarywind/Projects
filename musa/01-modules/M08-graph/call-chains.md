# Graph 执行：调用链

- 文档目的：graph：关键调用链：从公共入口到真实副作用的追踪。
- 适用范围：M08 模块的《Graph 执行：调用链》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


## 主链 A：capture（流捕获建图）

```text
muapiStreamBeginCapture_v2 [src/driver/mu_stream.cpp:391-421]
 -> TlsCtxTop 取 current context，InfoStream 解析句柄
 -> pContext->CreateGraph 新建空图
 -> pStream->BeginCapture(pGraph, mode) [src/musa/core/stream.cpp:45-73]
    置 m_CaptureStatus = ACTIVE、记 m_CaptureGraph/m_BeginThreadId、登记 platform capturedStreams
 -> 失败时 driver 侧回滚 DestroyGraph [src/driver/mu_stream.cpp:410-415]
 -> 后续命令 API 走 Context::GeneralLaunch* 的 capture 分支 -> Stream::CaptureNode
 -> muapiStreamEndCapture [src/driver/mu_stream.cpp:473-501]
 -> Stream::EndCapture 校验 origin 线程/状态/leaf 后返回 m_CaptureGraph [src/musa/core/stream.cpp:75-139]
```

节点入图的两条入口：kernel 走 `Context::GeneralLaunchKernel` 的三分支（capture -> CaptureNode、invalidated -> 报错、否则 -> CmdLaunchKernel），memcpy 走同一形态的分支。[src/musa/core/context.cpp:666-674] [src/musa/core/context.cpp:722-730]

`Stream::CaptureNode` 把 `m_LastCapturedNodes` 当作新节点的依赖，随后用自身替换该列表，形成默认的链式依赖。[src/musa/core/stream.cpp:673-694]

## 主链 B：手动建图（add node / add dependency）

```text
muapiGraphAddNode [src/driver/mu_graph.cpp:1012-1069]
 -> 按 nodeParams->type 分派：13 类有实现，其余返回 MUSA_ERROR_NOT_SUPPORTED
 -> 单类入口（如 muapiGraphAddKernelNode_v2）做参数/句柄/依赖校验 [src/driver/mu_graph.cpp:778-840]
 -> pContext->CreateXxxNode（如 CreateKernelNode [src/musa/core/context.cpp:2230-2238]）
 -> pGraph->AddGraphNode -> AddNode 建顶点、AddEdge 建边并维护入度出度 [src/musa/core/graph.cpp:245-272]
 -> 失败时调用方 delete pGraphNode 回滚 [src/driver/mu_graph.cpp:827-830]
```

独立的依赖接口 `muapiGraphAddDependencies` / `muapiGraphRemoveDependencies` 直接落到 `Graph::AddDependencies` / `Graph::RemoveDependencies`。[src/driver/mu_graph.cpp:269-294] [src/driver/mu_graph.cpp:3147-3172]

## 主链 C：instantiate

```text
muapiGraphInstantiate_v2 [src/driver/mu_graph.cpp:2304-2324]
 -> pContext->ValidateGraph + 空指针检查
 -> Context::CreateGraphExec [src/musa/core/context.cpp:1420-1450]
    -> HasUnboundConditionalHandle 未绑定则 MUSA_ERROR_INVALID_VALUE
    -> graphUserQ && supportEngineSync && GetPerfGraphExecVersion()==2 -> GraphExec2
    -> 否则 -> GraphExec
    -> graphExec->Init(pGraph) 失败则 delete 后返回
 -> 登记 ctxCrit->AddGraphExec
v1 Init = CloneGraph + ResolveGraph + CreateExecResource + PrepareAllSubmissions
   [src/musa/core/graph/graph1/graphExec.cpp:709-724]
v2 Init = CloneGraph + GraphFlatten
   [src/musa/core/graph/graph2/graphExec.cpp:362-371]
```

## 主链 D：launch（v1）

```text
muapiGraphLaunch [src/driver/mu_graph.cpp:2374-2401]
 -> InfoStream 解析 stream，ValidateGraphExec
 -> Stream::CmdLaunchGraph 的 v1 分支 [src/musa/core/stream.cpp:281-292]
 -> new GraphCommand(this, GraphExec*)，SetGraphID
 -> Context::ResolveDependencyAndQueueCommand 入队
 -> GraphCommand::Submit -> Execute -> ExecuteImpl(0) [src/musa/core/command/graphCommand.cpp:280-431]
 -> UniversalManager::CmdSubmission/CmdCallback/CmdHostDevice/CmdGraph [src/musa/core/graph/graph1/universalManager.cpp:73-104]
 -> UniversalManager::Submit 真正下发
 -> GraphExec::SetCommand 记录 weak_ptr 并把 graph alloc 内存挂到该 command [src/musa/core/graph/graph1/graphExec.cpp:2095-2100]
```

## 主链 E：launch（v2）

```text
muapiGraphLaunch -> Stream::CmdLaunchGraph 的 v2 分支 [src/musa/core/stream.cpp:293-414]
 -> SetDefaultStream(this)，把本流未完成的 last command 透传给内部 launch stream
 -> 按 GraphExec2::GetBfsNodes 顺序逐节点：
      跨 stream 父依赖 -> SetCurrentDependencies
      按节点类型调用 CmdLaunchKernel/CmdCopyMemory/CmdMemset/AsyncSetEvent...
 -> 结束后把各内部 stream 的最后命令 join 回调用 stream
```

v2 分支里 `MU_GRAPH_NODE_TYPE_MEM_ALLOC`/`MU_GRAPH_NODE_TYPE_MEM_FREE`/`MU_GRAPH_NODE_TYPE_CONDITIONAL` 的 body 是空实现，`MU_GRAPH_NODE_TYPE_EXT_SEMAS_SIGNAL`/`MU_GRAPH_NODE_TYPE_BATCH_MEM_OP` 落到 `MUSA_ERROR_NOT_SUPPORTED`。[src/musa/core/stream.cpp:362-392]

## 主链 F：update

```text
muapiGraphExecUpdate [src/driver/mu_graph.cpp:1638-1654]
 -> IGraphExec::Update
 v1: 比较节点数/类型/父节点类型，全等才 m_Submissions.clear() + 重新 Init
     [src/musa/core/graph/graph1/graphExec.cpp:654-707]
 v2: 同样的拓扑比较后重新 Init [src/musa/core/graph/graph2/graphExec.cpp:308-360]
```

## 主链 G：exec 节点参数就地更新

```text
muapiGraphExecNodeSetParams [src/driver/mu_graph.cpp:1071-1155]
 -> 按类型转 IGraphExec::SetXxxNodeParams
 -> 在 m_Graph 的 m_MapCloneByOriginal 里找克隆节点 -> 命中 kick 后 UpdateParams
 -> child graph 额外重跑 ResolveGraphImpl 并标 isDirty
    [src/musa/core/graph/graph1/graphExec.cpp:460-513]
muapiGraphNodeSetEnabled [src/driver/mu_graph.cpp:2982-3000]
 -> GraphExec::SetNodeEnabled 改 kick.flags.isEnable，DEVICE 类型同时置 isDirty
    [src/musa/core/graph/graph1/graphExec.cpp:598-626]
```

## 主链 H：destroy

```text
muapiGraphDestroy -> Context::DestroyGraph：摘登记 -> LockedWait -> delete graph
   [src/musa/core/context.cpp:1618-1659]
muapiGraphExecDestroy [src/driver/mu_graph.cpp:1364-1383]
 -> ValidateGraphExec -> Context::DestroyGraphExec
    -> (v2) ReleaseStream -> RemoveGraphExec -> LockedWait -> delete
    [src/musa/core/context.cpp:1690-1738]
GraphExec 析构对未完成的 GraphCommand 执行 Wait [src/musa/core/graph/graph1/graphExec.cpp:37-41]
```

## 相关证据

- capture 状态计数：`s_BeginCaptureNum`/`t_BeginCaptureNum` 定义于 [src/musa/core/stream.cpp:39-41]
- 依赖合法性校验（空、非本图、重复）：[src/musa/core/graph.cpp:494-511]
- 环检测：`outQueueNumber != nodes.size()` 判环，位于 [src/musa/core/graph/graph1/graphExec.cpp:1290-1294]
- 条件值读取：`GetContionalValue` 先等待 semaphore 再读宿主映射内存，见 [src/musa/core/graph/graph1/universalManager.cpp:1049-1087]

## 链路缺口

`UniversalManager::Submit` 之后的 HAL 内部排队（`src/hal/m3d`）本模块未逐行读取，只能确认接口边界；`Context::ResolveDependencyAndQueueCommand` 的跨流依赖合并属于 M06，本页只确认它在 v1 分支被调用。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/driver/mu_graph.cpp`（11 处，例：[src/driver/mu_graph.cpp:1012-1069]）
- `src/musa/core/stream.cpp`（7 处，例：[src/musa/core/stream.cpp:45-73]）
- `src/musa/core/graph/graph1/graphExec.cpp`（7 处，例：[src/musa/core/graph/graph1/graphExec.cpp:709-724]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
