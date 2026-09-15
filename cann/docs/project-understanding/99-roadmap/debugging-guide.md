# 开发实践：调试指南

- 文档目的：解释 99-roadmap/debugging-guide.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cann HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：流程建议；未执行硬件验证
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 99-roadmap/debugging-guide.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。

## 通用定位

1. 固定版本、SoC、Toolkit、Driver/Firmware 和构建选项。
2. 记录 API、阶段、线程、设备、Context、Stream、Graph/Model 和原始错误码。
3. 找到第一个失败边界，而不是只看顶层 `ACL_ERROR`。
4. 沿 `ACL → Runtime → HAL → SDK/kernel` 或 `GE → Runtime` 反查。
5. 检查异步完成、buffer/handle 生命周期和清理顺序。

## 断点建议

- GE：`GEInitialize`、`GeSession::Impl`、`ModelV2Executor::Load/Execute/UnLoad`。
- ACL：`aclrtSetDeviceImpl`、模型/Stream/Memory 对应 `*Impl`。
- Runtime：`rtSetDevice`、`Api::Instance()` 分发、句柄验证宏。
- Driver：`halGetSocVersion`、`drvHdcClientCreate/Destroy`、queue open/release、HDC connect/close。

## 结果记录

区分参数错误、状态错误、资源错误、设备/固件错误和模型错误；若没有 NPU，只记录静态分析或构建/UT 的实际结果。

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
