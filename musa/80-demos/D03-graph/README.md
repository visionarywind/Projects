# D03：Graph 候选

- 文档目的：给出 D03 的两条候选执行路径（显式 graph API 与 stream capture）、静态追踪链、观察点与用例矩阵。
- 适用范围：`source/musa` 快照（5.3.0 develop）的 graph 路径，预计覆盖 M04 / M05 / M06 / M08 / M09。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）。
- 证据状态：**静态分析，未逐行验证顺序，未读取或运行 Demo 源码**。
- 最后更新：2026-10-07
- 前置阅读：[`../D02-memory-copy/README.md`](../D02-memory-copy/README.md)
- 后续阅读：[`../99-roadmap/feature-development-recipes.md`](../../99-roadmap/feature-development-recipes.md)（配方 B）

## 结论摘要

候选入口 `tests/childGraph.cu`、`tests/conditionalNode.cu`、`tests/allocnode.cu` **在本快照不存在**。静态可确认的是两条路径共用同一批落点：**显式 API** 走 `muapiGraphCreate` → `AddKernelNode` → `muapiGraphInstantiate_v2` → `muapiGraphLaunch`；**capture 路径**走 `Stream::BeginCapture` → `CaptureNode` → `muapiStreamEndCapture` 得到图，之后同样 `Instantiate` + `Launch`。两条路径最终都汇入 `UniversalManager::Submit`。跳与跳之间的顺序为静态推断，**未断点验证**。

---

## 1. 入口现状

| 项 | 状态 |
|---|---|
| `tests/childGraph.cu`、`tests/conditionalNode.cu`、`tests/allocnode.cu` | 快照内不存在，需向外部获取 |
| `tests/`、`unittest/` 目录 | 不存在，见 [`../99-roadmap/testing-recipes.md`](../../99-roadmap/testing-recipes.md) |
| 应用侧符号 | `muGraphCreate` 已在头文件声明 [src/musa_shared_include/musa.h:20620-20630] |

拿到 Demo 源码前，不要把下表写成"已验证结果"。

---

## 2. 路径 A：显式 graph API

| # | 环节 | 落点 | 要点 |
|---|---|---|---|
| 1 | 建图 | `muapiGraphCreate` [src/driver/mu_graph.cpp:1188-1209] | 空值/标志校验后建 `Graph` |
| 2 | 加节点 | `muapiGraphAddKernelNode` [src/driver/mu_graph.cpp:718-767] | 校验 → 工厂 → 挂图失败则 `delete pGraphNode` |
| 2b | 通用入口 | `muapiGraphAddNode` 按 `nodeParams->type` 分发 [src/driver/mu_graph.cpp:1012-1069] | 未知类型落 `default` 返回 `MUSA_ERROR_NOT_SUPPORTED` |
| 3 | 节点类型 | `MU_GRAPH_NODE_TYPE_KERNEL = 0` 起的枚举 [src/musa_shared_include/musa.h:1963-1982] | childGraph / conditionalNode 分别落在 child graph 与 conditional 类型上 |
| 4 | Core 构造 | `Context::CreateKernelNode` [src/musa/core/context.cpp:2230-2267] → `GraphKernelNode::Create` [src/musa/core/node/graphKernelNode.cpp:12-30] | 创建失败释放半成品对象 |
| 5 | 挂图 | `Graph::AddGraphNode` [src/musa/core/graph.cpp:245-271] → `Graph::AddNode` [src/musa/core/graph.cpp:65-70] | 登记顶点、`SetParentGraph`、逐依赖 `AddEdge` |
| 6 | 读参数（可选） | `muapiGraphNodeGetParams` 按类型回填 [src/driver/mu_graph.cpp:3304-3363] | 节点类型经 `GraphNode::GetType` 暴露 [src/musa/core/node/graphNode.h:30-50] |
| 7 | 实例化 | `muapiGraphInstantiate_v2` [src/driver/mu_graph.cpp:2304-2325] | 生成 `GraphExec` |
| 8 | launch | `muapiGraphLaunch` [src/driver/mu_graph.cpp:2374-2398] | 下到流 |
| 9 | 执行 | `GraphCommand::Submit` [src/musa/core/command/graphCommand.cpp:69-79] → `UniversalManager::Submit` [src/musa/core/graph/graph1/universalManager.cpp:470-500] | 组装 `Hal::QueueSubmitInfo` 交 HAL |

---

## 3. 路径 B：stream capture

| # | 环节 | 落点 | 要点 |
|---|---|---|---|
| 1 | 开始 capture | `muapiStreamBeginCapture_v2` [src/driver/mu_stream.cpp:391-430] → `Stream::BeginCapture` [src/musa/core/stream.cpp:45-74] | 进入 capture 状态 |
| 2 | 录节点 | `Stream::CaptureNode` [src/musa/core/stream.cpp:673-700] | 普通命令被转录成 graph 节点而非下发 |
| 3 | 结束 capture | `muapiStreamEndCapture` [src/driver/mu_stream.cpp:473-502] | 产出图句柄，此后仍需 Instantiate/Launch |
| 4 | 图内 launch | `Stream::CmdLaunchGraph` [src/musa/core/stream.cpp:281-305] | 支持 launch 已实例化的图 |

> capture 期间执行的 `CmdLaunchGraph` 与"被录下来的图"是两个概念，`childGraph.cu` 这类用例要先分清读的是哪一条 [src/musa/core/stream.cpp:281-305]。

---

## 4. 观察点（静态标注，未设置）

1. **`AddGraphNode` 的失败路径**：挂图失败是否真的走到 `delete` [src/driver/mu_graph.cpp:718-767]。
2. **依赖校验**：driver 层每个 `muapiGraphAdd*Node` 都先调 `ValidateDependencies` 再挂图 [src/driver/mu_graph.cpp:718-767]。
3. **条件节点**：`conditionalNode` 相关类型在 `MU_GRAPH_NODE_TYPE_CONDITIONAL` 段，其实现分支落在 `GraphExec` 私有子模块内 [src/musa_shared_include/musa.h:1963-1982]——**这部分无法在快照内展开**。
4. **两次提交形态**：单节点图与多节点图是否都走同一条 `GraphCommand` → HAL 提交 [src/musa/core/command/graphCommand.cpp:69-79]、[src/musa/core/graph/graph1/universalManager.cpp:470-500]。
5. **图内 memcpy/分配节点**：与 D02 的链路在 `CreateMemcpyNode`、`CreateMemory` 处交汇 [src/musa/core/context.cpp:2154-2209]、[src/musa/core/context.cpp:1044-1099]。
6. **capture 与并发流**：capture 状态下其他流仍在跑，依赖是否被正确阻断 [src/musa/core/stream.cpp:45-74]。

---

## 5. 用例矩阵（目标状态，全部未执行）

| 用例 | 路径 | 期望 |
|---|---|---|
| 单 kernel 图 | A | 结果与直接 launch 一致 |
| 多节点 + 依赖边 | A | 拓扑不串味；坏依赖被 `ValidateDependencies` 拒绝 [src/driver/mu_graph.cpp:718-767] |
| 子图（child graph） | A/B | 嵌套图句柄生命周期独立，销毁顺序正确 |
| 条件节点 | A | 依赖私有子模块，快照内无法验证 |
| capture 一段 memcpy+kernel | B | 图节点类型映射正确（参照 D02 第 4 节） |
| instantiate → launch → 销毁 | A/B | 重复 launch、销毁后再 launch 返回正确错误码 |
| 多设备 / peer | A | 需多设备，本环境不可用 |

---

## 6. 环境与限制

1. **无 Demo 源码**：候选 `.cu` 不在快照，第 5 节只是需求清单。
2. **私有子模块边界**：`GraphExec` 的执行器（graph1/graph2）与条件节点求值在 `src/hal/m3d/m3d` 内部，行为未知。
3. **不要写成已验证**：本文件任何链路都未断点确认。
4. **改动纪律**：新增节点类型请走 [`../99-roadmap/feature-development-recipes.md`](../../99-roadmap/feature-development-recipes.md) 配方 B，别在 D03 里临时打补丁。

## 相关文档

- [`../D02-memory-copy/README.md`](../D02-memory-copy/README.md)
- [`../01-modules/M08-graph/README.md`](../../01-modules/M08-graph/README.md)
- [`../99-roadmap/feature-development-recipes.md`](../../99-roadmap/feature-development-recipes.md)

## 源码证据摘要

- Driver：[src/driver/mu_graph.cpp:1188-1209]、[src/driver/mu_graph.cpp:718-767]、[src/driver/mu_graph.cpp:1012-1069]、[src/driver/mu_graph.cpp:2304-2325]、[src/driver/mu_graph.cpp:2374-2398]、[src/driver/mu_graph.cpp:3304-3363]、[src/driver/mu_stream.cpp:391-430]、[src/driver/mu_stream.cpp:473-502]
- Core：[src/musa/core/graph.cpp:245-271]、[src/musa/core/graph.cpp:65-70]、[src/musa/core/context.cpp:2230-2267]、[src/musa/core/node/graphKernelNode.cpp:12-30]、[src/musa/core/stream.cpp:45-74]、[src/musa/core/stream.cpp:281-305]、[src/musa/core/stream.cpp:673-700]
- 执行：[src/musa/core/command/graphCommand.cpp:69-79]、[src/musa/core/graph/graph1/universalManager.cpp:470-500]
- 契约：[src/musa_shared_include/musa.h:1963-1982]、[src/musa_shared_include/musa.h:20620-20630]、[src/musa/core/node/graphNode.h:30-50]

## 未解决问题

1. Demo 源码缺失，两条路径均未断点验证。
2. `GraphExec` 执行器与条件节点在私有子模块内，未展开。
3. capture 期间的并发/同步语义只有函数级证据。

## 下一步阅读建议

先拿 `tests/childGraph.cu` 确认它走路径 A 还是 B；同时读 [`../01-modules/M08-graph/README.md`](../../01-modules/M08-graph/README.md) 确认 M08 文档覆盖度，再回到本文件第 4 节补观察点。
