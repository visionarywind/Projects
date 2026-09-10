# 分析状态

- 文档目的：记录知识库批次、版本、覆盖范围和断点
- 适用范围：整个工作区
- 对应源码版本：GE `47020afc8`；ACL `d26308d3`；Runtime `dae460b78`；Driver `6e2914c`
- 证据状态：已确认（Git 状态与静态盘点）；细节覆盖部分完成
- 最后更新：2026-09-10
- 前置阅读：项目根 README
- 后续阅读：`90-cross-module/end-to-end-flows.md` → `99-roadmap/next-steps.md`

## 当前批次

第二批已完成 Runtime 内存与资源生命周期的源码级专题，并将关键事实回写到 Runtime 与跨模块文档。重点区分 KernelMemoryPool、SOMA stream-ordered pool 和普通 device/Host memory 三条路径，核对了 Segment 状态、PoolRegistry ownership、AICPU 异步 ABI、Driver V3 free 的不回滚窗口，以及 `TryToReuse` 和隐式 trim 的当前 no-op 实现。四仓库源码、构建、测试和真实设备行为仍未在本环境执行验证。

## 已完成文档

- `00-overview/project-overview.md`
- `00-overview/architecture.md`
- `00-overview/analysis-state.md`
- `00-overview/evidence-index.md`
- `00-overview/build-and-deploy.md`
- `00-overview/glossary.md`
- `00-overview/design-principles.md`
- `00-overview/runtime-model.md`
- `00-overview/global-data-flow.md`
- `00-overview/dependency-map.md`
- `00-overview/global-error-model.md`
- `00-overview/decision-log.md`
- 四个模块的 README/design/source-map/interfaces/data-structures/call-chains/diagrams/line-level-analysis/examples/testing/development-guide/risks-and-debt
- `01-modules/M03-runtime/memory-pool-analysis.md`
- `90-cross-module/memory-and-resource-lifecycle.md`
- `90-cross-module/*`
- `99-roadmap/*`

## 版本证据

| 仓库 | 分支 | HEAD | 工作树 |
|---|---|---|---|
| GE | master | `47020afc8` | clean，跟踪 origin/master |
| ACL | master | `d26308d3` | clean，跟踪 origin/master |
| Runtime | master | `dae460b78` | clean，跟踪 origin/master |
| Driver | master | `6e2914c` | clean，跟踪 origin/master |

## 覆盖情况

| 指标 | 当前状态 | 缺口 |
|---|---|---|
| 目录覆盖 | 四个顶层模块已归类 | GE/Runtime/Driver 内部大量子模块未逐一展开 |
| 模块覆盖 | 4 个稳定模块均有入口文档 | GE Compiler/Executor、ACL Model、Driver 调度仍待源码级展开 |
| Runtime 内存 | KernelMemoryPool、SOMA、普通 device/Host memory 的关键路径已覆盖 | 全部 SoC/后端分支和设备侧完成语义待验证 |
| 入口覆盖 | 构建、主要 API、GE Session、Runtime API 门面、Driver HAL/SDK 已覆盖 | 具体设备产品分支待补 |
| 流程覆盖 | 初始化、模型执行、设备调用、队列/HDC、内存生命周期概要 | 完整异常/清理链待继续追踪和实验 |
| 符号覆盖 | 关键入口已有行号 | 全量符号映射未完成 |
| 测试覆盖 | 测试目录、命令和限制已记录 | 未在本环境执行，真实覆盖率未知 |
| 证据覆盖 | 重要摘要有源码/README/CMake 引用 | 个别跨仓推断需补二进制或调用图证据 |
| 图示覆盖 | 总体和端到端核心图已提供 | 各子模块详细状态图待补 |
| 开发场景覆盖 | 小功能/API/测试/调试配方已给出通用步骤 | 具体 API 需按接口逐项补 |

## 未解决问题

1. 需要配套发布版本矩阵来确认四仓版本兼容关系。
2. 需要设备环境验证 `npu-smi`、构建、安装、样例、UT 和错误码。
3. 需要继续从 Runtime `Api`、HAL 表和 SDK-driver 追踪具体功能的完整调用链。
4. 需要继续补齐 GE Compiler/Executor、ACL Model/单算子和 Driver Queue/HDC/esched 的源码级链路。
5. SOMA 重用策略、隐式 trim 契约、异步失败回滚和统计一致性需要专门测试确认。

## 下一批起点

优先分析 Runtime `PrimaryContextRetain`、Stream/Event/Task 资源路径；随后分析 M04 Driver 的 HAL dispatch、HDC、Queue、esched；最后补充 GE Compiler/Executor V1/V2 和 ACL 模型/单算子路径。

## 相关文档

- [evidence-index.md](evidence-index.md)
- [../01-modules/M03-runtime/memory-pool-analysis.md](../01-modules/M03-runtime/memory-pool-analysis.md)
- [../90-cross-module/memory-and-resource-lifecycle.md](../90-cross-module/memory-and-resource-lifecycle.md)
- [../90-cross-module/change-impact-map.md](../90-cross-module/change-impact-map.md)

## 源码证据摘要

版本来自四个子仓库的 `git status`/`git log`；目录职责和 Runtime 内存结论来自对应仓库源码、README 与构建边界。本文不把文档命令写成已通过的测试结果。

## 下一步阅读建议

从“下一批起点”开始，不重复读取已覆盖的入口；先阅读 Runtime 内存专题，再沿其证据进入 HAL 和 Driver SVM 实现。
