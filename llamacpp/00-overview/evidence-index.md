# 证据索引

## 构建与版本

- 版本、选项和子目录：[CMakeLists.txt:5-23] [CMakeLists.txt:129-149] [CMakeLists.txt:204-258]
- ggml backend 选项：[ggml/CMakeLists.txt:86-90] [ggml/CMakeLists.txt:189-281]
- server target：[tools/server/CMakeLists.txt:36-67]
- mtmd target 和解耦：[tools/mtmd/CMakeLists.txt:15-35] [tools/mtmd/CMakeLists.txt:83-115]

## Runtime

- backend init：[src/llama.cpp:122-151]
- device preparation：[src/llama.cpp:158-312]
- model load：[src/llama.cpp:316-444]
- architecture dispatch：[src/llama-model.cpp:343-369]
- context validation：[src/llama-context.cpp:3660-3765]
- graph scheduler：[src/llama-context.cpp:2492-2518]
- decode wrapper：[src/llama-context.cpp:4247-4255]

## Server

- startup/error wrapper：[tools/server/server.cpp:52-112]
- batch/decode/error：[tools/server/server-context.cpp:3645-3781]
- completion handler：[tools/server/server-context.cpp:4227-4414]
- architecture/thread/stream overview：[tools/server/README-dev.md:37-149]
- sleep and wake：[tools/server/README-dev.md:294-322]

## Tests/Demo

- CTest helpers and fixtures：[tests/CMakeLists.txt:1-117] [tests/CMakeLists.txt:201-257]
- server runner：[tools/server/tests/tests.sh:1-20]
- chat completion assertions：[tools/server/tests/unit/test_chat_completion.py:28-51]
- stream assertions：[tools/server/tests/unit/test_chat_completion.py:82-120]
- UI scripts：[tools/ui/package.json:7-27]

---

**页尾：** 引用行号针对版本快照；更新源码后应重新核验。
