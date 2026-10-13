# Graph 执行：实现说明

- 文档目的：graph：核心函数/流程的逐段实现解读。
- 适用范围：M08 模块的《Graph 执行：实现说明》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


## 正常实现路径

```text
capture/create node -> graph exec -> launch/update/destroy
```

capture 由 stream 状态机承担，手动建图由 `muapiGraphAddNode` 分派到具体 add 函数，实例化在 `Context::CreateGraphExec` 选择 `GraphExec`/`GraphExec2`，launch 在 `GraphCommand`/`UniversalManager` 完成设备提交。

## 关键实现观察

- 公共句柄一律先经 `ICast` 转为 Core 对象，再由 `ValidateGraph`/`ValidateGraphExec` 校验登记关系，不把 C 句柄当作任意指针直接使用。[src/driver/mu_graph.cpp:54-110]
- 失败沿 `MUresult` 向上传递；node 创建失败后由调用方 `delete pGraphNode` 回滚。[src/driver/mu_graph.cpp:841-892]
- `GraphExec::Init` 首先 `CloneGraph`，因此 GraphExec 不直接持有调用方 graph，而是持有克隆图。[src/musa/core/graph/graph1/graphExec.cpp:709-724]
- `Graph::CloneGraph` 复制节点、入度/出度、边表，并维护 `m_MapCloneByOriginal` 供 `FindNodeInClone` 使用。[src/musa/core/graph.cpp:126-162] [src/musa/core/graph.cpp:333-352]
- 拓扑依赖用入度/出度表维护：`AddEdge` 增加父子度并写边表，`DeleteNode` 反向扣减并禁止含 alloc/free 节点的删除。[src/musa/core/graph.cpp:71-96] [src/musa/core/graph.cpp:187-242]
- conditional handle 必须先创建并绑定到图，未绑定时 `CreateGraphExec` 直接拒绝。[src/musa/core/context.cpp:2579-2597] [src/musa/core/context.cpp:1420-1450]

## 分支矩阵

| 分支 | 触发 | 结果 | 当前证据 |
|---|---|---|---|
| 参数/句柄无效 | NULL、越界、无 current context | `MUSA_ERROR_INVALID_VALUE`/`MUSA_ERROR_INVALID_CONTEXT` | 已确认于 API 层 |
| 图不是当前 context 登记对象 | `ValidateGraph` 失败 | `MUSA_ERROR_INVALID_VALUE` | 已确认 |
| capture 已激活再 BeginCapture | 状态为 ACTIVE | `MUSA_ERROR_ILLEGAL_STATE` | [src/musa/core/stream.cpp:51-54] |
| EndCapture 跨线程（非 RELAXED） | 线程不同 | `MUSA_ERROR_STREAM_CAPTURE_WRONG_THREAD` | [src/musa/core/stream.cpp:80-83] |
| leaf 未全部出现在 last captured nodes | 非法 join | `MUSA_ERROR_STREAM_CAPTURE_UNJOINED` | [src/musa/core/stream.cpp:88-95] |
| 拓扑变更后更新 | 节点数/类型/父节点不匹配 | `MU_GRAPH_EXEC_UPDATE_ERROR_TOPOLOGY_CHANGED` | [src/musa/core/graph/graph1/graphExec.cpp:661-696] |
| 图含环 | 拓扑遍历 `outQueueNumber != nodes.size()` | `MUSA_ERROR_INVALID_VALUE` | [src/musa/core/graph/graph1/graphExec.cpp:1290-1294] |
| 上传类 API | `muapiGraphUpload` | `MUSA_ERROR_NOT_SUPPORTED` | [src/driver/mu_graph.cpp:2403-2405] |

## 尚未证明

该模块是否在所有错误分支都释放中间对象、conditional WHILE 在设备侧更新条件变量时的并发语义、以及 v1/v2 在 `graphUserQ` 关闭或引擎不支持 sync 时的退化路径，必须结合完整函数体和目标机测试确认。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/musa/core/graph.cpp`（4 处，例：[src/musa/core/graph.cpp:126-162]）
- `src/driver/mu_graph.cpp`（3 处，例：[src/driver/mu_graph.cpp:54-110]）
- `src/musa/core/graph/graph1/graphExec.cpp`（3 处，例：[src/musa/core/graph/graph1/graphExec.cpp:709-724]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
