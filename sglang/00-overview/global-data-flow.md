# 全局数据流

## 1. 请求方向

```text
HTTP JSON
  -> protocol model
  -> GenerateReqInput
  -> normalize_batch_and_arguments
  -> tokenizer/chat template/multimodal processing
  -> TokenizedGenerateReqInput
  -> IPC
  -> Scheduler Req
  -> waiting queue
  -> prefix match + KV admission
  -> ScheduleBatch
  -> ForwardBatch
  -> ModelRunner.forward
  -> logits processor + sampler
  -> next_token_ids
  -> Req/output batch update
  -> IPC BatchTokenIDOutput/BatchStrOutput
  -> incremental detokenize
  -> JSON/SSE
```

## 2. 启动数据方向

```text
CLI/YAML
  -> ServerArgs raw input
  -> resolution stash
  -> runtime config bags
  -> process publish
  -> ParallelState / ModelConfig / LoadConfig
  -> model loader
  -> model parameters
  -> KV pool + attention metadata
  -> CUDA Graph/eager runners
```

## 3. 关键数据对象

| 对象 | 所在阶段 | 作用 |
|---|---|---|
| `ServerArgs` | 启动/进程 | 原始和解析后的服务器配置载体 |
| `ReqState` | TokenizerManager | HTTP 请求生命周期、流式发送状态 |
| `Req` | Scheduler | prompt/output、停止条件、KV/cache 状态 |
| `ScheduleBatch` | Scheduler | 本轮请求、forward mode、批次 metadata |
| `ForwardBatch` | ModelRunner | 设备输入、位置、slot/KV/attention metadata |
| `ModelConfig` | worker 启动 | HF config、架构、dtype、上下文和模型能力 |
| `LoadConfig` | loader | checkpoint 格式、下载、并行和缓存选择 |
| `Batch*Output` | IPC/output | 批量结果和请求 ID 的桥梁 |

## 4. 共享状态与所有权

- Scheduler 拥有请求队列和批次状态。
- KV allocator/pool 拥有设备缓存槽位；请求只保存映射。
- ModelRunner 拥有模型执行对象和 backend runner。
- TokenizerManager 拥有 HTTP-facing request state。
- IPC socket 和 startup pipe 由对应进程角色创建/关闭。

这是当前源码阅读中的**所有权模型**；具体对象是否通过共享内存传递，取决于消息类型和配置分支。
