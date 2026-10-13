# Graph 执行：行级分析

- 文档目的：graph：关键代码位置的行级证据与解读。
- 适用范围：M08 模块的《Graph 执行：行级分析》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-08
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


## 证据表：入口与对象创建

| 行号 | 观察 | 结论状态 |
|---|---|---|
| [src/musa/core/context.cpp:1398-1456] [src/musa/core/context.cpp:2116-2536] | context 内 graph 对象创建（`CloneGraph`、`CreateGraphExec`）与资源创建函数所在区间 | 区间已核对，逐函数语义见 [`call-chains.md`](call-chains.md) |
| [src/musa/core/context.cpp:1420-1450] `Context::CreateGraphExec` | 未绑定 conditional handle 返回 `MUSA_ERROR_INVALID_VALUE`；按 `graphUserQ`、`supportEngineSync`、`GetPerfGraphExecVersion()==2` 三条件选 `GraphExec2`/`GraphExec`，`Init` 失败即 `delete` 回滚 | 已逐行确认 |
| [src/driver/mu_graph.cpp:2304-2324] `muapiGraphInstantiate_v2` | `TlsCtxTop()` 为空返回 `MUSA_ERROR_INVALID_CONTEXT`；`ValidateGraph` 失败或输出指针为空返回 `MUSA_ERROR_INVALID_VALUE`；通过才 `CreateGraphExec` 写出句柄 | 已逐行确认 |
| [src/driver/mu_graph.cpp:2374-2401] `muapiGraphLaunch` | 依次校验 exec 空指针、当前 context、`InfoStream` 解析 stream、`ValidateGraphExec`，全部通过才 `CmdLaunchGraph` | 已逐行确认 |

## 证据表：建图（graph 对象层）

| 行号 | 观察 | 结论状态 |
|---|---|---|
| [src/musa/core/graph.cpp:65-96] `Graph::AddNode` 与 `Graph::AddEdge` | 建顶点时把 `m_NodeInDegree`/`m_NodeOutDegree` 置 0；加边前先补齐缺失顶点，去重后才增度数并 `AddParent` | 已逐行确认 |
| [src/musa/core/graph.cpp:245-272] `Graph::AddGraphNode` | CONDITIONAL 节点先查 `m_ConditionalHandleBindings` 的绑定状态，成功才登记顶点并按依赖逐条 `AddEdge` | 已逐行确认 |
| [src/musa/core/graph.cpp:494-513] `Graph::ValidateDependencies` | 空指针、`ValidateNode` 失败、重复依赖任一命中即置 `MUSA_ERROR_INVALID_VALUE` | 已逐行确认 |
| [src/driver/mu_graph.cpp:1012-1069] `muapiGraphAddNode` | 按 `nodeParams->type` switch 分派到各单节点入口，`default` 分支返回 `MUSA_ERROR_NOT_SUPPORTED` | 已逐行确认 |

## 证据表：exec 实例与节点类型

| 行号 | 观察 | 结论状态 |
|---|---|---|
| [src/musa_shared_include/musa.h:1964-2000] `MUgraphNodeType` | 公共枚举取值 0..17；`MU_GRAPH_NODE_TYPE_CONDITIONAL` 的注释列出"不能作子节点、只能有一个实例化、不能克隆"三项限制 | 已确认（公共头契约） |
| [src/musa/core/graph/graph1/graphExec.cpp:709-724] `GraphExec::Init` | CloneGraph → `ResolveGraph` → `CreateExecResource` → `PrepareAllSubmissions` 四步串行，任一失败即不再继续 | 已逐行确认 |
| [src/musa/core/graph/graph2/graphExec.cpp:362-371] `GraphExec2::Init` | 只做 CloneGraph 与 `GraphFlatten`，不准备提交资源 | 已逐行确认 |
| [src/musa/core/graph/graph1/graphExec.cpp:1290-1294] 环检测 | `outQueueNumber != nodes.size()` 判定成环后返回 `MUSA_ERROR_INVALID_VALUE` 并 `m_Submissions.clear()` | 已逐行确认 |
| [src/musa/core/stream.cpp:673-694] `Stream::CaptureNode` | 把 `m_LastCapturedNodes` 当作新节点依赖，再用自身替换该列表，形成 capture 期的链式依赖 | 已逐行确认 |

## 未覆盖范围

| 层 | 观察 | 结论状态 |
|---|---|---|
| 下游 HAL | `src/hal/m3d` 的队列/cmd buffer 提交 | 构建/接口已确认，硬件副作用未知 |
| 节点参数实现 | `src/musa/core/node/*.cpp` 各节点 `Init`/`UpdateParams` | 文件已定位，本页未逐行展开 |

## 阅读方法

1. 先看参数检查和 early return。
2. 再看对象创建、登记、引用计数或 tracker 更新。
3. 最后看 HAL 调用、异步入队、错误回滚和析构。

本页不把没有逐行读取的代码扩写成确定行为；行号随源码演进会漂移，改动后需重跑行号校验。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/musa/core/context.cpp`（3 处，例：[src/musa/core/context.cpp:1420-1450] `Context::CreateGraphExec`）
- `src/driver/mu_graph.cpp`（3 处，例：[src/driver/mu_graph.cpp:2304-2324] `muapiGraphInstantiate_v2`）
- `src/musa/core/graph.cpp`（3 处，例：[src/musa/core/graph.cpp:245-272] `Graph::AddGraphNode`）
- `src/musa/core/graph/graph1/graphExec.cpp`（2 处，例：[src/musa/core/graph/graph1/graphExec.cpp:709-724] `GraphExec::Init`）
- `src/musa_shared_include/musa.h`（1 处，例：[src/musa_shared_include/musa.h:1964-2000] `MUgraphNodeType`）
- `src/musa/core/graph/graph2/graphExec.cpp`（1 处，例：[src/musa/core/graph/graph2/graphExec.cpp:362-371] `GraphExec2::Init`）
- `src/musa/core/stream.cpp`（1 处，例：[src/musa/core/stream.cpp:673-694] `Stream::CaptureNode`）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
