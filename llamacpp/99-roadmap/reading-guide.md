# 阅读路线

- 文档目的：解释 99-roadmap/reading-guide.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/llamacpp HEAD 093a2f86c（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 99-roadmap/reading-guide.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


1. **用户入口：** `README.md` -> [项目概览](../00-overview/project-overview.md)。
2. **底层：** [M02 llama runtime](../01-modules/M02-llama-runtime/README.md) -> [M01 ggml/backend](../01-modules/M01-ggml-backend/README.md)。
3. **应用：** [M03 common](../01-modules/M03-common-app-infra/README.md) -> [M05 CLI/app](../01-modules/M05-cli-app/README.md) -> [D02](../80-demos/D02-cli-inference/README.md)。
4. **服务：** [M04 server](../01-modules/M04-server/README.md) -> [D01](../80-demos/D01-server-chat/README.md) -> [跨模块调用链](../90-cross-module/call-chains.md)。
5. **工程化：** [M08](../01-modules/M08-build-tests-ci/README.md) -> [测试配方](testing-recipes.md) -> [风险登记](risk-register.md)。
6. **专项：** 需要多模态读 M06；模型格式读 M07；前端读 M09。

---

**页尾：** 阅读顺序按依赖和调用链设计，不表示模块重要性排名。

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
