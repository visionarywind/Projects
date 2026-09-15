# 开发工作流

- 文档目的：提供与仓库真实入口匹配的修改、验证和文档维护流程。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/concurrency-queue HEAD 683b9e3（2026-09-15 只读确认）。
- 证据状态：步骤依据源码和构建布局；不是仓库内强制流程。
- 最后更新：2026-09-10
- 前置阅读：[快速入口](quick-start.md)
- 后续阅读：[调试指南](debugging-guide.md)
## 结论摘要

本页聚焦 99-roadmap/development-workflow.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。

## 阶段 1：界定

确认 HEAD、目标平台、改动模块和稳定 API 契约。区分“实现细节优化”和“公开语义改变”。

## 阶段 2：追踪

从公共 API 沿调用链追到 placement-new、原子发布/领取、析构、empty/recycle 或平台 wait；不要停在 wrapper。

## 阶段 3：实现

保持 C++11 和条件编译；异常路径与正常路径同样处理；不修改 token/queue 生命周期假设而不更新文档。

## 阶段 4：验证

按影响图选择 unit、threaded、bulk、exception、blocking、C API、Relacy/CDSChecker、native/RISC-V 和 benchmark。命令没执行就标记未验证。

## 阶段 5：交付

检查 CMake interface/install 和 legacy Make 是否都仍正确；更新行号证据、analysis-state、链接和覆盖矩阵。

## 不在本流程中假定

没有仓库证据时，不假定 clang-format、clang-tidy、服务部署、发布流水线、性能阈值或第三方内部修复流程存在。

## 相关文档
- [项目入口](../README.md)
- [分析状态](../00-overview/analysis-state.md)
- [源码证据索引](../00-overview/evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
