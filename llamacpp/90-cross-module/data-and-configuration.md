# 数据与配置边界

- 文档目的：解释 90-cross-module/data-and-configuration.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/llamacpp HEAD 093a2f86c（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 90-cross-module/data-and-configuration.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


| 数据 | 产生处 | 消费处 | 生命周期/风险 |
|---|---|---|---|
| GGUF metadata/tensor | loader/file mapping | model/architecture | model 生命周期；格式和 tensor name 必须匹配 |
| vocab/tokenizer | model load | common/server/CLI | model/context 使用期间有效 |
| prompt JSON | HTTP | tokenizer/task | HTTP worker 转成 native task |
| batch | server slots/context | graph/decode | 一次或多次 decode 的临时聚合 |
| logits/embedding | graph/backend | sampler/HTTP/CLI | output flag 决定需要计算的位置 |
| task/result | HTTP/context queue | 对端 reader | move/queue 转移所有权 |
| SSE bytes | response producer | ring consumer | 有界缓存、TTL GC、可取消 |
| model cache | common/download | server router/converter | 外部文件能力必须考虑默认关闭 |

server 文档明确不建议把 raw JSON 传入 slot；应尽早解析为 native 类型。[tools/server/README-dev.md:90-103] completion handler 的实际转换点是 `tokenize_input_prompts`、schema evaluation 和 task construction。[tools/server/server-context.cpp:4291-4342]

配置层次：CMake 选编译/平台能力，model params 选设备和加载方式，context params 选 batch/cache/attention，server params 选 HTTP/slot/router 行为，request JSON 选一次请求参数。层次关系是源码和 CMake 的归纳。

---

**页尾：** 不把运行时 request 参数写成编译时 ABI。

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
