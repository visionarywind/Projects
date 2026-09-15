# 全局数据流

- 文档目的：解释 00-overview/global-data-flow.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/sglang HEAD 78be4b50af（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](analysis-state.md)。
## 结论摘要

本页聚焦 00-overview/global-data-flow.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


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
  -> speculative draft/verify (optional)
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
