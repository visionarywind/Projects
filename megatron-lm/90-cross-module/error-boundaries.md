# 错误边界

- 文档目的：解释 90-cross-module/error-boundaries.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/megatron-lm HEAD 3703d4e33（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 90-cross-module/error-boundaries.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


| 边界 | 典型错误 | 传播 |
|---|---|---|
| distributed init | CUDA/NCCL/rank env | 初始化阶段直接失败 |
| model builder | spec/import/非法组合 | setup_model 阶段失败 |
| dataset build | 文件、tokenizer、index helper | build iterator 阶段失败 |
| schedule | shape、PP stage、unsupported overlap | train_step 失败 |
| optimizer | overflow、state mismatch | step 返回 skip 或抛错 |
| checkpoint | 路径、metadata、配置不兼容 | save/load 失败 |
| inference | capacity、KV handoff、server rank | request 或 server loop 失败 |

当前文档没有实测异常输出；只记录源码中显式 assert、RuntimeError 和调用边界。

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
