# 训练方向高级面试题（T-A001–T-A015）

> 每题按 12 段结构展开：元数据、背景知识、问题分析、具体答案、技术洞察、图文说明、拓展知识、常见追问、易错点与反例、实践与验证、分层评分、证据与来源。
> 证据规则：源码/文档明确陈述标 `C`，跨文档或工程推断标 `I`，需要目标硬件、NCCL、多进程才能成立的行为标 `U / 待验证`；除标注 `[待验证]` 外不编造统计数据。版本锚点统一为 Megatron-LM main @ `3703d4e33a3a2b2d11ebcc8e41f45af7ce7d1eda`，行号与符号需随源码更新复核。

### T-A001. DP/TP/PP/CP 如何组合：给定模型规模与集群拓扑，怎么选出并行方案并证明它是最优取舍？

- 难度：高级
- 知识域：T02 分布式初始化与通信组
- 考察能力：约束建模、方案对比、定量成本估算
- 题型：系统设计 + 定量分析
- 面试频率：高频
- 前置知识：三种经典并行轴的显存/通信特征、`world_size` 整除关系、NVLink/IB 带宽量级
- 关联题目：T-A002（气泡）、T-A005（CP 通信）、T-A011（激活重计算）、T-A012（瓶颈归因）
- 事实状态：并行轴的定性特征 `[已确认]`；具体配置下的吞吐/显存 `[待验证]`

#### 1. 背景知识

并行策略不是“三种模式”，而是一组正交通信域的组合：DP 切 batch 并归约梯度，TP 切权重与激活的 hidden 维，PP 切层并 P2P 传激活，CP 切序列激活（不切权重）。四者约束不同：

- 显存上，权重与优化器状态被 TP×PP 切，激活被 TP×CP 切，优化器状态还能被 DP（distributed optimizer）再分片。
- 通信上，MT-NLG 给出的定性排序是 TP > DP > PP（`C`，来自论文总结），因此 TP 优先放同节点 NVLink，PP 可跨节点。
- 约束上，`world_size` 必须能被 TP×PP×CP 整除（`C`，M02 不变量）；PP>1 才能开 VP；VP 只在 PP>1 时合法。

#### 2. 问题分析

选型要先把“硬约束”和“优化目标”分开，再逐轴决策：

1. **硬约束**：单卡必须装下 `权重/(TP·PP) + 梯度 + 优化器状态 + 激活峰值`；`world_size % (TP·PP·CP) == 0`；PP 需要足够层数均分且 `MB ≫ PP`。
2. **拓扑约束**：TP 通信量最大，超过单节点 NVLink 域通常不划算；PP 通信量最小，可跨节点；DP 梯度归约居中，优先同节点或近邻。
3. **优化目标**：在满足约束的前提下最大化 MFU（等价于最小化 compute 之外的 comm + idle）。
4. **决策顺序**（`I`，工程推导）：先 DP 打满 → 装不下就加 TP（同节点）→ 层多且 batch 大就加 PP → 序列过长再加 CP → 仍紧张用 SP + selective recompute/FSDP。

关键点：并行度不是越大越好。TP 增大使 GEMM 碎片化与未重叠延迟占比上升，PP 增大使气泡上升，CP 增大且未 overlap 时通信占主。

#### 3. 具体答案

面试中可以这样回答：

> 我会把它当成一个带约束的优化问题。第一步算单卡显存账：fp16 下权重 2B/参数、梯度 2B、fp32 master 与 Adam 两个 moment 共 12B，合计约 16B/参数（`I`）；70B 模型光状态就上千 GB，必须切。第二步看拓扑：TP 每层 4 次 allreduce、通信量最大，我只在同节点 NVLink 内开 TP；PP 只传 stage 边界 P2P、通信量最小，放到跨节点；DP 梯度归约居中，尽量放近邻。
>
> 第三步按顺序定轴：单卡装得下就 DP-only；装不下先开 TP 切 hidden（同节点）；层数够多、global batch 也够大（MB 至少是 PP 的若干倍）再开 PP；序列到 8K 以上再加 CP。第四步校验两个不变量——`world_size` 能被 TP×PP×CP 整除，以及每卡激活峰值低于显存预算。
>
> 第五步用气泡公式和通信体积估算验证：`efficiency ≈ 1 - (PP-1)/(MB+PP-1)`，若 MB=4×PP 才约 81%，所以 PP 不能随便加大。最后我会把方案和一个更“保守”的方案各跑几步，比 step time、MFU 和峰值显存，用数据而不是直觉定稿（`U / 待验证`）。

#### 4. 技术洞察

显存与效率的一阶估算（`I`，量级演示）：

```text
per_gpu_weight  ≈ P · 2B / (TP · PP)
per_gpu_optstate≈ P · 12B / (TP · PP · DP)      # distributed optimizer 时
per_gpu_act     ≈ f(s, b, hidden) / (TP · CP) · (在飞 MB 数)   # 受 SP/recompute 调节
约束：weight + grad + optstate + act_peak ≤ GPU_mem

PP 气泡：efficiency ≈ 1 - (PP - 1) / (MB + PP - 1)   # MB 为 microbatch 数
TP 每层通信：4 次 allreduce，与 TP 规模无关（次数恒定）
DP 每步通信：2·(DP-1)/DP · P · 2B（ring allreduce）
```

决策的实质：用通信（TP/PP/CP）换显存，并让通信落进计算空隙。

#### 5. 图文说明

| 轴 | 切什么 | 显存收益 | 通信量 | 拓扑偏好 | 主要风险 |
|---|---|---|---|---|---|
| DP | batch | 权重/激活不省；状态可 1/DP | 中 | 同节点或近邻 | global batch 过大影响收敛 |
| TP | hidden | 权重/激活 /TP | 高（每层 4 原语） | 同节点 NVLink | TP 过大 GEMM 碎片化 |
| PP | 层/stage | 权重+状态 /PP，激活不省 | 低（P2P） | 可跨节点 | MB 不足气泡大 |
| CP | 序列激活 | 激活 /CP，权重不省 | 中 | 同节点优先 | 未 overlap 通信占主 |

```text
单卡装得下? ──能──▶ DP-only
     │否
加 PP 可行(层多+MB 足)? ──是──▶ PP（跨节点）
     │否
加 TP（同节点 NVLink 内）
     │
序列 ≥8K? ──是──▶ CP（序列维）
     │否
仍紧张? ──是──▶ SP + selective recompute / FSDP
```

#### 6. 拓展知识

- 与 T-A002 交叉：PP 的收益强烈依赖 microbatch 数与 interleaving 选择，选 PP 时必须同时定 schedule。
- 与 T-A005/A006 交叉：CP 与 EP 都改变“通信落在哪个域”，CP 的梯度归约复用 DP 域，EP 走 all-to-all。
- 与 T-A011 交叉：激活峰值往往是 OOM 主因，selective recompute 与 SP 的组合会改变 TP/PP 的边际收益。

#### 7. 常见追问及回答要点

1. **为什么 TP 不宜跨越节点？** ——TP 每层 4 次 allreduce，是层内高频同步，落在 IB/RDMA 上延迟与带宽都吃亏；MT-NLG 的排布法则把 TP 限在同节点 NVLink（`C`）。
2. **什么情况下只用 DP+overlap 就够？** ——模型权重+优化器状态+激活能装进单卡，且 global batch 足够大能维持单卡计算密度；小模型微调通常属于此类（`I`）。
3. **TP 和 PP 二选一时怎么定？** ——看瓶颈在显存还是效率：TP 切得细、效率高但通信贵；PP 通信便宜但要够大的 batch 填气泡，层数最好能被 PP 均分。

#### 8. 易错点与反例

- 反例：为了“多卡都要用上”把 TP 开得很大，GEMM 退化到很小的矩阵，单卡计算密度暴跌，通信占比反升。
- 反例：PP 开得很大但 global batch 很小，MB≈PP，气泡效率接近 50%，多出来的卡全在等。
- 易错：忽略 `world_size` 整除约束，配置在 group 创建阶段才报错；应把整除校验放在选型第一步。

#### 9. 实践与验证

`[待验证]` 在同一集群上固定模型与 global batch，对 2–3 组候选配置各跑 50–100 step，记录 step time 中位数、tokens/s、峰值显存与 MFU；oracle 是“候选里 MFU 最高且峰值显存留有余量”的配置胜出。需要用 nsys/NCCL trace 区分 comm 与 idle。本仓库未执行多卡实验，此步骤为方法建议。

#### 10. 评分标准

- 不合格：只会背“TP 切权重、PP 切层、DP 切数据”，无法给出选型理由。
- 合格：能列出四轴特征与整除约束，给出 DP→TP→PP→CP 的基本顺序。
- 良好：能写出显存与气泡的定量估算，解释 TP 为何同节点、PP 为何跨节点。
- 优秀/专家：能引入 FSDP/heterogeneous/Parallel Folding，指出并行度的非单调收益，并给出可执行的对比实验与验收指标。

**证据与来源：** `insights/05-megatron-architecture.md` 第 5/6/8/9/10/15 章（并行轴特征、排布法则、气泡公式、决策流程，`C`/`I` 混标）；`megatron-lm/01-modules/M02-parallelism/README.md`（不变量，`C`）。显存与吞吐为量级推算（`I`），实际数值 `U / 待验证`。

---

### T-A002. pipeline schedule 与 interleaving：1F1B、GPipe 和虚拟流水线各自的代价边界在哪里？

- 难度：高级
- 知识域：T05 流水线并行与调度
- 考察能力：调度机制对比、气泡定量、边界条件识别
- 题型：机制对比 + 定量分析
- 面试频率：高频
- 前置知识：microbatch、warmup/steady/cooldown、P2P send/recv、在飞激活
- 关联题目：T-A001（PP 选型）、T-A011（重计算与显存）、T-A012（气泡归因）
- 事实状态：调度分派与气泡公式 `[已确认]`；具体配置收益 `[待验证]`

#### 1. 背景知识

三种 forward/backward 组织方式：

- **GPipe 风格**：把一个 batch 的 microbatch 全部 forward 再全部 backward，气泡最宽；内存需保存全部在飞激活。
- **1F1B（non-interleaved）**：稳定阶段每个 rank 交替执行一个 forward 与一个 backward，在飞 microbatch 数被 bound 到 stage 数而非总 MB 数（`C`，MT-NLG 论文明确）。
- **Interleaved / Virtual Pipeline**：把模型层切成多个虚拟 chunk，每个 PP rank 轮流处理多个 chunk，进一步压缩气泡，代价是 P2P 次数与调试复杂度上升。

M02 的分派规则（`C`）：PP=1 走 `forward_backward_no_pipelining`；PP>1 且无 VP 走非交错；PP>1 且有 VP 走交错；多模块 collection 走多模块兼容路径。

#### 2. 问题分析

选 schedule 要同时回答三个问题：

1. **气泡多大**：`efficiency ≈ 1 - (PP-1)/(MB+PP-1)`（`C`，论文）。因此“PP 能否加大”取决于 MB 是否同比增大。
2. **在飞激活多少**：1F1B 把在飞 microbatch 限制在 stage 数附近；interleaved 把单 stage 层数从 `L/PP` 降到 `L/(PP·vp)`，激活进一步下降。
3. **通信与复杂度**：interleaved 增加 stage 边界数 → P2P 次数与 tensor 保存增加；`overlap_p2p_comm` 在非交错路径上会抛 `ValueError`（`C`，M02 execution-flows）。

边界反例：层数不能被 PP（或在 interleaved 下不能被 PP·vp）整除时，stage 负载不均，慢 stage 拖住整条流水线。

#### 3. 具体答案

面试中可以这样回答：

> 我会先算气泡再看内存。气泡用论文的效率公式 `1-(PP-1)/(MB+PP-1)`：MB=4×PP 时约 81%，MB=8×PP 时约 90%，所以只要 MB 跟不上 PP，加 PP 就是在买气泡。
>
> 常规情况我选 1F1B：它把在飞 microbatch 数 bound 到 stage 数，激活驻留远低于 GPipe。只有两种场景我会考虑 interleaved——层数多到想让每个 rank 的激活再降一截，或者想在层数不能被 PP 整除时用 VP 更细地摊平负载。代价是 P2P 边界变多、tensor 保存和调试更复杂，而且 VP 必须在 PP>1 时才合法。
>
> 我还会检查两个工程边界：`overlap_p2p_comm` 与非交错 schedule 不兼容会直接报错；层数分配要尽量均匀，否则气泡之外又多一个慢 stage。最后用 step time 和 nsys 里的空白区分“气泡”和“通信未重叠”（`U / 待验证`）。

#### 4. 技术洞察

```text
1F1B 伪代码（基于论文描述）：
for step in range(num_microbatches + num_pipeline_stages - 1):
    if should_forward(step):   # warmup / steady
        forward(microbatch(step))
    if should_backward(step):  # steady / cooldown
        backward(microbatch(step - num_pipeline_stages))
        maybe_finalize_grads()

在飞 microbatch ≤ num_pipeline_stages          # 1F1B 的内存界
气泡效率 ≈ 1 - (PP - 1) / (MB + PP - 1)
interleaved 单 chunk 层数 ≈ L / (PP · vp)
```

#### 5. 图文说明

| schedule | 适用 | F/B 性质 | 气泡 | 通信/复杂度 |
|---|---|---|---|---|
| no-pipeline | PP=1 | 顺序 F/B | 无 | 无 P2P |
| 1F1B（非交错） | PP>1, VP=1 | 交替 F/B | 中 | P2P 少 |
| interleaved | PP>1, VP>1 | 虚拟 chunk 交替 | 低 | P2P 多、调试难 |

```text
PP=4, MB=8 时序示意（F=forward, B=backward, .=空）
rank0: F0 F1 F2 F3 B0 F4 B1 F5 B2 F6 B3 F7
rank1: .  F0 F1 F2 F3 B0 F4 B1 F5 B2 F6
rank2: .  .  F0 F1 F2 F3 B0 F4 B1 F5
rank3: .  .  .  F0 F1 F2 F3 B0 F4
开头/结尾各 PP-1 个时间片是气泡
```

#### 6. 拓展知识

- 与 T-A011 交叉：interleaved 降低在飞激活，selective recompute 降低单激活，两者可叠加但都增加调度复杂度。
- 与 T-A010 交叉：world size 变化时，PP/VP 划分改变会影响 sharded checkpoint 的 reshard 映射。
- `deallocate_output_tensor` 通过替换已发送输出的 storage 省显存，依赖 viewless tensor 与 custom backward 不变量（`C`，M02 implementation）。

#### 7. 常见追问及回答要点

1. **1F1B 相比 GPipe 省在哪？** ——在飞 microbatch 数从“全部”降到“约 stage 数”，激活驻留显著下降，气泡也被交错收发摊薄。
2. **interleaved 的收益与代价各是什么？** ——收益是更低在飞激活、更细的负载均衡；代价是更多 P2P 边界、更多 tensor 保存与更难的调试。
3. **为什么 VP 必须 PP>1？** ——VP 是对 PP stage 内部的再切分，没有 PP 就没有可再分的 stage，初始化会拒绝该组合。

#### 8. 易错点与反例

- 反例：MB=PP 仍强行加大 PP，气泡效率接近 50%，多卡只是让 step time 更长。
- 反例：在非交错 schedule 上开 `overlap_p2p_comm`，M02 记录该路径直接抛 `ValueError`，不是性能问题而是非法配置。
- 易错：忽略层数均分，某 stage 层数更多导致流水线被最慢 stage 卡住，气泡公式亦不再适用。

#### 9. 实践与验证

`[待验证]` 固定模型与 MB，对比 1F1B 与 interleaved 的 step time、峰值激活显存与 P2P 通信量；oracle 是“峰值显存达标前提下 step time 更低”。需在具备 NCCL 的多卡环境执行，本仓库仅给出方法，未运行实验。

#### 10. 评分标准

- 不合格：只能说出“PP 是切层”，不知道 schedule 有三种。
- 合格：能区分 GPipe/1F1B/interleaved，说出 1F1B 的在飞上界。
- 良好：能写出气泡公式并解释 MB 与 PP 的耦合，指出 interleaved 的通信代价。
- 优秀/专家：能结合 `get_forward_backward_func` 的分派条件、`deallocate_output_tensor`、overlap 限制，给出可验证的 schedule 选择与验收方案。

**证据与来源：** `insights/05-megatron-architecture.md` 第 8 章（1F1B、气泡公式、VP，`C`/`I`）；`megatron-lm/01-modules/M02-parallelism/implementation.md`（schedule 分派、`deallocate_output_tensor`，`C`）；`M02-parallelism/execution-flows.md`（`overlap_p2p_comm` 抛错，`C`）。运行收益 `U / 待验证`。

---

### T-A003. MoE 的 capacity factor 与负载均衡：如何在吞吐、显存和收敛质量之间取舍？

- 难度：高级
- 知识域：T07 MoE 与专家并行
- 考察能力：约束下的多目标取舍、机制到可观测指标的映射
- 题型：系统设计 + 定量分析
- 面试频率：高频
- 前置知识：MoE 路由 top-K、token dispatch/combine、CUDA Graph 的静态 shape 约束
- 关联题目：T-A006（EP 通信）、T-A004（FP8 与路由数值）、T-A014（大 batch 稳定性）
- 事实状态：dispatcher 与 capacity 配置项 `[已确认]`；收敛影响 `[待验证]`

#### 1. 背景知识

MoE 每个 token 只路由到 top-K experts（常见 top-2）。为保证计算 shape 可控，框架允许给每个 expert 设一个容量上限，超出容量的 token 被丢弃（token dropping）或保留（dropless）：

- `--moe-expert-capacity-factor`：容量系数，决定每个 expert 队列长度。
- `--moe-pad-expert-input-to-capacity`：把输入 pad 到 capacity，使 shape 静态化以支持 CUDA Graph。
- `--moe-token-drop-policy probs/position`：按概率或按位置丢弃溢出 token（`C`）。

负载均衡是核心难点：路由偏移会让少数专家成为热点，A2A 长时间等待，而多数专家空闲。

#### 2. 问题分析

capacity factor 是三个目标的旋钮：

1. **吞吐**：capacity 越小、丢弃越多，计算越恒定越易被 CUDA Graph 捕获；dropless 则 shape 随路由变化，难以 graph 化。
2. **显存与通信**：capacity 决定每 expert 缓冲区，也决定 A2A 体积上限。
3. **质量**：丢弃 token 等价于对部分样本“少算”，capacity 太低会损害收敛（`I`）。

均衡手段：aux loss / sinkhorn 校准路由分布，配合合理 capacity；只在 GPU 预算允许时用 dropless。判据不是“越大越好”，而是“丢弃率与 graph 收益的平衡点”。

#### 3. 具体答案

面试中可以这样回答：

> 我会把 capacity factor 看成“计算确定性”与“信息完整性”的兑换率。先用 `capacity = ceil(capacity_factor · num_tokens · topK / num_experts)` 估每 expert 队列长度，再选 dropless 还是 dropping：dropless 不丢 token、质量最稳，但 shape 动态、难被 CUDA Graph 捕获；dropping 用 `pad-expert-input-to-capacity` 把 shape 固定，能上 graph、吞吐更稳，代价是溢出 token 被丢弃、可能损收敛。
>
> 均衡不能只靠 capacity 硬切，要先加 aux loss 或 sinkhorn 把路由分布压平，否则热点专家持续溢出、冷门专家吃不饱。drop policy 我倾向 `probs`，尽量丢低置信 token。上线前我会监控每 expert 的 token 数与丢弃率，按曲线调 capacity factor（`U / 待验证`）。
>
> 边界是：experts 很少（如 8 个）且 batch 小的时候，A2A 开销占比高，EP 本身可能就不划算，capacity 调优也救不回。

#### 4. 技术洞察

```text
capacity = ceil(capacity_factor · num_tokens · topK / num_experts)
每个 expert 实际处理 min(route_count, capacity) 个 token
溢出量 = Σ_experts max(0, route_count_e - capacity)

dropless：shape = Σ route_count_e（动态）  → 难 graph capture
dropping+pad：shape = num_experts · capacity（静态）→ 可 graph capture
均衡目标：min Var(route_count_e) ≈ min Σ (route_count_e - mean)^2
```

#### 5. 图文说明

| 配置 | shape | CUDA Graph | 质量风险 | 吞吐稳定性 |
|---|---|---|---|---|
| dropless | 动态 | 难捕获 | 低 | 随路由波动 |
| capacity + pad, 低 factor | 静态 | 可捕获 | 丢弃多 | 高 |
| capacity + pad, 高 factor | 静态但大 | 可捕获 | 丢弃少 | 显存/通信高 |

```text
tokens ──router──▶ top-K expert 队列
                     │
              route_count > capacity ? ──是──▶ drop（policy: probs/position）
                     │否
                grouped GEMM ──▶ combine(reduce) ──▶ 输出
```

#### 6. 拓展知识

- 与 T-A006 交叉：capacity 直接决定 all-to-all 体积；EP 开 overlap 时，A2A 藏进 FFN 计算能显著缓解热点。
- 与 T-A011 交叉：MoE 提供 `--moe-layer-recompute` 与细粒度 offload，激活管理粒度可到 per layer per expert。
- 与 T-A014 交叉：aux loss 权重过大本身会干扰主任务，调 routing 与调 LR 应分开做。

#### 7. 常见追问及回答要点

1. **dropless 一定更好吗？** ——质量上更好，但动态 shape 破坏 CUDA Graph 的静态假设，吞吐与显存更不可控；是否值得取决于质量预算。
2. **怎么判断路由失衡？** ——看每 expert 的 token 直方图与 A2A 等待时间；热点 expert 队列长期顶到 capacity 即为失衡。
3. **capacity factor 和 EP 规模的关系？** ——EP 越大每 rank 专家越少，单专家承受 token 越多，capacity 与 A2A 体积的敏感性都上升。

#### 8. 易错点与反例

- 反例：直接开 dropless 又开 CUDA Graph，动态 shape 让捕获失败或 replay 出错。
- 反例：只调 capacity 不加 aux loss，热点专家长期溢出、冷门专家闲置，丢弃率与不均衡同时恶化。
- 易错：把 capacity factor 当成“越大越安全”，忽略它同时放大显存与 A2A 体积。

#### 9. 实践与验证

`[待验证]` 固定模型与 batch，扫描 capacity factor ∈ {1.0, 1.25, 2.0}，记录丢弃率、每 expert token 分布、step time 与 eval 指标；oracle 是“丢弃率可控且 eval 不劣化时吞吐最优”。本仓库未运行 MoE 实验。

#### 10. 评分标准

- 不合格：知道 MoE 有 expert，却说不清 capacity 的作用。
- 合格：能解释 dropping 与 dropless 的取舍及 capacity 的计算口径。
- 良好：能联系 aux loss、drop policy、CUDA Graph 静态 shape 给出配置建议。
- 优秀/专家：能定量说明路由方差与丢弃率对吞吐/质量的影响，设计 A2A overlap 与 capacity 的联合调优验证。

**证据与来源：** `insights/05-megatron-architecture.md` 第 11.2/11.6/13 章（capacity factor、drop policy、dispatcher、A2A overlap，`C`/`I`）。cap 公式与收敛影响为工程推断（`I`），实测收益 `U / 待验证`。

---
### T-A004. FP8 与 Transformer Engine 集成：什么时候该开、怎么保证数值不出问题？

- 难度：高级
- 知识域：T09 混合精度与数值稳定
- 考察能力：精度机制、跨 rank 一致性、失败回退设计
- 题型：机制 + 系统集成
- 面试频率：中高频
- 前置知识：fp16/bf16 与 loss scaling、Adam fp32 master、TE fused kernel
- 关联题目：T-A007（优化器状态）、T-A014（稳定性）、T-A012（吞吐归因）
- 事实状态：TE/FP8 的机制描述 `[已确认]`；本仓库未执行任何 FP8 训练 `[待验证]`

#### 1. 背景知识

混合精度是显存与通信的主要杠杆：fp16 前反向激活减半，FP8（Hopper 之后）把 QKV 投影与 MLP 等 GEMM 再减半。风险来自动态范围：

- fp16 exponent 只有 5 位，需要动态 loss scaling 防梯度下溢/溢出。
- bf16 动态范围接近 fp32 但尾数 7 位，通常无需 loss scaling。
- FP8 分 E4M3（宽动态范围，适合 forward 权重/激活）与 E5M2（更大范围，适合梯度），需要 delayed scaling：用前一次迭代的 amax 统计设置 scale（`C`，官方/文档）。

Transformer Engine 提供 fused GEMM+activation 与自动精度分配，Megatron 侧是“定并行与调度”，TE 侧是“定 kernel 与精度”（`C`）。

#### 2. 问题分析

开 FP8 要处理三层问题：

1. **精度分层**：哪些 GEMM 上 FP8、哪些保持 bf16/fp32（LayerNorm、softmax、master 参数通常不 FP8）。
2. **scale 一致性**：`I` 推断，TP 下各 rank 若独立统计 activation 幅值，可能产生不一致的 scale，降低收敛确定性；需要把 scale 统计与传播对齐。
3. **失败回退**：硬件不支持 FP8、TE 未安装或 kernel 不支持时必须有 fallback（原生 PyTorch 路径），否则训练直接不可用。

判据：先用单卡固定 seed 做基线 diff，确认数学等价性，再逐步开 FP8 层，避免“一次全开”难以定位。

#### 3. 具体答案

面试中可以这样回答：

> 我会先确认三件事：硬件是否支持 FP8（Hopper/Ada/Blackwell 起优化）、TE 是否可用、optimizer 是否保留 fp32 master。都满足才考虑开。
>
> 配置上，forward 权重/激活用 E4M3、梯度用 E5M2，采用 delayed scaling：每步用上一步的 amax 统计算 scale，避免幅值突变。master 参数仍 fp32，Adam 状态不受 FP8 影响。LayerNorm、softmax 这类对精度敏感的算子保持高精度，只对 GEMM 走 FP8。
>
> 集成风险我重点防两点：一是 TP 下各 rank 的 scale 统计要对齐，否则梯度数值不一致（`I`）；二是必须有 fallback——硬件或 TE 不满足时切回 bf16/fp16，而不是直接崩。验证流程是先做 TP=1 单卡 seed 基线，逐层开 FP8 对比 loss 与若干层参数 diff，最后再多卡验证（`U / 待验证`）。

#### 4. 技术洞察

```text
delayed scaling（示意）：
  scale_t = amax_max(history) / FP8_max           # E4M3: FP8_max=448
  x_fp8  = clamp(x / scale_t, -FP8_max, FP8_max)
  反量化：x ≈ x_fp8 · scale_t
  amax_history 滑动窗口记录最近若干步，防单步异常

精度分配（I）：
  GEMM(QKV/MLP): FP8        LN/softmax/residual: bf16/fp32
  master params: fp32       Adam m/v: fp32
TP 一致性：各 rank 的 per-tensor scale 应来自一致统计，避免梯度分片数值漂移
```

#### 5. 图文说明

| 精度 | 激活/权重字节 | 动态范围 | 需要 scaling | 典型用途 |
|---|---|---|---|---|
| fp32 | 4 | 大 | 否 | master、LN、softmax |
| bf16 | 2 | ≈fp32 | 否 | 默认前反向 |
| fp16 | 2 | 小（5 位 exponent） | 是（loss scaling） | 旧默认 |
| FP8 E4M3 | 1 | 中 | 是（delayed） | forward 权重/激活 |
| FP8 E5M2 | 1 | 大 | 是（delayed） | 梯度 |

```text
x ──amax 统计──▶ scale_t ──▶ x_fp8 ──TE GEMM──▶ y_fp8 ──▶ 反量化 ──▶ 累加(fp32)
                  ▲
            上一迭代 amax_history（delayed）
```

#### 6. 拓展知识

- 与 T-A012 交叉：FP8 的吞吐收益最终要体现在 MFU 上，不能只看单算子 profiler。
- 与 T-A007 交叉：delayed scaling 的 scale 元数据可能需要随 checkpoint 保存，否则恢复后首个 step 的数值会跳。
- 与 T-A003 交叉：MoE 路由的 top-K 打分若在 FP8 下计算，可能改变专家选择，需单独验证。

#### 7. 常见追问及回答要点

1. **FP8 为什么不能全模型使用？** ——对幅值敏感的算子（softmax、归约、LayerNorm）在 FP8 下误差不可接受，且 master 参数需保持 fp32 以稳定更新。
2. **delayed scaling 为什么“延迟”？** ——当步的 scale 需要该步的 amax，而 amax 在计算前未知，只能用历史统计；滑动窗口防止单步异常污染。
3. **TP 下 FP8 的额外风险？** ——`I` 推断各 rank 统计不一致会引入数值漂移，需要对齐 scale 统计或传播。

#### 8. 易错点与反例

- 反例：在不支持 FP8 的硬件上强行开启，又没有 fallback，训练直接不可用。
- 反例：各 TP rank 各自统计 activation amax 并各自设置 scale，导致同一逻辑参数的梯度分片数值不一致。
- 易错：把 FP8 当成“无损加速开关”，忽略 scale 元数据在 checkpoint 恢复时的传播问题。

#### 9. 实践与验证

`[待验证]` 单卡固定 seed 的 bf16 基线 vs 逐层开 FP8，比较 loss 曲线、grad norm 与若干层参数 diff；oracle 是“loss 差异在容差内且 MFU 上升”。多卡与 MoE 路由部分需专门验证。本仓库未执行 FP8 训练。

#### 10. 评分标准

- 不合格：只知道 FP8 更省显存，说不出格式与 scaling。
- 合格：能区分 E4M3/E5M2 与 bf16/fp16 的适用面。
- 良好：能说明 delayed scaling、精度分层与 fallback 设计。
- 优秀/专家：能指出 TP/MoE/checkpoint 与 FP8 的交互风险，并给出可执行的逐层验证流程。

**证据与来源：** `insights/05-megatron-architecture.md` 第 12A 章（混合精度、FP8 架构要点，`C`/`I`，明确标注“本仓库未执行 FP8 训练”）。TP 一致性与 checkpoint 传播为推断（`I`），实测 `U / 待验证`。

---

### T-A005. context/sequence parallel 的通信代价：SP 真的“零额外带宽”吗？CP 又付出了什么？

- 难度：高级
- 知识域：T06 序列与上下文并行
- 考察能力：通信量推导、反直觉结论辨析、方案边界
- 题型：定量分析 + 机制对比
- 面试频率：高频
- 前置知识：ring allreduce 的 RS+AG 分解、f/g 共轭算子、attention 的 s² 项
- 关联题目：T-A001（选型）、T-A006（EP 通信）、T-A009（打包序列）
- 事实状态：SP 带宽等价论据 `[已确认]`；CP 实测收益 `[待验证]`

#### 1. 背景知识

- **SP**：TP 只切 GEMM 区，LayerNorm/Dropout 在 TP 组内是复制的。SP 把这些逐序列元素独立的算子沿序列维切分，用 `g`（forward all-gather / backward reduce-scatter）与 `ḡ` 做 layout 转换。
- **CP**：沿序列维切激活，**不切权重**；attention 需在 CP 组内交换信息（ring/zigzag 等），因此 backward 需要对复制的权重梯度做额外归约（`C`，M02 明确记录）。

#### 2. 问题分析

两个反直觉点必须先讲清：

1. **SP 不增加带宽**：ring all-reduce 本质 = reduce-scatter + all-gather，把一次 all-reduce 拆成 g/ḡ 两个半步并移到正确位置，总带宽不变（`C`）。SP 的真实收益在激活显存与 wait 时间分布，而不是“省带宽”。
2. **CP 不是免费的**：CP 的 attention 通信随序列长增长；且因权重复制，backward 需要额外归约权重梯度，该归约常复用 DP 域（`C`）。

判据：SP 适合“激活显存是瓶颈但不想牺牲计算密度”；CP 适合“序列 ≥8K、attention 的 s² 项成为主导，且能把序列维通信与计算重叠”。

#### 3. 具体答案

面试中可以这样回答：

> 先纠正一个常见误解：SP 并不是“多了一层通信”。它把 TP 原来的一次 all-reduce 拆成 all-gather 与 reduce-scatter 两个半步，而 ring all-reduce 本来就是这两步的组合，所以带宽等价（`C`）。它真正的好处是把 LayerNorm/Dropout 这些复制区从 `10·s·b·h` 降到按序列/隐藏维切分，激活显存明显下降。
>
> CP 则要付出两笔代价。一是 attention 的跨 rank 通信随序列长增长，ring/zigzag 需要在环上推 KV 与 softmax 统计，必须与计算重叠才划算。二是 CP 不切权重，backward 要对复制的权重梯度额外归约，工程上通常复用 DP 域——也就是说 CP 的“通信域”是 DP∩CP 的复合。
>
> 选型上：序列 8K 以上、attention 成瓶颈时开 CP；只想省激活又不想动序列语义时开 SP。两者可以叠加，但 axis order 必须正确，否则 TP 与 CP 的 stride 混淆会产生隐式错误。

#### 4. 技术洞察

```text
SP 的带宽等价（C）：
  ring all-reduce(x) ≡ reduce_scatter(x) + all_gather(x)
  把原 all-reduce 替换为 g (AG forward / RS backward) + ḡ (RS forward / AG backward)
  → 总通信体积不变；收益 = 激活显存 / 等待分布

SP 激活收益（论文口径，C）：
  TP-only：非 TP 区 ~10·s·b·h 不被 t 除
  TP+SP ：这些激活按序列/隐藏维被 t 切分

CP 代价：
  每步 attention 通信量 ∝ s（序列维分块逐步推进）
  backward 额外归约复制权重梯度：域 = DP ∩ CP
```

#### 5. 图文说明

| 轴 | 切什么 | 权重是否复制 | 主要通信 | 额外梯度归约 | 适用 |
|---|---|---|---|---|---|
| SP | 序列（LN/Dropout 区） | 否（属 TP） | g/ḡ | 无（带宽等价） | 省激活 |
| CP | 序列激活 | 是 | 环上 KV/统计 或 P2P | 是（复用 DP 域） | 序列 ≥8K |

```text
TP-only:   [GEMM(t)] [LN 复制×t] [GEMM(t)] [Dropout 复制×t]
TP+SP :    [GEMM(t)] [LN 序列切] [g] [GEMM(t)] [ḡ] [Dropout 序列切]
                        └─ 复制区变序列并行，激活 /t ─┘
```

#### 6. 拓展知识

- 与 T-A009 交叉：packed sequence 与 CP 叠加时，`cu_seqlens` 等元数据必须随序列切片一起切分，否则 varlen attention 边界错位。
- 与 T-A003/A006 交叉：官方要求 TP+EP 组合时必须开启 SP，因为 token dispatch 需要统一 layout。
- M02 记录 decoder 与 expert rank generator 产生的 PP group 必须一致，这是多 axis 组合下的不变量（`C`）。

#### 7. 常见追问及回答要点

1. **SP 不增带宽，那它省的是什么？** ——省激活显存和等待时间，让更大 microbatch/更高 batch 密度成为可能，间接抬 MFU。
2. **CP 为什么不能像 TP 一样切权重？** ——CP 的目标是降低序列维激活与 attention 代价；若再切权重，attention head 与序列的对应关系会复杂化，且权重复制带来的额外梯度归约是可控成本。
3. **CP 何时不划算？** ——序列不够长时 attention 不是瓶颈，CP 的通信额外开销超过收益；且若与 DP 组合不佳，梯度归约域变大。

#### 8. 易错点与反例

- 反例：以为 SP 增加了通信开销而拒绝开启，错失了几乎零带宽代价的激活显存收益。
- 反例：开 CP 却不做通信重叠，序列维通信成为主导，step time 反而上升。
- 易错：axis order 把 TP 与 CP 的 stride 写反，同一 rank 同时被当作 TP 与 CP rank，产生隐式错误而非显式报错。

#### 9. 实践与验证

`[待验证]` 对比 TP-only 与 TP+SP 的峰值激活显存、step time；对比有无 CP 在长序列下的 attention 时间与通信占比。oracle 是“SP 降低峰值激活且 step time 基本不变；CP 在长序列下降低 attention 时间”。需多卡与长序列环境，本仓库未执行。

#### 10. 评分标准

- 不合格：把 SP 与 CP 混为一谈，或认为 SP 必然增加带宽。
- 合格：能说明 SP 的 g/ḡ 转换与 CP 切序列不切权重。
- 良好：能推导 SP 的带宽等价，指出 CP 的额外权重梯度归约。
- 优秀/专家：能结合 axis order、packed sequence、DP∩CP 复合归约域给出可验证的配置与失败排查路径。

**证据与来源：** `insights/05-megatron-architecture.md` 第 7/9 章（SP 带宽等价、CP 不切权重与额外归约，`C`/`I`）；`megatron-lm/01-modules/M02-parallelism/README.md`（轴定义与不变量，`C`）。通信量与收益为推导（`I`），实测 `U / 待验证`。

---

### T-A006. expert parallel 与 EP 通信：all-to-all 什么时候是瓶颈，怎么判断该不该开 EP？

- 难度：高级
- 知识域：T02 分布式初始化与通信组
- 考察能力：通信域分析、dispatcher 选型、边界判定
- 题型：系统设计 + 失效分析
- 面试频率：中高频
- 前置知识：MoE dispatch/combine、NVLink 与 IB 差异、Parallel Folding
- 关联题目：T-A003（capacity）、T-A005（SP 与 EP 的 layout 约束）、T-A012（归因）
- 事实状态：dispatcher 类型与推荐配置 `[已确认]`；EP 收益 `[待验证]`

#### 1. 背景知识

EP 把 MoE 的 experts 分布到多个 rank，token 通过 all-to-all 被派发到持有目标 expert 的 rank 再计算。可用 dispatcher：

- `allgather`：先全量汇聚再按 expert 分拣，简单但通信量大。
- `alltoall`：直接交换 token，通信更省。
- `flex`（DeepEP/HybridEP）：去冗余、跨节点融合，面向细粒度 MoE（`C`，官方）。

约束：EP 与 TP 组合时必须开启 SP（统一 layout）；Parallel Folding 把 attention 与 MoE 层的并行配置解耦（attention 用 TP×CP×DP×PP，MoE 用 ETP×EP×EDP×PP），因为 MoE 层喜欢高 EP 而没必要高 TP（`C`）。

#### 2. 问题分析

判断 EP 是否值得，取决于 A2A 成本与显存收益的相对大小：

1. **A2A 体积**：约 `tokens × hidden × topK`，随 EP 组规模与 batch 变化；跨节点时落到 IB/RDMA，延迟显著上升。
2. **路由均衡**：热点专家会把 A2A 变成“少数 rank 长时间等待”，是 MoE 吞吐的主要瓶颈（`C`，官方列为优化重心）。
3. **收益**：EP 让每 rank 只需装 `experts/EP` 个专家，省的是专家参数显存；但若 experts 很少（如 8 个）且 batch 小，A2A 开销占比重，可能不如每 rank 复制全部专家的 DP 式（`I`）。

结论：EP 优势随 experts 数与 batch 增大而放大；跨节点且 NVLink 不全覆盖时需谨慎。

#### 3. 具体答案

面试中可以这样回答：

> 我会先估 A2A 的量级：token 数、hidden、top-K 决定派发体积，再问它落在哪个域——同节点 NVLink 还是跨节点 RDMA。若 A2A 要跨节点且没有高带宽互联，延迟会成为主项，这时 EP 往往不划算。
>
> dispatcher 选择：结构简单、规模不大用 alltoall；需要去冗余、跨节点融合用 flex（DeepEP/HybridEP）。无论哪种，我会把 A2A 藏进 FFN 计算里，用 `--overlap-moe-expert-parallel-comm` 加相邻 microbatch 的 FWD/BWD 合并来重叠。
>
> 还有一个硬约束：EP 与 TP 组合时必须开 SP，否则 token dispatch 的 layout 不统一。若用 Parallel Folding，可以让 attention 层用高 TP/CP、MoE 层用高 EP/ETP=1，各取所需。
>
> 我判断“该不该开 EP”的标准是：experts 数够多、batch 够大、A2A 能落在高带宽域且能被重叠；三条不满足时优先 DP 式复制 experts。

#### 4. 技术洞察

```text
A2A 体积量级（I）：
  dispatch ≈ num_tokens · topK · hidden · dtype_bytes
  combine  ≈ 同量级（反向再传一遍）
EP 显存收益：per_rank_expert_params ≈ total_expert_params / EP
EP 有效条件（工程判据，I）：
  experts/rank 足够多  ∧  batch 足够大  ∧  A2A 在高带宽域  ∧  可 overlap
```

#### 5. 图文说明

| dispatcher | 机制 | 通信量 | 适用 |
|---|---|---|---|
| allgather | 全量汇聚后分拣 | 高 | 简单/小规模 |
| alltoall | 直接交换 token | 中 | 常规 EP |
| flex (DeepEP/HybridEP) | 去冗余、跨节点融合 | 优化 | 细粒度 MoE |

```text
token(r) ──router──▶ 目标 expert 所在 rank
   │
   ├─ dispatch: all-to-all 把 token 送到持有 expert 的 rank
   ├─ grouped GEMM 在本 rank 的 expert 上计算
   └─ combine: all-to-all 把输出加权求和回原 rank
        （用相邻 microbatch 重叠，把 A2A 藏进 FFN 计算）
```

#### 6. 拓展知识

- 与 T-A003 交叉：capacity 决定单次 A2A 的缓冲上限，与 overlap 联合调优。
- 与 T-A005 交叉：SP 是 EP+TP 的前置条件，layout 不一致时 token 会被错误派发。
- 与 T-A011 交叉：`--moe-layer-recompute` 与 expert offload 可缓解 MoE 层激活压力，但与 A2A 重叠策略会相互影响。

#### 7. 常见追问及回答要点

1. **EP 什么时候反而不如 DP？** ——experts 少、batch 小、A2A 跨节点时，通信开销盖过显存收益，复制 experts 更简单更快。
2. **为什么 EP+TP 必须开 SP？** ——MoE 的 token dispatch 需要统一 layout，SP 保证 TP 组内激活的序列/隐藏切分一致，否则派发目标错位。
3. **Parallel Folding 解决了什么？** ——attention 与 MoE 层的最优并行不同，Folding 把二者的并行配置解耦，避免被强制共享单一拓扑。

#### 8. 易错点与反例

- 反例：experts 只有 8 个、batch 很小却强行开大 EP，A2A 开销占比高，吞吐不如 DP 式复制。
- 反例：EP+TP 忘记开 SP，token layout 不一致，计算看似跑通但结果错误。
- 易错：开了 EP 却不做 A2A overlap，all-to-all 每次都是完整停机，吞吐被砍半量级（`I`）。

#### 9. 实践与验证

`[待验证]` 对比 EP 与 DP 式复制在相同 batch 下的 step time、每 expert token 直方图与 A2A 时间占比；oracle 是“EP 的 A2A 时间被 overlap 隐藏后 step time 更低”。需 MoE 模型与多节点环境，本仓库未执行。

#### 10. 评分标准

- 不合格：只知道 EP 是“切 experts”，说不清 A2A。
- 合格：能说明 dispatch/combine 与三种 dispatcher 的差异。
- 良好：能估算 A2A 体积、指出跨节点与路由失衡的瓶颈，给出 overlap 方案。
- 优秀/专家：能结合 SP 前置约束、Parallel Folding、capacity 与 overlap 给出端到端配置与验证指标。

**证据与来源：** `insights/05-megatron-architecture.md` 第 11 章（EP、dispatcher、Parallel Folding、A2A overlap，`C`/`I`）。A2A 体积公式与收益判据为推断（`I`），实测 `U / 待验证`。

---
### T-A007. optimizer state 的内存模型与分片：distributed optimizer 到底省了什么、又约束了什么？

- 难度：高级
- 知识域：T08 优化器与梯度更新
- 考察能力：内存核算、状态一致性、checkpoint 耦合
- 题型：定量分析 + 系统约束
- 面试频率：高频
- 前置知识：Adam 的 m/v、fp32 master、reduce-scatter vs all-reduce、sharded state dict
- 关联题目：T-A010（world size 变化）、T-A004（FP8 master）、T-A012（OOM 归因）
- 事实状态：optimizer factory 与 dist optimizer 行为 `[已确认]`；具体省量 `[待验证]`

#### 1. 背景知识

Adam 的显存常被低估。fp16 训练典型每参数占用（`I`）：

- 权重 fp16：2B
- 梯度 fp16：2B
- fp32 master：4B
- Adam 一阶 m：4B
- Adam 二阶 v：4B

合计约 16B/参数。分布式优化器把梯度与优化器状态按 DP rank 分片（1/DP），用 reduce-scatter 替代 all-reduce（`C`）。Megatron 的组合特征是“TP 全量、DP 分片”（`C`，M02/insights）：M02 本身不决定 optimizer 是否 sharded，只提供通信域。

#### 2. 问题分析

要点在于区分它省了什么、约束了什么：

1. **省的是状态**：分片对象是梯度与 optimizer state，参数量本身在 DP 副本间仍复制（除非叠加 FSDP）。
2. **通信形态改变**：reduce-scatter 后每 rank 只持有自己负责的分片，update 完再 all-gather 回副本；本质是把 all-reduce 拆成两步。
3. **状态一致性约束**：checkpoint 必须与 shard 布局对齐，`GPTModel.sharded_state_dict` 是模型到 checkpoint 层的适配边界；distributed optimizer 的 shard 所有权不能与普通 model state 混淆（`C`）。
4. **与 TP 的关系**：optimizer 状态不随 TP 分片（TP 全量），所以“TP 越大状态越省”是误解——省状态的是 DP 分片或 FSDP。

#### 3. 具体答案

面试中可以这样回答：

> 我先算账。fp16 + fp32 master 的 Adam，每参数约 2+2+4+4+4=16 字节；一个 10B 模型光状态就 160GB，单卡放不下，这是分布式优化器存在的根本原因。
>
> 分布式优化器把梯度与 optimizer state 按 DP rank 分片，用 reduce-scatter 得到本 rank 负责的分片，Adam 只更新这一片，再 all-gather 回各副本。它省的是状态，不是参数——参数在 DP 维度仍然复制，要再切参数字节得上 FSDP。
>
> 关键约束在 checkpoint：sharded state dict 必须记录 shard 布局，load 时布局不一致会数值漂移。M05 把 checkpoint 和 optimizer 一起讲就是因为这个“状态集合点”。另外要强调，TP 不参与 optimizer 状态分片，所以指望靠加大 TP 省优化器状态是错的。
>
> 最后提醒和 ZeRO/FSDP 的关系：它们是同一族的状态分片方案，distributed optimizer 可以理解为“DP 维的 ZeRO-1/2 式分片”（`I`）。

#### 4. 技术洞察

```text
每参数状态（fp16 + fp32 master，I）：
  weight(2) + grad(2) + master(4) + m(4) + v(4) = 16 B/param
分布式优化器：
  state_total ≈ P · 12B / DP        # master + m + v 分片，grad 也分片
  step: reduce_scatter(grad) → local Adam → all_gather(param)
  ≡ all_reduce 拆成 RS + AG（与 SP 的带宽等价思路同源）
```

#### 5. 图文说明

| 对象 | 普通优化器 | distributed optimizer |
|---|---|---|
| 权重 | 每 rank 复制（DP） | 每 rank 复制（DP） |
| 梯度 | 全量 all-reduce | reduce-scatter 分片 |
| optimizer state | 每 rank 全量 | 1/DP 分片 |
| 通信原语 | all-reduce | reduce-scatter + all-gather |
| checkpoint | 稠密/按 rank | sharded state dict + metadata |

```text
grad(all) ──reduce_scatter──▶ grad_shard(rank) ──Adam──▶ param_shard(rank)
                                                              │
                                              all_gather ◀────┘
                                              param(all, 下一轮)
```

#### 6. 拓展知识

- 与 T-A010 交叉：world size 变化会改变 DP 分片数，distributed checkpoint 的 shard 布局必须能 reshard。
- 与 T-A004 交叉：FP8 训练下 master 仍 fp32，delayed scaling 的 scale 元数据也需要进入 checkpoint。
- 与 T-A011 交叉：optimizer 有 CUDA Graph 路径时，step 与 full-iteration graph 可能共享 capture stream 与 graph memory pool，NCCL MemPool 注册须覆盖实际通信 group（M05 记录，`C`）。

#### 7. 常见追问及回答要点

1. **distributed optimizer 和 FSDP 的区别？** ——前者只分片状态，参数仍复制；FSDP 连参数字节一起分片，用 all-gather 换取更小驻留。
2. **为什么用 reduce-scatter 而不是 all-reduce？** ——状态已按 rank 分片，每 rank 只需自己的梯度分片，all-reduce 会把不需要的完整梯度也算出来，浪费带宽。
3. **checkpoint 为什么会不兼容？** ——shard 布局与并行配置（尤其 DP）绑定，配置变化而不做 reshard 会导致 load 失败或数值漂移。

#### 8. 易错点与反例

- 反例：以为加大 TP 能省优化器状态——optimizer 状态不随 TP 分片，只有 DP/FSDP 分片。
- 反例：只保存模型不保存 optimizer state，恢复后 Adam 的 m/v 从零开始，loss 出现跳变。
- 易错：混用不同 ZeRO/分片层级，导致 shard 所有权与 checkpoint metadata 不一致。

#### 9. 实践与验证

`[待验证]` 记录开启 distributed optimizer 前后的每卡峰值显存、step time 与 checkpoint 体积；oracle 是“状态显存显著下降且 step time 增幅可接受、load 后 loss 连续”。需多卡环境，本仓库未执行。

#### 10. 评分标准

- 不合格：只知道“有优化器状态”，算不出内存账。
- 合格：能列出 Adam 各部分字节并说明 distributed optimizer 分片对象。
- 良好：能解释 reduce-scatter/all-gather 替换 all-reduce 的动机与 checkpoint 耦合。
- 优秀/专家：能区分 DP 分片与 FSDP、处理 world size 变化的 reshard、并指出 CUDA Graph/NCCL 注册的边界。

**证据与来源：** `insights/05-megatron-architecture.md` 第 5.4/12.1/12.2/12.5/15 章（distributed optimizer 特征与显存账，`C`/`I`）；`megatron-lm/01-modules/M05-optimizer-checkpointing/implementation.md`（factory、sharded state、dist optimizer，`C`）。16B 估算与 ZeRO 类比为推断（`I`），实测 `U / 待验证`。

---

### T-A008. RoPE 与长序列外推：旋转位置编码在并行训练里会踩哪些坑？

- 难度：高级
- 知识域：T06 序列与上下文并行
- 考察能力：模型侧细节与分布式实现的交互、边界识别
- 题型：机制 + 系统集成
- 面试频率：中频
- 前置知识：RoPE 的复数/旋转视角、base 频率、位置插值与频率缩放、packed sequence
- 关联题目：T-A005（CP）、T-A009（打包与 position ids）、T-A012（长序列性能）
- 事实状态：RoPE 公式为通用模型知识（`I`）；本仓库模块文档未覆盖 RoPE 细节；本仓库未做长序列实验 `[待验证]`

#### 1. 背景知识

RoPE 把位置信息编码为对 Q/K 的分组旋转变换。每个二维分组的旋转频率为

```text
θ_i = base ^ (-2i/d),  i = 0, 1, ..., d/2 - 1
q_rot = R(θ, pos) · q,  k_rot = R(θ, pos) · k
```

长序列外推关注三点：训练长度之外的 extrapolation、base 频率调整（NTK/YaRN 等缩放）、位置 id 的重置与连续性。Megatron 模块文档（M01）覆盖的是装配与 kernel 选择，未细化 RoPE 具体实现，故本节结论以通用模型知识为主并标 `I`。

#### 2. 问题分析

RoPE 在并行训练中的坑集中在“位置语义”与“序列切分”的交互：

1. **外推失败**：直接推理超过训练长度会因频率外推退化，需插值或频率缩放（`I`）。
2. **position ids 语义**：packed sequence 下必须按文档重置位置，否则跨文档拼接被当作连续位置（与 T-A009 交叉）。
3. **CP 下的位置一致性**：CP 把序列切到多个 rank，每段的 position ids 必须相对全局序列而非局部切片，否则 attention 位置错乱（`I`）。
4. **base 修改与微调**：改变 base 频率通常需要相应微调，否则短期可能能跑、长期收敛变差。

#### 3. 具体答案

面试中可以这样回答：

> 先说结论：RoPE 本身是逐 token 的确定性变换，并行化时它不会像 TP 那样引入数值问题，但它对“位置 id 从哪来、是否跨文档重置”极度敏感。
>
> 外推上，我不会直接把推理长度拉到训练长度之外，而是用位置插值或频率缩放（NTK/YaRN 一类方法）重设 base 与缩放系数，并配合少量长序列微调（`I`）。改 base 不是纯推理期 hack，训练分布要对得上。
>
> 并行侧我重点看两点：一是 packed sequence 必须按文档重置 position ids，否则拼接处被当成连续位置；二是 CP 切分时每段的位置必须相对全局序列计算，不能让每个 CP rank 从 0 开始，否则 attention 的旋转相位错位。
>
> 验证我会用长序列 eval（比训练长度更长）看 loss/perplexity 是否退化，并对比打包与非打包下的位置连续性（`U / 待验证`）。

#### 4. 技术洞察

```text
RoPE（按二维分组旋转）：
  θ_i = base^(-2i/d)
  [q_{2i}, q_{2i+1}] ← rotate([q_{2i}, q_{2i+1}], pos·θ_i)
  相对位置性质：<q_rot(p), k_rot(m)> 只依赖 (p - m)

长序列外推的常见缩放（I，通用做法）：
  NTK-aware：base' = base · (scale)^(d/(d-2))
  YaRN      ：混合频率插值与注意力温度缩放
  position interpolation：pos' = pos · (L_train / L_target)
```

#### 5. 图文说明

| 场景 | 位置 id 来源 | 风险 | 处理 |
|---|---|---|---|
| 常规训练 | 0..s-1 | 无 | 默认 |
| packed sequence | 每文档重置 | 跨文档被当连续 | reset position ids |
| CP 切分 | 相对全局序列 | 每 rank 从 0 起会相位错位 | 用全局 offset |
| 长序列外推 | 缩放后的位置 | 频率外推退化 | 插值/频率缩放 + 微调 |

```text
文档A [0..a) 文档B [0..b)
packing: pos = [0..a-1, 0..b-1]   # 重置，避免 A 尾部与 B 头部被视为相邻
CP=2:    全局 s=16, rank0 拿 pos 0..7, rank1 拿 pos 8..15（不是 0..7）
```

#### 6. 拓展知识

- 与 T-A009 交叉：`reset_position_ids`、`reset_attention_mask` 会改变缓存与 `cu_seqlens` 生成（`C`，M04）。
- 与 T-A005 交叉：CP 的 ring attention 下 RoPE 相位必须与全局位置一致，否则 softmax 统计合并出错。
- 与 T-A012 交叉：长序列下 attention 的 s² 项主导，RoPE 本身不是瓶颈，但位置处理错误常表现为“loss 异常而非报错”。

#### 7. 常见追问及回答要点

1. **RoPE 为什么天然支持相对位置？** ——旋转内积只依赖位置差 `(p-m)`，这让它比绝对位置编码更易外推。
2. **改 base 不微调会怎样？** ——`I` 推断短程可能暂时可用，但长程频率与训练分布不匹配，收敛质量下降。
3. **CP 下最容易错在哪？** ——把局部切片位置当全局位置，导致各 CP rank 的相位不一致，attention 结果静默错误。

#### 8. 易错点与反例

- 反例：packed sequence 不重置 position ids，跨文档位置被当作连续，长上下文能力被破坏且不易察觉。
- 反例：CP 下每 rank 从 0 编号位置，attention 相位错位，loss 异常但不报错。
- 易错：把改变 base 当作纯推理期技巧，忽略训练分布必须匹配，导致下游退化。

#### 9. 实践与验证

`[待验证]` 用长于训练长度的样本做 eval，比较默认、位置插值、频率缩放三者的 perplexity；并对比打包/非打包、CP=1/CP>1 下位置连续性的数值一致性。本仓库模块文档未覆盖 RoPE 实现细节，本节标注为推断，未运行实验。

#### 10. 评分标准

- 不合格：只说“RoPE 是旋转位置编码”，讲不出外推与并行坑。
- 合格：能写出 RoPE 公式与相对位置性质。
- 良好：能说明外推缩放方法与 packed sequence 的位置重置。
- 优秀/专家：能指出 CP 下位置相位一致性、base 修改与微调的耦合，并设计长序列验证。

**证据与来源：** 本节 RoPE 公式与缩放方法为通用模型知识（`I`）；`megatron-lm/01-modules/M04-data-pipeline/README.md` 与 `implementation.md` 提供 `reset_position_ids`/`cu_seqlens` 的分支证据（`C`）；`insights/05-megatron-architecture.md` 第 9 章提供 CP 长序列背景（`I`）。长序列行为 `U / 待验证`。

---

### T-A009. sequence packing 与 loss mask：打包提效的代价是哪些正确性边界？

- 难度：高级
- 知识域：T12 数据管线与交付
- 考察能力：数据语义正确性、边界条件、与 attention 后端的契约
- 题型：数据流 + 正确性分析
- 面试频率：高频
- 前置知识：next-token 标签构造、causal mask、`cu_seqlens`、varlen attention
- 关联题目：T-A008（位置重置）、T-A005（CP 切片）、T-A013（数据重叠）
- 事实状态：`GPTDataset.__getitem__`、mask/position 生成、`get_batch` 分支 `[已确认]`；实际收敛影响 `[待验证]`

#### 1. 背景知识

打包把多条短样本拼成一条长序列，减少 padding 浪费、提升 GPU 利用率。M04 记录的语义（`C`）：

- `GPTDataset.__getitem__` 把文本转 long tensor；配置 `add_extra_token_to_sequence` 时用 `text[:-1]`/`text[1:]`，否则 labels 为左移后的 text，末位填 pad token。
- pad 位置从 loss mask 排除，输入中的 pad token 映射为安全 token。
- `_get_ltor_masks_and_position_ids` 按配置生成 causal attention mask、loss mask 与 position ids；跨文档 masking 时额外生成每个 packed segment 的起止位置与最大长度，供 varlen attention 使用。
- `reset_position_ids`、`reset_attention_mask`、`eod_mask_loss`、inter-document masking 会改变缓存与 `cu_seqlens` 生成。

#### 2. 问题分析

打包提效的代价是三条正确性边界：

1. **loss mask 对齐**：mask 必须与有效 label 一一对应；标签左移后 mask 也要相应左移，否则把“预测下一个 token”的监督算在错误位置。
2. **跨文档隔离**：若不 reset attention mask，文档 A 的 token 会 attend 到文档 B，破坏样本独立性；position ids 也要重置（与 T-A008 交叉）。
3. **attention 后端契约**：packed 的 flatten tensor 必须伴随 `cu_seqlens` 等 metadata；不同 attention 后端（varlen vs 稠密）对 metadata 的要求不同，缺失会静默算错。

M04 明确把“loss mask 必须与有效 label 对齐”“packed sequence 的 flatten tensor 必须伴随 cu-sequence metadata”列为不变量（`C`）。

#### 3. 具体答案

面试中可以这样回答：

> 打包的收益是消掉 padding、把短样本拼满一条序列，但有三条必须守住。第一是 loss mask 与 label 对齐：标签左移一位预测下一个 token，mask 也必须跟着移，pad 位置要排除，否则在无意义的 pad 上算 loss。第二是跨文档隔离：用 `reset_attention_mask` 和 `reset_position_ids` 让文档 A 看不到文档 B、位置从每段重新编号，否则样本间信息泄漏，position 也不连续。
>
> 第三是对后端契约负责：打包后是 flatten 的一维张量，必须带 `cu_seqlens`（每段起止与最大长度），varlen attention 才知道段边界。如果后端是稠密的 block mask，则要生成对应的 block-diagonal mask。两者不能混。
>
> 验证我会做几个小样本的逐元素检查：构造两条已知文档，确认 loss mask 只在有效 label 处为 1、段边界与 `cu_seqlens` 一致、reset 后 position 从 0 重新开始。然后再看 CP 切片时这些 metadata 是否被正确切分（`U / 待验证`）。

#### 4. 技术洞察

```text
标签与 mask（I，示意）：
  tokens = [t0 t1 t2 ... tN]
  labels = [t1 t2 t3 ... pad]        # 左移一位
  loss_mask = [1 1 1 ... 0]          # pad 位为 0
  loss = -Σ loss_mask_i · log p(labels_i | tokens_≤i)

打包与隔离：
  packed = concat(doc_1, doc_2, ...)
  cu_seqlens = [0, len1, len1+len2, ...]
  reset_position_ids: pos 在每段从 0 重新计数
  reset_attention_mask: 段间不可见
```

#### 5. 图文说明

| 配置 | 作用 | 不设的后果 |
|---|---|---|
| add_extra_token_to_sequence | 标签构造方式 | 末位语义错位 |
| reset_position_ids | 每段位置重置 | 跨文档位置连续，外推失效 |
| reset_attention_mask | 段间不可见 | 文档间信息泄漏 |
| eod_mask_loss | EOD 位不作监督 | 在文档边界算 loss |
| inter-document masking | 生成段边界 metadata | varlen attention 段边界缺失 |

```text
doc1: [a b c]   doc2: [d e]
packed: [a b c d e]
cu_seqlens: [0, 3, 5]
pos:        [0 1 2 0 1]        # reset
attn:  a 只看 a..; d 只看 d..  # 段间隔离
loss_mask: 有效 label 处为 1，pad 处为 0
```

#### 6. 拓展知识

- 与 T-A008 交叉：position 重置与 RoPE 相位直接相关。
- 与 T-A005 交叉：CP 切序列时 `cu_seqlens` 必须随切片调整，否则段边界跨 rank 错位。
- 与 T-A013 交叉：sampler 决定每个 DP rank 看到的样本，打包粒度与 DP 切片相互影响。

#### 7. 常见追问及回答要点

1. **打包为什么能提效？** ——消除 padding 计算浪费，把多条短样本拼满，提升有效 token 占比与 GEMM 利用率。
2. **不 reset attention mask 的后果？** ——文档间信息泄漏，模型学到跨样本的伪关联，训练指标虚高、泛化变差。
3. **varlen 与稠密 mask 怎么选？** ——varlen 用 `cu_seqlens` 表达段边界、更适合打包；稠密 block mask 表达简单但浪费，需与 attention 后端能力匹配。

#### 8. 易错点与反例

- 反例：标签左移了但 loss mask 没跟着移，监督落在错误 token 上，loss 曲线异常但难定位。
- 反例：打包后只传 flatten tensor、不传 `cu_seqlens`，varlen attention 把整条序列当一段，段间隔离失效。
- 易错：CP 切片时忘记同步切 `cu_seqlens`/position，段边界错位。

#### 9. 实践与验证

`[待验证]` 用两条人工构造文档跑 `get_batch`，断言 loss_mask 仅在有效 label 处为 1、position 每段从 0 起、`cu_seqlens` 与段长一致；再在 CP>1 下断言 metadata 随切片正确切分。本仓库未运行数据管线实验。

#### 10. 评分标准

- 不合格：知道有 padding，但说不清 loss mask 与打包。
- 合格：能说明标签左移与 mask 对齐的基本要求。
- 良好：能解释跨文档隔离、position 重置与 `cu_seqlens` 的作用。
- 优秀/专家：能结合 varlen 后端契约、CP 切片与 `eod_mask_loss` 给出可执行的正确性验证。

**证据与来源：** `megatron-lm/01-modules/M04-data-pipeline/README.md` 与 `implementation.md`（`GPTDataset.__getitem__`、mask/position、`cu_seqlens`、`get_batch` 分支，`C`）。标签构造示例与收敛影响为推断（`I`），实测 `U / 待验证`。

---
### T-A010. 弹性恢复与 world size 变化：checkpoint reshard 到底要保住哪些状态？

- 难度：高级
- 知识域：T10 检查点、恢复与容错
- 考察能力：状态完整性、reshard 语义、恢复一致性
- 题型：系统设计 + 失效分析
- 面试频率：中高频
- 前置知识：sharded state dict、distributed checkpoint、RNG 状态、数据 iterator 位置
- 关联题目：T-A007（optimizer 分片）、T-A001（并行配置）、T-A015（权重交接）
- 事实状态：dist_checkpointing 与 save/load 涉及的状态 `[已确认]`；reshard 实测 `[待验证]`

#### 1. 背景知识

训练 checkpoint 不只是模型权重。M05 指出 save/load 同时涉及模型、optimizer、scheduler、RNG、tokenizer、数据加载状态和 tracker（`C`）。distributed checkpoint 用 sharded state dict + metadata，支持并行度变化时的 reshard（`C`，insights 14.1）。

关键约束：checkpoint 兼容性取决于模型结构与并行配置，`check_checkpoint_args` 会拒绝或警告关键结构参数不匹配（`C`）。异步保存与 non-persistent/local checkpoint 的清理与恢复语义尚未实测（`C`）。

#### 2. 问题分析

弹性恢复要回答“world size 变了，哪些状态必须能重建、哪些必须原样保留”：

1. **参数与优化器分片**：TP/PP/DP 任一改变都会改变 shard 布局，必须 reshard 到新布局；distributed optimizer 的 shard 所有权与 metadata 必须对齐（与 T-A007 交叉）。
2. **RNG 状态**：dropout 与数据 shuffle 依赖跨 rank 一致的随机流；不恢复 RNG 会让恢复点后的数值与不中断训练分叉。
3. **数据迭代位置**：必须记录已消费样本/token 数，否则会重复或跳过数据。
4. **tracker/调度器**：学习率调度器的步数、token 计数必须一致，否则 LR 曲线错位。

失败模式：只存模型 → Adam 状态从零、loss 跳变；不做 reshard 直接 load → shard 布局冲突。

#### 3. 具体答案

面试中可以这样回答：

> 我会先把“要恢复的状态”列全：模型参数、optimizer（含 m/v/master）、LR scheduler 步数、RNG、数据 iterator 位置、token/sample 计数。少任何一项，恢复后的训练都会和不中断的轨迹分叉。
>
> 然后处理 world size 变化。distributed checkpoint 存的是每 rank 的 shard + metadata，load 时按新并行配置 reshard：TP/PP/DP 任一变化都可能改变每个 shard 的归属和形状。这里最容易错的是 optimizer shard 与模型 shard 的布局必须同时对齐，否则数值漂移。
>
> RNG 和数据位置我单独强调：dropout 的随机流必须跨 rank 恢复一致，数据要按已消费 token 数跳过，否则重复训练或漏训。checkpoint 参数校验要匹配模型结构，关键结构参数不一致应该拒绝加载而不是警告放行。
>
> 验证方式是“中断—恢复”对拍：从第 N 步恢复，与从头连续跑到第 N+1 步比较 loss 与 grad norm，差异应在容差内（`U / 待验证`）。

#### 4. 技术洞察

```text
sharded checkpoint = Σ_rank (local_shard + metadata)
load 时 reshard：把旧布局的 shard 重映射到新 (TP,PP,DP) 布局
必须一致保存的状态集合（C/I）：
  model params, optimizer state(m,v,master), lr_scheduler_steps,
  RNG state, data_iterator position, consumed_tokens,
  tokenizer/tracker 版本
不变量：load 后 step N 的状态 ≡ 未中断训练到 step N 的状态（数值容差内）
```

#### 5. 图文说明

| 状态 | 是否随 world size 变化 | 丢失后果 |
|---|---|---|
| 模型参数 | 布局变化，需 reshard | load 失败/数值错位 |
| optimizer 状态 | 布局变化，需 reshard | m/v 重置，loss 跳变 |
| LR scheduler 步数 | 否，但须一致 | LR 曲线错位 |
| RNG | 否，但须一致 | dropout/数据分叉 |
| 数据位置 | 否，但须一致 | 重复/漏训 |

```text
save(old 4 卡布局) ──metadata──▶ 磁盘
                                    │
                             reshard（重映射 shard）
                                    ▼
load(new 8 卡布局)：param/optimizer 布局对齐 + RNG/数据/LR 复原
```

#### 6. 拓展知识

- 与 T-A015 交叉：训练 sharded checkpoint 到推理权重的转换是另一类“布局重映射”。
- 与 T-A007 交叉：optimizer shard 所有权与 checkpoint metadata 的一致性。
- M05 记录 optimizer CUDA Graph 与 full-iteration graph 可能共享 capture stream 与 graph memory pool（`C`），恢复路径要覆盖这些资源边界。

#### 7. 常见追问及回答要点

1. **world size 变大/变小要重新切什么？** ——参数与优化器状态的 shard 布局，按新 TP/PP/DP 做 reshard；RNG、数据位置等逻辑状态原样恢复。
2. **为什么保存 RNG？** ——dropout 随机流与 shuffle 需要跨 rank 一致；不保存则恢复后随机序列与不中断训练不同。
3. **参数校验该拒绝还是警告？** ——关键结构参数（隐藏维、层数、并行配置契约）不匹配应拒绝，否则 load 成功但语义错误。

#### 8. 易错点与反例

- 反例：只存模型不存 optimizer，恢复后 Adam 的 m/v 归零，loss 出现明显跳变。
- 反例：world size 改变后直接 load 旧布局，shard 形状/归属不匹配导致 load 失败或静默错位。
- 易错：不恢复数据 iterator 位置，恢复点重复消费刚训练过的样本。

#### 9. 实践与验证

`[待验证]` 在两次不同 world size 之间做 save/load，比较恢复后若干步的 loss、grad norm 与参数 diff；oracle 是“差异在容差内”。异步保存与 non-persistent checkpoint 的清理语义 M05 标注未实测，本节延续 `U / 待验证`。

#### 10. 评分标准

- 不合格：认为 checkpoint 就是模型权重。
- 合格：能列出模型+optimizer 至少两类状态并说明需 reshard。
- 良好：能补充 RNG、数据位置、LR 步数与 reshard 语义。
- 优秀/专家：能处理异步保存、CUDA Graph 资源与参数校验策略，设计中断—恢复对拍验证。

**证据与来源：** `megatron-lm/01-modules/M05-optimizer-checkpointing/README.md` 与 `implementation.md`（save/load 状态集合、`check_checkpoint_args`、dist_checkpointing，`C`）；`insights/05-megatron-architecture.md` 第 14.1/14.5 章（reshard、复现≠逐位一致，`C`/`I`）。恢复一致性实测 `U / 待验证`。

---

### T-A011. activation checkpoint 的边界与重计算代价：selective recompute 为什么能同时省显存又不怎么慢？

- 难度：高级
- 知识域：T11 显存、性能与吞吐
- 考察能力：定量判据、边界划分、与调度的交互
- 题型：定量分析 + 机制设计
- 面试频率：高频
- 前置知识：autograd 保存的激活、重计算（gradient checkpoint）、attention 的中间量
- 关联题目：T-A005（SP）、T-A012（OOM 归因）、T-A002（PP 在飞激活）
- 事实状态：selective recompute 判据与论文数据 `[已确认]`（引自论文）；本仓库未复现 `[待验证]`

#### 1. 背景知识

反向需要前向保存的激活。全量重计算（full recompute）省显存但重算范围大，开销可达 30–40% compute（`C`，论文）。selective activation recomputation 只对“激活体积大但计算密度低”的区域做 checkpoint 与重算，典型是 attention 的 QKᵀ、softmax、softmax dropout、V 乘（`C`）。

判据（`C`，论文）：若 `5·a·s/h > 34`（a=heads, s=seq, h=hidden），重算这部分更划算。论文示例：GPT-3（96 heads, s=2048, h=12288）`5as/h=80`，省约 70% 激活；MT-NLG（128 heads, s=2048, h=20480）`5as/h=64`，省约 65%；额外 FLOPs 仅 2.7% 与 1.6%（`C`）。

#### 2. 问题分析

边界划分是核心：

1. **哪些区域 checkpoint**：激活大、计算便宜的中间量（attention 的 softmax 相关），而非计算密集的 GEMM 输出。
2. **冗余算力代价**：选择性重算引入的额外 FLOPs 很小（论文 ~1.6–2.7%），因此比 full recompute 划算得多。
3. **与 PP 的交互**：PP 下在飞 microbatch 的激活是 OOM 主因，recompute 与 interleaved/更小 MB 可叠加；`deallocate_output_tensor` 又进一步压缩 stage 边界驻留。
4. **与 SP 的交互**：SP 把非 TP 区激活按序列切分，recompute 与 SP 都作用于激活，收益可能部分重叠，需要联合测量。

#### 3. 具体答案

面试中可以这样回答：

> 我先区分两种做法。full recompute 前向只存 layer 输入，反向重算整层，省显存但多花 30–40% 算力。selective recompute 只重算激活大、计算密度低的区域——attention 里的 QKᵀ、softmax、softmax dropout、V 乘这些，代价只有约 2% 的额外 FLOPs，却拿回大部分激活。
>
> 判断标准用论文的 `5·a·s/h > 34`：比值越大，attention 中间激活相对 hidden 越“胖”，重算越划算。GPT-3 是 80、MT-NLG 是 64，都远大于 34，所以论文报的 65–70% 激活节省是可解释的。
>
> 实际配置我会把它和 SP、interleaved、`deallocate_output_tensor` 一起看，因为它们都在动激活。先开 SP，再开 selective recompute，然后量峰值显存和 step time；如果 PP 的 stage 边界还是大头，用 interleaved 降在飞 microbatch。不要一次全开，否则无法归因（`U / 待验证`）。

#### 4. 技术洞察

```text
判据（C，论文）：5·a·s/h > 34 → 重算 attention 中间量更划算
  其中 a=heads, s=seq, h=hidden

论文示例（C）：
  GPT-3  : 5·96·2048/12288 = 80  → 省约 70% 激活，额外 FLOPs 2.7%
  MT-NLG : 5·128·2048/20480 = 64 → 省约 65% 激活，额外 FLOPs 1.6%
  full recompute：compute 开销 30–40%

重算对象（I）：QKᵀ、softmax、softmax dropout、attention@V
保留对象（I）：GEMM 输出等计算密集、激活相对小的量
```

#### 5. 图文说明

| 方案 | 省激活 | 额外算力 | 适用 |
|---|---|---|---|
| 不重算 | 无 | 0 | 显存充裕 |
| full recompute | 高 | 30–40% | 显存极紧、算力富余 |
| selective recompute | 高（65–70%） | ~2% | 常规首选 |

```text
attention 区域:  [QKᵀ] [softmax] [dropout] [·V]   ← checkpoint 这些
                     │
     反向：不保存，重新 forward 这段（算力便宜、激活胖）
GEMM 区域:       [QKV proj] [out proj]            ← 保留（算力贵、激活相对小）
```

#### 6. 拓展知识

- 与 T-A005 交叉：SP + selective recompute 组合可同时降激活驻留与重算开销（论文报告 SP+selective 可减少 5× 激活、重算时间开销降超 90%，`C`）。
- 与 T-A003 交叉：MoE 提供 `--moe-layer-recompute` 与细粒度 offload，激活管理粒度到 per layer per expert。
- 与 T-A002 交叉：interleaved 降低在飞 microbatch，recompute 降低单 microbatch 激活，两者叠加但都增加调度复杂度。

#### 7. 常见追问及回答要点

1. **为什么 selective 比 full 划算？** ——只重算激活占比大、计算便宜的 attention 中间量，额外 FLOPs 约 2%，远低于 full 的 30–40%。
2. **`5as/h > 34` 的直觉是什么？** ——它刻画 attention 中间激活相对 hidden 激活的“胖瘦”，比值越大，重算节省的显存越值那点额外计算。
3. **recompute 会不会影响数值？** ——重算在同样精度下是确定性的，但浮点累加顺序可能与不重算不同，逐位一致不保证。

#### 8. 易错点与反例

- 反例：上来就开 full recompute，算力开销 30–40%，在算力受限集群上得不偿失。
- 反例：把 recompute 和 SP 的激活收益简单相加，忽略两者作用于重叠的激活区域，收益被高估。
- 易错：忽略了 `deallocate_output_tensor` 与 PP 在飞微生物批对激活的影响，OOM 排查方向跑偏。

#### 9. 实践与验证

`[待验证]` 在固定模型与 batch 下对比不重算 / selective / full 的峰值显存、step time 与 FLOPs；oracle 是“峰值显存达标且 step time 增幅最小”。本仓库未复现论文数字，论文结论标 `C`，实测 `U / 待验证`。

#### 10. 评分标准

- 不合格：只知道“重计算省显存但变慢”。
- 合格：能区分 full 与 selective 并说出 attention 中间量是重算重点。
- 良好：能使用 `5as/h > 34` 判据并引用论文的量级。
- 优秀/专家：能协调 selective recompute、SP、interleaved 与 `deallocate_output_tensor`，并设计归因清晰的验证。

**证据与来源：** `insights/05-megatron-architecture.md` 第 7.4/12.3 章（selective recompute 判据、论文数据、SP 组合，均标注引自论文，`C`）。`megatron-lm/01-modules/M02-parallelism/implementation.md`（`deallocate_output_tensor`，`C`）。本仓库未复现，实测 `U / 待验证`。

---

### T-A012. 训练吞吐与显存瓶颈归因：怎么区分“激活爆了”和“权重/优化器爆了”？

- 难度：高级
- 知识域：T11 显存、性能与吞吐
- 考察能力：系统性诊断、指标解读、因果链拆解
- 题型：故障分析 + 定量分析
- 面试频率：高频
- 前置知识：step time 分解、MFU 定义、nsys/NCCL trace、激活 vs 状态显存
- 关联题目：T-A001（选型）、T-A007（状态显存）、T-A011（激活重计算）
- 事实状态：step time 分解与 MFU 定义 `[已确认]`（公式层）；具体归因数值 `[待验证]`

#### 1. 背景知识

step time 可近似分解为（`C`/`I`）：

```text
step_time ≈ compute_time + communication_time + idle_time
compute = 总 FLOPs / (GPU 数 × 单卡峰值 FLOPs × MFU)
comm    = Σ(通信体积 / 有效带宽) + 原语次数 × 延迟
idle    = PP bubble + 未重叠等待 + kernel 启动开销
```

MFU = 实际模型 FLOPs / (GPU 数 × 单卡峰值 FLOPs × 运行时长)（`C`）。OOM 分两类：激活驻留超预算（受 micro-batch、SP、recompute 影响）与权重/优化器状态超预算（受 TP、PP、distributed optimizer、FSDP 影响）。

#### 2. 问题分析

归因要有顺序，避免“看到慢就怪通信”：

1. **先看 MFU**：MFU 低说明在算的 FLOPs 少（bubble/小 GEMM/序列短）或时间被非计算占用。
2. **看 profile 的 time 分布**：nsys/NCCL trace 区分 compute、comm、idle；若 comm 与 compute 有明显空隙，是未重叠。
3. **OOM 归类**：激活随 micro-batch/序列增长而增长，且对 recompute/SP 敏感；权重/优化器状态与模型规模、TP/PP/DP 相关，与 micro-batch 基本无关。用“减小 micro-batch 是否缓解”作为判别实验。
4. **常见因果**：PP bubble 大（MB 太小）、通信未 overlap、GEMM 碎片化（TP 太大）、激活峰值（recompute 不足）、状态峰值（未开 dist optimizer）。

#### 3. 具体答案

面试中可以这样回答：

> 我按“先定位再优化”的顺序走。第一步算 MFU：如果 MFU 很低，说明要么在算的 FLOPs 少，要么时间花在非计算上。第二步用 nsys/NCCL trace 把时间拆成 compute、comm、idle 三块，看谁是大头。第三步对 OOM 做判别实验：把 micro-batch 减半，如果显存明显缓解，是激活爆了；如果几乎不动，是权重/优化器状态爆了。
>
> 激活爆的处置：开 SP、selective recompute、降在飞 microbatch（interleaved）。状态爆的处置：distributed optimizer 分片状态，或调 TP/PP，极端情况上 FSDP。comm 占主且有空隙：开 overlap，检查是不是“通信等计算”的串行依赖没解除。idle 占主：看 PP 气泡，MB 与 PP 的比值是否够大。
>
> 我会强调不要凭直觉优化：先用 trace 确认瓶颈类型，再动对应的旋钮，改一步测一步（`U / 待验证`）。

#### 4. 技术洞察

```text
判别实验（I）：
  减小 micro-batch → 显存显著下降  ⇒ 激活主导
  减小 micro-batch → 显存基本不变  ⇒ 权重/优化器状态主导

MFU = 模型 FLOPs / (GPU 数 × 峰值 FLOPs × 时间)
comm-bound 判据：trace 中 comm 段与 compute 段几无重叠（存在长空隙）
bubble 判据：1 - (PP-1)/(MB+PP-1) 明显小于 1
```

#### 5. 图文说明

| 症状 | 根因方向 | 处置 |
|---|---|---|
| MFU 低、idle 大 | PP bubble、MB 不足 | 增大 MB、interleaved |
| comm 与 compute 有长空隙 | 未 overlap | 开 TP/PP/EP overlap |
| 减小 MB 显存大降 | 激活超预算 | SP、selective recompute |
| 减小 MB 显存不变 | 状态超预算 | dist optimizer/FSDP、调 TP/PP |
| GEMM 变碎 | TP 过大 | 降 TP，同节点内重排 |

```text
测 step_time ──▶ 算 MFU ──▶ trace 拆 compute/comm/idle
                                     │
        comm 大头 ──▶ 查 overlap      idle 大头 ──▶ 查 bubble/MB
                                     │
                                两者都不大 ──▶ 查 OOM 类别（减 MB 判别）
```

#### 6. 拓展知识

- 与 T-A001 交叉：并行配置本身决定瓶颈类型，归因结论应反馈到选型。
- 与 T-A011 交叉：激活主导时的处置主要是 SP/recompute；状态主导时是分片。
- 扩展效率测量口径（`C`）：固定 global batch 口径（weak/strong scaling 要说清）、记录每 step 时间而非 samples/s、至少重复 3 次取中位数、用 profiler 区分 comm 与 idle。

#### 7. 常见追问及回答要点

1. **怎么快速判断是激活还是状态 OOM？** ——做减小 micro-batch 的判别实验：明显缓解是激活，基本不动是状态。
2. **MFU 低一定是通信问题吗？** ——不一定，可能是 bubble、GEMM 碎片化或 kernel 启动开销；要结合 trace 判断。
3. **为什么不能用 samples/s 比较扩展性？** ——它掩盖了 global batch 变化；固定口径下用每 step 时间更可比。

#### 8. 易错点与反例

- 反例：一看到慢就归因通信，实际是 PP bubble 或 GEMM 碎片化，优化方向完全错。
- 反例：把激活 OOM 当作状态 OOM，盲目上 FSDP，而真正需要的是 SP+recompute。
- 易错：不固定 global batch 与重复次数就宣称“扩展效率”，测量口径不可比。

#### 9. 实践与验证

`[待验证]` 在一组配置上依次做：MFU 计算、nsys/NCCL trace 拆解、micro-batch 判别实验；oracle 是“trace 显示的主瓶颈与处置后 step time 下降一致”。本仓库未运行 profiler 实验。

#### 10. 评分标准

- 不合格：只会说“训练慢/OOM”，没有分解思路。
- 合格：能写出 step time 三项分解与 MFU 定义。
- 良好：能给出 micro-batch 判别法与对应的 overlap/recompute/分片处置。
- 优秀/专家：能结合 trace、气泡公式与状态账，形成“定位—处置—验证”闭环，并指出测量口径陷阱。

**证据与来源：** `insights/05-megatron-architecture.md` 第 12B/15 章（step time 分解、MFU、失效清单、测量口径，`C`/`I`）；`megatron-lm/01-modules/M03-training-runtime/README.md`（timer/日志入口，`C`）。判别实验为工程推断（`I`），实测 `U / 待验证`。

---
### T-A013. 数据管道与计算重叠：怎么保证 GPU 不因等数据而空转？

- 难度：高级
- 知识域：T02 分布式初始化与通信组
- 考察能力：pipeline 分析、瓶颈定位、并行语义约束
- 题型：数据流 + 性能分析
- 面试频率：中高频
- 前置知识：DataLoader worker、pin memory、H2D copy、`get_batch` 的 TP broadcast
- 关联题目：T-A009（打包与 sampler）、T-A001（MB 与 DP）、T-A012（归因）
- 事实状态：`get_batch` 的广播/切片分支 `[已确认]`；数据栈实测 `[待验证]`

#### 1. 背景知识

M04 的边界是 `pretrain_gpt.get_batch`：从 iterator 取字典，按 TP source rank 做 CUDA 搬运与广播，再按 packed sequence、CP、MTP 分支整理为固定 `BATCH_KEYS`（`C`）。sampler 决定每个进程看到的 batch（`C`，M04 不变量：sampler 的 DP rank/size 决定每个进程看到的 batch）。

数据与计算重叠的手段：多 worker 预取、pin memory、异步 H2D、把 TP broadcast 与首层计算重叠、以及 schedule 内 microbatch 粒度的 data iterator（`I`）。

#### 2. 问题分析

数据瓶颈常被 GPU 掩盖，定位要：

1. **看 GPU 空转间隙**：trace 里 compute 之间若有规律空隙、且与 step 边界对齐，可能是等数据。
2. **检查 worker 与预取**：worker 不足、未 pin memory、同步 H2D 会让数据准备串行在前。
3. **TP 广播语义**：`get_batch` 从 TP source rank 广播，若该 rank 慢则整个 TP 组等它；packed/CP/MTP 分支会增加 get_batch 的工作量。
4. **并行语义约束**：sampler 必须保证每个 DP rank 的 micro-batch 相异，且 microbatch 数与 schedule 匹配（M02 不变量：model/data iterator 数量必须与调度类型匹配，`C`）。

#### 3. 具体答案

面试中可以这样回答：

> 我先在 trace 里找“规律性的计算间隙”。如果空隙跟每个 step 的起点对齐，很可能是数据没准备好。然后查三件事：worker 数是否够、是否 pin memory、H2D 是否异步。数据准备如果卡在同步 copy 上，GPU 前面就得等。
>
> 第二点容易被忽略：`get_batch` 是从 TP source rank 广播的，那个 rank 要做 tokenize/pack 的收尾和 CUDA 搬运，它一慢整组都等。所以我会让重活（构造、打包）尽量在 worker 里完成，主进程只做轻量的 pin 与异步搬运。
>
> 第三是并行语义：sampler 要保证每个 DP rank 拿到不同的 micro-batch，microbatch 数还要和 schedule 对得上，否则 schedule 会缺 iterator。最后用 prefetch 把第 N+1 步的数据准备与第 N 步计算重叠起来（`U / 待验证`）。

#### 4. 技术洞察

```text
重叠骨架（I，示意）：
  prefetch(N+1) 与 compute(N) 并发
  for step:
      batch = prefetch_queue.pop()          # 已在 worker 里完成构造/打包
      async_h2d(batch)                      # 非阻塞搬运
      schedule.forward_backward(compute)    # 与搬运/预取重叠
      prefetch_queue.push(load(step+1))

每个 DP rank 的 micro-batch 数 = global_batch / (DP · micro_batch_size_per_gpu)
约束：sampler 保证 DP 间样本相异；iterator 数与 schedule 类型匹配（C）
```

#### 5. 图文说明

| 环节 | 同步的代价 | 重叠手段 |
|---|---|---|
| 构造/打包 | 阻塞主进程 | 放到 DataLoader worker |
| CPU→pin | 每步拷贝开销 | pin memory + 复用缓冲 |
| H2D | 阻塞等待 | 异步 copy 与计算重叠 |
| TP 广播 | source rank 拖累全组 | 让 source rank 只做轻量收尾 |

```text
无重叠:  [准备数据][计算][准备数据][计算]   ← 数据段 GPU 空闲
有重叠:  [准备N+1   ]───────┐
         [  计算 N        ] │ prefetch 与计算并发
                    [异步 H2D N+1]
```

#### 6. 拓展知识

- 与 T-A009 交叉：打包粒度越粗，worker 内构造越重，越需要足够 worker 与预取。
- 与 T-A001 交叉：micro-batch 与 DP 共同决定每 rank 的 iterator 数量，选型时就应把数据吞吐纳入。
- 与 T-A012 交叉：trace 里的“计算间隙”要区分是数据等待还是 PP 气泡。

#### 7. 常见追问及回答要点

1. **怎么确认瓶颈在数据而非通信？** ——看间隙是否与 step 起点对齐、是否与数据准备时间相关；通信间隙通常与 collective 位置对齐。
2. **TP 广播为什么可能成为瓶颈？** ——它从单一 source rank 出发，该 rank 若承担过多 CPU 侧工作，整个 TP 组都要等它。
3. **worker 越多越好吗？** ——不是，worker 过多会争抢 CPU/内存带宽，反而拖慢；需要按数据构造成本调。

#### 8. 易错点与反例

- 反例：只加 worker 但不开 pin memory，H2D 仍同步阻塞，GPU 依旧空转。
- 反例：让 TP source rank 同时承担全部 tokenize/pack，成为全组木桶最短的板。
- 易错：忽略 sampler 与 microbatch 数量的 schedule 约束，出现 iterator 数量不匹配的运行错误。

#### 9. 实践与验证

`[待验证]` 对比单 worker/多 worker、同步/异步 H2D、有/无 pin memory 的 step time 与 GPU 空转比例；oracle 是“计算间隙消失、step time 下降”。本仓库未运行数据管线实验。

#### 10. 评分标准

- 不合格：认为数据管线不影响训练速度。
- 合格：知道用 worker 与预取做重叠。
- 良好：能指出 pin memory/异步 H2D 与 TP source rank 广播的瓶颈。
- 优秀/专家：能结合 sampler 语义、iterator 数量约束与 trace 定位数据侧空转，并给出验证方案。

**证据与来源：** `megatron-lm/01-modules/M04-data-pipeline/implementation.md`（`get_batch`、sampler，`C`）；`M02-parallelism/README.md`（iterator 数量不变量，`C`）。重叠骨架与瓶颈判据为推断（`I`），实测 `U / 待验证`。

---

### T-A014. 大 batch 训练稳定性：global batch 放大后怎么保证不炸、不慢收敛？

- 难度：高级
- 知识域：T08 优化器与梯度更新
- 考察能力：超参—batch 耦合、数值稳定性、诊断闭环
- 题型：定量分析 + 故障分析
- 面试频率：高频
- 前置知识：global batch 构成、LR warmup、gradient clipping、loss scaling、distributed optimizer grad norm
- 关联题目：T-A004（FP8 scale）、T-A003（aux loss）、T-A007（状态）
- 事实状态：LR 缩放与 warmup 为通用实践（`I`）；本仓库模块文档未覆盖训练动力学；实测 `[待验证]`

#### 1. 背景知识

global batch = `micro_batch_size × DP × gradient_accumulation_steps`（`I`）。放大 batch 会降低梯度噪声，若不相应调 LR，收敛变慢或停滞；常见做法是线性/平方根 LR 缩放、LR warmup、gradient clipping（`I`）。低精度训练还需 loss scaling（fp16）或 FP8 delayed scaling，梯度 inf/nan 时 scaler 会跳过更新。

Megatron 侧：`train_step` 只在 `optimizer.step()` 报告 update successful 时推进 scheduler，gradient norm 与 zero count 用于日志/诊断（`C`，M03/M05）。

#### 2. 问题分析

大 batch 稳定性是多个旋钮的耦合：

1. **LR 与 batch 的匹配**：batch 放大 k 倍，LR 通常按 sqrt(k) 或 k 缩放（配合 warmup），否则单位步长相对噪声过小/过大。
2. **warmup 必要性**：大 batch 早期梯度方向更“平均”，但仍需 warmup 避免初期大 LR 造成发散。
3. **梯度裁剪**：全局 grad norm 需跨 DP 聚合统计；distributed optimizer 下裁剪逻辑要与分片一致。
4. **精度交互**：FP8/FP16 的 scale 突变、MoE 的 aux loss 权重过大、以及 skip update 的计数，都会表现为“loss 抖动”。

#### 3. 具体答案

面试中可以这样回答：

> 放大 batch 我先同步调 LR：batch 扩大 k 倍，LR 用 sqrt(k) 打底、必要时线性缩放，并配套 warmup 让早期步长平滑。不做这一步，大 batch 会表现为“loss 下降变慢甚至停滞”。
>
> 第二是梯度稳定性：开 gradient clipping，注意全局 grad norm 要在 DP 域聚合，distributed optimizer 下的裁剪要与其分片逻辑一致。低精度训练还要盯 scaler：fp16 的 loss scaling 溢出会 skip update，FP8 的 delayed scaling 若 amax 统计异常会让数值跳，这些都要看 zero/skip 计数和 grad norm 曲线。
>
> 第三是 MoE 场景，aux loss 权重和 capacity 丢弃率会干扰主损失，调 routing 和调 LR 我会分两步做，避免归因混乱。
>
> 验收我盯三条曲线：loss、grad norm、skipped updates。三者平稳才算稳定，出现尖峰先查 precision scale 再查 LR（`U / 待验证`）。

#### 4. 技术洞察

```text
global_batch = micro_batch · DP · grad_accum                    # I
LR 缩放（常用，I）：
  线性：lr' = lr · k         （k = batch 放大倍数）
  平方根：lr' = lr · sqrt(k) （更保守，大 batch 常用）
warmup：前若干 step 从 0 线性升到目标 LR
clip：||g||_global = sqrt(Σ_shards ||g_shard||²) 后按阈值缩放   # 需跨 DP 聚合
skip：loss/grad 出现 inf/nan → scaler 跳过更新，不推进 scheduler（C 语义）
```

#### 5. 图文说明

| 现象 | 可能原因 | 排查顺序 |
|---|---|---|
| loss 下降变慢/停滞 | LR 未随 batch 放大 | 先查 LR 缩放 |
| loss 发散/尖峰 | 无 warmup、clip 缺失 | 加 warmup + clip |
| loss 抖动但无 inf | FP8 scale/aux loss | 查 amax 与 aux 权重 |
| 更新被频繁跳过 | loss scaling 溢出 | 查 skipped updates 计数 |

```text
batch ↑ ──▶ LR 缩放 + warmup + clip ──▶ 稳定训练
                  │
        监控 loss / grad_norm / skipped_updates
                  │
     尖峰? ──▶ 先查 precision scale ──▶ 再查 LR/数据
```

#### 6. 拓展知识

- 与 T-A004 交叉：FP8 delayed scaling 的 scale 突变是大 batch 抖动的一个隐蔽来源。
- 与 T-A003 交叉：MoE 的 aux loss 与 token dropping 会改变有效梯度，稳定性调优要分开做。
- 与 T-A007 交叉：distributed optimizer 下 grad norm 的聚合与裁剪必须与分片一致，否则裁剪阈值失真。

#### 7. 常见追问及回答要点

1. **大 batch 为什么常配 sqrt LR 而不是线性？** ——`I` 推断大 batch 梯度噪声低，线性放大容易过冲，sqrt 更稳；具体视模型与数据而定。
2. **warmup 解决什么问题？** ——早期参数远离极值，大 LR 易发散；warmup 让步长平滑升到目标值。
3. **grad clipping 在分布式下要注意什么？** ——全局范数需跨 DP/分片聚合，且与 distributed optimizer 的分片梯度一致，否则裁剪点错误。

#### 8. 易错点与反例

- 反例：batch 放大 8 倍但 LR 不动，loss 下降明显变慢，误判为“数据质量差”。
- 反例：开 FP8 但迟迟查不出抖动来源，实际是 amax 统计异常导致 scale 突变。
- 易错：把 MoE 的 aux loss 权重调大与 LR 调整混在一起做，归因混乱。

#### 9. 实践与验证

`[待验证]` 扫描 LR/batch 组合，记录 loss、grad norm、skipped updates 与最终 eval；oracle 是“目标 batch 下 loss 曲线平滑且到达同等或更好指标”。本节为通用训练实践，本仓库模块文档未覆盖动力学细节，实测标注 `U / 待验证`。

#### 10. 评分标准

- 不合格：认为“batch 越大越好”，不知 LR 需联动。
- 合格：能给出 global batch 构成与 LR 缩放/warmup 的基本做法。
- 良好：能联系 grad clipping 的分布式聚合与 precision scale。
- 优秀/专家：能分离精度、LR、MoE 路由三类抖动来源，给出可观测的验收曲线与排查顺序。

**证据与来源：** `megatron-lm/01-modules/M03-training-runtime/implementation.md`（`train_step` 的 update successful 语义，`C`）；`M05-optimizer-checkpointing/README.md`（grad norm/zero count 诊断，`C`）。LR 缩放、warmup、clip 为通用训练实践（`I`）；本仓库未覆盖训练动力学实验，实测 `U / 待验证`。

---

### T-A015. 训练到服务的权重交接：sharded checkpoint 怎么变成推理引擎能用的权重？

- 难度：高级
- 知识域：T11 显存、性能与吞吐
- 考察能力：布局转换契约、跨系统一致性、验证设计
- 题型：系统设计 + 跨模块集成
- 面试频率：中高频
- 前置知识：sharded state dict、Megatron-Bridge、推理侧 TP decode、GQA/MLA 的 KV head 分片
- 关联题目：T-A010（reshard）、T-A007（优化器状态）、T-A005（TP/CP 布局）
- 事实状态：dist checkpoint 与推理入口 `[已确认]`；转换正确性 `[待验证]`

#### 1. 背景知识

训练产物是 sharded state dict（每 rank 局部 shard + metadata），推理引擎（vLLM/SGLang 等）通常按 TP head 分片读取权重。二者布局不同，需要一个转换契约：

- Megatron-Core 提供 `save_distributed_checkpoint`/`load_distributed_checkpoint`，支持并行度变化时 reshard（`C`）。
- Megatron-Bridge 提供与 HuggingFace 的双向转换，可在模型并行感知下转换参数布局（`C`，insights 14.1）。
- 推理侧：QKV 列切后每 TP rank 持有部分 head 的 KV，KV cache 按 head 维分片（`C`，insights 14.2）。GQA 要求每 rank 持有整数个 KV head，否则需复制或 padding（`C`，insights 6.10）。

#### 2. 问题分析

交接的核心是“布局契约 + 数值验证”：

1. **布局映射**：训练 TP/PP/EP 布局 → 推理 TP 布局；PP 训练侧重，推理多按 TP decode，需要合并 stage 权重。
2. **GQA/MLA 约束**：KV head 数必须能被推理 TP 整除，否则要么复制 KV 投影、要么 padding head（与 T-A005/A008 交叉）。
3. **元数据完整性**：LayerNorm、bias、scale（FP8）等非权重张量也要转换，漏掉会静默偏差。
4. **验证**：转换后必须用固定输入对比训练侧与推理侧的 logits/生成结果。

#### 3. 具体答案

面试中可以这样回答：

> 我会先把训练的 sharded checkpoint 当成“带布局契约的状态”，而不是一个可以直接 load 的文件。第一步用官方转换路径（dist checkpoint / Megatron-Bridge）做布局重映射：把训练的 TP/PP/EP 布局转成推理引擎的 TP 布局。PP 训练的分 stage 权重在这里要合并。
>
> 第二步处理 head 约束。推理按 TP head 分片读权重，GQA 下 KV head 数必须能被推理 TP 整除；不整除时要么复制 KV 投影，要么 padding head——这直接决定推理 TP 能取多大，不能随便设。
>
> 第三步别漏非权重张量：LayerNorm、bias，以及 FP8 训练的 scale 元数据，漏一个都会让数值静默偏掉。
>
> 最后一定要验证：固定 prompt，对比训练侧前向与推理引擎的 logits 或生成结果，差异在容差内才算交接成功。我会把这一步做成门禁，而不是靠“能跑通”（`U / 待验证`）。

#### 4. 技术洞察

```text
转换契约（I）：
  train layout: (TP_t, PP_t, EP_t) sharded state dict + metadata
  infer layout: (TP_i) 推理引擎权重（KV 按 head 分片）
  映射：reshard → 合并 PP stage → 对齐 head 分片 → 补非权重张量

GQA 约束（C）：
  num_kv_heads % TP_i == 0  （否则复制/padding KV 投影）
MLA：latent 投影的解压 GEMM 也需按推理 TP 对齐
验证：logits(train) ≈ logits(infer)（固定输入，容差内）
```

#### 5. 图文说明

| 对象 | 训练布局 | 推理布局 | 处理 |
|---|---|---|---|
| 权重 | TP×PP×EP 分片 | TP 分片（按 head） | reshard + 合并 PP stage |
| KV 投影 | TP head 列切 | TP head 分片 | GQA 整除约束 |
| LayerNorm/bias | 随 stage | 完整 | 必转，勿漏 |
| FP8 scale | 训练 scale 元数据 | 推理量化参数 | 转换或重新校准 |

```text
train sharded ckpt ──▶ 布局重映射 ──▶ 合并 PP stage ──▶ 对齐 head ──▶ infer 权重
                                                                    │
                                              logits 对拍 ◀──────────┘（门禁）
```

#### 6. 拓展知识

- 与 T-A010 交叉：world size 变化时的 reshard 与训练→推理的布局重映射是同类问题。
- 与 T-A005 交叉：TP/CP 决定 KV 在物理层与上下文层的切分形状，推理 KV 分片必须与其一致。
- P/D 分离与 KV transfer 让训练侧 stage 激活传输与推理侧 KV 生命周期在机制上同源（`I`）。

#### 7. 常见追问及回答要点

1. **为什么不能直接 load 训练 checkpoint 到推理引擎？** ——布局不同：训练是 TP×PP×EP 分片，推理按 TP head 读；直接 load 会形状/语义双重不匹配。
2. **GQA 如何限制推理 TP 的选择？** ——KV head 数必须能被推理 TP 整除，否则需复制或 padding KV 投影，直接约束推理并行度。
3. **怎么保证交接数值正确？** ——固定输入做 logits/生成结果对拍，差异在容差内才放行，并检查非权重张量与 scale 元数据。

#### 8. 易错点与反例

- 反例：假设训练 checkpoint 可直接被推理引擎加载，忽略 TP/PP 布局差异，load 失败或权重错位。
- 反例：KV head 数不能被推理 TP 整除却强行设置，导致 KV 分片错误或复制计算未处理。
- 易错：只转权重、漏掉 LayerNorm/bias 与 FP8 scale 元数据，推理结果静默偏差。

#### 9. 实践与验证

`[待验证]` 用固定 prompt 对比训练侧前向与推理引擎的 logits/生成结果，断言差异在容差内；并核验非权重张量与量化 scale 是否一致。需推理引擎与权重转换工具，本仓库未执行。

#### 10. 评分标准

- 不合格：认为“存下来就能推理”。
- 合格：知道需要布局转换并了解 sharded checkpoint。
- 良好：能说明 PP 合并、TP head 对齐与 GQA 整除约束。
- 优秀/专家：能覆盖非权重张量、量化 scale、KV 布局与 P/D 分离，设计 logits 对拍门禁验证。

**证据与来源：** `megatron-lm/01-modules/M06-inference-and-tools/README.md` 与 `implementation.md`（推理引擎、服务入口、KV handoff 边界，`C`）；`M05-optimizer-checkpointing/README.md`（dist_checkpointing，`C`）；`insights/05-megatron-architecture.md` 第 6.10/14 章（GQA 整除、TP 下 KV 分片、Bridge 转换，`C`/`I`）。转换正确性实测 `U / 待验证`。
