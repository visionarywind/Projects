# Graph 执行：开发指南

- 文档目的：给出在 M08 Graph 模块内新增功能、修改行为、排障与补测试的可执行配方
- 适用范围：`src/driver/mu_graph.cpp`、`src/musa/core/graph*`、`src/musa/core/node/graph*`、`src/musa/core/stream.cpp` 的 capture/launch 段
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：静态源码分析；构建与运行命令一律标注「未验证」
- 最后更新：2026-10-07
- 前置阅读：`design.md`、`call-chains.md`、`interfaces.md`
- 后续阅读：`testing.md`、`risks-and-debt.md`

## 修改前

1. 锁定源码提交并更新证据行号。本模块三个锚点入口是 `muapiGraphCreate`、`muapiGraphInstantiate_v2`、`muapiGraphLaunch`。[src/driver/mu_graph.cpp:1188-1208] [src/driver/mu_graph.cpp:2304-2324] [src/driver/mu_graph.cpp:2374-2401]
2. 判断改动落在哪一层：ABI/包装层（`mu_wrappers_generated.cpp`）、driver 校验层（`mu_graph.cpp`）、Core 拓扑层（`graph.cpp`）、GraphExec 翻译层（`graph1/graph2`）、stream 提交层（`stream.cpp`）。
3. 先画调用链和对象所有权再改代码：本模块同时存在原图、克隆图、submission、command 四类对象，改动前要明确谁创建、谁销毁。

## 场景 1：新增一种图节点

按下面顺序改，缺一环就会落进默认分支：

1. 枚举：在 `MUgraphNodeType` 里追加取值（当前 `KERNEL=0` 到 `MEM_TRANSFER=17`）。[src/musa_shared_include/musa.h:1964-2000]
2. Core 节点类：仿照 `src/musa/core/node/graphKernelNode.cpp` 新增 `GraphXxxNode`，实现 `Create`/`UpdateParams`/`GetParams`。
3. 创建入口：在 `Context` 上加 `CreateKernelNode` 同形态的工厂函数。[src/musa/core/context.cpp:2230-2238]
4. driver add 入口：仿照 `muapiGraphAddKernelNode` 新增同形态函数，并在 `muapiGraphAddNode` 的 switch 中补一个 case；漏补会落到 `MUSA_ERROR_NOT_SUPPORTED`。[src/driver/mu_graph.cpp:1012-1069]
5. v1 提交：确认 `NodeTypeToKickType` 与 submission 归类能识别新类型。[src/musa/core/graph/graph1/graphExec.cpp:1061-1083]
6. v2 提交：在 `Stream::CmdLaunchGraph` 的 switch 中补 case；漏补会打 `Unexpected node type` 日志并返回 `MUSA_ERROR_NOT_SUPPORTED`。[src/musa/core/stream.cpp:385-393]
7. 参数 variant：`NodeParams` 宏当前是 12 类参数的 `std::variant`，新增参数类型要同步改宏。[src/musa/core/graph/graph1/graphExec.h:61-64]

**验证**：先做静态 `rg` 检查所有 switch 是否都覆盖新枚举，再在授权目标机补一条建图→实例化→launch 用例（未验证）。

## 场景 2：修改 v1/v2 实现的选择条件

分叉只有一处：`Context::CreateGraphExec` 里 `graphUserQ && supportEngineSync && GetPerfGraphExecVersion()==2` 三者同时成立才创建 `GraphExec2`，否则创建 `GraphExec`。[src/musa/core/context.cpp:1420-1434]

- 改配置开关：动 `graphUserQ`（Platform settings）会影响全部图，回归面最大。
- 改拓扑启发：动 `Graph::GetPerfGraphExecVersion` 只影响接近 v2 形态的图；它把「存在 kernel 节点」直接判为 v1。[src/musa/core/graph.cpp:477-494]
- 回滚方式：把条件恢复成原表达式即可，两条路径都保留，不存在数据迁移。

## 场景 3：新增一个 Graph API

四处登记缺一不可：

1. 声明：`src/driver/muapi.h`（如 `muapiGraphCreate` 的声明）。[src/driver/muapi.h:152-152]
2. 实现：`src/driver/mu_graph.cpp`，按 `InitPlatform -> 空指针检查 -> TlsCtxTop -> ICast -> ValidateGraph -> 核心调用` 的既有顺序写，样板是 `muapiGraphAddKernelNode_v2`。[src/driver/mu_graph.cpp:778-804]
3. 观测包装：在 `mu_wrappers_generated.cpp` 中加同名 wrapper，内部转发到 `muapiGraphCreate` 形态的实现；可直接复制 `muGraphCreate` 的模板。[src/driver/mu_wrappers_generated.cpp:6034-6073]
4. 导出登记：在 `DriverEntryPointTable` 构造函数里调用 `addEntryPoints`，可参考 `muGraphCreate` 的登记方式。[src/driver/internal.cpp:497]

跨进程/跨 loader 的符号还需要 loader manifest 条目，参考 `muGraphCreate` 在 `loader_api_list.inc` 中的登记方式。[src/loader/loader_api_list.inc:426]

## 场景 4：修一个句柄/生命周期类 Bug

- 校验不足：对照 `muapiGraphAddKernelNode_v2` 的四段检查（参数空、func/kern 空、context 空、依赖合法）逐段补齐。[src/driver/mu_graph.cpp:778-804]
- 入图失败：保持「创建成功但 `AddGraphNode` 失败就 `delete pGraphNode`」的回滚写法，不要只返回错误码。[src/driver/mu_graph.cpp:826-831]
- 销毁顺序：`DestroyGraphExec` 必须是 `ReleaseStream -> 摘登记 -> LockedWait -> delete`，任何一步都不能省。[src/musa/core/context.cpp:1690-1736]
- 登记查找：改 `ValidateGraphExec` 时注意三段式查找的第三段当前写法与 `ValidateGraph` 不同（见 `risks-and-debt.md`）。[src/musa/core/context.cpp:1661-1681]

## 场景 5：增加日志与调试信息

- 拓扑级失败日志已有先例：`ResolveGraphImpl` 判环失败后 `tprintf(LOG_ERR, ...)`。[src/musa/core/graph/graph1/graphExec.cpp:1296-1305]
- 想要人可读的图结构，优先复用 `muapiGraphDebugDotPrint` 而不是新加打印；它把节点类型、kernel 名、memcpy 方向写成 DOT 文件。[src/driver/mu_graph.cpp:3208-3232]
- 不要在 `muapi*` 热路径里加锁内打印：`ValidateGraph` 走的是 `ReadLockedAccessor`，长日志会拉长临界区（推断，未测量）。[src/musa/core/context.cpp:1588-1616]

## 场景 6：扩展依赖/edge 语义

当前 `_v2` 的 `dependencyData` 被忽略，只支持 force dependency，源码注释写明这是刻意的占位：[src/driver/mu_graph.cpp:769-777]

- 加边语义要落在 `Graph::AddEdge`：它同时维护 `m_Edges`、出入度和 `m_ParentNodes`。[src/musa/core/graph.cpp:71-96]
- 删除语义要同步维护三处，否则度数与父表会漂移；`RemoveDependencies` 目前只改边表（见 `risks-and-debt.md`）。[src/musa/core/graph.cpp:299-331]
- 影响面：出入度被 `GetRootNodes`/`GenLeafNodes`/`ResolveGraphImpl` 消费，改错会直接导致误判环。

## 场景 7：性能优化

- v1 的可优化点在 submission/kick 组装（`PrepareAllSubmissions`）与 `GraphCommand` 的一次性提交。
- v2 的可优化点在 `GraphFlatten` 的 BFS 排序与内部 launch stream 数量：内部流数量由 `ceHwqCount` 决定。[src/musa/core/graph/graph2/graphExec.cpp:417-430]
- 任何「跳过 CloneGraph」类优化都会破坏 update 语义（原图可反复改），不建议在没有替代方案时动。
- 上述均为静态分析结论，未做性能测量。

## 场景 8：增加回归测试

1. 本仓库当前快照没有 `tests/` 与 `unittest/` 目录，`MUSA_BUILD_UT` 打开会在 `add_subdirectory(unittest)` 处失败（静态分析，未验证）。[CMakeLists.txt:167-169]
2. 新增用例优先放在外部测试工程，覆盖：建图、加节点、实例化、launch、update 拓扑变化、destroy 六步。
3. 静态自检：改动后运行知识库校验脚本，确认引用行号未漂移。

## 修改后

- 运行 `python3 verify_kb.py` 与 `python3 verify_content.py` 校验本知识库引用与行号一致性。
- 先做静态 diff/格式检查，再在授权目标机做最小构建与专项测试；安装脚本只在明确授权时执行。以上命令在本知识库环境中**未执行、未验证**。

## 相关文档

- `design.md`：分层与取舍
- `call-chains.md`：八条调用链
- `risks-and-debt.md`：本页配方对应的已知风险

## 源码证据摘要

- 入口样板：`muapiGraphAddKernelNode_v2`、`muGraphCreate` 包装、`DriverEntryPointTable`
- 分叉点：`Context::CreateGraphExec`、`Graph::GetPerfGraphExecVersion`
- 提交点：`Stream::CmdLaunchGraph` 的 v1/v2 两分支

## 未解决问题

- 新节点类型在 v1 submission 归类中的 `KICK_TYPE` 选择规则需结合硬件能力矩阵确认。
- edge data 的硬件支持程度需要目标机验证。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
