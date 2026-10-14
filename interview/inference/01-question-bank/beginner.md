# 推理方向：入门级面试题

- 题数：15
- 稳定 ID：`I-B001`–`I-B015`
- 证据规则：源码/文档结论区分 C/I；GPU、模型、多卡、网络、多进程和性能实验标为 `U / 待验证`。
- 单题门槛：元数据、背景知识、问题分析、具体答案、技术洞察、图文说明、拓展知识、追问、易错点、实践、评分、证据。

### I-B001 大模型在线推理服务中，prefill 阶段与 decode 阶段的计算特征有何不同？为什么同一模型在两个阶段的吞吐与延迟瓶颈截然不同？

**元数据：** 知识域：`I06 prefill/decode 与 ForwardBatch`；能力目标：`C06 双阶段差异`；题型：概念解释与辨析；频率：高频；前置/后续：`I-B002`（batch 调度）、`I-I004`（continuous batching 机制）；证据状态：源码/文档 C，运行结论 U。

**背景知识：** 自回归推理把生成分为两阶段：prefill 一次性处理完整 prompt（计算量正比于 prompt 长度，可做并行矩阵乘法），decode 每步只产生一个 token（计算量固定但访存正比于 KV cache 总量）。SGLang 用 `ForwardBatch` 统一接口，但通过 `extend`（prefill）与 `decode` 两个模式走不同 kernel 路径 `sglang/02-request-flow/03-调度器与连续批处理.md`。两阶段的算术强度差异决定了前者常是 compute-bound、后者常是 bandwidth-bound。

**问题分析：** 1) 先给出两阶段的定义与划分边界；2) 从算术强度（FLOPs/byte）推导为何 prefill 偏 compute、decode 偏 bandwidth；3) 说明这对 batching 策略的影响——prefill 可以大 batch 摊薄固定开销，decode 受显存与带宽限制 batch 不能无限大。

**具体答案：** 面试中可以这样回答：

> prefill 阶段一次性把 prompt 所有 token 并行送入模型做 attention 与 FFN，主要计算是大矩阵乘法 GEMM，算术强度（FLOPs 每 byte 访存）高，通常落在 roofline 的 compute-bound 区。decode 阶段每步只生成一个 token，计算量固定为一次模型前向，但需要把不断增长的 KV cache 从显存读回——当序列变长，KV cache 的读取量线性增长而计算量不变，算术强度持续下降，最终变成 bandwidth-bound。
>
> 这直接解释了为什么 decode 的瓶颈常在显存带宽：一个 70B 模型 decode 时，每步读取的 KV cache 可达数十 MB，而每步新增计算仅一次前向。prefill 则相反——长 prompt 意味着大 GEMM，计算占主导。

**技术洞察：**

```text
prefill 算术强度 I_prefill ≈ FLOPs_prefill / bytes_read ∝ prompt_len  (随长度增长)
decode 算术强度 I_decode ≈ FLOPs_decode / KV_cache_bytes ∝ 1/seq_len  (随长度下降)

→ prefill 越过 ridge point 容易落在 compute 区
→ decode 几乎总是 bandwidth-bound（长序列尤甚）

ridge point I* = FLOPs_peak / Bandwidth_peak   # 硬件算力带宽比
```

C：`sglang/02-request-flow/03-调度器与连续批处理.md` 的 ForwardBatch/extend vs decode 双路径描述。I：上式的 roofline 推导。U：具体 ridge point 数值与实测分界线因型号/模型/量化方案而异。

**图文说明：** 横轴算术强度 I、纵轴吞吐的 roofline 图上标两点：prefill 在高 I 侧（右侧，compute-bound），decode 在低 I 侧（左侧，带宽斜坡）。箭头表示序列增长时 decode 点沿斜坡向左下方移动。图示不能给出具体阈值——实测需用 profiler 对不同 prompt/decode 长度分布采样。

**拓展知识：** 与 I08 衔接：KV cache 容量（M08-kv-cache）决定了 decode 阶段能服务的最大并发序列数；与 I13 衔接：PD 分离把 prefill 和 decode 拆到不同节点，正是为了分别优化 compute 与 bandwidth。

**常见追问及回答要点：**

1. **问：长 prompt 场景下 prefill 反而更慢，为什么不用更大的 batch？** 答：prefill 的 batch 受显存限制（每个请求的中间激活随 prompt 长度增长），且 prefill 本身是 compute-bound，batch 增大虽摊薄固定开销但总计算量线性增长——收益递减。
2. **问：decode 能否也通过增大 batch 提升吞吐？** 答：能提升但有限。decode 是 bandwidth-bound，batch 增大使 KV cache 总读取量线性增长，最终撞上显存带宽墙。收益拐点取决于模型大小与硬件带宽。
3. **问：量化对两阶段的影响一样吗？** 答：不一样。权重量化降低两阶段的计算与显存压力，但 decode 更受益（因为 KV cache 的 bytes 也减少）；KV 量化（I12）直接影响 decode 带宽瓶颈。

**易错点与反例：**

- 反例："decode 慢是因为计算量太大"——错误。decode 每步计算量固定（一次前向），慢的原因是 KV cache 访存随序列增长，瓶颈在带宽而非算力。
- 反例："prefill 可以用 continuous batching 像 decode 一样持续交错"——有误。prefill 虽然可被调度器交错到 batch 中，但其 GEMM 大矩阵特性意味着长 prefill 会独占计算资源，挤压同 batch 其他请求的 decode 进度。这就是为什么要限制 prefill 预算（`PrefillAdder`）。
- 易错：混淆"batch 吞吐高"与"单请求延迟低"——大 batch 提升总吞吐但可能增加单请求的排队与首 token 延迟（TTFT），需区分优化目标。

**实践与验证：** [U] 固定模型（如 Llama-3-8B）与硬件，分别测量不同 prompt 长度下的 prefill 吞吐（tokens/s）与不同并发下的 decode 吞吐，绘制 roofline 上的散点图。控制温度、时钟与后台负载。oracle：prefill 点应落在 roofline 平坦区或接近峰值，decode 点落在带宽斜坡上。当前未执行 GPU 与模型实验。

**分层评分标准：** 2分：能区分 prefill 与 decode 的阶段定义；4分：能推导两阶段的算术强度差异并关联 roofline；6分：能分析 batch 对两阶段的非对称收益、结合 KV cache 与量化讨论优化方向。

**证据与来源：** `sglang/02-request-flow/03-调度器与连续批处理.md`（ForwardBatch/extend/decode 双路径）；`sglang/99-roadmap/performance-guide.md`（分阶段测量法）；`cuda/99-roadmap/performance-guide.md`（roofline 模型）。

### I-B002 [I04] admission机制如何决定哪些新请求可以加入当前batch？拒绝时请求会怎样？

**元数据：** 知识域：`I04 scheduler与continuous batching`；能力目标：`C07 admission约束体系`；题型：机制解释；频率：高频；前置/后续：`I-B001`（双阶段差异）、`I-B005`（static vs continuous）；证据状态：源码 C，运行结论 U。

**背景知识：** SGLang 的 `PrefillAdder` 是 admission 的核心守卫。它在 `get_new_batch_to_run` 中被构造，综合 token budget（`max_prefill_tokens`）、KV allocator 剩余容量、running batch 已占用空间、`max_running_requests`、`page_size`、chunked prefill 上限以及 attention backend tile 约束，对 waiting queue 中按策略（LPM/FCFS/HRRN 等）排序后的请求逐个调用 `add_one_req`。`add_one_req` 计算本轮需要扩展的 token 数、KV page 开销与混合 SWA/Mamba 预算；预算不足时返回 `NO_TOKEN`，不再继续取请求 `sglang/01-modules/M04-scheduler-batching/README.md`。队列满时 `_abort_on_queued_limit` 拒绝新请求或按优先级淘汰旧请求 `sglang/02-request-flow/03-调度器与连续批处理.md:15.1`。

**问题分析：** 1) 梳理 admission 的多维约束——不是只看请求数，还受 token budget、KV 容量、running 占用共同限制；2) 说明拒绝后的两种路径：尚未进入队列的请求被服务端拒绝（HTTP 503 / service unavailable），已在 waiting 中的请求则继续等待下一轮调度；3) 区分 queue-full 拒绝与 decode OOM retract——前者发生在请求进入系统前，后者发生在已经运行后。

**具体答案：** 面试中可以这样回答：

> admission 由 `PrefillAdder` 执行，每一轮调度时它按调度策略从 waiting queue 取候选请求，逐个检查 token budget、KV page 容量、running batch 占用的空间以及 `max_running_requests` 等约束。只有全部约束都满足的请求才放入 `can_run_list`，成为本轮 prefill batch。预算耗尽时 `add_one_req` 返回 `NO_TOKEN`，adder 停止遍历，剩余请求留在 waiting queue 等下一轮。
>
> 尚未进入 waiting queue 的请求（即 queue 已满时）由 `_abort_on_queued_limit` 直接拒绝，通过 tokenizer IPC 返回 service unavailable；若启用了 priority scheduling，更高优先级的请求可以淘汰 waiting 中已有的低优先级请求。已经在 running 但 KV 不足时走的不是 admission 而是 `retract_decode`——撤回后请求被重新放入 waiting queue，而不是直接丢弃。

**技术洞察：**

```text
admission 约束链（按 PrefillAdder 检查顺序）：
  1.  running_bs < max_running_requests？
  2.  token_budget_left ≥ extend_tokens_of_this_req？
  3.  KV allocator 有足够 page 容纳 prefix + 扩展？
  4.  chunked_prefill_size 上限？
  5.  PP micro-batch / LoRA / beam width 附加约束？
任一不满足 → NO_TOKEN → batch_is_full → 停止加入更多请求
```

C：`sglang/01-modules/M04-scheduler-batching/README.md` 的 PrefillAdder 章节；`sglang/02-request-flow/03-调度器与连续批处理.md:15.1` 的队列满处理。U：具体 `max_prefill_tokens` 与 KV pool 比例在不同部署中不同。

**图文说明：** 画 waiting queue 从左到右按策略排序，顶部标注 `max_prefill_tokens=4096`、`max_running_requests=3` 等预算条。`PrefillAdder` 逐个扫描：前两个请求通过约束进入 batch，第三个请求触发 `NO_TOKEN`，之后的所有请求即使实际资源可能足够也停止扫描。右侧独立画出 queue-full 拒绝路径（不进队列）与 retract 路径（从 running batch 退回 waiting）。

**拓展知识：** 与 I08 衔接：KV pool 容量（M08-kv-cache）直接决定 admission 能放多少并发请求；与 I02 衔接：被拒绝的请求在 TokenizerManager 的 `ReqState` 尚未 dispatch 前可以直接删除本地状态；与 I13 衔接：PD 分离把 admission 拆到不同节点。

**常见追问及回答要点：**

1. **问：为什么遇到第一个无法调度的请求就停止？后面可能有更小的请求可以放入。** 答：因为 waiting queue 已经按调度策略排序（如 LPM/FCFS），跳过前面的请求选择后面的会破坏调度语义（如公平性、cache 亲和性）。如果需要更紧凑的 packing 则应从调度策略层面优化，而不是在 admission 处跳选。
2. **问：prefix cache 命中率高时 admission 行为会有什么变化？** 答：prefix 命中高意味着 `extend_range` 小，单个请求消耗的 token budget 和 KV page 更少，admission 能在相同预算内容纳更多请求，提升并行度。
3. **问：如果所有 waiting 请求都因 KV 不足被拒绝，running batch 会空转吗？** 答：不会。admission 失败后 Scheduler 会转入 decode 分支继续为 running batch 服务，GPU 仍有 decode batch 可执行。

**易错点与反例：**

- 反例："admission 只看请求数"——错误。token budget、KV capacity、running 占用共同决定，一个 4096-token 的 prompt 可能占满整个 `max_prefill_tokens` 预算。
- 反例："请求被 admission 拒绝就丢了"——错误。未入 queue 的请求才直接拒绝；已在 waiting queue 的请求只是本轮未选中，继续等待下一轮调度。
- 易错：混淆 admission 与 retract——admission 是"能不能进来"，retract 是"已经运行时资源不足请出去"，触发条件和执行对象不同。

**实践与验证：** [U] 固定模型与 workload，统计不同 `max_prefill_tokens` 和 `max_running_requests` 配置下的 admission 拒绝率与平均等待轮次。可通过 scheduler metrics 观察 `batch_is_full` 触发频率和 waiting queue 长度分布。当前未执行 GPU 实验。

**分层评分标准：** 2分：能说 admission 由 PrefillAdder 按预算决定；4分：能列出多维约束（token/KV/running/queue），区分 queue-full 拒绝与 retract；6分：能分析停止遍历的设计理由、prefix cache 对 admission 吞吐的影响，以及 priority scheduling 下的淘汰策略。

**证据与来源：** `sglang/01-modules/M04-scheduler-batching/README.md`（PrefillAdder/add_one_req/NO_TOKEN）；`sglang/02-request-flow/03-调度器与连续批处理.md`（§7 PrefillAdder、§15.1 队列满）。

### I-B003 [I02] 推理服务中一个请求的 ReqState 从创建到终止经历哪些状态？什么条件下提前终止？

**元数据：** 知识域：`I02 协议、tokenizer与请求状态`；能力目标：`C08 ReqState生命周期`；题型：状态机描述；频率：高频；前置/后续：`I-B002`（admission）、`I-B004`（tokenizer职责）；证据状态：源码 C，运行结论 U。

**背景知识：** 在 SGLang 中，请求生命周期跨越两个进程边界。主进程 TokenizerManager 维护 `ReqState`（`tokenizer_manager.py:224-269`），保存 `out_list`、`finished`、`event`、`dispatched`、`text_chunks` 等字段；Scheduler 子进程维护 scheduler-side `Req`（`schedule_batch.py:926-1018`），保存 `origin_input_ids`、`output_ids`、`prefix_indices`、KV 信息和 finish reason。`ReqState` 的 `dispatched` 标志区分"尚未发往 scheduler"和"已发往 scheduler"——前者失败可直接删除本地状态，后者失败必须通知 scheduler abort `sglang/01-modules/M03-tokenizer-request-state/README.md`。

**问题分析：** 1) 按时间顺序描述 `ReqState` 从 `generate_request` → `_init_req_state` → tokenize → dispatch → 等待 output → finished 的完整链路；2) 重点说明 `dispatched` 标志的双路径清理语义；3) 列出提前终止条件：用户主动 abort、HTTP 断连、scheduler 侧 retract-abort、max_new_tokens 达到、EOS token、stop string 命中。

**具体答案：** 面试中可以这样回答：

> 一个请求的 `ReqState` 生命周期大致分五段：
>
> 1. **创建**：`generate_request` 调用 `_init_req_state`，在 `rid_to_state` 中注册一条新记录，此时 `dispatched=False`、`finished=False`；
> 2. **tokenize + dispatch**：经过 `_tokenize_one_request` 和 `_create_tokenized_object` 后，`_send_one_request` 把 `TokenizedGenerateReqInput` 推送给 scheduler，并把 `dispatched=True`；
> 3. **等待执行**：scheduler 侧 `Req` 经历 waiting → prefill → running decode 的流转，中间每产生一批 token 就通过 `BatchStrOutput` 回写增量；
> 4. **增量汇聚**：`_handle_batch_output` 把文本/ids 追加到 `ReqState.text_chunks` 与 `state.output_ids`；
> 5. **完成/终止**：`finished=True` 后删除 `rid_to_state[rid]`，构造最终 output dict 并通过 `event.set()` 唤醒调用者。
>
> 提前终止分两类：一是调用侧主动触发（用户 abort 或 HTTP 断连），TokenizerManager 发 `AbortReq` 给 scheduler；二是模型侧自然终止（EOS、max_new_tokens、stop string、grammar termination），或资源不足被 scheduler retract-abort。

**技术洞察：**

```python
# ReqState 关键字段（简化）
@dataclasses.dataclass
class ReqState:
    out_list: List[Dict]
    finished: bool           # 终止标志
    event: asyncio.Event     # 唤醒调用者
    dispatched: bool         # 双路径清理的分界
    abort_sent: bool
    text: str                # get_text() 合并 text_chunks
```

C：`sglang/01-modules/M03-tokenizer-request-state/README.md` 的 ReqState 定义与双路径清理。I：`dispatched=True` 后失败必须发 abort 的推导。U：实际终止原因分布依赖具体部署与 workload。

**图文说明：** 画出 `ReqState` 状态机：创建 → (dispatched=False) → dispatch → (dispatched=True) → [running via scheduler] → finished=True → 删除。两条提前终止边：① 从任意状态到 abort（调用侧触发）；② 从 running 到 retract-abort（资源不足）。标注 `dispatched` 标志两侧的清理差异：左分支直接 `del rid_to_state[rid]`，右分支发 `AbortReq`。

**拓展知识：** 与 I04 衔接：scheduler-side `Req` 的 finish reason 最终通过 `BatchStrOutput.finished_reasons` 回传给 `ReqState`；与 I17 衔接：graceful shutdown 时需要等待所有 `rid_to_state` 清空才能退出。

**常见追问及回答要点：**

1. **问：`dispatched=False` 和 `dispatched=True` 的失败清理为什么不同？** 答：`dispatched=False` 时请求尚未到达 scheduler，scheduler 不知道该 rid 存在，因此只需删除本地状态；`dispatched=True` 后 scheduler 已持有该 `Req`，必须显式发 abort 让 scheduler 释放 KV 等资源。
2. **问：HTTP 断连后请求会立刻停止计算吗？** 答：不会立刻停止。断连后 `StreamingResponse.background` 触发 `create_abort_task`，向 scheduler 发送 `AbortReq`，scheduler 在安全阶段（`to_finish` 状态）清理，不是从 CUDA forward 中途删除。
3. **问：一个请求的 `ReqState` 和 scheduler 侧的 `Req` 是同一个对象吗？** 答：不是。`ReqState` 在主进程，`Req` 在 scheduler 子进程，通过 `rid` 和 ZMQ 消息关联。二者字段名可能相似但生命周期和归属进程完全不同。

**易错点与反例：**

- 反例："请求完成就是模型产生了 EOS"——不全面。完成原因还包括 max_new_tokens 达到、stop string 命中、用户 abort、retract-abort、grammar termination 等多种路径。
- 反例："ReqState.finished=True 就可以安全删除"——需要注意与 scheduler 侧的竞态：可能 abort echo 和完成消息同时到达，TokenizerManager 需要处理"abort echo 已被完成路径删除"的情况。
- 易错：混淆主进程 `ReqState` 与 scheduler 进程 `Req`——二者跨进程分布，不能通过 Python 对象引用共享。

**实践与验证：** [U] 通过压测观察不同 finish reason 的比例分布（正常完成 vs abort vs retract）。可在 TokenizerManager 侧埋点记录 `ReqState` 从创建到删除的平均生命周期时长。当前未部署真实服务。

**分层评分标准：** 2分：能描述创建→dispatch→running→完成的主干；4分：能区分 `dispatched` 双路径清理、列出≥3种提前终止条件；6能：能解释主进程 ReqState 与 scheduler Req 的跨进程关系、abort echo 竞态、以及 HTTP 断连的传播链路。

**证据与来源：** `sglang/01-modules/M03-tokenizer-request-state/README.md`（ReqState 字段、M03-FLOW-MAIN-001、§6.1 双路径清理）；`sglang/02-request-flow/02-请求进入与分发.md`（GenerateReqInput→Req 转换）。

### I-B004 [I02] tokenizer 在推理服务中扮演什么角色？为什么服务端需要自己管理 tokenizer 而非交给客户端？

**元数据：** 知识域：`I02 协议、tokenizer与请求状态`；能力目标：`C09 tokenizer服务化理由`；题型：设计决策分析；频率：中频；前置/后续：`I-B003`（ReqState 生命周期）；证据状态：源码 C，运行结论 U。

**背景知识：** SGLang 的 `TokenizerManager` 承担三类职责：文本/token id 转换（`_tokenize_one_request`）、跨进程 IPC（`_send_one_request`/`handle_loop`）和响应汇聚（`_handle_batchOutput`/`_stream_one_request`）。它同时处理 `input_embeds` 输入、多模态 processor 预处理、批量 tokenization（`_should_use_batch_tokenization`）三种路径 `sglang/01-modules/M03-tokenizer-request-state/README.md`。服务端自管 tokenizer 的核心原因是：(1) tokenizer 与模型版本强绑定，客户端可能使用不兼容版本；(2) 服务端需要精确的 token 数以计算 admission budget 和 KV 分配；(3) 增量 detokenization 需要在服务端维护 decode 状态以避免跨 token 切分错误。

**问题分析：** 1) 列举 tokenizer 在推理服务中的完整角色——不止是"文本转 token"；2) 从一致性、资源管理和 detokenization 正确性三个角度论证服务端管理的必要性；3) 简要说明如果交给客户端做 tokenize 会引入什么问题。

**具体答案：** 面试中可以这样回答：

> TokenizerManager 在推理服务中实际承担四个角色：
>
> 1. **输入转换**：把 text、input_ids 或 input_embeds 统一成 scheduler 可消费的 `TokenizedGenerateReqInput`，并做长度校验；
> 2. **sampling 参数校验**：在 `_create_tokenized_object` 中对 `SamplingParams` 做 normalize 和 verify，确保 temperature/top-p 等参数合法；
> 3. **IPC 发送**：通过 ZMQ PUSH socket 把 tokenized request 发给 scheduler；
> 4. **响应汇聚**：收到 `BatchStrOutput` 后按 `rid` 找到本地 `ReqState`，累积增量文本与 token ids，完成时唤醒调用者。
>
> 服务端必须自管 tokenizer 的原因：第一，tokenizer 与模型一一绑定（vocab 大小、special tokens、chat template），客户端版本不一致会导致 token id 语义偏移，服务端无法正确计算 admission 需要的 token 数和 KV 空间；第二，增量 detokenization 需要维护 decode 状态（如跨 token 的 BPE 合并），如果客户端做 tokenize 而服务端只做 detokenize，流式输出中的中间状态会出错；第三，服务端需要精确的 token 长度来执行 prefix cache 匹配和 chunked prefill 切分。

**技术洞察：**

```text
服务端 tokenizer 的核心不可替代性：
  ① token count → admission budget (max_prefill_tokens)
  ② token count → KV slot allocation (PagedTokenToKVPoolAllocator)
  ③ decode state → incremental detokenization (BPE/merge boundary)
  ④ chat template → prompt 结构一致性 (system + user + assistant prefix)
  ⑤ multimodal → MM processor 覆盖 input_ids

客户端自行 tokenize 的风险：
  - tokenizer 版本不一致 → 同文本产生不同 id → 模型语义错误
  - 服务端无法预知长度 → admission/KV 规划失准
  - 失去 prefix cache 匹配前提（需要精确 token 序列）
```

C：`sglang/01-modules/M03-tokenizer-request-state/README.md` 的职责表（§1.1）。I：③增量 detokenization 需要 decode 状态的推导。U：不同 tokenizer 库（transformers/tokenizers/sentencepiece）的具体行为差异需实测验证。

**图文说明：** 画 TokenizerManager 作为"前台+分拣中心"的示意图：左侧接收 text/input_ids，中间经过 normalize → tokenize → create object → IPC，右侧从 detokenizer 收 `BatchStrOutput` 汇聚到 `ReqState`。用红色标注四个"不可外包给客户端"的关键点：admission 预算计算、KV 分配、decode 状态维护、chat template 一致性。

**拓展知识：** 与 I06 衔接：prefill 处理的 token 数直接取决于 tokenizer 输出长度；与 I16 衔接：tokenizer 耗时是 TTFT 的组成部分之一。

**常见追问及回答要点：**

1. **问：客户端传 `input_ids` 跳过 tokenizer 可以吗？** 可以，`_tokenize_one_request` 会检测 `input_ids` 存在时跳过 tokenize。但服务端仍需校验长度和构造 `SamplingParams`，且失去 prefix cache 匹配能力（因为 radix match 依赖 token 序列精确性）。
2. **问：多模态输入时 tokenizer 做什么？** 多模态数据经 multimodal processor 处理后可能覆盖或扩充 input_ids（如图像 token 占位符），tokenizer 对纯文本部分正常分词，最终合并成完整的 `TokenizedGenerateReqInput`。
3. **问：skip_tokenizer_init 模式是什么？** 当不需要 tokenizer/detokenizer 时（如 embedding 服务或外部已 tokenize 的场景），Scheduler 的 `send_to_detokenizer` 直接连到 `tokenizer_ipc_name`，跳过 detokenizer 进程，输出直接回 TokenizerManager。

**易错点与反例：**

- 反例："tokenizer 就是把字符串变数字"——过于简化。它还包括 sampling 参数校验、多模态融合、decode 状态维护、IPC 发送/接收等。
- 反例："客户端 tokenize 可以减少服务端计算"——实际会引入版本不一致风险，且服务端仍需要完整 token 信息做资源规划。
- 易错：混淆 TokenizerManager（主进程）和 DetokenizerManager（子进程）——前者管 tokenize + 状态汇聚，后者管 token ids 到增量文本的转换。

**实践与验证：** [U] 固定模型与 prompt 集合，对比客户端 tokenize + 传 input_ids 与服务端 tokenize 的 token 序列一致性。重点测试特殊 token、多语言字符和 chat template 处理。当前未执行端到端对比。

**分层评分标准：** 2分：能说出 tokenize + IPC + 响应汇聚三个基本职责；4分：能从一致性/detokenization/资源规划三个角度论证服务端管理的必要性；6分：能讨论 input_ids 旁路路径的 trade-off、多模态扩展、以及 skip_tokenizer_init 的设计动机。

**证据与来源：** `sglang/01-modules/M03-tokenizer-request-state/README.md`（§1.1 职责表、§6.2 tokenization 分支）；`sglang/02-request-flow/02-请求进入与分发.md`（三对象区分：GenerateReqInput / TokenizedGenerateReqInput / Req）。

### I-B005 [I04] continuous batching 与静态 batching 在吞吐与延迟上有何本质区别？

**元数据：** 知识域：`I04 scheduler与continuous batching`；能力目标：`C10 batching策略对比`；题型：对比分析；频率：高频；前置/后续：`I-B001`（双阶段差异）、`I-B002`（admission）；证据状态：源码 C，运行结论 U。

**背景知识：** 静态 batching 把一组请求打包成一个 batch，等所有请求完成才释放 GPU——一个长请求会让同 batch 其他请求全程等待。continuous batching（如 Orca 论文提出的方案）在每轮调度时重新组成 batch：已完成请求即时释放，新到达请求在下一轮即可加入，prefill 与 decode 可以在相邻轮次交错执行 `sglang/02-request-flow/03-调度器与连续批处理.md:1`。SGLang 的 `event_loop_normal` 每轮调用 `get_next_batch_to_run` 重新决策 batch 构成 `sglang/01-modules/M04-scheduler-batching/README.md`。

**问题分析：** 1) 定义两种 batching 的核心差异——静态 batch 是一次性打包后不再变化，continuous batch 是每轮重新组 batch；2) 从排队延迟和 GPU 利用率两个角度分析延迟与吞吐差异；3) 指出 continuous batching 不修改正在执行的 kernel，而是通过"下一轮重新组 batch"实现动态性。

**具体答案：** 面试中可以这样回答：

> 静态 batching 一次取 N 个请求打包成一个 batch，全部完成后才释放 GPU。如果一个请求要生成 1000 个 token，其他 7 个请求即使已经生成完毕也要等它——GPU 在等最慢的请求，排队延迟被尾部延迟主导。
>
> continuous batching 每一轮都重新决定 batch 构成：一个请求完成后立刻释放资源，新到达的请求在下一轮就能加入 prefill 或 decode。GPU 不等最慢请求，吞吐接近各请求独立的解码速率之和。
>
> 关键区别是：continuous batching 不会修改已经发射的 CUDA kernel——动态性来自"每一轮调度决策时重新拼 batch"，而不是运行时修改 batch。

**技术洞察：**

```text
静态 batch:
  batch = [r1, r2, ..., r8]  →  forward → 等待最慢完成 → 释放
  吞吐瓶颈 = max(latency_i)，GPU 在等待时空转

continuous batching (SGLang):
  loop:
    每轮 get_next_batch_to_run() → 动态组 batch
    完成请求即时 filter_out → 新请求即时加入
  吞吐 ≈ Σ(decode_rate_i)，GPU 几乎无空闲等待
```

C：`sglang/02-request-flow/03-调度器与连续批处理.md:1` 的餐厅类比与连续批处理定义；`sglang/01-modules/M04-scheduler-batching/README.md` 的 normal event loop。U：具体吞吐提升倍数取决于 workload 的请求长度分布。

**图文说明：** 画时间线对比：静态 batching 上排显示 8 个请求被框在同一 batch 中，右侧短请求完成后仍留空白等长请求；continuous batching 下排显示每轮 batch 动态变化，完成的请求（灰色）即时退出，新请求（绿色）即时加入。两排底部标注 GPU 利用率（静态 batch 有明显空闲段，continuous batch 接近满利用率）。

**拓展知识：** 与 I06 衔接：continuous batching 中 prefill 和 decode 如何交错进入同一轮 batch 取决于 `is_mixed_chunk` 配置；与 I16 衔接：continuous batching 的 TTFT 受 prefill budget 与排队轮次影响。

**常见追问及回答要点：**

1. **问：continuous batching 能保证公平性吗？** 不能完全保证。如果一直有短请求到达，长请求的 prefill 可能持续被延迟（饥饿）。SGLang 通过 HRRN 策略和 `processed_tokens_counter` 做老化处理来缓解。
2. **问：static batch 完全没优势吗？** 静态 batch 实现简单、确定性高，在离线批量推理（所有请求同时到达、长度相近）场景下反而更高效，因为没有调度开销。
3. **问：mixed chunk 是什么？** mixed chunk 允许同一轮 forward 中同时包含 prefill（extend）和 decode 请求。条件满足时 running batch 的 decode 请求与新的 prefill batch 合并执行，减少 kernel launch 间隙。

**易错点与反例：**

- 反例："continuous batching 是把新请求插入正在执行的 kernel"——错误。它是在当前 forward 完成后、下一轮调度决策时加入新 batch，不是修改正在执行的 CUDA kernel。
- 反例："continuous batching 的延迟一定比静态 batch 低"——不一定。continuous batching 的 TTFT 受排队轮次和 prefill budget 影响，若 admission 被频繁拒绝，排队时间可能更长。
- 易错：混淆 batching 策略与调度策略——continuous batching 是"每轮重新组 batch"的调度范式，LPM/FCFS/HRRN 是在 waiting queue 内的排序策略，二者正交。

**实践与验证：** [U] 固定模型与混合长度 workload，对比 static batch（固定 batch size 等待全部完成）与 continuous batch 在 TTFT、吞吐和 GPU 利用率上的差异。需要控制到达率、长度分布和硬件配置。当前未执行。

**分层评分标准：** 2分：能说 static batch 等最慢、continuous batch 每轮重组；4分：能从排队延迟、GPU 利用率、kernel 发射层面分析差异；6分：能讨论公平性/饥饿问题、mixed chunk 的作用、以及 static batch 在离线场景的适用性。

**证据与来源：** `sglang/02-request-flow/03-调度器与连续批处理.md`（§1 餐厅类比、§4 普通循环）；`sglang/01-modules/M04-scheduler-batching/README.md`（normal event loop 结构）。

### I-B006 [I04] 什么是 batch 内请求的"同步退出"问题？continuous batching 如何解决？

**元数据：** 知识域：`I04 scheduler与continuous batching`；能力目标：`C11 同步退出问题与解法`；题型：问题-解法分析；频率：中频；前置/后续：`I-B005`（continuous batching）、`I-B010`（KV cache）；证据状态：源码 C，运行结论 U。

**背景知识：** "同步退出"问题指静态 batching 中同 batch 的所有请求必须一起结束——一个请求完成时不能单独退出，必须等 batch 内最慢的请求完成才能释放 GPU 资源。continuous batching 通过每轮 `filter_batch` 即时移除已完成的请求来解决：达到 EOS、stop string、max tokens 或 abort 状态的请求被从 `running_batch` 中过滤掉，剩余请求继续 decode `sglang/02-request-flow/03-调度器与连续批处理.md:9.1`。这使得 batch 内各请求可以独立退出，GPU 不必等待最慢请求。

**问题分析：** 1) 定义同步退出问题——同 batch 请求的生命周期被绑定；2) 说明 continuous batching 如何通过 `filter_batch` + 每轮重新组 batch 实现独立退出；3) 指出独立退出对 GPU 利用率和吞吐的直接收益。

**具体答案：** 面试中可以这样回答：

> 同步退出问题是静态 batching 的固有缺陷：一个 batch 内的请求必须一起结束。如果 batch 中有 8 个请求，7 个已经生成完毕，但第 8 个还在继续，这 7 个已完成请求不能单独退出 batch——它们继续占着 GPU 的 request row、KV slot 和 batch 位置，但不再产出有用的计算。
>
> continuous batching 的解法是每轮 decode 前调用 `filter_batch`，把已经满足终止条件的请求（EOS、stop string、max_new_tokens、abort）从 `running_batch` 中移除。下一轮调度时 batch 只包含仍然活跃的请求，新到达的请求可以立即填补空位。这样 batch 内各请求的生命周期完全解耦。

**技术洞察：**

```text
同步退出问题（static batch）:
  batch = [r1(done), r2(done), ..., r8(still running)]
  → 即使 r1..r7 已完成，batch 仍为 8，GPU 为 8 个请求分配计算槽位
  → r1..r7 占位但不产出 → 浪费

continuous batching 解法:
  filter_batch() → 移除 r1..r7
  running_batch = [r8]
  → 下一轮 get_next_batch_to_run 可以加入新 prefill/decode
  → batch size 动态收缩，GPU 利用率保持
```

C：`sglang/02-request-flow/03-调度器与连续批处理.md:9.1` 的 filter_batch 行为。I：filter_batch 释放 request row 与 KV 资源的推导。U：不同 workload 下 batch size 的动态变化分布需运行时观测。

**图文说明：** 画两排时间线：上排 static batch 显示 8 个请求被绑定，7 个完成后仍保持 batch_size=8 的空白计算区域；下排 continuous batch 显示每轮 filter 后 batch_size 递减，新请求即时加入。用颜色标注已完成（灰）、运行中（蓝）、新加入（绿）的请求。

**拓展知识：** 与 I08 衔接：filter_batch 完成后请求释放的 KV 资源可以被 RadixCache 接管（`cache_finished_req`），不会立即释放而是成为可复用的 prefix cache；与 I16 衔接：独立退出使单请求延迟分布更紧凑，降低 P99。

**常见追问及回答要点：**

1. **问：filter_batch 释放的资源会立即归还 KV pool 吗？** 不一定。已完成请求的 KV 先经过 `cache_finished_req` 进入 RadixCache（作为可复用 prefix cache），只有淘汰或不需要缓存时才会归还 allocator。
2. **问：abort 请求也会被 filter_batch 移除吗？** 会。abort 请求被设置 finish reason 后，filter_batch 在下一轮把它从 running batch 中移除。但 abort 后的 KV 释放路径可能不同于正常完成（不走 cache_finished_req）。
3. **问：retract 和 filter_batch 有什么区别？** filter_batch 处理"自然终止"的请求，retract_decode 处理"资源不足需要腾空间"的请求。retract 的请求可能尚未完成，只是被迫暂停。

**易错点与反例：**

- 反例："continuous batching 中 batch 大小固定"——错误。batch 大小每轮动态变化，filter_batch 后缩小、admission 后扩大。
- 反例："filter_batch 就是简单的 list remove"——不完全。它需要同步释放 request row、更新调度状态，在 overlap 路径中还涉及 batch snapshot 的正确性。
- 易错：混淆同步退出与 retract——同步退出是"完成但不能走"，retract 是"还没完成但必须走"。

**实践与验证：** [U] 构造混合长度 workload（短输出 + 长输出混编），对比 static batch 与 continuous batch 中 GPU 在"等待最慢请求"阶段的空闲时间占比。通过 scheduler metrics 观察 `batch_size` 随时间的变化曲线。当前未执行。

**分层评分标准：** 2分：能定义同步退出问题并说 filter_batch 移除已完成请求；4分：能分析 filter_batch 对 GPU 利用率的影响、区分自然终止与 retract；6分：能讨论 KV 释放路径（进入 RadixCache vs 直接释放）、overlap 下 batch snapshot 的影响。

**证据与来源：** `sglang/02-request-flow/03-调度器与连续批处理.md`（§9.1 filter_batch、§13 continuous batching 完整例子）；`sglang/01-modules/M04-scheduler-batching/README.md`（§7 retraction vs filter）。

### I-B007 [I05] Transformer 推理时 attention 与 MLP/FFN 的计算量比例大致是多少？为什么不同？

**元数据：** 知识域：`I05 Transformer推理计算图`；能力目标：`C12 计算量比例分析`；题型：定量估算与推导；频率：中频；前置/后续：`I-B001`（prefill/decode 双阶段）、`I-B010`（KV cache）；证据状态：公式推导 C，具体数值 U。

**背景知识：** Transformer 单层包含两个主要计算块：attention（QKV 投影 + QK^T 矩阵乘 + softmax + ×V）和 MLP/FFN（up projection + down projection，可能加 gate）。attention 的计算量随序列长度 n 的 QK^T 项呈 O(n²)（或 O(n·d) 有 KV cache 时），而 MLP 的计算量是 O(n·d²)，与序列长度呈线性关系 `sglang/01-concepts/02-Transformer与KV-Cache.md`。对于 decoder-only 模型，prefill 时 attention 的 QK^T 是完整 n×n 矩阵，decode 时因 KV cache 存在只需读缓存。

**问题分析：** 1) 写出 attention 和 MLP 的 FLOPs 公式；2) 以典型 7B/8B 模型参数为例估算 prefill 阶段的比值；3) 解释为何 decode 阶段比例会变化（因为 KV cache 使 attention 的 FLOPs 从 O(n²) 降为 O(n·d)）。

**具体答案：** 面试中可以这样回答：

> 以 Llama-3-8B 为例（d=4096, n_heads=32, head_dim=128, intermediate_size=14336）：
>
> Attention 部分：QKV 投影各为 n×d×d FLOPs，QK^T 为 n×d×n FLOPs，×V 为 n×n×d FLOPs。有 KV cache 时 QK^T 变为 n_cur×d×n_total，decode 时 n_cur=1。
>
> MLP 部分：up projection n×d×(3d)（含 gate），down projection n×(intermediate)×d。
>
> 长 prompt prefill 时，attention 的 n×n 项主导，attention 计算量通常占总 FLOPs 的 30%–50% 以上。decode 时因 KV cache 使 attention QK^T 从 O(n²) 降为 O(n)，MLP 的 O(n·d²) 项反而占主导——此时 MLP 计算量约为 attention 的 2–4 倍。

**技术洞察：**

```text
单层 FLOPs 估算（n=seq_len, d=hidden_dim, h=n_heads, d_h=head_dim）：
  QKV proj:   3 × n × d × d
  QK^T:       n × d × n   (prefill, 无 KV cache)
              n_cur × d × n_total (decode, 有 KV cache)
  ×V:         n × n × d   (prefill)
              n_cur × n_total × d (decode)
  MLP up:     n × d × (intermediate × 2)  # gate+up
  MLP down:   n × intermediate × d

Llama-3-8B 典型比值:
  prefill (n=2048): attention ≈ 35-45%, MLP ≈ 55-65%
  decode (n_cur=1, n_total=2048): MLP ≈ 70-80%, attention ≈ 20-30%
```

C：`sglang/01-concepts/02-Transformer与KV-Cache.md` 的注意力公式。I：上式的 FLOPs 推导。U：具体比例随模型架构（如 GQA、MLA、MoE）和序列长度变化。

**图文说明：** 画堆叠柱状图，横轴 prefill vs decode，纵轴 FLOPs 占比。prefill 柱中 attention 与 MLP 接近 40:60；decode 柱中 MLP 占绝对主导（~75%）。用虚线标注 KV cache 使 attention QK^T 从 O(n²) 降到 O(n) 的分界。

**拓展知识：** 与 I08 衔接：KV cache 的存在直接改变了 attention 的计算量，使 decode 从 compute-bound 变为 bandwidth-bound；与 I13 衔接：PD 分离中 prefill 节点优化 compute（attention GEMM），decode 节点优化 bandwidth（KV cache 读取）。

**常见追问及回答要点：**

1. **问：MLA（Multi-head Latent Attention）会改变这个比例吗？** 会。MLA 把每层的 K/V 压缩为共享的 latent vector，大幅减少了 KV cache 的 bytes 和 attention 的读取量，使 decode 阶段 attention 占比进一步降低。
2. **问：GQA（Grouped Query Attention）影响计算量吗？** GQA 减少了 K/V head 数量，主要影响 KV cache 大小和 QK^T 的 FLOPs（K/V 侧维度降低），但计算量比例的整体趋势不变。
3. **问：MoE 模型的 MLP 部分有什么不同？** MoE 的 FFN 被拆成多个 expert，每个 token 只激活其中 top-k 个，实际 FLOPs 按激活参数量计算而非全部 expert 之和。

**易错点与反例：**

- 反例："attention 总是计算量最大的部分"——在 decode 阶段不正确。decode 时 KV cache 使 attention 计算量大幅下降，MLP 反成主导。
- 反例："prefill 和 decode 的 attention/MLP 比例相同"——错误。KV cache 改变了 attention 的计算形态，两个阶段的比例截然不同。
- 易错：忘记区分有无 KV cache——没有 KV cache 时 attention QK^T 始终是 O(n²)，比例完全不同。

**实践与验证：** [U] 固定模型（如 Llama-3-8B），用 FLOPs 计数器分别测量不同 prompt 长度下的 prefill 与 decode FLOPs，绘制 attention/MLP 比例随序列长度的变化曲线。当前未执行 GPU profiler。

**分层评分标准：** 2分：能说 attention O(n²) 与 MLP O(n·d²) 的复杂度差异；4分：能给出具体模型参数下的估算值，解释 prefill vs decode 的比例变化；6分：能讨论 MLA/GQA/MoE 等变体对比例的影响，并关联 roofline 模型解释 throughput 差异。

**证据与来源：** `sglang/01-concepts/02-Transformer与KV-Cache.md`（注意力公式、Q/K/V 定义）；`sglang/99-roadmap/performance-guide.md`（分阶段测量法）。

### I-B008 [I05] RoPE 位置编码在推理时如何处理不同长度的已生成序列？

**元数据：** 知识域：`I05 Transformer推理计算图`；能力目标：`C13 RoPE位置编码推理行为`；题型：机制解释；频率：中频；前置/后续：`I-B007`（attention/MLP 比例）、`I-B009`（ForwardBatch positions）；证据状态：源码 C，运行结论 U。

**背景知识：** RoPE（Rotary Position Embedding）通过将 Q/K 向量旋转与位置相关的角度来编码位置信息。每个位置 m 对应一个旋转矩阵 R(m)，推理时每个 token 的位置根据其当前序列中的绝对位置计算旋转角度 `sglang/02-request-flow/04-模型执行与输出.md:3.5`。在 SGLang 的 `ForwardBatch.init_new` 中，decode 模式使用 `clamp_position(batch.seq_lens)` 作为每个请求的 position，extend 模式根据 prefix_lens 和 extend_lens 生成连续位置段。

**问题分析：** 1) 解释 RoPE 的核心原理——每个 token 的位置编码取决于其在序列中的绝对位置；2) 说明 prefill 和 decode 阶段如何生成 position tensor；3) 说明同一 batch 内不同长度的序列如何保持各自独立的位置编码。

**具体答案：** 面试中可以这样回答：

> RoPE 给每个位置 m 定义一个旋转角度 θ(m) = m · base_freq，对 Q/K 向量施加旋转矩阵。推理时，新 token 的位置就是当前序列长度——第 10 个 token 用 θ(10)，第 100 个用 θ(100)，不需要重新计算前面位置的编码。
>
> 在 SGLang 中：prefill（extend）阶段，一个 prefix=10、extend=3 的请求生成位置 [10, 11, 12]；decode 阶段，每个请求的 position 就是当前 seq_len（如 [10, 25, 7] 三个请求各自的最新位置）。同一 batch 内不同长度的序列各用各的位置，互不干扰。
>
> 关键点：RoPE 的位置编码是增量式的——生成第 n 个 token 时只算 θ(n)，不需要重新处理 0..n-1。这与 Transformer 的绝对位置 embedding 不同。

**技术洞察：**

```text
RoPE 位置生成规则:
  prefill (extend):  位置 = [prefix_len, prefix_len+1, ..., prefix_len+extend_len-1]
  decode:            位置 = seq_len（当前序列长度，即新 token 的绝对位置）
  idle/dummy:        位置 = 0 或 pad

ForwardBatch.init_new 中的处理:
  if decode:  positions = clamp_position(batch.seq_lens)
  else:      positions = compute_position(prefix_lens, extend_lens)
```

C：`sglang/02-request-flow/04-模型执行与输出.md:3.5` 的 positions 生成逻辑。I：RoPE 增量式位置编码的推导。U：不同 RoPE scaling（linear/dynamic/YaRN）的具体频率参数需查模型 config。

**图文说明：** 画三个请求的序列：A 长 10（position 10）、B 长 25（position 25）、C 长 7（position 7）。每个序列右侧标注旋转角度 θ(m) 的值。用箭头表示 decode 时各请求取各自最新的位置作为新 token 的 position。对比 prefill 阶段一个 prefix=10 extend=3 的请求生成 [10,11,12] 三个连续位置。

**拓展知识：** 与 I09 衔接：ForwardBatch 的 positions 在 prefill 和 decode 模式下生成逻辑不同；与 I13 衔接：RoPE 的频率参数影响最大可处理序列长度（context length），超过预训练长度时需 RoPE scaling。

**常见追问及回答要点：**

1. **问：RoPE 支持超过预训练长度的序列吗？** 原生不支持，超出预训练长度的位置会产生未训练过的旋转角度。RoPE scaling（linear/dynamic/YaRN 等）通过调整频率 base 来外推。
2. **问：如果 batch 内有两个序列长度不同，attention 如何处理位置？** 每个请求的 Q/K 根据自己的位置做 RoPE 旋转，attention score 计算时 Q/K 已经携带各自的位置信息，不需要在 attention 内部对齐位置。
3. **问：M-RoPE 和普通 RoPE 有什么区别？** M-RoPE 为多模态输入（文本/图像/视频）使用多维位置编码（如 temporal/height/width），普通 RoPE 只有单一维度的位置。

**易错点与反例：**

- 反例："RoPE 每次生成都要重新计算所有历史位置"——错误。RoPE 是增量式的，只需要计算新 token 的位置。
- 反例："batch 内所有请求共享同一个 position 序列"——错误。每个请求根据自己的序列长度独立计算位置。
- 易错：混淆 RoPE 与绝对 position embedding——前者是旋转矩阵乘法，后者是可学习的 embedding 查表。

**实践与验证：** [U] 固定模型，对超长序列（超过预训练 context length）生成文本，观察 RoPE scaling 策略（无 scaling vs linear vs YaRN）下的生成质量差异。当前未执行。

**分层评分标准：** 2分：能说 RoPE 按绝对位置旋转、增量式处理；4分：能描述 prefill/extend/decode 三种模式下的 position 生成逻辑；6分：能讨论 RoPE scaling 外推策略、M-RoPE 扩展、以及与绝对 position embedding 的对比。

**证据与来源：** `sglang/02-request-flow/04-模型执行与输出.md`（§3.5 positions 生成）；`sglang/01-concepts/02-Transformer与KV-Cache.md`（注意力公式）。

### I-B009 [I06] ForwardBatch 如何用一个统一接口同时服务 prefill(extend) 和 decode？shape 如何处理？

**元数据：** 知识域：`I06 prefill/decode 与 ForwardBatch`；能力目标：`C14 ForwardBatch统一接口`；题型：机制解释；频率：高频；前置/后续：`I-B001`（双阶段差异）、`I-B008`（RoPE positions）；证据状态：源码 C，运行结论 U。

**背景知识：** `ForwardBatch.init_new` 是 SGLang 中统一服务 prefill(extend) 和 decode 的关键接口。它从 `ScheduleBatch` 读取 `forward_mode` 字段，根据模式决定哪些 metadata 有意义：decode/idle 模式下 `extend_seq_lens` 和 `extend_prefix_lens` 置为 None；extend 模式下从 `batch.extend_lens` 和 `batch.prefix_lens` 取值 `sglang/02-request-flow/04-模型执行与输出.md:3.3`。`init_new` 的设计约束是不修改输入 `ScheduleBatch`，确保 overlap scheduling 下多执行路径的正确性。

**问题分析：** 1) 解释 ForwardBatch 作为"计划的编译结果"的角色；2) 说明 `forward_mode` 如何控制 extend-only 字段的有无；3) 描述 positions 在两种模式下的生成差异——decode 用 seq_len 标量，extend 用连续位置段。

**具体答案：** 面试中可以这样回答：

> `ForwardBatch.init_new` 是一个从 `ScheduleBatch` 到 device 执行视图的纯映射函数。它读取 `forward_mode` 字段决定行为：
>
> - **decode**：每个请求只输入 1 个新 token，positions 由 `clamp_position(batch.seq_len)` 生成，`extend_seq_lens`/`extend_prefix_lens` 设为 None——因为这些字段对 decode 无意义；
> - **extend/prefill**：每个请求输入不同数量的 token，positions 由 `compute_position(prefix_lens, extend_lens)` 生成连续段（如 prefix=10, extend=3 → [10,11,12]），`extend_seq_lens`/`extend_prefix_lens` 携带给 attention backend 以区分各请求的 KV 边界；
> - **idle**：没有真实请求，但 DP/MLP 同步可能需要占位 forward。
>
> 统一接口的核心是 `forward_mode`——同一个 `ForwardBatch` 结构体，通过 mode 标志位让 attention backend 走不同代码路径。

**技术洞察：**

```text
ForwardBatch.init_new 字段差异:

字段                decode          extend
forward_mode        DECODE          EXTEND
input_ids           [t1,t2,t3]      [A_new1,...,B_new1,...]
positions           [10,25,7]       [10,11,12,25,26]
extend_seq_lens     None            [3, 2]
extend_prefix_lens  None            [4, 2]
extend_num_tokens   None            5
out_cache_loc       各请求下一个位置   flatten 后的新位置

关键不变量:
  - input_ids 是 flatten 的 token 序列
  - seq_lens 标识每个请求的当前序列总长
  - out_cache_loc 标识新 KV 写入位置
  - extend_* 字段仅 extend 模式非空 → attention backend 据此区分
```

C：`sglang/02-request-flow/04-模型执行与输出.md:3.3` 的 decode/extend 字段差异。I：flatten input_ids 与 extend_lens 对齐 attention 边界的推导。U：不同 attention backend 对 extend 元数据的具体消费方式需查阅 backend 实现。

**图文说明：** 画两个 ForwardBatch 并排：decode 模式左列显示 input_ids 是 3 个单 token [t1,t2,t3]，positions=[10,25,7]，extend 字段为 None；extend 模式右列显示 input_ids 是 flatten 的 5 个 token，positions=[10,11,12,25,26]，底部标注 extend_lens=[3,2]、prefix_lens=[4,2] 让 attention 知道前两 token 属请求 A、后两属请求 B。

**拓展知识：** 与 I08 衔接：`out_cache_loc` 由 M08 的 allocator 产出；与 I05 衔接：positions 直接影响 RoPE 的角度计算；与 I04 衔接：`ScheduleBatch` 的 `prepare_for_extend/prepare_for_decode` 负责产出 `ForwardBatch` 消费的所有字段。

**常见追问及回答要点：**

1. **问：为什么 extend 需要 flatten input_ids 而不是 pad 成 2D matrix？** flatten 避免了 padding 浪费计算（不同请求 extend 长度不同），attention backend 通过 `extend_start_loc` 知道每个请求 token 的起止位置。
2. **问：ForwardBatch 为什么不直接修改 ScheduleBatch？** overlap scheduling 可能同时存在一个 batch 的 snapshot 和当前 batch。如果 `init_new` 修改了 ScheduleBatch，另一条执行路径可能读到半更新状态。代码注释明确要求 `init_new must not mutate the input ScheduleBatch`。
3. **问：MIXED forward mode 是什么？** MIXED 允许同一轮 forward 同时包含 extend 和 decode 请求。running batch 的 decode 请求与新的 prefill batch 合并执行——此时 forward_mode 仍为 EXTEND，但 `decoding_reqs` 字段记录了哪些行实际是 decode。

**易错点与反例：**

- 反例："ForwardBatch 对 prefill 和 decode 用不同的类"——错误。是同一个类，通过 `forward_mode` 区分行为。
- 反例："extend 的 input_ids 是 2D [batch, max_len] 矩阵"——错误。是 flatten 的 1D 数组，用 `extend_start_loc` 标识边界。
- 易错：忘记 idle 模式——idle 用于 DP/MLP 同步的占位 forward，没有真实请求但需要执行以保持多卡同步。

**实践与验证：** [U] 在调试断点 `ForwardBatch.init_new:758-908` 处打印 extend 和 decode 两种模式下的字段值，验证 `extend_seq_lens` 在 decode 时为 None、positions 生成逻辑正确。当前未启动 GPU。

**分层评分标准：** 2分：能说 ForwardBatch 根据 forward_mode 决定 extend/decode 行为；4分：能描述 positions 和 input_ids 在两种模式下的 shape 差异、extend 字段的含义；6分：能分析不修改 ScheduleBatch 的设计原因、MIXED mode 的语义、以及 flatten + extend_start_loc 的 attention 边界机制。

**证据与来源：** `sglang/02-request-flow/04-模型执行与输出.md`（§3.3 decode/extend 字段差异、§3.5 positions 生成）；`sglang/01-modules/M05-model-execution/README.md`（ForwardBatch.init_new 不修改输入约束）。

### I-B010 [I08] 什么是 KV cache？为什么自回归解码必须使用它？不用会怎样？

**元数据：** 知识域：`I08 KV pool/paged attention`；能力目标：`C15 KV cache必要性`；题型：概念与必要性论证；频率：高频；前置/后续：`I-B007`（attention/MLP 比例）、`I-B011`（PagedAttention）；证据状态：文档 C，运行结论 U。

**背景知识：** 自回归生成中每个新 token 都要 attend 到所有历史 token。没有 KV cache 时，生成第 n 个 token 需要重新计算前 n-1 个 token 的 K/V——总计算量是 O(n²)。KV cache 把历史 token 的 K/V 保存下来，每步只需计算新 token 的 K/V 再读取缓存，总计算量降为 O(n) `sglang/01-concepts/02-Transformer与KV-Cache.md:1`。SGLang 通过 `ReqToTokenPool`（请求→slot 映射）和 `RadixCache`（前缀→索引映射）两层抽象管理 KV cache `sglang/01-modules/M08-kv-cache/README.md`。

**问题分析：** 1) 解释 KV cache 的核心原理——缓存历史 K/V 避免重复计算；2) 用计算复杂度论证必要性——从 O(n²) 到 O(n)；3) 分析不用 KV cache 的后果——计算量爆炸、decode 几乎不可用。

**具体答案：** 面试中可以这样回答：

> Transformer 的自回归生成要求新 token 关注所有历史 token。每生成一个 token 时，attention 需要所有历史位置的 K/V。没有 KV cache，每步都要重新计算全部历史 token 的 K/V——生成第 n 步需要 n 次 K/V 计算，总计 1+2+...+n = O(n²) 的计算量。
>
> KV cache 把历史 K/V 保存下来，每步只计算新 token 的 K/V 再与缓存拼接——总计 O(n) 计算量。对于一个生成 1000 token 的 7B 模型，没有 KV cache 时 decode 计算量约增加 500 倍，实际上不可用。
>
> 在 SGLang 中，KV cache 不是简单的 dict 而是三层抽象：`MHATokenToKVPool` 持有物理 K/V tensor，`ReqToTokenPool` 映射请求的第 n 个 token 到物理槽位，`RadixCache` 管理可复用前缀的索引。

**技术洞察：**

```text
无 KV cache 的 decode 计算量:
  step 1: 计算 token[0] 的 K,V                         → 1 次前向
  step 2: 重新计算 token[0,1] 的 K,V                    → 1 次前向
  step 3: 重新计算 token[0,1,2] 的 K,V                  → 1 次前向
  ...
  step n: 重新计算 token[0..n-1] 的 K,V                 → 1 次前向
  总计 n 次完整前向，生成 n 个 token                    → O(n²)

有 KV cache 的 decode 计算量:
  step 1: 计算 token[0] 的 K,V, 写入 cache             → 1 次前向
  step 2: 计算 token[1] 的 K,V, 读取 cache[0]           → 1 次前向（但只有 1 个 token）
  step 3: 计算 token[2] 的 K,V, 读取 cache[0,1]         → 1 次前向（但只有 1 个 token）
  总计 n 次单 token 前向                               → O(n)
```

C：`sglang/01-concepts/02-Transformer与KV-Cache.md:1` 的直观解释与简化注意力公式。I：O(n²) vs O(n) 的推导。U：具体加速比取决于模型大小、序列长度和硬件。

**图文说明：** 画两排计算图：上排"无 KV cache"每步都重新展开完整序列，灰色部分标注"重复计算"；下排"有 KV cache"每步只算新 token（蓝色），读取缓存（绿色）。底部标注复杂度对比 O(n²) vs O(n)。

**拓展知识：** 与 I07 衔接：MLA 等架构通过压缩 K/V 进一步减少 KV cache 大小；与 I13 衔接：PD 分离中 decode 节点的核心负载就是读取 KV cache。

**常见追问及回答要点：**

1. **问：KV cache 的内存占用有多大？** 每个 token 的 KV = 2 × n_layers × n_heads × head_dim × dtype_size。Llama-3-8B 在 bf16 下约 2 × 32 × 32 × 128 × 2 bytes ≈ 512 KB/token。一个 2048 token 的序列约需 1 GB 显存。
2. **问：KV cache 能无限增长吗？** 不能。GPU 显存有限，当 KV pool 满时 scheduler 执行 retraction 或等待请求完成释放空间。
3. **问：prefix cache 和 KV cache 是什么关系？** prefix cache 是 KV cache 之上的复用层。RadixCache 识别相同前缀的请求，让它们共享已计算的 KV，避免重复 prefill。

**易错点与反例：**

- 反例："KV cache 缓存的是 attention 的输出"——错误。缓存的是 K/V 向量（或 MLA 下的 latent vector），不是 attention 的最终输出。
- 反例："KV cache 只是优化，不是必需的"——对于在线服务，没有 KV cache 的计算开销是不可接受的。
- 易错：只记住"避免重复计算"而忽略"KV cache 大小随序列线性增长"带来的显存管理挑战。

**实践与验证：** [U] 固定模型，分别在有/无 KV cache 模式下测量 decode 吞吐与延迟。无 KV cache 可以通过禁用缓存或用短序列近似。当前未执行对照实验。

**分层评分标准：** 2分：能说 KV cache 缓存历史 K/V 避免重复计算；4分：能推导 O(n²) vs O(n) 的复杂度差异，估算典型模型的 KV cache 大小；6分：能讨论 KV cache 带来的显存管理挑战（retraction/paged/eviction）以及 RadixCache 的前缀复用机制。

**证据与来源：** `sglang/01-concepts/02-Transformer与KV-Cache.md`（§1 直观解释、§4 三层关系）；`sglang/01-modules/M08-kv-cache/README.md`（ReqToTokenPool + RadixCache）。

### I-B011 [I08] PagedAttention 如何解决 KV cache 的显存碎片问题？与传统预留方式对比。

**元数据：** 知识域：`I08 KV pool/paged attention`；能力目标：`C16 PagedAttention碎片解决方案`；题型：方案对比分析；频率：高频；前置/后续：`I-B010`（KV cache 必要性）、`I-B002`（admission 与容量）；证据状态：论文 C，运行结论 U。

**背景知识：** 传统方式为每个请求预留最大输出长度的连续 KV 显存，导致两种碎片：internal fragmentation（预留空间 > 实际使用）和 external fragmentation（不同请求长度不一，释放后留下不连续空洞无法被新请求使用）。PagedAttention（vLLM 论文，arXiv 2309.06180）借鉴 OS 虚拟内存分页：将 KV cache 分为固定大小的 block（page），通过 block table 建立逻辑 block → physical block 映射。请求按需分配 block，物理 block 不需要连续，消除了 external fragmentation；小 block 和按需分配缓解 internal fragmentation `insights/03-arxiv-reading-notes.md:1`。SGLang 的 `PagedTokenToKVPoolAllocator` 实现了类似的 page 级分配，`page_size > 1` 时 allocator 按 page 粒度分配。

**问题分析：** 1) 定义两种碎片类型；2) 解释 PagedAttention 的 block table 机制——逻辑与物理分离；3) 与传统连续预留方式对比优缺点。

**具体答案：** 面试中可以这样回答：

> 传统连续预留方式有两个问题：一是 internal fragmentation——每个请求预分配最大长度的连续空间，但实际输出可能远小于最大值，浪费显存；二是 external fragmentation——不同请求长度不同，释放后留下不连续的空洞，新请求即使总空间够也找不到连续块。
>
> PagedAttention 借鉴 OS 虚拟内存的分页机制：把 KV cache 切为固定大小的 block，每个请求维护一个 block table 记录逻辑 block 到物理 block 的映射。请求增长时按需分配新 block，物理 block 不需要连续——消除 external fragmentation；block size 小且按需分配——缓解 internal fragmentation。
>
> SGLang 的 `PagedTokenToKVPoolAllocator` 是类似实现：`page_size` 可配，`alloc_extend/alloc_decode` 生成 page-aware 写入位置。当 `page_size=1` 时退化为普通 slot allocator。

**技术洞察：**

```text
传统连续预留:
  请求 A: [max_len=2048] 预分配连续 2048 slots → 实际用 200 → 浪费 1848
  请求 B: [max_len=2048] 预分配连续 2048 slots → 完成释放
  空洞不连续 → 新请求需要 1000 连续 slots → 分配失败

PagedAttention:
  block_size = 64 tokens
  请求 A: 按需分配 block [0,1,2] → 物理 block [5, 12, 3]（不连续！）
  请求 B 释放 → 物理 block [8,9,10] 归还 free pool
  新请求: 从 free pool 取任意可用 block → 无需连续
  block table: logical_block[i] → physical_block[j]
```

C：`insights/03-arxiv-reading-notes.md:1` 的 PagedAttention PDF 原文确认；`sglang/01-modules/M08-kv-cache/README.md` 的 `PagedTokenToKVPoolAllocator`。I：block table 消除 external fragmentation 的推导。U：最优 block size 需权衡 page 内浪费和 kernel 访问开销。

**图文说明：** 画对比图：左列"连续预留"显示 3 个请求各占一大段连续空间，中间有灰色空洞标注"不可用的外部碎片"；右列"PagedAttention"显示 free pool 和 block table 映射，请求 A 的逻辑 block [0,1,2] 映射到分散的物理 block。标注 "blocks as pages, tokens as bytes, requests as processes"。

**拓展知识：** 与 I09 衔接：PagedAttention 需要 attention kernel 适配 non-contiguous block 访问；与 I04 衔接：page-aware allocation 影响 admission 的 KV budget 计算。

**常见追问及回答要点：**

1. **问：PagedAttention 有没有缺点？** 有。需要 attention kernel 支持 block 访问（不能直接用标准 FlashAttention），block 大小需要在 page 内浪费和 kernel 访问粒度之间权衡，且 copy-on-write 机制增加实现复杂度。
2. **问：SGLang 的 RadixCache 和 PagedAttention 是什么关系？** RadixCache 在前缀复用的场景下管理 KV indices 的归属，可以工作在 paged allocator 之上。两者解决不同问题：PagedAttention 解决碎片，RadixCache 解决复用。
3. **问：page_size=1 时 PagedAttention 退化成什么？** 退化为普通 slot allocator（`TokenToKVPoolAllocator`），每个 token 独立分配，没有 page 概念。

**易错点与反例：**

- 反例："PagedAttention 完全消除了内存浪费"——错误。block 内部仍有 partial page 浪费（一个 block 可能只用了部分 slots）。
- 反例："PagedAttention 的 block 必须连续"——错误。核心就是物理 block 不需要连续，通过 block table 建立映射。
- 易错：混淆 PagedAttention（vLLM 提出）和 RadixAttention（SGLang 的 prefix cache）——两者互补而非替代。

**实践与验证：** [U] 固定模型与 workload，对比 page_size=1 和 page_size=64 下的显存利用率（实际使用 / 分配总量）和 decode 吞吐。当前未执行。

**分层评分标准：** 2分：能说 PagedAttention 分块 + block table 映射；4分：能对比 internal/external fragmentation、解释 block table 如何消除碎片；6分：能讨论 page size 取舍、与 RadixCache 的协同、以及 vAttention 等替代方案的对比。

**证据与来源：** `insights/03-arxiv-reading-notes.md`（§1 PagedAttention PDF 原文确认：logical→physical block mapping、copy-on-write）；`sglang/01-modules/M08-kv-cache/README.md`（§8.1 PagedTokenToKVPoolAllocator）；`insights/arxiv-pdf/2309.06180-pagedattention-vllm.md`。

### I-B012 [I09] temperature 与 top-p 采样分别控制生成的什么特性？如何组合使用？

**元数据：** 知识域：`I09 sampling/grammar/停止`；能力目标：`C17 采样参数语义`；题型：参数语义与组合；频率：高频；前置/后续：`I-B010`（KV cache 必要性）、`I-B007`（attention/MLP）；证据状态：源码 C，运行结论 U。

**背景知识：** `SamplingParams` 包含 temperature、top-p、top-k、min-p 等采样参数，在 TokenizerManager 创建 tokenized object 时 normalize/verify `sglang/01-modules/M10-sampling-constraints/README.md:2`。temperature 控制 softmax 的平滑度——高温度使分布更平坦（更随机），低温度使分布更尖锐（更确定）。top-p（nucleus sampling）从概率累积占比前 p 的最小 token 集合中采样 `sglang/01-modules/M10-sampling-constraints/README.md:7`。SGLang 的 sampler 执行顺序：temperature → softmax → top-k → top-p → min-p → multinomial。

**问题分析：** 1) 分别解释 temperature 和 top-p 控制的分布特性；2) 说明它们在采样流水线中的执行顺序和相互作用；3) 给出常见组合策略及其适用场景。

**具体答案：** 面试中可以这样回答：

> temperature 控制 logits 经过 softmax 后的分布形状。temperature=1 是原始分布；temperature→0 趋近 greedy（选概率最高的 token）；temperature>1 使分布更平坦，低概率 token 被选中的机会增大——增加多样性但也增加不可预测性。
>
> top-p（nucleus sampling）控制候选集合的大小：按概率从高到低排序，累积概率首次超过 p 时截断，只从这最小集合中采样。p=1 不排除任何 token；p=0.9 排除尾部 10% 概率的 token——过滤掉明显不合理的候选。
>
> 组合使用时 temperature 先作用于 logits，再经过 top-p 截断。典型组合：temperature=0.7 + top-p=0.9 用于对话（多样但不离谱）；temperature=0 用于代码生成（确定性 greedy）。

**技术洞察：**

```text
采样流水线（SGLang Sampler.forward）:
  logits
    → temperature scaling: logits / T
    → softmax → probs
    → top-k: 保留概率最高的 k 个
    → top-p: 累积概率 ≤ p 的最小集合
    → min-p: 按 max_prob × min_p 过滤
    → multinomial / argmax → token_id

temperature 语义:
  T → 0: 分布趋近 one-hot → greedy
  T = 1: 原始分布
  T > 1: 分布平坦化 → 高多样性

top-p 语义:
  p = 1.0: 不截断
  p = 0.9: 排除尾部 10%
  p → 0: 趋近 greedy

组合效果:
  temperature=0.7, top-p=0.9: 柔和但过滤长尾 → 对话
  temperature=0, top-p=1.0: 纯 greedy → 代码/数学
  temperature=1.0, top-p=0.95: 更随机的创意写作
```

C：`sglang/01-modules/M10-sampling-constraints/README.md:7` 的采样流水线。I：temperature 对分布形状影响的推导。U：最优参数组合依赖具体任务和模型。

**图文说明：** 画概率分布图：原始 logits → temperature=0.5 分布变尖 → temperature=2.0 分布变平 → top-p=0.9 截断尾部（用竖线标注累积 0.9 位置）。四个子图对比展示不同 (T, p) 下的采样候选集。

**拓展知识：** 与 I10 衔接：grammar 约束在采样前通过 vocab mask 屏蔽非法 token；与 I16 衔接：sampling 耗时通常很小（< 5% decode time），但 grammar mask 计算可能增加 CPU 开销。

**常见追问及回答要点：**

1. **问：top-k 和 top-p 有什么区别？** top-k 固定保留概率最高的 k 个 token，不考虑概率绝对值；top-p 动态确定候选集合大小，累积概率到 p 即停止。top-p 更灵活——高确定性时候选少，低确定性时候选多。
2. **问：min-p 是什么？** min-p 按当前最高概率的 min_p 倍过滤，把相对概率太低的 token 去掉。比 top-p 更适合处理分布平坦的场景。
3. **问：为什么 sampling 需要 seed？** 为了复现结果。SGLang 的 PyTorch sampler 支持 `multinomial_with_seed`，但 flashinfer backend 的复杂采样路径要求 seed 为空。

**易错点与反例：**

- 反例："temperature 和 top-p 是或的关系"——错误。它们顺序执行：先 temperature 缩放再 top-p 截断，是组合关系。
- 反例："top-p=0.1 只选 10% 的 token"——不精确。是累积概率首次超过 0.1 的最小 token 集合，不是固定 10% 的数量。
- 易错：temperature 应该作用于 logits 还是 probs？——logits（先除以 T 再做 softmax），而不是直接缩放 probs。

**实践与验证：** [U] 固定模型和 prompt，分别测试 temperature∈{0, 0.3, 0.7, 1.0} × top-p∈{0.5, 0.9, 1.0} 组合下的生成多样性与质量。可用 perplexity 和 self-BLEU 量化多样性。当前未执行。

**分层评分标准：** 2分：能说 temperature 控制确定性、top-p 控制候选集；4分：能描述执行顺序和组合效果，给出典型参数组合；6分：能讨论 top-k/top-p/min-p 三者的区别与取舍、seed 复现性限制、以及 grammar mask 与采样的交互。

**证据与来源：** `sglang/01-modules/M10-sampling-constraints/README.md`（§2 SamplingParams、§7 sampler pipeline）；`sglang/01-concepts/01-推理服务基础概念.md`（采样参数）。

### I-B013 [I16] TTFT 和 TPOT 分别衡量推理服务的什么性能？它们受哪些因素影响？

**元数据：** 知识域：`I16 观测/测试/故障`；能力目标：`C18 核心性能指标`；题型：指标解释与因素分析；频率：高频；前置/后续：`I-B001`（prefill/decode 双阶段）、`I-B002`（admission）；证据状态：文档 C，具体数值 U。

**背景知识：** TTFT（Time To First Token）是请求发起到返回首个 token 的时间，涵盖 tokenize、排队等待 admission、prefill 计算三个环节。TPOT（Time Per Output Token）是首个 token 之后每个 output token 的平均间隔，主要取决于 decode step 时间 `sglang/99-roadmap/performance-guide.md`。TTFT 主要受 prompt 长度（prefill 计算量）、admission 排队时间、prefix cache 命中率影响；TPOT 受 batch size（KV cache 读取量）、模型大小、KV cache 带宽影响。

**问题分析：** 1) 分别定义 TTFT 和 TPOT 及其物理含义；2) 分析影响 TTFT 的主要因素——prefill 计算、排队、tokenizer 耗时；3) 分析影响 TPOT 的主要因素——batch 大小、KV cache 带宽、模型架构。

**具体答案：** 面试中可以这样回答：

> TTFT 衡量"从发请求到看到第一个 token 要等多久"——它包括 tokenizer 耗时、在 waiting queue 等 admission 的排队时间、以及 prefill 阶段的计算时间。TTFT 直接决定用户感知的"响应速度"。TTFT 主要受 prompt 长度（prefill 计算量正比于 prompt token 数）、admission 排队轮次（budget 和 KV 是否足够）、以及 prefix cache 命中率（命中高则 prefill 计算少）影响。
>
> TPOT 衡量"生成过程中每个 token 的平均间隔"——它基本就是一次 decode step 的时间。TPOT 主要受 batch size（越大则 KV cache 总读取量越大）、模型大小（参数量决定计算量）、以及显存带宽（decode 是 bandwidth-bound）影响。TPOT 决定"生成流畅度"。

**技术洞察：**

```text
TTFT = t_tokenize + t_queue + t_prefill
  t_tokenize:   text → token ids 的 CPU 耗时
  t_queue:      等待 admission 的轮次 × 每轮时间
  t_prefill:    prompt_len × FLOPs_per_token / compute_capacity
               — prefix cache 命中高 → t_prefill 下降

TPOT = t_decode / batch_effective
  t_decode ≈ max(compute_time, kv_cache_read_time)
  kv_cache_read = batch_size × seq_len × KV_bytes_per_token / bandwidth
  → batch_size ↑ → TPOT ↑（带宽瓶颈）
  → 量化 KV → KV_bytes ↓ → TPOT ↓
```

C：`sglang/99-roadmap/performance-guide.md` 的分阶段测量法。I：TTFT/TPOT 分解的推导。U：具体数值因模型/硬件/workload 差异巨大。

**图文说明：** 画时间线：请求发起 → [t_tokenize] → [t_queue] → [t_prefill] → 第一个 token → [t_decode]×N → 完成。TTFT 覆盖前三段，TPOT 是后段的平均。用不同颜色标注各段，下方标注影响因素（prompt len → t_prefill; batch_size → t_decode）。

**拓展知识：** 与 I04 衔接：admission 排队是 t_queue 的主要来源；与 I13 衔接：PD 分离中 prefill 和 decode 在不同节点，TTFT 和 TPOT 可以独立优化。

**常见追问及回答要点：**

1. **问：如何降低 TTFT？** 三方面：(1) 提高 prefix cache 命中率减少 prefill 计算；(2) 优化 admission 策略减少排队轮次；(3) 使用 PD 分离把 prefill 拆到专用节点。
2. **问：batch size 对 TPOT 的影响是线性的吗？** 在 bandwidth-bound 区域近似线性——每个请求的 KV cache 读取量固定，batch 增大使总读取量线性增长。但在 compute-bound 区域影响较小。
3. **问：TTFT 和 TPOT 哪个对用户体验更重要？** 取决于场景。交互式对话中 TTFT 更关键（用户不想等）；长文本生成中 TPOT 更关键（生成流畅度）。通常用 TTFT + max_new_tokens × TPOT 估算总延迟。

**易错点与反例：**

- 反例："TTFT 就是 prefill 时间"——错误。TTFT 还包括 tokenize 和排队等待时间。
- 反例："TPOT 与 batch size 无关"——错误。batch 增大使 KV cache 总读取量增长，直接影响 decode 带宽。
- 易错：混淆 TTFT 与端到端延迟——TTFT 只是到第一个 token，总延迟 = TTFT + TPOT × (output_tokens - 1)。

**实践与验证：** [U] 固定模型，分别测量不同 prompt 长度（128/512/2048/8192 tokens）下的 TTFT 和不同 batch size（1/4/8/16/32）下的 TPOT，绘制变化曲线。控制温度=0 以排除 sampling 变异。当前未执行 benchmark。

**分层评分标准：** 2分：能定义 TTFT 和 TPOT；4分：能分析各自的主要影响因素（prompt len、batch size、bandwidth）；6分：能讨论优化手段（prefix cache、PD 分离、KV 量化）并关联 roofline 模型解释 batch size 对 TPOT 的非线性影响。

**证据与来源：** `sglang/99-roadmap/performance-guide.md`（分阶段测量法）；`sglang/02-request-flow/03-调度器与连续批处理.md`（§10 prefill/decode 差异）。

### I-B014 [I16] 推理服务的 GPU 利用率低是否意味着资源浪费？如何正确评估推理服务健康度？

**元数据：** 知识域：`I16 观测/测试/故障`；能力目标：`C19 推理服务健康度评估`；题型：辨析与评估框架；频率：中频；前置/后续：`I-B013`（TTFT/TPOT）、`I-B005`（batch 策略）；证据状态：文档 C，运行结论 U。

**背景知识：** 推理服务的 GPU 利用率通常低于训练——decode 阶段是 bandwidth-bound，大量 GPU 计算单元空闲等待显存读取。低利用率在推理场景下是正常现象，甚至"高利用率"反而可能是瓶颈信号 `sglang/99-roadmap/performance-guide.md`。正确评估健康度需要多维指标：TTFT/TPOT（用户体验）、吞吐（tokens/s）、KV cache 利用率（容量效率）、admission 拒绝率、abort/retract 率。

**问题分析：** 1) 解释推理服务低 GPU 利用率的原因——decode bandwidth-bound 特性；2) 说明"利用率低=浪费"是误区；3) 给出正确的健康度评估框架——延迟、吞吐、资源效率三个维度。

**具体答案：** 面试中可以这样回答：

> 推理服务的 GPU 利用率天然低于训练。decode 阶段每步只做一次前向但需要读取全部 KV cache，计算量小而访存量大——GPU 的计算单元大量时间等在显存带宽上，利用率可能只有 20%–40%。这不代表浪费，而是推理 workload 的 roofline 特性决定的。
>
> 正确评估推理服务健康度需要三层指标：
>
> 1. **延迟层**：TTFT（首 token 延迟）和 TPOT（每 token 间隔）——直接决定用户体验；
> 2. **吞吐层**：总 tokens/s、QPS——决定服务的处理能力；
> 3. **资源效率层**：KV cache 利用率、admission 拒绝率、retract/abort 率——决定资源是否被合理使用。
>
> 如果 GPU 利用率低但 TTFT 正常、吞吐达标、admission 不频繁拒绝——服务健康，只是 decode 的带宽特性使然。如果 GPU 利用率低且 TTFT 在恶化——可能是 admission 排队过长或 prefill/decode 调度比失衡。

**技术洞察：**

```text
推理 GPU 利用率低的原因:
  decode step: FLOPs/byte（算术强度）≈ 1/seq_len
    → 长序列远低于 ridge point → bandwidth-bound
    → 计算单元空闲等待显存 → GPU 利用率低
  → 这是 roofline 模型的正常现象，不是异常

健康度三层评估:
  延迟:  TTFT < SLO_target (e.g. 200ms)
         TPOT < SLO_target (e.g. 30ms/token)
  吞吐:  tokens/s 接近 roofline 带宽极限
  资源:  KV cache 利用率 60-90%（不过空不过满）
         admission_rejection_rate < 5%
         retract_rate < 1%

反直觉信号:
  GPU 利用率 100% → 可能 prefill 主导或 batch 过大
  KV cache 利用率 100% → 即将频繁 retract → TPOT 恶化
```

C：`sglang/99-roadmap/performance-guide.md` 的分阶段测量法与约束。I：decode bandwidth-bound 导致低利用率的推导。U：具体 SLO 阈值和 healthy 区间因业务需求而异。

**图文说明：** 画 roofline 图：横轴算术强度、纵轴吞吐，标注 decode 点落在带宽斜坡区（低利用率区）。旁边画健康度仪表盘三层：绿色区（healthy）标注正常范围、黄色区（warning）标注异常信号、红色区（critical）标注需干预。标注"利用率低 + 延迟正常 = healthy"。

**拓展知识：** 与 I01 衔接：continuous batching 的目标是充分利用 decode 阶段的带宽而非提升计算利用率；与 I17 衔接：优雅关闭时需要确认所有已 dispatch 请求已完成——资源利用率指标帮助判断关闭时机。

**常见追问及回答要点：**

1. **问：什么情况下低利用率是有问题的？** 当 admission 拒绝率同时很高——说明 KV pool 配置过小或 `max_running_requests` 设得太低，GPU 空闲不是因为 bandwidth-bound 而是因为"没活可干"。
2. **问：KV cache 利用率 100% 是好事吗？** 不是。100% 意味着没有余量给新请求增长，下一步 scheduler 就会触发 retract_decode 把部分请求踢出——TPOT 会抖动。
3. **问：如何在吞吐和延迟之间取舍？** batch size 增大提升吞吐但增加 TPOT（带宽压力）。需要根据 SLO 选择合适的 batch 上限，而不是一味追求吞吐。

**易错点与反例：**

- 反例："GPU 利用率低就是资源浪费，应该加大 batch"——错误。盲目加大 batch 会使带宽更紧张，TPOT 恶化，用户体验下降。
- 反例："只看吞吐就够了"——错误。高吞吐可能伴随高 TTFT（排队严重），用户感知差。
- 易错：用训练的利用率标准衡量推理——训练的 roofline 在 compute 区，推理的 roofline 在 bandwidth 区，评估标准完全不同。

**实践与验证：** [U] 固定模型与 workload，同时采集 GPU 利用率、TTFT、TPOT、KV cache 利用率、admission 拒绝率，绘制多维散点图观察相关性。当前未执行。

**分层评分标准：** 2分：能说 decode bandwidth-bound 导致利用率低；4分：能给出延迟/吞吐/资源三层评估框架，解释异常信号；6分：能讨论利用率与健康度的非线性关系、batch size 对吞吐/延迟的 trade-off、以及 retract 对稳定性的影响。

**证据与来源：** `sglang/99-roadmap/performance-guide.md`（分阶段测量法、约束）；`sglang/02-request-flow/03-调度器与连续批处理.md`（§10 prefill/decode 差异）。

### I-B015 [I17] 优雅关闭(graceful shutdown)时推理服务需要完成哪些步骤才能不丢请求？

**元数据：** 知识域：`I17 部署/扩缩/优雅关闭`；能力目标：`C20 graceful shutdown步骤`；题型：流程描述；频率：中频；前置/后续：`I-B003`（ReqState 生命周期）、`I-B014`（健康度评估）；证据状态：源码 C，运行结论 U。

**背景知识：** SGLang 的 graceful shutdown 跨越三个进程。收到 SIGTERM 后，TokenizerManager 的 watchdog 停止接收新请求、等待现有 `rid_to_state` 清空、停止 subprocess watchdog、dispatch `ShutdownReq`、等待 scheduler processes、`kill_process_tree` 处理残余子进程 `sglang/01-modules/M15-ipc-control-plane/README.md:8.3`。Scheduler 的 `handle_shutdown` 设置 `gracefully_exit=True` 让 event loop 退出，`run_scheduler_process` 的 `finally` 在 graceful path 释放 host resources。Engine 的 shutdown 还关闭 RPC socket、终止 weight-cache daemons，`finally` 关闭 multimodal processor 和 CUDA VMM feature transport。

**问题分析：** 1) 按顺序列出 graceful shutdown 的关键步骤——从"停止接新请求"到"确认子进程退出"到"释放资源"；2) 强调已 dispatch 请求不能只删本地状态而不通知 scheduler；3) 说明 finally 兜底的必要性——graceful wait 超时后仍需 hard-kill。

**具体答案：** 面试中可以这样回答：

> 优雅关闭需要四个阶段：
>
> 1. **停止接收新请求**：TokenizerManager 的 watchdog 检测到 SIGTERM 后标记关闭中，不再接受新的 HTTP 请求或 `generate` 调用；
> 2. **等待已处理请求完成**：等待 `rid_to_state` 中所有条目清空——即所有已 dispatch 请求都从 scheduler 返回最终结果。这是最关键的"不丢请求"保证；
> 3. **通知子进程关闭**：dispatch `ShutdownReq` 广播给各 TP rank 的 scheduler，scheduler 设置 `gracefully_exit=True` 让 event loop 自然退出，不中断正在执行的 forward；
> 4. **兜底与清理**：graceful wait 结束后如果仍有残余子进程，`kill_process_tree` 硬终止。`finally` 阶段释放 host resources、关闭 RPC socket、终止 weight-cache daemons 和 multimodal processor。
>
> 核心不变量：已 dispatch 请求（`dispatched=True`）的 `ReqState` 不能只删除本地状态，必须等 scheduler 确认完成后再清理。

**技术洞察：**

```text
graceful shutdown 顺序（SGLang）:

TokenizerManager.watchdog:
  1. 停止接收新请求/等待 rid_to_state 清空
  2. 停止 subprocess watchdog
  3. dispatch ShutdownReq → 各 scheduler
  4. 等待 scheduler processes 退出
  5. kill_process_tree（兜底）
  6. sys.exit(0)

Scheduler:
  handle_shutdown → gracefully_exit=True → event loop 退出
  run_scheduler_process.finally:
    → metrics 发布线程 teardown
    → graceful path: release_host_resources

Engine.shutdown:
  → 停止 watchdog
  → 关闭 RPC socket
  → 终止 weight-cache daemons
  → kill_process_tree
  → finally: 关闭 multimodal processor + CUDA VMM transport
```

C：`sglang/01-modules/M15-ipc-control-plane/README.md:8.3` 的 graceful shutdown 顺序。I：`rid_to_state` 清空作为关闭完成条件的推导。U：shutdown 耗时取决于最长未完成请求的剩余生成长度。

**图文说明：** 画三进程时序：主进程 TokenizerManager → SIGTERM → 停止新请求 → wait rid_to_state → dispatch ShutdownReq → wait schedulers → kill → exit。Scheduler 进程 → 收到 ShutdownReq → gracefully_exit → 当前 forward 完成 → event loop break → release resources → exit。Detokenizer 同理。底部标注清理不变量：已 dispatch 请求不能本地独删、daemon/IPC/multimodal 必须各自释放。

**拓展知识：** 与 I16 衔接：graceful shutdown 的等待时间直接影响服务的可用性 SLO（需设置合理的超时）；与 I13 衔接：PD 分离场景中 prefill 和 decode 节点的关闭需要协调，避免 decode 节点关闭后 prefill 节点无处发 KV。

**常见追问及回答要点：**

1. **问：graceful wait 超时了怎么办？** 兜底是 `kill_process_tree` 硬终止残余子进程。不能无限等——否则运维无法回收资源。
2. **问：正在执行 CUDA forward 的请求会被中断吗？** 不会。`gracefully_exit=True` 是在 event loop 层面设置的标志，当前 forward 自然完成后 loop 才退出。强制中断 CUDA forward 可能导致 GPU wedged。
3. **问：为什么 `run_scheduler_process` 的异常路径不调用 `release_host_resources`？** 因为异常状态下 GPU 可能 wedged，同步清理操作可能阻塞。只有 graceful path 才执行释放。

**易错点与反例：**

- 反例："graceful shutdown 就是发 SIGTERM 然后等"——不够。需要多层协调：停止新请求、等待已有请求完成、通知子进程、兜底硬杀。
- 反例："关闭时直接 kill scheduler 就行"——危险。scheduler 可能正在执行 forward，直接 kill 会导致 GPU wedged 和请求丢失。
- 易错：忘记 multimodal/CUDA VMM 等非 scheduler 资源——它们必须由各自拥有者清理，只等 scheduler 退出不够。

**实践与验证：** [U] 部署测试环境，在高负载下触发 SIGTERM，验证：(1) 新请求被拒绝；(2) 已 dispatch 请求全部完成；(3) 子进程全部退出；(4) GPU 资源释放（无 wedged）。监控 shutdown 耗时。当前未执行。

**分层评分标准：** 2分：能说停止新请求 → 等待完成 → 通知子进程 → 清理；4分：能描述三进程协作、`rid_to_state` 清空作为完成条件、finally 兜底；6分：能讨论 timeout 硬杀的必要性、异常路径不释放 GPU 资源的原因、以及 PD 分离场景的关闭协调。

**证据与来源：** `sglang/01-modules/M15-ipc-control-plane/README.md`（§8.3 graceful shutdown 顺序、清理不变量）；`sglang/02-request-flow/06-多进程与IPC.md`（shutdown 传播）。

