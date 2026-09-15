# M06 源码地图

- 文档目的：解释 01-modules/M06-inference-and-tools/source-map.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/megatron-lm HEAD 3703d4e33（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M06-inference-and-tools/source-map.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


| 文件 | 重点 |
|---|---|
| `core/inference/engines/abstract_engine.py` | engine 抽象接口 |
| `engines/static_engine.py` | static 兼容层和 fallback |
| `engines/dynamic_engine.py` | 请求调度、生命周期和 dynamic batch |
| `contexts/static_context.py` | 固定容量推理状态 |
| `contexts/dynamic_context.py` | 动态 token/block 状态 |
| `contexts/kv_block_allocator.py` | KV block 分配 |
| `sampling/` | torch/FlashInfer sampling |
| `model_inference_wrappers/gpt/gpt_inference_wrapper.py` | GPT 推理适配 |
| `text_generation_controllers/text_generation_controller.py` | tokenize/detokenize/输出 |
| `tools/run_text_generation_server.py` | 服务启动 |
| `examples/inference/` | offline/advanced inference examples |

推荐代表样例：先读 `examples/inference/offline_inference.py`，再读 dynamic advanced example，最后读 server。

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
