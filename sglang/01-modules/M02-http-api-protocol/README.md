# M02 HTTP/API 与协议

- 文档目的：解释 FastAPI/native API 与 OpenAI 兼容层如何把外部请求转换为内部请求，并在流式、错误和断连时维持状态契约。
- 源码证据 checkout：`/home/mtuser/workspace/repos/Github/sglang` 当前 checkout。
- 证据状态：源码静态分析已确认；真实服务请求未验证。
- 前置阅读：[M01 CLI 与服务启动](../M01-cli-service-startup/README.md)、[M03 Tokenizer 与请求状态](../M03-tokenizer-request-state/README.md)。

## 1. 一句话模型

M02 是协议适配与生命周期层，不负责 admission、KV 分配或 Transformer 计算。它完成 HTTP body/schema 校验、协议字段到 `GenerateReqInput`/`EmbeddingReqInput` 的映射、流式响应封装和错误序列化，然后把执行交给 `TokenizerManager`。

```text
HTTP / WebSocket
  -> FastAPI route + protocol model
  -> OpenAIServingBase.handle_request
  -> validate / convert
  -> GenerateReqInput 或 EmbeddingReqInput
  -> TokenizerManager.generate_request
  -> SSE/non-stream response
  -> disconnect background abort
```

## 2. Server lifespan

`python/sglang/srt/entrypoints/http_server.py:273-435` 的 `lifespan` 是统一控制面生命周期：

1. 设置 `_GlobalState`，保存 `tokenizer_manager`、`template_manager`、`scheduler_info`；
2. 初始化 OpenAI completion/chat/embedding/classify/score/rerank/tokenize/detokenize/transcription handler；
3. 初始化 Ollama、Anthropic 兼容 handler；
4. 按配置创建 demo/MCP/native tool server；
5. 尝试初始化 Responses API；失败只禁用该 endpoint 并记录 warning；
6. 按单 tokenizer 与 gRPC 配置启动 native gRPC 和 sidecar；
7. 启动 warmup thread 后 `yield` 进入稳态。

退出时以 `finally` 停止 sidecar、关闭 native gRPC、异步关闭 tool server、join warmup thread。Responses API 的可选初始化是重要错误边界：其失败不会自动等价于整个 HTTP server 启动失败。

## 3. 请求解析和公共错误模型

应用把 `ORJSONRoute` 设置为 router route class；`ORJSONRequest.json()` 在 `http_server.py:437-462` 中缓存 body 并调用 `orjson.loads`，适合较大的 multimodal JSON。`validate_json_request`（`:643-662`）检查 content type 的 media type 必须为 `application/json`；multipart 音频 endpoint 是单独路径，不应套用该规则。

`OpenAIServingBase` 位于 `python/sglang/srt/entrypoints/openai/serving_base.py:27-...`：

- `handle_request` 先调用 `_validate_request`；
- `_convert_to_internal_request` 产生内部 request 和 protocol request；
- 根据 `stream` 选择 streaming/non-streaming；
- 捕获 `HTTPException`、`ValueError`、编码异常和未知异常；
- `create_error_response` 统一生成 `ErrorResponse` 与状态码。

LoRA model 参数使用 `base:model-or-adapter` 形式时，`_parse_model_parameter` 分割第一个冒号；model 参数中的 adapter 优先于显式 `lora_path`（`serving_base.py` 的 `_resolve_lora_path`）。这会改变 M14 的 adapter 选择，不是纯展示字段。

## 4. Native `/generate` 路径

`python/sglang/srt/entrypoints/http_server.py:905-957` 的 `generate_request`：

- FastAPI 先把 body 转为 `GenerateReqInput`；
- header override 可覆盖请求字段；
- 非流式调用 `TokenizerManager.generate_request(...).__anext__()`，只取最终对象并序列化；
- 流式路径异步迭代每个输出，编码为 `data: <json>\\n\\n`，最后发送 `data: [DONE]`；
- `StreamingResponse.background` 安排 `create_abort_task(obj)`，客户端离开时把 abort 传播到上游。

流式 `ValueError` 若同时检测到 client disconnected，则只记录并停止，不再向已经断开的客户端发送 400；否则编码成 `invalid_request_error` SSE event。这个判断维护了“断连是取消，不是输入错误”的语义。

`/encode` 和 `/classify`（`:959-981`）复用 `EmbeddingReqInput` 和同一个 tokenizer manager，但走非流式单结果路径。管理类 endpoint（flush cache、HiCache storage、profile、weights、LoRA、session、abort 等，`:983-1728`）不是普通生成主线，通常带 admin auth 或内部状态检查。

## 5. OpenAI 适配链

路由位于 `http_server.py:1729-1808`：

```text
POST /v1/completions
  -> OpenAIServingCompletion.handle_request
POST /v1/chat/completions
  -> OpenAIServingChat.handle_request
POST /v1/embeddings
  -> OpenAIServingEmbedding.handle_request
POST /v1/tokenize|/tokenize
  -> OpenAIServingTokenize.handle_request
POST /v1/detokenize|/detokenize
  -> OpenAIServingDetokenize.handle_request
```

### 5.1 Completion 字段投影

`serving_completions.py:48-192`：

- 空 prompt 被拒绝；
- completion template 可重写 prompt；
- `echo + logprobs` 设置 logprob 起点并警告兼容性；
- 字符串/list-string 映射 `text`，token id list 映射 `input_ids`；
- temperature、token budget、stop、top-p/top-k/min-p、penalties、grammar、`n`、logit bias、seed 等进入 `sampling_params`；
- response format 的 `json_schema`、`json_object`、`structural_tag` 转成约束字符串；
- header 中 custom labels、routed DP rank、routing key 会进入内部 request；
- model 中的 LoRA adapter 通过基类解析。

### 5.2 Chat 的额外语义

`serving_chat.py:117-224,256-354`：

- `normalize_tool_content` 将 OpenAI text parts 合并为文本，其他 media/tool parts 保持结构；
- `parse_tool_call_arguments` 要求 function arguments 是合法 JSON object；
- `OpenAIServingChat.__init__` 固化 tool-call parser、reasoning parser、默认 generation config、chat encoding spec；
- 消息经 chat template 或专用 dsv4/dsv32/inkling 编码；
- assistant prefix、`continue_final_message` 会改变最终 prompt；
- image/video/audio 和 tool schema 进入 multimodal/grammar 分支；
- response parser 再将 reasoning、tool call、logprob 和文本转换为 OpenAI chunk。

因此“OpenAI compatible”不是简单字段重命名，而是协议 schema、模板、约束编译和输出 parser 的组合。

## 6. 代表调用链

```text
POST /v1/chat/completions
  -> openai_v1_chat_completions()                    [http_server.py:1740-1747]
  -> OpenAIServingChat.handle_request()              [serving_base.py:72-132]
  -> _validate_request()
  -> _convert_to_internal_request()                  [serving_chat.py]
  -> GenerateReqInput
  -> _handle_streaming_request / _handle_non_streaming_request
  -> TokenizerManager.generate_request()             [tokenizer_manager.py]
  -> BatchStrOutput / response parser
  -> SSE chunk 或 ChatCompletionResponse
```

native `/generate` 的返回链是：

```text
generate_request()
  -> tokenizer_manager.generate_request
  -> scheduler/detokenizer IPC
  -> output dict
  -> orjson_response 或 SSE
```

M03 负责 `rid_to_state`、tokenize、detokenize 和 cleanup；M02 只拥有协议级 response wrapper。

## 7. Health、metadata 与 warmup

`/health`、`/health_generate`（`http_server.py:664-743`）区分 shutdown/starting/unhealthy 状态；默认以 UUID rid 构造 `max_new_tokens=1` 的内部请求，并等待 tokenizer manager 的 heartbeat。成功设置 `ServerStatus.Up`，超时设置 `UnHealthy` 并返回 503。health probe 的 rid 前缀与 UUID 用来避免多 tokenizer worker 冲突。

`/model_info`、`/server_info` 和 `/v1/models`（`:744-845,1862-1918`）暴露 resolved model/config、scheduler info、context length、LoRA adapter 等元数据，但不代表模型请求已经成功执行。warmup 在 `http_server.py:2203-2432` 中单独执行，Rust server 模式也可能先 warmup 再对外宣告 ready。

## 8. 异常、断连和清理

| 场景 | M02 行为 | 上游影响 |
|---|---|---|
| JSON/content type 错误 | FastAPI validation/400 | 不创建有效 request |
| 参数 ValueError | 标准错误 JSON 或 SSE event | 不应留下 request state |
| stream client disconnect | abort task + 安静结束 | M03 向 scheduler 发送 abort |
| scheduler/detokenizer 错误 | handler 捕获并序列化 | 检查 M15 子进程和 M03 state |
| Responses 初始化失败 | 只禁用 Responses endpoint | 其他 endpoints 继续服务 |
| lifespan 退出 | 关闭 sidecar/gRPC/tool/warmup | Engine/scheduler 由 M15 清理 |

## 9. 小例子和调试断点

例：请求 `/v1/completions`，`prompt=[1,2,3]`、`stream=true`、`response_format=json_object`。

1. protocol model 验证 prompt；
2. completion handler 选择 `input_ids`；
3. 将 JSON object 约束写入 sampling params；
4. `GenerateReqInput` 进入 tokenizer manager；
5. 每个 batch output 被转成 SSE completion chunk；
6. disconnect 触发 abort。

断点建议：

- `validate_json_request`：区分 body schema 错误与 content type 错误；
- `OpenAIServingBase.handle_request`：确认转换前后 request；
- `serving_completions.py:_convert_to_internal_request`：核对 sampling/LoRA/routing 字段；
- `http_server.py:generate_request` 的 `create_abort_task`；
- `http_server.py:lifespan` 的 `finally`：确认可选资源完整关闭。

## 10. 测试与未验证范围

- `test/registered/core/test_srt_endpoint.py`：native generate、OpenAI、logprob、Rust server；
- `test/registered/openai_server/basic/test_openai_server.py`：OpenAI 基础协议；
- `test/registered/openai_server/validation/test_request_length_validation.py`：请求验证；
- `test/registered/openai_server` 下的 chat/tool/structured output 专项；
- `test/manual/test_health_check.py`、`test/manual/test_sagemaker_server.py`、`test/manual/test_vertex_endpoint.py`：特殊 endpoint。

本次未执行上述 GPU/服务器测试；模型、端到端 SSE、真实断连和 Rust server 结果均未验证。

## 11. 修改影响

- protocol schema 改动会影响 OpenAI/Ollama/Anthropic handler 和 M03 输入；
- `GenerateReqInput` 字段改动会跨 tokenizer IPC、scheduler 和 sampling；
- stream framing 改动会影响所有 SDK 和 abort 生命周期；
- lifespan 改动会影响 warmup、native gRPC、tool server、metrics、sidecar；
- chat template/parser 改动会影响 multimodal placeholder、grammar、reasoning/tool call 和输出计费。

**已确认**：M02 是协议适配层。**推断**：具体 HTTP server 部署方式由 M01 resolved config 决定。**未验证**：真实网络行为与全部协议组合。
