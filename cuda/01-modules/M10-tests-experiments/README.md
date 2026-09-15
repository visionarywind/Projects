# M10 Tests/Experiments：构建适配、测试聚合与实验资料

- 文档目的：解释 01-modules/M10-tests-experiments/README.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M10-tests-experiments/README.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


- **职责**：把 `libcuda_test`、各测试 binary、dispatcher 和 DVS 结果聚合起来，并保存用于回归/性能/bug 研究的 experiments。
- **性质**：M10 是验证层，不是 runtime 实现层；测试源码可证明调用意图和断言，不能证明本工作区已经在 GPU 上执行。
- **当前状态**：构建依赖和聚合逻辑已静态确认；nvmake、nvcc、DVS、GPU 和 sudo persistence mode 均未运行。

```text
cuda_test.nvmk
 → libcuda_test dependency
 → per-test binary
 → generated cuda_test_list.cpp
 → dispatch_main.cpp
 → run_tests.py
 → DVS Pass/Failure/Waived aggregate
```

## 结果语义与边界

- **静态确认**：make rule 先检查 `TEST_BINARY`、case name 和 disposition，再生成 `cuda_test_list.cpp`；生成的 dispatcher 负责列 GPU 或运行测试，不能把生成列表当成测试已执行（[tests/cuda_test/cuda_test.nvmk:60-120,140-153]；[tests/cuda_test/dispatch_main.cpp:22-50]）。
- **静态确认**：runner 并行消费 stdout/stderr，按每测试 timeout 处理子进程；没有可解析结果的测试会被视为 failure，`Waived` 与 Pass/Failure 分开聚合，零 Pass 或存在 failure 时返回非零（[tests/run_tests.py:8-73,85-132,171-189,191-245]）。
- **未验证**：Linux 入口还可能调用 persistence mode、`nvidia-smi` 和 `cuda_test --listGpus --forceRun`，这些是运行环境动作，不是本知识库的验证结果。

experiments 中的 cubin、SASS、日志和报告只能作为历史/边界线索；源码级证据与实际硬件结果必须分开记录。

## 文档元数据（规范补充）

- 文档目的：说明 `01-modules/M10-tests-experiments/README.md` 的源码分析范围、结论和维护入口。
- 适用范围：当前项目对应模块/入口的静态源码与测试分析。
- 对应源码版本：以本项目 `00-overview/analysis-state.md` 或同页版本字段为准。
- 证据状态：静态源码证据；未执行的构建、测试、GPU、网络或多进程行为保持“未验证”。
- 最后更新：2026-09-15
- 前置阅读：本项目根 README 与 `00-overview/analysis-state.md`。
- 后续阅读：本模块/示例的实现、测试和风险页面。

## 深度审计

| 分析对象 | 入口落地 | 正常路径 | 分支 | 异常 | 清理 | 数据生命周期 | 执行上下文 | 行级证据 | Demo 映射 | 状态/缺口 |
|---|---|---|---|---|---|---|---|---|---|---|
| `01-modules/M10-tests-experiments` | 已完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 已映射或不适用 | 部分完成：动态行为、边界或专用变体仍需验证 |

## 相关文档
- [项目入口](../../README.md)
- [分析状态](../../00-overview/analysis-state.md)
- [源码证据索引](../../00-overview/evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](../../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
