# M07 测试与开发指南

- 文档目的：解释 01-modules/M07-kernel-syscall/testing.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M07-kernel-syscall/testing.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## 测试

- `basic_sanity:kernel` 是当前最直接的 CNP/内建 device launch 使用者；通过 `CNP_SUPPORTED` 和设备 capability 分支决定预期结果（静态确认：[tests/cuda_test/basic_sanity.cu:127-145,194-225]）。
- `bik/` 下有 kernel memcpy 和 memory utility 测试；文件存在但未执行。
- stream/channel 测试可间接验证 syscall/launch 产生的提交活动，但不单独证明 device-side ABI。

## 修改配方

1. 新 syscall：更新 CUI resource setup、tracking list、所有架构实现和生成构建输入。
2. 修改 CNP：同时核对 `cuicnp.c`、launch queue、stream/channel 提交、错误和 context destroy。
3. 修改生成 kernel：标明输入、生成器、目标架构和产物，不能只提交二进制。
4. 调试：先检查 launchData/QMD/constant-bank，再检查 device-side syscall buffer，最后到 backend/RM；当前均未运行验证。

## 未验证

需要外部 nvmake、compiler、GPU 和匹配驱动才能构建或执行本模块；本知识库未执行。

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
