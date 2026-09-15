# Demo 注册表

- 文档目的：解释 80-demos/demo-registry.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/sglang HEAD 78be4b50af（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 80-demos/demo-registry.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


`80-demos/` 只收录真实存在于 checkout 中的示例、CLI 或测试入口。Demo 文章必须继续追踪到状态改变、资源访问或外部输出，不能停在 `sgl.Engine.generate` 这一层。

## D01：离线批量 Engine 推理

- 源文件：`examples/runtime/engine/offline_batch_inference.py`
- 入口：`argparse` → `ServerArgs.add_cli_args` → `ServerArgs.from_cli_args` → `main`。
- 主要链路：构造 `sgl.Engine` → 启动 scheduler/detokenizer → `Engine.generate` → TokenizerManager tokenize/IPC → Scheduler continuous batching → ModelRunner forward/sample → output state → `print`。
- 适合覆盖：CLI 参数、`ServerArgs`、Python API、spawn 子进程、batch request、采样参数、返回值和 shutdown。
- 当前状态：已创建深度文章；真实模型启动、checkpoint 加载、GPU 输出均未验证。
- 目标文章：`D01-offline-engine/01-离线批量推理.md`。

## D02：协议与结构化输出静态链路

- 来源入口：`python/sglang/srt/entrypoints/http_server.py`、`python/sglang/srt/entrypoints/openai/serving_chat.py`。
- 主要链路：OpenAI chat request → schema/工具参数校验 → chat template 与 `GenerateReqInput` → grammar/structured-output 参数 → TokenizerManager/Scheduler → sampler/grammar mask → SSE 或 JSON response。
- 适合覆盖：协议适配、tool call、structured output、streaming、错误 envelope 和 disconnect/abort。
- 当前状态：静态源码 Demo，未运行服务、模型、grammar backend 或端到端输出。

## D03：Speculative draft/verify 静态链路

- 来源入口：`python/sglang/srt/speculative/` 下 `BaseSpecWorker`、DFlash/EAGLE/MTP 等实现。
- 主要链路：draft proposal → target verify → accept/reject 与 bonus token → rejected suffix/KV 状态 → 下一轮 draft 或普通 decode。
- 适合覆盖：候选 token 提交协议、KV ownership、grammar/overlap/graph 变体和异常回滚。
- 当前状态：静态源码 Demo，未运行 draft/target worker、CUDA Graph、GPU 或接受率 benchmark。


每篇文章至少记录：

1. 运行前提和明确命令；
2. 参数如何进入配置和对象构造；
3. wrapper/adapter/binding 后真正执行状态改变或资源访问的源码位置；
4. 同步、异步、进程和设备边界；
5. 输出如何从内部状态变成用户可见结果；
6. 失败、abort 和 cleanup 路径；
7. 可设置断点的函数和变量；
8. 实际执行与“未验证”范围。

## 未验证约定

本注册表不表示 Demo 已运行。没有在当前环境执行的模型下载、权重加载、多 GPU 通信、CUDA Graph capture 和端到端输出，都必须在对应文章中标记为“未验证”。

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
