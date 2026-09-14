# M15 多进程与 IPC 控制面

- 文档目的：解释 SGLang 如何组装 TokenizerManager、Scheduler、Detokenizer 的进程拓扑，如何分配并传输 IPC 地址，以及请求、结果、abort、ready 和 shutdown 如何跨边界流动。
- 证据 checkout：`/home/mtuser/workspace/repos/Github/sglang`。
- 证据版本：以该 checkout 当前 `main` 为准；本文只做源码分析，不把 GPU 服务、模型加载或多卡通信当作已运行验证。
- 前置阅读：[M01 CLI 与服务启动](../M01-cli-service-startup/README.md)、[M03 Tokenizer 与请求状态](../M03-tokenizer-request-state/README.md)、[多进程与 IPC（请求流程）](../../02-request-flow/06-多进程与IPC.md)。
- 相关模块：[M04 Scheduler 与连续批处理](../M04-scheduler-batching/README.md)、[M05 模型执行](../M05-model-execution/README.md)、[M02 HTTP/API 与协议](../M02-http-api-protocol/README.md)。

## 1. 一句话模型

**已确认**：标准 Python SRT runtime 把 API/Tokenizer 状态、GPU scheduler 和 CPU detokenizer 分成不同执行上下文，并用一组由 `PortArgs` 集中生成的 ZMQ endpoint 连接它们。`Engine` 负责创建拓扑和等待 ready；Scheduler 负责接收请求、执行模型并发回输出；Detokenizer 把 token ids 增量转换为文本；TokenizerManager 按 `rid` 找回本地 `ReqState` 并唤醒 API/Engine 调用者。

```text
主进程
  FastAPI / offline Engine
        │
        └─ TokenizerManager
             ├─ text → token ids
             ├─ rid_to_state
             ├─ PUSH tokenized request
             └─ PULL output
                    │ ZMQ
                    ▼
Scheduler 子进程（每个 TP/PP rank，或 DP controller）
  request receiver → admission/batch → worker/ModelRunner
        │
        └─ PUSH BatchTokenIDOutput / control output
                    │ ZMQ
                    ▼
Detokenizer 子进程（一个或多个 + router）
  token ids → incremental text
        │
        └─ PUSH BatchStrOutput
                    │ ZMQ
                    ▼
TokenizerManager → ReqState → HTTP/SSE/Engine result
```

**边界**：M15 解释进程、通道、序列化、ready 和清理；它不决定 continuous batching 的 admission、不执行 Transformer forward，也不定义 HTTP 协议字段。

## 2. 进程拓扑和职责

### 2.1 Engine 是拓扑组装者

`Engine` 的类注释把标准运行时拆成三类对象：主进程中的 TokenizerManager、Scheduler subprocess 和 DetokenizerManager subprocess；HTTP server、Engine 和 TokenizerManager 在主进程，IPC 通过 ZMQ 完成。[`python/sglang/srt/entrypoints/engine.py:222-234`]

`Engine._launch_subprocesses` 的顺序是：

```text
configure_logger
  → resolve_once / set env / load plugins
  → check_server_args / resolve auto parsers
  → publish tokenizer config
  → PortArgs.init_new
  → scheduler process(es)
  → optional DP controller / expert backup
  → detokenizer process(es)
  → TokenizerManager
  → wait_for_ready
  → watchdog
```

其中配置在 spawn 前完成并发布，避免子进程各自从未解析的 raw config 推导出不同结果。[`python/sglang/srt/entrypoints/engine.py:1051-1108`]

### 2.2 Scheduler 进程数量不等于“一个请求线程”

当不需要 DP controller 时，Engine 按 `pp_rank` 和 `tp_rank` 创建 scheduler process，为每个 process 创建单向 `mp.Pipe(duplex=False)` 作为启动信息回传通道，并计算对应 GPU id 与 attention/MoE ranks。[`python/sglang/srt/entrypoints/engine.py:847-917`]

当启用 DP 或 elastic EP join mode 时，Engine 改为先创建一个 data-parallel controller；controller 再管理实际 scheduler。`SchedulerInitResult.all_child_pids` 会在收到 ready 信息后追加 controller 报告的子进程 pid。[`python/sglang/srt/entrypoints/engine.py:862-965`]

因此：

- `mp.Pipe` 是父进程等待初始化信息的 startup control path，不是请求数据通道；
- ZMQ 是稳态请求、结果和控制消息通道；
- TP/PP rank 的 scheduler 只有 rank-zero IPC endpoint 承担主请求收发，其他 rank 使用空的发送 wrapper 或通过分布式协作参与计算；
- Ray backend 可重写 scheduler 启动实现，因此不能把 `mp.Process` 当作所有 backend 的共同实现。

## 3. `PortArgs`：拓扑地址的唯一来源

### 3.1 字段契约

`PortArgs` 是一次 Engine 实例的连接配置，不是模型或请求配置，当前包含：

| 字段 | 方向/用途 |
|---|---|
| `tokenizer_ipc_name` | Detokenizer → TokenizerManager；或 skip-tokenizer 时 Scheduler 直接回主进程 |
| `scheduler_input_ipc_name` | TokenizerManager → rank-zero Scheduler/DP controller |
| `detokenizer_ipc_name` | Scheduler → Detokenizer；多 worker 时先被 router 接管 |
| `nccl_port` | torch.distributed rendezvous |
| `rpc_ipc_name` | Engine/RPC → Scheduler |
| `metrics_ipc_name` | Scheduler → metrics collector |
| `tokenizer_worker_ipc_name` | 多 tokenizer worker → router |
| `decoupled_spec_ipc_config` | decoupled speculative 的独立 endpoint 配置 |
| `load_collector_ipc_name` | DP attention load snapshot 通道 |
| `instance_id` | load snapshot 的稳定共享实例标识 |

字段声明位于 `server_args.py:741-771`。把它作为整体传给各子进程可以避免各进程重复拼接地址。

### 3.2 单机 IPC 模式

未启用 DP attention 时，`PortArgs.init_new` 为 tokenizer、scheduler input、detokenizer、RPC 和 metrics 分别创建临时文件名，并形成 `ipc://...` endpoint；`nccl_port` 仍单独分配。[`python/sglang/srt/server_args.py:773-824`]

```text
TokenizerManager --PUSH--> scheduler_input_ipc_name
Scheduler        --PUSH--> detokenizer_ipc_name
Detokenizer      --PUSH--> tokenizer_ipc_name
Engine           --DEALER-> rpc_ipc_name
Scheduler        --PUSH--> metrics_ipc_name
```

这里的临时文件名是 ZMQ IPC 地址的命名载体；它不等于一个普通文件协议，也不应被文档误解为请求内容写入磁盘。

### 3.3 DP attention / 多节点 TCP 模式

启用 DP attention 时，代码根据 `dist_init_addr` 或服务端口派生 TCP 地址，为基础 rendezvous、detokenizer、RPC、metrics、load collector 和 scheduler input 分配不同端口，并在绑定前调用 `wait_port_available` 检查冲突。[`python/sglang/srt/server_args.py:812-915`]

scheduler input 在三种情形中含义不同：

1. `dp_rank is None`：TokenizerManager 到 DP controller；
2. Rust server + DP attention：按 `dp_rank` 使用 Rust path 的派生端口；
3. Python DP worker：使用 controller 分配的 `worker_ports[dp_rank]`。

**推断**：端口偏移本身不是业务协议；它只是让同一 instance 的各通道可确定地互不冲突。实际跨节点可达性仍取决于绑定地址、防火墙和部署网络，这些不是 `PortArgs` 单独能够保证的。

## 4. 每个进程如何建立通道

### 4.1 TokenizerManager

`TokenizerManager.init_ipc_channels` 建立两个主要 socket：从 detokenizer `PULL` 接收输出，向 scheduler `PUSH` 发送 tokenized request。[`python/sglang/srt/managers/tokenizer_manager.py:558-588`]

```python
recv_from_detokenizer = PULL(tokenizer_ipc_name)
send_to_scheduler = PUSH(scheduler_input_ipc_name)
```

多 tokenizer worker 模式下，发送目标改为 `tokenizer_worker_ipc_name`，并把本 worker 的 IPC 名称写入每个 outgoing object 的 `http_worker_ipc`，使 detokenizer/router 能把结果返回正确 worker。[`python/sglang/srt/managers/tokenizer_manager.py:558-588`]

### 4.2 Scheduler

`SchedulerIpcChannels.create` 只给 rank-zero scheduler 建立主收发通道：

- `PULL scheduler_input_ipc_name`：收 TokenizerManager 请求；
- `DEALER rpc_ipc_name`：收 Engine/RPC 控制请求；
- `PUSH tokenizer_ipc_name`：直接回 TokenizerManager 的控制/跳过 detokenizer 输出；
- `PUSH detokenizer_ipc_name`：把需要 decode 的输出发给 detokenizer；
- 可选 `PUSH metrics_ipc_name`：发统计数据。

非 rank-zero scheduler 使用 `SenderWrapper(None)`，而不是让每个 TP/PP rank 都竞争消费同一个 API 输入 socket。[`python/sglang/srt/managers/scheduler_components/ipc_channels.py:16-89`]

`skip_tokenizer_init` 是一个重要分支：若不需要 tokenizer/detokenizer，Scheduler 的 `send_to_detokenizer` 实际连接到 `tokenizer_ipc_name`，令结果直接回 TokenizerManager；这改变的是输出路径，不是 `BatchTokenIDOutput` 的对象契约。[`python/sglang/srt/managers/scheduler_components/ipc_channels.py:52-65`]

### 4.3 Detokenizer

`DetokenizerManager.__init__` 依次初始化 IPC、tokenizer、运行状态和 dispatcher。单 worker 模式下从 `detokenizer_ipc_name` PULL，并向 `tokenizer_ipc_name` PUSH；多 tokenizer 模式则由 mixin 的 socket mapping 直接回各 worker。[`python/sglang/srt/managers/detokenizer_manager.py:102-177`]

其 `event_loop` 每次接收一个对象，按类型 dispatch；输出非 `None` 时发给 tokenizer。[`python/sglang/srt/managers/detokenizer_manager.py:179-187`]

## 5. IPC 对象契约和序列化

### 5.1 两阶段请求对象

用户侧 `GenerateReqInput` 可以含 text、input_ids、input_embeds、multimodal data 和批量字段；TokenizerManager 完成 normalize、tokenize、采样参数校验后，构造 `TokenizedGenerateReqInput`。[`python/sglang/srt/managers/io_struct.py:173-224`][`python/sglang/srt/managers/tokenizer_manager.py:1356-1470`]

`TokenizedGenerateReqInput` 的 IPC 重点字段包括：

- `rid`、`http_worker_ipc`：请求身份和回程路由；
- `input_text`、`input_ids`、`input_embeds`、`mm_inputs`：执行输入；
- `sampling_params`：已 normalize/verify 的采样配置；
- `stream`、logprob、hidden states、grammar/custom processor 等输出要求；
- `bootstrap_*`、`routed_dp_rank`、`routing_key`：disaggregation/DP routing；
- `time_stats`：用 `PickleWrapper` 传输的观测状态。[`python/sglang/srt/managers/io_struct.py:972-1075`]

批量发送时，若请求可以合并，TokenizerManager 创建 `BatchTokenizedGenerateReqInput(batch=tokenized_objs)`；每个内部 item 保留自己的 `http_worker_ipc`，不能错误地只依赖 batch-level 的 `http_worker_ipcs`。[`python/sglang/srt/managers/io_struct.py:1078-1091`][`python/sglang/srt/managers/tokenizer_manager.py:1620-1650`]

### 5.2 输出对象

`BatchStrOutput` 按并行数组保存每个 rid 的：

- finish reason、增量 `output_strs`、`output_ids`；
- prompt/completion/reasoning/cached token 计数；
- input/output logprob 和 entropy；
- hidden states、routed experts、indexer top-k；
- retraction count、weight versions、token steps、customized info；
- optional multimodal token counts、speculative decoding 统计和 DP rank。[`python/sglang/srt/managers/io_struct.py:1531-1622`]

`AbortReq` 是反向控制消息，携带 `rid`、`abort_all`、`finished_reason`、`abort_message` 和 weight-version 信息；`ShutdownReq` 是广播到各 TP rank 的终止消息。[`python/sglang/srt/managers/io_struct.py:2042-2054`][`python/sglang/srt/managers/io_struct.py:2151-2158`]

### 5.3 msgpack 与 pickle fallback

默认路径：

```text
msgpack_encode
  → msgspec Encoder(enc_hook=enc_hook)
  → socket.send(bytes)

socket.recv(bytes)
  → msgspec Decoder(Union[_all_types])
  → msgpack_decode
```

`msgpack_encode/decode` 和同步/异步 socket helper 位于 `io_struct.py:2398-2504`。只有 msgspec Struct 和允许的 primitive 可以直接编码；任意 opaque object 必须显式包装为 `PickleWrapper`。设置 `_USE_PICKLE_IPC` 的环境开关后，才使用 `send_pyobj` / `recv_pyobj`。

**已确认的设计意图**：显式类型约束让 IPC schema 变化可被发现；pickle wrapper 只隔离确实无法 msgpack 化的 payload，而不是让所有 IPC 消息失去结构约束。

## 6. 从请求到响应：一条完整链

### M15-FLOW-001：普通单请求

| 步骤 | 源码 | 状态/数据变化 | 跨边界效果 |
|---|---|---|---|
| 1 | `TokenizerManager.generate_request` `[tokenizer_manager.py:776-845]` | normalize、校验、创建 `rid_to_state` | 进入本地状态表 |
| 2 | `_create_tokenized_object` `[tokenizer_manager.py:1356-1470]` | text/input ids → `TokenizedGenerateReqInput`；sampling normalize/verify | 准备 IPC payload |
| 3 | `_send_one_request` / `_send_batch_request` `[tokenizer_manager.py:1577-1650]` | wrap shared-memory/pickle fields，标记 dispatched | `PUSH` 到 scheduler |
| 4 | Scheduler request receiver / event loop | waiting → running → batch | GPU 执行并生成 token ids |
| 5 | Scheduler `send_to_detokenizer` | 形成 `BatchTokenIDOutput` | `PUSH` 到 detokenizer |
| 6 | Detokenizer `event_loop` `[detokenizer_manager.py:179-187]` | decode status 增量更新 | 形成 `BatchStrOutput` |
| 7 | TokenizerManager `handle_loop` `[tokenizer_manager.py:2225-2238]` | 接收 output | dispatch 到 `_handle_batch_output` |
| 8 | `_handle_batch_output` `[tokenizer_manager.py:2240-2429]` | 按 rid 更新 `ReqState`、out_list、event | 唤醒等待者 |
| 9 | `_stream_one_response` `[tokenizer_manager.py:1740-1839]` | drain 输出，完成时移除/记录 | yield 到 HTTP 或 Engine |

Scheduler 和 ModelRunner 的 admission/forward 细节属于 M04/M05；M15 只关注它们在 IPC 边界两侧消费和产生的对象。

### 6.1 输出回程的两个重要分支

1. **普通 decode**：Scheduler → Detokenizer → TokenizerManager，适合需要 tokenizer 增量 decode 的生成请求；
2. **skip tokenizer init**：Scheduler → TokenizerManager，跳过 detokenizer，适用于 tokenizer 不应初始化或输出已可直接消费的场景。[`python/sglang/srt/managers/scheduler_components/ipc_channels.py:52-65`]

多 tokenizer/detokenizer 模式还会增加 router 和 per-worker route；不能只根据一个全局 tokenizer socket 推断最终回程。

## 7. ready、健康与 backpressure

### 7.1 startup pipe 的语义

每个 scheduler process 创建后，`run_scheduler_process` 构造 Scheduler，调用 `scheduler.get_init_info()` 写入 `pipe_writer`，再进入阻塞 event loop。[`python/sglang/srt/entrypoints/engine.py:5744-5812`]

父进程的 `SchedulerInitResult.wait_for_ready` 调用 `_wait_for_scheduler_ready`，收集 scheduler info；只有等 ready 后，Engine 才设置 startup time、启动 watchdog 并把 runtime 交给上层。[`python/sglang/srt/entrypoints/engine.py:938-965`][`python/sglang/srt/entrypoints/engine.py:1237-1259`]

**已确认**：startup pipe 传的是初始化信息和 readiness，不是逐 token 的数据面；把它误认为请求 IPC 会导致错误的调试方向。

### 7.2 ZMQ 发送不是 API 完成确认

PUSH 发送成功只表示消息进入 ZMQ 的发送路径，不代表 scheduler 已 admission、GPU 已执行或客户端已收到响应。请求是否完成必须看后续 `BatchStrOutput`/abort 和 `ReqState.finished`。

`TokenizerManager._stream_one_response` 会等待 `state.event`；HTTP request disconnect 时会调用 `abort_request`，以避免等待中的请求无限占用本地状态。[`python/sglang/srt/managers/tokenizer_manager.py:1740-1765`]

### 7.3 backlog 与批量汇聚

当多个增量 chunk 已排队，TokenizerManager 可 coalesce streaming chunks，再一次性合并 text/output ids；这减少消费者处理开销，但 backlog 可能增加受影响请求的 P99 ITL。[`python/sglang/srt/managers/tokenizer_manager.py:1652-1687`]

因此 M15 的 backpressure 不只来自 ZMQ high-water mark，还可能来自：

- scheduler 侧 waiting/running batch；
- detokenizer decode capacity；
- TokenizerManager 的 event loop 和 response consumer；
- streaming chunk coalescing。

**未知**：仅凭这些模块源码不能给出某一部署环境的实际队列容量、端到端吞吐或最佳 HWM 参数；需要运行时 profiling。

## 8. abort、异常传播和资源清理

### 8.1 请求级 abort

当 handler 在 dispatch 前失败时，`_release_req_states_on_failure` 删除未投递的本地 state；已经 dispatch 的请求则发送 abort，并保留 state 等 scheduler 回应清理。[`python/sglang/srt/managers/tokenizer_manager.py:3508-3528`]

Scheduler 的 `abort_request` 会从 waiting queue 删除匹配请求，释放相应请求资源，并通过 `send_to_tokenizer` 发送 abort output；对于已进入执行或特殊 disaggregation 状态的请求，还会走对应的 KV/metadata/receiver 清理。[`python/sglang/srt/managers/scheduler.py:5171-5217`]

TokenizerManager 收到 abort echo 后，会处理“正常完成和 abort 同时到达”的竞态：若 `rid_to_state` 已被完成路径删除，则记录并忽略该 echo；否则构造带 finish reason 的最终输出。[`python/sglang/srt/managers/tokenizer_manager.py:3247-3299`]

### 8.2 子进程异常

Scheduler 进程异常时：

1. 记录 traceback；
2. 向 parent 发送 `SIGQUIT`；
3. 可选地通过 `SGLANG_KILLPG_ON_SCHEDULER_EXCEPTION` kill process group；
4. graceful exit 时才调用 `release_host_resources`，避免异常状态下可能阻塞的同步清理。[`python/sglang/srt/managers/scheduler.py:5744-5833`]

Detokenizer 异常时记录 traceback，清理 multi-worker socket mapping（若已创建），然后向 parent 发送 `SIGQUIT`。[`python/sglang/srt/managers/detokenizer_manager.py:539-563`]

这是一种“子进程主动通知 + 主进程统一收尾”的模型；SIGQUIT 本身不是请求级错误 envelope。

### 8.3 graceful shutdown 的顺序

收到 SIGTERM 后，TokenizerManager 的 watchdog 会：

```text
停止接收新请求/等待现有 rid 清空
  → 停止 subprocess watchdog（子进程退出是预期行为）
  → dispatch ShutdownReq
  → 等待 scheduler processes
  → kill_process_tree 处理残余子进程
  → sys.exit(0)
```

[`python/sglang/srt/managers/tokenizer_manager.py:3197-3241`]

Scheduler 的 `handle_shutdown` 只设置 `gracefully_exit=True`，让 event loop 退出；`run_scheduler_process` 的 `finally` 再执行指标发布线程 teardown，并在 graceful path 释放 host resources。[`python/sglang/srt/managers/scheduler.py:5607-5610`][`python/sglang/srt/managers/scheduler.py:5810-5833`]

Engine 的 `shutdown` 还会停止 watchdog、关闭 RPC socket、优先终止 weight-cache daemons，最后调用 `kill_process_tree`；无论中间过程如何，`finally` 会关闭 multimodal processor 和 CUDA VMM feature transport。[`python/sglang/srt/entrypoints/engine.py:1273-1304`]

**清理不变量**：

- 进程退出不应留下仍被 watchdog 当作 crash 的子进程；
- 已 dispatch 请求不能只删除本地 state 而不通知 scheduler；
- shutdown 的 graceful wait 结束后仍需要 hard-kill 兜底；
- daemon、IPC、multimodal transport 等非 scheduler 资源必须由拥有它们的对象清理。

## 9. 小例子和调试断点

### 例子：单机普通 Engine 的一次请求

假设 `tokenizer_worker_num=1`、`detokenizer_worker_num=1`、未启用 DP attention：

```text
PortArgs.init_new
  tokenizer_ipc_name       = ipc:///tmp/tokenizer-...
  scheduler_input_ipc_name = ipc:///tmp/scheduler-...
  detokenizer_ipc_name     = ipc:///tmp/detokenizer-...

TokenizerManager PUSH scheduler_input
Scheduler PUSH detokenizer
Detokenizer PUSH tokenizer
```

调用 `engine.generate("hello")` 时，`rid` 首先只存在主进程 `rid_to_state`；scheduler 的 `Req`、KV block 和 batch 状态是另一个进程中的对象；detokenizer 还有自己的 `DecodeStatus`。三个状态不会因为同名字段自动共享，跨边界只能通过序列化消息和显式 route 关联。这是静态调用链示例，未在本 checkout 启动。

### 推荐断点

1. `engine.py:1075-1108`：确认 resolve、publish 和 `PortArgs` 创建前后的配置。
2. `engine.py:883-917`：确认 TP/PP scheduler 数量、GPU id 和 startup pipe。
3. `server_args.py:812-915`：确认 IPC/TCP endpoint、DP rank 和端口冲突。
4. `tokenizer_manager.py:558-588`：确认主进程 socket 类型与多 tokenizer route。
5. `scheduler_components/ipc_channels.py:36-88`：确认 rank-zero 和 skip-tokenizer 分支。
6. `tokenizer_manager.py:1577-1650`：确认消息是否单条/批量发送以及 dispatched 标记。
7. `tokenizer_manager.py:2225-2240`：确认主进程是否仍在消费 detokenizer 输出。
8. `scheduler.py:5171-5217`：排查 abort 后 waiting/KV/metadata 是否释放。
9. `tokenizer_manager.py:3231-3241`：排查 graceful shutdown 是否发送 `ShutdownReq`。
10. 子进程 `run_scheduler_process` / `run_detokenizer_process` 的 exception handler：确认 SIGQUIT 来源，而不是把它误判为 HTTP 400。

## 10. 测试和证据边界

推荐静态检查和专项测试位置：

- `test/registered/unit/` 中与 `io_struct`、tokenizer manager、request state、IPC 相关的单元测试；
- `test/registered/core/test_srt_endpoint.py`：端点和 runtime 集成路径；
- `test/manual/` 中的 Engine、Ray、多进程和部署变体测试；
- `python/sglang/srt/test/`（若 checkout 当前版本存在对应专项目录）：优先按实际文件确认。

本文可先执行低成本的源码/文档检查；没有把任何测试文件的存在当作本次测试成功的证据。

**未验证**：本次未启动 scheduler/detokenizer，未建立真实 ZMQ IPC，未加载 checkpoint，未执行 GPU、多卡、Ray、Rust server、CUDA Graph、NCCL 或完整测试 suite；实际端口可用性、进程时序、队列压力和异常恢复仍需运行验证。

## 11. 修改影响

修改 M15 时至少检查：

| 改动 | 需要联动复核 |
|---|---|
| `PortArgs` 字段/派生端口 | Engine、DP controller、scheduler、detokenizer、Rust/多节点部署 |
| msgpack Struct 字段 | TokenizerManager、Scheduler、Detokenizer、版本兼容和 PickleWrapper |
| socket 类型或 bind/connect | 所有对应 producer/consumer、启动顺序和多 tokenizer/router |
| batch routing 字段 | `http_worker_ipc`、MultiTokenizerRouter、Batch output 对齐 |
| `AbortReq` 语义 | ReqState、waiting/running/ KV 释放、HTTP disconnect |
| `ShutdownReq` / signal handler | Scheduler event loop、watchdog、Engine shutdown |
| detokenizer worker 数量 | router 拓扑、socket mapping、decode state 容量 |
| skip tokenizer | 输出对象路径、tokenizer 初始化假设、协议结果格式 |

**已确认**：M15 不是“换一个 socket”这么局部的改动；它同时影响请求对象 schema、进程生命周期、状态所有权、异常传播和 GPU/CPU 资源释放。
