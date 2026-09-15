# 架构总览

- 文档目的：解释 00-overview/architecture.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/sglang HEAD 78be4b50af（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](analysis-state.md)。
## 结论摘要

本页聚焦 00-overview/architecture.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


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

## 相关文档
- [项目入口](../README.md)
- [分析状态](analysis-state.md)
- [源码证据索引](evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
