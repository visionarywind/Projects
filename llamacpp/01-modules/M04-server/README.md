# M04 - llama-server

- 文档目的：解释 01-modules/M04-server/README.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/llamacpp HEAD 093a2f86c（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M04-server/README.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


> 状态：首版实现层；D01 对应模块。server 修改前必须阅读 `tools/server/README-dev.md`。

## 组件和线程

server 有 inference/router 两种模式；inference 模式由 `server_context` 持有 model/context/slots，HTTP 由 `server_http_context` 提供，`server_queue` 和 `server_response` 传递 task/result。[tools/server/README-dev.md:37-74] context 在专用单线程运行，HTTP worker 负责 JSON、模板、tokenize 和 response formatting。[tools/server/README-dev.md:86-103]

## Completion 正常路径

`handle_completions_impl` 创建 response generator，解析 prompt，tokenize，构造 `server_task` 并放入 reader/queue。[tools/server/server-context.cpp:4257-4344] context 批处理 slots，在 batch 有 token 后通过 queue yield 调用 `llama_decode`；需要 output 时同步 context。[tools/server/server-context.cpp:3645-3686] 非流式等待所有 result，流式先发首条再逐条格式化 SSE。[tools/server/server-context.cpp:4348-4469]

## 错误路径

decode 的 context exceeded、invalid batch 和 compute error 会向 processing slots 发送错误、释放 slot、清理 prompt，并停止本轮；无法找到 KV 空间时尝试清理 idle slot 或缩小 batch。[tools/server/server-context.cpp:3688-3735] HTTP wrapper 把 invalid_argument 变成 400，其它异常变成 500。[tools/server/server.cpp:52-85]

## 其他生命周期

resumable streaming 用 producer/ring/consumer 和 GC 保存 SSE；启动后必须在清理时停止 manager。[tools/server/README-dev.md:121-149] sleep 模式可释放 context/mtmd，HTTP read-only endpoints 使用 cached responses，不唤醒 server。[tools/server/README-dev.md:294-322]

## 测试与修改影响

Python suite 自动启动 server；主测试断言状态码、completion id、usage、内容和 finish reason，stream 测试断言 SSE 顺序和 id。[tools/server/tests/unit/test_chat_completion.py:28-51] [tools/server/tests/unit/test_chat_completion.py:82-120] router/MCP/文件能力需专项审计，默认外部文件能力必须关闭。[tools/server/README-dev.md:24-32]

---

**页尾：** API 行为以实际 server code 和测试为准。

## 文档元数据（规范补充）

- 文档目的：说明 `01-modules/M04-server/README.md` 的源码分析范围、结论和维护入口。
- 适用范围：当前项目对应模块/入口的静态源码与测试分析。
- 对应源码版本：以本项目 `00-overview/analysis-state.md` 或同页版本字段为准。
- 证据状态：静态源码证据；未执行的构建、测试、GPU、网络或多进程行为保持“未验证”。
- 最后更新：2026-09-15
- 前置阅读：本项目根 README 与 `00-overview/analysis-state.md`。
- 后续阅读：本模块/示例的实现、测试和风险页面。

## 深度审计

| 分析对象 | 入口落地 | 正常路径 | 分支 | 异常 | 清理 | 数据生命周期 | 执行上下文 | 行级证据 | Demo 映射 | 状态/缺口 |
|---|---|---|---|---|---|---|---|---|---|---|
| `01-modules/M04-server/README.md` | 已完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 已映射或不适用 | 部分完成：动态行为、边界或专用变体仍需验证 |

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
