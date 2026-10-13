# D02：内存拷贝 / 批量异步候选

- 文档目的：给出 D02 的目标、静态追踪链、观察点与用例矩阵，并明确当前快照缺入口的事实。
- 适用范围：`source/musa` 快照（5.3.0 develop）的 memcpy 路径，预计覆盖 M02 / M04 / M05 / M06 / M09。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）。
- 证据状态：**链路为静态分析，未逐行验证顺序，未读取或运行 Demo 源码**。
- 最后更新：2026-10-07
- 前置阅读：[`../D01-muinfo/README.md`](../D01-muinfo/README.md)
- 后续阅读：[`../D03-graph/README.md`](../D03-graph/README.md)、[`../99-roadmap/performance-guide.md`](../../99-roadmap/performance-guide.md)

## 结论摘要

候选入口 `tests/memcpyBatchAsync.cu`、`tests/memcpy3DBatchAsync.cu` **在本快照不存在**，所以 D02 目前只能做静态追踪。可以确认的主链是：`muapiMemcpyAsync` 完成校验后下到 `Context`，经 `Stream::CmdCopyMemory` 变成 `AsyncMemcpyCommand`，再进 `QueueCommand` / `AsyncSubmit` 提交；批量路径另有独立入口 `muapiMemcpyBatchAsync`。以下每一跳都给了行级证据，但**跳与跳之间的调用顺序是静态推断**。

---

## 1. 入口现状

| 项 | 状态 |
|---|---|
| `tests/memcpyBatchAsync.cu`、`tests/memcpy3DBatchAsync.cu` | 快照内不存在，需向外部获取 |
| `tests/`、`unittest/` 目录 | 不存在，见 [`../99-roadmap/testing-recipes.md`](../../99-roadmap/testing-recipes.md) |
| 替代观测物 | `muInfo` 的 `muMemGetInfo_v2` 打印 [src/tools/muInfo.cpp:363-369]，只读不拷贝 |

在拿到 Demo 源码之前，不要把下面的链路写成"已验证结果"。

---

## 2. 静态追踪链

| # | 环节 | 落点 | 要点 |
|---|---|---|---|
| 0 | 应用符号 | `muMemcpyAsync` [src/musa_shared_include/musa.h:11540-11545] | 应用侧原型 |
| 1 | Driver 单次拷贝 | `muapiMemcpyAsync` [src/driver/mu_memory.cpp:1047-1058] | 参数校验后下到 Core |
| 1b | Driver 异步 H2D 变体 | `muapiMemcpyHtoDAsync_v2` [src/driver/mu_memory.cpp:861-876] | `stream == null` 视为同步、pageable 内存的特殊处理 |
| 1c | Driver 批量拷贝 | `muapiMemcpyBatchAsync` [src/driver/mu_memory.cpp:1059-1120] | 逐条校验 copy parameter，失败即整体失败 |
| 2 | Core 分发 | `Context::GeneralMemcpy` [src/musa/core/context.cpp:702-735] | 按拷贝类型与设备位置分派 |
| 3 | 变成流命令 | `Stream::CmdCopyMemory` [src/musa/core/stream.cpp:696-727] | 构造 `AsyncMemcpyCommand` 并入流 |
| 4 | 命令构建 | `AsyncMemcpyCommand::BuildCopyMemory` [src/musa/core/command/AsyncMemcpyCommand.cpp:202-234] | 把参数翻成 cmdBuffer 指令 |
| 5 | 入队 | `Stream::QueueCommand` [src/musa/core/stream.cpp:1056-1104] | 容量自旋 + `m_SubmitMtx` + 压入 `m_CommandList` |
| 6 | 依赖与提交 | `Context::ResolveDependencyAndQueueCommand` [src/musa/core/context.cpp:2012-2076] → `AsyncMemcpyCommand::Submit` [src/musa/core/command/AsyncMemcpyCommand.cpp:129-201] | 默认流/barrier 流依赖建立；提交结果逐条检查 |

> 顺序说明：上表 1→6 是按代码位置与命名推断的主链，**未逐步断点验证**；`GeneralMemcpy` 到 `CmdCopyMemory` 之间可能还经过 barrier / 队列选择分支，需拿到运行环境后确认。

### 2.1 与既有推荐链的差异

原推荐链 `muapiMemcpyAsync -> Context::GeneralMemcpy -> Stream::CmdCopyMemory -> AsyncMemcpyCommand -> QueueCommand -> Submit` 与上表一致，只是把"Submit"拆成 `QueueCommand`（入队）与 `AsyncMemcpyCommand::Submit`（实际下发）两步——它们不是同一个函数，定位时要分别打断点。

---

## 3. 观察点（静态标注，未设置）

1. **校验是否兜底**：`muapiMemcpyAsync` 对 `dst/src == nullptr`、`ByteCount == 0` 的处理 [src/driver/mu_memory.cpp:1047-1058]。
2. **批量的原子性**：`muapiMemcpyBatchAsync` 中途失败时，前面已提交的条目如何回滚 [src/driver/mu_memory.cpp:1059-1120]。
3. **依赖建立**：默认流写入时对同 context 其他流的遍历成本 [src/musa/core/context.cpp:2036-2076]。
4. **合并与延迟**：`AsyncMemcpyCommand` 是否进 `m_MergingList`、何时被 `mergingListMaxSize = 32` 触发提交 [src/musa/core/stream.cpp:1286-1307]。
5. **提交期错误**：`Submit` 返回非零时命令是否仍留在 `m_InflightList` [src/musa/core/command/AsyncMemcpyCommand.cpp:129-201]。
6. **分配侧**：拷贝前后的池分配/裁剪（`MemMgr::Allocate` 与 `UpdateUserPools`）是否在 hot path 上 [src/hal/m3d/memMgr.cpp:81-147]。

---

## 4. 用例矩阵（目标状态，全部未执行）

| 用例 | 变量 | 期望 |
|---|---|---|
| 同步 vs 异步 | `muMemcpyAsync` + `muStreamSynchronize` | 数据一致，顺序不越界 [src/musa_shared_include/musa.h:11540-11545] |
| pageable vs pinned | host 源类型 | 行为差异落在 `muapiMemcpyHtoDAsync_v2` 的分支 [src/driver/mu_memory.cpp:861-876] |
| 批量 1 条 / N 条 / 含非法条目 | `muapiMemcpyBatchAsync` 的条目数与合法性 | 非法条目导致整体失败且无半成品 |
| D2H、H2D、D2D、peer | 方向 | peer 需多设备，见第 6 节 |
| 图内拷贝 | capture 中调用 | 转为 `MU_GRAPH_NODE_TYPE_MEMCPY`，走 `CreateMemcpyNode` [src/musa/core/context.cpp:2154-2209] |
| 错误延迟暴露 | 提交期注入错误 | 下一次同步才返回，不在提交时吞错 |

---

## 5. 用到的其他证据

- 图内 memcpy 节点构造：`Context::CreateMemcpyNode` [src/musa/core/context.cpp:2154-2209]
- capture 期间的入口：`muapiStreamBeginCapture_v2` [src/driver/mu_stream.cpp:391-430]
- 拷贝前的内存来源：`MemMgr::Allocate` [src/hal/m3d/memMgr.cpp:81-147]

---

## 6. 环境与限制

1. **无 Demo 源码**：候选 `.cu` 不在快照，第 4 节用例目前只能当需求清单。
2. **无硬件**：D2D/peer 用例需要多设备，本环境不可用。
3. **不要写成已验证**：本文件任何一行链路都未断点确认。
4. **改动纪律**：D02 是观察型 Demo，若为跑通而改 `src/driver`，请改用 [`../99-roadmap/feature-development-recipes.md`](../../99-roadmap/feature-development-recipes.md) 的正式配方并登记影响面。

## 相关文档

- [`../D01-muinfo/README.md`](../D01-muinfo/README.md)
- [`../D03-graph/README.md`](../D03-graph/README.md)
- [`../01-modules/M05-memory-mempool/README.md`](../../01-modules/M05-memory-mempool/README.md)
- [`../90-cross-module/performance-critical-paths.md`](../../90-cross-module/performance-critical-paths.md)

## 源码证据摘要

- Driver：[src/driver/mu_memory.cpp:1047-1058]、[src/driver/mu_memory.cpp:1059-1120]、[src/driver/mu_memory.cpp:861-876]、[src/driver/mu_stream.cpp:391-430]
- Core：[src/musa/core/context.cpp:702-735]、[src/musa/core/context.cpp:2012-2076]、[src/musa/core/context.cpp:2154-2209]、[src/musa/core/stream.cpp:696-727]、[src/musa/core/stream.cpp:1056-1104]、[src/musa/core/stream.cpp:1286-1307]
- 命令：[src/musa/core/command/AsyncMemcpyCommand.cpp:129-201]、[src/musa/core/command/AsyncMemcpyCommand.cpp:202-234]
- HAL 与工具：[src/hal/m3d/memMgr.cpp:81-147]、[src/tools/muInfo.cpp:363-369]

## 未解决问题

1. Demo 源码缺失，链路顺序未断点验证。
2. 批量失败的回滚语义未确认。
3. peer / D2D 依赖多设备，无环境。

## 下一步阅读建议

先获取 `tests/memcpyBatchAsync.cu`；在拿到之前，按第 2 节在本地静态对照一遍 `muapiMemcpyAsync` → `AsyncSubmit`，并把不一致处记入 [`../00-overview/analysis-state.md`](../../00-overview/analysis-state.md)。
