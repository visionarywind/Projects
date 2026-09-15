# 线程、队列与资源生命周期

- 文档目的：解释 90-cross-module/thread-queue-resource.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/llamacpp HEAD 093a2f86c（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 90-cross-module/thread-queue-resource.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


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
