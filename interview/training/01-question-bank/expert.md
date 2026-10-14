# 训练方向专家级面试题（T-E001–T-E015）

> 每题按 12 段结构给出：元数据、背景知识、问题分析、具体答案、技术洞察、图文说明、拓展知识、常见追问、易错点与反例、实践与验证、分层评分、证据与来源。
> 证据规则：源码/文档结论区分 `C`（源码或官方文档明确陈述）/`I`（跨文档或基于证据的工程推断）；GPU、NCCL、多机、多进程与性能数字未经目标环境运行时标 `U / 待验证`；本题库不写入未声明的实测统计。

---

### T-E001. 如何为大规模训练定义一组可检查的形式化不变量，并在训练循环里持续验证？

- 难度：专家级
- 知识域：T01 训练入口与生命周期
- 考察能力：把"训练正确"从模糊目标转化为可判定谓词，并落到运行时断言
- 题型：开放设计 + 形式化
- 面试频率：中频（系统/训练架构岗）
- 前置知识：Hoare 前后置条件、`torch.distributed`、梯度累积语义、分布式 checkpoint
- 关联题目：T-E006（checkpoint 一致）、T-E014（新并行风险评审）、入门 T-B001
- 事实状态：拓扑与更新类不变量有源码/文档依据（`C`）；跨 rank 数值一致性、运行期断言开销需实测（`U / 待验证`）

#### 1. 背景知识

"训练没报错"和"训练正确"是两件事。源码级事实（`C`）：`initialize_model_parallel` 要求 `torch.distributed` 先初始化且 world size 可被模型并行规模整除；VP 只在 PP>1 时合法；GTP rematerialization 与多个 distributed-optimizer instance 由初始化逻辑禁止组合（M02 `README.md` 关键不变量）。更新侧的约束（`C`）：`train_step` 只有在 `optimizer.step()` 报告 update successful 时才推进 param scheduler 与迭代计数（M03 `implementation.md`）。这些分散事实可以收敛成一组不变量。

不变量要能被机器检查，而不是停留在口头。可检查意味着：有明确触发点、有观测变量、有通过/失败判定、失败可定位到轴。

#### 2. 问题分析

1. **分层归类**：把不变量分成四类——拓扑类（init 前可判定）、调度类（每个 global step 判定）、数值类（每个 step 或周期判定）、状态类（checkpoint 边界判定）。混合在一处会导致触发时机错乱。
2. **判定谓词的观测量必须存在且廉价**：例如"梯度完整"不能只看有没有调 `finalize_model_grads`，而要看反向完成后 DP/CP 域上的梯度是否已归约；这需要 accessor 或统计量，不能靠日志猜。
3. **失败可归因**：同一条不变量失败要能区分是配置错、数值错还是拓扑错，否则断言只增加噪音。设计上应让每条断言携带 `(axis, stage, step, rank)` 上下文。

#### 3. 具体答案

面试中可以这样回答：

> 我会先把"正确"写成前后置条件，再把它们挂到四个检查点。拓扑类在初始化和模型构造之间判定：`pg_initialized ≺ model_built`，以及 `world_size % (tp*pp*cp) == 0`，VP 必须满足 `pp>1`，GTP 与多 optimizer instance 互斥——这几条源码里已经有显式校验，我只需把它们提升成可返回诊断的断言。
>
> 调度类挂在 `train_step` 的出口：本步所有 microbatch 的反向都完成后、`optimizer.step()` 之前必须恰好进行一次梯度 finalize；`optimizer.step` 返回 update successful 才允许 `scheduler.step()` 和 `iteration += 1`，否则本步视为未推进。
>
> 数值类是概率性的：周期性地比较各 rank 的 loss 与 grad norm，若同一 global step 上 loss 跨 DP rank 偏差超过容差，直接标记该步为 suspect；TP=1 基线与 TP=N 的 loss 曲线 diff 是更严格的判据。
>
> 状态类只在 checkpoint 边界判定：保存点必须落在一次成功 step 之后，且被切取的 model/optimizer/scheduler/RNG/data-iterator 属于同一个 global step，不允许把上一步的优化器状态和本步的迭代计数拼在一起。

#### 4. 技术洞察

用状态机描述最清楚：把训练看作带状态 `S = (params, opt_state, sched, rng, data_pos, step)` 的迁移系统，每个 global step 是一个迁移 `S_k → S_{k+1}`。正确性 = 每个迁移满足前置条件，只在 update successful 时提交，且 checkpoint 是迁移系统上的一个切面。

```text
Invariants(I, stage):
  I1 (topology):  before model_build:
      pg_initialized(S) == true
      world_size % (tp*pp*cp) == 0
      (vp>1)  ==> (pp>1)
      not (gtp_remat and multi_distopt)
  I2 (schedule):  at train_step.exit, before opt.step:
      all_microbatches_backwarded(S) == true
      finalize_model_grads_called_once(S) == true
  I3 (commit):    if opt.step.success:
      sched.step(); step += 1
    else:
      S := S_k            # 本步不提交, lr/iteration 不回滚也不前进
  I4 (numeric):   every K steps:
      max_rank_spread(loss(step)) <= eps_loss
  I5 (state cut): at checkpoint:
      forall rank r: step_r == step_0        # 全局同一步
      saved_set == {model, opt, sched, rng, data_pos} at same step
```

这段伪代码的要点不是照搬实现，而是说明"不变量 = 谓词 + 触发点 + 提交语义"。

#### 5. 图文说明

| 不变量 | 触发点 | 观测量 | 失败归因 | 证据 |
|---|---|---|---|---|
| topology | init 与 build 之间 | world_size、PG 集合 | 配置/整除 | `C` M02 |
| schedule | step 出口 | mb 计数、finalize 调用 | 调度/回调 | `C` M02/M03 |
| commit | opt.step 返回 | update_successful | 数值(溢出) | `C` M05 |
| numeric | 每 K 步 | 跨 rank loss/grad norm 散布 | 数值/并行 | `I` |
| state cut | checkpoint | 各 rank step 一致 | 状态/异步 | `I` |

```text
init ──[I1]──> build ──> step ──[I2]──> opt.step ──[I3]──> commit
                                    K 步 ──[I4]──> 数值检查
                              checkpoint ──[I5]──> 持久化切面
```

#### 6. 拓展知识

- 与 T-E006 交叉：I5 是 checkpoint 一致性的形式化版本，异步保存会挑战"切面"语义。
- 与 T-E014 交叉：引入 CP/EP/GTP 时，I1 的合法组合集合被改写，评审必须重算谓词。
- 与可观测性交叉：I4 的观测需要 timer 与统计量暴露，见 T-E012。

#### 7. 常见追问及回答要点

1. **不变量太多会不会拖慢训练？** ——拓扑与状态类只在边界判定，几乎零开销；数值类做成周期采样，用统计而非逐元素比对。
2. **TP=1 基线 vs TP=N 的 loss diff 算哪条不变量？** ——属于 I4 的加强版，判据是曲线形状而非逐位相等；并行会改变归约顺序，逐位一致本就不保证（`I`）。
3. **如果 I3 不满足会怎样？** ——溢出或异常时若仍推进 lr 与 iteration，恢复点与状态错位，表现为"重启后 loss 跳变"。

#### 8. 易错点与反例

- 反例：把不变量写成日志字符串，没有机器判定，训练异常时无人可比对。
- 反例：只在 rank0 校验 loss 一致性，掩盖了少数 rank 上的 NaN 或分片错位。
- 易错：认为"梯度已 finalize"等于"梯度完整"——若 CP 组权重复制份未参与归约，finalize 也会通过，但梯度少了 CP 域的贡献（`I`）。

#### 9. 实践与验证

`[待验证]` 在 D01 最小训练入口上植入 I1–I3 的断言，跑 TP=1 与 TP=2 各若干步；oracle：I1/I2 在所有步通过，TP=2 相对 TP=1 的 loss 曲线在容差内（曲线差而非逐位差），且人为破坏一处（例如把 finalize 去掉一次）能让 I2 立即报错并给出 axis/stage/step。跨 rank 数值散布与断言开销需在多卡与 NCCL 环境实测，本仓库未执行。

#### 10. 评分标准

- 不合格：只泛谈"要保证正确性"，给不出可判定谓词。
- 合格：能列出拓扑、调度、状态三类不变量及各自触发点。
- 良好：能区分"调用发生过"与"语义成立"，并把提交语义（update successful）纳入。
- 优秀/专家：能给出 Hoare 式前后置条件、checkpoint 切面语义，并说明容差判据为何不能逐位比较。

**证据与来源：** `megatron-lm/01-modules/M02-parallelism/README.md`（关键不变量，`C`）；`M03-training-runtime/implementation.md`（`train_step` 更新语义，`C`）；`M05-optimizer-checkpointing/README.md`（更新不变量与 checkpoint 状态集合，`C`）；`insights/05-megatron-architecture.md` 4.3、12.5、14.5（跨 rank 数值与"复现≠逐位"讨论，`I`）。版本锚点 Megatron-LM main @ `3703d4e33a3a2b2d11ebcc8e41f45af7ce7d1eda`；运行结论标 `U / 待验证`。

---

### T-E002. 给定集群规模与显存预算，如何为一次新训练选择并行度，使通信-计算权衡最优？

- 难度：专家级
- 知识域：T05 流水线并行与调度
- 考察能力：把并行配置当成带约束的优化问题，而非经验表
- 题型：开放设计 + 定量推导
- 面试频率：高频（大规模训练必问）
- 前置知识：TP/PP/DP/CP 通信原语、气泡公式、MFU、strong/weak scaling
- 关联题目：T-E009（吞吐/成本度量）、T-E010（异构并行）、T-E005（长上下文）
- 事实状态：气泡公式、TP 每层 4 原语为文档/论文事实（`C`）；具体通信带宽与 overlap 收益为 `U / 待验证`

#### 1. 背景知识

一个 Transformer 层在 TP 下 forward/backward 各 2 次 allreduce，合计每层 4 次、且次数与 TP 规模无关，通信体积才随 t 变化（`C`，insights 6.5/6.6）。PP 的气泡效率有闭式近似 `1 - (PP-1)/(MB+PP-1)`（`C`，insights 8.4）。DP 的梯度通信体积约为 `2·(N-1)/N·params`（`I`）。CP 不切权重，backward 需在 CP∪DP 域额外归约复制权重的梯度（`C`，M02 design）。这些是"给定配置估计 step 时间"的构件。

#### 2. 问题分析

1. **目标函数要写出来**：`step_time ≈ compute + comm + idle`。compute 由模型 FLOPs 与单卡有效算力决定；comm 是各轴原语体积除以有效带宽加次数乘延迟；idle 主要是 PP 气泡与未重叠等待。不写出来就只能凭感觉。
2. **约束优先于目标**：显存是硬约束（单卡能否装下权重+优化器分片+在飞激活），整除是合法性约束（`world_size % (tp*pp*cp)`）。最优解必须在可行域内。
3. **轴间非独立**：TP 变大使单 GEMM 变小、MFU 下降；PP 变大需要更大 MB 才不亏气泡，而 MB 受 global batch 限制；CP 变大降低激活但增加序列通信。因此是联合搜索，不能逐轴贪婪。

#### 3. 具体答案

面试中可以这样回答：

> 我会先算可行域再谈最优。硬约束两条：单卡显存要装下 `权重/(tp*pp) + 优化器分片/(tp*pp*dp) + 在飞激活`，以及 `world_size` 被 `tp*pp*cp` 整除。在可行域里，我按 step 时间拆项：compute 是 `FLOPs/(GPU数·单卡峰值·MFU)`；comm 里 TP 是每层常数个原语、体积随 t 变化，PP 是少而小的 P2P，DP 是梯度 reduce-scatter，CP 与 EP 是序列/专家通信。idle 主要来自 PP 气泡与未重叠的 allreduce。
>
> 起点通常按拓扑约束选：TP 放同节点（NVLink 域），PP 跨节点，DP 顶横向，长序列加 CP。然后用气泡公式反推 MB：`MB = 4·PP` 时效率约 81%、`MB = 8·PP` 约 90%，而 MB 受 global batch 上限约束，所以 PP 不能随意加大。
>
> 我会把候选配置列成小表（如 TP 取 4/8、PP 取 4/8、CP 取 1/2），对每个算一遍显存账与 step 时间估计，去掉违反显存的，再比较通信占比。最后一步才是实机验证：先开 overlap，再看 nsys 里 comm 与 idle 的实际占比。

#### 4. 技术洞察

把配置选择写成约束优化：

```text
minimize  T(cfg) = T_compute + T_comm + T_idle
cfg = (tp, pp, cp, dp, mb)

s.t.  world_size == tp*pp*cp*dp
      mem_weight + mem_opt + mem_act(cp, mb, recompute) <= mem_gpu
      global_batch == dp * mb * micro_bs
      pp > 1  if vp > 1

T_compute = FLOPs_step / (world_size * P_peak * MFU(cfg))
T_comm    = (L/tp 原语的体积)/BW_tp + (PP 边界 P2P)/BW_pp + (grad)/BW_dp
T_idle    ~ T_compute * (PP-1)/(MB+PP-1)          # 气泡主导
```

关键洞察：`T_idle` 随 PP 升而升、随 MB 升而降；`T_compute` 随 tp 升（GEMM 碎片化）而效率降。二者对 PP、TP 的单调性相反，所以存在内点最优。

#### 5. 图文说明

| 轴 | step 时间中的项 | 对配置的单调性 | 主要约束 |
|---|---|---|---|
| TP | 每层常数原语 × 体积 | t↑ → MFU↓、comm 占比↑ | 需同 NVLink 域 |
| PP | 气泡 ∝ (PP-1)/(MB+PP-1) | P↑ → 气泡↑，除非 MB↑ | 层数、MB≤global_batch/DP |
| DP | 梯度 reduce-scatter | N↑ → 体积比↑ | 全局 batch 要够大 |
| CP | 序列通信 + 复制权重梯度归约 | C↑ → 激活↓、comm↑ | 序列长度、需 overlap |

```text
显存/整除可行域 ──> 列候选 ──> 每候选算 T(cfg) ──> 去违规 ──> 比较 ──> 实机
                                  ^                                    |
                                  └──────── 反馈：实测 comm/idle ───────┘
```

#### 6. 拓展知识

- 与 T-E009 交叉：`T(cfg)` 的分母口径（goodput 还是 raw step time）会改变结论，度量不统一就没法比较配置。
- 与 T-E010 交叉：异构集群里同一 cfg 的各轴物理带宽不同，`BW_tp` 等参数要按实际排布取。
- 与 T-E005 交叉：长序列下 attention 的 s² 项使 CP 的边际收益与序列长度强相关。

#### 7. 常见追问及回答要点

1. **为什么不是"每个轴都拉满"？** ——并行度越大越好的直觉在 PP=5 气泡比例升高、EP 过大把 A2A 推出 NVLink 域、CP 未重叠时通信占主等场景下失效，最优是 P&L 问题（`I`，insights 15.3）。
2. **TP 越大通信次数会变多吗？** ——不会，TP 每层 allreduce 次数是常数 4，变的是体积与 GEMM 碎片化带来的 MFU 下降（`C`）。
3. **strong scaling 还是 weak scaling？** ——必须先说清：固定 global batch 是 strong，随 GPU 数放大 batch 是 weak；两者 MFU/扩展效率口径完全不同。

#### 8. 易错点与反例

- 反例：先定 TP=8 再挑 PP，忽略 TP 放大后单 GEMM 变小、MFU 掉落，实测吞吐不升反降。
- 反例：把 MB 设成远小于 PP，气泡按公式吃满，却归因于"通信慢"。
- 易错：用 raw step time 比较不同配置，而其中一个配置重启更频繁，goodput 才是真实成本（见 T-E009）。

#### 9. 实践与验证

`[待验证]` 固定模型规模与 global batch，枚举 TP×PP×CP 的一组候选，在目标集群跑固定步数；oracle：每个候选的显存不 OOM、step 时间稳定（≥3 次取中位数），并且 `1-(PP-1)/(MB+PP-1)` 估计的气泡与实际 idle 占比（nsys 拆 comm/idle）趋势一致。所有带宽、重叠收益与实测 step 时间需目标硬件，本仓库未执行。

#### 10. 评分标准

- 不合格：只会背"先 DP 再 TP 再 PP"，给不出目标函数。
- 合格：能写出 compute/comm/idle 拆项与整除、显存约束。
- 良好：能用气泡公式反推 MB、解释各轴对 step 时间的相反单调性。
- 优秀/专家：能构造小规模候选搜索、说明 strong/weak scaling 口径差异、并给出实机验证的反馈回路。

**证据与来源：** `insights/05-megatron-architecture.md` 6.5/6.6（TP 每层 4 原语，`C`）、8.4（气泡公式，`C`）、15.3/15.5（反直觉与决策流程，`I`）；`megatron-lm/01-modules/M02-parallelism/design.md`（CP 梯度归约复制份，`C`）。带宽与重叠收益标 `U / 待验证`。

---

### T-E003. MoE 训练的系统性设计决策：从 dispatcher、容量到并行折叠，你如何逐项取舍？

- 难度：专家级
- 知识域：T07 MoE 与专家并行
- 考察能力：多目标设计决策（吞吐、质量、显存、可捕获性）与耦合识别
- 题型：开放设计
- 面试频率：中高频（MoE 岗位）
- 前置知识：top-K 路由、all-to-all、grouped GEMM、CUDA Graph 静态 shape
- 关联题目：T-E010（Parallel Folding）、T-E014（新并行风险）、T-E002（并行权衡）
- 事实状态：dispatcher 种类、EP+TP 需 SP、Parallel Folding 有官方/文档依据（`C`）；A2A 收益与路由热点为 `U / 待验证`

#### 1. 背景知识

MoE 把参数与计算解耦：每 token 只路由到 top-K experts，参数量大但 FLOPs/token 不线性增长（`C`）。官方给的可选 token dispatcher 有 allgather / alltoall / flex（DeepEP、HybridEP 后端）；EP 与 TP 组合时必须开 sequence parallel；性能优化含 grouped GEMM、permute 融合、A2A overlap、shared expert overlap、token dropping 与 capacity factor（`C`，insights 11.2）。MoE Parallel Folding 把 attention 层（TP×CP×DP×PP）与 MoE 层（ETP×EP×EDP×PP）的并行拓扑解耦（`C`，insights 11.3）。

#### 2. 问题分析

1. **决策耦合**：dispatcher 选择决定 A2A 体积与是否跨节点；capacity factor 决定 shape 是否静态、能否被 CUDA Graph 捕获；路由均衡决定是否有热点 expert 拖慢全组。三者不能分开拍板。
2. **目标冲突**：dropless 保质量但 shape 动态、难以 graph capture；dropping 换静态 shape 与稳定吞吐，但引入信息损失（`I`）。
3. **拓扑与算法绑定**：attention 喜欢大 TP/CP，MoE 喜欢高 EP、ETP=1；强制共享单一并行配置会让两类层互相将就，这正是 Folding 的动机。

#### 3. 具体答案

面试中可以这样回答：

> 我按四个相互耦合的决策来走。第一是并行拓扑：attention 与大 GEMM 用 TP，MoE 用 EP，EP 与 TP 并存时必须开 SP。若两类层形状差异大，我会考虑 Parallel Folding 让 attention 用 TP×CP、MoE 用 EP×ETP，而不是强行统一。
>
> 第二是 dispatcher：专家数与 batch 小时，allgather 简单但通信量大；专家多、跨节点时用 alltoall 甚至 flex/DeepEP，把 A2A 压到 NVLink 域内，并用 overlap 把 dispatch 藏在 FFN 计算后。
>
> 第三是容量与丢弃：预算允许就 dropless 保质量；需要对吞吐稳定或要 CUDA Graph 捕获就设 capacity factor 丢溢出 token，让 shape 静态。
>
> 第四是均衡：用 aux loss 或 sinkhorn 把路由摊平，避免单个 expert 长时间排队，因为一个热点 expert 会拖住整条 A2A 与它所在 rank 的 FFN。最后我会把这几项写成一张决策表，逐项标注它牺牲了吞吐、质量还是可捕获性。

#### 4. 技术洞察

容量与丢弃的可判定定义：

```text
per_expert_capacity = ceil( (tokens_per_batch * top_k / num_experts) * capacity_factor )
dropped = tokens_routed_to_expert - min(tokens_routed_to_expert, capacity)
dropless  ==> capacity = max_routed               # shape 依赖数据, 动态
dropping  ==> capacity 固定                        # shape 静态, 可 graph capture
```

负载均衡的代价函数（标准形式）：`L_aux = α · E · Σ_e f_e · P_e`，其中 `f_e` 是路由到 expert e 的 token 比例、`P_e` 是门控概率均值；`α` 越大路由越均匀但可能牺牲"该专家更合适"的偏好。

#### 5. 图文说明

| 决策 | 选项 | 换取的 | 付出的 | 证据 |
|---|---|---|---|---|
| dispatcher | allgather / alltoall / flex | 通信量 | 复杂度、后端依赖 | `C` |
| 容量 | dropless / capacity-factor | 质量 / 静态 shape | 动态 shape / 信息损失 | `C`+`I` |
| 均衡 | aux loss / sinkhorn | 避免热点 | 可能偏离最优路由 | `I` |
| 拓扑 | 统一 vs Folding | 各层最优 / 简单 | 层间 layout 转换 | `C` |

```text
tokens ──> router(top-K) ──> dispatch(A2A) ──> grouped GEMM ──> combine ──> out
              | 均衡(aux/sinkhorn)       | capacity 控制 shape
              └──── overlap: A2A 藏进 FFN 计算 ────┘
```

#### 6. 拓展知识

- 与 T-E010 交叉：Folding 与 FSDP 使专家参数只在 EDP 组内 shard，减轻全 EP 组的 all-gather/reduce-scatter 带宽（`C`）。
- 与 T-E014 交叉：动态 capacity 会让 CUDA Graph 捕获失效，官方建议只捕获 attention 部分（`C`）。
- 与 T-E012 交叉：路由分布与丢弃率是必须上报的可观测指标，否则热点不可见。

#### 7. 常见追问及回答要点

1. **Dropless 一定更好吗？** ——质量上通常更好，但 shape 动态、难 graph capture；是否值得取决于吞吐与质量的联合目标（`I`）。
2. **为什么 EP 与 TP 组合必须开 SP？** ——官方约束；token dispatch 需要一致的 layout，SP 保证激活切分一致（`C`）。
3. **专家很少时还该用 EP 吗？** ——专家少且 batch 小时 A2A 开销占比高，可能不如每 rank 复制全部 experts 的 DP 式（`I`）。

#### 8. 易错点与反例

- 反例：为追求 dropless 关掉 capacity，结果动态 shape 让 CUDA Graph 捕获失败，吞吐抖动。
- 反例：只调大 aux loss 强行均衡，掩盖了专家容量配置本身不合理，收敛变差还以为是数据问题。
- 易错：把 expert 数除以 world_size 当作每 rank 专家数，忽略 ETP/EP 的乘积与 EDP 的存在。

#### 9. 实践与验证

`[待验证]` 在具备 NVLink 的多卡环境，固定模型与 batch，跑 alltoall 与 allgather 两种 dispatcher，并分别开/关 A2A overlap；oracle：overlap 开启后 step 时间下降、nsys 中 A2A 与 FFN 计算出现时间重叠；容量实验中累计丢弃率可被计数上报，且与 loss 曲线的抖动相关。跨节点 A2A 的延迟与 flex 后端行为需目标硬件与依赖，本仓库未执行。

#### 10. 评分标准

- 不合格：只知道"MoE 要 EP、要 all-to-all"。
- 合格：能列出 dispatcher、容量、均衡、拓扑四类决策。
- 良好：能说明 dropless/dropping 与 CUDA Graph 的耦合、EP+TP 需 SP 的约束。
- 优秀/专家：能用容量/均衡公式、Folding 的动机与层间 layout 转换代价，给出互相耦合的取舍。

**证据与来源：** `insights/05-megatron-architecture.md` 11.1–11.7（dispatcher/容量/均衡/Folding，`C`+`I`）、7.6（EP+TP 需 SP，`C`）、13.3（CUDA Graph 与动态 shape，`C`）。A2A 实测收益标 `U / 待验证`。

---

### T-E004. 混合精度训练的数值稳定性如何治理？FP16/BF16/FP8 各自的风险面与监控手段是什么？

- 难度：专家级
- 知识域：T09 混合精度与数值稳定
- 考察能力：精度格式的第一性差异分析 + 可观测治理
- 题型：开放设计 + 定量
- 面试频率：中频
- 前置知识：浮点表示（exponent/mantissa）、loss scaling、FP8 E4M3/E5M2、delayed scaling
- 关联题目：T-E001（数值不变量）、T-E006（溢出与恢复）、T-E012（可观测）
- 事实状态：Megatron 传统方案与 FP8 架构要点有文档依据（`C`+`I`）；FP8 与硬件收益为 `U / 待验证`

#### 1. 背景知识

FP16 exponent 只有 5 位，动态范围不足，需要动态 loss scaling：master 参数保持 fp32，forward/backward 用 fp16，loss 乘 scale 防梯度下溢，scaler 监测 inf/nan（`C`，insights 12A.2）。BF16 动态范围与 fp32 相近、无需 loss scaling（`C`）。FP8 分 E4M3（前向权重/激活）与 E5M2（梯度），需延迟缩放（delayed scaling）依据前一迭代统计设定 per-tensor scale（`I`，insights 12A.3）。M05 的 `MixedPrecisionOptimizer` 维护低精度/主参数、loss scaling 与溢出处理（`C`）。

#### 2. 问题分析

1. **三种格式的失效模式不同**：FP16 主要怕下溢（梯度趋零）与上溢；BF16 不怕范围但精度低，误差在长链累加中累积；FP8 范围与精度都紧，靠 per-tensor scale 把动态范围搬回来，scale 失配会整块精度崩塌。
2. **并行放大问题**：TP 下若各 rank 独立统计激活幅值来定 FP8 scale，可能产生跨 rank 不一致（`I`），使 reduce 结果与单卡基线偏离。
3. **治理需要闭环**：仅有 loss scaling 不够，还要有 inf/nan 计数、跳过步计数、scale 轨迹、每层 FP8 amax 直方图，否则事故无法复盘。

#### 3. 具体答案

面试中可以这样回答：

> 我把混合精度治理分成"格式风险"和"闭环监控"两层。格式层：FP16 用 fp32 master 加动态 loss scaling，scaler 发现 inf/nan 就跳过该步更新并回退 scale；BF16 去掉 scaling，但要担心低尾数在深层累加里的误差；FP8 用 E4M3 走前向、E5M2 走梯度，scale 按前一迭代统计延迟设定，本质是用 scale 把 tensor 搬进 FP8 的可表示带。
>
> 闭环层：我会持续上报溢出步计数、被跳过的步数、loss scale 的历史轨迹、以及 FP8 每张量的 amax 分布。治理规则是：连续溢出超过阈值就告警并回退精度档；scale 长时间贴边就说明该张量不适合 FP8。
>
> 并行一致性是常被忽视的一点：TP/CP 组内统计 scale 必须同源或做归约，否则同一逻辑张量在不同 rank 用了不同 scale，归约后数值与单卡基线偏移。做 FP8 灰度时我会先固定 seed，单卡基线逐层 diff，再逐层开 FP8，而不是一次全开。

#### 4. 技术洞察

动态 loss scaling 的更新规则（标准形式）：

```text
每步:
  if any(isinf(grad)) or any(isnan(grad)):
      skip optimizer.step
      scale /= 2                     # 回退
      overflow_counter += 1
  else:
      if overflow_counter == 0: scale *= 2
      scale = min(scale, scale_max)
      apply(grad / scale)            # 更新前还原真实尺度
```

FP8 延迟缩放（per tensor）：

```text
amax_t   = max(|x|)                            # 本迭代统计
scale_t  = amax_history_window_max / FP8_MAX   # 用近窗历史, 而非当前瞬时
x_fp8    = quantize(x * scale_t)
```

要点：用历史窗口而非瞬时 amax，避免单次幅值突变把 scale 拉坏。

#### 5. 图文说明

| 格式 | 范围风险 | 精度风险 | 需要的机制 | 主要监控 |
|---|---|---|---|---|
| FP16 | 下溢/上溢 | 中 | fp32 master、动态 scaling | 溢出计数、scale 轨迹 |
| BF16 | 低 | 累加误差 | 无需 scaling | 长链误差、loss 曲线 |
| FP8(E4M3/E5M2) | 高（靠 scale） | 高 | delayed scaling、分级格式 | per-tensor amax、贴边率 |

```text
forward ── fp16/bf16/fp8 ──> loss ──> x scale(仅 fp16) ──> backward
                                                   |
                                          inf/nan? ──> skip + scale/2
```

#### 6. 拓展知识

- 与 T-E001 交叉：溢出导致 skip 时若不遵守"update successful 才推进"的提交语义，恢复点会错位。
- 与 T-E006 交叉：scale 本身是需要随 checkpoint 保存的状态，否则恢复后数值行为不同。
- 与 T-E012 交叉：amax 直方图与溢出计数是可观测性设计里的高价值指标。

#### 7. 常见追问及回答要点

1. **BF16 为什么不用 loss scaling？** ——BF16 exponent 位宽与 fp32 相当，动态范围足够，下溢风险低（`C`）。
2. **FP8 的 scale 为什么是 per-tensor？** ——FP8 可表示带宽窄，需要按张量幅值缩放；代价是 scale 元数据要传播，跨 rank 需一致（`I`）。
3. **连续溢出该怎么处理？** ——跳过当步并回退 scale；若持续，说明格式或初始化不匹配，应降精度档或检查数据。

#### 8. 易错点与反例

- 反例：各 TP rank 独立算 FP8 scale，reduce 后与单卡基线偏移，被误判为"学习率问题"。
- 反例：overflow 后仍然推进了 lr 与 iteration 计数，重启时状态错位。
- 易错：以为 BF16 没有数值问题——低尾数在极深网络与长序列累加中仍有可观误差。

#### 9. 实践与验证

`[待验证]` 固定 seed，先做单卡 fp32/bf16 基线，再开 fp16 动态 scaling 与 FP8，逐层比较激活与梯度的相对误差；oracle：fp16 的 scale 轨迹随训练稳定收敛、溢出计数稀疏，FP8 层在容差内且无持续贴边张量。FP8 与各硬件的实际行为、kernel 差异需目标平台，本仓库未执行。

#### 10. 评分标准

- 不合格：只会说"用 fp16 加 loss scaling"。
- 合格：能区分 FP16/BF16/FP8 的范围与精度风险及各自机制。
- 良好：能给出 scale 更新规则、FP8 delayed scaling，并指出跨 rank scale 一致性问题。
- 优秀/专家：能设计监控闭环（溢出计数、scale 轨迹、amax 分布）与灰度验证流程。

**证据与来源：** `insights/05-megatron-architecture.md` 12A.1–12A.4（精度格式与 FP8 要点，`C`+`I`）；`megatron-lm/01-modules/M05-optimizer-checkpointing/implementation.md`（`MixedPrecisionOptimizer` 维护 scaling 与溢出，`C`）。FP8 硬件事项标 `U / 待验证`。

---

### T-E005. 长上下文训练如何做并行与显存设计？CP、SP、recompute 各自解决哪一段代价？

- 难度：专家级
- 知识域：T06 序列与上下文并行
- 考察能力：按显存增长来源分而治之，并识别 CP 的隐藏通信代价
- 题型：开放设计
- 面试频率：中高频
- 前置知识：attention s² 复杂度、packed sequence/cu_seqlens、selective recompute 判据
- 关联题目：T-E002（并行权衡）、T-E014（CP 风险评审）、T-E007（推理长上下文）
- 事实状态：CP 不切权重且需额外归约梯度复制份、CP≥8K 建议、selective recompute 判据为文档事实（`C`）；实际重叠收益为 `U / 待验证`

#### 1. 背景知识

长序列下激活显存按 s 线性增长、attention 计算按 s² 增长，而 TP 切 hidden、DP 切 batch，都不能降低 s 维代价（`C`，insights 9.1）。CP 沿序列切分，激活约降 1/N、但不切权重，backward 需额外归约复制权重的梯度，且归约域与 DP 复合（`C`，M02 design + insights 9.7）。SP 切 LayerNorm/Dropout 的序列维，带宽等价于 TP 不额外增加（`C`）。selective recompute 的判据 `5·a·s/h > 34`（`C`，insights 12.3）。M04 的 packed sequence 生成 cu_seqlens 供 varlen attention（`C`）。

#### 2. 问题分析

1. **先分类显存增长源**：绝大多数层的激活随 s 线性；attention 的中间量（QKᵀ、softmax）随 s²。两者要分别处理，CP 主要压 attention 的 s²，SP 压非 TP 区的线性部分。
2. **CP 不是免费**：它不切权重，backward 要在复制权重上额外归约，通信域是 CP∩DP；未与计算重叠时通信占主（`C`）。
3. **数据布局是前提**：长序列通常 packed，必须携带 cu_seqlens 等元数据，否则 varlen attention 与 mask 会错位（`C`）。

#### 3. 具体答案

面试中可以这样回答：

> 我先看显存是被谁吃掉的。如果瓶颈是 attention 的 s² 中间量，用 CP 沿序列切，把每 rank 的序列段和对应的 attention 块降下来；如果瓶颈是 LayerNorm/Dropout 这类非 TP 区的线性激活，用 SP 切序列维，它不额外增加带宽。两者可叠加。
>
> CP 有个代价必须一起说：它不切权重，所以权重在 CP 组内是复制的，backward 要额外归约这些复制份的梯度，而 distributed optimizer 又要在 DP 域做 reduce-scatter，于是梯度归约域变成 CP∩DP 的复合。所以 CP 的净收益取决于通信能否被计算重叠，官方给的建议是序列 ≥8K 时启用，并强调 overlap。
>
> 再叠加 selective recompute：对 attention 里体积大、计算密度低的区域做 checkpoint 重算，判据是 `5·a·s/h > 34`，超过就重算更划算，额外 FLOPs 很小。
>
> 数据侧我会确保使用 packed sequence 并正确生成 cu_seqlens 与 mask，长序列下 mask/position 一旦错位，训练仍能跑但学的是错误的东西。

#### 4. 技术洞察

按增长源分解激活：

```text
mem_act(s) ≈ mem_linear(s) + mem_attention(s²)
  mem_linear  ~ L * s * b * h            # 被 SP 除, 或 recompute
  mem_attention ~ a * s² * b / CP        # 被 CP 除
CP 代价:
  grad_reduce_domain = CP ∪ DP           # 权重复制份额外归约
recompute 判据:
  if 5*a*s/h > 34: recompute region      # 重算更省激活
```

这套分解解释了"CP 管 s²、SP 管线性、recompute 兜底、overlap 决定 CP 是否划算"。

#### 5. 图文说明

| 手段 | 主要压什么 | 代价 | 触发条件 |
|---|---|---|---|
| SP | 非 TP 区线性激活 | layout 转换通信 | 与 TP 并用 |
| CP | attention s² 与序列激活 | 复制权重梯度额外归约、序列通信 | s ≥ 8K |
| selective recompute | 大体积低密度激活 | 少量额外 FLOPs | `5as/h>34` |
| packed seq | 无（是前提） | 需正确 cu_seqlens | 变长序列 |

```text
s² 中间量 ──> CP 切序列 ──> 每 rank 一段 + ring/p2p 通信 ──> 与计算 overlap
线性激活 ──> SP 切 LN/Dropout ──> 带宽等价
残余      ──> selective recompute
```

#### 6. 拓展知识

- 与 T-E007 交叉：训练侧 CP 切长序列、推理侧靠 KV 池化/压缩解决同一"容量 vs 序列长"边界（`I`）。
- 与 T-E014 交叉：CP 的合法性与梯度归约域是引入新并行技术时必须重算的不变量。
- 与 T-E011 交叉：packed sequence 的 mask/position 正确性是数据门禁的一部分。

#### 7. 常见追问及回答要点

1. **CP 和 SP 能否混用？** ——可以且维度正交：CP 沿序列、SP 与 TP 配合处理非 TP 区；但都要保证 layout 转换链一致。
2. **CP 为什么不减少权重显存？** ——它只切激活，权重在组内复制；因此梯度需要额外归约复制份（`C`）。
3. **recompute 会不会拖慢？** ——full recompute 有 30–40% 计算开销，selective 只重算小部分，额外 FLOPs 约百分之几（`C`）。

#### 8. 易错点与反例

- 反例：只开 CP 不做 overlap，序列通信占主，长序列下吞吐反而低于不开。
- 反例：packed sequence 忘带 cu_seqlens，varlen attention 把不同文档拼在一起，loss 异常但不报错。
- 易错：认为 CP 像 TP 一样切权重，导致 checkpoint 并行度假设错误。

#### 9. 实践与验证

`[待验证]` 固定模型，序列从 4K 增到 32K，分别测 TP-only、TP+SP、加 CP、再加 selective recompute 四组；oracle：激活显存随 CP 增加而下降、`5as/h` 判据与实测省下的激活比例方向一致、CP 通信在 nsys 中与计算重叠。具体带宽与重叠收益需目标硬件，本仓库未执行。

#### 10. 评分标准

- 不合格：只会说"长序列用 CP"。
- 合格：能区分 s² 与线性激活来源、说出 CP 不切权重。
- 良好：能给出 CP 的梯度归约复合域、SP 带宽等价、recompute 判据。
- 优秀/专家：能把 packed seq 元数据、overlap 与 CP 净收益统一进设计，并指出未重叠时 CP 可能负收益。

**证据与来源：** `insights/05-megatron-architecture.md` 9.1–9.7（CP 机制与代价，`C`+`I`）、12.3（selective recompute 判据，`C`）；`megatron-lm/01-modules/M02-parallelism/design.md`（CP 复制权重梯度，`C`）；`M04-data-pipeline/README.md`（packed sequence 与 cu_seqlens，`C`）。CP 实测收益标 `U / 待验证`。

---

### T-E006. 容错训练中，checkpoint 的一致性边界在哪里？重放（replay）要保证什么才不破坏收敛？

- 难度：专家级
- 知识域：T10 检查点、恢复与容错
- 考察能力：定义全局一致切面 + 识别异步保存与重放的隐性状态
- 题型：开放设计 + 故障分析
- 面试频率：中高频（大规模训练）
- 前置知识：distributed checkpoint / sharded state dict、RNG 状态、data iterator 位置、异步保存
- 关联题目：T-E001（状态不变量）、T-E008（故障注入）、T-E015（版本兼容）
- 事实状态：checkpoint 涉及的状态集合与 `check_checkpoint_args` 有源码依据（`C`）；异步保存与恢复语义未实测（`U / 待验证`）

#### 1. 背景知识

传统 checkpoint 按 iteration、TP/PP/EP rank 与 distributed optimizer 形成路径；distributed checkpoint 用 sharded state dict 与 metadata，save/load 同时涉及模型、optimizer、scheduler、RNG、tokenizer、数据加载状态与 tracker（`C`，M05 README）。`check_checkpoint_args` 会对关键结构参数不匹配拒绝或告警（`C`）。optimizer.step 是最大的状态集合点：save 必须在 step 完成后读取一致的参数/优化器分片（`I`，insights 12.5）。异步保存与 non-persistent/local checkpoint 的清理和恢复语义尚未实测（`C`，M05 风险）。

#### 2. 问题分析

1. **一致切面的定义**：一次 checkpoint 必须是一个跨越所有 rank、跨越所有状态分量的"切面"，所有 rank 必须停在同一 global step。异步保存容易在后台读到下一步的参数。
2. **隐性状态易被遗漏**：RNG 状态、data iterator（已消费 token/sample 位置）、scheduler 的 lr 位置、loss scaler 的 scale，漏掉任何一个，重启后轨迹都与不中断训练不同。
3. **重放的等价性目标不能是逐位**：并行改变归约顺序会带来浮点差异；重放应保证"分布等价/曲线连续"，而不是逐位复现（`I`）。

#### 3. 具体答案

面试中可以这样回答：

> 我把 checkpoint 当成训练状态机上的一个切面来定义：只有当所有 rank 都停在同一 global step，且 model、optimizer、scheduler、RNG、data iterator、tokenizer、tracker 都取自这同一步时，才允许落盘。最自然的切点是一次 optimizer.step 成功之后、下一次 forward 之前。
>
> 异步保存最危险的地方在于：后台线程读参数时主循环可能已经在写下一步，于是存下来的是"参数是第 k 步、优化器状态是第 k+1 步"的拼接体。所以我会加一个保存屏障，或者在保存期间冻结参数更新，让保存代理看到的是同一 step 的快照。
>
> 重放要保证的是轨迹连续，而不是逐位相同。恢复后我期望 loss 曲线与不中断训练在容差内续接、数据不重复不跳过、lr 与 scale 从正确位置继续。data iterator 的位置必须随 checkpoint 一起存，否则重启后数据会跳一段，长训练里这会改变有效数据分布。
>
> 拓扑变了要做 reshard，distributed checkpoint 的 sharded metadata 支持并行度变化后的重映射，前提是保存与加载两侧的模型结构与分片 schema 能对齐。

#### 4. 技术洞察

用切面语义写重放契约：

```text
Save(step k):
  barrier_all_ranks()                 # 所有 rank 到同一 step
  assert step_r == k for all r
  persist { model_k, opt_k, sched_k, rng_k, data_pos_k, scaler_k }

Load -> Resume:
  restore all above components atomically
  assert resumed_step == saved_step
  assert data_pos consistent with step (无重复/无跳段)

Replay equivalence (目标, 非逐位):
  E[ loss_resume(t) ]  ≈  E[ loss_no_restart(t) ]   for t > restart
```

关键不变量：`saved_components` 必须是同一步的笛卡尔切面；`data_pos` 与 `step` 必须一致。

#### 5. 图文说明

| 状态分量 | 缺失后果 | 保存时机 | 证据 |
|---|---|---|---|
| model 参数 | 权重回退 | step 后 | `C` |
| optimizer 状态 | Adam 动量错位 | step 后 | `C` |
| scheduler | lr 跳变 | step 后 | `C` |
| RNG | dropout/shuffle 不连续 | step 后 | `C`/`I` |
| data iterator | 数据跳段/重复 | step 后 | `I` |
| loss scaler scale | 溢出行为突变 | step 后 | `I` |

```text
... step k-1 -> [step k] -> barrier -> SNAPSHOT(k) -> step k+1 ...
                    ^ 成功提交点          ^ 唯一合法切面
```

#### 6. 拓展知识

- 与 T-E008 交叉：故障越频繁，checkpoint 间隔与保存屏障的开销越关键，goodput 直接受影响。
- 与 T-E015 交叉：版本升级后 sharded schema 变化，需要 reshard 或转换，否则 load 失败。
- 与 T-E001 交叉：I5"状态切面"正是这里的形式化形式。

#### 7. 常见追问及回答要点

1. **异步保存如何保证一致？** ——两个做法：保存前屏障快照，或保存期间暂停参数更新；否则后台读到的可能是不同步的状态（`I`）。
2. **为什么重放不要求逐位相同？** ——并行改变算子与归约顺序，浮点结果只保证数学等价近似（`I`）。
3. **data iterator 一定要存吗？** ——要。否则重启后数据跳段或重复，长训练里改变有效训练分布。

#### 8. 易错点与反例

- 反例：异步保存读到"参数 k 步、优化器 k+1 步"的拼接体，恢复后 loss 突跳。
- 反例：只存模型与优化器，漏掉 RNG 与 data 位置，重启后 dropout/shuffle 序列改变，被误判为随机性。
- 易错：并行度改变后直接 load 旧 checkpoint，未做 reshard，shard schema 不匹配。

#### 9. 实践与验证

`[待验证]` 在 D01 训练上跑若干步存 checkpoint，中断后恢复，与不中断基线对比 loss 曲线；再改一次并行度验证 reshard；oracle：恢复后 loss 曲线在容差内续接、数据消费位置与 step 一致、reshard 后参数数值与加载前一致。异步保存的一致性与 non-persistent/local checkpoint 清理语义需实测，本仓库未执行。

#### 10. 评分标准

- 不合格：只知道"定期存 checkpoint"。
- 合格：能列出 model/optimizer/scheduler/RNG/data 等状态分量与统一切面。
- 良好：能指出异步保存的一致性风险与保存屏障思路。
- 优秀/专家：能定义重放的等价目标（曲线续接而非逐位）、处理拓扑变化的 reshard、并解释失配的数值后果。

**证据与来源：** `megatron-lm/01-modules/M05-optimizer-checkpointing/README.md`（状态集合与 `check_checkpoint_args`，`C`）、`M05-optimizer-checkpointing/implementation.md`（sharded metadata 与两层 checkpoint，`C`）；`insights/05-megatron-architecture.md` 12.5、14.1、14.5（状态集合点与"复现≠逐位"，`I`）。异步保存标 `U / 待验证`。

---

### T-E007. 训练-推理一致性如何验证？训练侧的并行布局在推理时会以哪些方式破坏等价性？

- 难度：专家级
- 知识域：T12 数据管线与交付
- 考察能力：识别跨阶段契约漂移，并设计可判定的等价验证
- 题型：开放设计 + 验证框架
- 面试频率：中频（训练到服务链路岗）
- 前置知识：TP head 分片、GQA/MLA KV 布局、checkpoint 转换、flash vs 朴素 attention
- 关联题目：T-E015（版本/格式迁移）、T-E004（精度差异）、T-E005（长上下文）
- 事实状态：TP 决定 KV 物理布局、Bridge 双向转换、static 引擎回退 legacy 有依据（`C`）；logit 级差异需实测（`U / 待验证`）

#### 1. 背景知识

QKV 投影列切后每个 TP rank 持部分 head 的 KV，KV cache 相应按 head 维分片（`C`，insights 14.2）。GQA 要求每 rank 持整数个 KV head，KV head 数不能被 t 整除时需复制或 padding（`I`）。Megatron-Bridge 提供与 HuggingFace 的双向 checkpoint 转换，模型并行感知（`C`，insights 14.1）。推理侧 `StaticInferenceEngine` 非 legacy 路径会创建 dynamic context/engine，异常时回退 legacy，因此实际执行 backend 依赖安装与配置，不能从调用名断定（`C`，M06）。

#### 2. 问题分析

1. **等价性的定义**：训练模型与推理引擎在同一权重、同一输入上应给出容差内的相同输出。差异来源有四类：并行布局、数值精度、attention kernel、输入预处理（mask/padding/position）。
2. **布局契约漂移最隐蔽**：TP 的 head 分片与 GQA 的 KV 复制、MLA 的 latent 结构决定 KV tensor 的实际 layout；训练权重转换到推理引擎时若 head 归属不一致，错的是语义而非形状。
3. **执行路径不可假设**：static→dynamic 的回退意味着同名接口下可能跑不同 backend，验证必须覆盖实际路径而非文档路径。

#### 3. 具体答案

面试中可以这样回答：

> 我先给等价性一个可判定的定义：给定同一份 checkpoint、同一段 token、同一采样参数，训练侧算出的 logits（或若干层的中间 hidden）与推理引擎的输出，在给定容差内一致。然后我逐类排查偏离来源。
>
> 布局类：训练时 QKV 按 head 列切到 TP rank，推理的 KV cache 也按 head 分片；GQA 里 KV head 数不能被 TP 整除时要复制或 padding，训练和推理必须采用同一约定，否则某个 head 的 KV 落到不同 rank 上。MLA 更麻烦，latent 解压的升维 GEMM 也要对齐切分。
>
> 数值类：训练可能用 FP8/BF16，推理 kernel 未必提供同样精度路径，我要在容差里吸收这种系统性差异，而不是要求逐位相等。
>
> kernel 类：训练用 flash 系 attention、推理可能用不同实现，softmax 累加顺序不同会带来小差异，通常可接受但要量化。
>
> 预处理类：mask、position、packed 边界必须一致，长序列下尤其容易错。
>
> 执行路径类：我会确认推理实际走的是 dynamic 还是 legacy 路径——M06 里 static 引擎会尝试转 dynamic，失败回退 legacy，所以同名调用不代表同一 backend。验证要按实际路径跑。

#### 4. 技术洞察

把一致性写成等价性谓词并分层定位：

```text
Equiv(train, infer; x) := || f_train(x; sharded) - f_infer(x; engine) || <= eps

分解偏差来源:
  d_total <= d_layout + d_precision + d_kernel + d_preprocess
定位顺序:
  1) 单卡 TP=1、无 CP/EP、一致精度   -> 应近似相等 (隔离布局与精度)
  2) 加布局 (TP/CP/EP)                -> 看 d_layout
  3) 换精度 (FP8)                     -> 看 d_precision
  4) 换 attention kernel              -> 看 d_kernel
```

要点：控制变量，逐层加轴/换精度，让每一类偏差可归因。

#### 5. 图文说明

| 偏差来源 | 典型触发 | 影响 | 验证手段 |
|---|---|---|---|
| 布局 | TP head / GQA KV / MLA latent | 语义错（非形状错） | TP=1 基线 + 逐轴开 |
| 精度 | FP8/bf16 kernel 差异 | 系统性偏移 | 容差 + 分位差 |
| kernel | flash vs 朴素 attention | 小数值差 | 固定输入的逐层 diff |
| 预处理 | mask/position/packed | 长序列错位 | 输入级断言 |
| 执行路径 | static→dynamic 回退 | backend 不一致 | 运行期确认实际引擎 |

```text
训练 checkpoint ──Bridge 转换──> 推理权重 ──引擎──> logits
         |                                          |
         └────同一输入 logits(训练)──── diff ────────┘
                      (逐层加轴定位偏差来源)
```

#### 6. 拓展知识

- 与 T-E015 交叉：checkpoint 格式版本与并行布局共同构成迁移时的契约，转换要双向可校验。
- 与 T-E004 交叉：精度差异是训练-推理偏差的固定成分，应在容差建模里显式考虑。
- 与 T-E005 交叉：长序列下 CP 布局与推理 KV 池化的分片契约必须对齐。

#### 7. 常见追问及回答要点

1. **为什么不用逐位比较？** ——并行与 kernel 差异导致归约顺序不同，逐位一致不成立；目标是容差内的语义一致。
2. **GQA 的 KV head 不能整除 TP 怎么办？** ——复制 KV 投影或 padding head，训练与推理必须同约定（`I`）。
3. **static 引擎为什么可能跑成 dynamic？** ——非 legacy 路径会尝试转 dynamic context，异常回退 legacy，实际 backend 依环境而定（`C`）。

#### 8. 易错点与反例

- 反例：训练用 GQA KV 复制、推理按 head 均分，输出整体正常但个别 head 语义错，评测指标掉点难以归因。
- 反例：把 M06 的 static 引擎调用当成实际执行路径，忽略了回退 legacy，验证覆盖不到真实 backend。
- 易错：以逐位相等作为通过标准，导致任何 kernel 升级都被判失败。

#### 9. 实践与验证

`[待验证]` 用同一 checkpoint 在训练侧与推理引擎上对同一批 token 计算 logits，控制变量逐步加 TP/CP/EP 与切精度；oracle：TP=1 一致精度下偏差最小，加布局/精度/kernel 后偏差单调可控且能归因到对应类别；定位到具体层与 head。logit 级偏差量与容差需实测，本仓库未执行。

#### 10. 评分标准

- 不合格：只说"用同一个 checkpoint 就对"。
- 合格：能列出布局/精度/kernel/预处理四类偏差来源。
- 良好：能给出控制变量的逐层定位方法与等价性谓词。
- 优秀/专家：能把 GQA/MLA 的 KV 布局契约、执行路径回退、容差建模统一进验证框架。

**证据与来源：** `insights/05-megatron-architecture.md` 14.1–14.3（KV 布局、Bridge、P/D，`C`+`I`）、6.10（GQA/MLA 对 TP 切分的影响，`I`）；`megatron-lm/01-modules/M06-inference-and-tools/implementation.md`（static→dynamic 回退，`C`）。logit 偏差标 `U / 待验证`。

---

### T-E008. 数千卡规模下如何做故障注入与降级策略，让训练在故障中保持有效进度？

- 难度：专家级
- 知识域：T10 检查点、恢复与容错
- 考察能力：故障分类、可用性建模与分级降级设计
- 题型：开放设计 + 故障分析
- 面试频率：中频（大规模/平台岗）
- 前置知识：MTBF/MTTR、NCCL hang、checkpoint 间隔、goodput、exit code 语义
- 关联题目：T-E006（checkpoint 一致）、T-E012（可观测）、T-E013（平台化）
- 事实状态：错误三分类来自文档（`C`）；MTBF、重启开销、降级收益需线上指标（`U / 待验证`）

#### 1. 背景知识

Megatron 训练错误大体分三类：配置/初始化错误（整除失败、非法组合、数据集缺失，早失败）、数值/收敛错误（inf/nan，查 scaling/FP8/学习率/数据）、系统/通信错误（NCCL hang、OOM、checkpoint 不一致）（`C`，insights 14.7）。建议采用"保存-重启"策略：周期 sharded checkpoint + async save，失败后从最近 checkpoint 恢复（`C`，insights 14.7）。M06 静态引擎在动态化失败时保留 legacy fallback，说明"降级到旧路径"是既有的设计手法（`C`）。

#### 2. 问题分析

1. **故障按可恢复性分层**：瞬时（NCCL hang 一次、单节点抖动）→ 重试/重启；持续（坏卡、慢节点）→ 隔离与降级；逻辑（配置错、数值崩）→ 需人工或策略回退。
2. **有效进度是目标，不是"作业不失败"**：真正指标是 goodput = 有效训练时间 / 总墙钟时间，包含重启与恢复开销。
3. **降级要有阶梯且可回滚**：降 batch、关 overlap、回退引擎/精度、reshard checkpoint、隔离节点，每一级都要能回到上一级，否则降级本身成为新故障源。

#### 3. 具体答案

面试中可以这样回答：

> 我先把故障分成三类：配置/初始化类早失败，靠启动前校验拦掉；数值类靠 loss scaling、FP8 scale 与数据门禁拦掉；系统类最难，NCCL hang、OOM、慢节点、checkpoint 不一致。规模越大，瞬时故障越多，所以策略核心是"频繁可恢复的 checkpoint + 快速重启"。
>
> 故障注入是验证这套策略的手段：主动注入 hang、OOM、单 rank 异常退出、网络抖动，观察系统能否在预期时间内恢复并续接。
>
> 降级我设计成阶梯，从轻到重：先降在飞 microbatch 与 batch 缓解 OOM，再关闭部分 overlap 降低偶发竞争，再回退精度档或引擎路径，再 reshard checkpoint 换更小拓扑，最后隔离坏节点。每一级都要能用上一步的 checkpoint 回滚。
>
> 衡量指标是 goodput——有效训练时间除以总墙钟时间。checkpoint 间隔是它的直接杠杆：太稀则故障后白跑很多，太密则保存屏障拖慢训练。

#### 4. 技术洞察

可用性建模（用于推理，不是实测）：

```text
checkpoint 间隔 = T_ckpt
故障到达率 = λ (1/MTBF)
平均每故障白跑时间 ≈ T_ckpt / 2
恢复开销 r = 检测 + 重启 + 加载

goodput ≈ MTBF / (MTBF + T_ckpt/2 + r)

最优 T_ckpt* ≈ sqrt(2 * r / λ)     # 粗平衡: 保存成本 vs 白跑成本
```

关键：T_ckpt 太小，保存屏障本身吃掉吞吐；太大，故障后白跑多。存在内点最优。

#### 5. 图文说明

| 故障类型 | 症状 | 处置层级 | 回滚点 |
|---|---|---|---|
| NCCL hang | 卡住不退出 | 重启 + 换拓扑 | 最近 ckpt |
| OOM | 中断 | 降 MB/batch、开 recompute | 最近 ckpt |
| 慢节点 | 吞吐塌陷 | 隔离节点 + reshard | 最近 ckpt |
| checkpoint 不一致 | load 失败/跳变 | 用上一份 ckpt | 上上份 ckpt |
| inf/nan | 数值崩 | 回退精度档、查数据 | 最近 ckpt |

```text
[正常] ──故障──> 检测 ──> 重试/重启 ──> 恢复 ckpt ──> 续接
                       └─持续故障─> 降级阶梯(可回滚) ──> 继续
指标: goodput = 有效训练时间 / 总墙钟时间
```

#### 6. 拓展知识

- 与 T-E006 交叉：降级中 reshape 拓扑依赖 checkpoint 的 reshard 能力。
- 与 T-E012 交叉：故障检测、恢复耗时、降级触发都要作为可观测指标上报。
- 与 T-E013 交叉：平台化后，故障编排与调度器结合，降级策略成为平台能力。

#### 7. 常见追问及回答要点

1. **checkpoint 越频繁越好吗？** ——不是，保存屏障有成本；goodput 模型给出内点最优间隔（`I`）。
2. **慢节点怎么发现？** ——靠 per-rank 计时分布与通信等待统计，单节点抖动会表现为它所在通信组的额外等待（`I`）。
3. **降级到什么程度算失败？** ——预设一个不可回滚的下限（例如精度或拓扑会导致无法与之前状态续接），触底即停止并求助人工。

#### 8. 易错点与反例

- 反例：只追求"作业不退出"，频繁重启却没保证 goodput，实际训练进度极慢。
- 反例：降级时换了拓扑但没 reshard checkpoint，恢复失败后又被误判为新故障。
- 易错：把瞬时 hang 当坏节点永久隔离，浪费资源。

#### 9. 实践与验证

`[待验证]` 在目标集群注入 hang/OOM/单 rank 退出三类故障，记录检测时间、恢复时间与白跑步数；oracle：每类故障能在预期窗口恢复并续接，goodput 可计算，降级每级可回滚。MTBF、恢复开销、降级收益均为线上指标，本仓库未执行。

#### 10. 评分标准

- 不合格：只说"定期存、失败重启"。
- 合格：能按可恢复性分类故障并给出重启策略。
- 良好：能定义 goodput 并说明 checkpoint 间隔的权衡。
- 优秀/专家：能给出 goodput 的近似模型、分级可回滚降级阶梯，并区分瞬时/持续/逻辑故障的处置。

**证据与来源：** `insights/05-megatron-architecture.md` 14.7（错误分类与保存-重启，`C`）、15.2（失效模式清单，`I`）；`megatron-lm/01-modules/M06-inference-and-tools/implementation.md`（legacy fallback，`C`）；`M05-optimizer-checkpointing/README.md`（checkpoint 与 async save，`C`，运行语义 `U`）。

---

### T-E009. 训练吞吐与成本应该用哪套标准化度量？为什么同一配置两个人报出的数字能差一倍？

- 难度：专家级
- 知识域：T11 显存、性能与吞吐
- 考察能力：度量口径的严格定义 + 识别不可比因素
- 题型：开放设计 + 定量
- 面试频率：中高频
- 前置知识：MFU/HFU、tokens/s、strong/weak scaling、goodput、profiler 归因
- 关联题目：T-E002（并行权衡）、T-E008（goodput）、T-E012（可观测）
- 事实状态：MFU 定义与测量纪律来自文档（`C`+建议）；论文数字为作者报告，非本地复现（`C`）

#### 1. 背景知识

step 时间 ≈ compute + communication + idle（`I`）。MFU = 实际模型 FLOPs /（GPU 数 × 单卡峰值 × 运行时长）（`C`，insights 12B.2）。测量纪律建议固定：全局 batch 大小、每 step 时间（而非每秒样本数）、warmup 与重复次数（≥3 次取中位数）、用 profiler 区分 comm 与 idle（建议，insights 12B.4）。论文报告 530B 在 2240 A100 上 MFU 54.2%（作者报告，`C`）。

#### 2. 问题分析

1. **口径混用是数字差异的主因**：raw step time vs goodput（扣重启）、strong vs weak scaling、MFU vs HFU（后者把 recompute 的额外 FLOPs 也计入、数字更高）、是否含数据加载。
2. **单位要对齐**：tokens/s 与 samples/s 在变长序列下不等价，必须绑定序列长度与 batch 定义。
3. **成本是"每有效 token 的花费"**：`cost = 时间 × GPU 数 × 单价`，除以有效 token 数才是可比成本；用 raw 吞吐会低估重启频繁配置的真实成本。

#### 3. 具体答案

面试中可以这样回答：

> 我会固定一套度量口径再比。吞吐报 tokens/s，并且绑定全局 batch 与序列长度；利用率报 MFU，公式是实际模型 FLOPs 除以 GPU 数乘峰值算力乘时长。若要和"含重算"的口径比，必须声明是 MFU 还是 HFU——重算的额外 FLOPs 会抬高 HFU，两个数字不可直接比。
>
> 扩展性上先说清是 strong scaling（固定 batch）还是 weak scaling（batch 随卡数放大），两者扩展效率口径完全不同。
>
> 成本口径我会用"每有效 token 的花费"：`时间 × GPU 数 × 单价 / 有效 token 数`，其中有效 token 要去掉重启白跑的时间，也就是用 goodput 而不是 raw step time。
>
> 测量纪律我会固定 warmup 与重复次数、取中位数，并用 nsys/NCCL trace 把 comm 与 idle 单独归因。这样两个人报出的差价，通常能定位到口径不同——比如一个含补数据加载、一个用 strong scaling、或者一个用了 HFU。

#### 4. 技术洞察

统一到可比较的公式：

```text
MFU   = (6 * N_params * tokens) / (t_run * N_gpu * P_peak)   # 训练: fwd+bwd ≈ 6N/token
HFU   = include_recompute_FLOPs / 同一分母                   # HFU >= MFU
tokens/s = tokens_step / t_step
goodput  = t_有效训练 / t_墙钟
cost/token = t_墙钟 * N_gpu * price / tokens_有效
```

为什么差一倍：分母里 `t_run` 用 step time 还是墙钟（含重启）、FLOPs 口径 MFU vs HFU、scaling 口径 strong vs weak，三者叠加就够翻倍。

#### 5. 图文说明

| 指标 | 定义 | 常见坑 | 可比前提 |
|---|---|---|---|
| tokens/s | 每步 token / step 时间 | 序列长度未绑定 | 固定 batch/seq |
| MFU | 模型 FLOPs / 峰值算力时间 | 与 HFU 混用 | 声明 FLOPs 口径 |
| HFU | 含重算 FLOPs 利用率 | 被当成 MFU | 声明含重算 |
| 扩展效率 | 相对单卡/小规模 | strong/weak 混用 | 固定 scaling 类型 |
| cost/token | 花费/有效 token | 用 raw 吞吐低估 | 用 goodput |

```text
t_墙钟 = t_有效训练 + t_重启白跑 + t_加载
分母口径不同 ──> 数字不可比 ──> 统一后再比较
```

#### 6. 拓展知识

- 与 T-E002 交叉：配置搜索的目标函数若用 raw step time，会偏好短 checkpoint 间隔但重启贵的配置。
- 与 T-E008 交叉：goodput 是容错策略的直接 KPI。
- 与 T-E012 交叉：trace 归因是分辨 compute/comm/idle 的前提。

#### 7. 常见追问及回答要点

1. **MFU 和 HFU 差在哪？** ——HFU 把重算引入的额外 FLOPs 也算进实际计算量，数字更高，不能与 MFU 直接比。
2. **tokens/s 与 samples/s 何时不等价？** ——变长序列下每个 sample 的 token 数不同，必须绑定序列长度。
3. **为什么成本要用有效 token？** ——重启白跑的时间不是产出，用 raw 吞吐会低估频繁重启配置的真实成本。

#### 8. 易错点与反例

- 反例：一个报 MFU、一个报含重算的 HFU，直接比较得出"新配置更快"的错误结论。
- 反例：strong scaling 的扩展效率与 weak scaling 混在一起画曲线。
- 易错：把某个配置的纸面峰值算力当分母，而不看实测 MFU，导致规划过度乐观。

#### 9. 实践与验证

`[待验证]` 在同一硬件上跑同一配置，分别用 raw step time、goodput、MFU、HFU 四种口径报告，并做 strong/weak 两组；oracle：口径写在报告里后，两组数字可解释地收敛，nsys 里 comm/idle 占比可复算 MFU 的损失来源。具体 MFU 数值与扩展效率需目标硬件与较长训练，本仓库未执行。

#### 10. 评分标准

- 不合格：只报一个"吞吐"数字，不声明口径。
- 合格：能定义 MFU/tokens/s 并指出 strong/weak 的差异。
- 良好：能区分 MFU 与 HFU、说明 goodput 与成本的关系。
- 优秀/专家：能给出统一公式解释"数字差一倍"的口径来源，并设计可复算的归因流程。

**证据与来源：** `insights/05-megatron-architecture.md` 12B.1–12B.6（step 时间分解、MFU 定义、测量纪律，`C`+建议）；论文数字为作者报告，非本地复现。运行类结论标 `U / 待验证`。

---

### T-E010. 集群从同构走向异构（混合代际 GPU、混合互连）时，并行策略应如何演进？

- 难度：专家级
- 知识域：T02 分布式初始化与通信组
- 考察能力：把"排布法则"从同构推广到异构，并识别新引入的一致性风险
- 题型：开放设计
- 面试频率：中频
- 前置知识：axis order/stride 编码、NVLink vs IB 带宽、heterogeneous parallelism、FSDP+EP 双 DeviceMesh
- 关联题目：T-E002（并行权衡）、T-E003（Folding）、T-E014（新并行风险）
- 事实状态：heterogeneous parallelism 与 Folding、双 DeviceMesh 有文档依据（`C`+`I`）；异构吞吐收益为 `U / 待验证`

#### 1. 背景知识

同构集群的排布法则：通信量排序 TP>DP>PP，因此 TP 置于同节点（NVLink 域）、DP 尽量同节点或近邻、PP 可跨节点（`C`，insights 8/10.2）。RankGenerator 把各轴映射为 shape/stride 生成 rank 列表，axis order 决定谁与谁同组（`C`）。异构并行允许不同 layer/module 使用不同并行配置，Folding 把 attention 与 MoE 层解耦；FSDP+EP 用双 DeviceMesh，专家参数只在 EDP 组内 shard（`C`，insights 10.5/10.6/11.3）。

#### 2. 问题分析

1. **异构下"同节点"不再是均匀概念**：不同代际 GPU 的算力与显存不同，NVLink 域形状也不同。把高通信轴放到"高带宽"域的规则要按实际拓扑重算，而不是假设节点内全互通。
2. **按能力分配轴**：强算力卡适合承担大 GEMM（高 TP），弱卡适合通信少的 stage（PP 边界），但阶差过大会使最慢卡成为瓶颈，需要负载均衡。
3. **新的一致性风险**：逐层使用不同并行配置时，层间切换需要 layout 转换；axis order 与 stride 必须逐层一致，否则 f/g 契约断裂（`I`，insights 10.6/16.5）。

#### 3. 具体答案

面试中可以这样回答：

> 同构的排布法则在异构下要重算。核心仍是"高通信轴靠高带宽域"，但异构节点里带宽域形状不均，得按实际拓扑把 TP 放到真正全互联的那部分、把 PP 放到跨节点、把 DP 放在同代际或近邻。代际差异大时，我会按算力与显存把强卡分给大 GEMM 的 stage。
>
> 演进路径上，我从"全模型统一配置"走到"按模块定制"：attention 层用 TP×CP，MoE 层用 EP×ETP，这正是 Parallel Folding 的思路；FSDP 与 EP 组合时用双 DeviceMesh，专家参数只在 EDP 组内 shard，避免把 all-gather 铺满整个 EP 组。
>
> 代价是层间 layout 转换：从 TP=4 的层过渡到 ETP=1、EP=8 的层，token dispatch 处要做转换，转换本身有开销，而且动态并行不利于 CUDA Graph 捕获。
>
> 我会特别盯一个风险：逐层的 axis order 与 stride 必须一致，否则同一个 rank 可能既当 TP rank 又当 CP rank，出错时是隐式的、不会显式报错，只能靠单测和逐轴开轴 diff 发现。

#### 4. 技术洞察

异构排布是带权重的分组问题：

```text
目标: 给定层集合与物理拓扑, 把通信密集轴放入高带宽域
约束:
  world_size == tp*pp*cp*dp
  每个节点/域的轴分配满足 mem & compute 上限
  layout(layer_i) -> layout(layer_{i+1}) 必须存在转换路径
代价:
  T = Σ_layer max_over_ranks(compute)         # 慢卡决定
      + Σ_boundary layout_conversion
      + Σ_axis comm/BW_actual(axis_placement)
```

关键：`max_over_ranks(compute)` 说明异构下负载均衡比同构更关键，一个慢卡拖住整个同步组。

#### 5. 图文说明

| 演进阶段 | 配置粒度 | 主要收益 | 新增风险 |
|---|---|---|---|
| 统一 3D | 全模型一套 | 简单、可预测 | 层类型互将就 |
| 异构 TP/PP | 按 stage | 匹配算力差异 | 慢卡瓶颈、布局转换 |
| Folding | attention vs MoE | 各层最优 | dispatch 处转换开销 |
| FSDP+EP 双 Mesh | 组内 shard | 减轻带宽 | Mesh 组合复杂 |

```text
Node N0 [强卡, 全互联] ── TP/CP 密集轴
Node N1 [弱卡, 部分互联] ── PP/低通信 stage
层间: layout(A) ──转换──> layout(B)
```

#### 6. 拓展知识

- 与 T-E002 交叉：异构下 `BW_tp` 等参数需按实际放置取值，配置搜索的可行域也随之改变。
- 与 T-E014 交叉：逐层 axis order 一致性是引入异构并行时最容易被忽略的不变量。
- 与 T-E008 交叉：异构集群更易出现慢节点，降级策略需覆盖拓扑重排。

#### 7. 常见追问及回答要点

1. **异构下 TP 还能靠"同节点"吗？** ——要看该节点是否真全互联；带宽域形状不均时按实际拓扑而非"节点"划分（`I`）。
2. **Folding 的代价是什么？** ——层间 layout 转换开销、拓扑不可预测使 CUDA Graph 捕获困难（`I`）。
3. **双 DeviceMesh 解决什么？** ——让专家 FSDP 只在 EDP 组内 all-gather/reduce-scatter，减轻全 EP 组带宽压力（`C`）。

#### 8. 易错点与反例

- 反例：把 TP 放进"标称同节点"但实际部分走 IB 的域，通信被拖慢，误判为 TP 不适合。
- 反例：逐层并行配置不同但 axis order 不一致，隐式 rank 复用，训练数值错乱但不报错。
- 易错：忽略慢卡对同步组的拖累，只看平均算力。

#### 9. 实践与验证

`[待验证]` 在混合代际的多节点环境，跑统一配置与异构/Folding 配置两组，记录每 rank 计算时间分布与 layout 转换耗时；oracle：异构配置下慢卡不再成为整体瓶颈、layout 转换开销可测量、axis order 一致性单测通过。异构收益与实际拓扑需目标集群，本仓库未执行。

#### 10. 评分标准

- 不合格：照搬同构"TP 同节点"口诀。
- 合格：能说明异构下需按实际带宽域重排、按算力分配轴。
- 良好：能解释 Folding 与双 DeviceMesh 的动机与代价。
- 优秀/专家：能识别逐层 axis order/stride 一致性风险、用带权分组与代价函数框架描述排布问题。

**证据与来源：** `insights/05-megatron-architecture.md` 10.2/10.5/10.6（排布法则与异构，`C`+`I`）、11.3/11.4（Folding 与双 Mesh，`C`）、16.5（axis order 风险，`I`）；`megatron-lm/01-modules/M02-parallelism/implementation.md`（RankGenerator shape/stride，`C`）。异构收益标 `U / 待验证`。

---

### T-E011. 数据质量与训练稳定性如何用治理门禁保证？哪些数据不变量一旦破坏就是静默错误？

- 难度：专家级
- 知识域：T12 数据管线与交付
- 考察能力：把数据侧不变量变成门禁，识别"能跑但错"的模式
- 题型：开放设计
- 面试频率：中频
- 前置知识：loss mask、packed sequence、sampler/DP 可见性、blend 采样权重、tokenizer
- 关联题目：T-E005（packed seq）、T-E001（数值不变量）、T-E004（数据引发的数值崩）
- 事实状态：M04 不变量为源码/文档事实（`C`）；数据门禁的实际拦截效果需实测（`U / 待验证`）

#### 1. 背景知识

M04 不变量（`C`）：tokens/labels 长度与 sequence-length 约束一致；loss mask 必须与有效 label 对齐；packed sequence 的 flatten tensor 必须伴随 cu-sequence metadata；sampler 的 DP rank/size 决定每个进程看到的 batch。`GPTDataset.__getitem__` 在 `add_extra_token_to_sequence` 下用 `text[:-1]`/`text[1:]`，否则 labels 是左移 text、末位填 pad，pad 从 loss mask 排除、输入 pad 映射为安全 token（`C`）。blend builder 按权重混合数据集（`C`）。

#### 2. 问题分析

1. **静默错误最危险**：mask 与 label 错位、packed 段 metadata 缺失，训练照跑、loss 照降，但学的是错误目标，评测掉点难以归因。
2. **门禁要在数据入口而非事后**：在 dataset 构建与 get_batch 边界做断言，比在 loss 异常后回溯便宜得多。
3. **质量与稳定性同源**：低质数据（重复、污染、超长/截断、tokenizer 不匹配）既损害质量也引发 loss 尖峰，门禁要同时服务两者。

#### 3. 具体答案

面试中可以这样回答：

> 我会把数据侧不变量做成构建期和 get_batch 期的门禁。构建期：tokenizer 与语料匹配、序列长度在上下界内、blend 权重归一、去重与污染检测；get_batch 期：tokens 与 labels 等长、loss mask 与有效 label 一一对齐、packed 时每个段的 cu_seqlens 存在且连续、sampler 的 DP rank/size 与当前 batch 划分一致。
>
> 其中几类是静默错误：mask 与 label 错位、packed 段边界丢失、pad 未从 loss 排除。它们不报错，只是让模型学错东西，所以我用断言而不是告警。
>
> 稳定性上，数据噪声是 loss 尖峰的常见来源。我会把超长截断、空文档、tokenizer 边界异常单独计数上报；一旦某批次异常比例超阈值就暂停并回退。
>
> CI 层面用 mock dataset 做自包含验证，保证数据管线改动不会破坏这些不变量。

#### 4. 技术洞察

把不变量写成门禁断言：

```text
build_time_assert:
  tokenizer.matches(corpus)
  min_seq <= len <= max_seq
  sum(blend_weights) == 1
  no_duplicate(contamination_check)

get_batch_assert:
  len(tokens) == len(labels)
  loss_mask[i] == 1  => label[i] is valid and != pad
  if packed: cu_seqlens 单调, 覆盖全长, 段数与文档数一致
  sampler_rank_local_batch == expected(global_batch, dp)
```

静默错误的三条：mask/label 错位、packed 边界丢失、pad 未排除——三者都满足"可运行、loss 下降、目标错误"。

#### 5. 图文说明

| 不变量 | 破坏后果 | 门禁位置 | 是否静默 |
|---|---|---|---|
| tokens/labels 等长 | 张量错位 | build | 否（多会崩） |
| loss mask 对齐 | 学错目标 | get_batch | 是 |
| packed cu_seqlens | 段拼接错误 | build | 是 |
| pad 排除 | 学 pad token | build | 是 |
| sampler DP 一致 | 数据重复/丢失 | get_batch | 部分 |

```text
语料 ──tokenizer──> 文档 ──blend──> dataset ──sampler──> batch ──get_batch──> 模型
      [门禁1:匹配/长度]   [门禁2:权重归一]  [门禁3:DP一致]  [门禁4:mask/packed]
```

#### 6. 拓展知识

- 与 T-E005 交叉：长序列 packed 的 cu_seqlens 既是数据不变量也是 attention 正确性前提。
- 与 T-E004 交叉：数据异常引发的 loss 尖峰常被误判为精度问题。
- 与 T-E012 交叉：数据门禁的计数与拦截率应作为可观测指标。

#### 7. 常见追问及回答要点

1. **为什么要断言而非告警？** ——静默错误不触发异常，只有断言能阻止错误数据进入训练。
2. **blend 权重为什么要归一？** ——否则采样分布偏斜，等价于改变了数据配比（`C`）。
3. **mock dataset 的意义？** ——让 CI 不依赖外部语料即可验证数据管线不变量（`C`）。

#### 8. 易错点与反例

- 反例：交换 mask 与 label 的错位几个月未被发现，直到评测异常才回溯。
- 反例：packed 序列丢失段 metadata，varlen attention 把不同文档算成一段。
- 易错：pad token 未被 loss mask 排除，模型学会预测 pad。

#### 9. 实践与验证

`[待验证]` 在 D01 mock dataset 上注入三类静默错误（错位 mask、丢 cu_seqlens、pad 未排除），验证门禁能拦截；oracle：每类错误在 get_batch/build 期被断言拒绝并给出定位信息，正常数据不误报。真实语料的污染与去重效果需在实际数据上评估，本仓库未执行。

#### 10. 评分标准

- 不合格：只说"数据要清洗"。
- 合格：能列出数据侧不变量并指出门禁位置。
- 良好：能识别哪些破坏是静默错误、并给出对应断言。
- 优秀/专家：能把数据质量与训练稳定性统一到门禁与可观测闭环，并用 mock dataset 支撑 CI。

**证据与来源：** `megatron-lm/01-modules/M04-data-pipeline/README.md`（不变量与样本语义，`C`）、`M04-data-pipeline/implementation.md`（masks/positions、sampler、get_batch，`C`）；`insights/05-megatron-architecture.md` 14.4（数据管线，`C`）。门禁实际效果标 `U / 待验证`。

---

### T-E012. 训练系统的可观测性应该如何分层设计？怎样在三分钟内定位"慢在哪一层"？

- 难度：专家级
- 知识域：T12 数据管线与交付
- 考察能力：指标分层、基数控制与快速归因
- 题型：开放设计
- 面试频率：中频
- 前置知识：metrics/logs/traces、timer、nsys/NCCL、CUDA Graph 捕获分析
- 关联题目：T-E002（吞吐）、T-E009（度量口径）、T-E008（故障检测）
- 事实状态：timer 与 telemetry 注入、log-interval、profiler 手段有源码/文档依据（`C`+`I`）；具体归因效率需实测（`U / 待验证`）

#### 1. 背景知识

Megatron 内置 timer 统计 per-iteration、per-microbatch、FP 与 BP 耗时（`timers('forward-compute')` 等），`--log-interval` 打印 tokens/s、MFU、loss（`C`+`I`，insights 14.6）。`pretrain` 会把 fault tolerance、telemetry 等横切能力注入运行时（`C`，M03 implementation）。分布式分析用 nsys/NCCL 查看每 step 通信与空闲比（`I`）。CUDA Graph 捕获分析确认算子是否静态 shape/地址（`I`）。

#### 2. 问题分析

1. **分层而非平铺**：作业级（goodput、总 tokens）→ step 级（step time 分解）→ 通信级（每原语耗时/体积）→ kernel 级（trace）。不同层回答不同问题，混在一起会淹没信号。
2. **基数控制**：per-rank × per-parameter × per-step 的指标基数爆炸，必须采样与聚合，否则观测本身成为负担。
3. **归因路径要固定**：先看 step time 是否上升，再拆 compute/comm/idle，再下钻到具体轴与 group，最后到 kernel。

#### 3. 具体答案

面试中可以这样回答：

> 我分四层。作业层看 goodput、有效 tokens、重启次数，回答"训练有没有在有效推进"。step 层拆 step time 为 compute、comm、idle，回答"慢在算还是等"。通信层统计每个原语的耗时与体积、绑定到具体并行组，回答"哪条通信在拖"。kernel 层用 nsys/NCCL trace 看空隙与 launch 抖动，回答"为什么 idle"。
>
> 三分钟定位的流程是固定的：先比当前 step time 与基线，若上升就查 idle 占比；idle 高就去通信层看是否某个 group 的 P2P/allreduce 变慢；若通信正常但 compute 高，就查是否 batch/序列变了或重算开了；若都正常，查数据加载是否成为前置等待。
>
> 指标基数上，per-rank 指标只上报聚合分位（如 p50/p99），异常时再按 rank 下钻；per-step 指标按 log-interval 采样。telemetry 在 pretrain 里就能注入，不用侵入模型代码。
>
> CUDA Graph 场景单独看捕获分析：确认被捕获算子是静态 shape/地址，MoE 的动态 capacity 会让捕获失效。

#### 4. 技术洞察

用分解树把 step time 归因：

```text
step_time
├── compute (fwd)      -> timer('forward-compute')
├── compute (bwd)
├── comm: Σ 原语       -> 绑定 group (tp/pp/dp/cp/ep)
├── idle:
│    ├── PP bubble
│    ├── 未重叠 allreduce/P2P/A2A
│    └── kernel launch 抖动
└── data wait           -> iterator 阻塞
```

归因伪代码：

```text
if step_time > baseline:
    if idle_ratio rising:
        if any_group_comm_slow:  -> 通信层 (查该 group)
        else:                    -> 调度/layout 转换
    elif compute rising:
        -> batch/seq/recompute 变化
    else:
        -> data wait / checkpoint 屏障
```

#### 5. 图文说明

| 层 | 主要指标 | 回答的问题 | 基数策略 |
|---|---|---|---|
| 作业 | goodput、重启数 | 是否有效推进 | 低 |
| step | compute/comm/idle | 慢在算还是等 | 中 |
| 通信 | 每原语耗时/体积 | 哪条通信拖 | 按组聚合 |
| kernel | trace 空隙 | 为何 idle | 按需采样 |

```text
作业 ──> step 分解 ──> 通信/轴 ──> kernel trace
 [goodput]  [c/c/idle]   [per-group]   [nsys]
```

#### 6. 拓展知识

- 与 T-E009 交叉：可观测性提供 MFU 归因的原始数据，口径统一才有可比性。
- 与 T-E008 交叉：故障检测依赖 step/通信层的分布异常。
- 与 T-E011 交叉：数据门禁计数是作业层指标的一部分。

#### 7. 常见追问及回答要点

1. **per-rank 指标怎么不爆炸？** ——只上报聚合分位，异常时按 rank 下钻，而不是默认全量（`I`）。
2. **如何区分"通信慢"与"没重叠"？** ——通信慢是原语本身耗时上升，没重叠是原语耗时正常但 idle 上升；两者在分解树里是不同叶子。
3. **telemetry 会拖慢训练吗？** ——采样与聚合控制得当则开销小；全量 per-step 上报才是负担。

#### 8. 易错点与反例

- 反例：只上报全局平均 step time，掩盖了单 rank 的长时间等待。
- 反例：把 idle 一律归因为"通信慢"，未区分气泡、未重叠与 kernel 抖动。
- 易错：per-step 全量指标导致观测本身成为瓶颈。

#### 9. 实践与验证

`[待验证]` 人为制造三种退化（加一个未重叠 allreduce、制造 PP 气泡、注入慢节点），验证归因流程能在分钟内指出正确层级；oracle：每种退化的主导项落在预期的分解树叶上，聚合分位能捕获单 rank 异常。归因耗时需实测，本仓库未执行。

#### 10. 评分标准

- 不合格：只有 loss 日志。
- 合格：能给出作业/step/通信/内核的分层。
- 良好：能定义 step time 分解并说明基数控制。
- 优秀/专家：能给出固定归因流程，把气泡/未重叠/kernel 抖动区分到叶，并处理 CUDA Graph 场景。

**证据与来源：** `insights/05-megatron-architecture.md` 14.6（timer、log-interval、profiler，`C`+`I`）、13.3（CUDA Graph 捕获，`I`）；`megatron-lm/01-modules/M03-training-runtime/implementation.md`（telemetry 注入、train loop 处理 profiling，`C`）。归因效率标 `U / 待验证`。

---

### T-E013. 从单体训练脚本走向训练平台，哪些抽象值得做，哪些是过度设计？如何平滑迁移？

- 难度：专家级
- 知识域：T12 数据管线与交付
- 考察能力：抽象收益与复杂度成本的判断，兼容策略设计
- 题型：开放设计
- 面试频率：中频（架构/平台岗）
- 前置知识：配置单一事实源、ModuleSpec、ProcessGroupCollection vs global parallel_state、错误边界
- 关联题目：T-E010（异构并行）、T-E012（可观测）、T-E011（数据门禁）
- 事实状态：配置驱动、spec 与实现分离、显式组与全局兼容层并存有源码依据（`C`）；平台收益为 `I`/`U`

#### 1. 背景知识

Megatron-Core 的第三代变化（`C`，insights 2.3）：配置驱动（`TransformerConfig` 作为单一事实源）、并行状态显式化（`parallel_state`/`ProcessGroupCollection` 保存正交 process group，`RankGenerator` 统一生成）、调度函数化（`get_forward_backward_func` 依 PP/VP 返回不同 schedule）、ModuleSpec 把模块规格与实现分离（gpt_builder 按 spec/TE/MoE/异构选择具体 ModuleSpec）。新旧 API 并存：`ProcessGroupCollection` 是较新显式上下文，`parallel_state` 全局兼容接口仍在（`C`，insights 3.3）。ModuleSpec 的代价是 spec 与实际 class 的绑定藏在 registry、读代码跳转变多（`C`，insights 4.2）。

#### 2. 问题分析

1. **抽象的价值判据**：能否消除重复、能否隔离变化、能否降低协作成本。`TransformerConfig` 与 `ModuleSpec` 都满足；但"过度抽象"往往是把一次性区别强行统一。
2. **兼容层是迁移的桥**：全局 `parallel_state` 与显式 `ProcessGroupCollection` 并存，说明可以在不破坏旧调用方的前提下逐步迁移。这是 strangler 模式。
3. **平台化的核心是契约而非代码量**：配置即契约、checkpoint 即状态契约、错误边界即失败契约。平台的价值是把这些契约固化并可校验。

#### 3. 具体答案

面试中可以这样回答：

> 我会用"消除重复、隔离变化、降低协作成本"三条来筛抽象。`TransformerConfig` 作为单一事实源值得做，因为它把模型拓扑与并行参数收进一份配置，换模型变体变成换配置而不是改类；`ModuleSpec` 也值得，它让同一模型外观可被原生/TE/异构精度实现替换。
>
> 但要警惕过渡抽象：把只出现一次的差异强行塞进 registry，结果 spec 与实际 class 的绑定藏起来，读代码成本上升。判断标准是这类替换是否真的需要多后端并存，而不是为将来可能的替换买单。
>
> 迁移上我会用兼容层做桥：新代码优先用显式 `ProcessGroupCollection`，旧调用方通过全局 `parallel_state` 兼容接口继续工作，逐步把调用点迁到显式上下文，而不是一次性替换。这样每一步都可回滚。
>
> 平台的三个核心契约是配置、状态、失败：配置驱动保证可复现，checkpoint 的 sharded schema 保证状态可迁移，错误边界把失败分类成配置/数值/系统，让自动化能据此处置。

#### 4. 技术洞察

用"变化轴"驱动抽象决策：

```text
抽象 = f(变化频率, 隔离价值, 复杂度成本)
  TransformerConfig: 变化频率高(模型/并行) -> 高隔离 -> 值得
  ModuleSpec:        变化频率中(后端)     -> 高隔离 -> 值得
  一次性分支:         变化频率低           -> 低隔离 -> 不合算

迁移(strangler):
  新调用点 -> 显式 ProcessGroupCollection
  旧调用点 -> 全局 parallel_state 兼容层
  逐步替换, 每步可回滚
```

关键：抽象要沿"变化轴"切，而不是沿"代码相似度"切。

#### 5. 图文说明

| 抽象 | 隔离的变化 | 收益 | 成本 |
|---|---|---|---|
| TransformerConfig | 模型拓扑、并行、精度 | 单一事实源、可复现 | 配置项膨胀 |
| ModuleSpec | 实现后端 | 多后端可换 | 跳转变多、绑定隐藏 |
| 显式 ProcessGroupCollection | 多模块/多网格 | 上下文清晰 | 旧代码需适配 |
| 全局 parallel_state | —（兼容） | 不破坏旧调用 | 隐式状态 |

```text
单体脚本 ──配置驱动──> 参数化 ──spec+组显式化──> 可组合库 ──契约固化──> 平台
                每阶段保留兼容层, 可回滚
```

#### 6. 拓展知识

- 与 T-E010 交叉：异构并行的逐层配置正需要 spec 与显式组这类抽象支撑。
- 与 T-E012 交叉：平台把可观测性作为默认能力注入，而非每个作业自建。
- 与 T-E015 交叉：兼容层是版本迁移的基础设施。

#### 7. 常见追问及回答要点

1. **过多 spec 是不是过度设计？** ——当替换需求真实存在时值得，为假想需求加 registry 则不值（`I`）。
2. **为什么保留全局 parallel_state？** ——让旧调用方渐进迁移，避免一次性重写（`C`）。
3. **平台最早的收益点是什么？** ——配置即契约带来的可复现与 checkpoint 状态契约的可迁移（`I`）。

#### 8. 易错点与反例

- 反例：为"将来可能换后端"提前抽象，导致所有代码走 registry 跳转，实际只有一个实现。
- 反例：一次性删掉全局兼容接口，破坏大量旧作业。
- 易错：把平台等同于堆服务，忽略配置/状态/失败三个契约才是核心。

#### 9. 实践与验证

`[待验证]` 选一个真实训练作业，把配置抽成单一事实源、把并行组改为显式传递、保留全局兼容层，逐步替换调用点；oracle：每步替换后作业可运行、结果与替换前在容差内一致、回滚可执行。平台收益与迁移成本需在真实工程中度量，本仓库未执行。

#### 10. 评分标准

- 不合格：把平台化理解为"加调度加界面"。
- 合格：能识别配置与 spec 两类抽象及其收益。
- 良好：能用变化轴判据区分值得与过度设计、说明兼容层迁移。
- 优秀/专家：能提炼配置/状态/失败三契约，并给出可回滚的 strangler 迁移路径。

**证据与来源：** `insights/05-megatron-architecture.md` 2.3/3.3/4.2（配置驱动、组显式化与兼容层、spec 代价，`C`）；`megatron-lm/01-modules/module-registry.md`（模块边界，`C`）；`M01-core-models/implementation.md`（各 spec 分支，`C`）。平台收益标 `I`/`U`。

---

### T-E014. 引入一项新并行技术（CP/EP/GTP）前，风险评审要过哪些门？如何设计可回滚的采纳流程？

- 难度：专家级
- 知识域：T02 分布式初始化与通信组
- 考察能力：把"能不能上"拆成合法性、正确性、性能、可运维四类门
- 题型：开放设计 + 风险评审
- 面试频率：中高频
- 前置知识：并行轴不变量、梯度归约域复合、CUDA Graph、overlap 与数值
- 关联题目：T-E005（CP）、T-E003（EP）、T-E010（异构）、T-E001（不变量）
- 事实状态：拓扑不变量与互斥规则有源码依据（`C`）；各技术实测风险为 `U / 待验证`

#### 1. 背景知识

M02 关键不变量（`C`）：world size 必须被模型并行规模整除；VP 只在 PP>1 合法；GTP rematerialization 与多个 distributed-optimizer instance 当前由初始化逻辑禁止组合；decoder 与 expert rank generator 产生的 PP group 必须一致；schedule 的 model/data iterator 数量必须与调度类型匹配。CP 不切权重、梯度归约复用 DP 域（`C`）。EP+TP 需 SP（`C`）。overlap 会改变梯度计算顺序，对数值有扰动（`I`，insights 13.6）。动态 MoE shape 会让 CUDA Graph 捕获失效（`C`）。

#### 2. 问题分析

1. **四类门缺一不可**：合法性（组合是否被初始化逻辑允许）、正确性（数值是否等价于基线）、性能（是否真的更快、goodput 是否升）、可运维（可观测、可降级、可回滚）。
2. **新技术最危险的是"隐式正确性假设"**：CP 的梯度归约域、EP 的 dispatch layout、GTP 与 optimizer instance 的互斥，都是破坏后才显形的不变量。
3. **采纳流程的分级**：小规模对照 → 逐轴开启 → 灰度 → 全量，每级有验收指标与回滚点。

#### 3. 具体答案

面试中可以这样回答：

> 我用四道门来评审。第一道合法性：查初始化逻辑里这套组合是否被允许。比如 GTP rematerialization 与多个 distributed-optimizer instance 目前是互斥的，VP 要求 PP>1，EP 与 TP 组合必须开 SP。这些在启动期就能判定，不该带到运行期。
>
> 第二道正确性：与不加新技术的基线做对照。CP 不切权重，backward 要在复制份上额外归约，归约域是 CP∩DP；这层如果漏了，loss 会偏离单卡基线。我会用固定 seed 的对照训练，比较 loss 曲线与 grad norm，而不是逐位相等。
>
> 第三道性能：开新轴后 step time 与 goodput 是否真降。CP 未与计算重叠时通信占主，可能负收益；EP 的 A2A 跨节点会更慢。
>
> 第四道可运维：新轴要可观测（能看该组的通信与负载）、可降级（能退回原拓扑）、可回滚（checkpoint 能 reshard 回旧拓扑）。overlap 会改变梯度计算顺序，可能扰动数值，必须能一键关闭。
>
> 采纳流程是：小规模对照训练 → 逐轴开启并留 diff → 灰度 → 全量，每一步都有验收指标与回滚点。

#### 4. 技术洞察

风险评审矩阵加门禁谓词：

```text
Gate1 合法性 (init 期):
  allowed(tp, pp, cp, ep, gtp) == true
  vp>1 ==> pp>1
  not (gtp_remat and multi_distopt)
  ep>1 and tp>1 ==> sp_enabled

Gate2 正确性 (对照):
  |loss_new(t) - loss_base(t)| <= eps_curve   # 曲线差, 非逐位
  grad_norm_new(step0) ≈ grad_norm_base(step0)

Gate3 性能:
  step_time_new <= step_time_base
  goodput_new >= goodput_base

Gate4 可运维:
  observable(group) and degradable(to_prev) and rollbackable(reshape_ckpt)

risk = likelihood * impact * (1 - detectability)
采纳: 小规模 -> 逐轴 -> 灰度 -> 全量
```

关键：`detectability` 低的风险最危险——CP 梯度归约域、逐层 axis order 都属于"错了也不报错"一类。

#### 5. 图文说明

| 门 | 检查对象 | 失败信号 | 回滚动作 |
|---|---|---|---|
| 合法性 | 组合规则 | 启动即报错 | 改配置 |
| 正确性 | loss/grad 对照 | 曲线偏离 | 关该轴 |
| 性能 | step/goodput | 变慢 | 关该轴/overlap |
| 可运维 | 观测/降级 | 不可逆 | 换旧拓扑 |

```text
  新并行技术
      |
 [门1 合法] -> [门2 正确] -> [门3 性能] -> [门4 运维]
      |             |             |             |
    改配置        关轴          关轴/overlap    退旧拓扑
      └──────────── 每级可回滚 ────────────────┘
```

#### 6. 拓展知识

- 与 T-E005 交叉：CP 的梯度归约域复合是 Gate2 最容易被漏的项。
- 与 T-E003 交叉：EP 的 dispatch layout 一致性属于 Gate2。
- 与 T-E010 交叉：逐层并行引入 axis order 风险，属高影响低可检测。
- 与 T-E015 交叉：回滚依赖 checkpoint reshard 与版本兼容。

#### 7. 常见追问及回答要点

1. **为什么合法性要在启动期判定？** ——组合非法是静态事实，早失败远好于运行期 hang 或静默错误（`C`）。
2. **正确性为什么要用曲线而非逐位？** ——并行改变归约顺序，逐位一致不成立（`I`）。
3. **overlap 为什么算风险？** ——它改变梯度计算顺序，fp16 下可能放大数值扰动，必须可关闭（`I`）。

#### 8. 易错点与反例

- 反例：引入 CP 时只测吞吐不测 loss 对照，梯度归约域漏项导致收敛变差却归因于数据。
- 反例：把 GTP 与 multiple distributed-optimizer 一起开，初始化互斥规则触发，浪费一次长时启动。
- 易错：认为"能跑起来"就是通过了 Gate2，忽略了静默的数值偏离。

#### 9. 实践与验证

`[待验证]` 对 CP/EP/GTP 各做一次二对照（加轴 vs 不加），记录合法性启动结果、loss 曲线 diff、step 时间与 goodput、以及回滚到旧拓扑的 reshard 是否成功；oracle：Gate1 静态判定、Gate2 曲线容差内、Gate3 不劣化、Gate4 可回滚。实测数值与性能需目标硬件，本仓库未执行。

#### 10. 评分标准

- 不合格：只说"新并行能加就加"。
- 合格：能列出四类门与对应检查。
- 良好：能把不变量（VP/SP/GTP 互斥、CP 归约域）落到 Gate1/Gate2。
- 优秀/专家：能用风险矩阵（低可检测性最危险）与分级可回滚流程组织评审。

**证据与来源：** `megatron-lm/01-modules/M02-parallelism/README.md`（关键不变量与互斥，`C`）、`M02-parallelism/design.md`（CP 梯度归约，`C`）；`insights/05-megatron-architecture.md` 7.6/13.6/14.4/15.2（SP 约束、overlap 数值扰动、失效模式，`C`+`I`）。实测风险标 `U / 待验证`。

---

### T-E015. 训练系统的版本兼容与迁移策略如何设计？升级时哪些状态和契约最容易被破坏？

- 难度：专家级
- 知识域：T08 优化器与梯度更新
- 考察能力：识别跨版本契约，设计可验证的迁移路径
- 题型：开放设计 + 迁移方案
- 面试频率：中频
- 前置知识：sharded state dict/metadata、optimizer shard 布局、API 弃用与 fallback、HF↔Megatron 转换
- 关联题目：T-E006（checkpoint 一致）、T-E007（训练-推理一致）、T-E013（兼容层）
- 事实状态：checkpoint 结构校验、reshard、static→dynamic 弃用与回退有源码依据（`C`）；迁移实测需环境（`U / 待验证`）

#### 1. 背景知识

`check_checkpoint_args` 会拒绝或告警关键结构参数不匹配（`C`，M05 README）。distributed optimizer 以参数 range 与 DP shard 管理 optimizer state，保存/恢复需与 model sharding metadata 对齐（`C`，M05 implementation）。`GPTModel.sharded_state_dict` 是模型到 checkpoint 层的适配边界（`C`）。distributed checkpoint 支持并行度变化时的 reshard（`C`，insights 14.1）。M06 的 `StaticInferenceEngine` 被明确标记为未来弃用，非 legacy 路径先尝试 dynamic、失败回退 legacy（`C`）。

#### 2. 问题分析

1. **版本兼容 = 契约兼容**：模型结构、并行配置、sharded schema、optimizer shard 布局、API 语义，任何一项变化都可能让旧 checkpoint 或旧脚本失效。
2. **最易破坏的是"隐式契约"**：optimizer shard 与 model sharding metadata 的对齐关系、参数顺序、命名映射，这些没有显式版本号，升级时容易错位。
3. **迁移要可验证**：加载后必须能证明参数数值等价、shard 覆盖完整、恢复后的训练行为与升级前一致，而不是"load 没报错"。

#### 3. 具体答案

面试中可以这样回答：

> 我把兼容性拆成几条契约来看：模型结构契约、并行拓扑契约、sharded state schema 契约、optimizer shard 布局契约、以及 API 语义契约。升级时任意一条变化都可能破坏旧 checkpoint 或旧脚本。
>
> 最容易被忽视的是隐式契约——optimizer 的 shard 按参数 range 划分，必须和 model 的 sharding metadata 对齐；参数顺序或命名映射一变，"load 成功但数值错位"就可能发生。`check_checkpoint_args` 会拦结构参数不匹配，但拦住的是显式项，隐式对齐要自己校验。
>
> 迁移路径我分三步。第一步格式迁移：用官方 reshard/转换路径把旧 checkpoint 映射到新 schema，两侧 shard 覆盖必须完整可核对。第二步语义迁移：处理 API 弃用，例如 static 引擎被标记弃用、会尝试转 dynamic 并回退 legacy，迁移脚本必须显式走目标路径而不是依赖回退。第三步行为校验：加载后做参数与中间量的对照，确认与升级前等价，再进入正式训练。
>
> 我会保留兼容层与旧路径一段时间，用 canary 小作业先跑通，再全量，且保留回滚旧版本的能力。

#### 4. 技术洞察

兼容性矩阵与迁移不变量：

```text
契约维度:
  model_structure | parallel_topo | sharded_schema | opt_shard_layout | api_semantics

升级 -> 迁移:
  reshard(old_ckpt) -> new_schema
  校验不变量:
    coverage: ∀ param -> 恰好一个 shard 覆盖 (无遗漏/无重叠)
    alignment: opt_shard_range 与 model_sharding_metadata 对齐
    equivalence: load 后参数/中间量与升级前在容差内一致
    semantics: 新 API 路径 == 目标路径 (不依赖隐式 fallback)

回滚: 保留旧版本与旧 checkpoint 副本, canary -> 全量
```

关键：`coverage` 与 `alignment` 是分布式 checkpoint 特有的不变量，单机迁移里不存在，却正是升级事故的高发点。

#### 5. 图文说明

| 契约 | 变化来源 | 破坏表现 | 校验手段 |
|---|---|---|---|
| model_structure | 改层/维度 | 参数 shape 不符 | `check_checkpoint_args` |
| parallel_topo | 改 TP/PP/EP | shard 对不上 | reshard |
| sharded_schema | 升级框架 | load 失败 | schema 覆盖检查 |
| opt_shard_layout | optimizer 变更 | 数值错位（静默） | range 对齐校验 |
| api_semantics | 弃用/新增 | 走错 backend | 显式路径确认 |

```text
旧 ckpt ──reshard──> 新 schema ──load──> 参数/中间量对照 ──> canary ──> 全量
   |                                   |                                  |
 保留副本                          覆盖+对齐+等价校验                可回滚旧版本
```

#### 6. 拓展知识

- 与 T-E006 交叉：版本迁移与容错恢复共用"状态切面 + 加载校验"的机制。
- 与 T-E007 交叉：checkpoint 格式与并行布局同时决定训练-推理转换是否可行。
- 与 T-E013 交叉：兼容层是版本迁移的基础设施，strangler 模式同样适用。

#### 7. 常见追问及回答要点

1. **load 成功就说明兼容吗？** ——不一定。参数顺序或 shard 对齐错误可能静默通过，需做数值对照（`I`）。
2. **`check_checkpoint_args` 管什么？** ——管关键结构参数不匹配，会拒绝或告警；但它覆盖显式项，隐式对齐要另测（`C`）。
3. **为什么要注意 API 弃用？** ——弃用路径可能静默回退到旧 backend，行为与预期不同，迁移要显式走目标路径（`C`）。

#### 8. 易错点与反例

- 反例：升级后 optimizer shard 与 model metadata 未对齐，load 成功但训练数值漂移，被当成超参问题。
- 反例：依赖 static 引擎的隐式 fallback，实际跑了 legacy 路径，迁移目标未达成。
- 易错：改并行度后直接 load 未 reshard 的 shard，schema 不匹配。

#### 9. 实践与验证

`[待验证]` 准备一份旧版本 checkpoint，执行 reshard 与 schema 迁移，做参数与中间量对照，再跑 canary 训练若干步；oracle：shard 覆盖完整无重叠、opt shard range 与 model metadata 对齐、加载后数值与升级前在容差内一致、canary 可回滚到旧版本。真实版本差异与 reshard 行为需目标环境，本仓库未执行。

#### 10. 评分标准

- 不合格：只说"用官方转换脚本"。
- 合格：能列出模型/拓扑/schema/optimizer/API 五条契约。
- 良好：能指出 optimizer shard 与 model metadata 的隐式对齐是最易静默破坏的点。
- 优秀/专家：能给出覆盖/对齐/等价/语义四类迁移不变量与 canary 回滚流程。

**证据与来源：** `megatron-lm/01-modules/M05-optimizer-checkpointing/README.md`（`check_checkpoint_args`、结构兼容，`C`）、`M05-optimizer-checkpointing/implementation.md`（distributed optimizer shard 与 model metadata 对齐、sharded_state_dict 边界，`C`）；`insights/05-megatron-architecture.md` 14.1（reshard 与 Bridge，`C`）；`megatron-lm/01-modules/M06-inference-and-tools/README.md`（static 引擎弃用与回退，`C`）。迁移实测标 `U / 待验证`。
