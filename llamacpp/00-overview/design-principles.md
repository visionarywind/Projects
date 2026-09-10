# 设计原则与决策

## 已确认的设计约束

1. 用 ggml graph/backend 把模型数学表达与设备执行分开。[CMakeLists.txt:204-221]
2. 通过 opaque C API 隐藏 C++ model/context 实现。[include/llama.h:1-40]
3. GGUF 是 runtime 的模型输入边界，loader 负责 metadata、词表、tensor。[src/llama.cpp:316-377]
4. 架构枚举派发具体模型类，未知架构显式失败。[src/llama-model.cpp:343-369]
5. server 用 task/result queue 隔离 HTTP worker 与单独推理线程。[tools/server/README-dev.md:46-60]
6. JSON/template/tokenize 留在 HTTP 层，避免 inference thread 做重 post-processing。[tools/server/README-dev.md:86-103]
7. mtmd 作为独立公共库，不链接 llama-common。[tools/mtmd/CMakeLists.txt:83-115]
8. tensor split 使用 meta device 和架构相关 split state。[src/llama.cpp:158-220] [src/llama-model.cpp:371-470]
9. stream replay 用有界 ring 和明确 GC，避免无限缓存。[tools/server/README-dev.md:121-157]
10. 外部文件相关 server 能力默认关闭，降低默认暴露面。[tools/server/README-dev.md:24-32]

## “为什么”证据等级

上述条目中，机制由源码确认；“降低风险”“便于打包”等目的来自开发文档或合理归纳，属于解释性推断，不应当当作未在源码表达的作者意图。

## 未决取舍

后端动态加载、router IPC、speculative re-evaluation、UI API 稳定性和多模型 cache 策略仍需要按具体改动重新评估。

---

**页尾：** 决策日志只记录当前证据，不替维护者做未来 API 设计决定。
