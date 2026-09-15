# 分析状态

- 文档目的：记录知识库批次、版本、覆盖范围和断点
- 适用范围：整个工作区
- 对应源码版本：GE `8ee1b040a`；ACL（Runtime 仓库内 ACL API）随 Runtime `50be4c922`；Runtime `50be4c922`；Driver `9773369137`；SHMEM `ea981bd`
- 证据状态：已确认（Git 状态与静态盘点）；细节覆盖部分完成
- 最后更新：2026-09-15
- 前置阅读：项目根 README
- 后续阅读：`90-cross-module/end-to-end-flows.md` → `99-roadmap/next-steps.md`

## 当前批次

第二批已完成 Runtime 内存与资源生命周期的源码级专题，并将关键事实回写到 Runtime 与跨模块文档。第三批进一步完成 Driver ordinary memory cache 的 V2/V3 专题与 M04 索引同步：确认 V2 heap/多棵红黑树、V3 cache_allocator/gen_allocator、exact/upper-bound 复用、split/merge、shrink 和产品构建选择。重点区分 KernelMemoryPool、SOMA stream-ordered pool、Driver ordinary cache 三条路径；`TryToReuse` 和隐式 trim 的当前 no-op 状态仍保持明确。四仓库源码、构建、测试和真实设备行为仍未在本环境执行验证。

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
- `90-cross-module/pooling-and-resource-management.md`：按当前四仓提交复核 Runtime MemoryPool/SOMA、Driver V3 cache、GE graph memory reuse 和 SHMEM MemSegment/heap。

## 版本证据

| 仓库 | 分支 | HEAD | 工作树 |
|---|---|---|---|
| GE | master | `8ee1b040a` | clean，跟踪 origin/master |
| ACL | master | `50be4c922` | clean，跟踪 origin/master |
| Runtime | master | `50be4c922` | clean，跟踪 origin/master |
| Driver | master | `977336913` | clean，跟踪 origin/master |

## 覆盖情况

| 指标 | 当前状态 | 缺口 |
|---|---|---|
| 目录覆盖 | 四个顶层模块已归类 | GE/Runtime/Driver 内部大量子模块未逐一展开 |
| 模块覆盖 | 4 个稳定模块均有入口文档 | GE Compiler/Executor、ACL Model、Driver 调度仍待源码级展开 |
| Runtime 内存 | KernelMemoryPool、SOMA、普通 device/Host memory 的关键路径已覆盖 | 全部 SoC/后端分支和设备侧完成语义待验证 |
| Driver ordinary cache | V2/V3 用户态 cache 的主要结构、分配、释放和 shrink 已覆盖 | 当前构建产物、闭源内核/固件和物理页行为待验证 |
| 四仓池化与资源管理 | Runtime 固定池/SOMA、Driver cache、GE offset reuse、SHMEM VA/slice 已形成统一专题；补充 first-fit/编译期 offset 与运行期 backing 的边界 | 产品宏、异步设备行为、GE executor→Runtime 地址闭环、跨节点和 CRIU 仍待验证 |
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
6. Driver V2/V3 的当前构建产物、cache 命中统计、BUSY recycle 完成时机和内核物理 backing 路径需要目标环境验证。
7. 当前四仓 checkout 已更新为 Runtime `50be4c922`、Driver `9773369137`、GE `8ee1b040a`、SHMEM `ea981bd`；旧模块文档中的版本和行号需按证据索引增量复核。

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
