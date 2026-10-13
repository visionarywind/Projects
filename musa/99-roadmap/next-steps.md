# 下一步

- 文档目的：把已有分析（[`../00-overview/analysis-state.md`](../00-overview/analysis-state.md)、[`risk-register.md`](risk-register.md)）折成可执行的今天 / 一周 / 一个月清单，关键项附源码证据。
- 适用范围：`source/musa` 快照（5.3.0 develop）后续的分析、修复与验证工作。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）。
- 证据状态：已确认（引用均为静态核对）；所有运行、构建、硬件结论均**未验证**。
- 最后更新：2026-10-07
- 前置阅读：[`../00-overview/analysis-state.md`](../00-overview/analysis-state.md)、[`risk-register.md`](risk-register.md)
- 后续阅读：[`feature-development-recipes.md`](feature-development-recipes.md)、[`testing-recipes.md`](testing-recipes.md)

## 结论摘要

最短路径是：先修 `SplayTree` 分支反转与 `MemMgr` 判空（有确定的崩溃证据），再补齐命令提交链的行级证据，最后才谈运行验证——因为本快照既没有 `unittest/`、`tests/`，也没有可用的目标机日志。反过来先做性能基线或 Demo 运行记录，会卡在缺测试入口与缺授权环境上。

---

## 今天（P0）

### 1. 修掉已确认的查找缺陷

- `Util::SplayTree::Search` 两个递归分支方向相反，导致 Find 对**已存在**的键也返回空 [src/util/utilSplayTree.h:216-226]。
- `MemMgr::FindPool` 无条件解引用 `Find` 结果，`Find` miss 返回 `nullptr`，调用即崩 [src/hal/m3d/memMgr.cpp:41-44]。
- `MemMgr::Free` 用 `Get`（返回"最近节点"而非失败）做精确查找，release 下断言被裁剪后可能错池或空指针 [src/hal/m3d/memMgr.cpp:149-161]。

完成标准：三分支方向修正 + 四处查找判空，并把 `Hal::IMemMgr` 接口的空值语义写进注释（当前 `FindPool`/`GetPool` 返回裸指针、无任何空值约定）[src/hal/halMemMgr.h:17-40]。详细根因与验证矩阵见 [`rca-splaytree-memmgr.md`](rca-splaytree-memmgr.md)。

### 2. 记录目标源码版本与行号锚点

知识库所有引用都按当前快照行号书写，源码一旦更新即整体漂移。先固定一次"引用基线"（版本宏 `MUSA_VERSION` 位置 [src/musa_shared_include/musa.h:210-215]），再开始任何修复提交。

---

## 一周内（P1）

### 1. 补齐命令提交链的行级证据

以下三段是 kernel / memcpy / graph 共用的执行主干，目前只到函数级：

- `DispatchCommand::Build`：合并级别的判定、print kernel 保留、`Command::Build` 收尾 [src/musa/core/command/dispatchCommand.cpp:78-155]。
- `AsyncMemcpyCommand::Submit`：把 `CopyParameter` 翻译成 `cmdBuffer.CmdCopyMemoryAdvanced` 并检查返回值 [src/musa/core/command/AsyncMemcpyCommand.cpp:129-201]。
- 合并上限与提交时机：`mergingListMaxSize = 32` 与 `EngineSubmissionReady` 决定一批命令何时提交 [src/musa/core/stream.cpp:1286-1307]。

产出：`01-modules/M06-stream-command/` 与 `M07-module-kernel/` 各补一条从 `ResolveDependencyAndQueueCommand` 到 HAL `Queue::Submit` 的完整轨迹。

### 2. 落实 wrapper / export 生成链

`mu_wrappers_generated.cpp` 首行包含 `generated_musa_meta.h` 等生成头 [src/driver/mu_wrappers_generated.cpp:1-11]，生成脚本不在快照。行动项：向持有生成脚本的一方索取脚本与输入清单，确认参数结构体（`mu*_params`）与原型是否由同一份定义产出；在拿到之前，`M13` 的 ABI 结论只能标"部分推断"。

### 3. 打通 loader 可配置性

`src/loader/CMakeLists.txt` 要求 `SHARED_INCLUDE_PATH` 指向共享接口目录，缺失时直接 `FATAL_ERROR` [src/loader/CMakeLists.txt:15-25]，这是本快照 loader 无法配置编译的直接原因。行动项：从上游构建脚本取该变量取值，或把缺失依赖登记为阻断项。

### 4. 建立测试入口的现实认知

`MUSA_BUILD_UT` 打开时 `add_subdirectory(unittest)`，但本快照没有 `unittest/` 目录 [CMakeLists.txt:167-169]；`tests/` 目录同样不存在。行动项：先确认测试代码是否在另一仓库，再决定是"补测试目录"还是"记录为快照缺口"。详见 [`testing-recipes.md`](testing-recipes.md)。

---

## 一个月内（P1/P2）

1. **D02/D03 实际轨迹**：候选入口 `tests/memcpyBatchAsync.cu`、`tests/childGraph.cu` 在本快照不存在，需先拿到测试源码；在此之前只能按 [`../80-demos/D02-memory-copy/README.md`](../80-demos/D02-memory-copy/README.md) 与 [`../80-demos/D03-graph/README.md`](../80-demos/D03-graph/README.md) 的静态链路推进。
2. **accessor/export 符号快照**：`FillAccessors` 逐槽填充 8 张导出表 [src/driver/mu_entry.cpp:2146-2195]，手工维护易漏。做一份导出符号与 accessor 槽位的基线文件，纳入回归 diff。
3. **性能基线**：先测 API CPU 时间、排队延迟、提交延迟与分配/释放耗时；重点观察合并阈值 32 条这一常量对吞吐的影响 [src/musa/core/stream.cpp:1286-1307]。**静态分析，未验证**——没有硬件环境前不要写具体数字。
4. **D01 目标机运行记录**：`muInfo` 主流程只有 `muInit`、设备枚举、逐设备 retain primary context 与打印 [src/tools/muInfo.cpp:372-387]，是拿到授权后最先能跑的入口。

---

## 不建议初期修改的模块

| 模块 | 原因 | 证据 |
|---|---|---|
| `src/hal/m3d/m3d` 私有子模块 | 内部实现与构建依赖不透明 | 分析状态记为未展开，见 [`../00-overview/analysis-state.md`](../00-overview/analysis-state.md) |
| `mu_wrappers_generated.cpp` | 生成物，改了会被 clean build 覆盖 | [src/musa_shared_include/generated_musa_meta.h:1-1] |
| loader 转发层 | 567 条清单三处展开，手改极易不同步 | [src/loader/loader.cpp:119-123] |
| `export_table.h` 的表序 | 前排槽位被 legacy profiler ABI 锁定 | [src/musa_shared_include/export_table.h:859-866] |

---

## 需要向原作者确认的问题

1. wrapper 与 `generated_*_meta.h` 的生成脚本、输入清单在哪里？
2. `tests/`、`unittest/` 是否在其他仓库？测试框架用的是 FetchContent 拉取的 googletest 还是内网镜像 [CMakeLists.txt:146-154]？
3. `MUSA_BUILD_UT` 是否在 CI 打开？CI 配置文件（`.mthreads-ci.yml`）是否可提供？
4. `SplayTree::Search` 分支反转是已知问题还是从未被走到的路径？（`MemMgr` 之外还有谁用这棵树？）

---

## 完成标准

每个主模块至少具备：入口落地、正常路径、关键分支、异常路径、清理路径、资源所有权、执行上下文、行级证据与测试映射。任何未执行的命令一律标注**未验证**，禁止把静态推断写成运行结论。

## 相关文档

- [`risk-register.md`](risk-register.md)
- [`rca-splaytree-memmgr.md`](rca-splaytree-memmgr.md)
- [`../00-overview/analysis-state.md`](../00-overview/analysis-state.md)
- [`../80-demos/demo-registry.md`](../80-demos/demo-registry.md)

## 源码证据摘要

- 查找缺陷：[src/util/utilSplayTree.h:216-226]、[src/hal/m3d/memMgr.cpp:41-44]、[src/hal/m3d/memMgr.cpp:149-161]、[src/hal/halMemMgr.h:17-40]
- 提交链：[src/musa/core/command/dispatchCommand.cpp:78-155]、[src/musa/core/command/AsyncMemcpyCommand.cpp:129-201]、[src/musa/core/stream.cpp:1286-1307]
- 生成与构建缺口：[src/driver/mu_wrappers_generated.cpp:1-11]、[src/loader/CMakeLists.txt:15-25]、[CMakeLists.txt:146-154]、[CMakeLists.txt:167-169]
- 转发与导出：[src/loader/loader.cpp:119-123]、[src/driver/mu_entry.cpp:2146-2195]、[src/musa_shared_include/export_table.h:859-866]

## 未解决问题

1. 私有 M3D 子模块内部（queue/cmdBuffer/页分配）仍无行级证据。
2. 测试与 Demo 源码缺失，所有"跑一遍"的项无法在本快照闭环。
3. 无性能基线，性能相关判断全部为静态分析。

## 下一步阅读建议

先读 [`rca-splaytree-memmgr.md`](rca-splaytree-memmgr.md) 确认修复方案，再按 [`testing-recipes.md`](testing-recipes.md) 决定验证落在哪一层。
