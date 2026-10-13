# Graph 执行：风险与技术债

- 文档目的：按影响排序列出已从源码确认的风险、证据与建议
- 适用范围：M08 模块的《Graph 执行：风险与技术债》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：全部为静态阅读结论，未编译、未运行、未在目标机复现
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)
- 证据标注约定：`已确认` = 代码直接可见；`静态推断` = 由控制流推出但未实验验证；`环境缺口` = 本环境无法验证

## 1. 高风险（可能直接导致错误行为）

| 风险 | 影响 | 证据/状态 | 建议 | 源码位置 |
|---|---|---|---|---|
| 跨 context 查找写错对象 | 非当前 context 持有的执行图永远校验不过 | 已确认：`ValidateGraphExec` 的内层循环条件是 `*it != this`，实际调用的却是 `findGraphExec`，并且传入的是外层循环算出的 `curCtx` | 改传 `*it`，并补跨 context 的销毁用例 | [src/musa/core/context.cpp:1679-1683] |
| 同样的写法出现在销毁路径 | 执行图可能登记在别的 context 下却删不掉 | 已确认：`DestroyGraphExec` 的内层循环同样只调 `findGraphExec`，参数用的仍是 `curCtx` 而不是 `*it` | 与上一条一起修，抽公共查找函数 | [src/musa/core/context.cpp:1717-1725] |
| 删除节点不释放内存 | 节点对象泄漏 | 已确认：`Graph::DeleteNode` 只从 `m_Vertices`、`m_Edges` 摘除并 `SetParentGraph(nullptr)`，全程没有 `delete`，而 `Graph::~Graph` 遍历的是 `m_Vertices`，已经看不到它 | 在 `DeleteNode` 内 `delete`，或明确所有权归调用方并写进契约 | [src/musa/core/graph.cpp:231-238] |
| `RemoveDependencies` 不同步出入度 | 根节点判断与拓扑解析结果错误 | 已确认：只做 `m_Edges` 的删除，不改 `m_NodeInDegree`/`m_NodeOutDegree` | 删边时同步减度数 | [src/musa/core/graph.cpp:311-328] |
| 出入度是 `GetRootNodes` 的唯一依据 | 上一条的错误会传导到 BFS 与提交顺序 | 已确认：`GetRootNodes` 用 `m_NodeInDegree.at(...)` 判断根 | 度数表与边表由同一个入口维护 | [src/musa/core/graph.cpp:98-106] |
| 裸指针解引用 | 传入不属于任何图的节点句柄会空指针崩溃 | 静态推断 + 源码注释：`GetParentGraph()` 结果直接 `pGraph->DeleteNode(...)`，注释写明跟随 CUDA 的段错误行为 | 入口先判 `GetParentGraph()` 是否为空 | [src/driver/mu_graph.cpp:1244-1246] |

## 2. 中风险（接口语义与实现不符）

| 风险 | 影响 | 证据/状态 | 建议 | 源码位置 |
|---|---|---|---|---|
| `_v2` 忽略 `edgeData` | 调用方以为设了边属性，实际仍是普通依赖 | 已确认：注释写明「只支持 force dependency」，实现直接转调 v1 | 明确文档，或对非 force 边返回 `MUSA_ERROR_NOT_SUPPORTED` | [src/driver/mu_graph.cpp:296-303] |
| 移除依赖的 `_v2` 同样忽略 `edgeData` | 与上一条对称 | 已确认：直接转调 v1 | 同上 | [src/driver/mu_graph.cpp:3174-3181] |
| `muapiGraphExecNodeSetParams` 不校验执行图 | 可能对不属于自己的执行图写参数 | 已确认：函数体内没有 `ValidateGraphExec`，只有空指针与节点类型匹配检查 | 补 `ValidateGraphExec` | [src/driver/mu_graph.cpp:1071-1154] |
| `muapiGraphUpload` 恒失败 | 公共 API 存在但不可用 | 已确认：函数体只有一行 `return MUSA_ERROR_NOT_SUPPORTED` | 标注未实现，或从导出表移除 | [src/driver/mu_graph.cpp:2403-2405] |
| 统一入口只覆盖 13 种节点 | 5 种节点走 `muapiGraphAddNode` 恒返回 `MUSA_ERROR_NOT_SUPPORTED` | 已确认：`switch` 有 13 个 case，`default` 返回 `MUSA_ERROR_NOT_SUPPORTED` | 扩展 switch，或在文档列出可用集合 | [src/driver/mu_graph.cpp:1063-1065] |
| 有 mem 节点的图删不掉任何节点 | 编辑型 API 在真实工作负载上失效 | 已确认：只要图中**任一**节点是 mem_alloc/mem_free，`Graph::DeleteNode` 整体返回 `MUSA_ERROR_NOT_SUPPORTED` | 只检查目标节点自身类型 | [src/musa/core/graph.cpp:190-196] |
| DOT 导出不检查写流状态 | 路径不可写时仍返回成功 | 已确认：`std::ofstream` 打开后直接写，函数末尾 `return status`（初值 `MUSA_SUCCESS`） | 检查流状态并回传错误 | [src/musa/core/graph.cpp:354-357] |
| `muapiGraphGetNodes` 校验失败不回填数量 | 两段式调用拿不到节点数 | 已确认：失败分支把局部指针 `numNodes` 置空，调用方的 `numNodes` 指向的值未被改写 | 文档说明失败语义，或写回 `*numNodes = 0` | [src/driver/mu_graph.cpp:1867-1871] |

## 3. 低风险（一致性与可维护性）

| 风险 | 影响 | 证据/状态 | 建议 | 源码位置 |
|---|---|---|---|---|
| capture 状态恢复路径不对称 | `Query` 期间不改状态，`Synchronize` 期间却把流写成 `MU_STREAM_CAPTURE_STATUS_INVALIDATED` | 已确认：两者的 capture 检查分支行为不同 | 统一策略并写进 `implementation.md` | [src/musa/core/stream.cpp:223-227] |
| `MU_STREAM_CAPTURE_STATUS_INVALIDATED` 没有显式恢复接口 | 只能靠再次 `BeginCapture` 覆盖回 `ACTIVE` | 静态推断：`EndCapture` 在该状态直接返回 `MUSA_ERROR_STREAM_CAPTURE_INVALIDATED`，不写回 `MU_STREAM_CAPTURE_STATUS_NONE` | 提供恢复入口或文档写明清理方式 | [src/musa/core/stream.cpp:84-85] |
| 新增公共入口要改四处 | 漏改会出现「符号可查但调用崩」或「符号查不到」 | 静态推断：`export_table.h` 类型表、`ADDMEMBER` 成员、`addEntryPoints` 注册表、loader 桩缺一不可 | 加脚本做四处一致性检查 | [src/driver/internal.cpp:497] |
| capture 清理依赖平台全局注册表 | 捕获流列表与流对象生命周期耦合 | 已确认：`EndCapture` 直接遍历 `Platform` 的 `capturedStreams()` 并 erase | 保持注册与注销成对，补并发用例 | [src/musa/core/stream.cpp:125-135] |

## 4. 验证缺口

| 缺口 | 状态 | 证据 | 处理方式 |
|---|---|---|---|
| 没有可运行的单元测试目录 | 环境缺口 | 已确认：CMake 在 `MUSA_BUILD_UT` 下 `add_subdirectory(unittest)`，但源码快照中不存在 `unittest/` | 本知识库不声称任何测试通过 | [CMakeLists.txt:167-169] |
| 知识库登记的 graph 示例尚未读取 | 环境缺口 | 已确认：`80-demos/D03-graph` 只列候选并标注未读取，源码快照无 `tests/` | 示例与运行结果一律标注未验证 | [CMakeLists.txt:167-169] |
| 硬件与 HAL 侧行为不可证 | 环境缺口 | 静态推断：本机无目标 GPU 与驱动 | 所有提交时序结论标注静态分析 | [src/musa/core/command/dispatchCommand.cpp:302-347] |
| 构建未执行 | 环境缺口 | 本次任务为纯文档任务，未运行构建 | 不引用任何构建产物 | [CMakeLists.txt:167-169] |

## 5. 与既有债务清单的关系

- 本文件只收录 graph 子系统自身的债；跨模块的 ABI、导出表、CI 缺口见 `99-roadmap/` 与 M02。
- 命名与行号在源码变更后会漂移，复核方式见 `development-guide.md` 的「修改后检查」。

## 源码证据摘要

- 死分支：`ValidateGraphExec`（`context.cpp:1679-1683`）、`DestroyGraphExec`（`context.cpp:1717-1725`）
- 泄漏点：`Graph::DeleteNode` 无 `delete`（`graph.cpp:187-242`）
- 结构不一致：`Graph::RemoveDependencies` 不改出入度（`graph.cpp:311-328`）
- 未实现接口：`muapiGraphUpload`（`mu_graph.cpp:2403-2405`）

## 未解决问题

- 死分支是否已在别处补偿（例如调用方总是先切到持有 context），需要运行期验证。
- 节点泄漏在长期运行的建图场景里能否被观测到，需要压测。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
