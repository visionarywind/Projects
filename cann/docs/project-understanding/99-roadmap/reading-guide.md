# 开发实践：阅读指南

- 文档目的：解释 99-roadmap/reading-guide.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cann HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 99-roadmap/reading-guide.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## 按问题选择入口

| 问题 | 阅读顺序 |
|---|---|
| 模型编译失败 | M01 README → GE architecture → Parser/Compiler → error boundaries |
| 模型运行失败 | M01 call-chains → M03 interfaces → M04 HDC/Queue |
| `aclrtSetDevice` 失败 | M02 line-level → M03 `api_c_device.cc` → `Api::SetDevice` → M04 DMS |
| Stream/Event/内存问题 | M02 data structures → M03 Runtime objects → M04 queue/通信 |
| 驱动/设备节点问题 | M04 source-map → Queue/HDC/esched → Runtime caller |
| 性能问题 | cross-module performance → GE planning → Runtime queue → Driver |

## 阅读原则

先读接口契约，再读实现和调用者；区分已确认源码事实、架构推断和未验证硬件行为；跨仓变更必须看 change-impact-map。

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
