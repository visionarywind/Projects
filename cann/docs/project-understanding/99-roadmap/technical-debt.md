# 技术债务

- 文档目的：解释 99-roadmap/technical-debt.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cann HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：待办事项来自当前覆盖缺口
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 99-roadmap/technical-debt.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。

## 代码和调用图

- 补齐 Runtime `Api::SetDevice`、Context、Stream、Memory、Task 到 Driver HAL 的静态路径。
- 按设备、内存、Queue、HDC、esched 分别建立 HAL→ioctl→内核映射。
- 补齐 GE Compiler Pass/Engine/Planner、V1/V2 和 ACL Model/Single-op 源码地图。

## 契约和验证

- 生成公共头文件、导出符号、错误码和结构体 ABI 清单。
- 形成 Context/Stream/Event/Buffer/Session 的线程安全和所有权表。
- 建立 Toolkit/Driver/Firmware/SoC 兼容矩阵。
- 建立无设备 mock、UT、跨仓集成和真实设备 CI 分层。

## 文档维护

每次源码版本变化更新 `analysis-state.md`、证据索引、调用链和变更影响地图；不保留无法追溯来源的“已确认”结论。

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
