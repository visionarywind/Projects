# 推理方向：中级面试题

- 题数：20；稳定 ID：`I-I001`–`I-I020`
- 证据规则：源码/文档结论区分 C/I；GPU、模型、多卡、网络、多进程和性能实验标为 `U / 待验证`。
- 单题门槛：元数据、背景知识、问题分析、具体答案、技术洞察、图文说明、拓展知识、追问、易错点、实践、评分、证据。

### I-I001 推理服务启动时，配置从 CLI/环境变量到最终生效经过哪些阶段？如何保证多进程配置一致？

**元数据：** 知识域：`I01 启动链路`；能力目标：`C01 配置解析流程与跨进程一致性`；题型：流程辨析与架构解释；频率：高频；前置/后续：`I-B001`（prefill/decode 基础）、`I-I004`（IPC 多进程）；证据状态：源码/文档 C，多进程运行 U。

**背景知识：** SGLang 的配置系统不是一张扁平字典，而是一条四阶段流水线 `sglang/02-request-flow/05-配置解析与运行时上下文.md`：(1) CLI/YAML 通过 `ServerArgs.from_cli_args` 构造原始输入记录；(2) `run_resolution_pipeline` 按依赖顺序执行各 `hook`，每阶段通过 `declare_resolution(source, **fields)` 追加声明而非改写原始字段；(3) `resolve_once` 保证解析幂等且仅执行一次；(4) `publish(server_args, role=...)` 将声明投影为各进程的运行时 config bags（`get_memory()`、`get_schedule()` 等只读 namespace）。子进程通过 pickle 接收已解析的 `ServerArgs`，其 `__reduce__` 会携带 `_resolved_overrides` 和 `_resolution_finished` 状态，因此子进程调用 `resolve_once` 时直接返回，从同一份声明投影自己的 bags `sglang/02-request-flow/05-配置解析与运行时上下文.md`。

**问题分析：** 1) 从命令行到 `ServerArgs` 的构造过程；2) `resolution_pipeline` 的声明式解析与 last-writer-wins 规则；3) `resolve_once` 的幂等与失败拒绝机制；4) `publish` 如何将声明投影为各进程独立的 config bags；5) pickle 跨进程传递时如何保持一致性——关键在于子进程不重新解析，而是复用父进程的声明。

**具体答案：** 面试中可以这样回答：

> 配置经历四个阶段：首先是输入记录——`prepare_server_args` 把 CLI 和 YAML 统一转成 `argparse.Namespace`，再用 `ServerArgs.from_cli_args` 构造结构化原始记录，此时字段只保留用户输入或声明默认值。其次是解析声明——`run_resolution_pipeline` 按固定顺序执行各 `hook`（平台默认、模型 override、显存、CUDA Graph、attention backend 等），每阶段调用 `declare_resolution` 把决定追加到 `_resolved_overrides` 列表，而非改写原始字段，读取时 `resolving_view` 从末尾向前查找实现 last-writer-wins。第三是幂等保证——`resolve_once` 通过 `_resolution_finished` 标记保证只执行一次，失败的记录设置 `_resolution_failed` 并拒绝重试，避免半完成状态下的重复解析。第四是进程投影——主进程 `publish(server_args, role="tokenizer")` 调用 `_build_config_bags`，按 namespace 路径（如 `schedule.max_running_requests`）把有效值写入各 config bag；子进程通过 pickle 接收已解析的 `ServerArgs`，其 `__reduce__` 携带声明和完成状态，调用 `resolve_once` 直接返回后投影自己的 bags。多进程一致性的核心是：所有进程从同一份声明投影，没有人各自重新解释 raw 输入。

**技术洞察：**

```text
CLI/YAML → ServerArgs (raw record)
  → resolve_once → run_resolution_pipeline
    → declare_resolution(stage, field=value) × N
  → _resolved_overrides = [(stage_1,{...}), (stage_2,{...}), ...]
  → publish(role) → _build_config_bags → get_memory/schedule/... (read-only bags)

子进程: pickle(ServerArgs) → __reduce__ 携带 declarations + _resolution_finished
  → resolve_once → 直接返回 (幂等)
  → publish → 相同的 bags
```

C：`sglang/02-request-flow/05-配置解析与运行时上下文.md` 的四阶段流水线、`declare_resolution`、`ServerArgs.__reduce__`。I：last-writer-wins 的依赖顺序工程含义。U：多进程实际投影一致性未在本机验证。

**图文说明：** 横轴为阶段（CLI→Record→Resolution→Publish→Bags），纵轴为进程（主进程→子进程）。主进程流水线占四个方框，子进程只有 pickle 恢复 + publish 两个方框，表示子进程跳过解析。虚线箭头表示 pickle 传递 `_resolved_overrides`。

**拓展知识：** 与 I03 衔接：子进程启动时先 `publish(server_args, role="scheduler")` 再构造 Scheduler，因为 ModelConfig、ModelRunner 和通信初始化都依赖 config bags `sglang/01-modules/M15-ipc-control-plane/README.md`。

**常见追问及回答要点：**

1. **问：resolve_once 失败后为什么不允许重试？** 答：因为解析不是事务式的，阶段 C 失败时 A/B 已追加声明，重试会让 A/B 的输出被当作输入再次处理，可能产生错误结果。正确做法是构造新的 `ServerArgs` 重试。
2. **问：运行时如何修改已发布的配置？** 答：不能直接改 `ServerArgs`（已冻结），应通过 `get_context().override(source, **fields)` 修改 config bag 的叶子并记录来源。
3. **问：YAML 和 CLI 参数哪个优先级更高？** 答：CLI 更高。`ConfigArgumentMerger` 把 YAML 参数插在 CLI 参数之前，`argparse` 对单值参数取最后出现的值，因此命令行覆盖文件。
4. **问：`_ForeignDefaults` 的作用是什么？** 答：兼容旧版 resolver 的旧接口（直接写字段而非调用 `declare_resolution`）。它拦截写入操作到内部 `_written` dict，调用结束后再通过 `declare_resolution` 统一记录，保持"原始字段不可变"的不变量 `sglang/02-request-flow/05-配置解析与运行时上下文.md`。

**易错点与反例：**

- 反例："子进程各自读取环境变量自己解析配置"——错误。子进程通过 pickle 接收已解析记录，复用父进程声明。
- 反例："ServerArgs 字段在解析过程中被直接修改"——错误。`declare_resolution` 追加声明，原始字段保持不变。
- 易错：混淆 `get_server_args()`（返回 raw 输入）和 `get_memory()`（返回有效配置）。业务代码应读后者。另一个常见错误是以为 `_resolved_overrides` 是扁平 dict——实际上它是 `(source, dict)` 列表，同一字段可被多次声明，以最后一次为准。

**实践与验证：** [U] 构造两个不同参数的 `ServerArgs`，分别在 `resolve_once` 前后检查 `_resolved_overrides` 长度和内容；再用 `multiprocessing` 传递到子进程验证 bags 值一致。也可在 `resolve_once` 中故意触发某个 hook 的异常，观察 `_resolution_failed` 标记和第二次调用是否被拒绝。实际跨进程 pickle 行为和 hook 失败恢复未在本机执行。

**分层评分标准：** 2分：能描述 CLI→ServerArgs→解析的基本流程；4分：能解释 `declare_resolution` 的声明式语义和 last-writer-wins 规则、`resolving_view` 的只读特性；6分：能分析 `resolve_once` 幂等机制、pickle 跨进程一致性保障、运行时 override 路径、`_ForeignDefaults` 兼容旧插件的机制。

**证据与来源：** `sglang/02-request-flow/05-配置解析与运行时上下文.md`（四阶段流水线、`declare_resolution`、`ServerArgs.__reduce__`）；`sglang/01-modules/M15-ipc-control-plane/README.md`（子进程 publish 顺序）。

### I-I002 ReqState 的状态机包含哪些关键状态？从 RECEIVED 到 COMPLETED/ABORTED 的转换条件是什么？

**元数据：** 知识域：`I02 Tokenizer 与请求状态`；能力目标：`C02 ReqState 生命周期与状态转换`；题型：状态机分析；频率：高频；前置/后续：`I-I001`（配置）、`I-I003`（优雅终止）；证据状态：源码 C，运行 U。

**背景知识：** `ReqState` 是 `TokenizerManager` 侧的单请求本地状态，定义为 dataclass `sglang/01-modules/M03-tokenizer-request-state/README.md`：

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

状态不变量：`rid_to_state[rid]` 存在表示本地仍在等待或处理；`dispatched=False` 时失败可直接删除本地 state；`dispatched=True` 时必须发送 abort 并等待 scheduler 回应；`finished=True` 表示收到终止原因并删除 `rid_to_state`。`BatchStrOutput.finished_reasons` 携带 `"stop"`、`"length"`、`"abort"`、`"tool_call"` 等终止类型 `sglang/01-modules/M03-tokenizer-request-state/README.md`。

**问题分析：** 1) 从 `_init_req_state` 到 `rid_to_state[rid]` 的建立；2) `dispatched` 标志在 tokenize 前后的切换；3) `finished` 置真的条件——scheduler 发送 `BatchStrOutput` 带非空 `finished_reason`；4) 完成路径下 `del rid_to_state[rid]` 与 `state.event.set()` 的时序；5) abort 路径如何同时处理 "正常完成和 abort 竞态"。

**具体答案：** 面试中可以这样回答：

> ReqState 的状态机可以归纳为五个阶段：CREATED（`_init_req_state` 写入 `rid_to_state`，`dispatched=False`，`finished=False`）；TOKENIZING（文本转 token ids，失败时 `dispatched=False` 可直接删除 state）；DISPATCHED（`_dispatch_to_scheduler` 成功后 `dispatched=True`，此后失败必须走 abort）；STREAMING（每收到 `BatchStrOutput` 就 `state.text_chunks.append`、`state.output_ids.extend`、`state.out_list.append`，并通过 `event.set()` 唤醒 `_stream_one_response`）；TERMINATED（`finished=True`，写入完成时间，`del rid_to_state[rid]`）。从 STREAMING 到 TERMINATED 的转换条件是 scheduler 在 `BatchStrOutput.finished_reasons[i]` 写入非空值，常见值有 `"stop"`（遇到 EOS/stop string）、`"length"`（达到 max_new_tokens）、`"abort"`（外部取消或 retraction）。TokenizerManager 收到 abort echo 时的竞态处理是：如果 `rid_to_state` 已被完成路径删除，记录并忽略；否则构造带 finish reason 的最终输出。

**技术洞察：**

```text
CREATED → TOKENIZING → DISPATCHED → STREAMING → TERMINATED
  │            │             │            │           │
  │ dispatch   │ dispatch    │ dispatch   │ dispatch  │ dispatch
  │ =False     │ =False      │ =True      │ =True     │ =True
  │ finished   │ finished    │ finished   │ finished  │ finished
  │ =False     │ =False      │ =False     │ =False    │ =True
  │            │             │            │           │
  fail→del    fail→del     fail→abort  recv→append del rid_to_state
  locally     locally      + keep state + event.set   + out_list.append
                                       + out_dict
```

C：`sglang/01-modules/M03-tokenizer-request-state/README.md` 的 ReqState 字段、`dispatched` 不变量、完成路径的 `_handle_batch_output`。I：竞态处理的工程取舍。U：实际调度器发送 finish_reason 的具体值分布未采样。

**图文说明：** 五个节点水平排列，箭头标注转换触发事件（tokenize 成功、dispatch 成功、recv BatchStrOutput、recv finish reason）。从 STREAMING 到 TERMINATED 的箭头上标注 `finished_reason ∈ {stop, length, abort, tool_call, ...}`。失败分支从 CREATED/TOKENIZING 指向 `del locally`，从 DISPATCHED/STREAMING 指向 `send abort`。

**拓展知识：** 与 I03 衔接：scheduler 侧有独立的 `Req` 对象，其 `to_finish` 状态和 finish reason 通过 `BatchStrOutput` 传递给 `ReqState`，两侧状态不共享，只能通过 IPC 消息对齐 `sglang/01-modules/M15-ipc-control-plane/README.md`。

**常见追问及回答要点：**

1. **问：为什么 dispatched 标志很重要？** 答：它区分"请求是否已进入 scheduler"。未 dispatch 的失败只需删本地 state；已 dispatch 的请求在 scheduler 侧有 Req、KV 等资源，必须发 abort 让 scheduler 释放。
2. **问：非流式模式下 text 何时合并？** 答：非增量流式会延迟 `text_chunks` 到 `text` 的合并，避免每步重建完整字符串；只在最终 out dict 复制时才合并完整文本 `sglang/01-modules/M03-tokenizer-request-state/README.md`。
3. **问：BatchStrOutput 中同一请求的中间结果和 finish 能否分两次到？** 答：可以。增量 decode 的中间 chunk 先于 finish 到达；`_handle_batch_output` 对每次到达都追加并构造 out dict（流式）或延迟合并（非流式）。

**易错点与反例：**

- 反例："ReqState.finished 和 scheduler 侧 Req 的 finish 是同一个变量"——错误。两者位于不同进程，通过 IPC 消息同步。
- 反例："dispatched=True 后可以直接删除 rid_to_state"——错误。必须发 abort 等 scheduler 回应后再清理。
- 易错：混淆 `state.out_list`（等待者消费的队列）和 `state.text_chunks`（增量文本暂存）。

**实践与验证：** [U] 构造 mock `BatchStrOutput` 逐步发送（中间 chunk + finish），验证 `_handle_batch_output` 追加逻辑和最终 out dict 的完整性。当前未实际运行 TokenizerManager 进程。

**分层评分标准：** 2分：能列举 ReqState 的关键字段；4分：能绘制状态机并解释 dispatched 标志的不变量意义；6分：能分析竞态处理、完成路径删除时序、与 scheduler 侧 Req 状态的跨进程对应关系。

**证据与来源：** `sglang/01-modules/M03-tokenizer-request-state/README.md`（ReqState 字段、`_handle_batch_output`、完成路径）；`sglang/01-modules/M15-ipc-control-plane/README.md`（BatchStrOutput 对象契约）。

### I-I003 客户端断连或服务端取消请求时，ReqState 如何优雅终止？资源如何回收？

**元数据：** 知识域：`I02 Tokenizer 与请求状态`；能力目标：`C03 请求取消与资源回收`；题型：机制解释；频率：高频；前置/后续：`I-I002`（状态机）、`I-I005`（优雅关闭）；证据状态：源码 C，运行 U。

**背景知识：** 客户端断连检测在 HTTP 层：`StreamingResponse.background` 安排 `create_abort_task(obj)`，当 ASGI request disconnect 回调触发时向 tokenizer manager 发送 abort `sglang/01-modules/M02-http-api-protocol/README.md`。在 TokenizerManager 内部，`_stream_one_response` 在等待 `state.event` 时检测到 HTTP request disconnected，调用 `abort_request` 并抛异常结束调用栈 `sglang/01-modules/M03-tokenizer-request-state/README.md`。scheduler 侧的 `abort_request` 从 waiting queue 删除匹配请求，释放 request row 和 KV 资源，对 disaggregation 模式还需清理 metadata/receiver，最后通过 `send_to_tokenizer` 回送 abort output `sglang/01-modules/M15-ipc-control-plane/README.md`。关键不变量：`dispatched=False` 时只删本地 state；`dispatched=True` 时必须发 abort 消息，保留 state 等 scheduler 回应后再删除。

**问题分析：** 1) HTTP 层 disconnect 如何被检测到；2) `_stream_one_response` 从等待到调用 `abort_request` 的转换；3) AbortReq 消息跨越哪些 IPC 边界；4) scheduler 侧等待队列/运行队列/已 dispatch 三种场景各自的清理路径；5) abort echo 到达时与完成路径的竞态处理。

**具体答案：** 面试中可以这样回答：

> 优雅终止分四层配合。第一层是 HTTP 检测：`StreamingResponse.background` 注册 `create_abort_task`，FastAPI/Starlette 在客户端断开时调用该任务，向 tokenizer manager 发送 abort 信号 `sglang/01-modules/M02-http-api-protocol/README.md`。第二层是 TokenizerManager 判断：`_stream_one_response` 在 `await state.event.wait()` 期间如果检测到 request disconnected，标记后调用 `abort_request()`，它根据 `state.dispatched` 分支——未 dispatch 直接 `del rid_to_state[rid]`，已 dispatch 则构造 `AbortReq(rid=...)` 通过 `send_to_scheduler` 推送到 scheduler，保留 state 等后续回应 `sglang/01-modules/M03-tokenizer-request-state/README.md`。第三层是 scheduler 处理：`abort_request` 分三种情况——waiting queue 中的请求直接删除释放资源；running batch 中的请求设置 `to_finish` 让事件循环在安全边界清理；disaggregation 状态的请求额外释放 metadata/KV receiver `sglang/01-modules/M15-ipc-control-plane/README.md`。第四层是竞态处理：scheduler 回送 abort echo，如果 `rid_to_state` 已被完成路径删除（正常完成先到），记录并忽略该 echo；否则构造带 finish_reason="abort" 的最终输出。

**技术洞察：**

```text
HTTP disconnect → create_abort_task → abort_request()
  ├─ state.dispatched=False → del rid_to_state[rid]（本地清理）
  └─ state.dispatched=True  → send AbortReq → scheduler
                                        ├─ waiting queue → del + release KV/row
                                        ├─ running batch → set to_finish
                                        └─ disaggregation → release metadata/receiver
                                        → echo back → TokenizerManager
                                          ├─ state 已删 → ignore echo
                                          └─ state 仍存 → final output + del
```

C：`sglang/01-modules/M02-http-api-protocol/README.md` 的 abort background task；`sglang/01-modules/M03-tokenizer-request-state/README.md` 的 `_stream_one_response` abort 分支；`sglang/01-modules/M15-ipc-control-plane/README.md` 的 scheduler abort_request。I：disaggregaton 额外清理的工程必要性。U：真实 HTTP disconnect 触发时序未验证。

**图文说明：** 四个水平泳道（HTTP、TokenizerManager、Scheduler、竞态处理）。断连信号从 HTTP 泳道出发，经 `create_abort_task` 进入 TokenizerManager 泳道，`dispatched` 标志分两路——本地删除或发往 Scheduler。Scheduler 泳道内三种清理路径水平排列。echo 箭头从 Scheduler 回指 TokenizerManager，标注两种处理分支。

**拓展知识：** 与 I03 衔接：`AbortReq` 是反向控制消息，和 `ShutdownReq` 一样通过 ZMQ 传输；但与 shutdown 不同，abort 只针对单个 rid，不终止进程 `sglang/01-modules/M15-ipc-control-plane/README.md`。

**常见追问及回答要点：**

1. **问：为什么不能直接在 scheduler 删除后通知 HTTP 层？** 答：因为 HTTP 层看到的只是 `ReqState`，scheduler 侧的 `Req` 在不同进程。必须先回送 abort echo，让 `rid_to_state` 的完成路径做最终输出，HTTP 层才能 yield 结束事件。
2. **问：abort_request 时 scheduler 正在 forward 怎么办？** 答：不会在任意 CUDA forward 中途直接删除 Req，而是通过 `to_finish` 状态让事件循环在一致的阶段（`process_batch_result` 后）完成清理 `sglang/02-request-flow/03-调度器与连续批处理.md`。
3. **问：abort 和 retraction 有什么不同？** 答：abort 是外部取消或错误导致的请求级终止，释放请求所有资源；retraction 是 KV 不足时的临时撤回，释放 decode 资源后把请求放回 waiting queue 等待后续恢复 `sglang/02-request-flow/03-调度器与连续批处理.md`。
4. **问：disaggregaton 模式下的 abort 有什么额外操作？** 答：除了清理 waiting queue 和 running batch 的资源外，还需要释放 disaggregation 特有的 metadata 和 KV receiver 资源——这些是普通模式下不存在的额外状态 `sglang/01-modules/M15-ipc-control-plane/README.md`。

**易错点与反例：**

- 反例："客户端断连直接删除 scheduler 侧的 Req"——错误。两侧在不同进程，必须通过 AbortReq IPC 消息通知。
- 反例："dispatched=False 也要发 abort"——错误。未进入 scheduler 的请求本地直接删除即可，发 abort 反而会让 scheduler 收到无效 rid。
- 易错：忽略竞态——abort echo 到达时 `rid_to_state` 可能已被正常完成删除，需要判断后再处理。另一个常见错误是 `_stream_one_response` 中 stream 的 `ValueError` 与 disconnect 的组合处理：如果同时检测到 disconnect，只记录并停止，不再向已断开的客户端发送 400 错误，维护"断连是取消而非输入错误"的语义 `sglang/01-modules/M02-http-api-protocol/README.md`。

**实践与验证：** [U] 用 `curl` 发起 SSE 请求后强制断开，在 TokenizerManager 侧日志观察 `abort_request` 是否触发、`dispatched` 分支是否正确。也可以构造 dispatched=False 的 tokenize 失败场景验证本地直接清理路径。实际 HTTP 断连实验和 tokenize 失败注入未执行。

**分层评分标准：** 2分：能描述客户端断连检测的基本方式；4分：能分析 `dispatched` 标志的分支处理和 AbortReq 传递路径；6分：能解释竞态处理、三种 scheduler 清理路径、与 retraction 的语义差异、stream ValueError 与 disconnect 的组合处理逻辑。

**证据与来源：** `sglang/01-modules/M02-http-api-protocol/README.md`（abort task、stream ValueError 处理）；`sglang/01-modules/M03-tokenizer-request-state/README.md`（`_stream_one_response` abort）；`sglang/01-modules/M15-ipc-control-plane/README.md`（scheduler abort_request）。

### I-I004 SGLang 多进程架构中消息如何跨边界传递？背压机制如何防止下游过载？

**元数据：** 知识域：`I03 IPC 与多进程`；能力目标：`C04 IPC 通道拓扑与背压`；题型：架构解释；频率：高频；前置/后续：`I-I001`（配置跨进程）、`I-I005`（优雅关闭）；证据状态：源码 C，多进程运行 U。

**背景知识：** 标准 SRT runtime 把控制面、GPU 执行面和文本转换面分成三个进程，通过 ZMQ PUSH/PULL 通道连接 `sglang/01-modules/M15-ipc-control-plane/README.md`：

```text
TokenizerManager --PUSH--> scheduler_input_ipc_name --PULL--> rank-0 Scheduler
Scheduler        --PUSH--> detokenizer_ipc_name     --PULL--> Detokenizer
Detokenizer      --PUSH--> tokenizer_ipc_name       --PULL--> TokenizerManager
Engine/RPC       --DEALER-> rpc_ipc_name            --DEALER-> rank-0 Scheduler
Scheduler        --PUSH--> metrics_ipc_name
```

非 rank-zero scheduler 使用 `SenderWrapper(None)`，不竞争消费同一个 API 输入 socket `sglang/01-modules/M15-ipc-control-plane/README.md`。消息序列化默认走 msgspec，大字段用 `PickleWrapper` 或共享内存传输。backpressure 不只来自 ZMQ high-water mark，还来自 scheduler 侧 waiting/running batch、detokenizer decode capacity、TokenizerManager event loop 的消费速率，以及 streaming chunk coalescing 导致的积压 `sglang/01-modules/M15-ipc-control-plane/README.md`。

**问题分析：** 1) `PortArgs` 如何集中管理所有 endpoint 地址；2) TokenizerManager/Scheduler/Detokenizer 各自建立哪些 socket；3) rank-zero 与非 rank-zero scheduler 的差异；4) PUSH/PULL 模式下的消息路由；5) 多层 backpressure 的来源与传播方向。

**具体答案：** 面试中可以这样回答：

> 消息跨边界传递基于 `PortArgs` 集中分配的 ZMQ endpoint，避免各进程自行拼接地址导致不一致 `sglang/01-modules/M15-ipc-control-plane/README.md`。`PortArgs.init_new` 在单机模式下用临时文件名生成 `ipc://...` 地址，DP attention 模式下用 TCP 地址加端口偏移 `sglang/01-modules/M15-ipc-control-plane/README.md`。每个进程按角色建立 socket：TokenizerManager 创建 `recv_from_detokenizer`(PULL) 和 `send_to_scheduler`(PUSH)；rank-0 Scheduler 创建 `recv_from_tokenizer`(PULL)、`recv_from_rpc`(DEALER)、`send_to_detokenizer`(PUSH)；Detokenizer 创建 `recv_from_scheduler`(PULL) 和 `send_to_tokenizer`(PUSH)。非 rank-zero scheduler 不建立收发 socket，通过分布式通信参与计算。背压来源有四个层次：(1) ZMQ PUSH socket 的 high-water mark——超过后 send 阻塞或丢弃；(2) scheduler 侧的 waiting queue 满时 `_abort_on_queued_limit` 拒绝新请求；(3) detokenizer 的 `DecodeStatus` 消费速率——跟不上时会延迟 TokenizerManager 的 PULL；(4) streaming chunk coalescing——TokenizerManager 合并多个增量 chunk 减少处理开销，但可能增加 P99 ITL `sglang/01-modules/M15-ipc-control-plane/README.md`。

**技术洞察：**

```text
PortArgs (单点计算)
  ├─ tokenizer_ipc_name       ← Detokenizer PUSH → TokenizerManager PULL
  ├─ scheduler_input_ipc_name ← TokenizerManager PUSH → Scheduler PULL
  ├─ detokenizer_ipc_name     ← Scheduler PUSH → Detokenizer PULL
  ├─ rpc_ipc_name             ← Engine DEALER ↔ Scheduler DEALER
  └─ metrics_ipc_name         ← Scheduler PUSH → metrics PULL

backpressure:
  ZMQ HWM → send 阻塞
  waiting queue 满 → 拒绝新请求
  detokenizer 慢 → TokenizerManager PULL 延迟
  chunk coalescing → P99 ITL 上升
```

C：`sglang/01-modules/M15-ipc-control-plane/README.md` 的 `PortArgs` 字段、进程 socket 创建、backpressure 分析。I：chunk coalescing 对 P99 的影响机制。U：各层背压的实际触发阈值未测量。

**图文说明：** 三个进程方框水平排列，箭头标注 socket 类型和 IPC 名称。每个进程下方标注其 PULL/PUSH 角色。第四层 "backpressure sources" 用虚线箭头指向对应的瓶颈位置。

**拓展知识：** 与 I03 衔接：`skip_tokenizer_init` 时 Scheduler 的 `send_to_detokenizer` 实际连接到 `tokenizer_ipc_name`，跳过 Detokenizer 直接回 TokenizerManager `sglang/01-modules/M15-ipc-control-plane/README.md`。

**常见追问及回答要点：**

1. **问：为什么用 ZMQ 而不是 Python multiprocessing.Queue？** 答：ZMQ PUSH/PULL 支持多对一路由、跨节点 TCP 部署、独立的 high-water mark 控制，且 socket 生命周期与进程解耦更灵活。
2. **问：rank-zero scheduler 成为瓶颈怎么办？** 答：DP attention 模式下可创建多个 DP controller，每个管理一组 scheduler，分担请求入口压力。
3. **问：msgpack 和 pickle 的选择标准是什么？** 答：默认 msgspec 编码 Struct，类型约束让 IPC 变化可被发现；只有确实无法 msgpack 化的 payload 才用 `PickleWrapper` 隔离 `sglang/01-modules/M15-ipc-control-plane/README.md`。

**易错点与反例：**

- 反例："每个 TP rank 的 scheduler 都直接接收 HTTP 请求"——错误。只有 rank-zero scheduler 拥有请求收发 socket。
- 反例："PUSH 发送成功等于 scheduler 已处理"——错误。PUSH 成功只表示消息进入 ZMQ 发送路径，不代表 admission 或 GPU 执行完成。
- 易错：混淆 `mp.Pipe`（启动阶段 ready 信息）和 ZMQ（稳态数据流）。

**实践与验证：** [U] 用 `ipc://` endpoint 启动三进程拓扑，分别在各 socket 上统计消息速率和 queue 深度。实际多进程运行未执行。

**分层评分标准：** 2分：能描述 TokenizerManager→Scheduler→Detokenizer 的传递方向；4分：能分析 PortArgs 集中管理、rank-zero 独占收发、PUSH/PULL 路由；6分：能完整解释四层背压来源、chunk coalescing 的 P99 影响、skip_tokenizer 路径变化。

**证据与来源：** `sglang/01-modules/M15-ipc-control-plane/README.md`（PortArgs、socket 创建、backpressure、batch 消息契约）；`sglang/02-request-flow/06-多进程与IPC.md`（进程拓扑、序列化）。

### I-I005 优雅关闭时进程间如何协调？为什么不能直接 kill 所有进程？

**元数据：** 知识域：`I03 IPC 与多进程`；能力目标：`C05 优雅关闭协调`；题型：机制解释与辨析；频率：中频；前置/后续：`I-I003`（请求级终止）、`I-I004`（IPC 通道）；证据状态：源码 C，运行 U。

**背景知识：** 收到 SIGTERM 后，TokenizerManager 的 watchdog 执行协调关闭 `sglang/01-modules/M15-ipc-control-plane/README.md`：

```text
停止接收新请求/等待现有 rid 清空
  → 停止 subprocess watchdog（子进程退出是预期行为）
  → dispatch ShutdownReq（广播到各 TP rank）
  → 等待 scheduler processes
  → kill_process_tree 处理残余子进程
  → sys.exit(0)
```

`ShutdownReq` 是广播到各 TP rank 的终止消息。Scheduler 的 `handle_shutdown` 只设置 `gracefully_exit=True` 让 event loop 退出，`run_scheduler_process` 的 `finally` 再执行指标发布线程 teardown，并在 graceful path 释放 host resources `sglang/01-modules/M15-ipc-control-plane/README.md`。Engine 的 shutdown 还会停止 watchdog、关闭 RPC socket、优先终止 weight-cache daemons，最后 `kill_process_tree`，`finally` 关闭 multimodal processor 和 CUDA VMM feature transport。不能直接 kill 的原因：(1) 已 dispatch 请求只在本地删 state 会泄漏 scheduler 侧资源；(2) GPU wedged 时同步清理可能 hang；(3) daemon/IPC/multimodal transport 等非 scheduler 资源需要各自 owner 释放 `sglang/01-modules/M15-ipc-control-plane/README.md`。

**问题分析：** 1) SIGTERM 信号如何到达 TokenizerManager；2) watchdog 协调关闭的顺序设计——为什么先停接收再发 ShutdownReq；3) Scheduler 侧 `handle_shutdown` 为什么只设标志而不直接释放资源；4) 异常路径为什么不执行 `release_host_resources`；5) `finally` 兜底清理的内容。

**具体答案：** 面试中可以这样回答：

> 优雅关闭的协调分四阶段。第一阶段是"停流"：watchdog 停止接收新请求，等待 `rid_to_state` 清空，这是为了确保不再有新的 `ReqState` 建立。第二阶段是"广播"：dispatch `ShutdownReq` 到所有 TP rank 的 scheduler，同时停止 subprocess watchdog（子进程退出属于预期，不再当作 crash 重启）。第三阶段是"等待"：等待 scheduler 进程退出——scheduler 的 `handle_shutdown` 只设置 `gracefully_exit=True`，让 event loop 自然退出循环；`run_scheduler_process` 的 `finally` 块负责 teardown 指标线程，在 graceful path 才执行 `release_host_resources`（异常路径跳过以避免 GPU wedged 时同步操作 hang）。第四阶段是"兜底"：`kill_process_tree` 处理残余子进程，Engine shutdown 的 `finally` 关闭 RPC socket、multimodal processor 和 CUDA VMM feature transport。不能直接 kill 的原因有三：(1) 已 dispatch 的请求在 scheduler 侧持有 KV/request row，只删本地 state 会泄漏；(2) 异常路径在可能已 wedged 的 GPU 上执行同步清理会 hang；(3) multimodal transport 等非 scheduler 资源必须由各自 owner 释放。

**技术洞察：**

```text
SIGTERM → watchdog:
  1. stop accepting new requests
  2. wait for rid_to_state to drain
  3. stop subprocess watchdog
  4. dispatch ShutdownReq → all TP ranks
  5. wait for scheduler processes
  6. kill_process_tree (residual)
  7. sys.exit(0)

Scheduler handle_shutdown:
  set gracefully_exit=True → event loop exits
  finally: teardown metrics threads
  graceful path: release_host_resources
  except path: skip (GPU may be wedged)
```

C：`sglang/01-modules/M15-ipc-control-plane/README.md` 的 graceful shutdown 顺序、`handle_shutdown` 行为、异常路径跳过清理。I：异常路径跳过清理的取舍理由。U：真实 SIGTERM 下的进程退出时序未验证。

**图文说明：** 纵向流程图：SIGTERM 进入 TokenizerManager watchdog，依次执行四个阶段（停流→广播→等待→兜底）。从阶段 2 画虚线箭头到 Scheduler 的 `handle_shutdown`，标注 `gracefully_exit=True`。Scheduler 退出后进入 `finally` 块，graceful path 标注 `release_host_resources`，except path 标注 `skip`。

**拓展知识：** 与 I03 衔接：子进程异常路径会向 parent 发送 `SIGQUIT`，这是一种"子进程主动通知 + 主进程统一收尾"的模型 `sglang/01-modules/M15-ipc-control-plane/README.md`。

**常见追问及回答要点：**

1. **问：为什么 handle_shutdown 只设置标志而不直接释放资源？** 答：因为资源释放涉及 GPU 同步操作，必须由 `run_scheduler_process` 的 finally 块在正确上下文中执行；直接在 handler 中释放可能与正在进行的 forward 产生冲突。
2. **问：ShutdownReq 和 AbortReq 有什么区别？** 答：AbortReq 针对单个 rid 的请求级取消；ShutdownReq 是广播到所有 TP rank 的服务级终止信号。
3. **问：graceful wait 超时后怎么办？** 答：watchdog 的 `kill_process_tree` 兜底处理残余子进程，确保不会有僵尸进程残留。

**易错点与反例：**

- 反例："直接 SIGKILL 所有进程最快"——异常路径下可能泄漏 GPU 资源、留下悬挂的 IPC socket。
- 反例："handle_shutdown 直接释放 host resources"——错误。异常路径必须跳过避免 hang。
- 易错：忘记 `finally` 中的 multimodal/VMM 清理——Engine 的 shutdown 必须关闭这些非 scheduler 资源。

**实践与验证：** [U] 启动服务后发送 SIGTERM，用 `ps` 观察进程退出顺序和是否有残留。实际关闭时序未执行。

**分层评分标准：** 2分：能描述 shutdown 的基本步骤；4分：能解释顺序设计（先停流再广播）和 Scheduler 侧的标志+finally 模式；6分：能分析异常路径跳过清理的取舍、kill_process_tree 兜底、多类型资源的分层释放。

**证据与来源：** `sglang/01-modules/M15-ipc-control-plane/README.md`（graceful shutdown 顺序、`handle_shutdown`、`destroy_model_parallel` 清理链）。

### I-I006 continuous batching 下请求的调度单位是什么？batch 内请求如何完成与退出？

**元数据：** 知识域：`I04 调度器与连续批处理`；能力目标：`C06 调度单位与 batch 生命周期`；题型：概念解释与流程分析；频率：高频；前置/后续：`I-B001`（prefill/decode 基础）、`I-I007`（PrefillAdder）；证据状态：源码 C，运行 U。

**背景知识：** continuous batching 的调度单位不是一次 kernel launch，而是**一轮 scheduler iteration**——每轮先 `ingest_requests` 再 `get_next_batch_to_run` 决定本轮 batch，然后 `run_batch` 执行 forward，最后 `process_batch_result` 处理结果 `sglang/02-request-flow/03-调度器与连续批处理.md`。动态性来自"每一轮重新组成 batch"，新请求在 iteration 边界加入，而非插入正在执行的 kernel。调度策略包括 LPM（最长前缀命中）、DFS_WEIGHT、HRRN、FCFS 等，队列超过 128 时 LPM/HRRN 会降级为 FCFS `sglang/02-request-flow/03-调度器与连续批处理.md`。batch 完成：`update_running_batch` 中的 `filter_batch` 删除已完成请求（EOS/stop string/max tokens/abort），释放其不再需要的资源 `sglang/02-request-flow/03-调度器与连续批处理.md`。

**问题分析：** 1) scheduler event loop 每轮的四个步骤；2) "调度单位是 iteration 而非 kernel" 的含义；3) 调度策略的 cache-aware vs cache-agnostic 选择；4) `filter_batch` 的触发条件和完成判定；5) 完成请求的资源释放路径。

**具体答案：** 面试中可以这样回答：

> 调度单位是 scheduler 的一**轮 event loop iteration**，而不是单次 CUDA kernel launch。每轮分四步：(1) `ingest_requests` 从 IPC 取出新 tokenized request，创建 `Req` 加入 waiting queue；(2) `get_next_batch_to_run(running_batch, last_batch)` 做调度决策——先合并上一轮 prefill 结果到 running batch，再尝试构造新 prefill batch，没有可运行 prefill 时转 decode；(3) `run_batch` 把 batch 交给 TP worker 的 `forward_batch_generation`，取得 `GenerationBatchResult`；(4) `process_batch_result` 按 forward mode 分派 decode/prefill processor，更新 `Req.output_ids`、判断 finish、发送输出。新请求在 iteration 边界加入 batch，不会插入已发射的 kernel `sglang/02-request-flow/03-调度器与连续批处理.md`。batch 内请求完成退出的机制是 `update_running_batch` 调用 `filter_batch()`：达到 EOS、stop string、max_new_tokens 或 abort 状态的请求从 running batch 移除，释放 request row 和 KV slot；剩余请求继续下一轮 decode。调度策略在少量请求时用 LPM 最大化 prefix cache 命中，队列超过 128 时降级为 FCFS 避免 CPU 调度成本过高 `sglang/02-request-flow/03-调度器与连续批处理.md`。

**技术洞察：**

```text
while not gracefully_exit:
  1. ingest_requests()           ← 新请求入 waiting queue
  2. get_next_batch_to_run()     ← running_batch + waiting → batch_to_run
     ├─ prefill first (if budget allows)
     └─ decode (update_running_batch → filter_batch → prepare_for_decode)
  3. run_batch(batch)            ← forward → GenerationBatchResult
  4. process_batch_result()      ← update Req state, check finish, send output

调度单位 = 一次 iteration 的 batch_to_run
动态性   = 每轮重新组合，不在 kernel 执行中插入
```

C：`sglang/02-request-flow/03-调度器与连续批处理.md` 的普通循环、调度策略、`filter_batch`。I：队列长度对策略降级的影响。U：实际 batch 组合的时序未采样。

**图文说明：** 四个步骤的环形流程图。步骤 2 内展开两个分支：prefill 路径（从 waiting 选请求→`ScheduleBatch`→extend）和 decode 路径（`update_running_batch`→`filter_batch`→`prepare_for_decode`）。步骤 4 分出两条箭头：finished → output IPC，alive → `running_batch`（回到步骤 2）。

**拓展知识：** 与 I08 衔接：完成请求的 KV 在 `cache_finished_req` 中插入 Radix tree，供后续请求 prefix 复用 `sglang/01-modules/M08-kv-cache/README.md`。

**常见追问及回答要点：**

1. **问：为什么 prefill 优先于 decode？** 答：这是调度策略选择——prefill 优先让新请求尽快获得首 token（降低 TTFT），但长 prefill 可能挤压 decode，系统通过 token budget 和 mixed chunk 在两者间平衡 `sglang/02-request-flow/03-调度器与连续批处理.md`。
2. **问：mixed chunk 是什么？** 答：`is_mixed_chunk` 时新 prefill batch 可以把已有的 decode 请求行一起带入一次 forward，减少 kernel launch 间隙，但要求输入布局和结果语义兼容 `sglang/02-request-flow/03-调度器与连续批处理.md`。
3. **问：chunked prefill 如何影响调度单位？** 答：长 prompt 被切成多个 chunk 跨多轮处理，同一 `Req` 复用 request row，partial page 和 `cache_protected_len` 在 chunk 边界需要特殊处理 `sglang/02-request-flow/03-调度器与连续批处理.md`。
4. **问：`last_batch` 和 `running_batch` 的关系是什么？** 答：`last_batch` 是上一轮 forward 的具体 batch（可能是 prefill 或 decode），`running_batch` 是决策后仍活跃的请求集合。每轮开始时 `get_next_batch_to_run` 会先合并上一轮 prefill 结果到 running batch，再做本轮决策 `sglang/02-request-flow/03-调度器与连续批处理.md`。

**易错点与反例：**

- 反例："continuous batching 是修改正在执行的 kernel 来插入新请求"——错误。动态性发生在 iteration 边界。
- 反例："filter_batch 只在请求结束时调用"——不完全正确。它在每轮 `update_running_batch` 都调用，及时移除已完成请求。
- 易错：混淆 waiting queue 和 running_batch——前者是尚未开始执行的请求集合，后者是持续 decode 的活跃集合。另一个常见错误是以为 `running_batch` 中的请求一定在本轮执行——实际上如果 prefill 优先且 token budget 用完，running batch 的 decode 会被推迟到下一轮。

**实践与验证：** [U] 注入不同长度的 prompt 序列，观察 scheduler 日志中 batch 组成和 iteration 边界变化。具体可以固定 `max_prefill_tokens=4096`，注入 10 个不同长度的 prompt，观察哪些请求进入第一轮 prefill、哪些因 budget 不足被推迟。实际调度行为未执行。

**分层评分标准：** 2分：能描述 event loop 的基本步骤；4分：能解释 iteration 级动态性和调度策略选择（LPM/FCFS 的切换条件）；6分：能分析 mixed chunk 的兼容性约束、chunked prefill 对调度单位的影响、`last_batch` 与 `running_batch` 在 prefill-merge 中的协作机制。

**证据与来源：** `sglang/02-request-flow/03-调度器与连续批处理.md`（普通循环、调度策略、mixed chunk、filter_batch）；`sglang/01-modules/M04-scheduler-batching/README.md`（PrefillAdder、ScheduleBatch）。

### I-I007 PrefillAdder 如何预算 prefill 与 decode 的资源分配？拒绝新请求的判定条件？

**元数据：** 知识域：`I04 调度器与连续批处理`；能力目标：`C07 资源预算与准入`；题型：机制解释；频率：高频；前置/后续：`I-I006`（调度单位）、`I-I011`（PagedAttention）；证据状态：源码 C，运行 U。

**背景知识：** `PrefillAdder` 是 `get_new_batch_prefill` 创建的"入场检查器"，同时考虑多个预算维度 `sglang/02-request-flow/03-调度器与连续批处理.md`：

```text
admissible(request) =
    request-row budget (get_num_allocatable_reqs)
  ∩ prefill-token budget (max_prefill_tokens)
  ∩ KV page/slot budget (allocator capacity)
  ∩ max-running-requests
  ∩ chunked prefill size (chunked_prefill_size)
  ∩ LoRA / grammar / beam constraints
```

`add_one_req` 先调用 `req.init_next_round_input(tree_cache)` 计算 prefix hit，再尝试为扩展 token 分配 KV 位置。返回 `CONTINUE` 继续尝试下一个请求，`NO_TOKEN` 表示预算不足，设置 `batch_is_full` 并尝试 `preempt_to_schedule(req)`——抢占失败则停止遍历 `sglang/02-request-flow/03-调度器与连续批处理.md`。request-row budget 的核心计算：`min(pp_max_micro_batch_size - running_bs, req_to_token_pool.available_size())`；token budget：原始 prompt 长度不等于本轮计算量，prefix cache 命中 3000 的 4000-token prompt 实际只需 extend 1000 `sglang/02-request-flow/03-调度器与连续批处理.md`。

**问题分析：** 1) `PrefillAdder` 的构造参数及其对应的预算维度；2) `add_one_req` 的两阶段——先 prefix match 再分配 KV；3) `get_num_allocatable_reqs` 的双重约束；4) token budget 基于原始长度还是实际 extend 长度；5) 拒绝/抢占/停止遍历的三种结果。

**具体答案：** 面试中可以这样回答：

> `PrefillAdder` 在构造时接收 page_size、tree_cache、allocator、running_batch、new_token_ratio、max_prefill_tokens、chunked_prefill_size、max_prefill_bs、max_running_requests 等参数，每个参数对应一个预算维度 `sglang/02-request-flow/03-调度器与连续批处理.md`。对每个候选请求执行两步检查：第一步 `req.init_next_round_input(tree_cache)` 做 Radix tree prefix match，计算 `prefix_indices`（已命中的 KV）和 `extend_range`（本轮需要写入的区间）；第二步 `adder.add_one_req(req)` 尝试为 extend_range 分配 KV 位置，同时扣减 request-row 余额和 prefill-token 余额。request-row 预算受 `min(pp_max_micro_batch_size - running_bs, pool.available_size())` 双重约束——既不能超过 PP micro batch 上限，也不能超过 request-to-token pool 的可用行数。prefill-token 预算基于**实际 extend 长度**而非原始 prompt——prefix cache 命中 3000 的 4000-token prompt 实际只消耗 1000 token 的预算。当 `add_one_req` 返回 `NO_TOKEN` 时，设置 `batch_is_full=True`，尝试 `preempt_to_schedule(req)` 抢占低优先级请求；抢占失败则停止遍历，不再尝试后续请求，因为排序后的高优先级请求都放不进去，跳过可能破坏调度语义 `sglang/02-request-flow/03-调度器与连续批处理.md`。

**技术洞察：**

```text
PrefillAdder 预算维度:
  ├─ request-row: min(pp_max_bs - running_bs, pool.available)
  ├─ prefill-token: sum(extend_lengths) ≤ max_prefill_tokens
  ├─ KV capacity: allocator.available_size() ≥ needed pages
  ├─ max-running: len(running.reqs) + new ≤ max_running_requests
  ├─ chunked: extend ≤ chunked_prefill_size (if chunking enabled)
  └─ beam/grammar/LoRA: per-request constraints

add_one_req 流程:
  init_next_round_input → prefix match → extend_range
  → allocate KV for extend_range
  ├─ success → CONTINUE (next request)
  └─ NO_TOKEN → try preempt → fail → stop traversal
```

C：`sglang/02-request-flow/03-调度器与连续批处理.md` 的 PrefillAdder 参数、`add_one_req`、`get_num_allocatable_reqs`、NO_TOKEN 处理。I：停止遍历的调度语义保护。U：实际各维度拒绝比例未采样。

**图文说明：** 左侧列出候选请求 A、B、C。中间是 PrefillAdder 检查流程：先过 request-row 预算，再过 prefill-token 预算，再过 KV capacity。A 和 B 通过进入 `can_run_list`，C 在 prefill-token 预算处失败 → NO_TOKEN → 尝试 preempt → 失败 → 停止。

**拓展知识：** 与 I08 衔接：allocator capacity 不足不仅拒绝新请求，还会在 decode 阶段触发 `retract_decode`——撤回已有请求释放 KV 空间 `sglang/01-modules/M08-kv-cache/README.md`。

**常见追问及回答要点：**

1. **问：为什么 token budget 不看原始 prompt 长度？** 答：因为 prefix cache 可能已缓存大部分 prompt，本轮只需 extend 未命中部分。按原始长度预算会浪费本可容纳的请求。
2. **问：抢占和 retraction 有什么不同？** 答：抢占（preempt）是把 running batch 中的请求移除以腾出空间给更高优先级的 prefill；retraction 是 decode 阶段 KV 不足时的临时撤回，请求回到 waiting queue 等待恢复 `sglang/02-request-flow/03-调度器与连续批处理.md`。
3. **问：`AddReqResult.NO_TOKEN` 和 `CONTINUE` 的区别？** 答：CONTINUE 表示当前请求成功加入，继续尝试下一个；NO_TOKEN 表示预算耗尽，尝试抢占或停止。

**易错点与反例：**

- 反例："PrefillAdder 只看 batch size"——错误。它同时考虑 token budget、KV capacity、request-row 等多个维度。
- 反例："NO_TOKEN 后继续尝试后面的请求"——错误。尝试抢占失败后应停止遍历，否则可能破坏优先级排序语义。
- 易错：混淆 request-row 和 KV slot 的容量——前者限制并发请求数，后者限制 KV 总 token 数。

**实践与验证：** [U] 设置 `max_prefill_tokens=4096`，注入不同 prefix hit 率的请求序列，观察 `can_run_list` 组成和拒绝点。实际调度未执行。

**分层评分标准：** 2分：能描述 PrefillAdder 是入场检查器；4分：能列出多个预算维度并解释 `add_one_req` 的两步流程；6分：能分析 request-row 双重约束、实际 extend 长度预算、抢占与停止遍历的调度语义。

**证据与来源：** `sglang/02-request-flow/03-调度器与连续批处理.md`（PrefillAdder 参数、add_one_req、get_num_allocatable_reqs、NO_TOKEN 处理）；`sglang/01-modules/M04-scheduler-batching/README.md`（PrefillAdder 数据结构）。

### I-I008 attention kernel 选择(FA/flash/paged)的决策因素是什么？不同 kernel 的适用场景？

**元数据：** 知识域：`I05 计算图与 Attention backend`；能力目标：`C08 attention backend 选择与适用场景`；题型：对比分析；频率：中频；前置/后续：`I-B001`（prefill/decode 基础）、`I-I010`（CUDA Graph）；证据状态：源码 C，GPU 运行 U。

**背景知识：** SGLang 的 attention backend 不是"导入哪个 Python 文件"这么简单 `sglang/01-modules/M09-attention-cuda-graph/README.md`。backend registry 先根据名称映射到创建函数，再按设备、MLA、speculative、PDMUX 和 overlap 改变最终对象拓扑。例如 `flashinfer` 在非 MLA 时实例化 `FlashInferAttnBackend`，MLA 时转到 `FlashInferMLAAttnBackend`；`trtllm_mla`、`tokenspeed_mla`、`cutedsl_mla` 强制要求 MLA，且 TRT-LLM MLA 在 DCP+speculative 组合下直接拒绝 `sglang/01-modules/M09-attention-cuda-graph/README.md`。`HybridAttnBackend` 是协议适配器而非 kernel，EXTEND 交给 prefill backend，decode/idle 交给 decode backend `sglang/01-modules/M09-attention-cuda-graph/README.md`。`TorchNativeAttnBackend` 是正确性 fallback，使用 PyTorch SDPA，性能语义不同 `sglang/01-modules/M09-attention-cuda-graph/README.md`。决策因素包括：model architecture（MHA/MLA/DSA/NSA/Mamba）、ForwardMode（extend/decode/idle/target_verify）、KV pool 类型（paged/vma/custom）、page size、TP/DP/CP/DCP 宽度、是否 speculative `sglang/01-modules/M09-attention-cuda-graph/README.md`。

**问题分析：** 1) backend registry 的分层实例化逻辑；2) 普通 MHA vs MLA vs 稀疏 attention vs Mamba 对 backend 选择的影响；3) PDMUX 和 overlap 模式下的 backend 拓扑；4) `HybridAttnBackend` 的协议适配器角色；5) `TorchNativeAttnBackend` 的 fallback 定位。

**具体答案：** 面试中可以这样回答：

> backend 选择是一个多维度的能力协商，不能仅凭 `--attention-backend` 推断 kernel layout `sglang/01-modules/M09-attention-cuda-graph/README.md`。决策因素有七个：(1) 模型架构——MHA 走 flashinfer/triton 等 fused backend，MLA（DeepSeek）走 `FlashInferMLAAttnBackend` 或 TRT-LLM MLA，DSA/NSA/QSA 各有专用 backend，Mamba/linear 维护 recurrent state 而非 QKV attention；(2) ForwardMode——EXTEND 通常交给 prefill backend，decode/idle 交给 decode backend，speculative 的 TARGET_VERIFY 根据 attention mode 选择其中之一；(3) KV pool 类型——paged pool 需要 backend 支持 page table 索引，VMA pool 需要 virtual memory 映射；(4) TP/DP/CP/DCP 宽度——不同并行维度影响 local heads 数和 token 分布；(5) speculative decoding——`trtllm_mla` 在 DCP+speculative 组合下直接拒绝，因为实现不能提供跨 rank merge 所需 metadata；(6) PDMUX 模式——创建一个主 backend 加多个 decode backend；(7) overlap 模式——用 `TboAttnBackend` 包装子 backend `sglang/01-modules/M09-attention-cuda-graph/README.md`。`HybridAttnBackend` 不是独立 kernel，而是根据 ForwardMode 分发到子 backend 的协议适配器。`TorchNativeAttnBackend` 是明确的正确性 fallback，用 PyTorch SDPA，性能不如 fused backend 但保证功能正确。

**技术洞察：**

```text
backend 选择 = f(arch, mode, pool, parallel, spec, deployment)

MHA    → FlashInfer / Triton (fused, paged)
MLA    → FlashInferMLA / TRT-LLM-MLA / TokenSpeed-MLA
DSA    → 专用 DSA backend (indexer metadata)
NSA    → 专用 NSA backend (sparse page table)
Mamba  → Mamba backend (recurrent state, no QKV)
Native → PyTorch SDPA (fallback, correct but slow)

PDMUX   → 1 main + N decode backends
overlap → TboAttnBackend wrapper
```

C：`sglang/01-modules/M09-attention-cuda-graph/README.md` 的 registry 逻辑、FlashInfer MLA 分支、HybridAttnBackend 角色、TorchNativeAttnBackend fallback。I：多维决策的工程取舍。U：各 backend 的性能对比未在本地测量。

**图文说明：** 决策树形式：根节点是 `--attention-backend` 名称。第一层分支按架构（MHA/MLA/DSA/Mamba）。MHA 层按部署模式分支（普通/PDMUX/overlap）。MLA 层按平台分支（FlashInfer/TRT-LLM/TokenSpeed）。每叶节点标注适用场景和限制。

**拓展知识：** 与 I07 衔接：attention metadata 刻意拆成 graph 外（动态 shape、CPU 操作）和 graph 内（可录制的静态设备操作），因为 CUDA Graph 要求地址和 shape 稳定 `sglang/01-modules/M09-attention-cuda-graph/README.md`。

**常见追问及回答要点：**

1. **问：为什么 `trtllm_mla` 在 DCP+speculative 下拒绝？** 答：该实现不能提供跨 rank merge 所需的 metadata，强行使用会导致验证阶段信息不完整。
2. **问：`HybridAttnBackend` 和直接使用两个 backend 有什么区别？** 答：`HybridAttnBackend` 提供统一的 `needs_cpu_seq_lens`、ragged verify 和 CUDA graph state 能力汇总，简化上游调用。
3. **问：什么时候必须回退到 `TorchNativeAttnBackend`？** 答：当请求使用 sliding-window attention 但 pool 不支持 fused 路径，或 fused backend 不支持当前形状/配置组合时 `sglang/01-modules/M09-attention-cuda-graph/README.md`。
4. **问：attention metadata 为什么要拆成 graph 外和 graph 内？** 答：`.cpu()`、`.item()` 和动态 shape 创建等 host/dynamic 操作不能安全录进 CUDA Graph；只有纯 device 操作才能捕获。因此 `init_forward_metadata_out_graph` 做动态准备，`init_forward_metadata_in_graph` 做可录制的静态 device 操作 `sglang/01-modules/M09-attention-cuda-graph/README.md`。

**易错点与反例：**

- 反例："backend 名称就是 kernel 名称"——错误。`flashinfer` 可能实例化多个 wrapper 对象。
- 反例："MLA 模型和 MHA 用同一个 backend"——错误。MLA 有独立的 backend 实现。
- 易错：忘记记录 TP/CP/DCP 维度——同一模型不同并行配置可能需要不同的 backend 变体。

**实践与验证：** [U] 在 A100/H100 上分别运行 MHA/MLA 模型，用 `nsys profiler` 比较不同 backend 的 kernel 时间和显存占用。也可以强制指定 `--attention-backend torch_native` 观察 fallback 路径是否正确工作以及性能损失幅度。实际对比和 fallback 测试未执行。

**分层评分标准：** 2分：能列举常见 backend 名称和基本适用场景；4分：能解释七维决策因素和 registry 实例化逻辑、`TorchNativeAttnBackend` 的 fallback 定位；6分：能分析 PDMUX/overlap 拓扑、Hybrid 适配器角色、DCP+speculative 拒绝条件、`SharedReadEnds` metadata 生命周期合约。

**证据与来源：** `sglang/01-modules/M09-attention-cuda-graph/README.md`（registry、FlashInfer MLA、Hybrid/TorchNative、PDMUX/overlap）；`sglang/05-sglang-inference-technical-insights.md`（backend 名称不是完整语义）。

### I-I009 extend(prefill) 与 decode 路径在 ForwardBatch 中如何区分？各自的 tensor shape 有何特点？

**元数据：** 知识域：`I06 prefill-decode 与 ForwardBatch`；能力目标：`C09 ForwardBatch 双路径差异`；题型：对比分析；频率：中频；前置/后续：`I-B001`（算术强度差异）、`I-I006`（调度）；证据状态：源码 C，GPU 运行 U。

**背景知识：** `ForwardBatch.init_new` 从 `ScheduleBatch` 复制元数据并明确不修改输入 `sglang/01-modules/M05-model-execution/README.md`。对于 extend，`extend_lens`、`prefix_lens` 和 logprob 起点被放入 forward metadata；对于 decode/idle，这些 extend-only 字段置为 `None` `sglang/01-modules/M05-model-execution/README.md`。extend 的 `input_ids` 是 flatten 后的未缓存 token（长度可变），shape 为 `[total_extend_tokens]`；decode 的 `input_ids` 每请求一个 token，shape 为 `[batch_size]` `sglang/02-request-flow/03-调度器与连续批处理.md`。`seq_lens` 在 extend 中是各请求的当前总长度，decode 中是 `prev_seq_lens + 1` `sglang/01-modules/M05-model-execution/README.md`。`out_cache_loc` 在 extend 中指向一段连续新分配的位置，decode 中每请求一个位置。`positions` 在 extend 用 `seq_lens - 1` 的最后位置，decode 用 `forward_batch.positions`。

**问题分析：** 1) `ForwardBatch.init_new` 如何根据 forward mode 填充不同字段；2) extend 的 tensor shape 特点——变长、flatten、需 `extend_lens`/`prefix_lens` 划分边界；3) decode 的 tensor shape 特点——固定 batch_size、每请求一个 token；4) `out_cache_loc` 在两路径中的差异；5) attention backend 如何依赖这些字段区分行为。

**具体答案：** 面试中可以这样回答：

> `ForwardBatch.init_new` 通过 `forward_mode.is_extend()` 区分两条路径 `sglang/01-modules/M05-model-execution/README.md`。extend 路径的特点：`input_ids` 是所有请求的未缓存 token flatten 后的一维 tensor，shape `[total_extend_tokens]`，需配合 `extend_lens`（每请求的扩展长度）和 `prefix_lens`（每请求的已缓存前缀长度）划分各请求边界；`out_cache_loc` 是为所有新 token 分配的一段 KV 写入位置，shape 与 `input_ids` 相同；`positions` 使用 `seq_lens - 1` 取每条序列的最后位置 `sglang/01-modules/M05-model-execution/README.md`。decode 路径的特点：`input_ids` 每请求一个最近生成的 token，shape `[batch_size]`；`extend_lens`、`prefix_lens` 等字段置为 `None`；`out_cache_loc` 每请求分配一个新 KV 位置；`positions` 使用 `forward_batch.positions`。两条路径共享 `req_pool_indices`（指向 request-to-token pool 的行）、`seq_lens`（当前 forward 使用的序列长度）和 `rids`（输出关联）。attention backend 依赖 `extend_lens`/`prefix_lens` 知道每个 query 应读取哪个请求的历史 KV `sglang/02-request-flow/03-调度器与连续批处理.md`。

**技术洞察：**

```text
ForwardBatch.extend:
  input_ids:      [t1, t2, t3, t4, t5]  shape=[total_extend]
  extend_lens:    [3, 2]               ← A 扩展 3, B 扩展 2
  prefix_lens:    [4, 2]               ← A 缓存 4, B 缓存 2
  out_cache_loc:  [c1, c2, c3, c4, c5]
  positions:      [seq_lens - 1]       ← 最后位置

ForwardBatch.decode:
  input_ids:      [a_new, b_new]       shape=[batch_size]
  extend_lens:    None
  prefix_lens:    None
  out_cache_loc:  [ca, cb]            ← 每请求 1 个
  positions:      forward_batch.positions
```

C：`sglang/01-modules/M05-model-execution/README.md` 的 `init_new` 行为、`extend_lens`/`prefix_lens` 设置。I：backend 依赖这些边界的工程含义。U：实测 tensor shape 因模型配置而异。

**图文说明：** 两个并排的 batch 示意图。extend batch 中有两个请求 A/B，A 有 4 个已缓存的 KV（灰色）加 3 个新 token（蓝色），B 有 2 个已缓存加 2 个新 token。`input_ids` 展平为 5 个蓝色 token。decode batch 中 A/B 各只有 1 个新 token（橙色），`input_ids` 长度为 2。

**拓展知识：** 与 I07 衔接：`ForwardBatch.init_new` 的 GPU tensor 创建可能使用 pinned CPU memory 和 non-blocking copy，调试时不能假定所有复制都同步完成 `sglang/01-modules/M05-model-execution/README.md`。

**常见追问及回答要点：**

1. **问：为什么 extend 需要 prefix_lens 而 decode 不需要？** 答：extend 需要知道每个 query 前面有多少已缓存的 KV 可以复用；decode 的 KV 全部已知，只需追加一个新位置。
2. **问：mixed chunk 时 extend 和 decode 的 tensor 如何共存？** 答：`mix_with_running` 把 decode 请求的行拼接到 extend batch 后，`input_ids` 是 extend 部分和 decode 部分的拼接，`decoding_reqs` 标记哪些请求是 decode 模式 `sglang/02-request-flow/03-调度器与连续批处理.md`。
3. **问：`ForwardBatch` 为什么不直接复用 `ScheduleBatch`？** 答：overlap 模式下 scheduler 需要持续更新 `ScheduleBatch`，而 `ForwardBatch` 是本次 forward 的快照；复用会导致跨 stream 生命周期问题 `sglang/01-modules/M05-model-execution/README.md`。
4. **问：`ForwardBatch.init_new` 的 GPU tensor 创建有什么注意事项？** 答：可能使用 pinned CPU memory 和 non-blocking copy，调试时不能假定所有复制都同步完成——需要显式同步或等待 event `sglang/01-modules/M05-model-execution/README.md`。

**易错点与反例：**

- 反例："extend 和 decode 的 input_ids shape 相同"——错误。extend 是 flatten 变长，decode 是 `[batch_size]`。
- 反例："ForwardBatch 会修改 ScheduleBatch"——错误。`init_new` 明确不修改输入。
- 易错：忘记 `positions` 在 extend 用 `seq_lens - 1` 而非 `forward_batch.positions`。

**实践与验证：** [U] 在 `init_new` 后打印各字段的 shape 和 forward mode，验证 extend/decode 差异。实际 GPU 运行未执行。

**分层评分标准：** 2分：能区分 extend 和 decode 的基本差异；4能分析 tensor shape 特点和 `extend_lens`/`prefix_lens` 的边界作用；6分：能解释 mixed chunk 共存、`ForwardBatch` 作为快照的原因、与 overlap 的交互。

**证据与来源：** `sglang/01-modules/M05-model-execution/README.md`（`init_new`、extend/decode 字段差异）；`sglang/02-request-flow/03-调度器与连续批处理.md`（prepare_for_extend/decode、mixed chunk）。

### I-I010 CUDA Graph 捕获对推理性能有什么收益？什么情况下必须回退到 eager 模式？

**元数据：** 知识域：`I07 Attention 与 CUDA Graph`；能力目标：`C10 CUDA Graph 收益与回退`；题型：机制解释与场景分析；频率：高频；前置/后续：`I-I008`（backend 选择）、`I-I009`（ForwardBatch）；证据状态：源码 C，GPU 运行 U。

**背景知识：** CUDA Graph 把一段形状稳定的 GPU 操作预先录制，后续用固定 buffer 重放，消除 kernel launch 开销和 Python dispatch 成本 `sglang/01-modules/M09-attention-cuda-graph/README.md`。`ModelRunner._forward_raw` 判断 forward mode、runner 存在以及 `can_run_graph(forward_batch)`——三者都满足时 replay，否则回退 eager `sglang/01-modules/M09-attention-cuda-graph/README.md`。`DecodeCudaGraphRunner.capture` 先 warmup/autotune，按大 batch 到小 batch 逆序捕获各 bucket，每个 bucket 按 LoRA variant 和 DSA dense/sparse variant 各捕获一份 `sglang/01-modules/M09-attention-cuda-graph/README.md`。必须回退 eager 的情况：动态 token embedding override、ragged verify 的 candidate width 不匹配、batch size 超过 capture 范围、DP/MLP sync 条件不满足、超过 padding waste 阈值、speculative num_tokens_per_req 与 capture width 不同 `sglang/01-modules/M09-attention-cuda-graph/README.md`。

**问题分析：** 1) CUDA Graph 消除的开销类型；2) decode graph vs prefill graph 的独立捕获；3) `can_run_graph` 的多维检查条件；4) capture 的 bucket 策略和逆序捕获的原因；5) `can_run_graph=True` 不等于结果正确——仍需经过 logits/sample。

**具体答案：** 面试中可以这样回答：

> CUDA Graph 的核心收益是消除 kernel launch 开销——decode 阶段每步只处理少量 token，launch 成本占比大；录制后重放只需一次 `graph.launch()` 替代多次 `kernel.launch()`，同时消除 Python dispatch 和 CUDA driver 的 per-call 成本 `sglang/01-modules/M09-attention-cuda-graph/README.md`。decode graph 和 prefill graph 有独立的 bucket 和生命周期；decode runner 的 `capture` 按大 batch 到小 batch 逆序捕获，让较小 graph 复用已建立的 CUDA memory pool `sglang/01-modules/M09-attention-cuda-graph/README.md`。`can_run_graph` 是运行时协商而非总开关，拒绝条件包括：(1) 请求有 `replace_embeds`（动态 token embedding override）；(2) speculative 的 `num_tokens_per_req` 与 capture width 不匹配；(3) batch size 超出 capture bucket 范围；(4) DP/MLP sync 或 ragged verify 的 shape 约束不满足；(5) prefill 的 padding waste 超过阈值 `sglang/01-modules/M09-attention-cuda-graph/README.md`。`can_run_graph=True` 只表示 runner 选择 graph 执行，结果仍要经过 logits/sample 和 M04 result processor。

**技术洞察：**

```text
收益:
  消除 kernel launch overhead (decode 尤甚)
  消除 Python dispatch / driver per-call cost
  固定 buffer 地址 → 更优的 GPU 调度

回退条件:
  replace_embeds → can_run_graph=false
  speculative width mismatch → false
  batch_size > capture_range → false
  DP/MLP sync 不满足 → false
  padding waste > threshold → false
  sliding-window special layout → false

capture 策略:
  大 batch → 小 batch (逆序)
  每个 bucket × LoRA variant × DSA variant
  warmup/autotune → capture stream → store by shape/variant key
```

C：`sglang/01-modules/M09-attention-cuda-graph/README.md` 的 `_forward_raw` graph 判断、`can_run_graph` 拒绝条件、capture 逆序策略。I：逆序捕获复用 memory pool 的工程动机。U：各拒绝条件的实际触发频率未采样。

**图文说明：** `_forward_raw` 的决策树：先判断 ForwardMode 是否支持 graph → 再判断 runner 是否存在 → 再判断 `can_run_graph()` → 全部满足走 graph replay，任一不满足走 eager。右侧列出五种回退场景及其对应的条件。

**拓展知识：** 与 I08 衔接：attention metadata 拆成 graph 外和 graph 内，因为 `.cpu()`、`.item()` 和动态 shape 创建不能安全录进 CUDA Graph `sglang/01-modules/M09-attention-cuda-graph/README.md`。

**常见追问及回答要点：**

1. **问：为什么按大 batch 到小 batch 逆序捕获？** 答：大 batch 捕获时建立 CUDA memory pool 中的 allocation，小 batch 的 graph 重放时复用这些地址；如果先捕获小 batch，大 batch 可能导致地址失效。
2. **问：prefill graph 和 decode graph 为什么分开？** 答：两者 shape 契约不同——prefill 处理变长 token，decode 每请求一个 token；分开设 bucket 可以各自优化。
3. **问：CUDA Graph 是否影响模型正确性？** 答：不影响。Graph 只是固定了计算路径的 shape/地址，数值结果与 eager 相同；前提是输入写入正确的固定地址。
4. **问：prefill graph 的 `load_batch` 和 `capture_prepare` 有什么区别？** 答：`capture_prepare` 在捕获时把 synthetic requests 写入固定 buffer registry 建立静态地址绑定；`load_batch` 在 serving 时把 live batch 写入同一组固定地址并将 MIXED replay 映射到 EXTEND graph `sglang/01-modules/M09-attention-cuda-graph/README.md`。

**易错点与反例：**

- 反例："开启 CUDA Graph 就一定用 graph"——错误。`can_run_graph` 是运行时协商。
- 反例："can_run_graph=True 等于结果正确"——错误。只表示执行方式选择，结果仍需后续处理。
- 易错：忘记 `load_batch` 在 serving 时把 live batch 写入固定地址——如果地址写错，replay 会读到错误数据。

**实践与验证：** [U] 用 nsys profiler 比较 eager 和 graph 模式下的 kernel launch 密度和 decode 延迟。实际 GPU profiling 未执行。

**分层评分标准：** 2分：能描述 CUDA Graph 消除 launch 开销的收益；4分：能分析 `can_run_graph` 的多种回退条件和 capture 策略；6分：能解释 graph 内/外 metadata 拆分、逆序捕获的内存复用、prefill/decode graph 分离的原因。

**证据与来源：** `sglang/01-modules/M09-attention-cuda-graph/README.md`（`_forward_raw`、`can_run_graph`、capture 逆序策略、SharedReadEnds metadata 拆分）；`sglang/05-sglang-inference-technical-insights.md`（graph eligibility 是运行时协商）。

### I-I011 PagedAttention 的页表管理如何实现逻辑连续到物理映射？缺页与淘汰如何操作？

**元数据：** 知识域：`I08 KV Cache`；能力目标：`C11 页表映射与淘汰`；题型：机制解释；频率：高频；前置/后续：`I-I007`（PrefillAdder）、`I-I012`（radix cache）；证据状态：源码 C，GPU 运行 U。

**背景知识：** PagedAttention 借鉴 OS 虚拟内存分页 `insights/arxiv-pdf/2309.06180-pagedattention-vllm.md`：请求的 KV cache 被划分为固定大小的 block/page，通过 block table 记录逻辑 block 到物理 block 的映射。SGLang 的 `PagedTokenToKVPoolAllocator` 把 `size` 解释为 token 总数，`num_pages = size // page_size` 作为真正的页数；`alloc(n)` 要求 page-aligned，把页 id 展开为连续 token indices `sglang/01-modules/M08-kv-cache/README.md`。`alloc_extend()`/`alloc_decode()` 使用 page-aware kernel 根据 prefix、sequence length 和 last location 生成写入位置 `sglang/01-modules/M08-kv-cache/README.md`。`free()` 必须把 token indices 去重到 page id；`free_segment()` 利用起点 page-aligned 的契约只取 `free_index[::page_size]`。淘汰通过 `RadixCache.evict` 实现——从 `evictable_leaves` 选叶子，按 node 的 page-exact value 调用 allocator 的 `free_segment`，删除叶子并在父节点变空时重新入堆 `sglang/01-modules/M08-kv-cache/README.md`。

**问题分析：** 1) block table 类比 OS 页表的逻辑-物理映射；2) `PagedTokenToKVPoolAllocator` 的 alloc/free 协议；3) `alloc_extend` vs `alloc_decode` 的区别；4) partial page 和 page-aligned 的约束；5) eviction 与 allocator 的配合。

**具体答案：** 面试中可以这样回答：

> PagedAttention 的核心抽象是逻辑 KV blocks → block table → physical KV blocks，类比 OS 的虚拟页→页表→物理页框 `insights/arxiv-pdf/2309.06180-pagedattention-vllm.md`。在 SGLang 中，`PagedTokenToKVPoolAllocator` 把 `size`（token 总数）除以 `page_size` 得到页数，维护 free pages 列表；`alloc(n)` 要求 page-aligned，把页 id 展开为 `page * page_size + offset` 的连续 token indices `sglang/01-modules/M08-kv-cache/README.md`。extend 和 decode 有各自的分配 kernel：`alloc_extend` 根据 prefix 命中、sequence length 和 last location 计算写入位置；`alloc_decode` 每请求分配一个新 page 的最后一个位置 `sglang/01-modules/M08-kv-cache/README.md`。释放时 `free()` 去重 token indices 到 page id，`free_segment()` 利用起点 page-aligned 契约避免 device-side `unique`。缺页处理由 RadixCache 的 eviction 实现——从 `evictable_leaves` 构造 priority heap，取出叶子后按 page-exact value 调用 allocator 的 `free_segment` 归还物理 slot，删除叶子并在父节点变空且未锁定时重新入堆 `sglang/01-modules/M08-kv-cache/README.md`。`cache_protected_len` 与 `prefix_indices` 可能不同——page_size > 1 时 partial page 属于请求工作集但尚未成为 tree 中的完整节点 `sglang/01-modules/M08-kv-cache/README.md`。

**技术洞察：**

```text
逻辑 → 物理映射:
  req.kv.req_pool_idx → ReqToTokenPool row → token indices → physical KV tensor slots
  PagedAllocator: page_id → page * page_size + offset

alloc 协议:
  alloc_extend: prefix-aware, 可能复用 partial page 尾部
  alloc_decode: 每请求 +1 token, 可能需要新 page
  free: unique(token_indices) → page_ids → free_pages

eviction:
  RadixCache.evictable_leaves → priority heap
  → free_segment(node.value) → delete leaf → promote parent
```

C：`sglang/01-modules/M08-kv-cache/README.md` 的 `PagedTokenToKVPoolAllocator` 协议、eviction 步骤、partial page 处理；`insights/arxiv-pdf/2309.06180-pagedattention-vllm.md` 的分页类比。I：partial page 延迟释放的必要性。U：实际缺页频率和淘汰开销未测量。

**图文说明：** 三层映射图：Request → ReqToTokenPool row（逻辑 block 列表）→ PagedAllocator（block table）→ Physical KV tensor（实际 GPU memory）。下方标注 eviction 路径：RadixCache → free_segment → allocator free_pages。

**拓展知识：** 与 I08 衔接：CUDA Graph 的 `DecodeInputBuffers` 要求固定 shape/address 与 KV slot view 在 replay 期间保持有效 `sglang/01-modules/M08-kv-cache/README.md`。

**常见追问及回答要点：**

1. **问：page_size 如何选择？** 答：大 page 减少 page table 开销但增加 tail waste；小 page 减少浪费但增大 table 和 kernel 索引成本。需要权衡。
2. **问：partial page 为什么不能释放？** 答：page_size > 1 时，一个 page 可能只有部分 token 属于某请求（其余属于 prefix 的其余部分或另一个请求的共享前缀），释放整个 page 会影响其他拥有者。
3. **问：eviction 的 num_tokens_evicted 为什么可能不等于请求数？** 答：因为 node 长度按 page 对齐，实际释放的 token 数可能大于请求的目标数 `sglang/01-modules/M08-kv-cache/README.md`。
4. **问：`free_group_begin/end` 批量释放协议的作用？** 答：把多个请求的释放延迟到批次边界，避免在同一调度更新中反复拼接 free tensor，减少临时分配和碎片；debug 模式还检查 double-free `sglang/01-modules/M08-kv-cache/README.md`。

**易错点与反例：**

- 反例："PagedAttention 需要物理连续"——错误。PagedAttention 的核心就是允许物理不连续。
- 反例："free_segment 释放任意区间"——错误。起点必须 page-aligned。
- 易错：混淆 `free`（token 粒度）和 `free_segment`（page-aligned segment）。

**实践与验证：** [U] 设置 page_size=16 运行长序列请求，观察 partial page 行为和 eviction 触发点。也可以用 `TokenToKVPoolAllocator`（page_size=1）与 `PagedTokenToKVPoolAllocator`（page_size=16）分别测量同一负载下的内存浪费率和 attention kernel 时间差异。实际 GPU 运行和浪费率测量未执行。

**分层评分标准：** 2分：能描述分页类比和 block table 映射；4分：能分析 alloc/free 协议和 eviction 步骤、page-aligned segment 的释放约束；6分：能解释 partial page 延迟释放、`cache_protected_len` 与 `prefix_indices` 的差异、CUDA Graph 固定地址与 KV slot view 的配合。

**证据与来源：** `sglang/01-modules/M08-kv-cache/README.md`（PagedAllocator、eviction、partial page）；`insights/arxiv-pdf/2309.06180-pagedattention-vllm.md`（分页类比、block table）；`sglang/90-cross-module/pooling-and-resource-management.md`（三层池模型）。

### I-I012 radix cache 如何复用相同 prefix 的 KV？命中失败的情况有哪些？

**元数据：** 知识域：`I08 KV Cache`；能力目标：`C12 radix cache 复用与失效`；题型：机制解释与场景分析；频率：高频；前置/后续：`I-I011`（PagedAttention）、`I-I006`（调度）；证据状态：源码 C，运行 U。

**背景知识：** `RadixCache` 维护 token 前缀到 KV indices 的映射树，让后续请求复用 `sglang/01-modules/M08-kv-cache/README.md`。`insert` 先将 key 转为适合 bigram/Eagle 的视图、按 page 对齐，截断 value 使其与 key 长度一致 `sglang/01-modules/M08-kv-cache/README.md`。`cache_finished_req` 把完成请求的 `origin_input_ids + output_ids` 插入树，释放重复部分和未对齐 tail；`cache_unfinished_req` 支持 chunked 标记，插入后重新 match prefix 并写回 request row `sglang/01-modules/M08-kv-cache/README.md`。命中失败的情况：(1) page alignment 导致未对齐尾部不能作为完整缓存页；(2) disable cache 时直接返回；(3) eviction 已删除的 node；(4) `cache_protected_len` 与 `prefix_indices` 的差异——partial page 不属于 tree 节点；(5) 未完成请求的 chunked 缓存尚未覆盖后续 token `sglang/01-modules/M08-kv-cache/README.md`。

**问题分析：** 1) Radix tree 的基本结构和 insert 流程；2) 命中时 `prefix_indices` 如何写回 request row；3) page alignment 对命中长度的影响；4) finished vs unfinished request 的缓存差异；5) 命中失败的多种场景。

**具体答案：** 面试中可以这样回答：

> RadixCache 把 token 前缀映射为树节点，node 持有 KV slot indices 引用 `sglang/01-modules/M08-kv-cache/README.md`。新请求到达时，`match_prefix_for_req` 用 `req.origin_input_ids + req.output_ids` 在 tree 上逐 token 匹配，命中部分写入 `req.prefix_indices`——这些 indices 直接复用已有的 KV，不需要重新计算 `sglang/02-request-flow/03-调度器与连续批处理.md`。完成请求的 KV 在 `cache_finished_req` 中插入 tree：组合 token ids → page-aligned RadixKey → insert → 释放重复/未对齐 tail → `dec_lock_ref(last_node)`。未完成请求在 `cache_unfinished_req` 中支持 chunked 标记 `sglang/01-modules/M08-kv-cache/README.md`。命中失败的情况有：(1) 尾部未对齐——page_size=16 时只有 10 token 的前缀不能作为完整 page 缓存，下次匹配只能命中 0 token；(2) disable cache——`insert` 直接返回，所有 token 都需要重新计算；(3) eviction 已删除——node 被 `evict()` 从 tree 移除并归还 allocator；(4) partial page——`cache_protected_len` > `len(prefix_indices)` 时，partial page 在请求工作集中但不在 tree 中；(5) chunked 未完成——只缓存了已处理 chunk 部分，后续 token 未缓存。

**技术洞察：**

```text
复用流程:
  match_prefix → req.prefix_indices = [已命中的 KV indices]
  → prepare_for_extend 只把未命中部分放入 input_ids
  → 命中部分直接读取已有 KV，不重算

命中失败:
  (1) page alignment: tail 未对齐不能缓存
  (2) cache disabled: insert 直接返回
  (3) evicted: node 已从 tree 删除
  (4) partial page: cache_protected_len > len(prefix_indices)
  (5) chunked incomplete: 只缓存到当前 chunk
```

C：`sglang/01-modules/M08-kv-cache/README.md` 的 insert、cache_finished_req、cache_unfinished_req、page alignment。I：partial page 的工程处理。U：不同 page_size 下的实际 hit rate 未测量。

**图文说明：** Radix tree 示意图：root → "system" → "prompt" → "A" / "B"。节点旁标注 KV indices 和 lock_ref。新请求 "system prompt C" 匹配到 "system prompt" 节点，命中部分标绿色，"C" 为未命中标红色。虚线框标注 partial page 区域（不属于任何节点）。

**拓展知识：** 与 I09 衔接：radix cache 的 prefix hit 直接影响 extend 的 token 数量，是 `PrefillAdder` 预算计算的关键输入 `sglang/02-request-flow/03-调度器与连续批处理.md`。

**常见追问及回答要点：**

1. **问：lock_ref 的作用？** 答：被请求或 host/storage 操作引用的 node 必须先解除 lock 才能被驱逐，避免正在读取的 KV 被淘汰 `sglang/01-modules/M08-kv-cache/README.md`。
2. **问：多个请求共享同一 prefix 时如何管理？** 答：Radix node 被多个请求的 `prefix_indices` 引用，每个引用通过 `inc_lock_ref` 增加计数，驱逐前检查 `lock_ref==0` `sglang/01-modules/M08-kv-cache/README.md`。
3. **问：`cache_unfinished_req` 和 `cache_finished_req` 有什么区别？** 答：前者支持 chunked 标记，只插入当前已完成的 fill ids；后者插入完整序列并释放重复部分和未对齐 tail `sglang/01-modules/M08-kv-cache/README.md`。
4. **问：eviction policy 有哪些选择？** 答：当前 checkout 提供 LRU（last_access_time）、LFU（hit_count 后按访问时间）、FIFO（creation_time）、MRU/FILO（对时间取负）、priority-aware（请求 priority 后按访问时间）、SLRU（按 hit threshold 分 probationary/protected 再按访问时间）`sglang/01-modules/M08-kv-cache/README.md`。

**易错点与反例：**

- 反例："radix cache 存的是 KV 数据副本"——错误。Radix node 存的是 slot indices 引用，不是另一份 KV。
- 反例："page alignment 不影响命中"——错误。未对齐尾部不能作为完整缓存页。
- 易错：混淆 `cache_protected_len` 和 `len(prefix_indices)`——前者可能包含 partial page。

**实践与验证：** [U] 构造相同 prefix 的多个请求，观察 `prefix_indices` 长度和首轮 extend 计算量。也可以 disable cache 做对照组，量化 prefix hit 对 TTFT 和首 token 计算量的影响。实际运行和对照实验未执行。

**分层评分标准：** 2分：能描述 radix tree 的基本复用机制和 prefix match 流程；4分：能分析 page alignment 约束、finished/unfinished 缓存差异、lock_ref 引用计数；6分：能完整解释五种命中失败场景、`cache_protected_len` 与 `prefix_indices` 的差异、与 allocator eviction 的配合、`evictable_leaves` 与 `protected_size_` 的状态转换。

**证据与来源：** `sglang/01-modules/M08-kv-cache/README.md`（insert、cache_finished_req、cache_unfinished_req、eviction、lock_ref）；`sglang/02-request-flow/03-调度器与连续批处理.md`（prefix match 流程）。

### I-I013 grammar 约束采样如何在不重采的情况下保证输出符合 CFG？原理是什么？

**元数据：** 知识域：`I09 采样与约束`；能力目标：`C13 grammar 约束原理`；题型：机制解释；频率：高频；前置/后续：`I-B001`（prefill/decode）、`I-I014`（停止条件）；证据状态：源码 C，GPU 运行 U。

**背景知识：** grammar 约束不是生成后修正文案，而是在 logits 上构建 vocab mask 屏蔽非法 token `sglang/01-modules/M10-sampling-constraints/README.md`。`update_regex_vocab_mask` 找到 batch 中的 grammar，申请 vocab mask，为仍需约束的行填充 mask，移到 device 包装成 `GrammarMask` `sglang/01-modules/M10-sampling-constraints/README.md`。`apply_logits_bias` 的顺序：pre-grammar transforms → grammar mask → post-grammar logit bias `sglang/01-modules/M10-sampling-constraints/README.md`。`ModelRunner._preprocess_logits` 先更新 regex vocab mask 再调用 `apply_logits_bias`，之后立即清空 `grammar_mask` GPU tensor 避免 overlap 延迟闭包持有 `sglang/01-modules/M10-sampling-constraints/README.md`。grammar backend（XGrammar/Outlines/LLGuidance）负责把 JSON schema/regex/structural tag 编译成 grammar object，通过 `allocate_vocab_mask`、`fill_vocab_mask_batched`、`move_vocab_mask` 提供 mask `sglang/01-modules/M10-sampling-constraints/README.md`。编译是异步的——`GrammarManager.process_req_with_grammar` 提交 future，PP0 周期性轮询 `Future.done()`，超时的进入 failed 集合，在 DP/TP 组内取 ready 交集、failed 并集 `sglang/01-modules/M10-sampling-constraints/README.md`。

**问题分析：** 1) vocab mask 如何屏蔽非法 token——不是重采样而是预防；2) mask 应用的顺序和生命周期；3) grammar backend 的编译缓存和异步等待机制；4) 编译失败的传播路径；5) 行对齐和 batch 过滤的约束。

**具体答案：** 面试中可以这样回答：

> grammar 约束的核心是在采样前把非法 token 的 logits 设为 `-inf`，这样 multinomial 采样时它们的概率自然为零——不需要重采 `sglang/01-modules/M10-sampling-constraints/README.md`。具体流程：grammar backend 把 JSON schema 编译成 grammar 对象，维护当前状态下每个 token 是否合法的 mask；`SamplingBatchInfo.update_regex_vocab_mask` 申请 vocab mask buffer，为 batch 中仍需约束的行调用 `fill_vocab_mask_batched` 填充，移到 device；`apply_logits_bias` 按 pre-transforms → mask → post-bias 的顺序把 mask 应用到 logits `sglang/01-modules/M10-sampling-constraints/README.md`。`ModelRunner._preprocess_logits` 完成后立即释放 grammar_mask GPU tensor，避免 overlap 的延迟闭包持有大 tensor 造成显存持续增长 `sglang/01-modules/M10-sampling-constraints/README.md`。grammar 编译是异步的——首次到达的请求提交编译 future 到 `grammar_queue`，PP0 轮询 `Future.done()`，超过 `SGLANG_GRAMMAR_MAX_POLL_ITERATIONS` 的请求进入 failed 集合，通过 PP group 向后续 stage 传播；DP/TP 组内取 ready 的交集和 failed 的并集，确保所有相关 rank 都准备好的 grammar 才能进入 waiting queue `sglang/01-modules/M10-sampling-constraints/README.md`。

**技术洞察：**

```text
原理: vocab mask → logits[illegal] = -inf → P(illegal) = 0
  (不是生成后过滤，而是预防)

流程:
  grammar backend compile → GrammarMask
  → update_regex_vocab_mask → fill per-row mask
  → apply_logits_bias: pre-transforms → mask → post-bias
  → sampler: masked logits → multinomial → always legal

异步编译:
  req arrives → submit future → grammar_queue
  PP0 poll done() → ready ∩ per-rank, failed ∪ per-rank
  timeout → InvalidGrammarObject → req.set_finish_with_abort
```

C：`sglang/01-modules/M10-sampling-constraints/README.md` 的 vocab mask 机制、`apply_logits_bias` 顺序、grammar 编译异步等待、失败传播。I：overlap 延迟闭包持有 tensor 的风险。U：实际编译耗时和命中率未测量。

**图文说明：** 流程图：grammar 对象 → vocab mask matrix [batch × vocab_size]，白色=允许，黑色=禁止。mask 与 logits 逐元素相乘后进入 sampler。右侧标注 "no resampling needed"。下方是异步编译的时间线：arrive → submit → poll → ready/failed。

**拓展知识：** 与 I09 衔接：grammar mask 和 sampling mask 是两个不同概念——前者描述语法允许集合，后者描述采样分布/观测结果 `sglang/01-modules/M10-sampling-constraints/README.md`。

**常见追问及回答要点：**

1. **问：grammar 约束会影响模型分布吗？** 答：理论上只屏蔽非法 token，不改变合法 token 的相对概率；但实际上可能使某些本应低概率的合法 token 相对概率上升。
2. **问：编译失败如何传播？** 答：future 异常包装为 `InvalidGrammarObject` 写入 backend cache；缓存命中的无效对象和本轮编译的无效对象都调用 `req.set_finish_with_abort`，把编译错误转为请求级失败 `sglang/01-modules/M10-sampling-constraints/README.md`。
3. **问：为什么 PP 需要同步 grammar ready 状态？** 答：TP/PP 所有 rank 必须在同一步使用相同 grammar，否则 speculative verify 或 collective 会读到不一致的 mask。
4. **问：grammar 的 NUL 字节输入防护是如何工作的？** 答：`BaseGrammarBackend` 在 dispatch 前递归检查 JSON/structural-tag 内容中的 NUL 字节并直接拒绝，这是对上游 regex converter 崩溃风险的输入防护；与普通 schema 语法错误一样最终表现为 `InvalidGrammarObject` `sglang/01-modules/M10-sampling-constraints/README.md`。

**易错点与反例：**

- 反例："grammar 是生成后检查并丢弃非法 token"——错误。是在 logits 上屏蔽，采样结果一定合法。
- 反例："grammar mask 在所有行都填充"——错误。finished、terminated 或没有 grammar 的行不填充。
- 易错：混淆 grammar mask（语法约束）和 sampling mask（观测结果）。

**实践与验证：** [U] 构造 JSON schema 请求，验证输出是否符合 schema 以及 grammar mask 的覆盖率。也可以故意提交非法 schema（如不合法的 regex 或自引用 JSON），观察 `InvalidGrammarObject` 的 abort 传播路径和错误信息。实际 GPU 运行和失败注入未执行。

**分层评分标准：** 2分：能描述 vocab mask 的基本原理和采样前屏蔽机制；4分：能分析 mask 应用顺序、grammar 编译异步等待、`InvalidGrammarObject` 失败传播；6分：能解释 overlap 下 grammar_mask tensor 的生命周期管理、DP/TP/PP ready 交集机制、NUL 字节输入防护 `sglang/01-modules/M10-sampling-constraints/README.md`。

**证据与来源：** `sglang/01-modules/M10-sampling-constraints/README.md`（vocab mask、apply_logits_bias、grammar 编译异步等待、InvalidGrammarObject）；`sglang/01-modules/M05-model-execution/README.md`（`_preprocess_logits`）。

### I-I014 停止条件(eos/max_len/stop_string)如何组合？优先级如何确定？

**元数据：** 知识域：`I09 采样与约束`；能力目标：`C14 停止条件组合与优先级`；题型：机制解释；频率：中频；前置/后续：`I-I013`（grammar 约束）、`I-I006`（调度）；证据状态：源码 C，运行 U。

**背景知识：** SGLang 的停止条件在 `SamplingParams` 中声明 `sglang/01-modules/M10-sampling-constraints/README.md`：`max_new_tokens`（最大生成长度）、`stop`（stop token ids）、`stop_strings`（停止字符串）、grammar 的 termination 状态、EOS token。sampler 只产生 token id，是否停止由 M04 result processing、`Req` 状态和 M03 detokenization/streaming 协作判断 `sglang/01-modules/M10-sampling-constraints/README.md`。`process_batch_result` 在 decode 路径检查：EOS token → finish_reason="stop"；达到 `max_new_tokens` → finish_reason="length"；`stop_strings` 在 detokenizer 层面通过增量文本匹配；grammar termination 在 grammar backend 中判断。新增停止条件需检查 `SamplingParams` normalize/verify、scheduler-side `Req` finish 判断、grammar rollback/terminate、`BatchStrOutput.finished_reasons`、M03 `ReqState` 删除 `sglang/01-modules/M10-sampling-constraints/README.md`。

**问题分析：** 1) 各停止条件的检测位置（sampler/scheduler/detokenizer）；2) EOS 和 stop_string 的检测差异——前者在 token id 层面，后者在文本层面；3) max_len 和 grammar termination 的组合；4) 多种条件同时满足时的优先级；5) 新增停止条件的完整改动链。

**具体答案：** 面试中可以这样回答：

> 停止条件的检测分布在三个层面。第一层是 scheduler/result processor：decode 后检查 EOS token id → `finished_reason="stop"`；检查 `len(output_ids) >= max_new_tokens` → `finished_reason="length"` `sglang/01-modules/M10-sampling-constraints/README.md`。第二层是 detokenizer：增量文本到达后与 `stop_strings` 列表逐条匹配，命中则在当前 offset 截断并标记 finish `sglang/01-modules/M10-sampling-constraints/README.md`。第三层是 grammar backend：grammar matcher 判断当前状态是否到达终止状态（不可再扩展）。优先级方面，EOS 和 max_len 在同一层面判断，通常先检查 EOS（因为 EOS 出现时即使未达 max_len 也应停止）；stop_string 在 detokenizer 层面，可能在 EOS 之后才检测到（因为 EOS token 可能对应空字符串或特殊字符）。grammar termination 独立判断，可能与 EOS 同时触发。多种条件同时满足时按"最保守"原则——即最早触发的那一个生效，避免继续生成无意义 token `sglang/01-modules/M10-sampling-constraints/README.md`。新增停止条件需要联动检查所有五个环节。

**技术洞察：**

```text:
停止条件检测位置:
  scheduler/result_processor:
    EOS token → "stop"
    max_new_tokens reached → "length"
  detokenizer:
    stop_string match → "stop"
  grammar backend:
    grammar terminated → "stop"

优先级（实际实现层面）:
  1. EOS (token-level, earliest)
  2. max_new_tokens (count-level)
  3. stop_string (text-level, may lag)
  4. grammar_terminate (parallel)

finish_reason 统一通过 BatchStrOutput 传给 ReqState
```

C：`sglang/01-modules/M10-sampling-constraints/README.md` 的停止条件检测分布、finish_reason 传递。I：优先级的工程含义。U：各条件触发的时序差异未采样。

**图文说明：** 三行分别标注 scheduler、detokenizer、grammar backend 的检测点和对应 finish_reason。箭头从各检测点汇聚到 `BatchStrOutput.finished_reasons[i]`，再指向 `ReqState.finished=True`。

**拓展知识：** 与 I02 衔接：`finished_reason` 通过 `BatchStrOutput` 从 scheduler 进程传递到 TokenizerManager 进程，写入 `ReqState` 后触发 `del rid_to_state[rid]` `sglang/01-modules/M03-tokenizer-request-state/README.md`。

**常见追问及回答要点：**

1. **问：stop_string 为什么在 detokenizer 而不是 scheduler 检测？** 答：因为 stop_string 是文本级别概念，需要增量 detokenization 后的字符串才能匹配；token 层面无法判断多 token 组合成的字符串。
2. **问：grammar termination 和 EOS 冲突怎么办？** 答：两者通常同时触发；如果 grammar 要求继续但 EOS 已出现，EOS 优先（因为 EOS 是硬停止信号）。
3. **问：max_new_tokens 包含 prompt token 吗？** 答：不包含。`max_new_tokens` 只限制生成 token 的数量。

**易错点与反例：**

- 反例："sampler 负责判断停止"——错误。sampler 只产生 token id，停止判断由 M04/M03/grammar 协作完成。
- 反例："stop_string 比 EOS 更快检测到"——不一定。stop_string 需要完整的文本匹配，可能在 EOS 之后。
- 易错：新增停止条件只改 `SamplingParams` 而忘记 scheduler 和 ReqState 的联动 `sglang/01-modules/M10-sampling-constraints/README.md`。

**实践与验证：** [U] 设置 `max_new_tokens=10` 和 `stop_strings=["\n\n"]`，观察哪个条件先触发。实际运行未执行。

**分层评分标准：** 2分：能列举常见停止条件；4分：能分析各条件的检测位置和优先级；6分：能解释新增停止条件的完整联动链和 finish_reason 跨进程传递。

**证据与来源：** `sglang/01-modules/M10-sampling-constraints/README.md`（停止条件检测分布、finish_reason、新增联动检查清单）；`sglang/01-modules/M03-tokenizer-request-state/README.md`（ReqState 完成路径）。

### I-I015 TP 推理中每个 forward 步骤发生几次集合通信？通信量与序列长度有何关系？

**元数据：** 知识域：`I10 分布式并行`；能力目标：`C15 TP 集合通信分析`；题型：量化分析；频率：中频；前置/后续：`I-I016`（TP vs PP）、`I-B001`（prefill/decode）；证据状态：源码 C，多卡运行 U。

**背景知识：** TP 中每个 forward 步骤的集合通信取决于模型结构和 parallel linear 的消费模式 `sglang/01-modules/M07-分布式并行.md`。典型 dense Transformer：column-parallel linear 在输入分片后直接计算（无通信）；row-parallel linear 需要 TP all-reduce 聚合结果 `sglang/01-modules/M07-分布式并行.md`。attention TP 的消费：`AttentionInputs.tp_all_gather_hidden_states` 在需要时通过 `get_tp_group().all_gather_into_tensor` 聚合 hidden states，但通过 `is_pre_gathered` 避免 DSA/MHC 已预聚合的输入重复 gather `sglang/01-modules/M07-分布式并行.md`。DP attention 路径还可能包含 attention-TP reduce-scatter 和 WORLD/TP all-gather `sglang/01-modules/M07-分布式并行.md`。logits processor 在特定 DP/TP 布局下用 TP `all_to_all_single` 交换 vocab-parallel logits `sglang/01-modules/M07-分布式并行.md`。通信量与 hidden dimension（而非序列长度）直接相关——attention gather 的通信量 ≈ `batch_size × seq_len × hidden_dim`；all-reduce 通信量 ≈ `2 × tensor_size`（ring all-reduce）`sglang/01-modules/M07-分布式并行.md`。

**问题分析：** 1) TP 中 column-parallel / row-parallel 的通信差异；2) attention TP 的 all-gather 和 reduce-scatter；3) logits 的 all-to-all 交换；4) 通信量与 hidden_dim 的关系而非 seq_len；5) prefill 和 decode 阶段通信模式的差异。

**具体答案：** 面试中可以这样回答：

> 典型 TP forward 中的通信点有以下几处：(1) attention TP hidden-state gather——`tp_all_gather_hidden_states` 用 all-gather 聚合 hidden states，通信量 ≈ `B × S × H`，但 `is_pre_gathered` 可跳过 DSA/MHC 的重复 gather `sglang/01-modules/M07-分布式并行.md`；(2) row-parallel linear 的 all-reduce——每层 FFN 和 attention output 的 row-parallel 部分需要 all-reduce，通信量 ≈ `2 × B × S × H`（ring all-reduce 的两倍 tensor size）；(3) DP attention 路径——可能额外包含 attention-TP reduce-scatter 和 WORLD/TP all-gather，通信量与 token buffer 布局相关 `sglang/01-modules/M07-分布式并行.md`；(4) logits processor 的 all-to-all——vocab-parallel 布局下交换 logits row block `sglang/01-modules/M07-分布式并行.md`。prefill 的通信量随 prompt 长度增长（`B × S × H` 中 S 是 prompt 长度），decode 的 S≈1 但 B（batch size）可能更大，通信量 ≈ `B × H`。关键点是通信量与 hidden_dim 和 batch 大小成正比，对序列长度不敏感（attention gather 除外）`sglang/01-modules/M07-分布式并行.md`。

**技术洞察：**

```text
TP forward 通信点:
  attention gather: all_gather(H)  ≈ B × S × H
  row-parallel AR: all_reduce(Out) ≈ 2 × B × S × H  (per row-parallel layer)
  DP attn: TP-RS + WORLD/TP-AG ≈ token buffer related
  logits: all_to_all(logits)     ≈ B × V / tp_size

prefill: S = prompt_len → 通信量随 prompt 增长
decode:  S ≈ 1, B 可能更大 → 通信量 ≈ B × H

总量 ≈ num_layers × 2 × B × S × H (row-parallel AR dominates)
```

C：`sglang/01-modules/M07-分布式并行.md` 的 attention TP gather、logits all-to-all、row-parallel all-reduce 消费点。I：通信量与 seq_len 的关系分析。U：各通信点的实际时间占比未在多卡上测量。

**图文说明：** Transformer 层示意图，标注各通信点的位置。左侧列通信类型（all-gather、all-reduce、all-to-all），右侧列公式和对应的 tensor shape。

**拓展知识：** 与 I07 衔接：TP 通信可用 NCCL、PyNCCL、custom all-reduce、symmetric memory 等多种 communicator 实现；关闭 custom all-reduce 只改变实现不改变拓扑 `sglang/01-modules/M07-分布式并行.md`。

**常见追问及回答要点：**

1. **问：`is_pre_gathered` 如何避免重复 gather？** 答：DSA/MHC 等 backend 在初始化时已将 hidden states 预聚合到所有 TP rank，forward 时跳过 gather 直接使用本地副本 `sglang/01-modules/M07-分布式并行.md`。
2. **问：all-to-all 和 all-reduce 哪个更贵？** 答：all-to-all 的通信量是 `O(V)`（vocab 大小），all-reduce 是 `O(H)`；通常 V >> H，但 all-to-all 只在 logits 层发生一次，all-reduce 每层都有。
3. **问：通信能否 overlap 计算？** 答：可通过 stream 并行实现——在 all-reduce 执行的同时计算下一层的 column-parallel 部分；但需要仔细管理依赖。
4. **问：`init_torch_distributed` 的通信预热有什么作用？** 答：在 parallel groups 初始化后，如果 TP>1 会用一次 TP all-reduce 预热 NCCL/RCCL/HCCL，减少第一次请求的冷启动延迟；同时记录模型加载前的可用显存作为后续 KV cache 内存预算的基线 `sglang/01-modules/M07-分布式并行.md`。

**易错点与反例：**

- 反例："TP 每个 forward 只发生一次 all-reduce"——错误。每层 row-parallel 部分都需要。
- 反例："通信量与序列长度成正比"——不完全正确。attention gather 是，但 all-reduce 也是。
- 易错：忘记 DP attention 的额外通信——某些路径比纯 TP 更复杂。

**实践与验证：** [U] 用 NCCL profiling 工具（`NCCL_DEBUG=INFO` 或 nsys）统计各 collective 的通信量和时间占比。也可以在不同 TP 宽度（1/2/4/8）下测量 decode 吞吐，观察通信量增长与吞吐收益的拐点。实际多卡 profiling 未执行。

**分层评分标准：** 2分：能描述 TP 中 all-reduce 的基本存在和每层发生的事实；4分：能列出所有通信点（attention gather、row-parallel AR、logits all-to-all、DP attention）并分析 prefill/decode 差异；6分：能精确分析通信量与 batch×hidden_dim 的关系、`is_pre_gathered` 优化条件、attention-TP reduce-scatter 与 WORLD/TP all-gather 的组合逻辑。

**证据与来源：** `sglang/01-modules/M07-分布式并行.md`（attention TP gather、logits all-to-all、DP attention 路径）；`sglang/01-modules/M20-parallel-strategies/README.md`（collective 实现选择）。

### I-I016 什么场景用 TP、什么用 PP？各自的显存与通信权衡是什么？

**元数据：** 知识域：`I10 分布式并行`；能力目标：`C16 TP vs PP 权衡`；题型：对比分析；频率：高频；前置/后续：`I-I015`（TP 通信）、`I-B001`（prefill/decode）；证据状态：源码 C，多卡运行 U。

**背景知识：** TP 沿 tensor 维度拆分——column-parallel 保留输出分片，row-parallel 需要 all-reduce；通信每层发生，量与 hidden_dim 相关 `sglang/01-modules/M07-分布式并行.md`。PP 按 Transformer layer range 拆分——每个 stage 只实例化当前层的权重，中间激活通过 P2P proxy tensors 传递；通信只在 stage 边界发生，量与 intermediate activation 大小相关 `sglang/01-modules/M07-分布式并行.md`。TP 适合单节点内（NVLink/NVSwitch 高带宽），通信频繁但单次量小；PP 可跨节点扩展（标准 Ethernet/RoPE），通信次数少但单次量大且有 pipeline bubble `sglang/01-modules/M20-parallel-strategies/README.md`。显存方面：TP 每个 rank 持有完整模型的部分权重（1/tp_size），但需要完整 KV cache；PP 每个 rank 只持有部分层的权重（1/pp_size），KV cache 也按 stage 分配 `sglang/01-modules/M07-分布式并行.md`。world_size = tp_size × pp_size `sglang/01-modules/M07-分布式并行.md`。

**问题分析：** 1) TP 的显存模型和通信模式；2) PP 的显存模型和 pipeline bubble；3) 各自的适用场景（单节点 vs 跨节点）；4) TP+PP 组合的权衡；5) world_size 约束和 group 拓扑。

**具体答案：** 面试中可以这样回答：

> TP 的核心优势是通信带宽高（NVLink/NVSwitch），适合单节点部署。每个 rank 持有 1/tp_size 的模型权重，但需要完整的 KV cache；通信每层发生（all-reduce / all-gather），单次通信量小（≈ hidden_dim）但频率高。PP 的核心优势是可跨节点扩展、显存节省大。每个 rank 只持有 1/pp_size 的层权重和对应的 KV cache；通信只在 stage 边界通过 P2P proxy tensors 传递（intermediate activation），次数少但单次量大（batch × seq_len × hidden_dim），且有 pipeline bubble（第一个 batch 需要填满所有 stage）`sglang/01-modules/M07-分布式并行.md`。选择原则：(1) 模型能放入单节点 NVLink 域内 → 优先 TP，通信快；(2) 模型超大需跨节点 → PP 或 TP+PP 组合；(3) 延迟敏感（decode 单 token）→ TP 更优（PP 的 bubble 和 stage 间延迟会放大 TPOT）；(4) 吞吐优先 → PP 可隐藏部分通信延迟。`world_size = tp_size × pp_size` 约束了两者的组合 `sglang/01-modules/M07-分布式并行.md`。

**技术洞察：**

```text
TP:
  权重: 1/tp_size per rank
  KV cache: 完整（每个 rank 需要全部）
  通信: 每层 all-reduce / all-gather
  通信量: ≈ hidden_dim per layer
  适合: 单节点 NVLink, 延迟敏感

PP:
  权重: 1/pp_size per rank (layer range)
  KV cache: 按 stage 分配
  通信: stage 边界 P2P proxy
  通信量: batch × seq × hidden_dim per boundary
  适合: 跨节点, 显存节省大

组合: world_size = tp × pp
  先建 TP 组（连续 rank），再建 PP 组（跨区间步长）
```

C：`sglang/01-modules/M07-分布式并行.md` 的 TP/PP 定义、建组顺序、world_size 公式、端到端 4GPU 示例；`sglang/01-modules/M20-parallel-strategies/README.md` 的 world_size 约束。I：选择原则的工程取舍。U：跨节点 PP 的实际通信延迟未测量。

**图文说明：** 两个并排的模型示意图。TP：同一层的权重切分到 2 个 GPU，KV cache 完整保留在每个 GPU。PP：前半层在 GPU0-1，后半层在 GPU2-3，中间激活通过箭头传递。

**拓展知识：** 与 I07 衔接：PP 的 layer ownership 由 `get_pp_indices` 计算，模型构造时 `make_layers` 只实例化当前 stage 的 layer range `sglang/01-modules/M07-分布式并行.md`。

**常见追问及回答要点：**

1. **问：PP 的 pipeline bubble 如何量化？** 答：bubble ratio ≈ `(pp_size - 1) / num_microbatches`；microbatches 越多 bubble 越小，但调度复杂度增加。
2. **问：为什么 TP 需要完整 KV cache？** 答：TP 是按 tensor 维度拆分，每个 rank 需要参与所有 token 的 attention 计算（通过 all-gather 聚合 hidden states），因此每个 rank 需要完整的 KV cache。
3. **问：TP+PP 的建组顺序是什么？** 答：先建 TP 组（连续 global rank 区间），再建 PP 组（跨区间步长序列）；DCP、attention CP/TP、MoE DP/EP/TP 在 TP 之后派生 `sglang/01-modules/M07-分布式并行.md`。

**易错点与反例：**

- 反例："TP 每个 rank 只需要 1/tp_size 的 KV cache"——错误。TP 需要完整 KV cache。
- 反例："PP 没有通信"——错误。PP 通过 P2P proxy tensors 传递中间激活。
- 易错：混淆 PP group 的构造方式——是跨区间步长 `[0,2,4,6]` 而非连续 `[0,1,2,3]` `sglang/01-modules/M07-分布式并行.md`。

**实践与验证：** [U] 在 8-GPU 节点上分别运行 TP=8、TP=4+PP=2、TP=2+PP=4，比较吞吐和延迟。实际多卡运行未执行。

**分层评分标准：** 2分：能描述 TP 和 PP 的基本定义；4分：能分析显存模型和通信模式的差异；6分：能给出场景选择原则、pipeline bubble 量化、TP+PP 组合的建组约束。

**证据与来源：** `sglang/01-modules/M07-分布式并行.md`（TP/PP 定义、world_size 公式、建组顺序、4GPU 示例）；`sglang/01-modules/M20-parallel-strategies/README.md`（world_size 约束、TP+PP rank 示例）。

### I-I017 speculative decoding 的 draft 模型如何选择？接受率受哪些因素影响？

**元数据：** 知识域：`I11 speculative decoding`；能力目标：`C17 draft 模型选择与接受率`；题型：机制解释与因素分析；频率：中频；前置/后续：`I-B001`（prefill/decode）、`I-I015`（分布式）；证据状态：源码 C，GPU 运行 U。

**背景知识：** SGLang 当前 checkout 包含 EAGLE/MTP、ngram、DFlash、DSpark、UNO、standalone/decoupled 等路径 `sglang/06-speculative-decoding-as-transaction.md`。draft 模型的选择标准：(1) 与 target 模型共享词表（确保 token 空间一致）；(2) 足够小/快以降低 draft 开销；(3) 分布与 target 相似以提高接受率。MTP（Multi-Token Prediction）使用 target 模型的额外 head 作为 draft，无需独立模型 `sglang/01-modules/M11-speculative-decoding/README.md`。ngram 不需要模型，基于语料匹配。接受率受以下因素影响：(1) draft 与 target 的分布相似度；(2) batch size（大 batch 提高 target 验证成本但可能降低单请求接受率）；(3) grammar 约束（限制合法 token 空间，提高接受率但可能降低候选质量）；(4) temperature（高温增加随机性降低接受率）；(5) candidate width k（更大的 k 提高吞吐潜力但降低全接受概率）`insights/06-speculative-decoding-as-transaction.md`。

**问题分析：** 1) draft 模型的类型（独立小模型 / MTP head / ngram）；2) MTP 不需要独立模型的优势；3) 接受率的定义和计算方式；4) 影响接受率的多维因素；5) 接受率与吞吐的关系。

**具体答案：** 面试中可以这样回答：

> draft 模型有三种主要选择 `sglang/01-modules/M11-specoding-decoding/README.md`：(1) 独立小模型——与 target 共享词表，足够快以降低 draft 开销；(2) MTP head——target 模型的额外预测 head，无需独立模型，天然共享表示；(3) ngram——基于语料匹配，不需要模型推理。接受率定义为 `accepted_tokens / candidate_width`，受多维因素影响 `insights/06-speculative-decoding-as-transaction.md`：(1) 分布相似度——draft 与 target 分布越接近，接受率越高；(2) batch size——大 batch 下 target 验证的单请求时间增加，但接受率本身不一定下降；(3) grammar 约束——限制合法 token 空间后 draft 候选更易被接受，但限制了多样性；(4) temperature——高温增加 target 的随机性，降低与 draft 的匹配概率；(5) candidate width k——k 从 4 增到 8 提高吞吐潜力，但 `P(all_accepted)` 下降。自适应策略如 `AdaptiveController` 根据 verify 结果动态调整 k `sglang/01-modules/M11-speculative-decoding/README.md`。

**技术洞察：**

```text
draft 类型:
  独立小模型: 共享词表, 轻量, 分布相似即可
  MTP head:   无需独立模型, 天然共享表示
  ngram:      语料匹配, 零推理成本

接受率因素:
  ↑ 分布相似度 → ↑ acceptance
  ↑ grammar 约束 → ↑ acceptance (但↓多样性)
  ↑ temperature  → ↓ acceptance
  ↑ k (width)    → ↓ P(all accepted) (但↑ throughput potential)
  batch size     → 影响验证延迟, 不直接决定 acceptance

吞吐 ≈ target_iterations × (1 + accepted_per_iteration)
```

C：`sglang/01-modules/M11-speculative-decoding/README.md` 的 draft 类型、commit 不变量；`insights/06-speculative-decoding-as-transaction.md` 的接受率因素。I：自适应 k 调整的工程含义。U：各因素对接受率的实际影响未在 GPU 上测量。

**图文说明：** draft-verify-commit 流程图。draft 阶段产生 k 个候选 token，target 验证后接受前缀（绿色）+ replacement（黄色），reject 后缀（红色）。右侧标注影响因素及其方向（↑/↓ acceptance）。

**拓展知识：** 与 I07 衔接：speculative verify 比普通 decode 更难 graph 化——每请求 candidate width 可能不同，accepted length 是运行后才知道的动态值 `insights/06-speculative-decoding-as-transaction.md`。

**常见追问及回答要点：**

1. **问：MTP head 相比独立 draft 模型有什么优势？** 答：无需加载额外模型权重，天然共享 target 的表示空间，接受率通常更高 `sglang/01-modules/M11-speculative-decoding/README.md`。
2. **问：grammar 约束如何影响 speculative decoding？** 答：grammar mask 使 draft 候选更可能合法，但 target 验证仍按概率接受；两者独立判断——draft 合法不代表 target 接受 `insights/06-speculative-decoding-as-transaction.md`。
3. **问：commit 时需要同步哪些状态？** 答：accepted token ids、target KV、draft KV/state、`seq_lens`、request row mapping、sampling/grammar state、finish/stop state `insights/06-speculative-decoding-as-transaction.md`。
4. **问：为什么 rejected suffix 不能只截短 Python list？** 答：因为 `Req.output_ids` 只是可见状态之一，还需回收 request-to-token row、target allocator slots/pages、draft allocator、graph/static metadata、prefix/radix protection——否则下一轮仍可能读取 rejected suffix 或 free page 持续下降 `insights/06-speculative-decoding-as-transaction.md`。

**易错点与反例：**

- 反例："draft 模型越大越好"——错误。draft 的目标是快且分布相似，不是大。
- 反例："acceptance rate 只取决于 draft 模型质量"——错误。temperature、grammar、batch size 都影响。
- 易错：忽略 commit 的多状态同步——只追加 output_ids 而不同步 KV/sampling state 会导致后续错误 `insights/06-speculative-decoding-as-transaction.md`。

**实践与验证：** [U] 固定模型和 k，改变 temperature（0.3/0.6/1.0）测量接受率变化。也可以固定 temperature 改变 k（2/4/8）观察吞吐和接受率的 trade-off。实际 GPU 运行和参数扫描未执行。

**分层评分标准：** 2分：能描述 draft-verify-commit 的基本流程；4分：能分析 draft 类型选择（独立模型/MTP/ngram）和接受率影响因素；6分：能解释 commit 的多状态同步不变量、grammar 交叉约束（draft 合法≠target 接受）、`AdaptiveController` 的动态 k 调整策略。

**证据与来源：** `sglang/01-modules/M11-speculative-decoding/README.md`（draft 类型、commit 边界、AdaptiveController）；`insights/06-speculative-decoding-as-transaction.md`（接受率因素、grammar 约束、commit 多状态同步）。

### I-I018 GPTQ/AWQ/GEMMINT 量化方案的误差特性与适用场景对比？

**元数据：** 知识域：`I12 量化`；能力目标：`C18 量化方案对比`；题型：对比分析；频率：中频；前置/后续：`I-B001`（算术强度）、`I-I012`（KV cache）；证据状态：源码 C，GPU 运行 U。

**背景知识：** SGLang 的量化契约由 `QuantizeMethodBase`、`LinearMethodBase`、`FusedMoEMethodBase` 和 `QuantizationConfig` 定义 `sglang/01-modules/M14-moe-quantization-lora/README.md`。GPTQ 使用 per-channel weight quantization + calibration dataset 优化，4-bit 典型精度；AWQ 通过激活感知的缩放因子保护重要权重通道，4-bit；GEMMINT（INT8/INT4）使用整数 GEMM kernel，低精度但高吞吐。量化误差特性：GPTQ 通过最小化量化前后的输出误差优化，适合通用推理；AWQ 保护激活幅度大的权重通道，适合混合精度场景；GEMMINT 误差较大但 kernel 效率最高 `sglang/01-modules/M14-moe-quantization-lora/README.md`。FP8 量化使用 E4M3/E5M2 格式，硬件原生支持（H100+），误差小且无需 calibration。

**问题分析：** 1) GPTQ 的 calibration 和误差优化方式；2) AWQ 的激活感知保护原理；3) GEMMINT 的整数 kernel 优势；4) FP8 的硬件原生支持；5) 各方案的适用场景权衡。

**具体答案：** 面试中可以这样回答：

> GPTQ 使用 per-channel weight quantization，通过 calibration dataset 优化量化参数以最小化输出误差；4-bit 下精度损失较小，适合通用推理场景，但需要校准过程 `sglang/01-modules/M14-moe-quantization-lora/README.md`。AWQ（Activation-aware Weight Quantization）通过分析激活分布，对大幅度激活对应的权重通道施加额外缩放因子保护，避免被量化噪声淹没；同样 4-bit 但精度通常优于 GPTQ，尤其适合混合精度部署 `sglang/01-modules/M14-moe-quantization-lora/README.md`。GEMMINT（INT8/INT4）使用纯整数 GEMM kernel，无需浮点转换，kernel 效率最高但精度损失较大；适合吞吐优先、精度可接受的场景。FP8（E4M3/E5M2）硬件原生支持（H100+），误差小且无需 calibration，是当前最推荐的推理量化方案。选择原则：精度优先 → GPTQ/AWQ 4-bit；吞吐优先 → GEMMINT 或 FP8；硬件限制 → 无 FP8 支持时用 4-bit 量化 `sglang/01-modules/M14-moe-quantization-lora/README.md`。

**技术洞察：**

```text
量化方案对比:
  GPTQ:      4-bit, calibration优化, 通用推理
  AWQ:       4-bit, 激活感知保护, 混合精度
  GEMMINT:   INT8/4, 整数kernel, 吞吐优先
  FP8:       E4M3/E5M2, 硬件原生, 精度+速度

适用场景:
  精度敏感 → GPTQ/AWQ 4-bit
  吞吐优先 → GEMMINT 或 FP8
  最新硬件 → FP8 (无需calibration)
  旧硬件   → GPTQ/AWQ 4-bit
```

C：`sglang/01-modules/M14-moe-quantization-lora/README.md` 的量化契约、算法替换。I：各方案误差特性的工程取舍。U：各方案在 SGLang 上的实际精度和吞吐未测量。

**图文说明：** 二维散点图：x 轴精度（高→低），y 轴吞吐（低→高）。GPTQ/AWQ 在左上（高精度中等吞吐），GEMMINT 在右下（低精度高通吐），FP8 在右上（高精度高通吐）。

**拓展知识：** 与 I12 衔接：KV cache 的 FP8/E4M3/E5M2 量化可进一步降低 decode 阶段的带宽压力，与权重 FP8 配合实现端到端 FP8 推理。

**常见追问及回答要点：**

1. **问：为什么 AWQ 通常比 GPTQ 精度高？** 答：AWQ 保护了大幅度激活对应的权重通道，这些通道对输出影响最大，量化噪声对它们的损害被显式抑制。
2. **问：FP8 为什么不需要 calibration？** 答：FP8 是浮点格式，动态范围足够大，直接 cast 即可；而 INT4/INT4 整数量化需要校准来确定 scale/zero-point。
3. **问：量化对 prefill 和 decode 的影响一样吗？** 答：不一样。prefill 是 compute-bound，权重量化降低计算和显存压力；decode 是 bandwidth-bound，权重量化和 KV 量化共同降低带宽需求 `sglang/01-modules/M14-moe-quantization-lora/README.md`。
4. **问：`QuantizationConfig` 的 `get_quant_method` 和 `process_weights_after_loading` 分别做什么？** 答：`get_quant_method` 决定每层用什么量化策略和参数 layout；`process_weights_after_loading` 在 checkpoint 加载完成后做 repack/workspace 等后处理——两步分离让量化策略独立于权重加载流程 `sglang/01-modules/M14-moe-quantization-lora/README.md`。

**易错点与反例：**

- 反例："GPTQ 和 AWQ 是同一种方法"——错误。AWQ 多了激活感知的保护机制。
- 反例："量化只影响权重"——错误。KV cache 也可以量化。
- 易错：忽略 FP8 的硬件依赖性——H100 之前的 GPU 不支持 FP8 GEMM。

**实践与验证：** [U] 在同一模型上分别用 GPTQ-4bit、AWQ-4bit、FP8 运行推理，比较 perplexity 和吞吐。也可以固定量化方案改变 batch size，观察 decode 吞吐变化以验证"量化对 bandwidth-bound 的 decode 更受益"。实际 GPU 运行未执行。

**分层评分标准：** 2分：能列举常见量化方案及其位宽；4分：能分析各方案的误差特性（calibration 需求、激活感知保护、kernel 效率）；6分：能解释 AWQ 激活感知原理、FP8 硬件依赖（H100+）、量化对 prefill/decode 的非对称影响（compute vs bandwidth）、KV cache 量化与权重量化的协同效应。

**证据与来源：** `sglang/01-modules/M14-moe-quantization-lora/README.md`（量化契约、算法替换、apply 流程）；`sglang/05-sglang-inference-technical-insights.md`（量化对两阶段的非对称影响）。

### I-I019 LoRA 适配器如何热加载到运行中的推理服务？不同 adapter 的 batching 如何隔离？

**元数据：** 知识域：`I14 LoRA`；能力目标：`C19 LoRA 热加载与 batching 隔离`；题型：机制解释；频率：中频；前置/后续：`I-I010`（CUDA Graph）、`I-I009`（ForwardBatch）；证据状态：源码 C，GPU 运行 U。

**背景知识：** LoRA 生命周期由 `LoRAManager` 管理 `sglang/01-modules/M14-moe-quantization-lora/README.md`：`load_lora_adapter` 加载 adapter 到 slot，`unload_lora_adapter` 释放或回收，`validate_lora_batch` 检查同一 batch 的 adapter 组合，`prepare_lora_batch` 生成执行元数据。HTTP 的 model 参数可携带 adapter 名称，M02 规定 model 中 adapter 优先于显式 `lora_path` `sglang/01-modules/M14-moe-quantization-lora/README.md`。batching 隔离：一个 batch 同时包含 base 和 adapter-A 请求时，manager 为 adapter-A 分配 slot，`ForwardBatch` 保存每 token 的 adapter id，backend 只对对应 token 加 delta；下一批 reset 不能泄漏 metadata `sglang/01-modules/M14-moe-quantization-lora/README.md`。LoRA backend 可选 Triton、Torch、Ascend、FlashInfer 等实现 `sglang/01-modules/M14-moe-quantization-lora/README.md`。

**问题分析：** 1) `LoRAManager` 的 load/unload/slot 管理；2) adapter 选择的 HTTP 入口——model 参数 vs lora_path；3) batching 隔离——ForwardBatch 中 adapter id 如何路由；4) CUDA Graph 与 LoRA 的交互；5) reset 时的 metadata 清理。

**具体答案：** 面试中可以这样回答：

> LoRA 热加载由 `LoRAManager` 管理。用户通过 HTTP `model` 参数（`base:adapter` 格式）或 `lora_path` 指定 adapter，model 中 adapter 优先 `sglang/01-modules/M14-moe-quantization-lora/README.md`。`LoRAManager.load_lora_adapter` 从路径加载 adapter 权重并分配 slot；`unload_lora_adapter` 释放或回收 slot；`validate_lora_batch` 检查同一 forward batch 的 adapter 组合是否合法。batching 隔离的核心机制：`ForwardBatch` 保存每 token 的 adapter id 数组，backend 根据 id 决定是否对对应 token 加 adapter delta——base 请求的 id 为空或 0，不做 delta；adapter-A 请求的 id 指向 slot 中的 adapter 权重 `sglang/01-modules/M14-moe-quantization-lora/README.md`。CUDA Graph 场景下，LoRA variant 是 graph key 的一部分——不同 adapter 组合可能触发不同的 capture bucket `sglang/01-modules/M14-moe-quantization-lora/README.md`。reset 时必须清理 adapter metadata，防止上一批的 adapter id 泄漏到下一批 `sglang/01-modules/M14-moe-quantization-lora/README.md`。

**技术洞察：**

```text
LoRA 热加载:
  HTTP model param → resolve adapter name
  → LoRAManager.load → allocate slot
  → ForwardBatch.lora_ids per token
  → backend apply delta (only for adapter tokens)

batching 隔离:
  ForwardBatch.lora_ids = [0, 0, 1, 1, 2, ...]
    0 = base (no delta)
    1 = adapter-A slot
    2 = adapter-B slot
  backend: if lora_id > 0 → apply_delta(weights[slot])

CUDA Graph:
  LoRA variant 是 graph key 的一部分
  不同 adapter 组合 → 不同 capture bucket
```

C：`sglang/01-modules/M14-moe-quantization-lora/README.md` 的 LoRAManager 方法、HTTP model 参数优先、ForwardBatch lora_ids、CUDA Graph variant。I：reset 时 metadata 泄漏的风险。U：多 adapter 并发加载的实际性能未测量。

**图文说明：** batch 中有三个请求：base、adapter-A、adapter-B。`lora_ids` 数组按 token 分配 id。backend 根据 id 选择性加 delta。右侧是 LoRA slot 管理器，标注 load/unload 操作。

**拓展知识：** 与 I14 衔接：LoRA slot 数量有限，`unload_lora_adapter` 支持 LRU 淘汰——长时间未使用的 adapter 被卸载以释放 slot 给新 adapter。

**常见追问及回答要点：**

1. **问：同一 batch 中不同请求使用不同 adapter 时，forward 如何工作？** 答：`ForwardBatch.lora_ids` 为每个 token 标注所属 adapter；backend 遍历 batch，只对非零 id 的 token 加对应的 adapter delta。
2. **问：adapter 切换需要重新捕获 CUDA Graph 吗？** 答：如果 adapter 组合变化导致 forward 路径或 shape 变化，可能需要；graph key 包含 LoRA variant。
3. **问：如何限制并发 adapter 数量？** 答：LoRA slot 数量由配置限制；超出时 LRU 淘汰不活跃的 adapter。

**易错点与反例：**

- 反例："所有请求共享同一个 adapter"——错误。每个请求可指定不同 adapter。
- 反例："LoRA 只影响 attention 层"——错误。LoRA 可应用于多个线性层。
- 易错：忘记 reset 时清理 adapter metadata——会导致上一批的 adapter id 泄漏。

**实践与验证：** [U] 同时发送 base 和 adapter 请求，验证 output 差异和 slot 管理。实际 GPU 运行未执行。

**分层评分标准：** 2分：能描述 LoRA 热加载的基本流程；4分：能分析 batching 隔离的 per-token id 机制；6分：能解释 CUDA Graph variant、LRU 淘汰、reset metadata 清理。

**证据与来源：** `sglang/01-modules/M14-moe-quantization-lora/README.md`（LoRAManager、HTTP model 参数优先、ForwardBatch lora_ids、CUDA Graph variant）；`sglang/01-modules/M02-http-api-protocol/README.md`（model 参数 adapter 解析）。

### I-I020 长上下文推理时 KV cache 超出显存有哪些处理策略？各有什么代价？

**元数据：** 知识域：`I15 长上下文`；能力目标：`C20 长上下文 KV 溢出处理`；题型：对比分析；频率：中频；前置/后续：`I-I011`（PagedAttention）、`I-I012`（radix cache）；证据状态：源码 C，GPU 运行 U。

**背景知识：** KV cache 超出显存时 SGLang 有四种处理策略 `sglang/90-cross-module/pooling-and-resource-management.md`：(1) **retract_decode**——decode 阶段 `check_decode_mem` 发现容量不足，按 retraction order 释放请求、做 host backup 并重新入队，无法恢复的请求设置 abort `sglang/01-modules/M04-scheduler-batching/README.md`；(2) **eviction**——RadixCache 从 `evictable_leaves` 选叶子，按 page-exact value 归还 allocator，删除叶子 `sglang/01-modules/M08-kv-cache/README.md`；(3) **chunked prefill**——长 prompt 分块处理，限制单轮 token 和 activation 峰值 `sglang/02-request-flow/03-调度器与连续批处理.md`；(4) **HiCache/offload**——把 KV 移到 host CPU 或 storage，需要时 prefetch/restore `sglang/01-modules/M13-disaggregation-hicache/README.md`。代价：retract 增加重新调度和潜在 prefill 成本；eviction 丢失已缓存的 prefix，下次需重算；chunked prefill 增加调度复杂度和 chunk 边界管理；offload 引入 PCIe/网络传输延迟和 staging buffer 成本。

**问题分析：** 1) retract_decode 的触发条件和代价；2) RadixCache eviction 的 page-aligned 代价；3) chunked prefill 的公平性和资源峰值控制；4) HiCache/offload 的传输开销和 prefetch 策略；5) 四种策略的组合使用。

**具体答案：** 面试中可以这样回答：

> KV cache 超出显存时 SGLang 有四层防御。第一层是 chunked prefill——从源头限制单轮 token 数量和 activation 峰值，避免一次性分配过多 KV；代价是调度复杂度增加，同一 `Req` 需复用 request row 和处理 partial page `sglang/02-request-flow/03-调度器与连续批处理.md`。第二层是 RadixCache eviction——从 `evictable_leaves` 选 priority 最低的叶子，按 page-exact value 归还 allocator；代价是丢失已缓存的 prefix，下次匹配时需重算，且 page-aligned 导致实际释放量可能大于请求目标 `sglang/01-modules/M08-kv-cache/README.md`。第三层是 decode retraction——`check_decode_mem` 失败后按 retraction order 释放请求、做 host backup 并重新入队；代价是重新调度增加延迟，无法恢复的请求被 abort `sglang/01-modules/M04-scheduler-batching/README.md`。第四层是 HiCache/offload——把 KV 移到 host CPU 或 storage，decode 时按需 prefetch/restore；代价是 PCIe/网络传输延迟和 staging buffer 占用，且 restore 超时需要 abort `sglang/01-modules/M13-disaggregation-hicache/README.md`。这四层按"预防→回收→降级→扩展"的顺序递进触发。

**技术洞察：**

```text
KV 溢出处理策略（按触发顺序）:

1. chunked prefill（预防）:
   限制单轮 token / activation 峰值
   代价: 调度复杂度, partial page 管理

2. RadixCache eviction（回收）:
   驱逐可淘汰叶子, 归还 allocator
   代价: prefix 丢失需重算, page-aligned 浪费

3. decode retraction（降级）:
   释放 decode 请求, host backup, requeue
   代价: 重新调度延迟, 不可恢复的 abort

4. HiCache/offload（扩展）:
   KV 移到 host/storage, 按需 restore
  代价: PCIe/网络延迟, staging buffer, 超时 abort
```

C：`sglang/01-modules/M04-scheduler-batching/README.md` 的 retract_decode；`sglang/01-modules/M08-kv-cache/README.md` 的 eviction；`sglang/02-request-flow/03-调度器与连续批处理.md` 的 chunked prefill；`sglang/01-modules/M13-disaggregation-hicache/README.md` 的 HiCache。I：四层递进的防御策略设计。U：各策略在实际负载下的触发频率未测量。

**图文说明：** 四层防御的纵向堆叠图。从上到下：chunked prefill → RadixCache eviction → decode retraction → HiCache offload。每层标注触发条件和代价。

**拓展知识：** 与 I13 衔接：PD disaggregation 把 prefill 和 decode 拆到不同节点，使 KV cache 压力分散；prefill 节点完成 KV 计算后通过 KV transfer 传给 decode 节点 `sglang/01-modules/M13-disaggregation-hicache/README.md`。

**常见追问及回答要点：**

1. **问：retract 和 eviction 的触发顺序？** 答：eviction 先尝试——从 RadixCache 回收不再需要的 prefix；如果 eviction 仍不够（所有 node 都被 lock），才触发 retract——撤回正在 decode 的请求。
2. **问：HiCache 的 prefetch 如何与调度配合？** 答：`DecodeHiCacheTransferMixin` 在 prefix match 后判断哪些部分需要 restore，prefetch 完成后才提交 request 继续 decode；restore 超时或失败时 abort 请求并清理 staging 资源 `sglang/01-modules/M13-disaggregation-hicache/README.md`。
3. **问：chunked prefill 的 chunk size 如何选择？** 答：需要在 activation 工作集（大 chunk 峰值高）、调度公平性（小 chunk 让其他请求获得更多机会）和 prefix cache 效率（大 chunk 更好地利用已缓存 prefix）之间权衡；SGLang 通过 `chunked_prefill_size` 和 `max_prefill_tokens` 联合控制 `sglang/02-request-flow/03-调度器与连续批处理.md`。

**易错点与反例：**

- 反例："KV 不够就 OOM 退出"——错误。SGLang 有四层防御，只有在所有策略都失败时才 abort 请求。
- 反例："eviction 总能释放足够空间"——错误。被 lock 的 node 不能被驱逐，如果所有 node 都被 active 请求引用，eviction 无法释放空间。
- 易错：忘记 retraction 后请求可以恢复——retraction 是临时撤回不是终止，请求回到 waiting queue 等待资源足够后继续。

**实践与验证：** [U] 限制 KV pool 大小为很小值（如仅能容纳 2 个并发长序列），注入 10 个 8K token 请求，依次观察 chunked prefill（限制单轮计算量）、radix eviction（回收已完成 prefix）、decode retraction（撤回活跃请求）、HiCache restore（从 host 恢复）的触发顺序和延迟影响。实际 GPU 运行未执行。

**分层评分标准：** 2分：能列举 KV 溢出的四种处理策略；4分：能分析各策略的代价（重算成本、传输延迟、调度复杂度）和触发条件；6分：能解释四层递进防御的顺序逻辑、eviction 与 lock_ref 的交互（被锁 node 不可驱逐）、HiCache prefetch/restore 的超时 abort 路径、PD disaggregation 作为架构级分散 KV 压力的方案。

**证据与来源：** `sglang/01-modules/M04-scheduler-batching/README.md`（retract_decode、chunked prefill）；`sglang/01-modules/M08-kv-cache/README.md`（eviction、RadixCache）；`sglang/01-modules/M13-disaggregation-hicache/README.md`（HiCache prefetch/restore）；`sglang/90-cross-module/pooling-and-resource-management.md`（四层池模型与 retraction ownership）。