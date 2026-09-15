# 跨模块接口契约

- 文档目的：说明请求、配置、批次、模型执行和输出在模块边界上的输入/输出保证。
- 适用范围：M01-M18 的普通请求主线，以及 speculative、multimodal、disaggregation、MoE/LoRA、设备和 Rust/router 变体。
- 对应源码版本：`78be4b50af88e9ea72d75b4c3a3e42b7297d2501`
- 证据状态：部分完成
- 最后更新：2026-09-10
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

SGLang 的跨模块契约不仅是函数签名，还包括 `rid` 对齐、字段序列化、状态生命周期、ready 同步和错误清理。最容易破坏的契约是：请求已 dispatch 后不能只删除本地 state；完成输出必须按 `rids[i]` 找到对应 `ReqState`；scheduler ready 必须在模型/资源初始化后发送。[`python/sglang/srt/managers/tokenizer_manager.py:1-2816`][`python/sglang/srt/managers/tokenizer_manager.py:2255-2265`][`python/sglang/srt/managers/scheduler.py:1-4005`]

## 契约表

| 上游 | 下游 | 接口/数据 | 前置条件 | 下游保证 | 错误语义 | 生命周期 |
|---|---|---|---|---|---|---|
| M01 | M15 | `PortArgs`、process args | config 已 resolve | 建立 socket/pipe | startup error 在 ready 前传播 | Engine 持有 child |
| M03 | M04 | `TokenizedGenerateReqInput` / batch object | rid state 已建、对象可序列化 | 接收并 admission | dispatch 失败需释放临时资源 | scheduler 接管请求 |
| M04 | M05 | `ScheduleBatch`/`ForwardBatch` | batch 已 admission | forward/sample 返回结果 | worker/device error 进入 scheduler error path | batch 持有 Req 引用 |
| M05 | M04 | `next_token_ids`、model output | last PP rank 或 proxy 完成 | 更新请求/KV/finish | forward mode 决定 result processor | 每轮 batch |
| M04/M15 | M03 | `BatchStrOutput` | rid 仍在 `rid_to_state` | 输出按 rid 汇聚 | 未知 rid 记录错误并跳过 | 完成时删除 state |
| M03 | API/Engine | `out_dict` | state event 已触发 | yield/return | abort/断连抛异常 | 调用者拥有返回值 |

## 稳定字段契约

- `rid`：跨输入、scheduler 和输出的关联键；批量对象以数组下标配对。
- `input_ids`：tokenized request 的序列表示；普通文本在 tokenizer 中产生。[`python/sglang/srt/managers/io_struct.py:972-1000`]
- `sampling_params`：在 `_create_tokenized_object` 中构造成并校验的 `SamplingParams`。[`python/sglang/srt/managers/tokenizer_manager.py:1369-1383`]
- `finished_reasons`：输出侧判断 `state.finished` 的依据。[`python/sglang/srt/managers/tokenizer_manager.py:2267-2273`][`python/sglang/srt/managers/tokenizer_manager.py:2379-2380`]
- `out_list` + `event`：响应生产者和等待消费者之间的异步契约。[`python/sglang/srt/managers/tokenizer_manager.py:2530-2538`]

## 修改契约的规则

新增字段必须至少检查用户输入结构、normalize/batch slicing、tokenized IPC 结构、scheduler 消费、输出结构和测试。删除字段要检查跨进程旧 worker 兼容性；当前具体兼容策略需由对应版本源码确认。

## 相关文档

- [M03 Tokenizer 与请求状态](../01-modules/M03-tokenizer-request-state/README.md)
- [M07 分布式并行](../01-modules/M07-分布式并行.md)
- [共享数据与类型](shared-data-and-types.md)

## 源码证据摘要

- [`python/sglang/srt/managers/io_struct.py:173-224`](../../source/sglang/python/sglang/srt/managers/io_struct.py)
- [`python/sglang/srt/managers/io_struct.py:972-1034`](../../source/sglang/python/sglang/srt/managers/io_struct.py)
- [`python/sglang/srt/managers/tokenizer_manager.py:1577-1650`](../../source/sglang/python/sglang/srt/managers/tokenizer_manager.py)
- [`python/sglang/srt/managers/tokenizer_manager.py:2240-2538`](../../source/sglang/python/sglang/srt/managers/tokenizer_manager.py)

## 未解决问题

完整 HTTP/OpenAI schema 到 `GenerateReqInput` 的字段映射、旧版本 IPC 兼容和所有 embedding/多模态契约尚未逐项整理。

## 下一步阅读建议

结合 M03 的状态机阅读 [错误边界](error-boundaries.md)，重点观察 dispatch 前后失败语义为何不同。
