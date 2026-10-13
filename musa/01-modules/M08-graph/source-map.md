# Graph 执行：源码地图

- 文档目的：graph：源码文件/目录到文档章节的映射。
- 适用范围：M08 模块的《Graph 执行：源码地图》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


## 分层与文件清单

行数为 `wc -l` 实测（单位：行，仅统计 graph 直接相关文件）。

| 层 | 路径 | 行数 | 关注点 |
|---|---|---|---|
| 驱动 API | `src/driver/mu_graph.cpp` | 3379 | 全部 `muapiGraph*`/`muapiGraphNode*`/`muapiGraphExec*` 出口、参数校验、`TlsCtxTop` 取 context |
| 驱动 API | `src/driver/mu_stream.cpp` | 962 | `BeginCapture`/`EndCapture`/`GetCaptureInfo`/`UpdateCaptureDependencies` |
| Core 图对象 | `src/musa/core/graph.h` | 109 | `Graph` 顶点/边/入度出度/leaf/clone 映射 |
| Core 图对象 | `src/musa/core/graph.cpp` | 513 | 拓扑维护、clone、dot 打印、`GetPerfGraphExecVersion` |
| 节点 | `src/musa/core/node/` | 26 文件 | 每种节点一个类，见 [`data-structures.md`](data-structures.md) |
| v1 实例 | `src/musa/core/graph/graph1/graphExec.h` | 283 | `MusaKick`/`MusaSubmission`/`GraphExec` |
| v1 实例 | `src/musa/core/graph/graph1/graphExec.cpp` | 2103 | 拓扑解析、submission 切分、HAL cmd buffer 写入 |
| v1 提交 | `src/musa/core/graph/graph1/universalManager.cpp` | 1090 | `CmdSubmission`/`CmdCallback`/`CmdGraph`/条件变量读取 |
| v2 实例 | `src/musa/core/graph/graph2/graphExec.h` | 147 | `GraphExec2` BFS 列表与内部 launch stream |
| v2 实例 | `src/musa/core/graph/graph2/graphExec.cpp` | 555 | `GraphFlatten` 扁平化与 stream 绑定 |
| 命令 | `src/musa/core/command/graphCommand.h` | 64 | `GraphCommand` 成员与 MUpti 上下文表 |
| 命令 | `src/musa/core/command/graphCommand.cpp` | 762 | `Submit`/`ExecuteImpl`/`Postprocess`、错误消息生成 |
| stream | `src/musa/core/stream.cpp` | 1970 | capture 状态机与 `CmdLaunchGraph`（graph 段为 45-139、281-414、673-694） |
| context | `src/musa/core/context.cpp` | 3010 | `CreateGraph`/`CreateGraphExec`/`DestroyGraph*`/node 创建/capture 分派 |
| 公共头 | `src/musa_shared_include/musa.h` | — | `MUgraphNodeType`、`MUSA_CONDITIONAL_NODE_PARAMS` 等公共类型 |

## 关键锚点

- 建图入口：[src/driver/mu_graph.cpp:1188-1208]（`muapiGraphCreate`），分派表在 [src/driver/mu_graph.cpp:1012-1069]（`muapiGraphAddNode`）
- 实例化与提交：[src/driver/mu_graph.cpp:2304-2324]、[src/driver/mu_graph.cpp:2374-2401]
- v1 解析：[src/musa/core/graph/graph1/graphExec.cpp:1033-1301]
- v2 扁平化：[src/musa/core/graph/graph2/graphExec.cpp:417-548]
- 提交分派：[src/musa/core/command/graphCommand.cpp:280-431]
- 节点类型枚举：[src/musa_shared_include/musa.h:1964-2000]

## 上下游

- 上游（谁调用）：外部程序经 muapiGraph* 系列；capture 由 muapiStream* 系列转入；Context::GeneralLaunch 系列在 stream 非 capture 态下直接入队，在 capture 态下改走入图，见 [src/musa/core/context.cpp:660-674]
- 下游（调用谁）：`Hal::CreateCmdPool`/`CreateSemaphore` 等在 [src/musa/core/graph/graph1/graphExec.cpp:726-754]，提交经 `UniversalManager::Submit`；HAL 内部（`src/hal/m3d`）本模块未逐行读取

## 搜索建议

```bash
rg -n "muapiGraph|MU_GRAPH_NODE_TYPE" src/driver/mu_graph.cpp
rg -n "CaptureNode|CmdLaunchGraph|EndCapture" src/musa/core/stream.cpp
rg -n "MUSA_SUBMISSION_|ResolveGraphImpl|PrepareSubmission" src/musa/core/graph/graph1
rg -n "GetContionalValue|CmdSubmission|CmdGraph" src/musa/core/graph/graph1/universalManager.cpp
```

上述命令是阅读建议，本知识库未在远端执行。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/driver/mu_graph.cpp`（4 处，例：[src/driver/mu_graph.cpp:1188-1208]）
- `src/musa/core/graph/graph1/graphExec.cpp`（2 处，例：[src/musa/core/graph/graph1/graphExec.cpp:1033-1301]）
- `src/musa/core/graph/graph2/graphExec.cpp`（1 处，例：[src/musa/core/graph/graph2/graphExec.cpp:417-548]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
