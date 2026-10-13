# Graph 执行（M08）

- 源码范围：`src/driver/mu_graph.cpp`、`src/driver/mu_stream.cpp`（capture 入口）、`src/musa/core/graph.cpp`、`src/musa/core/graph.h`、`src/musa/core/graph/graph1`、`src/musa/core/graph/graph2`、`src/musa/core/node/*`、`src/musa/core/command/graphCommand.*`、`src/musa/core/stream.cpp`（capture 与 launch 段）、`src/musa/core/context.cpp` 的 graph 创建/捕获部分
- 对应版本：`5.3.0 develop`（顶层 CMakeLists 第 33-34 行的两个 set 赋值）[CMakeLists.txt:33-34]
- 文档目的：graph：总览：职责、结构、关键结论与文档导航。
- 适用范围：M08 模块的《Graph 执行（M08）》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：静态源码已确认；未编译、未在目标机运行，硬件与并发行为未验证
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)

## 一句话职责

Graph 执行把「依赖图」从描述变成可提交的命令序列：负责 `capture / add-node -> instantiate -> launch -> update -> destroy` 全链路，是命令式 stream 工作流之外的第二套依赖调度模型。

## 阅读入口

1. 先读本页和 [`source-map.md`](source-map.md)（文件清单与行数）。
2. 再读 [`implementation.md`](implementation.md)（实现要点与分支矩阵）、[`call-chains.md`](call-chains.md)（八条调用链）。
3. 数据结构与所有权见 [`data-structures.md`](data-structures.md)，执行流见 [`execution-flows.md`](execution-flows.md)。
4. 逐段行号证据见 [`line-level-analysis.md`](line-level-analysis.md)，图示见 [`diagrams.md`](diagrams.md)。
5. 契约见 [`interfaces.md`](interfaces.md)，用法骨架见 [`examples.md`](examples.md)。
6. 风险见 [`risks-and-debt.md`](risks-and-debt.md)，改动前的检查清单见 [`development-guide.md`](development-guide.md)，验证手段见 [`testing.md`](testing.md)。
7. 设计取舍与不变量见 [`design.md`](design.md)。

## 边界

- **本模块拥有**：`Graph`/`GraphExec`/`GraphExec2` 对象状态、18 种节点类型与依赖拓扑、`GraphCommand`/`UniversalManager` 的提交逻辑、stream capture 状态机。
- **本模块不拥有**：公共 ABI 契约（M02 的导出表与句柄布局）、HAL/内核驱动的最终提交实现（M09）、MUpti 观测实现（M10）。
- **证据边界**：下列结论来自源码行号，不等同于目标机运行成功；本页没有声称任何 graph 用例已跑通。

## 核心证据

- 节点类型枚举 `MUgraphNodeType` 共 18 个取值（`KERNEL=0` 到 `MEM_TRANSFER=17`）：[src/musa_shared_include/musa.h:1964-2000]
- `muapiGraphCreate` 取 current context 后调用 `CreateGraph`：[src/driver/mu_graph.cpp:1188-1208]
- 手动建图入口 `muapiGraphAddNode` 按 `nodeParams->type` 分派到 13 类 add 函数：[src/driver/mu_graph.cpp:1012-1069]
- 实例化入口 `muapiGraphInstantiate_v2` 先 `ValidateGraph` 再 `CreateGraphExec`：[src/driver/mu_graph.cpp:2304-2324]
- 提交入口 `muapiGraphLaunch` 校验句柄后转 `CmdLaunchGraph`：[src/driver/mu_graph.cpp:2374-2401]
- capture 入口 `muapiStreamBeginCapture_v2` 先建图再进 capture：[src/driver/mu_stream.cpp:391-421]
- v1/v2 分叉开关在 `CreateGraphExec`：[src/musa/core/context.cpp:1420-1450]

## 深度状态

正常路径（创建、加节点、实例化、launch、destroy）已建立行级证据；分支矩阵、失败回滚、并发与清理见子文档。凡标注「推断」的结论是静态读码得到的因果链，未在运行时复现；凡标注「未验证」的项目需要目标机或完整测试补证。

## Demo 覆盖

- **真实经过本模块的 Demo 步骤**：无。D01 不调用任何 `muGraph*` 或 capture API，trace §1–§2 也没有 graph 步骤（见 [`../../80-demos/D01-muinfo/execution-trace.md`](../../80-demos/D01-muinfo/execution-trace.md)）。
- **候选但未覆盖**：D04 `childGraph`、D05 `conditionalNode`、D06 `allocnode`、D07 `addnode` 计划覆盖 M08，D02/D03 不覆盖 graph；但四者都只是候选——本快照无 `tests/` 目录，入口未核实。
- **尚无 Demo 覆盖的重要实现**：
  1. 创建 `muapiGraphCreate` [src/driver/mu_graph.cpp:1188-1208]；
  2. 手动建图 `muapiGraphAddNode` [src/driver/mu_graph.cpp:1012-1069]；
  3. 实例化 `muapiGraphInstantiate_v2` [src/driver/mu_graph.cpp:2304-2324]；
  4. 提交 `muapiGraphLaunch` [src/driver/mu_graph.cpp:2374-2401]；八条调用链见 [`call-chains.md`](call-chains.md)。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/driver/mu_graph.cpp`（4 处，例：[src/driver/mu_graph.cpp:1188-1208]）
- `src/musa_shared_include/musa.h`（1 处，例：[src/musa_shared_include/musa.h:1964-2000]）
- `src/driver/mu_stream.cpp`（1 处，例：[src/driver/mu_stream.cpp:391-421]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
