# 线程、队列与资源生命周期

## 线程边界

HTTP worker 做 JSON、template、tokenize 和 response formatting；server context 使用专用单线程。task/result queue 是二者的同步数据边界。[tools/server/README-dev.md:86-119]

## 生命周期

```text
common_init
 -> stream manager start
 -> backend init
 -> model load
 -> context/slots
 -> task/result
 -> stream manager stop
 -> context/model/backend cleanup
```

server 启动顺序中 stream manager 在 backend init 前启动；开发文档要求 cleanup 先停 stream manager，以 finalize live session 并避免 reader 悬挂。[tools/server/server.cpp:99-112] [tools/server/README-dev.md:127-149]

## Queue 语义

HTTP 侧 `server_res_generator` 创建 reader；非 sleep fast path 可绕过等待，普通请求调用 `wait_until_no_sleep`。handler post tasks 后等待 result 或逐项 next。[tools/server/server-context.cpp:4231-4238] [tools/server/server-context.cpp:4342-4469]

## 资源风险

- queue/reader 提前停止时必须响应 `should_stop`。
- SSE ring 有 4 MiB 上限，旧 bytes 可丢弃；offset 过旧返回错误。[tools/server/README-dev.md:127-145]
- context error 后 server 清理 slot 的 prompt，不能继续复用损坏批次。[tools/server/server-context.cpp:3710-3725]

---

**页尾：** 该页说明拥有关系，不替代各类析构函数的全量审计。
