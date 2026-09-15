# M03 Tokenizer 与请求状态

- 文档目的：解释 SGLang 如何把 Engine/API 输入变成可跨进程传输的 tokenized request，并把批量输出重新关联到调用者。
- 适用范围：`TokenizerManager`、`ReqState`、请求输入/输出结构、tokenize、scheduler dispatch 和响应汇聚。
- 对应源码版本：`78be4b50af88e9ea72d75b4c3a3e42b7297d2501`
- 证据状态：部分完成
- 最后更新：2026-09-10
- 前置阅读：[请求进入与分发](../../02-request-flow/02-请求进入与分发.md)、[多进程与 IPC](../../02-request-flow/06-多进程与IPC.md)
- 后续阅读：[M04 Scheduler 与连续批处理](../../02-request-flow/03-调度器与连续批处理.md)、[D01 离线批量推理](../../80-demos/D01-offline-engine/01-离线批量推理.md)

## 结论摘要

**已确认**：M03 位于调用入口和 scheduler 之间。它维护 `rid_to_state`，执行文本到 token id 的转换，构造 `TokenizedGenerateReqInput`，通过 ZMQ socket 发送请求，并在收到 `BatchStrOutput` 后增量更新 `ReqState`，最后把完成状态转换成包含 `text`、`output_ids` 和 `meta_info` 的 Python 字典。[`python/sglang/srt/managers/tokenizer_manager.py:776-845`][`python/sglang/srt/managers/tokenizer_manager.py:1356-1470`][`python/sglang/srt/managers/tokenizer_manager.py:2225-2549`]

**通俗解释**：TokenizerManager 像“前台接待 + 回信分拣中心”。前台把用户说的话编号、切成 token、附上采样要求，送入后端；后端返回带 request id 的结果后，分拣中心根据 id 找到原来的请求，积累文本，等完成后交给调用者。

**部分推断**：Engine 离线批量调用中的四条 prompt 可能先走批量 tokenization，也可能逐条 tokenization 后分别发送；具体取决于 tokenizer batch 配置和输入条件。[`python/sglang/srt/managers/tokenizer_manager.py:1854-1897`]

## 1. 职责与非职责

### 1.1 负责什么？

| 职责 | 真实实现 | 证据 |
|---|---|---|
| 规范化请求参数 | `generate_request` 调用 `normalize_batch_and_arguments` | [`python/sglang/srt/managers/tokenizer_manager.py:776-808`] |
| 为每个 rid 建立本地状态 | `_init_req_state` 写入 `rid_to_state` | [`python/sglang/srt/managers/tokenizer_manager.py:1-2816`] |
| 文本/token id 输入转换 | `_tokenize_one_request` | [`python/sglang/srt/managers/tokenizer_manager.py:970-1138`] |
| 构造跨边界对象 | `_create_tokenized_object` | [`python/sglang/srt/managers/tokenizer_manager.py:1356-1470`] |
| 发送 scheduler 请求 | `_send_one_request`、`_send_batch_request`、`_dispatch_to_scheduler` | [`python/sglang/srt/managers/tokenizer_manager.py:580-588`][`python/sglang/srt/managers/tokenizer_manager.py:1577-1650`] |
| 汇聚增量响应 | `handle_loop`、`_handle_batch_output` | [`python/sglang/srt/managers/tokenizer_manager.py:2225-2549`] |
| 处理等待、断连和 abort | `_stream_one_response` | [`python/sglang/srt/managers/tokenizer_manager.py:1740-1852`] |

### 1.2 不负责什么？

- 不负责选择哪个模型层执行 forward；该工作在 scheduler/worker/ModelRunner 中完成。
- 不负责决定 continuous batching 的 admission；它只决定请求如何 tokenization 和 dispatch。
- 不等于 detokenizer 进程；`BatchStrOutput` 已经是 detokenizer 侧产生的文本输出，TokenizerManager 负责把它与本地请求状态合并。
- 不保证所有请求必然是一条 IPC 消息；batch policy 会选择批量发送或逐条发送。

## 2. 入口到真实副作用

```text
Engine.generate / HTTP handler
  -> TokenizerManager.generate_request
     -> _init_req_state
     -> _tokenize_one_request
        -> tokenizer / input validation
        -> _create_tokenized_object
     -> _send_one_request 或 _send_batch_request
        -> wrap_shm_features / wrap_pickle_fields
        -> _dispatch_to_scheduler
           -> sock_send(ZMQ)
  -> scheduler / detokenizer 返回 BatchStrOutput
     -> handle_loop
     -> _handle_batch_output
        -> state.append_text / state.output_ids.extend
        -> del rid_to_state[rid]（完成时）
        -> state.out_list.append(out_dict)
        -> state.event.set()
  -> _stream_one_response
     -> yield out_dict
```

真正的外部效果不是 `generate_request` 这个异步生成器本身，而是：

1. `sock_send` 把消息放入 scheduler IPC socket；
2. `ReqState` 的字段被写入和更新；
3. 完成时删除状态并唤醒等待者；
4. Python 调用者收到最终字典。[`python/sglang/srt/managers/tokenizer_manager.py:580-588`][`python/sglang/srt/managers/tokenizer_manager.py:2391-2434`][`python/sglang/srt/managers/tokenizer_manager.py:2524-2533`]

## 3. 核心实现组件表

| 实现组件 | 声明/定义位置 | 创建者 | 主要调用者 | 核心状态 | 实际副作用 |
|---|---|---|---|---|---|
| `TokenizerManager` | `tokenizer_manager.py:399` | Engine 或 HTTP server | Engine/API handler | `rid_to_state`、socket、event loop | tokenize、IPC、输出汇聚 |
| `ReqState` | `tokenizer_manager.py:224-269` | `_init_req_state` | `_handle_batch_output`、`_stream_one_response` | `out_list`、`finished`、`output_ids`、文本 chunks | 保存单请求生命周期 |
| `GenerateReqInput` | `io_struct.py:173-...` | Engine/API 请求入口 | `generate_request` | text、input_ids、sampling、rid | 请求的用户侧表示 |
| `TokenizedGenerateReqInput` | `io_struct.py:972-...` | `_create_tokenized_object` | `_send_one_request`/scheduler | array token ids、`SamplingParams`、rid | scheduler 输入消息 |
| `BatchStrOutput` | `io_struct.py:1531-...` | detokenizer/output streamer | `handle_loop` | rid、文本增量、token ids、finish reason | scheduler 结果消息 |

## 4. 关键数据结构与不变量

### 4.1 `ReqState`

`ReqState` 使用 dataclass 保存一个 request 的本地响应状态：

```python
@dataclasses.dataclass
class ReqState:
    out_list: List[Dict[Any, Any]]
    finished: bool
    event: asyncio.Event
    obj: Union[GenerateReqInput, EmbeddingReqInput]
    time_stats: APIServerReqTimeStats
    dispatched: bool = False
    abort_sent: bool = False
    last_output_offset: int = 0
    text: str = ""
    text_chunks: List[str] = ...
```

[`python/sglang/srt/managers/tokenizer_manager.py:224-269`]

状态不变量：

- `rid_to_state[rid]` 存在，表示本地仍在等待或处理该 rid；
- `dispatched=False` 时，失败清理可以直接删除本地 state；
- `dispatched=True` 时，失败清理必须发送 abort，并等待 scheduler 侧结果完成删除；
- `finished=True` 表示收到终止原因；完成路径会删除 `rid_to_state`；
- `text_chunks` 是增量文本暂存，`get_text()` 将其合并到 `text`。[`python/sglang/srt/managers/tokenizer_manager.py:249-257`]

### 4.2 输入对象的两阶段表示

`GenerateReqInput` 允许 `text`、`input_ids` 或 `input_embeds` 等输入；批量输入可以把它们表示为 list。[`python/sglang/srt/managers/io_struct.py:173-224`]

`TokenizedGenerateReqInput` 则要求明确的 `input_ids`（或 embedding 输入）、`SamplingParams`、`rid` 和输出选项；它是跨向 scheduler 的运行时消息，而不是用户 API 的原始对象。[`python/sglang/srt/managers/io_struct.py:972-1034`]

### 4.3 输出对象

`BatchStrOutput` 按 batch 保存：

- `rids`；
- `finished_reasons`；
- `output_strs` 增量文本；
- `output_ids`；
- prompt/completion/cached token 计数；
- logprob、hidden state 和其他可选元数据。[`python/sglang/srt/managers/io_struct.py:1531-1594`]

因此响应处理不能只取一个字符串：它必须按数组下标 `i` 和 `rids[i]` 对齐每一个请求。

## 5. 详细执行流程

### M03-FLOW-MAIN-001：单请求从文本到最终 output

| 步骤 | 源码 | 输入/状态 | 状态变化 | 副作用/下一步 |
|---|---|---|---|---|
| 1 | `generate_request` `[tokenizer_manager.py:776-808]` | `GenerateReqInput` | normalize、建立 rid 集合 | 进入异常保护区 |
| 2 | `_init_req_state` `[tokenizer_manager.py:3463-3506]` | rid、sub-object | `rid_to_state[rid] = ReqState(...)` | 本地可关联响应 |
| 3 | `_tokenize_one_request` `[tokenizer_manager.py:970-1010]` | text 或 input ids | text 经 tokenizer 产生 token ids | 进入校验/对象构造 |
| 4 | `_create_tokenized_object` `[tokenizer_manager.py:1356-1470]` | token ids、sampling dict | list → `array("q")`；构造并 normalize/verify `SamplingParams` | 生成 scheduler 输入 |
| 5 | `_send_one_request` `[tokenizer_manager.py:1577-1604]` | tokenized object | pickle/shared-memory 字段包装；state 标记 dispatched | `sock_send` 发往 scheduler |
| 6 | scheduler/detokenizer | 运行时请求 | 产生 `BatchStrOutput` | 通过 detokenizer socket 返回 |
| 7 | `handle_loop` `[tokenizer_manager.py:2225-2238]` | recv object | 识别 output 类型 | 调 `_handle_batch_output` |
| 8 | `_handle_batch_output` `[tokenizer_manager.py:2240-2434]` | `rid`、文本增量、ids | append text、extend ids、构造 out dict | 完成时删除 state |
| 9 | `_stream_one_response` `[tokenizer_manager.py:1740-1831]` | event/out_list | 排空 output、判断 finished | yield 最终结果 |

### M03-FLOW-BATCH-001：四条 prompt 的批量选择

`_handle_batch_request` 首先检查 `parallel_sample_num`。普通情况再根据 `_should_use_batch_tokenization` 选择：

```text
batch tokenization = true
  -> _batch_tokenize_and_process
  -> _send_batch_request
  -> 为每个 sub-request 建立 waiter

batch tokenization = false
  -> 对每个 sub-object 调 _tokenize_one_request
  -> 分别 _send_one_request
  -> 为每个 rid 建立 waiter
```

[`python/sglang/srt/managers/tokenizer_manager.py:1854-1897`]

这里的 Python list 是 API 输入形态，不是对 IPC 形态的承诺。

## 6. 关键逻辑块分析

### 6.1 状态先建、失败再清理

`generate_request` 在 tokenize 前调用 `_init_req_state`，之后用 `try/except BaseException` 包围 tokenize、dispatch 和等待。这样输入校验失败也不会留下永久的 `rid_to_state` 条目；已 dispatch 的请求则通过 `abort_request` 交给 scheduler 清理。[`python/sglang/srt/managers/tokenizer_manager.py:807-845`]

### 6.2 tokenization 不是只有文本分词

`_tokenize_one_request` 的分支顺序是：

1. 有 `input_embeds` 时要求关闭 radix cache；
2. 有 `input_ids` 时跳过 tokenizer；
3. 否则要求 tokenizer 已初始化并对文本分词；
4. 如果存在多模态输入，可能再经过 multimodal processor 覆盖 input ids；
5. 最后统一校验并构造 tokenized object。[`python/sglang/srt/managers/tokenizer_manager.py:970-1138`]

D01 是普通纯文本，因此静态上会落到第 3 步；实际 tokenizer 是否初始化成功依赖运行环境，本文未启动模型验证。

### 6.3 dispatch 的资源边界

`_send_one_request` 在发送前准备 VMM/shared-memory 特征，给对象包装 pickle 字段；只有 `_dispatch_to_scheduler` 成功后才把 `state.dispatched` 置为 true。失败时 `finally` 取消已准备的多模态资源。[`python/sglang/srt/managers/tokenizer_manager.py:1577-1604`]

即使纯文本没有多模态 item，这个结构仍然表明 dispatch 是一个可能分配临时资源、需要失败回滚的边界。

### 6.4 输出是增量状态机

收到 `BatchStrOutput` 后，处理器先生成 `meta_info`，再把 `output_strs[i]` 追加到 `text_chunks`，把 `output_ids[i]` 追加到 `state.output_ids`。非流式且未完成时不产生 out dict；完成后才复制完整文本和 ids。流式模式可以立即返回 delta，非增量流式中间结果则延迟 text 合并以避免每一步重建完整字符串。[`python/sglang/srt/managers/tokenizer_manager.py:2267-2434`]

## 7. 错误、取消和清理

### 输入或 tokenization 失败

- 触发：重复 rid、非法 input ids、未初始化 tokenizer、超过限制；
- 检测：`_init_req_state`、`_tokenize_one_request`、`_validate_one_request`；
- 处理：跳到 `except BaseException`；未 dispatch 状态直接从 `rid_to_state` 删除；[`python/sglang/srt/managers/tokenizer_manager.py:1-2816`][`python/sglang/srt/managers/tokenizer_manager.py:1-2816`]

### dispatch 后调用者失败或断连

- 触发：等待 response 超时期间发现 HTTP request disconnected；
- 检测：`_stream_one_response`；
- 处理：调用 `abort_request`，抛出异常结束调用栈；[`python/sglang/srt/managers/tokenizer_manager.py:1750-1765`][`python/sglang/srt/managers/tokenizer_manager.py:1840-1852`]

### scheduler 返回完成消息

- `state.finished=True`；
- 写入完成时间和 meta；
- 删除 `rid_to_state[rid]`；
- 将 out dict 放入 `out_list` 并设置 `state.event`；[`python/sglang/srt/managers/tokenizer_manager.py:2379-2382`][`python/sglang/srt/managers/tokenizer_manager.py:2491-2538`]

## 8. 调试建议

| 断点 | 观察内容 | 预期结果 |
|---|---|---|
| `generate_request:807` | `obj.rid`、`obj.text`、`obj.is_single` | 状态尚未 dispatch |
| `_init_req_state:3498-3503` | `rid_to_state`、`ReqState` | 每个 rid 有独立 state |
| `_tokenize_one_request:991-1009` | input ids 来源 | D01 纯文本进入 tokenizer |
| `_create_tokenized_object:1367-1383` | `input_ids_arr`、sampling kwargs | 产生 `array("q")` 和 `SamplingParams` |
| `_send_one_request:1590-1595` | socket 对象、rid、dispatched | send 后 state 变为 dispatched |
| `handle_loop:2229-2236` | `recv_obj` 类型 | 通常为 `BatchStrOutput` |
| `_handle_batch_output:2255-2273` | rid、finish reason、meta | 找到本地 state |
| `_handle_batch_output:2391-2434` | delta text、output ids、finished | state 累积并构造结果 |
| `_stream_one_response:1767-1831` | out_list、event、finished | 调用者收到最终 dict |
| `_release_req_states_on_failure:3515-3528` | dispatched 标志 | 删除或 abort 分支正确 |

## 9. 测试地图

已定位但未在本批执行：

- `test/registered/unit/tokenizer/`：tokenizer 相关单元测试；
- `test/registered/openai_server/validation/test_request_length_validation.py`：请求长度校验；
- `test/registered/scheduler/test_abort_with_metrics.py`：abort 与指标；
- `test/registered/unit/managers/test_tokenizer_config_updates.py`：TokenizerManager 配置更新；
- `test/registered/openai_server/basic/test_openai_server.py`：真实 API 集成路径。

**未验证**：本批没有运行上述测试，也没有启动模型或 API server。为修改 M03 增加字段时，至少应检查 `io_struct.py`、tokenize 构造、IPC 序列化、输出汇聚和对应测试。

## 10. 开发影响与练习

### 新增请求字段

应从 `GenerateReqInput` 的用户侧字段开始，检查 normalize/batch slicing，再补充 `TokenizedGenerateReqInput`、发送路径、scheduler 消费和输出/测试；不能只在 HTTP schema 添加字段。[`python/sglang/srt/managers/io_struct.py:173-224`][`python/sglang/srt/managers/io_struct.py:972-1034`]

### 修改输出字段

检查 `BatchStrOutput`、`_handle_batch_output`、流式和非流式分支、meta_info 以及客户端协议；任何只修改输出类而不修改 state 汇聚的方案都可能丢失字段。[`python/sglang/srt/managers/io_struct.py:1531-1594`][`python/sglang/srt/managers/tokenizer_manager.py:2384-2533`]

### 修改清理逻辑

必须区分“尚未 dispatch”和“已 dispatch”两个状态，分别验证本地删除、scheduler abort、event 唤醒和重复清理。[`python/sglang/srt/managers/tokenizer_manager.py:1-2816`]

## 11. 深度审计

| 分析对象 | 入口落地 | 正常路径 | 分支 | 异常 | 清理 | 数据生命周期 | 执行上下文 | 行级证据 | Demo 映射 | 状态/缺口 |
|---|---|---|---|---|---|---|---|---|---|---|
| TokenizerManager 请求主路径 | 已完成 | 已完成 | 已完成 | 已完成 | 已完成 | 已完成 | 已完成（async/ZMQ） | 已完成 | D01 已映射 | 部分完成：缺少实际运行验证和更广 API 协议覆盖 |
| batch tokenization | 已完成 | 已完成 | 已完成 | 部分完成 | 部分完成 | 已完成 | 已完成 | 已完成 | D01 已映射 | 部分完成：具体配置组合需运行确认 |
| multimodal 分支 | 已完成 | 部分完成 | 已完成 | 部分完成 | 部分完成 | 部分完成 | 已完成 | 已完成 | D01 未覆盖 | 部分完成：专项属于 M12 |
| response aggregation | 已完成 | 已完成 | 已完成 | 已完成 | 已完成 | 已完成 | 已完成 | 已完成 | D01 已映射 | 部分完成：detokenizer 上游仍由 M15/M10 补充 |

## 深度审计

| 分析对象 | 入口落地 | 正常路径 | 分支 | 异常 | 清理 | 数据生命周期 | 执行上下文 | 行级证据 | Demo 映射 | 状态/缺口 |
|---|---|---|---|---|---|---|---|---|---|---|
| sglang/01-modules/M03-tokenizer-request-state/README.md | 已定位 | 已追踪代表路径 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 已标注 | 已引用或待补 | 已映射或无专用 Demo | 部分完成：动态构建、运行和硬件边界仍未验证 |

## 相关文档

- [M04 请求调度](../../02-request-flow/03-调度器与连续批处理.md)
- [M05 模型执行与输出](../../02-request-flow/04-模型执行与输出.md)
- [M15 多进程与 IPC](../../02-request-flow/06-多进程与IPC.md)
- [D01 离线 Engine Demo](../../80-demos/D01-offline-engine/01-离线批量推理.md)

## 源码证据摘要

- [`python/sglang/srt/managers/tokenizer_manager.py:776-845`](../../../source/sglang/python/sglang/srt/managers/tokenizer_manager.py)
- [`python/sglang/srt/managers/tokenizer_manager.py:970-1138`](../../../source/sglang/python/sglang/srt/managers/tokenizer_manager.py)
- [`python/sglang/srt/managers/tokenizer_manager.py:1356-1470`](../../../source/sglang/python/sglang/srt/managers/tokenizer_manager.py)
- [`python/sglang/srt/managers/tokenizer_manager.py:1577-1650`](../../../source/sglang/python/sglang/srt/managers/tokenizer_manager.py)
- [`python/sglang/srt/managers/tokenizer_manager.py:1740-1852`](../../../source/sglang/python/sglang/srt/managers/tokenizer_manager.py)
- [`python/sglang/srt/managers/tokenizer_manager.py:2225-2549`](../../../source/sglang/python/sglang/srt/managers/tokenizer_manager.py)
- [`python/sglang/srt/managers/tokenizer_manager.py:1-2816`](../../../source/sglang/python/sglang/srt/managers/tokenizer_manager.py)
- [`python/sglang/srt/managers/io_struct.py:173-224`](../../../source/sglang/python/sglang/srt/managers/io_struct.py)
- [`python/sglang/srt/managers/io_struct.py:972-1034`](../../../source/sglang/python/sglang/srt/managers/io_struct.py)
- [`python/sglang/srt/managers/io_struct.py:1531-1594`](../../../source/sglang/python/sglang/srt/managers/io_struct.py)

## 未解决问题

- 当前未在真实 Engine 进程中确认四条 prompt 的具体 IPC 选择；
- 当前未运行 detokenizer 输出和 abort 竞态测试；
- 仍需补充 HTTP/OpenAI protocol 到 M03 的具体字段转换链；
- `ReqState` 与 scheduler-side `Req` 的完整跨进程对应关系应在 M04/M15 中继续展开。

## 下一步阅读建议

先阅读 D01 的端到端轨迹，再阅读 M04 的 admission/batch 实现，最后将本模块的 `TokenizedGenerateReqInput` 与 scheduler 接收端逐字段对照。
