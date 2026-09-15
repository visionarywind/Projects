# 06 多进程与 IPC：Tokenizer、Scheduler、Detokenizer 如何协作

- 文档目的：解释 02-request-flow/06-多进程与IPC.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/sglang HEAD 78be4b50af（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 02-request-flow/06-多进程与IPC.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


前面已经知道，一个请求会经过 TokenizerManager、Scheduler、ModelRunner 和 DetokenizerManager。本章回答另一个关键问题：

> 这些对象是在同一个 Python 进程里调用的吗？它们之间传递的到底是什么？为什么 SGLang 要用多个进程和 ZMQ，而不是在 HTTP handler 里直接调用模型？

答案是：标准 SRT 服务把**控制面、GPU 执行面和文本转换面**拆开。拆分后，各部分可以分别等待网络、做 CPU tokenization、调度 GPU batch 或进行增量 detokenization，而不会互相阻塞。

---

## 1. 先看进程地图

标准 Python HTTP 路径可以画成：

```text
                    ┌────────────────────────────┐
HTTP client ───────▶│ 主进程                       │
                    │ FastAPI                     │
                    │ TokenizerManager            │
                    └───────────┬────────────────┘
                                │ ZMQ PUSH
                                ▼
                    ┌────────────────────────────┐
                    │ Scheduler 子进程             │
                    │ 请求队列 / batch / GPU       │
                    │ ModelRunner / CUDA           │
                    └───────────┬────────────────┘
                                │ ZMQ PUSH
                                ▼
                    ┌────────────────────────────┐
                    │ Detokenizer 子进程           │
                    │ token ids → 增量文本          │
                    └───────────┬────────────────┘
                                │ ZMQ PUSH
                                ▼
                    ┌────────────────────────────┐
                    │ 主进程 TokenizerManager     │
                    │ 唤醒对应 HTTP 请求           │
                    └────────────────────────────┘
```

这里的“TokenizerManager”虽然在名字里有 tokenizer，但它实际上也承担：

- 接收 HTTP 侧的异步请求；
- 把文本转换为 token ids；
- 保存每个请求的 `ReqState`；
- 把 tokenized request 发给 Scheduler；
- 接收生成结果并唤醒正确的 HTTP 流；
- 处理 abort、session、metrics 和协议层状态。

Scheduler 则拥有 GPU 和推理循环。Detokenizer 是一个独立的 CPU 进程，负责把不断增长的 token id 序列转换成可发送给客户端的文本。

### 1.1 为什么要拆进程

假设只有一个进程：

```python
async def endpoint(request):
    ids = tokenizer.encode(request.text)
    output_ids = model.generate(ids)
    return tokenizer.decode(output_ids)
```

这个简化模型有几个问题：

- `model.generate` 可能长期占用执行线程；
- 多个请求不能被 Scheduler 统一合并成 continuous batch；
- detokenization 的慢路径可能拖住 GPU 调度；
- HTTP 请求的生命周期和 GPU batch 中的 request 生命周期强耦合；
- Tensor Parallel 的多个 rank 很难各自拥有稳定的 worker 进程。

SGLang 选择消息传递：

```text
网络输入、GPU 调度、文本输出
      分离并通过消息连接
```

因此每个环节可以有自己的事件循环和故障边界。

---

## 2. Engine 是进程拓扑的组装者

### 2.1 `launch_server` 不直接启动模型循环

源文件：`python/sglang/srt/entrypoints/http_server.py`，约 2798 行。

```python
def launch_server(server_args, ...):
    (
        tokenizer_manager,
        template_manager,
        port_args,
        scheduler_init_result,
        subprocess_watchdog,
        _weight_cache_daemon_procs,
    ) = Engine._launch_subprocesses(
        server_args=server_args,
        init_tokenizer_manager_func=init_tokenizer_manager_func,
        run_scheduler_process_func=run_scheduler_process_func,
        run_detokenizer_process_func=run_detokenizer_process_func,
    )

    if envs.SGLANG_RUST_SERVER.get():
        ...
    else:
        _setup_and_run_http_server(...)
```

`launch_server` 的职责是组装拓扑，然后把主进程交给 HTTP server。它没有把调度循环写在 FastAPI 路由中。

注释直接列出了标准 SRT 的三个核心部分：

1. HTTP server；
2. TokenizerManager；
3. Scheduler subprocess 和 DetokenizerManager subprocess。

标准情况下，HTTP server、Engine 和 TokenizerManager在主进程；Scheduler 和 DetokenizerManager 是子进程。它们使用 ZMQ IPC 连接。

### 2.2 `_launch_subprocesses` 的启动顺序

源文件：`python/sglang/srt/entrypoints/engine.py`，约 1050 行。

关键步骤可以归纳为：

```text
配置解析
  ▼
设置环境和插件
  ▼
启动前校验 / 自动 parser
  ▼
publish tokenizer 角色配置
  ▼
分配 PortArgs
  ▼
启动 detokenizer
  ▼
启动 scheduler 或 DP controller
  ▼
主进程初始化 TokenizerManager
  ▼
等待 scheduler ready
  ▼
启动 HTTP 和 warmup
```

源码中首先确保配置已经解析：

```python
configure_logger(server_args)
server_args.resolve_once()
_set_envs_and_config(server_args)
load_plugins()
server_args.check_server_args()
```

随后处理依赖 tokenizer 和 chat template 的自动 parser，最后发布 tokenizer 角色配置：

```python
parsers = resolving_view(server_args)
if parsers.reasoning_parser == "auto" or parsers.tool_call_parser == "auto":
    resolve_auto_parsers(server_args)

publish(server_args, role="tokenizer")
```

这解释了上一章的配置原则：**先 resolve，再 publish，再构造读取配置的对象**。TokenizerManager 初始化时读取的是 `get_serving()`、`get_model()` 等 bags，而不是一个尚未投影的配置。

---

## 3. `PortArgs`：把“通道地址”集中打包

### 3.1 为什么需要一个单独的数据结构

进程之间至少需要这些方向：

```text
TokenizerManager → Scheduler
Scheduler        → DetokenizerManager
Detokenizer      → TokenizerManager
Engine/RPC       → Scheduler
Scheduler        → metrics collector
```

如果每个进程各自拼接 socket 地址，很容易出现：

- 一端 bind、另一端 connect 的地址不一致；
- 多节点和单节点使用了错误的地址类型；
- DP rank 之间重用了不该重用的端口；
- 多 tokenizer/detokenizer 模式下覆盖了共享通道。

因此 `server_args.py` 中的 `PortArgs` 统一携带：

```python
@dataclasses.dataclass
class PortArgs:
    tokenizer_ipc_name: str
    scheduler_input_ipc_name: str
    detokenizer_ipc_name: str
    nccl_port: int
    rpc_ipc_name: str
    metrics_ipc_name: str
    tokenizer_worker_ipc_name: str | None
    decoupled_spec_ipc_config: ...
```

这些字段不是模型配置，而是本次 Engine 拓扑的连接信息。它们通常由主进程创建，再随启动参数传给子进程。

### 3.2 单机模式：IPC endpoint

当没有启用 DP attention 时，`PortArgs.init_new` 使用临时文件名生成 ZMQ IPC 地址：

```python
return PortArgs(
    tokenizer_ipc_name=f"ipc://{tempfile.NamedTemporaryFile(delete=False).name}",
    scheduler_input_ipc_name=f"ipc://{tempfile.NamedTemporaryFile(delete=False).name}",
    detokenizer_ipc_name=f"ipc://{tempfile.NamedTemporaryFile(delete=False).name}",
    nccl_port=nccl_port,
    rpc_ipc_name=f"ipc://{tempfile.NamedTemporaryFile(delete=False).name}",
    metrics_ipc_name=f"ipc://{tempfile.NamedTemporaryFile(delete=False).name}",
    ...,
)
```

可以把 `ipc:///tmp/...` 理解为本机进程间的“虚拟 Unix socket 名字”。数据不需要经过公开 TCP 地址，通常适合单机低延迟通信。

### 3.3 DP attention 或多节点：TCP endpoint

DP attention 需要跨进程甚至跨节点连接，因此 `PortArgs` 会生成 TCP 地址：

```python
NetworkAddress(dist_init_host, port_base).to_tcp()
NetworkAddress(dist_init_host, scheduler_input_port).to_tcp()
NetworkAddress(dist_init_host, detokenizer_port).to_tcp()
```

代码还会检查端口是否可用，并为 detokenizer、RPC、metrics 和 scheduler input 分配不同偏移量。这里的重点不是某个具体偏移量，而是：

> 连接地址是启动拓扑的一部分，必须由一个中心对象一次性计算，再传给所有参与者。

---

## 4. TokenizerManager：主进程里的消息网关

### 4.1 创建两个主要 socket

源文件：`python/sglang/srt/managers/tokenizer_manager.py`，约 558 行。

```python
def init_ipc_channels(self, port_args: PortArgs):
    context = zmq.asyncio.Context(2)
    self.recv_from_detokenizer = get_zmq_socket(
        context, zmq.PULL, port_args.tokenizer_ipc_name, True
    )
    if get_serving().tokenizer_worker_num == 1:
        self.send_to_scheduler = get_zmq_socket(
            context, zmq.PUSH, port_args.scheduler_input_ipc_name, True
        )
        self.tokenizer_ipc_name = None
    else:
        self.send_to_scheduler = get_zmq_socket(
            context, zmq.PUSH, port_args.tokenizer_worker_ipc_name, False
        )
        self.tokenizer_ipc_name = port_args.tokenizer_ipc_name
```

`PUSH` 和 `PULL` 是 ZeroMQ 的单向流水线模式：

```text
PUSH  ─────▶  PULL
```

主进程：

- 从 detokenizer `PULL` 输出；
- 向 scheduler `PUSH` tokenized request。

最后一个 `tokenizer_ipc_name` 在多 tokenizer 模式中承担回传路由标记的作用，后面会解释。

### 4.2 发送请求：不是直接调用 Scheduler

前一章已经看到 `_send_one_request` 会准备 multimodal features、封装共享内存字段并 dispatch。底层 dispatch 很薄：

```python
def _dispatch_to_scheduler(self, obj: Any) -> None:
    if self.tokenizer_ipc_name is not None:
        stamp_http_worker_ipc(obj, self.tokenizer_ipc_name)
    sock_send(self.send_to_scheduler, obj)
```

这里传送的是 `GenerateReqInput` 经 tokenization 后的结构化消息，而不是一个 Python 协程或 callback。

`stamp_http_worker_ipc` 的意义是：多 tokenizer worker 时，Scheduler 生成结果后需要知道该发回哪个 HTTP worker。

### 4.3 接收结果：异步事件循环

源文件：`tokenizer_manager.py`，约 2225 行。

```python
async def handle_loop(self):
    while True:
        with self.soft_watchdog.disable():
            recv_obj = await async_sock_recv(self.recv_from_detokenizer)
        if isinstance(
            recv_obj,
            (BatchStrOutput, BatchEmbeddingOutput, BatchTokenIDOutput),
        ):
            await self._handle_batch_output(recv_obj)
        else:
            self._result_dispatcher(recv_obj)
        self.last_receive_tstamp = real_time()
        self.soft_watchdog.feed()
```

这个循环不关心 GPU batch 是如何计算的。它只按消息类型分流：

- `BatchStrOutput`：detokenizer 已经转好的文本；
- `BatchTokenIDOutput`：需要在 tokenizer 侧处理 token ids 或特殊协议；
- `BatchEmbeddingOutput`：embedding 请求；
- 其他控制消息：交给 `_result_dispatcher`。

收到 batch 后，`_handle_batch_output` 按 `rid` 找到请求状态：

```python
for i, rid in enumerate(recv_obj.rids):
    state = self.rid_to_state.get(rid, None)
    if state is None:
        ...
        continue
```

这就是为什么一个 GPU batch 可以包含多个 API 请求：输出消息带着每行的 `rid`，TokenizerManager 再将每行结果分发给各自的 `ReqState`。

---

## 5. Scheduler 侧：先 publish，再建立 IPC channels

### 5.1 子进程入口

源文件：`python/sglang/srt/managers/scheduler.py`，约 5744 行。

```python
def run_scheduler_process(
    server_args,
    port_args,
    gpu_id,
    tp_rank,
    attn_cp_rank,
    moe_dp_rank,
    moe_ep_rank,
    pp_rank,
    dp_rank,
    pipe_writer,
    ...,
):
    load_plugins()
    publish(server_args, role="scheduler")
    dp_rank = configure_scheduler_process(...)

    scheduler = Scheduler(...)
    pipe_writer.send(scheduler.get_init_info())
    scheduler.run_event_loop()
```

顺序非常重要：

```text
publish
  ▼
configure device / distributed groups
  ▼
构造 Scheduler
  ▼
发送 ready 信息
  ▼
进入 event loop
```

Scheduler 构造阶段会读取大量有效配置，例如并行宽度、KV cache、attention backend 和 scheduling limits。因此不能把 `publish` 放到构造器后面。

### 5.2 `SchedulerIpcChannels.create`

源文件：`python/sglang/srt/managers/scheduler_components/ipc_channels.py`。

```python
if is_rank_zero:
    recv_from_tokenizer = get_zmq_socket(
        context, zmq.PULL, port_args.scheduler_input_ipc_name, False
    )
    recv_from_rpc = get_zmq_socket(
        context, zmq.DEALER, port_args.rpc_ipc_name, False
    )
    send_to_tokenizer_raw = get_zmq_socket(
        context, zmq.PUSH, port_args.tokenizer_ipc_name, False
    )

    if skip_tokenizer_init:
        send_to_detokenizer_raw = get_zmq_socket(
            context, zmq.PUSH, port_args.tokenizer_ipc_name, False
        )
    else:
        send_to_detokenizer_raw = get_zmq_socket(
            context, zmq.PUSH, port_args.detokenizer_ipc_name, False
        )
```

这里 rank zero 才拥有控制侧 IPC：

```text
TokenizerManager ──PUSH──▶ rank 0 Scheduler ──PUSH──▶ Detokenizer
                                      │
                                      └──────────────▶ Tokenizer
```

其他 TP/PP rank 通过分布式通信参与 forward，而不是每个 rank 都直接接受 HTTP 请求。这样 Scheduler 的请求入口和输出出口只有一个控制面。

### 5.3 为什么 `skip_tokenizer_init` 会改变输出路径

如果是 embedding 或某些无需生成文本的路径，Scheduler 可能直接把结果发回 tokenizer 侧：

```python
if skip_tokenizer_init:
    send_to_detokenizer_raw = get_zmq_socket(
        context, zmq.PUSH, port_args.tokenizer_ipc_name, False
    )
```

普通生成路径是：

```text
Scheduler → Detokenizer → TokenizerManager
```

跳过 tokenizer/detokenizer 的路径则是：

```text
Scheduler → TokenizerManager
```

这不是“少创建一个无关对象”，而是改变了消息拓扑；因此通道初始化必须知道这个配置。

---

## 6. DetokenizerManager：专门消费输出 token

### 6.1 进程入口

源文件：`python/sglang/srt/managers/detokenizer_manager.py`，约 539 行。

```python
def run_detokenizer_process(server_args, port_args, ...):
    kill_itself_when_parent_died()
    setproctitle.setproctitle("sglang::detokenizer")
    configure_logger(server_args)
    publish(server_args, role="detokenizer")

    manager = detokenizer_manager_class(server_args, port_args)
    if get_serving().tokenizer_worker_num == 1:
        manager.event_loop()
    else:
        manager.multi_http_worker_event_loop()
```

同样的启动规律再次出现：

```text
进程保护
  ▼
日志
  ▼
publish 有效配置
  ▼
构造 manager
  ▼
阻塞在消息 event loop
```

### 6.2 输入和输出 socket

`DetokenizerManager.init_ipc_channels`：

```python
self.recv_from_scheduler = get_zmq_socket(
    context, zmq.PULL, port_args.detokenizer_ipc_name, True
)
if get_serving().tokenizer_worker_num == 1:
    self.send_to_tokenizer = get_zmq_socket(
        context, zmq.PUSH, port_args.tokenizer_ipc_name, False
    )
```

它只关心：

```text
从 Scheduler 收什么
向 TokenizerManager 发什么
```

它不会参与 waiting queue、KV block 分配或模型 forward。收到消息后通过 type-based dispatcher 处理：

```python
self._request_dispatcher = TypeBasedDispatcher(
    [
        (BatchEmbeddingOutput, self.handle_batch_embedding_out),
        (BatchTokenIDOutput, self.handle_batch_token_id_out),
        (FreezeGCReq, self.handle_freeze_gc_req),
        (ConfigureLoggingReq, self.handle_configure_logging_req),
    ]
)
```

事件循环是同步的 CPU 消费循环：

```python
while True:
    with self.soft_watchdog.disable():
        recv_obj = sock_recv(self.recv_from_scheduler)
    output = self._request_dispatcher(recv_obj)
    if output is not None:
        sock_send(self.send_to_tokenizer, output)
    self.soft_watchdog.feed()
```

这里的 `sock_recv` 阻塞只发生在 detokenizer 自己的进程，不会阻塞主进程的 asyncio loop，也不会阻塞 Scheduler 的 GPU 调度 loop。

### 6.3 增量 detokenization 的核心思想

Scheduler 每轮 decode 只生成一小段新 token。Detokenizer 不必每次从头返回完整文本，而是维护每个 `rid` 的状态：

```text
历史 token ids： [A, B, C]
新 token ids：          [D]
                         ▼
只解码新增范围，产生 delta text
```

源代码中的状态包括 decoded text、chunk、长度和请求的增量偏移。这样 SSE 流可以不断发送 delta，而不需要客户端每次接收完整字符串。

特殊 token、stop string、UTF-8 不完整边界也在这个层面处理。比如一个多字节字符的 token 序列尚未完整时，detokenizer 可以暂不输出半个字符，避免流式响应产生非法文本。

---

## 7. 输出消息为什么带 `rid` 和一组列表

### 7.1 Batch 消息不是单请求消息

假设同一轮 GPU forward 中有三个请求：

```text
row 0 → rid=req-A
row 1 → rid=req-B
row 2 → rid=req-C
```

Scheduler 生成结果后可以构造一个 batch 输出：

```text
rids             = [req-A, req-B, req-C]
finished_reasons = [None, "stop", None]
output_ids       = [[101], [202, 203], [303]]
```

因此消息结构通常是“一个 batch + 每个 row 一个结果”，而不是连续发送三个完全独立的对象。

这样做的好处是：

- 与 GPU batch 的行布局一致；
- 减少 IPC 消息数量；
- TokenizerManager 可以一次处理一批结果；
- metrics、logprob 和 finish reason 可以按同一索引对齐。

### 7.2 `SenderWrapper` 传递回程路由

源文件：`python/sglang/srt/managers/scheduler_components/output_sender.py`。

```python
class SenderWrapper:
    def send_output(self, output, recv_obj=None):
        if self.socket is None:
            return

        http_worker_ipc = getattr(recv_obj, "http_worker_ipc", None)
        if (
            isinstance(output, BaseReq)
            and http_worker_ipc is not None
            and output.http_worker_ipc is None
        ):
            output.http_worker_ipc = http_worker_ipc

        sock_send(self.socket, output)
```

多 tokenizer worker 时，请求从某个 HTTP worker 来，输出也必须回到那个 worker。`http_worker_ipc` 就是消息中的 return route。

注意这不是把 Python callback 跨进程传递过去，而是传递一个可序列化的 IPC 地址/标识。Scheduler 只需把 route 复制到输出消息，路由器就能完成回送。

---

## 8. 初始化 ready 信号：为什么同时使用 `mp.Pipe` 和 ZMQ

### 8.1 两种通信解决不同问题

SGLang 并非所有消息都使用 ZMQ：

```text
ZMQ       → 长期运行时的数据流：请求、结果、控制消息
mp.Pipe   → 启动阶段的少量同步信息：ready、初始化信息
```

Engine 启动 Scheduler 时创建单向 pipe：

```python
reader, writer = mp.Pipe(duplex=False)
proc = mp.Process(
    target=run_scheduler_process_func,
    args=(..., writer),
)
proc.start()
```

Scheduler 完成初始化后：

```python
pipe_writer.send(scheduler.get_init_info())
```

主进程等待这些信息：

```python
def wait_for_ready():
    infos = _wait_for_scheduler_ready(
        scheduler_pipe_readers,
        scheduler_procs,
    )
    scheduler_infos.extend(infos)
```

### 8.2 为什么不用第一条 ZMQ 消息表示 ready

ready 信息只在启动阶段发送一次，并且主进程需要判断：

- 所有 TP/PP rank 是否成功初始化；
- 显存和 KV cache 是否分配成功；
- CUDA Graph warmup 信息是什么；
- 子进程是否已经可以接受请求。

`mp.Pipe` 简单、同步、与具体运行时消息队列解耦。ZMQ 则保留给长生命周期的高频消息。

### 8.3 ready 之后才开放 HTTP

如果 HTTP 先开放，而 Scheduler 尚未 ready，第一批请求会遇到：

- socket 尚未建立；
- 模型权重仍在加载；
- KV pool 尚未分配；
- CUDA Graph 仍在 warmup。

所以 Engine 先等待 scheduler initialization，再完成 HTTP setup 和 warmup，最后才宣布服务 ready。这也是为什么健康检查不能只看“端口已经 bind”。

---

## 9. 多 tokenizer 和多 detokenizer：路由器如何扩展拓扑

### 9.1 多 tokenizer worker

单 tokenizer 模式：

```text
主 HTTP/TokenizerManager ──▶ Scheduler
```

多 tokenizer 模式：

```text
HTTP worker 0 ─┐
HTTP worker 1 ─┼──▶ Tokenizer worker/router ──▶ Scheduler
HTTP worker 2 ─┘
```

`TokenizerManager.init_ipc_channels` 在 worker 数大于 1 时连接到 `tokenizer_worker_ipc_name`，并为当前 HTTP worker 生成自己的 IPC 名字。消息进入 Scheduler 前会被打上回程地址。

多 tokenizer worker 的初始化还通过共享内存传递一次性启动数据：

```python
port_args, server_args, scheduler_info = read_from_shared_memory(
    f"multi_tokenizer_args_{main_pid}"
)
publish(server_args, role="tokenizer")
```

这避免把完整启动信息重复放进环境变量，也避免让每个 worker 重新推导端口和 scheduler 状态。

### 9.2 多 detokenizer worker

Engine 的 `_launch_detokenizer_subprocesses` 对 worker 数量分支：

```python
if get_serving().detokenizer_worker_num <= 1:
    proc = mp.Process(
        target=run_detokenizer_process_func,
        args=(server_args, port_args),
    )
    proc.start()
    return ...
```

多个 worker 时，每个 detokenizer 得到私有 socket：

```python
worker_ipc = f"ipc://{tempfile.NamedTemporaryFile(delete=False).name}"
port_args.detokenizer_ipc_name = worker_ipc
proc = mp.Process(...)
```

然后恢复原始 router 地址，并启动 `MultiDetokenizerRouter`：

```text
Scheduler ──▶ 原始 detokenizer router ──┬──▶ worker 0
                                        ├──▶ worker 1
                                        └──▶ worker 2
```

Scheduler 不需要知道每个 worker 的细节；它只连接稳定的 router endpoint。这样可以在不改变 GPU 侧代码的情况下扩展 CPU detokenization 吞吐。

---

## 10. 消息序列化：跨进程传的不是共享 Python 引用

### 10.1 一个请求跨过的表示层

请求在进程边界上的变化可以画成：

```text
HTTP JSON
   ▼ FastAPI/Pydantic
GenerateReqInput
   ▼ tokenizer
TokenizedGenerateReqInput
   ▼ pickle / shared-memory wrapping
Scheduler-side Req
   ▼ GPU batch
BatchTokenIDOutput / BatchStrOutput
   ▼ IPC
TokenizerManager ReqState
   ▼ protocol serializer
JSON / SSE
```

进程间不能依赖“两个进程恰好持有同一个 Python 对象”。因此消息必须能够序列化，且要携带足够的 request id、采样信息、回程路由和结果元数据。

### 10.2 大字段为什么会走 shared memory

多模态输入、hidden states、logprob 数组或其他大对象如果全部通过普通 pickle 复制，会产生：

- CPU 内存拷贝；
- 序列化/反序列化 CPU 开销；
- IPC 队列中的峰值内存；
- 多 worker 模式下的重复拷贝。

因此请求 dispatch 前会对可传输 feature 进行准备，并把字段包装成共享内存引用。接收方再 unwrap。小的控制字段仍然可以普通序列化，大的 tensor-like payload 采用专门 transport。

这体现了一个常见原则：

```text
控制信息适合消息复制
大数据适合共享/注册后传输
```

### 10.3 ZMQ socket 的 bind/connect 方向

从代码可以读出典型方向：

```text
TokenizerManager: PUSH + bind scheduler input? / PULL + bind tokenizer output
Scheduler:        PULL/DEALER + connect; PUSH + connect
Detokenizer:      PULL + bind; PUSH + connect
```

具体 bind/connect 由 `get_zmq_socket` 的参数决定，但阅读代码时要关注两个维度：

1. socket 类型决定消息方向和连接拓扑；
2. `bind`/`connect` 决定哪一端拥有 endpoint。

不要只看到 `PUSH` 就推断它一定是服务端；ZeroMQ 的 socket 角色和 TCP server/client 不是同一概念。

---

## 11. 一次生成请求的完整 IPC 时间线

假设客户端请求：

```json
{
  "rid": "req-42",
  "text": "解释 KV cache",
  "stream": true,
  "max_new_tokens": 3
}
```

### 阶段 A：HTTP → TokenizerManager

```text
FastAPI /generate
  ▼
GenerateReqInput(rid=req-42)
  ▼
TokenizerManager 创建 ReqState
```

这一段在主进程内，HTTP handler 通过 async generator 等待输出。

### 阶段 B：TokenizerManager → Scheduler

```text
text
  ▼ tokenizer
input_ids = [ ... ]
  ▼
TokenizedGenerateReqInput
  ▼ ZMQ PUSH
Scheduler rank 0 的 PULL
```

消息中还可能有 sampling params、stop strings、grammar、multimodal metadata 和 `http_worker_ipc`。

### 阶段 C：Scheduler 放入队列并形成 batch

```text
PULL 收消息
  ▼
创建 Scheduler Req
  ▼
waiting queue
  ▼
prefix match / KV admission
  ▼
ScheduleBatch
```

这时 HTTP handler 仍然没有拿到输出；它只是在等待与 `req-42` 对应的状态事件。

### 阶段 D：Scheduler → GPU

```text
ScheduleBatch
  ▼
ForwardBatch
  ▼
ModelRunner
  ▼
next_token_ids = [token_1, ...]
```

Scheduler 记录每个 row 对应哪个 `rid`。

### 阶段 E：Scheduler → Detokenizer

```text
BatchTokenIDOutput(
    rids=["req-42"],
    output_ids=[[token_1]],
    finished_reasons=[None],
)
  ▼ ZMQ PUSH
Detokenizer PULL
```

### 阶段 F：Detokenizer → TokenizerManager

```text
token_1
  ▼ incremental decode
BatchStrOutput(
    rids=["req-42"],
    output_strs=["..."],
)
  ▼ ZMQ PUSH
TokenizerManager PULL
```

### 阶段 G：TokenizerManager → HTTP client

```text
rid=req-42
  ▼ rid_to_state
唤醒 async generator
  ▼
SSE data: {...}
```

下一轮 decode 重复 C-G，直到 Scheduler 发出 finish reason。最后 HTTP 层发送协议需要的结束事件。

---

## 12. 错误、退出和 watchdog

### 12.1 子进程异常不能只打印日志

Scheduler 入口捕获异常后会记录 traceback，并通知父进程：

```python
except Exception:
    traceback = get_exception_traceback()
    logger.error(f"Scheduler hit an exception: {traceback}")
    parent_process.send_signal(signal.SIGQUIT)
```

因为 Scheduler 掌握 GPU 状态，如果它已经异常，主进程继续接受请求通常是不安全的。父进程需要停止 HTTP、清理其他 worker，并避免客户端无限等待。

### 12.2 父进程死亡时子进程也应退出

Detokenizer 入口首先调用：

```python
kill_itself_when_parent_died()
```

否则主进程被杀掉后，detokenizer 可能成为孤儿进程，继续占用 socket、内存和 CPU。CLI 退出时还会执行 process-tree cleanup，形成双向保护：

```text
父进程退出 → 子进程自杀
子进程异常 → 通知父进程并结束服务
```

### 12.3 watchdog 观察的是消息活跃度

TokenizerManager 和 DetokenizerManager 在阻塞接收前暂时 disable watchdog，收到消息后更新时间并 feed：

```python
with self.soft_watchdog.disable():
    recv_obj = await async_sock_recv(...)
...
self.last_receive_tstamp = real_time()
self.soft_watchdog.feed()
```

阻塞在 socket 上本身不是故障；真正要观察的是进程是否长期没有处理任何消息，或者调度/输出链路是否卡死。

---

## 13. 与 Ray、Rust server 的关系

标准多进程路径不是唯一部署形态。

### 13.1 Ray

当启用 Ray 时，Scheduler 可能作为 Ray actor 运行，而不是由主进程直接 `mp.Process` 创建。这里变化的是进程/节点编排方式，不是请求语义：仍然存在 tokenizer、scheduler、GPU worker 和结果回传的职责边界。

### 13.2 Rust server

`launch_server` 中如果启用 `SGLANG_RUST_SERVER`，Rust server 会承担 api-server、tokenizer 和 detokenizer，主 Python 进程不再运行 Python HTTP server/TokenizerManager。Python Scheduler/GPU 路径仍通过相应的 IPC 或 Rust bridge 协作。

因此读代码时要先确认部署分支：

```text
默认 HTTP → Python FastAPI + Python tokenizer/detokenizer
Ray       → Ray 编排的 worker
Rust      → Rust api/tokenizer/detokenizer
```

不能把标准 Python 路径中的某个对象强行套到所有部署模式。

---

## 14. 小结：IPC 设计的五条主线

### 主线一：职责分离

```text
HTTP/TokenizerManager → 网络和请求生命周期
Scheduler             → admission、batch 和控制循环
GPU worker/ModelRunner → 张量计算
Detokenizer           → 增量文本转换
```

### 主线二：长连接消息传递

ZMQ PUSH/PULL 传输持续的 request/output/control 消息，`mp.Pipe` 主要传启动 ready 信息。

### 主线三：地址集中分配

`PortArgs` 把所有 endpoint 统一计算并传给进程，避免每个组件自行猜测拓扑。

### 主线四：批消息带 request identity

Scheduler 面向 batch 工作，但输出带有 `rids`，TokenizerManager 通过 `rid_to_state` 把结果重新拆回 API 请求。

### 主线五：每个进程先 publish 有效配置

子进程启动时先 `publish(server_args, role=...)`，再构造 manager 或 Scheduler；配置 bags 是进程内业务代码的读取入口。

把这几条主线和前面章节连起来，一次请求就变成：

```text
HTTP
  → 主进程 TokenizerManager
  → ZMQ
  → Scheduler / GPU
  → ZMQ
  → Detokenizer
  → ZMQ
  → 主进程请求状态
  → SSE / JSON
```

下一步阅读模型加载和并行初始化时，可以继续沿用本章的方法：先问“这个对象在哪个进程”，再问“它通过哪条消息或分布式通信得到输入”，最后才看它如何使用 GPU。

## 相关文档
- [项目入口](../README.md)
- [分析状态](../00-overview/analysis-state.md)
- [源码证据索引](../00-overview/evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
