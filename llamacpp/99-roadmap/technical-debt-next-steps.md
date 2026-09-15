# 技术债与后续路线

- 文档目的：解释 99-roadmap/technical-debt-next-steps.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/llamacpp HEAD 093a2f86c（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 99-roadmap/technical-debt-next-steps.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## 当前知识库缺口

1. M02 的 `llama_context::decode` 内部 graph builder 尚未逐阶段记录。
2. M01 仅覆盖统一 scheduler/device 边界，未覆盖 CUDA/Metal/MUSA/Vulkan 等具体 kernel。
3. M04 router、MCP、sleep、resumable stream 的实现仍以开发文档和关键片段为主。
4. M06 各视觉/音频模型和媒体 subprocess 未做真实资产 Demo。
5. M09 缺少 routes -> stores -> services -> API 的源码级矩阵。
6. 未执行 server pytest、UI tests、`test-backend-ops` 或真实模型命令；GGUF unittest 已执行但因缺少 `pytest` 导入依赖而失败。
7. CTest `-N` 已执行并确认注册信息，但测试 target 尚未构建；链接、行号和证据只做了人工首版整理，尚未自动审计。

## 下一步优先级

1. 在固定模型和 CPU build 上执行 D01/D02，保存命令、环境和结果。
2. 深入 `llama_context::decode`、构造函数、KV memory 和一个具体 `src/models/*.cpp`。
3. 在一个可用 accelerator 上走 `test-backend-ops` 和 benchmark。
4. 对 server streaming/sleep/router 做独立 call-chain 和错误清理审计。
5. 用 UI 真实 route/service 和 e2e 补齐 M09。
6. 每次源码升级先更新 [analysis-state](../00-overview/analysis-state.md)，再更新受影响引用。

## 审计结论

当前交付达到“静态阶段 0-6 首版”而非“全部源码深度完成”：关键公共 wrapper 未作为终点，已追踪到 model load、context creation、scheduler 和 server decode；Demo 路径已完整描述但未运行。该限制是已确认事实，不应隐藏。

---

**页尾：** 后续工作以实际输出补充，不用预填性能或通过状态。

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
