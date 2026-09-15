# Demos 注册表

- 文档目的：解释 80-demos/demo-registry.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/llamacpp HEAD 093a2f86c（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 80-demos/demo-registry.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


| ID | Demo | 目标 | 覆盖模块 | 实际状态 |
|---|---|---|---|---|
| D01 | Server 非流式 Chat Completion | 从 HTTP JSON 到生成结果 | M03、M04、M02、M01（并可涉及 M06） | 静态解剖，未运行 |
| D02 | CLI 文本推理 | 最短用户侧推理路径 | M05、M03、M02、M01 | 静态解剖，未运行 |

D01 是主 Demo，因为它的测试既覆盖 prompt/template、usage、response，又覆盖 server queue/decode；D02 用于在进入并发 server 前理解最小 consumer。这是文档组织推断，不是性能优先级。

---

**页尾：** Demo 状态只有执行命令并记录输出后才可改为“已运行”。

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
