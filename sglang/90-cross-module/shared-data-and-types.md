# 共享数据与类型

- 文档目的：记录跨进程、跨模块共享的关键对象及其生命周期。
- 对应源码版本：`f1a512c51c73ab660cf41e1af3110c7c11e3b600`
- 证据状态：部分完成
- 最后更新：2026-09-10

## 关键类型

| 类型/字段 | 生产者 | 消费者 | 不变量 |
|---|---|---|---|
| `rid` | M03 请求初始化 | M04、输出处理 | 批量输出按同一 rid 回关联 |
| `input_ids` | tokenizer/tokenized input | scheduler `Req` | 序列表示不能与生成 `output_ids` 混淆 |
| `Req` | scheduler dispatcher | admission、batch、result processor | scheduler 进程内可变状态 |
| `ScheduleBatch` | scheduler | worker/forward batch | 每轮执行的资源和 tensor 快照 |
| `next_token_ids` | model runner | scheduler result processor | 必须与 batch 请求顺序对应 |
| `BatchStrOutput` | scheduler/detokenizer | TokenizerManager | `rids[i]` 与各数组第 i 项对齐 |
| `ReqState` | TokenizerManager | response streamer | dispatch 后失败不能只删除本地 state |

**已确认**：`ReqState` 的完成清理由 `rid_to_state` 删除和 `event.set()` 共同完成；`ScheduleBatch` 同时引用请求、KV pool、allocator 和 tree cache。[`python/sglang/srt/managers/tokenizer_manager.py:2240-2538`][`python/sglang/srt/managers/schedule_batch.py:2184-2269`]

## 资源所有权

- TokenizerManager 拥有本地 `ReqState` 和调用者等待事件。
- Scheduler 拥有 `Req`、waiting/running 状态及调度期 batch 引用。
- KV pool/allocator 和 prefix cache 属于 scheduler runtime，但其 device tensor 生命周期独立于 Python 输出对象。
- Engine 负责子进程生命周期；worker 负责模型和 device 执行资源。

## 修改检查清单

修改共享字段时要同时检查 normalize/batch slicing、IPC 序列化、scheduler 消费、result processor、输出数组对齐和 abort 清理。具体版本兼容策略尚未独立确认。

## 相关文档

- [接口契约](interface-contracts.md)
- [跨模块调用链](cross-module-call-chains.md)
- [M03 Tokenizer 与请求状态](../01-modules/M03-tokenizer-request-state/README.md)
