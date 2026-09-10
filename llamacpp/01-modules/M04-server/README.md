# M04 - llama-server

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
