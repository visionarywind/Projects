# 训练方向中等级面试题（T-I001–T-I015）

> 证据规则：源码/文档结论区分 `C`（源码明确陈述）/`I`（跨文档推断）；GPU、NCCL、多机、多进程与性能结论未经目标环境运行时标 `U / 待验证`。

### T-I001. 断点续训时，随机状态、数据迭代器位置与优化器状态如何保持一致？

- 难度：中等级
- 知识域：T10 检查点、恢复与容错
- 考察能力：状态集合点识别、恢复顺序、可复现性
- 题型：故障定位/状态机
- 面试频率：高频
- 前置知识：`torch.distributed`、CPU/CUDA 随机种子、数据 sampler、Adam 状态
- 关联题目：T-B001（训练入口）；T-I010（分布式 checkpoint）；T-I013（DP sampler）
- 事实状态：checkpoint 状态类别 `[已确认]`（C）；恢复后逐位一致性与多卡行为 `[待验证]`（U）

#### 1. 背景知识

Resume 不等于“把权重读回来”。Megatron 的 training checkpoint 同时承载模型、optimizer、scheduler、RNG、tokenizer、数据加载状态和 tracker（C，`training/checkpointing.py:176-230,611+,1567+,2469+`）。少恢复任何一类都会让轨迹在恢复点之后偏离：optimizer 状态丢失意味着 Adam 的一阶/二阶矩从零重启；RNG 丢失会改变 dropout mask 与数据 shuffle；数据位置丢失会重复或跳过样本。

Megatron 的随机性分成三股相对独立的流（C/I，`insights §4.6`）：权重初始化用基础 seed 加 rank 派生 offset，保证同一逻辑参数在不同物理 rank 上初值一致；data loader 用独立 seed 控制 shuffle；inference sampling 由采样层自己管。断点续训主要关心前两股，再加上驱动 dropout 的 per-device CUDA generator。

#### 2. 问题分析

把 resume 看成“先找到一致的状态集合点，再整体回放”：

1. 集合点位置由 `train_step` 的顺序决定：清 grad buffer → `optimizer.zero_grad` → schedule → finalize grads → `optimizer.step` → 聚合 update 状态 → 推进 scheduler（C，`M03/implementation.md`）。checkpoint 只能写在这个“整步终了”的边界，写到 step 中途会让参数分片与 optimizer 分片的时间戳不一致（I，`insights §12.5`）。
2. RNG 覆盖面要包含 CPU generator、每个 CUDA device 的 generator、以及数据 sampler 的 shuffle seed/epoch；只存 CPU 一份无法复现 GPU dropout。
3. 样本消费量由 `load_checkpoint` 返回的 iteration 与 consumed samples 给出（C，`M05/execution-flows.md`），训练循环据此重建 sampler 的 epoch 与偏移，而不是从 0 重新 shuffle。

一个直接的量级推导：global batch 为 G、序列长 seq，则每步消耗 G×seq tokens。若恢复时只对齐 iteration、不对齐 sample offset，那么“第 k 步”取到的 batch 与中断前第 k 步不同，loss 曲线会在恢复点出现台阶——这类台阶常被误判成学习率或数据质量问题。

#### 3. 具体答案

面试中可以这样回答：

> 我会先问“恢复哪些状态”，再问“在哪个边界恢复”。
> 状态上，一次完整 checkpoint 至少六类：model 参数分片、optimizer state（含 fp32 master 与 Adam 两矩）、LR scheduler、RNG（CPU + 各 CUDA device + data seed）、data iterator 位置、tracker 元数据。前四类决定“数学从哪继续”，后两类决定“数据从哪继续”。
> 边界上，保存要落在 `optimizer.step()` 成功之后、`train_step` 收尾处：此时梯度已 reduce 完、参数与 optimizer 分片一致；如果这一步被 loss scaling 判成溢出而 skip，scheduler 不推进，checkpoint 也应忠实记录这个 skip 状态。`load_checkpoint` 返回 iteration 与 consumed samples，训练循环拿它重建 sampler 的 epoch 和偏移。
> 最容易漏的是 RNG 与数据偏移。只恢复权重会让 dropout mask 与 batch 顺序变样，loss 在恢复点错位。验收做法是把“连续跑 N 步”与“跑 N/2 步后存盘、恢复再跑 N/2 步”的 loss 逐点叠图。

#### 4. 技术洞察

恢复的状态机与不变量（I，基于 `M05/execution-flows.md` 与 `insights §12.5`）：

```text
save:  step 完成 → 收集 model/opt/sched/RNG/data/tracker → dist_checkpointing.save
load:  解析 iteration/path → check_checkpoint_args → 载入 model+opt+sched/RNG/data
        → 返回 (iteration, consumed_samples)
train: consumed_samples → 重建 sampler 偏移；iteration → lr schedule 与 checkpoint 间隔
```

不变量：checkpoint 里的 optimizer shard 布局必须与“恢复所用并行拓扑”的 shard 布局匹配；不匹配时要走 reshard 路径，而不是强行 load（C，dist checkpoint 的 reshard 能力）。

#### 5. 图文说明

| 状态类别 | 丢失后果 | 恢复来源 |
|---|---|---|
| model params | 权重错位 | sharded state dict |
| optimizer state | Adam 矩归零、恢复点扰动 | optimizer shard |
| LR scheduler | 学习率阶跃 | scheduler state |
| RNG | dropout / shuffle 改变 | 逐 device 的 rng state |
| data iterator | 重复或跳过样本 | consumed samples |
| tracker | 命名与元数据错位 | tracker 文件 |

```text
[连续 0..N]  ==对比==  [0..N/2 存盘] + [load → N/2..N]
       若两条 loss 轨迹在恢复点后重合 → 状态集合点正确
```

#### 6. 拓展知识

- 与 T-I010 交叉：支持 reshard 的分布式 checkpoint 让“拓扑不匹配”不再强制从零重训。
- 与 T-I013 交叉：sampler 的 DP rank/size 决定每个进程看到的 batch，恢复时 DP 布局需要一致或可映射。
- 与 T-I015 交叉：若 checkpoint 恰好落在梯度累积中途，还需记录累积相位，否则 global batch 等价性被破坏。

#### 7. 常见追问及回答要点

1. **只恢复 model 不恢复 optimizer 会怎样？** ——Adam 两矩归零，等效于动量重新积累，恢复点附近 loss 抖动；短训可能看不出，长训不可接受。
2. **为什么 checkpoint 要写在整个 step 之后？** ——否则参数分片、optimizer 分片、scheduler 三者状态不同步，load 后出现数值漂移（I）。
3. **为什么 RNG 要按 device 逐个存？** ——dropout 在 GPU 上由 per-device generator 驱动，只存 CPU RNG 无法复现网络体内的 mask。

#### 8. 易错点与反例

- 反例：只在 iteration 是 save-interval 倍数时存盘，忽略 `optimizer.step()` 被 skip 的步，导致 scheduler 状态与 iteration 计数脱节。
- 反例：恢复时把 consumed_samples 直接设成 `iteration × global_batch`，忽略 shuffle 的 epoch 边界与多数据集 blend 权重，样本顺序对不上。
- 易错：把“同一配置恢复”当成“逐位一致”。重叠、归约顺序和非确定性 kernel 都会改变浮点累加顺序，验收应看 loss 轨迹与关键层数值的近似一致（I）。

#### 9. 实践与验证

`[待验证]` 双跑对照：固定源码版本与 seed，设 `--save-interval` 在中途存盘；A 组连续跑 200 步，B 组前 100 步后杀进程、`load_checkpoint` 续 100 步。oracle：恢复点之后两条 loss 轨迹一致（允许浮点误差），consumed samples 单调且无跳变。RNG 与 dropout 的复现必须在含 GPU 的目标环境执行，当前未运行。若观测到台阶，先查 RNG，再查 sampler offset。

#### 10. 评分标准

- 不合格：只答“存模型权重”，不区分 optimizer / RNG / data 状态。
- 合格：能列出六类状态，并指出 checkpoint 边界在 `optimizer.step` 之后。
- 良好：能解释 consumed_samples 如何重建 sampler、为什么 resume 不保证逐位一致。
- 优秀/专家：能结合 shard 元数据、skip-update 语义与梯度累积相位，设计可验证的 resume 一致性对照实验。

**证据与来源：** `megatron-lm/01-modules/M05-optimizer-checkpointing/{README,implementation,execution-flows}.md`（checkpoint 两层、load 返回 iteration/consumed samples，`C`）；`megatron-lm/01-modules/M03-training-runtime/implementation.md`（train_step 顺序与 skip，`C`）；`insights/05-megatron-architecture.md` §4.6/§12.5/§14.5（随机流与状态集合点，`C/I`）。版本锚点 `3703d4e33`；运行类结论标 `U / 待验证`。

### T-I002. TP 下线性层的行切与列切如何决定 layout 与通信？GQA、SP 会带来哪些陷阱？

- 难度：中等级
- 知识域：T04 张量并行
- 考察能力：切分方向推导、通信量估算、整除陷阱
- 题型：原理推导/配置分析
- 面试频率：高频
- 前置知识：GEMM 分块、`all_reduce`、f/g 算子、GQA 结构
- 关联题目：T-B001（并行拓扑）；T-I005（通信原语）；T-I006（SP/CP）
- 事实状态：切分方向与 f/g `[已确认]`（C）；GQA 整除分支 `[已确认]`（C），具体 kernel 行为 `[待验证]`（U）

#### 1. 背景知识

TP 的切分方向不是任意选的，它由“非线性算子能否逐分片施加”决定（C，论文 arXiv:1909.08053）。考虑 `Y = GeLU(X·A)`：若把 A 按行切，则 `GeLU(X1A1 + X2A2) ≠ GeLU(X1A1) + GeLU(X2A2)`，必须在 GeLU 之前 all-reduce，凭空多一个同步点；若把 A 按列切，`A=[A1,A2]`，每个分片独立过 GeLU，无中间同步。

因此 Megatron 的 MLP 用“第一个 GEMM 列切 + 第二个 GEMM 行切”：列切输出正好是第二个 GEMM 的输入，无需重排；第二个 GEMM 的输出是部分和，做一次 all-reduce 得到完整 hidden。Attention 同理：QKV 投影按列切（head 维），out projection 按行切，末尾一次 all-reduce。

#### 2. 问题分析

先推导 layout，再推通信量，最后看整除陷阱：

1. hidden 为 h、TP 为 t、激活为 s×b×h。列切权重 `W1:[h,4h]` 每 rank 持 `[h,4h/t]`；行切权重 `W2:[4h,h]` 每 rank 持 `[4h/t,h]`。单卡 GEMM 计算量正比于 `h·4h/t`，t 越大单卡矩阵越小、计算密度越低。
2. 通信量：每次 all-reduce 在 ring 算法下搬运 `2(t-1)/t·s·b·h` 个元素（reduction + broadcast 两段）。每层共 4 次 all-reduce（forward 两次 g、backward 两次 f），与 t 无关（C，论文）——这是 TP 的旗舰结论，“TP 越大通信次数越多”是误解，变多的是单次延迟占比与 GEMM 碎片化。
3. 整除陷阱：GQA 的 KV head 数（query groups）必须能被 t 整除，否则无法让每个 rank 持有整数个 KV head；Megatron 需要复制 KV 投影或 padding（C/I，`insights §6.10`）。DeepSeek-V3、LLaMA-3 等在 KV head 数与 TP 之间做了整除设计。

#### 3. 具体答案

面试中可以这样回答：

> 我会从“非线性能否逐分片算”这条主线上讲。因为 GeLU、softmax 不能跨分片线性分解，所以第一个 GEMM 必须列切、第二个行切，这样非线性夹在两次 GEMM 之间且本地可算。
> 落到布局：MLP 每个 rank 存 `[h,4h/t]` 和 `[4h/t,h]` 两块权重，输出是部分和，需要一次 all-reduce；attention 的 QKV 列切让每个 head 完整落在一个 rank 上，softmax 只在序列维做，out projection 行切后再 all-reduce。
> 通信上，每层固定 4 次 all-reduce，次数与 t 无关；体积随 t 线性但 ring 带宽约 `2(t-1)/t` 倍数据，接近常数。真正的代价是 t 大后单卡 GEMM 变小、计算密度下降。
> 最容易踩的坑是 GQA：KV head 数不能被 t 整除时，要么 padding 要么复制 KV，通信与显存都会变化。配置时我会先确认 `num_query_groups % t == 0`，再决定 TP 度数。

#### 4. 技术洞察

切分契约与通信（C，论文 + `M02/README.md`）：

```text
MLP:      X ── W1[h,4h/t] ── GeLU(local) ── W2[4h/t,h] ── ⊕
                                                          └─ allreduce(g) ─→ out
Attention: X ── QKV[h, (3h)/t] ── local attention(head 完整) ── outproj[h/t,h] ── ⊕
                                                          └─ allreduce(g) ─→ out
每层: 2×g(forward) + 2×f(backward) = 4 次 allreduce，与 t 无关
ring volume/次 ≈ 2(t-1)/t · s·b·h
```

不变量：列切参数各 rank 不同、梯度无需跨 rank 归约；行切参数在输入侧经过 f 的 all-reduce，梯度沿 f̄ 传播（I，`insights §6.12`）。写错 f/g 的位置，TP=2 的首步 loss 就会偏离 TP=1 基线。

#### 5. 图文说明

| 子层 | 切分方向 | 中间通信 | 末尾通信 |
|---|---|---|---|
| QKV projection | 列切（head） | 无（softmax 本地） | — |
| out projection | 行切 | — | all-reduce |
| MLP fc1 | 列切 | 无（GeLU 本地） | — |
| MLP fc2 | 行切 | — | all-reduce |
| LayerNorm/Dropout | 复制或 SP | 无 / g,ḡ | — |

```text
TP=1:  X ──────────► [完整 GEMM] ──────────► out
TP=2:  X ─► [W1_0] ─► GeLU ─► [W2_0] ─┐
       X ─► [W1_1] ─► GeLU ─► [W2_1] ─┴─ allreduce ─► out
```

#### 6. 拓展知识

- 与 T-I006 交叉：SP 把 LayerNorm/Dropout 的复制区也按序列维切开，用 g/ḡ 在“序列切分”与“hidden 切分”两种 layout 间转换。
- 与 T-I007 交叉：EP 与 TP 组合时必须开 SP，因为 MoE 的 token dispatch 需要统一 layout（C，官方 MoE 文档）。
- GQA/MLA 的 TP：MLA 的 latent 投影与解压 GEMM 也要 TP 化，且更难与 head 维对齐（I，`insights §6.10`）。

#### 7. 常见追问及回答要点

1. **为什么不能两段都列切？** ——第二段列切后输出仍是分片的部分结果，无法直接拼成完整 hidden，且非线性位置会错；行切才是“部分和 + 末尾归约”的正确形态。
2. **TP 的通信次数会随 t 增长吗？** ——不会，每层固定 4 次；t 影响的是单次体积与 GEMM 尺寸、以及未 overlap 时的延迟占比。
3. **GQA 的 KV head 不能被 t 整除怎么办？** ——复制 KV 投影或 padding head；代价是冗余计算与显存，配置上通常预先让 head 数与 TP 对齐。

#### 8. 易错点与反例

- 反例：把 f/g 位置放反，forward 的 all-reduce 放到了 backward，梯度只聚合了一部分输入沿，TP=2 训练首步 loss 就偏离基线。
- 反例：看到 TP 报文变大就以“t 越大越慢”下结论；真正变慢的是 GEMM 碎片化与未重叠的延迟，通信次数是常数。
- 易错：忽略 query groups 整除约束，直接设 TP=16 跑到 QKV projection 才报 shape 错误，或在复制 KV 后误以为显存没变。

#### 9. 实践与验证

`[待验证]` TP 正确性对照：固定版本与 seed，跑 TP=1 与 TP=2 各若干步，比较 loss 曲线与每层权重/梯度的近似 diff。oracle：TP=2 与 TP=1 数学等价、浮点近似一致；若首步即偏离，先查 f/g 位置与整除约束。多卡集合通信需在含 NCCL 的目标环境执行，当前未运行。

#### 10. 评分标准

- 不合格：只知道“TP 切权重”，说不出切分方向与非线性关系。
- 合格：能给出列切/行切顺序、每层一次 all-reduce 的位置。
- 良好：能推导每层 4 次 all-reduce 与 `2(t-1)/t` 体积，指出 GQA 整除约束。
- 优秀/专家：能结合 f/g 的 backward 方向、SP layout 转换、MLA 的切分难点，给出可验证的 TP 对照方案。

**证据与来源：** `insights/05-megatron-architecture.md` §6.1–6.12（切分方向、f/g、4 次 all-reduce、GQA/MLA，`C/I`，论文 arXiv:1909.08053）；`megatron-lm/01-modules/M02-parallelism/README.md`（TP 组与并行轴，`C`）；`megatron-lm/01-modules/M01-core-models/README.md`（layer spec 与 `parallel_output`，`C`）。版本锚点 `3703d4e33`；kernel 行为标 `U / 待验证`。

### T-I003. PP 的 1F1B 时间线与 bubble：给定 PP 与 MB，如何推导效率与激活驻留？

- 难度：中等级
- 知识域：T05 流水线并行与调度
- 考察能力：调度建模、气泡推导、激活驻留估算
- 题型：推导/计算
- 面试频率：高频
- 前置知识：microbatch、warmup/steady/cooldown、P2P 通信
- 关联题目：T-B001（schedule 选择）；T-I004（显存）；T-I011（激活估算）
- 事实状态：1F1B 机制与公式 `[已确认]`（C）；实测气泡与并行效率 `[待验证]`（U）

#### 1. 背景知识

naive pipeline 让每个 microbatch 先走完整 forward 再走完整 backward，最坏并行效率趋近 `1/PP`。1F1B（one-forward-one-backward）在稳定阶段让每个 rank 交替执行一个 forward 和一个 backward，关键收益是“在飞 microbatch 数被限制在 stage 数 PP 上，而不是总 microbatch 数 MB”（C，论文 arXiv:2201.11990）。调度由 `get_forward_backward_func` 按 PP/VP 选择：PP=1 走无流水、PP>1 无 VP 走非交错、PP>1 有 VP 走交错（C，`schedules.py:53-168`）。

#### 2. 问题分析

时间线先切成三段：warmup 只 forward、steady 交替 F/B、cooldown 只 backward（C，`M02/implementation.md`）。设流水线总时隙数 = `MB + PP - 1`，其中两端各有 `PP-1` 个气泡时隙，于是：

```text
efficiency ≈ 1 - (PP - 1) / (MB + PP - 1)
```

代入两组数：MB=16、PP=4 → 气泡 = 3/19 ≈ 15.8%，效率约 84%；MB=8、PP=8 → 气泡 = 7/15 ≈ 46.7%，效率约 53%。这说明 PP 一旦拉大，就必须同步把 MB 抬上去，典型经验是 `MB = 4×PP` 时约 81%、`8×PP` 时约 90%（C，论文报告值）。

激活驻留：每个 stage 的 rank 需要同时持有在飞 microbatch 的激活。1F1B 下每个 rank 同时最多持有 PP 个 microbatch 的激活（C/I），而不是全部 MB。配置陷阱：`num_microbatches = global_batch / (DP × micro_batch_size)`，若 MB < PP，流水线无法填满甚至调度非法。

#### 3. 具体答案

面试中可以这样回答：

> 1F1B 的时间线是一个长度 `MB + PP - 1` 的时隙轴，两端各 `PP - 1` 个空槽，中间是稳定的 F/B 交替。气泡比例就是 `(PP-1)/(MB+PP-1)`。
> 举个具体数：MB=16、PP=4 时气泡约 15.8%；MB=8、PP=8 时约 46.7%。所以 PP 越大，越需要把 microbatch 数抬起来，经验上 MB 要取到 PP 的 4 到 8 倍，才能把并行效率推过 80%–90%。
> 显存上，1F1B 的价值是“在飞 microbatch 数被 bound 在 stage 数 PP”，而不是总 MB 数。也就是说每张卡最多同时驻留 PP 份 stage 激活，不会随 MB 线性膨胀。
> 落地时我会先算 `MB = global_batch / (DP × micro_batch_size)`，确认 MB ≥ PP；否则要么加 global batch，要么减 PP，或者用 interleaved 拿更细的切分。

#### 4. 技术洞察

调度伪代码（I，基于论文描述与 `M02` 执行流程）：

```text
for step in range(num_microbatches + num_pipeline_stages - 1):
    if should_fwd(step):         # warmup + steady
        fwd(microbatch(step))
    if should_bwd(step):         # steady + cooldown
        bwd(microbatch(step - num_pipeline_stages))
        finalize_grads_if_needed()
```

不变量：`MB = global_batch / (DP × micro_batch_size)`；`MB ≥ PP`；schedule 的 model/data iterator 数量必须与所选调度类型匹配（C，`M02/README.md`）。stage 负载要求每 stage 层数相等，否则慢 stage 拖整个 pipeline（C，`insights §8.7`）。

#### 5. 图文说明

以 PP=4、MB=8 为例（I，教学示意，非实测）：

```text
time -> 0  1  2  3  4  5  6  7  8  9  10 11
rank0:  F0 F1 F2 F3 B0 F4 B1 F5 B2 F6 B3 F7 ...
rank1:  .  F0 F1 F2 F3 B0 F4 B1 F5 B2 F6 ...
rank2:  .  .  F0 F1 F2 F3 B0 F4 B1 F5 ...
rank3:  .  .  .  F0 F1 F2 F3 B0 F4 ...
         └─ 气泡 ─┘       稳定 F/B         └─ 气泡 ─┘
```

| 参数 | 气泡时隙 | 效率 |
|---|---|---|
| MB=16, PP=4 | 3 | ~84% |
| MB=8, PP=8 | 7 | ~53% |
| MB=32, PP=4 | 3 | ~91% |

#### 6. 拓展知识

- 与 T-I004 交叉：PP 减压的是权重/优化器状态，不减激活；激活要靠 microbatch 节流与 recompute。
- 与 T-I011 交叉：PP 下每 rank 驻留“在飞 MB 数 × stage 激活”，进入激活估算公式。
- 与 interleaved/VP 交叉：VP 把单 stage 层数从 `L/PP` 降到 `L/(PP·vp)`，进一步降在飞激活，代价是 P2P 次数与调试复杂度上升（C，`M02/design.md`）。

#### 7. 常见追问及回答要点

1. **1F1B 比 GPipe 省内存的根因？** ——在飞 microbatch 数被限制在 stage 数而非总 MB 数，激活驻留显著下降。
2. **MB 增大到很大，效率会无限接近 1 吗？** ——趋近 1，但 MB 受 global batch 与显存约束，且更大 MB 不减少 steady 阶段的 P2P 需求。
3. **VP 为什么能进一步降激活？** ——每 rank 的虚拟 chunk 更小，单个 chunk 的层数减少，在飞激活按 `L/(PP·vp)` 摊薄；代价是边界增多。

#### 8. 易错点与反例

- 反例：直接把 `--pipeline-model-parallel-size` 从 4 调到 8 而不改 global batch，MB 相对 PP 变小，气泡从 ~16% 涨到近 50%，吞吐反降。
- 反例：以为 PP 能降激活显存。PP 只切权重/梯度/优化器状态，激活由 microbatch 数和 recompute 控制。
- 易错：忽略 stage 层数均衡，某 stage 多一层就成瓶颈，效率公式在“负载不均”时不再成立。

#### 9. 实践与验证

`[待验证]` 用固定配置跑 PP=2/4，记录每步时间与 `--log-interval` 的 MFU，对照公式 `1-(PP-1)/(MB+PP-1)`。oracle：实测气泡比例随 MB/PP 比值单调下降，与公式趋势一致；层数不均时会看到“慢 stage”拖尾。nsys 可观察 P2P 空隙。多卡需在含 NCCL 的目标环境执行，当前未运行。

#### 10. 评分标准

- 不合格：只答“1F1B 交错前向反向”，不会推导气泡。
- 合格：能写出 `1-(PP-1)/(MB+PP-1)` 并代入数值。
- 良好：能解释在飞 MB 被 bound 在 PP、MB 与 global batch/DP 的关系。
- 优秀/专家：能结合 VP 的层数切分、stage 均衡与 overlap，给出从 PP/MB 到激活驻留与吞吐的完整链路。

**证据与来源：** `insights/05-megatron-architecture.md` §8.2–8.9（1F1B、气泡公式、在飞 MB、显存模型，`C/I`，论文 arXiv:2201.11990）；`megatron-lm/01-modules/M02-parallelism/implementation.md`（非交错调度 warmup/steady/cooldown，`C`）；`megatron-lm/01-modules/M02-parallelism/execution-flows.md`（PP 迭代结构，`C`）。版本锚点 `3703d4e33`；实测效率标 `U / 待验证`。

### T-I004. ZeRO 与分布式优化器：状态分片换来什么、付出什么？

- 难度：中等级
- 知识域：T03 数据并行
- 考察能力：显存账推导、通信代价评估、实现取舍
- 题型：设计/权衡
- 面试频率：高频
- 前置知识：DP、Adam 优化器状态、reduce-scatter/all-gather、checkpoint shard
- 关联题目：T-I005（通信语义）；T-I008（fused optimizer）；T-I010（checkpoint）
- 事实状态：distributed optimizer 机制 `[已确认]`（C）；显存倍数与实测收益 `[推断/待验证]`（I/U）

#### 1. 背景知识

标准 DP 每个 rank 持有完整模型副本与完整优化器状态。Adam 在混合精度下要维护 fp32 master、一阶矩、二阶矩，再加梯度缓冲，优化器相关显存可达参数量的 12–16 倍量级（I，`insights §12.2`）。单卡装得下模型未必装得下优化器状态，这正是 ZeRO/分布式优化器存在的理由。

Megatron 的 distributed optimizer 是“DP + 状态分片”的组合：把梯度与优化器状态按 DP rank 切分，用 reduce-scatter 替代 all-reduce，再用 all-gather 把更新后的参数分片广播回各副本（C，`M05/implementation.md`、`insights §5.4/§12.1`）。它与 ZeRO-1 同族（只分片优化器状态），FSDP 则走到参数/梯度/状态全分片（C，`insights §15.6`）。

#### 2. 问题分析

先算显存，再算通信，最后看实现约束：

1. 显存账：参数量 P，DP=N。不分片时每 rank 优化器状态正比于 P；分片后降到约 P/N。这段节省是分布式优化器的核心卖点，也是它被官方并行表记成“Optimizer states: 1/N（with distributed optimizer）”的原因（C）。
2. 通信账：all-reduce 被拆成 reduce-scatter（梯度）+ all-gather（参数）。按 ring 等价性，两者带宽之和与一次 all-reduce 相当，因此分片本身不额外增加带宽，但把通信切成两段，需要 overlap 才能隐藏（I，`insights §15.3`）。
3. 实现约束：M02 不决定 optimizer 是否 sharded，只提供通信域（C，`M02/design.md`）。所以“分布式优化器不是并行策略”这句对错各半——它是 DP 维度内的状态分片方案。GTP rematerialization 与多个 distributed-optimizer instance 当前被初始化逻辑禁止组合（C，`M02/README.md`）。

#### 3. 具体答案

面试中可以这样回答：

> 我会把它当“用通信换显存”的账来算，而不是当作一种并行轴。
> 不分片时，Adam 的 fp32 master、一阶矩、二阶矩加上梯度缓冲，按参数量线性增长；DP=N 分片后，每 rank 只持有约 1/N，这是它解决 OOM 的核心。
> 通信上，它把 DP 的梯度 all-reduce 换成 reduce-scatter + 参数 all-gather。ring all-reduce 本来就等于这两段，所以带宽基本等价，没有凭空多出来；难点在把这两段和计算 overlap，否则会看到两个同步空隙。
> 实现上要注意三件事：它和 checkpoint 的 shard 元数据强绑定，换并行度要 reshard；它不改变模型结构，所以属于 DP 的优化而非新的并行轴；GTP rematerialization 与多优化器实例当前不能和它组合。
> 选型判断：如果瓶颈是优化器状态显存且网络带宽充足，就上分布式优化器；如果带宽是瓶颈、模型本身能放下，就不必为了 1/N 再引入 reduce-scatter+all-gather 的复杂度。

#### 4. 技术洞察

执行时序（I，`insights §12.5` 与 `M05/execution-flows.md`）：

```text
optimizer.step:
  1. scaler 检查（可能 skip）
  2. 准备 fp32 master
  3. 梯度 gather：distributed optimizer 先 reduce-scatter 得到本 rank 的分片
  4. Adam 更新本 rank 负责的参数分片
  5. 可选：all-gather 更新后的参数分片回各 DP 副本
  6. 记录 grad norm
```

不变量：optimizer shard 的布局必须与 model sharding metadata 对齐；checkpoint 保存/恢复时二者都要一致（C，`M05/implementation.md`）。

| 方案 | 分片对象 | 通信 | 典型代价 |
|---|---|---|---|
| 本地优化器 | 无 | 梯度 all-reduce | 优化器状态 ×1 |
| distributed optimizer (ZeRO-1) | 优化器状态 | reduce-scatter + all-gather | 需 overlap |
| FSDP/ZeRO-3 | 参数/梯度/状态 | all-gather + reduce-scatter 每层 | 通信更频繁 |

#### 5. 图文说明

```text
不分片:  grad ── allreduce ──► 每 rank 全量 Adam 状态
分片:    grad ── reduce-scatter ──► 每 rank 1/N 状态 ── Adam ── all-gather ──► 参数分片
         └──────── 与一次 allreduce 带宽等价 ────────┘
```

#### 6. 拓展知识

- 与 T-I005 交叉：为什么 reduce-scatter + all-gather 等价于一次 all-reduce，是理解分片不涨带宽的关键。
- 与 T-I010 交叉：分片优化器状态让 checkpoint 必须以 sharded state dict 保存，并支持 reshard。
- 与 T-I008 交叉：fused optimizer 与该分片路径组合时，kernel 入口与 state 布局都要匹配。

#### 7. 常见追问及回答要点

1. **分布式优化器是不是一种并行策略？** ——对错各半。它占用 DP 维、不改模型结构，属于 DP 内的状态分片，与 FSDP/ZeRO 同族；M02 只提供它依赖的通信域。
2. **它会让通信量翻倍吗？** ——不会凭空翻倍。reduce-scatter + all-gather 与 all-reduce 带宽等价；感受上是“两个同步段”，需要用 overlap 隐藏。
3. **和 checkpoint 的耦合点在哪？** ——优化器分片布局必须与 model shard metadata 一致，换并行度要么重新分片要么 reshard。

#### 8. 易错点与反例

- 反例：为了省显存无脑开分布式优化器，但网络带宽有限且没有 overlap，step 时间反而上升。
- 反例：只改 DP 度数却沿用旧的优化器分片 checkpoint，load 后分片对不上导致数值漂移。
- 易错：把“优化器状态 1/N”误读成“总显存 1/N”——权重与激活并不因此下降，激活仍要靠 SP/recompute 控制。

#### 9. 实践与验证

`[待验证]` 固定模型与 DP 数，对比开关 `--use-distributed-optimizer` 前后的显存峰值与 step 时间。oracle：优化器相关显存随 DP 显著下降，step 时间在 overlap 生效时接近不开启；未 overlap 时会看到 reduce-scatter 与 all-gather 两个空隙。多卡与带宽结论需在目标环境测，当前未运行。

#### 10. 评分标准

- 不合格：把分布式优化器当成 TP/PP 一类的并行轴。
- 合格：能说出“状态按 DP 分片、通信换成 reduce-scatter + all-gather”。
- 良好：能推导带宽等价、指出 overlap 与 checkpoint shard 的耦合。
- 优秀/专家：能对比 ZeRO-1/2/3 与 FSDP 的分片边界，结合模型规模与带宽给出选型判断。

**证据与来源：** `megatron-lm/01-modules/M05-optimizer-checkpointing/{README,implementation}.md`（distributed optimizer、shard 对齐，`C`）；`megatron-lm/01-modules/M02-parallelism/design.md`（M02 不决定 sharding，只提供通信域，`C`）；`insights/05-megatron-architecture.md` §5.4/§12.1/§12.5/§15.3/§15.6（ZeRO 同族、带宽等价，`C/I`）。版本锚点 `3703d4e33`；显存倍数与收益标 `I`，实测标 `U / 待验证`。

### T-I005. all-reduce 与 reduce-scatter 语义有何不同？为什么分布式优化器和 SP 都偏爱 reduce-scatter？

- 难度：中等级
- 知识域：T02 分布式初始化与通信组
- 考察能力：通信原语语义、带宽等价推导、正确选型
- 题型：原理/推导
- 面试频率：中频
- 前置知识：ring all-reduce、NCCL 基础、TP/DP/SP 通信域
- 关联题目：T-I002（TP 通信）；T-I004（分片优化器）；T-I006（SP/CP）
- 事实状态：原语语义与分解 `[已确认]`（C）；实际带宽与算法 `[待验证]`（U）

#### 1. 背景知识

三种集合通信语义要分清：all-reduce 让每个 rank 得到“所有 rank 局部结果之和”；reduce-scatter 求和后把结果按 rank 切成 N 份、每 rank 只留一份；all-gather 把各 rank 的分片拼起来广播给全体。ring 算法下 all-reduce = reduce-scatter + all-gather，两段的总体积与一次 all-reduce 相同（C，`insights §2A.3/§7.3`）。

这个等价关系是理解“为什么很多地方用 reduce-scatter”的钥匙：当每个 rank 后续只需要“自己负责的那一片”时，用 reduce-scatter 就够了，不必 all-gather 回全量；当每个 rank 又需要完整结果时，才需要 all-gather 补齐。

#### 2. 问题分析

按“消费方需要什么”选原语：

1. DP + 本地优化器：每个 rank 要更新完整参数，需要完整梯度 → all-reduce。
2. DP + 分布式优化器：每个 rank 只更新自己负责的参数分片 → 梯度用 reduce-scatter，参数更新后再 all-gather 回各副本（C，`M05/implementation.md`）。
3. SP：forward 需要把序列分片拼成完整序列 → all-gather；backward 把完整梯度求和后切回各 rank → reduce-scatter（g）；反向算子 ḡ 正好相反（C，`insights §7.2`）。
4. CP：梯度归约复用 DP 域，本质也是 reduce-scatter/all-reduce 在复合通信域上的应用（C，`M02/design.md`）。

带宽推导：ring all-reduce 的搬运量约 `2(t-1)/t·D`（D 为数据量），拆成 reduce-scatter 的 `(t-1)/t·D` 与 all-gather 的 `(t-1)/t·D`，相加正是 `2(t-1)/t·D`。所以“用 reduce-scatter 代替 all-reduce”不是省带宽，而是省“每个 rank 不需要的那部分”。

#### 3. 具体答案

面试中可以这样回答：

> 一句话区分：all-reduce 让每个人都拿到总和，reduce-scatter 让每个人只拿总和的一片，all-gather 把各片拼回去。
> 选型看消费方：DP 配本地优化器，每个 rank 要更新全量参数，就用 all-reduce；DP 配分布式优化器，每个 rank 只更新自己那片参数，梯度只需要 reduce-scatter，更新完再用 all-gather 把参数广播回去。
> SP 是同一个道理的两步：forward 需要完整序列做 GEMM，所以 all-gather；backward 需要把完整梯度按序列切回去，所以 reduce-scatter。ring all-reduce 本来就等于 reduce-scatter + all-gather，所以这套替换在带宽上等价，不是额外开销。
> 反过来说，如果某处只要一片却用了 all-reduce，就等于白搬运了别人不需要的部分；如果只要一片又必须拼回全量，那 all-gather 是省不掉的。

#### 4. 技术洞察

语义与分解（C）：

```text
all-reduce(D)          : 每 rank 得到 Σ_i D_i
reduce-scatter(D)      : 每 rank 得到 (Σ_i D_i) 的第 r 片
all-gather(D_shard)    : 每 rank 得到 [D_0 | D_1 | ... | D_{N-1}]

ring all-reduce ≈ reduce-scatter + all-gather
volume ≈ (t-1)/t·D  +  (t-1)/t·D  =  2(t-1)/t·D
```

使用位置（C/I，`insights §2A.3/§7.2`）：

| 场景 | forward | backward |
|---|---|---|
| TP 的 g | all-reduce | 恒等 |
| TP 的 f | 恒等 | all-reduce |
| SP 的 g | all-gather | reduce-scatter |
| SP 的 ḡ | reduce-scatter | all-gather |
| 分布式优化器 | — | 梯度 reduce-scatter + 参数 all-gather |

#### 5. 图文说明

```text
all-reduce:        [a0 a1 a2 a3] ×4 同域  ──►  每 rank:  Σ
reduce-scatter:    [a0 a1 a2 a3] ×4 同域  ──►  rank0:S0 rank1:S1 rank2:S2 rank3:S3
all-gather:        rank 各持分片           ──►  每 rank:  [S0 S1 S2 S3]
```

#### 6. 拓展知识

- 与 T-I004 交叉：分布式优化器把“梯度归约”和“参数分发”天然分成 reduce-scatter 与 all-gather 两段。
- 与 T-I006 交叉：SP 的 g/ḡ 与 CP 的序列维通信是同一族原语在不同轴上的复用。
- 与 T-I012 交叉：梯度 finalize 阶段要根据是否分布式优化器选择 reduce-scatter 还是 all-reduce。

#### 7. 常见追问及回答要点

1. **reduce-scatter 之后为什么还要 all-gather？** ——因为下一轮 forward 每个 rank 需要完整（或至少 TP/CP 分片后正确）的参数；只 reduce-scatter 会让参数永久分片在个别 rank 上。
2. **SP 用 reduce-scatter 替代 all-reduce，带宽真的不变吗？** ——不变。ring all-reduce 就是这两段的和；变的是延迟分布与激活 layout，收益落在激活显存。
3. **用错原语会报错吗？** ——不一定。用 all-reduce 顶替 reduce-scatter 通常只浪费带宽、不报错；用 reduce-scatter 顶替 all-reduce 会因拿到部分结果而产生静默的数值错误。

#### 8. 易错点与反例

- 反例：在只需要本体分片的地方用 all-reduce，通信量凭空增加一倍，训练慢但结果正确，很难通过正确性测试发现。
- 反例：在需要完整张量的位置只用 reduce-scatter，后续 GEMM 拿到的是残缺输入，前几步 loss 就异常。
- 易错：把 reduce-scatter 理解成“只做求和不做分发”或“只分发不求和”；它两者都做，且每 rank 只保留一片。

#### 9. 实践与验证

`[待验证]` 单机多卡的微基准：分别测 all-reduce、reduce-scatter、all-gather 的等效数据量耗时，验证 `t_allreduce ≈ t_scatter + t_gather`。oracle：三者在带宽上近似线性，误差在测量噪声内。需在含 NCCL 的目标环境执行，当前未运行。

#### 10. 评分标准

- 不合格：分不清 all-reduce 与 reduce-scatter，回答含糊。
- 合格：能准确描述三种原语的输入输出与两种使用场景。
- 良好：能推导 ring 分解的带宽等价，说明 SP 与分布式优化器的选型理由。
- 优秀/专家：能结合通信域（DP/CP/SP 复合）与 overlap，指出“用错原语多为静默错误”并设计检测方法。

**证据与来源：** `insights/05-megatron-architecture.md` §2A.3/§5.4/§7.2/§7.3（原语表、g/ḡ、带宽等价，`C`）；`megatron-lm/01-modules/M05-optimizer-checkpointing/implementation.md`（分布式优化器 reduce-scatter + all-gather，`C`）；`megatron-lm/01-modules/M02-parallelism/design.md`（CP 梯度归约复用 DP 域，`C`）。版本锚点 `3703d4e33`；带宽结论标 `U / 待验证`。

### T-I006. 序列并行与上下文并行的通信有何不同？g/ḡ 与 CP 的 KV 交换各解决什么问题？

- 难度：中等级
- 知识域：T06 序列与上下文并行
- 考察能力：切分对象辨析、通信形态对比、配置边界
- 题型：对比/设计
- 面试频率：中频
- 前置知识：TP、LayerNorm/Dropout、attention 的 s²、reduce-scatter
- 关联题目：T-I002（TP）；T-I005（原语）；T-I011（激活显存）
- 事实状态：SP 的 g/ḡ 与激活收益 `[已确认]`（C）；CP 通信实现 `[推断/待验证]`（I/U）

#### 1. 背景知识

TP 只切 GEMM 密集区，LayerNorm 与 Dropout 在 TP 组内仍是复制的，这部分激活约 `10·s·b·h` 不被 t 除（C，论文 arXiv:2205.05198）。SP 把这些逐元素算子沿序列维切开，用 g/ḡ 在“序列切分”与“hidden 切分”两种 layout 间转换：g forward 是 all-gather、backward 是 reduce-scatter；ḡ 相反（C，`insights §7.2`）。因为 ring all-reduce = reduce-scatter + all-gather，SP 不增加带宽，收益全在激活显存（C，`insights §7.3`）。

CP 则针对长序列：它沿序列维切分激活，但**不切权重**，attention 的 s² 代价被分摊到多个 rank（C，`insights §9.2`）。代价是 attention 需要在 CP 组内交换 KV/softmax 统计，且因为权重被复制，backward 要额外归约复制权重的梯度；CP 的梯度归约复用 DP 域（C，`M02/design.md`）。

#### 2. 问题分析

两者都碰“序列维”，但切的对象和通信内容不同：

1. 切分对象：SP 切的是 TP 区的“复制段”（LN/Dropout），目的是让激活被 t 除；CP 切的是整个序列的激活（含 attention），目的是让 s² 项被 N 除。
2. 通信内容：SP 传的是 hidden 张量的 layout 转换（g/ḡ，集合通信）；CP 传的是 attention 所需的 KV/中间统计，可能走 P2P（`--cp-comm-type p2p`）或集合型（I，`insights §9.6`）。
3. 权重与梯度：SP 自身不复制额外权重（LN 本来复制），梯度走 TP 的 f/g；CP 复制权重，backward 需要把复制份的权重梯度归约，且归约域是 DP∩CP 的复合（C，`insights §9.7`）。
4. 适用条件：SP 与 TP 并用（MoE+TP 时必须开 SP 以统一 layout，C）；CP 推荐 sequence length ≥ 8K，关键在把通信与计算重叠（C，`insights §9.5`）。

#### 3. 具体答案

面试中可以这样回答：

> 两者都在序列维做文章，但动机不同。
> SP 是为了补 TP 的漏：TP 只切 GEMM，LayerNorm/Dropout 这些逐元素算子的激活还是复制的。SP 把它们也按序列切开，用 g/ḡ 在“序列切分”和“hidden 切分”之间换 layout——g 的 forward 是 all-gather、backward 是 reduce-scatter，ḡ 反过来。因为 ring all-reduce 本来就等于这两步，SP 不涨带宽，收益是激活显存。
> CP 是给长序列用的：它把序列激活切成 N 份，attention 的 s² 被摊开，但权重是复制的、不切。所以 backward 要额外归约复制权重的梯度，而且 attention 在 CP 组内要交换 KV 或 softmax 统计，通信量随序列长度增长。
> 配置上，SP 通常和 TP 绑定，MoE+TP 场景必须开 SP 统一 token layout；CP 在 8K 以上序列才明显划算，且要能做通信/计算重叠，否则通信会成为瓶颈。

#### 4. 技术洞察

layout 转换与通信（C/I）：

```text
SP (g):   forward = all-gather(seq 维)   ; backward = reduce-scatter
SP (ḡ):  forward = reduce-scatter      ; backward = all-gather
带宽: g/ḡ 合计 = all-reduce，故 SP 不增加带宽

CP:      激活按 seq / N 切；权重复制
         attention 需组内 KV/softmax 交换（p2p 或集合）
         backward: 复制权重梯度归约，复用 DP∩CP 域
```

| 维度 | SP | CP |
|---|---|---|
| 切分对象 | LN/Dropout 复制区 | 整个序列激活 |
| 权重 | 不额外复制 | 复制 |
| 主通信 | g / ḡ（集合） | KV/统计交换（可能 P2P） |
| 激活收益 | 非 TP 区 /t | peak /N |
| 触发条件 | 与 TP 并用 | seq ≥ 8K |

#### 5. 图文说明

```text
SP:  [seq 切] --all-gather(g)--> [hidden 切] --GEMM/LN--> [hidden 切] --reduce-scatter(g)--> [seq 切]
CP:  rank0:[s0] rank1:[s1] rank2:[s2]   <--KV/softmax 交换-->   各自算局部 attention 分块
```

#### 6. 拓展知识

- 与 T-I002 交叉：SP 落地依赖 TP 的 hidden 切分布局，两者必须一起开。
- 与 T-I011 交叉：SP 让 LayerNorm/Dropout 激活被 t 除，进入激活估算公式。
- 与 T-I007 交叉：EP 与 TP 组合时强制开 SP，因为 token dispatch 需要统一 layout。

#### 7. 常见追问及回答要点

1. **SP 为什么不增加通信带宽？** ——ring all-reduce = reduce-scatter + all-gather，把一次 all-reduce 拆成 g/ḡ 两个半步并移到正确位置，总量不变。
2. **CP 为什么不切权重？** ——CP 的目标是分摊 s² 的 attention 与序列激活，不动 hidden/head 维；不切权重意味着要多一步复制权重的梯度归约。
3. **SP 和 CP 能混用吗？** ——可以，两者通信域正交（SP 在 TP 内、CP 在序列），但 CP 仅在长序列下有收益，配置要避免过度切分导致通信占主。

#### 8. 易错点与反例

- 反例：只开 SP 不开 TP，或只开 CP 期望权重显存下降——SP 依赖 TP 的 hidden 切分，CP 不切权重。
- 反例：把 CP 当成 TP 的替代来切 hidden，导致 attention head 与序列维布局不匹配，结果静默错误。
- 易错：开了 CP 却没做通信/计算重叠，长序列下通信时间占比上升，吞吐不升反降（U/待验证）。

#### 9. 实践与验证

`[待验证]` 关闭/开启 `--sequence-parallel` 比较激活显存与 step 时间，验证带宽近似不变、激活下降；再用长序列（如 16K）对比 CP=N 的峰值显存与 attention 时间，验证 CP 能把 s² 代价摊开。通信重叠收益与多卡行为需在目标环境测，当前未运行。

#### 10. 评分标准

- 不合格：把 SP 与 CP 混为一谈。
- 合格：能说出 SP 用 g/ḡ、CP 切序列激活不切权重。
- 良好：能解释 SP 带宽等价、CP 需组内 KV 交换与复制权重梯度归约。
- 优秀/专家：能结合序列长度、通信重叠与 CP∩DP 归约域，给出长序列配置的选型与验证方案。

**证据与来源：** `insights/05-megatron-architecture.md` §7.1–7.6/§9.1–9.7（SP 的 g/ḡ、带宽等价、CP 机制，`C/I`，论文 arXiv:2205.05198）；`megatron-lm/01-modules/M02-parallelism/{README,design}.md`（CP 不切权重、梯度复用 DP 域，`C`）。版本锚点 `3703d4e33`；CP 通信实现标 `I`，实测标 `U / 待验证`。

### T-I007. MoE 的 router 与 top-k：负载均衡、容量与通信如何共同决定一次 MoE 层的实现？

- 难度：中等级
- 知识域：T07 MoE 与专家并行
- 考察能力：路由推导、负载均衡、通信选型与配置陷阱
- 题型：设计/配置分析
- 面试频率：中频
- 前置知识：softmax top-k、all-to-all、EP、grouped GEMM
- 关联题目：T-I002（TP/SP）；T-I006（SP 约束）；T-I011（激活/显存）
- 事实状态：EP/dispatcher/token dropping 机制 `[已确认]`（C）；router dtype 与实测收益 `[推断/待验证]`（I/U）

#### 1. 背景知识

MoE 把参数与计算解耦：每个 token 只路由到 top-K 个 expert，参数量大但每 token FLOPs 不线性增长。若像 DP 一样复制全部 expert，每个 rank 都要装下所有专家，显存浪费；EP 把 expert 分布到多个 rank，token 通过 all-to-all 被派发到持有目标 expert 的 rank（C，`insights §11.1`）。官方 MoE 文档给出的关键约束是：EP 与 TP 组合时必须开 SP，因为 token dispatch 需要统一 layout（C，`insights §7.6/§11.2`）。

#### 2. 问题分析

一次 MoE 层的前向可以拆成六步，每一步都藏着配置陷阱（I，`insights §11.5`）：

1. router/gating：每个 token 对 E 个 expert 打分。这是本地 GEMM，但需要完整 token 表示，所以 TP/SP 下要先保证 layout 正确。
2. top-k 选择：每个 token 选 K 个 expert（常见 top-2）。K 与 E 决定 dispatch 的扇出。
3. dispatch：按 allgather / alltoall / flex（DeepEP/HybridEP）把 token 重排到目标 expert 所在 rank。
4. expert FFN：grouped GEMM 批量算多个 expert。
5. combine：把同一 token 的多个 expert 输出加权求和。
6. 回到原 rank 的统一 layout。

容量与负载：capacity = capacity_factor × (tokens/E)；溢出 token 按 `--moe-token-drop-policy probs/position` 丢弃，或设 dropless 不丢。负载不均可通过 aux loss / sinkhorn / capacity factor 校准（C，`insights §11.6`）。dropless 避免信息损失，token dropping 让计算 shape 静态化、可被 CUDA Graph 捕获（C，`insights §11.6`）。

#### 3. 具体答案

面试中可以这样回答：

> 我会把 MoE 层讲成“路由 + 派发 + 专家计算 + 回收”四件事。
> 路由是本地 GEMM：每个 token 对 E 个 expert 打分，取 top-K。这里要保证 token 表示是完整的——所以 TP 和 EP 一起用时必须开 SP，否则 dispatch 的 layout 对不上。
> 派发是通信重载：allgather dispatcher 简单但通信量大，alltoall 直接交换更省，flex + DeepEP/HybridEP 做去冗余和跨节点融合。派发和回收（combine）正是 `--overlap-moe-expert-parallel-comm` 想藏在 FFN 计算后的部分。
> 容量是稳定性的开关：capacity = capacity_factor × (tokens/E)，溢出按 probs 或 position 丢弃。dropless 不丢信息但 shape 动态，难以被 CUDA Graph 捕获；token dropping 换来静态 shape 和稳定吞吐，代价是可能的收敛损失。
> 负载不均要靠 aux loss 或 capacity 校准，否则热点 expert 的 all-to-all 会拖慢整步。选 dispatcher 时看规模：小 experts 数、小 batch 时 all-to-all 开销占比高，可能不如复制 expert。

#### 4. 技术洞察

MoE 前向骨架（I，`insights §11.5`）：

```text
X: s×b×h
1. router: score = local_GEMM(X)            # 需完整 token 表示
2. top-k: 每 token 选 K 个 expert
3. dispatch: token 重排 → 目标 expert rank   # allgather / alltoall / flex
4. expert FFN: grouped GEMM
5. combine: 同 token 的 K 个输出加权求和
6. 回到原 rank 的统一 layout
```

容量与丢弃：

```text
capacity = capacity_factor × (tokens / E)
overflow → drop (probs 优先 / position 优先) 或 dropless 保留
负载均衡: aux_loss / sinkhorn / capacity factor 校准
```

| 选项 | 效果 | 代价 |
|---|---|---|
| dropless | 不丢信息、收敛更稳 | shape 动态，难 graph 捕获 |
| token dropping | shape 静态、吞吐稳 | 可能损失信息、影响收敛 |
| allgather dispatcher | 实现简单 | 通信量大 |
| alltoall dispatcher | 通信省 | 对拓扑敏感 |

#### 5. 图文说明

```text
tokens ── router(top-k) ── dispatch(all-to-all) ──► expert 所在 rank
                                                      │ grouped GEMM
tokens ◄── combine(加权求和) ◄── dispatch 回传 ◄───────┘
              └── 3/5 步是 all-to-all，可与 FFN 计算重叠 ──┘
```

#### 6. 拓展知识

- 与 T-I006 交叉：EP 与 TP 组合必须开 SP，是 MoE layout 的硬约束。
- 与 T-I002 交叉：MoE 层的 attention 与 FFN 对并行度的偏好不同，Parallel Folding 让两类层各用最优拓扑（C，`insights §11.3`）。
- 与 T-I011 交叉：MoE 的 `--moe-layer-recompute` 提供 per-layer per-expert 的激活管理（C，`insights §12.4`）。

#### 7. 常见追问及回答要点

1. **top-k 的 router 为什么要用更高精度算？** ——路由分数决定离散的 expert 选择，数值抖动会改变选中的 expert，通常用 fp32 计算更稳（I，待验证）。
2. **dropless 与 token dropping 怎么选？** ——重吞吐与 graph 捕获选 dropping，重收敛质量选 dropless；常按目标吞吐与质量约束调 capacity factor。
3. **EP 什么时候不划算？** ——experts 数少、batch 小，或 all-to-all 被迫走慢链路时，复制 expert 的 DP 式可能更优（C，`insights §11.7`）。

#### 8. 易错点与反例

- 反例：开了 EP 但忘了开 SP，token dispatch 的 layout 与 TP 分片不一致，前向就出错或静默错误。
- 反例：capacity factor 设得过小导致大量 token 被丢，模型收敛明显变差，却误以为是学习率问题。
- 易错：以为 MoE 一定比稠密模型省显存——参数量不随 FLOPs 增长，但所有 expert 参数仍要存在，只是分摊到 EP 组。

#### 9. 实践与验证

`[待验证]` 固定 E/K/dispatcher，比较 capacity factor 与 drop policy 对 loss 与吞吐的影响；用 nsys 观察 all-to-all 与 FFN 的重叠程度。oracle：capacity 增大时丢弃减少、收敛改善但吞吐下降；alltoall 相对 allgather 应降低通信时间。E 与 EP 的实测需多卡环境，当前未运行。

#### 10. 评分标准

- 不合格：只说“MoE 选 top-k expert”，讲不出 dispatch/combine。
- 合格：能描述路由、派发、专家计算、回收四个阶段。
- 良好：能解释容量/丢弃策略与负载均衡手段，指出 EP+TP 必须开 SP。
- 优秀/专家：能对比 dispatcher 后端与 Parallel Folding，结合 batch/experts 规模给出 EP 收益判断与验证方案。

**证据与来源：** `insights/05-megatron-architecture.md` §11.1–11.7（EP、dispatcher、容量、Parallel Folding，`C/I`，NVIDIA Megatron-Core 官方 MoE 文档）；`megatron-lm/01-modules/M01-core-models/README.md`（MoE layer spec，`C`）；`megatron-lm/90-cross-module/configuration-impact-map.md`（num_experts/EP 影响，`C`）。版本锚点 `3703d4e33`；router dtype 标 `I`，实测标 `U / 待验证`。

### T-I008. fused optimizer 的边界：哪些情况下融合 kernel 的契约会被打破？

- 难度：中等级
- 知识域：T03 数据并行
- 考察能力：后端可插拔、数值边界、状态一致性
- 题型：故障定位/边界分析
- 面试频率：中频
- 前置知识：Adam、loss scaling、参数组、TE/Apex/Torch 后端
- 关联题目：T-I004（分布式优化器）；T-I009（精度）；T-I010（checkpoint state）
- 事实状态：优化器 factory 与 wrapper 选择 `[已确认]`（C）；融合 kernel 数值差异 `[推断/待验证]`（I/U）

#### 1. 背景知识

`get_megatron_optimizer` 会按参数组、混合精度、distributed optimizer、FSDP、CPU offload、MuP 以及 TE/Apex/PyTorch fallback 等配置选择具体实现，`ChainedOptimizer` 还能组合多个 optimizer（C，`M05/README.md`）。`MixedPrecisionOptimizer` 维护低精度参数/主参数、loss scaling 和溢出处理，`FP32Optimizer` 走普通精度路径；具体 wrapper 选择依赖 config 与可选 backend，不能只看类名推断最终 kernel（C，`M05/implementation.md`）。

这让“fused optimizer”成为一个边界很多的组件：它可能是 TE 的 `_FusedAdam`、Apex 的实现、也可能是 PyTorch 原生。融合内核把多步小 kernel 合并，性能好，但对输入布局、参数组划分、溢出处理有更严格的假设。

#### 2. 问题分析

把边界分成“配置边界”和“运行时边界”：

1. 配置边界：参数组配置不一致（weight decay、lr 分组）、空梯度、sparse 梯度、混合 `requires_grad` 的参数，都会让融合内核走 fallback 或产生与参考实现不同的数值（I）。`train_step` 只在 `optimizer.step()` 报告 update successful 时才推进 scheduler（C，`M03/implementation.md`）。
2. 运行时边界：loss scaling 溢出时 step 被 skip，融合 kernel 必须与普通路径一样保留状态；若 skip 逻辑写错，scheduler 会错误推进。
3. 状态边界：checkpoint 里 optimizer state 的布局由具体实现决定；从融合路径切到原生路径、或从单优化器切到 `ChainedOptimizer`，state 形状必须能对应（C，`M05/README.md`）。
4. 组合边界：GTP rematerialization 与多个 distributed-optimizer instance 当前被禁止组合（C，`M02/README.md`）。

#### 3. 具体答案

面试中可以这样回答：

> 我会先明确“fused optimizer 不是一种优化器，而是一组后端实现”，TE、Apex、PyTorch 原生都在这个入口后面，具体走哪个由配置和可选依赖决定。
> 边界主要三块。第一是参数组：不同 lr/weight decay 的参数组、空梯度、稀疏梯度、部分参数 `requires_grad=False`，都可能让融合 kernel 失效或走 fallback。第二是溢出处理：混合精度下 loss scaling 会把溢出的 step skip 掉，融合路径必须和普通路径一样保留状态、不推进 scheduler，否则学习率会错位。第三是状态布局：checkpoint 读写的 optimizer state 形状取决于具体实现，切换实现或改并行度时要能对应。
> 还有两个组合约束要记住：GTP rematerialization 与多个分布式优化器实例当前不允许一起用；CUDA Graph 捕获 optimizer step 时，通信 group 的 MemPool 注册要覆盖实际通信域（C，`M05/README.md`）。
> 验证我会做数值对照：同一组梯度下，融合路径与 PyTorch 参考路径的更新后参数做近似 diff，确认在容差内。

#### 4. 技术洞察

step 时序与 skip 语义（I，`insights §12.5`）：

```text
step:
  scaler 检查 ── 溢出? ──► skip update（保留状态，不推进 scheduler）
       └─ 正常 ──► fp32 master ──► 梯度 gather ──► Adam(分片) ──► 可选 all-gather ──► grad norm
返回: update_successful, grad_norm
```

| 边界 | 触发 | 风险 |
|---|---|---|
| 参数组不一致 | 多 lr/wd 组 | 融合 kernel fallback 或数值差异 |
| 空/稀疏梯度 | 条件分支参数 | 形状假设被打破 |
| loss scale 溢出 | 大梯度 | skip 逻辑错 → scheduler 错位 |
| 实现切换 | TE↔Apex↔Torch | checkpoint state 形状不匹配 |
| GTP + 多 dist-opt | 组合配置 | 初始化直接禁止 |

#### 5. 图文说明

```text
模型参数 ──► _get_param_groups ──► get_megatron_optimizer
                                     ├─ FP32Optimizer
                                     ├─ MixedPrecisionOptimizer ──► (TE/Apex/Torch) Adam
                                     └─ DistributedOptimizer
                                              └─ ChainedOptimizer 组合多个
train_step: 只有 update_successful=True 才 scheduler.step()
```

#### 6. 拓展知识

- 与 T-I004 交叉：分布式优化器把 optimizer state 分片，融合路径要在这套分片上工作。
- 与 T-I009 交叉：fp16 的 loss scaling 与 FP8 的 delayed scaling 都会在 step 边界做数值判断。
- 与 T-I010 交叉：不同实现的 optimizer state 布局决定 checkpoint 兼容性。

#### 7. 常见追问及回答要点

1. **融合 kernel 会改变数学吗？** ——理论上等价，但浮点累加顺序、近似实现与 fallback 分支可能带来微小数值差异，验收应做近似 diff 而非逐位比较（I）。
2. **step 被 skip 后训练怎么继续？** ——保留参数与 optimizer 状态、不推进 scheduler，等待下一 step；`train_step` 依据 update successful 决定是否推进（C）。
3. **为什么 GTP 与多优化器实例被禁止组合？** ——两者的权重/状态管理假设冲突，初始化阶段直接拒绝而不是运行期出错（C）。

#### 8. 易错点与反例

- 反例：混用多参数组与融合 Adam，某些组回退到原生 kernel，梯度裁剪与 scale 的行为不一致，loss 出现难解释的抖动。
- 反例：溢出 skip 的逻辑只处理了参数更新、忘了 scheduler，导致学习率按“成功步数”多走了几步。
- 易错：以为换 TE/Apex/Torch 后端只是性能差别，忽略了各自 checkpoint state 布局不同，迁移时 load 失败。

#### 9. 实践与验证

`[待验证]` 数值对照：固定随机梯度，分别用融合路径与 PyTorch 参考 Adam 执行若干步，比较参数 diff；再制造一次 overflow，确认两条路径都 skip 且 scheduler 不推进。oracle：数值在容差内，skip 行为一致。需 GPU 环境执行，当前未运行。

#### 10. 评分标准

- 不合格：把 fused optimizer 当成唯一实现，不知后端可插拔。
- 合格：能说出 factory 选择与 mixed precision / distributed 路径。
- 良好：能列出参数组、skip 语义、state 布局三类边界。
- 优秀/专家：能结合 CUDA Graph 的 MemPool 注册、GTP 组合约束与 checkpoint 兼容，给出可执行的数值与状态验证方案。

**证据与来源：** `megatron-lm/01-modules/M05-optimizer-checkpointing/{README,implementation}.md`（optimizer factory、mixed precision、ChainedOptimizer、CUDA Graph/NCCL MemPool，`C`）；`megatron-lm/01-modules/M03-training-runtime/implementation.md`（train_step 与 skip，`C`）；`insights/05-megatron-architecture.md` §4.7/§12.5（后端可插拔、step 时序，`C/I`）。版本锚点 `3703d4e33`；融合 kernel 数值差异标 `I`，实测标 `U / 待验证`。

### T-I009. FP8 训练的 scaling 与 recipe：E4M3/E5M2 如何分工，delayed scaling 的陷阱在哪？

- 难度：中等级
- 知识域：T09 混合精度与数值稳定
- 考察能力：精度格式分工、缩放机制、跨 rank 一致性
- 题型：原理/配置分析
- 面试频率：中频
- 前置知识：fp16/bf16 动态范围、loss scaling、amax 统计、TE
- 关联题目：T-I002（TP 数值一致性）；T-I008（step 边界）；T-I012（梯度）
- 事实状态：E4M3/E5M2 分工与 delayed scaling `[推断/待验证]`（I/U）；fp16/bf16 方案 `[已确认]`（C）

#### 1. 背景知识

混合精度里，fp32 训练显存翻倍；fp16/bf16 把激活减半；FP8 在 Hopper 及以后的硬件上把 QKV 投影、MLP 等 GEMM 再减半（C，`insights §12A.1`）。风险在于低精度的动态范围：fp16 指数只有 5 位，容易溢出/下溢；bf16 动态范围接近 fp32 但尾数只有 7 位；FP8 必须靠缩放（scaling）避免幅值突变（C，`insights §12A.2`）。

Megatron 的经典方案（C，`insights §12A.2`）：fp16 用动态 loss scaling，master 参数保持 fp32，forward/backward 用 fp16，loss 乘 scale 防梯度下溢，scaler 监测 inf/nan；bf16 不做 loss scaling，直接前向反向；FP8 由 Transformer Engine 提供，配合延迟缩放（delayed scaling），每次迭代依据统计自动调整 scale。

#### 2. 问题分析

FP8 的关键在于“哪些张量用什么格式、scale 怎么来、跨 rank 是否一致”：

1. 格式分工：E4M3 动态范围宽、适合 forward 的权重与激活；E5M2 范围更大、适合梯度（I，`insights §12A.3`）。
2. scale 来源：需要统计 activation/weight/gradient 的幅值（amax）来设 scale，通常用前一迭代的统计，即 delayed scaling；scale 是逐 tensor 而非全模型，需要额外的 scale 元数据传播（I，`insights §12A.3`）。
3. 跨 rank 一致性：TP 下各 rank 的激活分片不同，若各自独立统计 amax，scale 可能不一致，进而让梯度数值不一致、降低收敛确定性（I，`insights §12A.4`）。通常需要把 amax 在并行组内归约。
4. recipe：决定哪些算子走 FP8、amax history 长度、scale 更新间隔，直接影响数值稳定性与吞吐。

#### 3. 具体答案

面试中可以这样回答：

> FP8 的核心是“用缩放把有限的动态范围撑开”。
> 格式上，E4M3 范围宽，放 forward 的权重和激活；E5M2 范围更大，放梯度，因为梯度的动态范围更极端。
> 缩放上，通常用延迟缩放：本迭代的数值用上一迭代统计出来的 scale 转换，避免幅值突变；scale 是逐 tensor 的，所以需要额外的元数据在 kernel 间传递。
> 最容易出问题的是跨 rank 一致性：TP 下激活被切分，各 rank 独立统计 amax 会得到不同的 scale，梯度就对不齐。工程上要把 amax 在并行组内归约，保证同一个逻辑张量用同一套 scale。
> 调试建议是逐步来：先固定 seed、拿单卡 fp16/bf16 做基线，再逐层开 FP8 对比数值，不要一次全开，否则定位不到是哪一层出的问题。

#### 4. 技术洞察

数值流与缩放（I，`insights §12A.2–12A.4`）：

```text
fp16: master(fp32) ── cast ── fp16 fwd/bwd ── loss × scale ── grads ── unscale ──► master 更新
bf16: master(fp32) ── cast ── bf16 fwd/bwd ──► master 更新（无 loss scaling）
fp8 : amax 统计 → scale → E4M3(权重/激活) / E5M2(梯度) GEMM → amax 更新（delayed）
      scale 逐 tensor；TP 组内 amax 需归约以保持一致
```

| 格式 | 指数/尾数 | 适用 | 缩放 |
|---|---|---|---|
| fp16 | 5 / 10 | 通用 | 动态 loss scaling |
| bf16 | 8 / 7 | 通用 | 无 |
| FP8 E4M3 | 4 / 3 | 权重、激活 | delayed scaling |
| FP8 E5M2 | 5 / 2 | 梯度 | delayed scaling |

#### 5. 图文说明

```text
step t:   张量 ── amax_t ── 存历史
step t+1: 张量 ── 用 amax_t 算 scale ── 量化 ── GEMM ── 反量化
TP:       各 rank 局部 amax ──(allreduce/max)──► 统一 scale
```

#### 6. 拓展知识

- 与 T-I002 交叉：TP 下 scale 与分片布局相互牵制，f/g 归约前必须保证数值口径一致。
- 与 T-I008 交叉：loss/scale 的溢出判断发生在 step 边界，skip 语义要统一。
- 与 T-I010 交叉：scale 元数据、master 参数属于 checkpoint 需要保存的状态集合。

#### 7. 常见追问及回答要点

1. **bf16 为什么不用 loss scaling？** ——bf16 的指数位与 fp32 相同，动态范围足够，梯度下溢风险低；fp16 指数位少，需要 scale（C）。
2. **delayed scaling 的“延迟”延迟了什么？** ——延迟的是 scale 的计算，本迭代用上一迭代的统计，换取不打断计算流水；代价是幅值突变时有滞后（I）。
3. **FP8 会不会影响收敛？** ——会有数值差异，通常 fp8 配合更高精度的 master 与合理的 scale 策略来缓解；收益与稳定性需要在目标硬件验证（U/待验证）。

#### 8. 易错点与反例

- 反例：各 TP rank 独立统计 amax，scale 不一致，梯度聚合后数值偏差累积，loss 曲线抖动。
- 反例：把 loss scaling 的 scale 当作 FP8 的 scale，两者机制不同（loss scale 在损失上、FP8 scale 在张量上），混用导致下溢或溢出。
- 易错：一次性全模型开 FP8，出现问题无法定位；应逐层开并做单卡基线 diff。

#### 9. 实践与验证

`[待验证]` 逐层开启 FP8：固定 seed 与单卡 fp16 基线，按 attention/MLP 逐模块开 FP8，比较每层输出与梯度范数的偏差。oracle：偏差在容差内、loss 下降趋势一致；跨 TP rank 的 amax 归约后 scale 一致。所有 FP8 收益与稳定性均为官方报告口径，本仓库未实测，标 `U / 待验证`。

#### 10. 评分标准

- 不合格：把 FP8 当成“直接换成 8 位”。
- 合格：能说出 E4M3/E5M2 分工与 delayed scaling 的存在。
- 良好：能解释 scale 逐 tensor、跨 rank 需归约、与 fp16 loss scaling 的区别。
- 优秀/专家：能结合 recipe 设计、TP 数值一致性与逐层验证方法，给出 FP8 上线的风险控制。

**证据与来源：** `insights/05-megatron-architecture.md` §12A.1–12A.4（fp16/bf16/FP8、E4M3/E5M2、delayed scaling、跨 rank 一致性，`C/I`，Transformer Engine 官方）；`megatron-lm/01-modules/M05-optimizer-checkpointing/implementation.md`（mixed precision wrapper，`C`）；`megatron-lm/90-cross-module/configuration-impact-map.md`（fp8/fp4 影响，`C`）。版本锚点 `3703d4e33`；FP8 结论标 `I`，实测标 `U / 待验证`。

### T-I010. distributed checkpoint：sharded state dict、reshard 与配置校验如何配合？

- 难度：中等级
- 知识域：T10 检查点、恢复与容错
- 考察能力：分片存储、并行度变更、失败定位
- 题型：设计/故障定位
- 面试频率：中频
- 前置知识：TP/PP/EP 拓扑、state dict、文件系统
- 关联题目：T-I001（恢复状态）；T-I004（optimizer shard）；T-I012（梯度状态）
- 事实状态：两层 checkpoint 与 reshard `[已确认]`（C）；async/本地 checkpoint 语义 `[待验证]`（U）

#### 1. 背景知识

Megatron 的 checkpoint 分两层：training checkpointing 负责命名、tracker、参数校验和全局训练状态；`dist_checkpointing` 负责 sharded mapping 的存储/加载（C，`M05/implementation.md`）。传统路径按 iteration、TP/PP/EP rank 和 distributed optimizer 形成文件路径；分布式 checkpoint 用 sharded state dict 加 metadata（C，`M05/README.md`）。`GPTModel.sharded_state_dict` 是模型到 checkpoint 层的适配边界（C，`M01/implementation.md`）。

保存/恢复同时涉及模型、optimizer、scheduler、RNG、tokenizer、数据加载状态和 tracker（C，`M05/README.md`），并且支持并行度变化时的 reshard（C，`insights §14.1`）。

#### 2. 问题分析

把问题拆成“存什么、怎么存、怎么校验、怎么变更”：

1. 存什么：model sharded state dict + optimizer state + 训练状态（scheduler/RNG/data/tracker）。
2. 怎么存：每 rank 写自己的局部 shard 加 metadata，metadata 描述“这个 shard 属于哪个逻辑张量的哪一部分”，因此 load 时能重组或 reshard（C）。
3. 怎么校验：`check_checkpoint_args` 会拒绝或警告关键结构参数不匹配（C，`M05/README.md`）。这是 load 失败的第一道排查点。
4. 怎么变更：并行度变化时，若 metadata 记录了逻辑张量布局，就能把旧 shard 重新映射到新拓扑；这比“换并行度必须重训”前进了一大步。

#### 3. 具体答案

面试中可以这样回答：

> 我会把分布式 checkpoint 讲成“带元数据的分片存储”。
> 传统 checkpoint 按 rank 存文件，路径里带 iteration 和并行 rank；分布式 checkpoint 则让每个 rank 写自己那块 shard，外加一份描述逻辑张量如何被切分的 metadata。因为 metadata 解耦了“逻辑张量”和“物理 shard”，换并行度时可以 reshard，而不必重训。
> 存储内容至少七类：模型 shard、optimizer 分片、scheduler、RNG、tokenizer、数据加载状态、tracker。
> 校验上，load 先走 `check_checkpoint_args`，模型结构或并行配置不匹配时它会拒绝或警告，这是排查 load 失败的第一个入口。
> 故障定位顺序我一般这样走：先看报错是不是 checkpoint args 校验；再看 shard metadata 与当前拓扑是否对得上；再看 tracker 与文件路径；最后才怀疑文件系统或异步保存未 finalize。

#### 4. 技术洞察

保存/恢复与两层结构（C，`M05/execution-flows.md`）：

```text
save: train loop 触发 → save_checkpoint → generate model/optimizer/training state
      → dist checkpoint writer → tracker/metadata finalize
load: load_checkpoint → 解析 iteration/path → check_checkpoint_args
      → 载入 model + optimizer + scheduler/RNG/data → 返回 iteration 与 consumed samples
```

适配边界：`GPTModel.sharded_state_dict` 把模型状态映射成可被 `dist_checkpointing` 理解的分片表示（C，`M01/implementation.md`）。

| 变更 | 影响 | 处理 |
|---|---|---|
| 改 TP/PP/EP | 物理 shard 布局变 | 依赖 metadata reshard |
| 改 num_layers/hidden | 逻辑张量形状变 | `check_checkpoint_args` 拒绝 |
| 换 optimizer 实现 | state 布局变 | 需能映射或转换 |
| 异步保存中断 | tracker 未 finalize | 该 checkpoint 不可用 |

#### 5. 图文说明

```text
rank r ──► [model shard_r][opt shard_r] ──┐
rank r+1 ► [model shard_{r+1}][opt ...] ──┤──► dist_checkpointing.save
                                          └─ metadata(逻辑张量→shard 映射)
load: metadata + shards ──reshard──► 新拓扑下的 shard 布局
```

#### 6. 拓展知识

- 与 T-I001 交叉：resume 除了 checkpoint 本身，还要恢复 RNG 与数据位置。
- 与 T-I004 交叉：分布式优化器的分片状态是 checkpoint 的重要组成，布局须与 model shard metadata 对齐。
- 与 T-I006 交叉：CP 不切权重，其复制权重的梯度归约域会体现在 optimizer/state 的组织上。

#### 7. 常见追问及回答要点

1. **换了并行度还能加载吗？** ——能，前提是 checkpoint 记录了逻辑张量布局，`dist_checkpointing` 据此 reshard；结构参数变化则会被校验拒绝。
2. **load 失败先查什么？** ——先跑 `check_checkpoint_args`，确认模型/并行配置是否匹配，再查 shard metadata 与文件。
3. **异步保存有什么风险？** ——保存未 finalize 时 tracker 或 metadata 不完整，该 checkpoint 可能不可用；清理和恢复语义需要实测（U/待验证）。

#### 8. 易错点与反例

- 反例：改了 TP/PP 后强行 load 旧 checkpoint，metadata 对不上却不看校验输出，把错误归因到数据或学习率。
- 反例：只存 model 不存 optimizer 分片，resume 后 Adam 状态归零，恢复点附近 loss 抖动。
- 易错：把“分布式 checkpoint”和“传统 rank 文件”混用，路径与 metadata 两套逻辑并存时容易读错文件。

#### 9. 实践与验证

`[待验证]` round-trip：save 后用不同并行度（如 TP 变化）load，比较加载前后若干层参数与 optimizer 状态的等价性；再用一个结构不匹配的配置触发 `check_checkpoint_args` 拒绝。oracle：同拓扑 load 数值一致；跨拓扑 reshard 后数值在容差内。异步保存与文件系统带宽相关，需在目标环境测，当前未运行。

#### 10. 评分标准

- 不合格：以为 checkpoint 只是存模型权重文件。
- 合格：能说出 sharded state dict + metadata 与两层结构。
- 良好：能解释 reshard 与 `check_checkpoint_args` 的作用，给出 load 失败的排查顺序。
- 优秀/专家：能结合 optimizer shard 对齐、异步保存语义与跨拓扑变更，设计完整的 round-trip 与兼容性验证。

**证据与来源：** `megatron-lm/01-modules/M05-optimizer-checkpointing/{README,implementation,execution-flows}.md`（两层 checkpoint、check_checkpoint_args、save/load 流程，`C`）；`megatron-lm/01-modules/M01-core-models/implementation.md`（`sharded_state_dict` 适配边界，`C`）；`insights/05-megatron-architecture.md` §14.1（distributed checkpoint + reshard，`C`）。版本锚点 `3703d4e33`；异步/本地 checkpoint 语义标 `U / 待验证`。

### T-I011. 激活显存估算：从 `10·s·b·h` 到 selective recompute 的 `5as/h > 34` 判据

- 难度：中等级
- 知识域：T11 显存、性能与吞吐
- 考察能力：显存建模、重计算取舍、公式应用
- 题型：推导/计算
- 面试频率：高频
- 前置知识：attention 的 s²、TP/SP、activation recomputation
- 关联题目：T-I002（TP）；T-I003（PP 驻留）；T-I006（SP/CP）
- 事实状态：`10sbh` 与判据 `5as/h>34` `[已确认]`（C，论文）；具体模型实测 `[待验证]`（U）

#### 1. 背景知识

TP 只切 GEMM，LayerNorm 与 Dropout 等逐元素算子的激活在 TP 组内复制，论文给出这部分约 `10·s·b·h` 不被 t 除（C，论文 arXiv:2205.05198）。SP 把这些激活按序列维切开后，这部分也被 t 除。selective recomputation 进一步：对激活体积大但计算密度低的区域（QKᵀ、softmax、softmax dropout、V 乘）做 checkpoint 并在 backward 重算，对其余区域保留（C，`insights §12.3`）。

#### 2. 问题分析

先列激活的大头，再用判据决定重算什么：

1. attention 的分数矩阵正比于 `a·s²·b`（a 为 heads），序列长时按 s² 增长，是长序列的主要激活来源。
2. 每层的常规激活正比于 `s·b·h` 的若干倍；TP 之后 hidden 维被 t 除，但复制的 LN/Dropout 区不被除（除非开 SP）。
3. selective recompute 的判据：`5·a·s/h > 34` 时重算这部分更划算（C，论文）。论文示例：GPT-3（96 heads、s=2048、h=12288）算出 80 > 34，省约 70% 激活；MT-NLG（128 heads、s=2048、h=20480）算出 64 > 34，省约 65%。重算带来的额外 FLOPs 仅约 2.7%（GPT-3）与 1.6%（MT-NLG）（C，论文）。

推导一下判据的含义：重算省下的激活正比于 `a·s²·b` 类的存储，而重算付出的算力正比于 attention 的 FLOPs；两者比值与 `5as/h` 相关联，超过阈值说明“省下的显存远大于多花的时间”。full recomputation 能省更多但通常带来 30–40% 计算开销（C，论文），selective 小得多。

#### 3. 具体答案

面试中可以这样回答：

> 我会先把激活的大头点出来：长序列下 attention 的分数矩阵按 `a·s²·b` 增长，是主项；剩下的常规激活按 `s·b·h` 的倍数走，TP 把 hidden 切了、但复制的 LayerNorm/Dropout 区还在，直到开 SP 才被切。
> 重计算的选择用论文那条判据：`5·a·s/h > 34` 就重算这块更划算。比如 GPT-3 的配置算出来是 80，远大于 34，能省大约七成激活；MT-NLG 是 64，也能省六成多。代价是额外算力只有个位数百分比。
> 对比 full recomputation：省更多激活但通常要付 30%–40% 的计算开销，selective 只重算高存储、低计算密度的那几个算子，所以性价比高得多。
> 落地顺序我会是：先开 SP 把 LN/Dropout 的复制区切掉，再对 attention 内部做 selective recompute，最后才考虑靠 micro-batch 进一步压。

#### 4. 技术洞察

判据与收益（C，论文 arXiv:2205.05198）：

```text
判据:  5·a·s/h > 34  → 该区域重算更划算   (a=heads, s=seq, h=hidden)
GPT-3: 5·96·2048/12288 = 80  > 34 → 省 ~70% 激活, 额外 FLOPs ~2.7%
MT-NLG:5·128·2048/20480 = 64 > 34 → 省 ~65% 激活, 额外 FLOPs ~1.6%
```

分片与重计算的叠加：

| 手段 | 作用对象 | 激活收益 |
|---|---|---|
| TP | GEMM 区 hidden | /t（不含复制区） |
| SP | LN/Dropout 复制区 | 复制区 /t |
| selective recompute | attention 内部大激活 | 按判据省 |
| micro-batch 控制 | 在飞激活 | 线性 |

#### 5. 图文说明

```text
每层激活 ≈  [attention: a·s²·b 分数矩阵]  +  [其余: c·s·b·h]
TP:   hidden 维 /t，但 LN/Dropout 段仍复制
SP:   复制段也 /t
recompute: 若 5as/h>34，把分数矩阵等用重算代替驻留
```

#### 6. 拓展知识

- 与 T-I003 交叉：PP 下每 rank 驻留“在飞 MB 数 × stage 激活”，重计算与 microbatch 节流要一起调。
- 与 T-I006 交叉：SP 与 CP 都降低激活，CP 针对 s² 的 attention 部分。
- 与 T-I007 交叉：MoE 有 per-layer per-expert 的细粒度 recompute/offload 选项（C，`insights §12.4`）。

#### 7. 常见追问及回答要点

1. **`10·s·b·h` 为什么不被 t 除？** ——它对应 LayerNorm/Dropout/residual 这些 TP 复制区的激活；要开 SP 才会被序列维切分。
2. **判据里为什么要带 heads a？** ——因为重算省下的是 attention 分数类张量，其规模与 head 数相关；a 越大，重算越划算。
3. **full 与 selective 的区别？** ——full 重算所有层，省激活最多但计算开销 30%–40%；selective 只挑低计算密度的区域，开销通常只有个位数百分比。

#### 8. 易错点与反例

- 反例：只开 recompute 不开 SP，复制区激活仍在，省下的量小于预期。
- 反例：把 recompute 当成无损显存特效而忽略它增加的算力，短步时间反而上升。
- 易错：把 `s²` 的 attention 激活忽略，长序列下估算严重偏低导致 OOM。

#### 9. 实践与验证

`[待验证]` 固定模型与序列长，分别测：TP-only、TP+SP、再加 selective recompute 的显存峰值与 step 时间，对照判据计算 `5as/h`。oracle：随 SP 与 recompute 开启，激活显存下降、时间增幅与论文口径量级一致。所有数字需按目标模型与硬件实测，当前未运行。

#### 10. 评分标准

- 不合格：只会说“开 recompute 省显存”，没有量级概念。
- 合格：能说出 `10sbh` 与 SP 的关系、recompute 能省激活。
- 良好：能应用 `5as/h > 34` 判据并解释收益与开销。
- 优秀/专家：能结合 PP 在飞 MB、SP/CP 与 MoE 细粒度重计算，给出完整的显存预算方案。

**证据与来源：** `insights/05-megatron-architecture.md` §7.1–7.4/§12.3（`10sbh`、判据 `5as/h>34`、收益与开销，`C`，论文 arXiv:2205.05198）；`megatron-lm/90-cross-module/configuration-impact-map.md`（recompute/sequence_parallel 影响，`C`）。版本锚点 `3703d4e33`；具体模型实测标 `U / 待验证`。

### T-I012. backward 与梯度 finalize：no_sync、microbatch 累积与 DP/CP/SP 归约在何时发生？

- 难度：中等级
- 知识域：T02 分布式初始化与通信组
- 考察能力：梯度同步时机、no_sync 语义、归约域辨析
- 题型：流程/故障定位
- 面试频率：高频
- 前置知识：DDP、microbatch、reduce-scatter/all-reduce、finalize callback
- 关联题目：T-I003（PP 调度）；T-I004（分片优化器）；T-I013（sampler）
- 事实状态：no_sync 与 finalize 时机 `[已确认]`（C）；CP/SP 归约细节 `[推断/待验证]`（I/U）

#### 1. 背景知识

无 pipeline 调度逐 microbatch 调用 `forward_step` 与 backward，除最后一个 microbatch 外使用 `no_sync_func`，让梯度同步延迟到最后一次；结束后统一调用 `config.finalize_model_grads_func`（C，`schedules.py:723-928`）。也就是说无流水线路径里，这个函数既算 loss，也承担 schedule 级的同步边界。

`finalize_model_grads` 负责把各并行域（DP/CP/SP）的梯度归约齐；具体按是否分布式优化器选择 reduce-scatter 或 all-reduce（C/I，`insights §3.4`）。`train` 在启动时设置梯度同步回调与 finalize callback（C，`M03/implementation.md`）。

#### 2. 问题分析

梯度同步有三个独立维度要分清：

1. microbatch 维：DDP 的 all-reduce 每个 microbatch 都做很浪费，所以用 `no_sync` 累积，只在最后一次触发归约（C）。
2. 并行域维：DP 域做梯度归约；TP 的行切参数通过 f/g 在层内归约，列切参数不跨 rank；CP 因为权重复制要额外归约复制份（C，`M02/design.md`）。
3. 优化器维：本地优化器用 all-reduce 得到完整梯度；分布式优化器用 reduce-scatter 得到本 rank 分片（C，`M05/implementation.md`）。

不变量：finalize 必须在所有 microbatch 的 backward 之后、`optimizer.step` 之前调用；漏调则梯度只累积在本地、未跨 DP 归约，各 rank 参数发散。

#### 3. 具体答案

面试中可以这样回答：

> 梯度同步要分三个维度看。
> 第一是 microbatch 维：DDP 如果每个 microbatch 都 all-reduce 太浪费，所以除最后一个外都走 `no_sync`，把同步推迟到最后一次。无 pipeline 的调度就是靠这个边界，结束后统一调 `finalize_model_grads`。
> 第二是并行域维：DP 域做梯度归约；TP 的列切参数各 rank 不同、不需要跨 rank 归约，行切参数通过 f/g 归约；CP 因为权重复制，要多一步把复制份的梯度加回来。
> 第三是优化器维：本地优化器要完整梯度，用 all-reduce；分布式优化器只要自己那片，用 reduce-scatter。
> 判断实现是否正确就看一个不变量：finalize 必须在所有 microbatch 反向之后、`optimizer.step` 之前。漏了它，梯度只在本地累积，不同 rank 的参数会发散；重复调用又会把梯度加两遍。

#### 4. 技术洞察

无流水线调度的同步结构（C，`schedules.py:723-928`）：

```text
forward_backward_no_pipelining:
  for i, mb in enumerate(microbatches):
      forward_step(mb)
      if i != last_mb:
          backward(no_sync_func)      # 延迟同步
      else:
          backward()                  # 触发同步
  finalize_model_grads_func()         # DP/CP/SP 归约边界
  → optimizer.step
```

归约域对照（C/I）：

| 维度 | 归约时机 | 原语 |
|---|---|---|
| microbatch | 最后一次 backward | no_sync 累积 |
| DP | finalize / backward | all-reduce 或 reduce-scatter |
| TP 行切 | 层内 f/g | all-reduce |
| TP 列切 | 无需跨 rank | — |
| CP | 复制权重梯度 | 复用 DP 域归约 |

#### 5. 图文说明

```text
mb0 ── fwd/bwd(no_sync) ──┐
mb1 ── fwd/bwd(no_sync) ──┤ 累积本地梯度
...                        │
mb_last ─ fwd/bwd ─────────┘──► finalize_model_grads ──► optimizer.step
                                      └ DP/CP/SP 归约齐
```

#### 6. 拓展知识

- 与 T-I004 交叉：是否分布式优化器决定 finalize 阶段用 reduce-scatter 还是 all-reduce。
- 与 T-I003 交叉：PP 调度同样按 config 在合适时机调 finalize，不能只在无流水线路径处理。
- 与 T-I015 交叉：microbatch 累积的缩放（除以 microbatch 数）与 finalize 时机共同决定有效梯度。

#### 7. 常见追问及回答要点

1. **no_sync 为什么能省通信？** ——把多个 microbatch 的 DDP all-reduce 合并成一次，减少 NCCL 调用次数与启动开销。
2. **漏调 finalize 会怎样？** ——梯度只在本地累积，未跨 DP/CP 归约，各 rank 参数发散，loss 通常表现为 rank 间不一致或整体不收敛。
3. **CP 的梯度为什么要额外归约？** ——CP 不切权重，权重的梯度在 CP 组内是复制的，必须把各 rank 的部分和加回来（C，`M02/design.md`）。

#### 8. 易错点与反例

- 反例：自定义 schedule 忘了调 finalize，梯度未跨 DP 归约，训练看似在跑但各 rank 悄悄发散。
- 反例：把 finalize 放在 `optimizer.step` 之后，用旧梯度更新，等价于延迟一个 step 的同步。
- 易错：把 TP 列切参数也拿去跨 rank 归约，等于把各 rank 不同的参数错误求和，结果静默错误。

#### 9. 实践与验证

`[待验证]` 在无流水线与 PP 两种配置下打印各 rank 的梯度范数，确认 finalize 后一致、skip 时一致；故意去掉 finalize 观察是否发散。oracle：正常路径梯度范数跨 rank 一致，去 finalize 后不一致。多卡需含 NCCL 环境，当前未运行。

#### 10. 评分标准

- 不合格：认为梯度同步是 DDP 自动完成的，不知 no_sync/finalize。
- 合格：能说出 no_sync 累积与 finalize 的时机。
- 良好：能区分 DP/TP/CP/SP 的归约域，指出 finalize 的位置不变量。
- 优秀/专家：能结合分布式优化器选择、PP 调度与 microbatch 缩放，给出完整的梯度同步链路与验证方法。

**证据与来源：** `megatron-lm/01-modules/M02-parallelism/implementation.md`（无 pipeline 调度的 no_sync 与 finalize，`C`）；`megatron-lm/01-modules/M03-training-runtime/implementation.md`（train 设置同步与 finalize callback，`C`）；`megatron-lm/01-modules/M02-parallelism/design.md`（CP 复制权重梯度归约，`C`）；`insights/05-megatron-architecture.md` §3.4（grad buffer 与归约，`I`）。版本锚点 `3703d4e33`；CP/SP 归约细节标 `I`，实测标 `U / 待验证`。

### T-I013. 数据并行 sampler 一致性：DP rank/size 如何决定每个进程看到的 batch？

- 难度：中等级
- 知识域：T03 数据并行
- 考察能力：采样语义、数据分片、恢复一致性
- 题型：流程/配置陷阱
- 面试频率：中频
- 前置知识：DataLoader、shuffle、blended dataset、global batch 划分
- 关联题目：T-I001（恢复数据位置）；T-I010（checkpoint）；T-I015（global batch）
- 事实状态：sampler 语义与 blended builder `[已确认]`（C）；多数据集混合的实测一致性 `[待验证]`（U）

#### 1. 背景知识

M04 的数据链是 `GPTDatasetConfig → Indexed/Mock dataset → GPTDataset → blended builder → sampler/DataLoader → get_batch`（C，`M04/README.md`）。普通 sampler、random sampler、hybrid CP sampler 和 full validation sampler 把 dataset sample 变成 rank-local batch；`build_pretraining_data_loader` 负责选择 loader，随机数据集包装器用于 epoch/seed 可复现（C，`M04/implementation.md`）。`get_batch` 再从 iterator 取字典，按 TP source rank 做 CUDA 搬运和广播，再按 packed/CP/MTP 分支整理（C，`M04/implementation.md`）。

一个核心不变量：sampler 的 DP rank/size 决定每个进程看到的 batch（C，`M04/README.md`）。这意味着数据分片与并行拓扑强绑定。

#### 2. 问题分析

把问题拆成“怎么切、怎么复现、怎么恢复”：

1. 怎么切：global batch G 先被 DP 切成 `G/DP`，再在 rank 内按 PP/schedule 切成 micro-batch。所以每 rank 每步看到 `G/DP` 个样本，micro-batch = `G/(DP·MB)`。
2. 怎么复现：随机数据集包装器用独立的 seed 控制 shuffle，epoch/seed 决定样本顺序（C）。不同 DP size 下，“谁看到哪个 shard”由 sampler 逻辑决定，必须保证各 rank 的 micro-batch 彼此相异、并集覆盖 global batch。
3. 怎么恢复：resume 依赖 consumed samples 重建 sampler 的 epoch/offset（C，T-I001）；若只对齐 iteration 不对齐样本偏移，样本会重复或跳过。
4. 多数据集：`BlendedMegatronDatasetBuilder` 按采样权重混合多个数据集（C，`insights §14.4`），权重取整与边界最容易引入 rank 间不一致。

#### 3. 具体答案

面试中可以这样回答：

> 数据分片本质是两层切分：global batch 先被 DP 切到各 rank，再在 rank 内被 schedule 切到 microbatch。所以每 rank 每步的样本数固定是 `G/DP`，micro-batch 是 `G/(DP·MB)`。
> 复现的关键是 seed 与 sampler 的独立性。Megatron 给 data loader 单独的 shuffle seed，并用随机数据集包装器保证 epoch/seed 可复现；改 seed 或改 DP size 都会改变“谁看到哪片”，这是设计上允许的、但必须整组一起改。
> 恢复时不能只对齐 iteration，还要用 consumed samples 重建 sampler 的 epoch 和偏移，否则会重复或跳过样本。
> 多数据集混合是最容易出问题的地方：blend 权重怎么取整、边界样本归谁，都要保证各 rank 的并集严格等于 global batch，否则会静默地多训或少训一部分数据。

#### 4. 技术洞察

采样与分片（C/I）：

```text
global batch G ── DP 切 ──► 每 rank G/DP ── schedule 切 ──► MB 个 micro-batch
micro_batch_size = G / (DP × MB)
sampler: DP rank/size → 本进程样本集合
resume: consumed_samples → epoch/offset 重建
blend:  多数据集按权重混合，求并集 = global batch
```

不变量：sampler 的 DP rank/size 决定每进程 batch（C）；各 rank micro-batch 互异且并集覆盖 global batch；packed sequence 的 flatten tensor 必须伴随 cu-sequence metadata（C，`M04/README.md`）。

#### 5. 图文说明

| 层 | 切分依据 | 结果 |
|---|---|---|
| global → rank | DP size | 每 rank G/DP |
| rank → microbatch | MB / schedule | G/(DP·MB) |
| epoch 边界 | seed + offset | shuffle 顺序 |
| 多数据集 | blend 权重 | 采样分布 |

```text
global batch G
  ├─ DP0: [s0 s1 ...] ──► mb0 mb1 ...
  ├─ DP1: [s2 s3 ...] ──► mb0 mb1 ...
  └─ ...      并集 == G，且互不重叠
```

#### 6. 拓展知识

- 与 T-I001 交叉：恢复数据位置必须与 RNG 一起存，否则 shuffle 与 dropout 都会错位。
- 与 T-I015 交叉：global batch 的构成式 `G = micro_batch_size × MB × DP` 把采样与梯度累积绑在一起。
- 与 T-I004 交叉：DP size 变化会影响优化器分片，数据分片与状态分片都要一起改。

#### 7. 常见追问及回答要点

1. **改 DP size 要动什么？** ——sampler 的 rank/size、优化器分片、checkpoint shard 都要一起改；只改一个会导致样本重叠或状态错位。
2. **random sampler 的 seed 存在哪？** ——属于 checkpoint 要保存的数据加载状态，resume 时随 RNG/tracker 一起恢复（C/I）。
3. **blend 权重为什么容易出错？** ——权重取整与边界分配会决定某个样本属于哪个 rank；取整不当会造成并集不等于 global batch（I，待验证）。

#### 8. 易错点与反例

- 反例：改了 DP 但没改 sampler，多个 rank 拿到相同样本，等效于重复训练同一批数据。
- 反例：恢复时把 consumed samples 直接设成 `iteration × G`，忽略 shuffle 与 blend，样本顺序错位。
- 易错：以为 `get_batch` 只是搬运；它还要按 TP rank 广播、按 CP/MTP 分支整理，顺序与 layout 错了 loss 直接异常。

#### 9. 实践与验证

`[待验证]` 采样一致性检查：用固定 seed 跑一个 epoch，收集各 rank 的样本索引，验证两两不重叠且并集等于 global batch；再改 DP size 重跑，确认总量守恒。多数据集场景下用带权 blend 验证分布。多卡需运行环境，当前未运行。

#### 10. 评分标准

- 不合格：只知道“DP 分数据”，说不出 sampler 的决定权。
- 合格：能说出 sampler 的 DP rank/size 决定每进程 batch、两级切分。
- 良好：能解释 seed/offset 与 resume、blend 权重的一致性要求。
- 优秀/专家：能设计采样一致性与多数据集 blend 的验证方案，并指出 DP 变更时数据与状态需同步调整。

**证据与来源：** `megatron-lm/01-modules/M04-data-pipeline/{README,implementation,interfaces}.md`（sampler、blend、`get_batch`、不变量，`C`）；`insights/05-megatron-architecture.md` §14.4（blend 与 microbatch 关系，`C/I`）；`megatron-lm/90-cross-module/configuration-impact-map.md`（CP/sequence 对 batch 处理影响，`C`）。版本锚点 `3703d4e33`；blend 实测一致性标 `U / 待验证`。

### T-I014. 训练 profiler 归因：如何把一步时间拆成 compute / comm / idle 并定位 bubble？

- 难度：中等级
- 知识域：T11 显存、性能与吞吐
- 考察能力：性能建模、profiler 使用、瓶颈归因
- 题型：故障定位/度量
- 面试频率：中频
- 前置知识：MFU、nsys/NCCL trace、overlap、bubble
- 关联题目：T-I003（bubble）；T-I004（通信代价）；T-I013（数据搬运）
- 事实状态：timer/MFU/trace 口径 `[已确认]`（C）；实测数值 `[待验证]`（U）

#### 1. 背景知识

Megatron 内置 timer 统计 per-iteration、per-microbatch、FP/BP 耗时（`timers('forward-compute')` 等），`--log-interval` 打印 throughput、MFU 和 loss；分布式性能分析用 nsys/NCCL 查看每 step 的通信与空闲比，定位“通信未重叠”还是“kernel 启动抖动”（C/I，`insights §14.6`）。MFU 的定义为实际模型 FLOPs 除以 `GPU 数 × 单卡峰值 FLOPs × 运行时长`（C，`insights §12B.2`）。

性能关键路径文档也列出热点：attention/MLP forward、pipeline P2P、data loading、gradient reduce、checkpoint、inference KV cache，并强调结论必须依赖 profiler 或 benchmark（C，`performance-critical-paths.md`）。

#### 2. 问题分析

把一步步时间拆成三项（I，`insights §12B.1`）：

```text
step_time ≈ compute_time + communication_time + idle_time
compute  = 总 FLOPs / (GPU 数 × 单卡峰值 × MFU)
comm     = Σ(原语体积 / 有效带宽) + 原语次数 × 延迟
idle     = PP bubble + 未重叠等待 + kernel 启动开销
```

归因流程：

1. 先看 compute：用 MFU 与理论 FLOPs 反推，若 MFU 低，可能是小 GEMM、FP 利用率不足或 recompute 开销。
2. 再看 comm：nsys/NCCL 里若集合通信与计算时间线不重叠，说明 overlap 未生效；PP 的 P2P 空隙对应 bubble。
3. 最后看 idle：kernel 启动抖动、CUDA Graph 未捕获的部分、CPU 侧数据加载瓶颈都会体现为 GPU 空档。
4. 度量纪律：固定 global batch、说清 weak/strong scaling、至少重复 3 次取中位数，并区分 comm 与 idle（C/I，`insights §12B.4`）。

#### 3. 具体答案

面试中可以这样回答：

> 我会把一步拆成 compute、comm、idle 三块。compute 用理论 FLOPs 除以 `GPU 数 × 峰值 × MFU` 来估；comm 是各原语的体积除以带宽加延迟；idle 是 PP 气泡、未重叠的等待和 kernel 启动开销。
> 具体归因我按顺序走：先用 MFU 判断 compute 是否吃满；再打开 nsys 看集合通信有没有和计算并行，没并行就是 overlap 没生效；再看空档来自哪里——PP 的 warmup/cooldown 是气泡，数据加载慢会让 GPU 等 CPU，CUDA Graph 没覆盖的算子会有启动抖动。
> 报数要守纪律：固定 global batch、说清是 weak 还是 strong scaling、每 step 取时间而不是每秒样本、重复三次取中位数。移动开发机上没 GPU/NCCL 就没法下结论，所以我会把 comm 与 idle 的量级都标成待验证。
> 常见的误判是“通信慢”其实是没有 overlap，或者“计算慢”其实是 recompute 的额外算力被算进了有效 FLOPs。

#### 4. 技术洞察

三分解与归因（I，`insights §12B.1`）：

```text
step_time ≈ compute + comm + idle
MFU = 实际模型 FLOPs / (GPU 数 × 单卡峰值 FLOPs × time)

归因顺序:
  1) MFU 低?        → 小 GEMM / recompute / 精度
  2) comm 未重叠?   → overlap 开关 / stream 依赖
  3) idle 来源?     → PP bubble / 数据加载 / kernel launch
```

| 观测现象 | 可能根因 | 处置 |
|---|---|---|
| MFU 低但 NCCL 少 | GEMM 碎片化、recompute | 调 TP/MB、关 full recompute |
| NCCL 与计算不并行 | overlap 未生效 | 检查 stream 依赖与开关 |
| GPU 周期性空档 | PP bubble | 增大 MB/PP、interleaved |
| 空档非周期 | 数据加载/启动抖动 | 预取、CUDA Graph |

#### 5. 图文说明

```text
time ─►
[==== compute ====][ comm ][ compute ][ idle ][==== compute ====]
                    └ 未重叠: idle++     └ bubble: idle++
MFU 反映 compute 占比；trace 反映 comm 与 idle 的分布
```

#### 6. 拓展知识

- 与 T-I003 交叉：气泡比例公式给出 idle 的理论下界，可校准 profiler 观测。
- 与 T-I004 交叉：分布式优化器的 reduce-scatter/all-gather 是否重叠，直接决定 comm 项。
- 与 T-I013 交叉：数据加载与 `get_batch` 的 CPU→GPU 搬运常表现为非周期空档。

#### 7. 常见追问及回答要点

1. **MFU 低一定是计算慢吗？** ——不一定。等待通信、启动抖动、recompute 都会拉低观测 MFU；要结合 trace 区分。
2. **怎么判断通信是否重叠？** ——看 nsys 时间线上集合通信与 GEMM 是否并行；若串行，检查 stream 依赖和 overlap 开关。
3. **为什么不能只看 tokens/s？** ——吞吐受 batch 与序列长影响，跨配置不可比；应看 step 时间与 MFU，并固定 scaling 口径。

#### 8. 易错点与反例

- 反例：只看 tokens/s 横向对比不同 batch 的配置，误以为吞吐提升来自并行优化。
- 反例：把 recompute 增加的算力当成“有效 FLOPs”，MFU 虚高，掩盖真实瓶颈。
- 易错：在共享或不稳定环境测扩展性，未做重复与中位数，结论不可复现。

#### 9. 实践与验证

`[待验证]` 固定配置跑 nsys，分别统计 compute 时间、各集合通信时间、GPU 空档时间，三者相加对照 step_time；再开关 overlap 对比 comm 项。oracle：三分解在量级上闭合，overlap 开启后 comm 与 compute 并行、step 时间下降。须在含 GPU/NCCL 的目标环境执行，当前未运行。

#### 10. 评分标准

- 不合格：只会说“看 profiler”，给不出分解框架。
- 合格：能写出 compute/comm/idle 三分解与 MFU 定义。
- 良好：能按顺序归因，区分未重叠通信与 PP 气泡，说明度量纪律。
- 优秀/专家：能把三分解与并行配置、overlap、recompute 联动，设计可复现的扩展性测量方案。

**证据与来源：** `insights/05-megatron-architecture.md` §12B.1–12B.6/§14.6（step 三分解、MFU、观测习惯、度量纪律，`C/I`）；`megatron-lm/90-cross-module/performance-critical-paths.md`（热点路径与 profiler 要求，`C`）；`megatron-lm/01-modules/M03-training-runtime/README.md`（日志与 profiling 状态，`C`）。版本锚点 `3703d4e33`；实测数值标 `U / 待验证`。

### T-I015. 梯度累积与 global batch 等价性：num_microbatches 如何影响有效 batch 与梯度缩放？

- 难度：中等级
- 知识域：T01 训练入口与生命周期
- 考察能力：批量构成、梯度缩放、等价性验证
- 题型：推导/配置分析
- 面试频率：高频
- 前置知识：global batch、micro-batch、梯度累积、loss 缩放
- 关联题目：T-I003（microbatch 调度）；T-I012（梯度 finalize）；T-I013（采样）
- 事实状态：grad buffer 缩放与 global batch 构成 `[已确认/推断]`（C/I）；动态 microbatch 行为 `[待验证]`（U）

#### 1. 背景知识

训练里 global batch 是优化意义上的批量，micro-batch 是显存允许的单次前向规模，二者通过梯度累积与并行度联系（C/I，`insights §3.4`）：

```text
global_batch = micro_batch_size × num_microbatches × DP
```

无流水线调度逐 microbatch 前向后反向，除最后一个外使用 `no_sync` 延迟梯度同步，结束后统一 finalize（C，`schedules.py:723-928`）。grad buffer 在累积阶段做梯度缩放（除以 micro-batch 数），再在 reduce 阶段触发 reduce-scatter 或 all-reduce（I，`insights §3.4`）。

#### 2. 问题分析

等价性的数学前提：设 global batch 的一次 loss 是各 micro-batch loss 的均值 `L = (1/M)Σ L_i`，则

```text
∂L/∂w = (1/M) Σ ∂L_i/∂w
```

也就是说，先对每个 micro-batch 求梯度再除以 M 求和，等价于对完整 global batch 求一次梯度。若实现里把梯度相加却不除以 M，有效学习率会放大 M 倍，训练行为完全改变。

配置陷阱：

1. 改 `micro_batch_size` 与改 `num_microbatches` 对 global batch 的贡献等价，但对显存、通信次数、数值舍入的影响不同。
2. 动态 microbatch（rampup）会改变每步实际 M，若仍按固定 M 缩放，等价性被破坏（U/待验证）。
3. 若梯度累积与流水线、SP/CP 叠加，缩放必须发生在归约之前、且只做一次。

#### 3. 具体答案

面试中可以这样回答：

> 我会先写清构成式：`global_batch = micro_batch_size × num_microbatches × DP`。global batch 是优化器眼里的批量，micro-batch 是显存允许的单次规模，DP 和 MB 一起把它凑出来。
> 等价性的核心是梯度缩放。对完整 batch 的均值 loss 求导，等于对每个 micro-batch 的梯度取平均再求和；所以实现必须在累积时除以 micro-batch 数 M，并且只除一次。漏掉这一步，有效学习率就被放大 M 倍。
> 调参上，增 `micro_batch_size` 和增 `num_microbatches` 对 global batch 等价，但前者吃显存、后者多花通信与调度；显存紧就靠 MB 加，通信受限就谨慎加 MB。
> 动态 microbatch 是个特例：每步 M 变了，如果还按固定 M 缩放，数学等价性就破了，要按当步实际 M 缩放。verification 上我会固定 global batch，比较“MB=1 累加 M 次”与“MB=M 一次”的梯度范数。

#### 4. 技术洞察

grad buffer 两阶段（I，`insights §3.4`）：

```text
accumulate: grad += (1/M) · grad(microbatch_i)      # 除以 microbatch 数
reduce:     bucket → reduce-scatter(DP+dist-opt) 或 all-reduce
不变量:     global_batch = micro_batch_size × MB × DP
            除以 M 只在累积阶段做一次
```

| 变量 | 影响显存 | 影响通信 | 影响有效 batch |
|---|---|---|---|
| micro_batch_size ↑ | 显著 | 略 | 线性 |
| num_microbatches ↑ | 轻 | 增加调度/同步 | 线性 |
| DP ↑ | 无 | 增加梯度通信 | 线性 |

#### 5. 图文说明

```text
mb0 ── grad(no_sync) ──┐
mb1 ── grad(no_sync) ──┤ 累积: Σ grad_i / M
...                    │
mb_{M-1} ─ grad ───────┘──► finalize ──► optimizer.step
└──────── 等价于对 global batch 求一次均值梯度 ────────┘
```

#### 6. 拓展知识

- 与 T-I012 交叉：梯度缩放发生在累积阶段，finalize 只负责归约，两者顺序不能反。
- 与 T-I003 交叉：MB 既是梯度累积次数，也是流水线的 microbatch 数，同一参数服务两个目的。
- 与 T-I013 交叉：DP 决定每 rank 的样本分片，global batch 的构成同时约束采样与累积。

#### 7. 常见追问及回答要点

1. **不除以 microbatch 数会怎样？** ——有效学习率放大约 M 倍，收敛行为改变；这与“想用更大 batch”不是一回事，除非同步调 lr。
2. **MB 和 micro_batch_size 怎么选？** ——显存紧优先加 MB（不增单次激活），通信受限则谨慎加 MB；两者对 global batch 等价。
3. **动态 microbatch 有什么风险？** ——每步 M 不同，固定缩放会破坏等价性；需要按当步 M 缩放，且调度与 checkpoint 要能处理（U/待验证）。

#### 8. 易错点与反例

- 反例：为了用大 batch 只加 `num_microbatches` 却不调整 lr 策略，误以为与直接放大 micro-batch 等价。
- 反例：在累积阶段和 finalize 阶段各除一次 M，梯度被缩小 M 倍，训练几乎不收敛。
- 易错：动态 microbatch 下沿用固定 M 缩放，梯度幅值随步数波动，loss 曲线出现周期性抖动。

#### 9. 实践与验证

`[待验证]` 等价性对照：固定 global batch 与 seed，比较“micro_batch_size=1、MB=M”与“micro_batch_size=M、MB=1”的梯度范数与若干步 loss。oracle：两者梯度在容差内一致、loss 曲线吻合。动态 microbatch 与多卡行为需在目标环境验证，当前未运行。

#### 10. 评分标准

- 不合格：把 global batch 与 micro-batch 混为一谈。
- 合格：能写出 `G = micro_batch_size × MB × DP`。
- 良好：能推导均值梯度的缩放等价性，指出除以 M 的位置与次数。
- 优秀/专家：能结合动态 microbatch、PP/SP 归约顺序与 lr 策略，设计可验证的等价性实验。

**证据与来源：** `megatron-lm/01-modules/M02-parallelism/implementation.md`（no_sync 累积与 finalize，`C`）；`insights/05-megatron-architecture.md` §3.4/§5.6/§14.4（grad buffer 缩放、global batch 构成、microbatch 与 DP 关系，`C/I`）；`megatron-lm/90-cross-module/configuration-impact-map.md`（并行配置对 batch 的影响，`C`）。版本锚点 `3703d4e33`；动态 microbatch 行为标 `U / 待验证`。
