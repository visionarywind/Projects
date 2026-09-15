# M06 故障与验证清单

- 文档目的：解释 01-modules/M06-inference-and-tools/execution-flows.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/megatron-lm HEAD 3703d4e33（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M06-inference-and-tools/execution-flows.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


| 现象 | 首查位置 | 静态可得结论 |
|---|---|---|
| engine 初始化失败 | `StaticInferenceEngine.__init__` | 可能回退 legacy static |
| request 超出容量 | dynamic engine/scheduler | admission 受 max tokens/requests 限制 |
| KV handoff 不可用 | dynamic engine handoff stubs | 需 disaggregation engine |
| logits 未 gather | GPTModel forward | inference mode 要求 gather |
| 服务无响应 | server rank gate、distributed broadcast | 需检查首 stage/TP/EP rank 和进程同步 |

验证应分别覆盖单进程 offline、动态 batching、多 rank inference、server request、CUDA graph 和 disaggregation；本知识库当前仅完成源码阅读。

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
