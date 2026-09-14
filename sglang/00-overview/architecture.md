# 架构总览

## 1. 系统分层

```text
用户命令 / HTTP / OpenAI SDK
            │
            ▼
M01 CLI 与 M02 API
            │ 规范化请求、解析配置
            ▼
M03 TokenizerManager ── IPC ── M04 Scheduler
                                      │
                                      ├─ M07 distributed groups
                                      ├─ M06 model + weights
                                      ├─ M08 KV pools / Radix Cache
                                      └─ M05 ModelRunner
                                             │
                         M11 speculative / M12 multimodal / M13 transfer
                         M14 quant-MoE-LoRA / M16 kernels-platforms
                         M17 Rust-router-gateway / M18 verification
                                             │
                         kernels / attention / sampling / external services
                                             ▼
                                      GPU/NPU/XPU 等设备
```

**已确认**：`python/sglang/launch_server.py:16-58` 按配置选择 encoder-only、SMG gRPC、Ray 或默认 HTTP；默认 HTTP 再进入 `srt.entrypoints.http_server`。`python/sglang/srt/entrypoints/engine.py:847-1051` 负责把运行时拆成 tokenizer、detokenizer 和 scheduler 进程。

## 2. 控制面和执行面

控制面负责请求对象、队列、批次、配置和 IPC；执行面负责设备上的模型计算、KV 内存和通信。两者不是严格的两个进程：Scheduler 同时是控制循环和 worker 的拥有者，但职责仍可沿“决定做什么”和“实际算什么”区分。

这种划分解释了几个看似重复的对象：HTTP 请求不是 Scheduler 的 `Req`，API batch 不是 GPU `ForwardBatch`，服务器配置也不是每个函数都直接读取的 CLI namespace。

## 3. 一次请求的状态变化

```text
JSON
 -> GenerateReqInput
 -> TokenizedGenerateReqInput
 -> ReqState（TokenizerManager）
 -> Req（Scheduler）
 -> waiting/running ScheduleBatch
 -> ForwardBatch
 -> logits / sampled token
 -> Req.output_ids + KV mapping
 -> Batch*Output
 -> detokenized response
```

**推断**：这些状态跨越 CPU、进程、GPU 三个边界，因此任何字段改动都必须同时检查序列化、IPC、batch 构造和输出处理，而不能只修改 HTTP schema。

## 4. 启动时与请求时

启动时一次性或配置驱动地完成：配置解析、进程组、模型结构/权重、KV pool、attention backend、CUDA Graph 和 sampling prewarm。请求时重复完成：接收、tokenize、prefix match、admission、batch 构造、forward、采样、停止判断和输出。

**已确认**：`Scheduler.init_model_worker` 的顺序把 model runner、KV memory pool、attention backends、CUDA graphs 和 prewarm 串在 scheduler ready 之前；这些不是每个请求重新初始化的操作。

## 5. 变体不是主线

- `--use-ray` 改变 scheduler 部署方式，不改变所有模型执行协议。
- speculative decoding 增加 draft/target worker 和 token 接受逻辑。
- 多模态在 tokenize 前后增加媒体处理、视觉 encoder 或 M-RoPE。
- disaggregation 把 prefill/decode 或 encoder 拆到不同服务。
- diffusion runtime 是另一套 pipeline，不应套用 autoregressive Scheduler 的所有细节。

后续模块文章会把每个变体写成“主线契约的替换或扩展”，而不是重新发明一条流程。
