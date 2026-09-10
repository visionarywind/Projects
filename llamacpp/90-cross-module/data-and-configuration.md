# 数据与配置边界

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
