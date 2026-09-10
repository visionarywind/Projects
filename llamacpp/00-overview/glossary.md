# 术语表

| 术语 | 含义 | 主要证据 |
|---|---|---|
| GGUF | 模型 metadata、词表和 tensor 的文件格式 | `src/llama.cpp` loader |
| ggml | tensor、graph、allocator、scheduler 和 backend 抽象 | `ggml/` CMake |
| backend | CPU/GPU/NPU 的设备实现和注册项 | `src/llama.cpp:122-135` |
| model | 已加载权重、架构和 hparams | `llama_model` |
| context | 一次推理执行的 memory/KV/线程/调度状态 | `llama_init_from_model` |
| batch | 一批 token/embedding 及位置、sequence、输出标志 | `include/llama.h` |
| slot | server 中对应一个 llama sequence 的请求状态 | `tools/server/README-dev.md` |
| ubatch | context 内部实际送入一次 graph 的 micro-batch | `llama_context` 参数 |
| KV cache | attention 的 key/value 历史状态 | `src/llama-context.cpp` |
| common | 面向应用的参数、模板、grammar、采样和缓存工具 | `common/CMakeLists.txt` |
| mtmd | multimodal model/context 适配库 | `tools/mtmd/CMakeLists.txt` |
| SSE | HTTP Server-Sent Events 流式传输 | server stream 文档 |
| router mode | 多 inference 子进程的路由/模型管理模式 | `tools/server/README-dev.md` |
| vocab-only | 只加载词表，不加载权重 tensor | `src/llama.cpp:364-366` |
| tensor split | 多设备按 tensor 规则分配的模式 | `src/llama.cpp:158-220` |

---

**页尾：** 新术语应附源码或开发文档证据，避免把 CLI 参数名误写成稳定 API。
