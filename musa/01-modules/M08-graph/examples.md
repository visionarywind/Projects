# Graph 执行：例子

- 文档目的：给出可对照源码阅读的最小调用序列、边界输入与错误输入
- 适用范围：M08 模块的《Graph 执行：例子》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：**全部为静态推导骨架，未在本环境编译或运行**；仓库内没有已验证的 graph 示例程序
- 最后更新：2026-10-07
- 前置阅读：`interfaces.md`、`call-chains.md`
- 后续阅读：`testing.md`、`development-guide.md`

## 0. 仓库内示例现状

- 知识库 `80-demos/D03-graph` 只登记了候选项（`tests/childGraph.cu`、`tests/conditionalNode.cu`、`tests/allocnode.cu`），并标注「当前未逐行读取和运行」。
- 对当前源码快照做静态检查：仓库根目录下不存在 `tests/`、`unittest/` 或 `examples/` 目录，因此**没有可直接引用的仓库内 graph 示例**（静态分析，未验证）。[CMakeLists.txt:167-169]
- 下面所有例子都是按 `muapi*` 实现反推的调用序列，标注为「假设示例」。

## 1. 最小手动建图（假设示例，未运行）

```text
muapiGraphCreate(&g, 0)                                  创建空图，flags 必须为 0
  muapiGraphAddKernelNode_v2(&n, g, NULL, 0, &kp)        加一个无依赖的 kernel 节点
  muapiGraphInstantiate_v2(&exec, g, NULL, NULL, 0)      实例化，内部克隆图
  muapiGraphLaunch(exec, s)                              在 stream s 上提交
  muapiGraphExecDestroy(exec)                            释放执行图
  muapiGraphDestroy(g)                                   释放原图
```

逐步依据：

| 步骤 | 关键约束 | 源码位置 |
|---|---|---|
| `muapiGraphCreate` | `phGraph` 为空或 `flags != 0` 都返回 `MUSA_ERROR_INVALID_VALUE`；随后取 current context | [src/driver/mu_graph.cpp:1188-1208] |
| `muapiGraphAddKernelNode_v2` | `func` 与 `kern` 同时为空返回 `MUSA_ERROR_INVALID_HANDLE`；依赖列表为空时允许传 `NULL` | [src/driver/mu_graph.cpp:778-804] |
| `muapiGraphInstantiate_v2` | 必须有 current context，且 `ValidateGraph` 通过，`phGraphExec` 非空 | [src/driver/mu_graph.cpp:2304-2324] |
| `muapiGraphLaunch` | `hGraphExec` 空、stream 解析失败、`ValidateGraphExec` 失败分别返回不同错误码 | [src/driver/mu_graph.cpp:2374-2401] |
| `muapiGraphExecDestroy` | 校验后进 `DestroyGraphExec`，内部先同步再删除 | [src/driver/mu_graph.cpp:1364-1383] |
| `muapiGraphDestroy` | 校验失败返回 `MUSA_ERROR_ILLEGAL_STATE`，成功才 `DestroyGraph` | [src/driver/mu_graph.cpp:1210-1232] |

kernel 节点参数里的 `func` 句柄来自 M07：`muapiModuleGetFunction` 解析符号后返回 `MUfunction`。[src/driver/mu_module.cpp:157-178]

## 2. capture 建图（假设示例，未运行）

```text
muapiStreamBeginCapture_v2(s, mode)     建空图并把 stream 置为 ACTIVE
  ... 在 s 上提交普通命令，命令被转成节点 ...
muapiStreamEndCapture(s, &g)            校验后拿回图
muapiGraphInstantiate_v2(&exec, g, ...)
muapiGraphLaunch(exec, s2)
```

- `muapiStreamBeginCapture_v2` 先 `CreateGraph` 再 `BeginCapture`，失败时回滚销毁刚建的图。[src/driver/mu_stream.cpp:391-421]
- `muapiStreamEndCapture` 在无 current context、stream 为空、stream 解析失败时分别返回 `MUSA_ERROR_INVALID_CONTEXT`/`MUSA_ERROR_ILLEGAL_STATE`/`MUSA_ERROR_INVALID_VALUE`。[src/driver/mu_stream.cpp:473-489]
- 节点入图靠 `Stream::CaptureNode`，它把 `m_LastCapturedNodes` 当作默认依赖。[src/musa/core/stream.cpp:673-694]

## 3. 查询节点数（两段式，假设示例）

`muapiGraphGetNodes` 支持 `nodes == nullptr` 的探测调用：第一次传 `NULL` 拿到节点总数，再按该数量分配数组调用第二次。[src/driver/mu_graph.cpp:1854-1891]

- 探测路径写的是 `*numNodes = Inodes.size()`，第二次调用时若数组更大，多余槽位会被补 `nullptr` 并把 `*numNodes` 收缩为真实节点数。
- 校验失败分支把局部指针置空，调用方的 `*numNodes` 不会被改写（静态阅读结论，未验证）。

## 4. 导出 DOT 做结构诊断

`muapiGraphDebugDotPrint(hGraph, path, flags)` 校验句柄后调用 `Graph::DebugDotPrint`，把每个节点写成带属性的 DOT 顶点，把边写成有向边。[src/driver/mu_graph.cpp:3208-3232]

- kernel 节点的 label 使用 `Function::GetName()` 拼出 kernel 名。[src/musa/core/graph.cpp:369-373]
- memcpy 节点会带上方向（HtoH/HtoD/DtoH/DtoD）与字节数。[src/musa/core/graph.cpp:375-395]
- 这是只读副作用（写文件），可用来在没有 GPU 的情况下核对拓扑（未运行验证）。

## 5. 依赖操作（假设示例，未运行）

```text
muapiGraphAddDependencies(g, &from, &to, 1)
muapiGraphRemoveDependencies(g, &from, &to, 1)
```

- `muapiGraphAddDependencies` 依次做 `InitPlatform`、空指针检查、`TlsCtxTop`、`ValidateGraph`，再交给 `Graph::AddDependencies`。[src/driver/mu_graph.cpp:269-294]
- `Graph::AddDependencies` 拒绝自依赖（`from[i] == to[i]`）与任一侧不在 `m_Vertices` 的节点，统一返回 `MUSA_ERROR_INVALID_VALUE`。[src/musa/core/graph.cpp:274-297]
- `muapiGraphAddDependencies_v2` 忽略 `edgeData`，直接转发到 v1 版本。[src/driver/mu_graph.cpp:296-304]

## 6. 边界与错误输入

| 输入 | 预期行为（静态分析） | 源码位置 |
|---|---|---|
| `muapiGraphCreate(&g, 1)` | `flags` 非 0 直接 `MUSA_ERROR_INVALID_VALUE` | [src/driver/mu_graph.cpp:1188-1192] |
| 无 current context 调用任一入口 | 返回 `MUSA_ERROR_INVALID_CONTEXT` | [src/driver/mu_graph.cpp:791-795] |
| 加依赖时 `from == to` | `MUSA_ERROR_INVALID_VALUE` | [src/musa/core/graph.cpp:287-291] |
| 依赖节点不属于本图 | `MUSA_ERROR_INVALID_VALUE` | [src/musa/core/graph.cpp:288-291] |
| 重复依赖同一父节点 | `ValidateDependencies` 用 `unordered_set` 去重命中即失败 | [src/musa/core/graph.cpp:494-511] |
| 图中存在环 | `ResolveGraphImpl` 判定 `outQueueNumber != nodes.size()`，清空 submissions 并返回 `MUSA_ERROR_INVALID_VALUE` | [src/musa/core/graph/graph1/graphExec.cpp:1291-1294] |
| 图含未绑定 conditional handle | `CreateGraphExec` 直接返回 `MUSA_ERROR_INVALID_VALUE` | [src/musa/core/context.cpp:1423-1426] |
| `muapiGraphUpload` | 恒返回 `MUSA_ERROR_NOT_SUPPORTED` | [src/driver/mu_graph.cpp:2403-2405] |
| `muapiGraphAddNode` 传未实现的 5 类 type | 落到 default 分支返回 `MUSA_ERROR_NOT_SUPPORTED` | [src/driver/mu_graph.cpp:1062-1065] |
| update 时节点数/类型/父节点不一致 | 返回 `MU_GRAPH_EXEC_UPDATE_ERROR_TOPOLOGY_CHANGED` | [src/musa/core/graph/graph2/graphExec.cpp:308-360] |

统一入口只 switch 了 13 个 case，`MU_GRAPH_NODE_TYPE_BATCH_MEM_OP`、`MU_GRAPH_NODE_TYPE_MEM_ATOMIC`、`MU_GRAPH_NODE_TYPE_MEM_ATOMIC_VALUE`、`MU_GRAPH_NODE_TYPE_MEM_WAIT_WRITE`、`MU_GRAPH_NODE_TYPE_MEM_TRANSFER` 这 5 类落到 default。[src/musa_shared_include/musa.h:1977-1999]

这 5 类各有独立入口（如 `muapiGraphAddBatchMemOpNode`、`muapiGraphAddMemAtomicNode`），只是走不了统一入口。[src/driver/mu_graph.cpp:2450-2795]

## 7. 与 M07 协作（假设示例，未运行）

```text
muapiModuleLoad(&m, "app.mubin")
muapiModuleGetFunction(&f, m, "kernel_name")   -> MUfunction
muapiGraphAddKernelNode_v2(&n, g, NULL, 0, &kp)  其中 kp.func = f
muapiGraphInstantiate_v2(...) / muapiGraphLaunch(...)
```

- `muapiModuleGetFunction` 到 `Module::GetFunction` 的链路见 M07 `call-chains.md`；函数句柄在 kernel 节点参数中被 `GraphKernelNode` 解析成 `Hal::KernelState`。[src/musa/core/module.cpp:56-73]

## 示例与文档的对应关系

| 示例 | 详细链路 | 数据结构 |
|---|---|---|
| 例 1 | `call-chains.md` 主链 B/C/D | `data-structures.md` |
| 例 2 | `call-chains.md` 主链 A | `execution-flows.md` 的 capture 流 |
| 例 3、例 4 | `interfaces.md` 查询与观测契约 | `implementation.md` 分支矩阵 |
| 例 6 | `risks-and-debt.md` | — |

## 相关文档

- `interfaces.md`：参数、返回值与前置条件
- `testing.md`：如何把这些例子变成用例

## 源码证据摘要

- 入口序列：`muapiGraphCreate` -> `muapiGraphAddKernelNode_v2` -> `muapiGraphInstantiate_v2` -> `muapiGraphLaunch` -> `muapiGraphExecDestroy` -> `muapiGraphDestroy`
- capture 序列：`muapiStreamBeginCapture_v2` -> `muapiStreamEndCapture`

## 未解决问题

- 仓库内是否另有外部测试工程包含 graph 用例，当前快照无法确认。
- 所有例子的目标机行为均未验证。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
