# 阅读路线

- 文档目的：解释 99-roadmap/reading-guide.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/sglang HEAD 78be4b50af（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 99-roadmap/reading-guide.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## 初学者路线

1. `00-overview/project-overview.md`、`architecture.md`、`glossary.md`。
2. `01-concepts/01-推理服务基础概念.md` 和 `02-Transformer与KV-Cache.md`。
3. M03：理解输入如何 tokenize、建立 `ReqState`、跨 IPC 和回收。
4. M04：理解 waiting/running、PrefillAdder、ScheduleBatch 和 retraction。
5. M08：理解 request row、KV slots 和 Radix prefix。
6. M05：理解 ForwardBatch、worker、forward 和 sample。
7. M09/M10：理解 graph/eager、attention metadata、grammar 和 sampling。
8. M06/M07：回看模型加载和并行拓扑如何在启动时建立上述资源。
9. M11-M14：按需阅读 speculative、multimodal、disaggregation/HiCache 和 MoE/quant/LoRA 的扩展契约。
10. M15-M18：阅读 IPC、设备/kernel、Rust/router/gateway 和测试系统，理解进程、硬件、网络与验证边界。
11. D01 和 `90-cross-module/`：把模块拼成一条真实请求轨迹。

## 修改者路线

- 改输入或输出：M03 → interface-contracts → M04/M15；
- 改 admission/KV：M04 → M08 → M09 → retraction tests；
- 改 forward：M04 → M05 → M09 → model/attention backend；
- 改采样/grammar：M10 → M05 → M03 output/finish；
- 改 rank/group：M06 → M07 → M05 model layers；
- 改配置：configuration-impact-map → affected role/process → runtime path。

## 证据规则

先看函数真实调用者和被调用者，再写结论；每条关键结论给出相对路径和行号；静态推断与实际执行分开。

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
