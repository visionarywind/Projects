# 训练方向入门级面试题（T-B001–T-B015）

> 每题按 12 段结构给出：元数据、背景知识、问题分析、具体答案、技术洞察、图文说明、拓展知识、常见追问、易错点与反例、实践与验证、分层评分、证据与来源。
> 证据规则：源码/文档结论区分 `C`（源码明确陈述）/`I`（跨文档推断）；GPU、NCCL、多机、多进程与性能结论未经目标环境运行时标 `U / 待验证`。

### T-B001. Megatron 风格训练的入口到训练循环，`initialize_megatron`、`pretrain` 与训练主循环各自负责什么？

- 难度：入门级
- 知识域：T01 训练入口与生命周期
- 考察能力：职责边界、调用顺序与不变量
- 题型：流程/调用链/数据流
- 面试频率：高频
- 前置知识：Python 入口脚本、`torch.distributed` 基本概念、训练循环（前向/反向/更新）
- 关联题目：后续 T-B002（分布式初始化）、T-B006（global batch）、T-I001（恢复训练）
- 事实状态：`[已确认]`（接口职责）；运行与多卡行为 `[待验证]`

#### 1. 背景知识

Megatron 风格训练的启动分为三层职责，不能混为一谈：

- **入口脚本（`pretrain` 命令）**：解析命令行参数（`parse_args`），构造 `TransformerConfig`，注册模型 provider（`ModuleSpec`），设置数据 provider，然后交给 `initialize_megatron`。它决定"训练什么"。
- **`initialize_megatron`**：把解析结果落到运行环境——初始化 `torch.distributed`、根据并行参数调用 `initialize_model_parallel` 建立各通信组、设置随机种子与 `CUDA` 设备、构建全局 buffer。它决定"在什么拓扑上训练"。
- **训练主循环（`pretrain()` → `train()`）**：构造模型/优化器/学习率调度器，取 `forward_backward_func`，按 schedule 迭代 `global_step`，每步做 forward、backward、`optimizer.step()` 并记录日志。它决定"怎么迭代"。

关键事实（源码，`C`）：`initialize_model_parallel` 必须在 `init_process_group` 之后调用；`get_forward_backward_func` 返回的调度函数由 pipeline 并行度决定（无 / 非交错 / 交错）。参考源码版本锚点：Megatron-LM main @ `3703d4e33a3a2b2d11ebcc8e41f45af7ce7d1eda`。

#### 2. 问题分析

回答"入口到训练循环"要同时给顺序和边界：

1. **划定边界**：入口脚本负责参数与配置；`initialize_megatron` 负责分布式与设备环境；`train()` 负责迭代。三者不共享可变职责——模型构造发生在环境初始化之后，通信组在反向传播之前就已建好。
2. **调用顺序（不变量）**：
   - 不变量 A：`init_process_group` → `initialize_model_parallel` → 模型构造 → `get_forward_backward_func` → forward/backward。顺序颠倒会导致通信组未就绪或 rank 不匹配。
   - 不变量 B：`world_size` 必须能被模型并行规模（TP×PP×CP…）整除，否则 process group 创建失败。
3. **边界反例**：把模型构造放在 `initialize_model_parallel` 之前——TP 层不知道自己的 `tensor_model_parallel_rank`，权重分片位置错误，训练静默产生错误结果（不报错但收敛异常），这是最隐蔽的一类入口错误。

#### 3. 具体答案

面试中可以这样回答：

> 我会把启动拆成三个明确阶段。第一阶段是入口脚本：解析参数、生成 `TransformerConfig`、注册模型和数据 provider，它只产出配置。
>
> 第二阶段是 `initialize_megatron`：先 `torch.distributed.init_process_group`，再 `initialize_model_parallel(tensor_model_parallel_size, pipeline_model_parallel_size, ...)`，由 `RankGenerator` 产生 TP/PP/DP/CP/EP 各通信组，同时设置当前 CUDA 设备和随机种子。这一层回答"我在这张拓扑上是哪个 rank"。
>
> 第三阶段是 `pretrain`/`train`：构造模型、`Adam` 优化器、学习率调度器，拿到 `forward_backward_func`，然后进入 `global_step` 循环——`forward_step` 计算 loss、反向、梯度 all-reduce/finalize、`optimizer.step()`、记录 MFU 与 loss。
>
> 三段的边界很清楚：配置在入口定，拓扑在初始化定，迭代在循环里。判断实现正确看两个不变量——通信组必须在模型构造前建好；`world_size` 必须被并行度整除。

#### 4. 技术洞察

调用链与不变量：

```text
main()                     # 入口脚本
  parse_args()             # 产生 args / TransformerConfig
  initialize_megatron()    # 环境层
    init_process_group()
    initialize_model_parallel()
      RankGenerator -> create_group(NCCL/Gloo)
    set_seed / set_cuda_device
  pretrain()
    build model / optimizer / lr_scheduler
    forward_backward_func = get_forward_backward_func()
    for global_step in range(train_iters):
        loss = forward_backward_func(forward_step, ...)
        optimizer.step()
```

不变量约束：`init_process_group` ≺ `initialize_model_parallel` ≺ `model construction`；`world_size % (tp*pp*cp) == 0`。

#### 5. 图文说明

| 阶段 | 输入 | 输出/副作用 | 失败信号 |
|---|---|---|---|
| 入口脚本 | 命令行 args | `TransformerConfig`、provider 注册 | 参数冲突、配置缺失 |
| `initialize_megatron` | config | 通信组、设备、种子、全局 buffer | 整除失败、NCCL 初始化挂起 |
| 训练主循环 | 模型/优化器/调度器 | 逐步 loss、梯度、checkpoint | 不收敛、loss NaN、rank 间 loss 不一致 |

```text
args ──▶ initialize_megatron ──▶ (init_pg → model_parallel) ──▶ train loop
        [拓扑确定]                                     [迭代]
```

#### 6. 拓展知识

- 与 T02 分布式初始化交叉：`initialize_model_parallel` 内部如何用 `RankGenerator` 把 rank 映射到 (TP, PP, DP, CP, EP) 坐标。
- 与 T10 checkpoint 交叉：训练循环里 checkpoint 的触发点与 `global_step`、数据迭代位置的耦合。
- 与 T12 交付交叉：入口脚本的配置即"训练到服务"验收门禁的元数据来源。

#### 7. 常见追问及回答要点

1. **`initialize_model_parallel` 为什么必须在模型构造之前？** ——因为 TP/EP 层需要自己的并行 rank 才能确定权重切片；否则分片错位。
2. **`get_forward_backward_func` 返回什么、为什么单独抽出来？** ——它按 PP 配置返回无流水/非交错/交错 schedule 的可调用对象，把"调度策略"与"前向计算"解耦。
3. **`world_size` 不能整除模型并行度时会发生什么？** ——process group 创建阶段失败或 rank 映射越界，通常直接抛错；不会静默继续。

#### 8. 易错点与反例

- 反例：在 `initialize_model_parallel` 之前就 `build_model()`，TP 权重分片基于默认 rank（0），其余 rank 拿到错误切片，loss 异常但不报错。
- 反例：以为入口脚本负责建通信组——入口只产配置，通信组归属 `initialize_megatron`。
- 易错：把 `optimizer.step()` 放在梯度 finalize 之前，流水线下梯度未 all-reduce 完就更新，参数在不同 rank 间发散。

#### 9. 实践与验证

`[待验证]` 单卡最小复现：固定源码版本，跑 `run_simple_mcore_train_loop`（D01 demo），在 `initialize_model_parallel` 前后插入日志打印 `torch.distributed.get_rank()` 与各并行 rank；oracle 为：通信组创建成功、模型分片 shape 符合 `hidden_size / tp`、loss 单调下降。多卡与 PP 行为需在具备 NCCL 的目标环境另测。当前未执行 GPU/多卡实验。

#### 10. 评分标准

- 不合格：只能说出"先初始化再训练"，不能区分入口、环境、循环三层职责。
- 合格：能给出三阶段职责划分与主要调用顺序，指出通信组需先于模型构造。
- 良好：能列出整除约束、`forward_backward_func` 的调度选择、以及顺序颠倒的具体后果。
- 优秀/专家：能结合 `RankGenerator` 的坐标映射、全局 buffer、`finalize_model_grads` 的时机，说明多并行轴下的 rank 一致性保证。

**证据与来源：** `megatron-lm/01-modules/M02-parallelism/README.md`（`parallel_state.py`、`schedules.py` 职责与调用链，`C`）；`megatron-lm/80-demos/D01-simple-mcore-training`（入口示例，`C`）。版本锚点 Megatron-LM main @ `3703d4e33a3a2b2d11ebcc8e41f45af7ce7d1eda`；行号与符号需随源码更新复核。运行类结论标 `U / 待验证`。

### T-B002. 分布式训练里 process group 到底是什么，`initialize_model_parallel` 又建立了哪些组？

- 难度：入门级
- 知识域：T02 分布式初始化与通信组
- 考察能力：通信域建模、rank 坐标映射与不变量
- 题型：概念/调用链
- 面试频率：高频
- 前置知识：`torch.distributed`、collective 通信（all-reduce/all-gather）、rank/world_size 概念
- 关联题目：T-B001（启动生命周期）、T-B003（DP）、T-B004（TP）
- 事实状态：`[已确认]`（接口与约束）；NCCL 建立与多机行为 `[待验证]`

#### 1. 背景知识

分布式训练不是「一个程序跑在多张卡上」，而是一组**通信域**上的协同计算：

- **process group（进程组）**：`torch.distributed` 里可独立做 collective 的一个 rank 子集，每个 group 有自己的编号与通信后端。`new_group` 创建一个子集，group 内 collective 只在成员间发生（源码，`C`）。
- **global group**：`init_process_group` 建立的包含全部 rank 的默认组，`world_size` 即总进程数。
- **模型并行组**：`initialize_model_parallel` 一次建立 TP/PP/DP/CP/EP 等正交组，用 `RankGenerator` 把「各轴尺寸」按 shape/stride 展平成 rank 列表（源码，`C`，`megatron-lm/01-modules/M02-parallelism/README.md:20-31,55`）。
- **访问方式**：模型层通过 `parallel_state` 的 accessor（如 `get_tensor_model_parallel_group()`）或显式 `ProcessGroupCollection` 拿到自己该用的组，不自行推导全局 rank（源码/设计，`C`，同页 `design.md:14-19`）。

进程组是「谁能和谁通信」的声明；没有它，TP 的 all-reduce 会跨到不该涉及的 rank。

#### 2. 问题分析

回答要抓住「组 = rank 子集 + 通信域」和几条硬约束：

1. **划定边界**：`init_process_group` 建立全局环境；`initialize_model_parallel` 在其之上切出各轴子组；具体 layer 只消费组、不创建组。
2. **不变量 A**：`torch.distributed` 必须先初始化，`initialize_model_parallel` 才能建组。
3. **不变量 B**：`world_size` 必须能被模型并行规模（TP×PP×CP…）整除，否则组划分失败。
4. **不变量 C**：`RankGenerator` 的 order 必须包含所有非 1 轴；VP 只有 PP>1 时合法。
5. **边界反例**：把 TP 组当成整个 global group 来做 QKV 后的 all-reduce——TP=2、DP=2 时会把两个数据副本的激活错误地相加，loss 数值偏离但进程不报错。

#### 3. 具体答案

面试中可以这样回答：

> process group 是「可独立通信的 rank 子集」。全局组由 `init_process_group` 建立，包含所有 rank；模型并行组由 `initialize_model_parallel` 建立，它把 TP、PP、DP、CP、EP 这些正交轴交给 `RankGenerator`，按 shape/stride 生成每根轴上的 rank 列表，再逐个 `new_group`。
>
> 所以一个 rank 同时属于多个组：它在自己的 TP 组里做层内 all-reduce，在自己的 PP 组里和相邻 stage 做 P2P，在自己的 DP 组里做梯度归约。模型代码不去推全局 rank，而是通过 `parallel_state` 或 `ProcessGroupCollection` 问「我该在哪个组通信」。
>
> 两条判据：`torch.distributed` 必须先于建组；`world_size` 必须被 TP×PP×CP 整除。把这两个搞错，要么建组直接失败，要么静默算错。

#### 4. 技术洞察

组建立与使用链：

```text
torch.distributed.init_process_group()      # global group, world_size
  -> initialize_model_parallel(tp, pp, cp, ep, order)
       RankGenerator(axes) -> get_ranks(mask) -> list[rank]
       torch.distributed.new_group(ranks, backend)  # 每轴一个组
  -> parallel_state 保存 accessor
  -> layer: get_tensor_model_parallel_group()
            -> dist.all_reduce(tensor, group=that_group)
```

不变量：`init_process_group` ≺ `initialize_model_parallel` ≺ `new_group`；`world_size % (tp*pp*cp) == 0`。

#### 5. 图文说明

| 组 | 成员 | 承载通信 | 出错后果 |
|---|---|---|---|
| global | 全部 rank | 无（基准） | — |
| TP | 同一 TP 坐标的 rank | 层内 all-reduce | 跨 DP 相加 → 数值错误 |
| DP | 同一数据副本的 rank | 梯度 all-reduce/reduce-scatter | 数据重复 → 有效 batch 变小 |
| PP | 相邻 stage 的 rank | P2P 激活/梯度 | 收发错位 → hang |

```text
world = TP(2) × DP(2)，8 秩示意
rank:  0 1 | 2 3 | 4 5 | 6 7
TP组:  {0,1} {2,3} {4,5} {6,7}   <- 层内通信
DP组:  {0,4} {1,5} {2,6} {3,7}   <- 梯度通信
```

#### 6. 拓展知识

- 与 T-B004 张量并行交叉：TP 组的 rank 顺序决定 QKV 按 head 列切时谁拿到哪些 head。
- 与 T-B011 模型 forward 交叉：模型接受 `ProcessGroupCollection`，因此同一份 forward 代码可换不同拓扑。
- 与旧 API 兼容：全局 `parallel_state`（MPU）仍在，新代码倾向显式传组，这是仓库里并存两种初始化方式的根因（`I`）。

#### 7. 常见追问及回答要点

1. **为什么要这么多组，不能只有一个 global group？** ——因为不同轴的通信语义不同：TP 要层内同步激活，DP 要归约梯度，混在一起会算错；分组把每个 collective 限定在正确 rank 子集。
2. **`initialize_model_parallel` 中 order 参数干什么用？** ——决定各轴在展平 rank 时哪根优先、哪根权重最高，直接影响通信是否落在同一节点（`I`）。
3. **group 建太多会有代价吗？** ——有，每创建一个 NCCL communicator 都要初始化开销；大量小 group 会拖慢启动并占用资源（`U / 待验证`）。

#### 8. 易错点与反例

- 反例：把层内 all-reduce 的 `group` 省略，默认走 global group，TP>1 时把不同 data-parallel 副本也卷进来，数值错但不报错。
- 反例：在 `initialize_model_parallel` 之前调用 `new_group`，此时全局 rank 环境未就绪，创建失败或顺序错乱。
- 易错：以为一个 rank 只属于一个组——实际它同时属于 TP/PP/DP 等多个组，写通信时要选对组。

#### 9. 实践与验证

`[待验证]` 单机多卡最小复现：固定源码版本，TP=2/DP=2 起 4 进程，在 `initialize_model_parallel` 后打印每根轴的 `get_*_parallel_group()` 成员 rank；oracle 为：TP 组与 DP 组的 rank 集合与手算坐标一致、且每个 collective 仅落在预期成员内。多机 NCCL 建组顺序需在目标集群另测。当前未执行多卡实验。

#### 10. 评分标准

- 不合格：只会说「process group 是通信组」，说不出与并行轴的对应关系。
- 合格：能说明全局组与各轴组的关系，并给出建组顺序与整除约束。
- 良好：能画出 rank 到 (TP,DP) 的映射，指出组选错的具体数值后果。
- 优秀/专家：能谈 `RankGenerator` 的 order/shape/stride 展开、group 创建开销与拓扑排布对通信带宽的影响。

**证据与来源：** `megatron-lm/01-modules/M02-parallelism/README.md:20-31,45-59`（主链、并行轴、不变量，`C`）；`interfaces.md:15-24`（`initialize_model_parallel`/`RankGenerator` 契约，`C`）；`design.md:14-25`（拓扑与计算解耦、order 约束，`C`）。版本锚点 Megatron-LM main @ `3703d4e33a3a2b2d11ebcc8e41f45af7ce7d1eda`；行号需随源码更新复核。NCCL 建组与多机行为标 `U / 待验证`。

### T-B003. 数据并行里梯度是怎么同步的，为什么常用 all-reduce 而不是每次同步参数？

- 难度：入门级
- 知识域：T03 数据并行
- 考察能力：梯度语义、通信原语选择与 batch 关系
- 题型：原理/对比
- 面试频率：高频
- 前置知识：反向传播、梯度、all-reduce 与 reduce-scatter 区别
- 关联题目：T-B006（global batch）、T-B007（optimizer）、T-B013（梯度裁剪）
- 事实状态：`[已确认]`（机制）；实际带宽与扩展效率 `[待验证]`

#### 1. 背景知识

DP 的核心是「每个 rank 持完整模型副本，处理不同数据，然后把梯度归一化到一致」：

- **梯度归约**：每个 DP worker 处理不同 micro-batch，反向后各 worker 的梯度通过 all-reduce（或 reduce-scatter）合并，再用统一梯度更新本地副本（源码/论文，`C`，`05-megatron-architecture.md:476`）。
- **为什么同步梯度而不是参数**：同步参数需要每步广播全量权重（参数量级），同步梯度只需通信一次梯度；更新后再让各副本用同一梯度收敛到同一权重，等价但通信时机更省（`I`）。
- **distributed optimizer**：把梯度与优化器状态按 DP rank 分片，用 reduce-scatter 替代 all-reduce，避免每 rank 持有全量优化器状态（源码，`C`，同文 `:495`）。
- **梯度 bucket**：把多个参数的梯度拼成大 buffer 再通信，减少 NCCL 调用次数与 kernel launch 开销（源码/论文，`C`，同文 `:488`）。

注意 DP 只增加 batch，不减少激活；模型必须整体装进单卡（`C`）。

#### 2. 问题分析

要从「梯度语义」「通信原语」「与 batch 的关系」三点回答：

1. **划定边界**：DP 负责复制模型 + 归约梯度；参数更新本身归 T-B007 optimizer。
2. **不变量 A**：各副本用同一份梯度更新 ⇒ 各副本参数在每步后保持数值一致。
3. **不变量 B**：梯度必须按 micro-batch 数做 scale（除以 accumulation 步数），否则有效学习率随累积步数漂移。
4. **边界反例**：归约梯度前就 `optimizer.step()`——不同 rank 用不同局部梯度更新，参数立刻发散，loss 曲线缓慢变坏而不报错。

#### 3. 具体答案

面试中可以这样回答：

> DP 里每个 rank 拿完整模型、喂不同数据。一个 step 的梯度先在本地 micro-batch 上累积，累积完再在 DP 组内做 all-reduce 或 reduce-scatter，让所有 rank 拿到同一份平均梯度，然后各自用这份梯度更新参数——因为梯度一样、初始权重一样，更新后参数仍然一致。
>
> 为什么不直接同步参数？同步参数每步要广播整个模型的权重，量级是参数量；同步梯度同样能保证一致，而且能和 bucket、overlap、reduce-scatter 分片结合，把通信藏进计算。启用 distributed optimizer 时更省：梯度和优化器状态按 rank 分片，用 reduce-scatter 只保留自己负责的那片。
>
> 两条判据：梯度在 update 之前必须归约完；累积梯度要按 micro-batch 数缩放。少做任一条，参数就会在 rank 间悄悄发散。

#### 4. 技术洞察

单步 DP 同步时序：

```text
for each microbatch:
    loss.backward()                 # 梯度就地累积到 GradBuffer
grad_buffer.scale(micro_batch_count)        # 除以累积步数
reduce over DP group:
    allreduce()            # 普通 DP：每 rank 得到全量平均梯度
    或 reduce_scatter()    # distributed optimizer：每 rank 只留分片
optimizer.step()                        # 用一致梯度更新
```

经验法则：`有效 batch = micro_batch × DP × accumulation`；`world_size % (tp*pp*cp) == 0` 决定 DP 大小。

#### 5. 图文说明

| 方案 | 每步通信对象 | 通信量 | 每 rank 状态 | 备注 |
|---|---|---|---|---|
| 同步参数 | 全量权重 | ~参数量 | 全量 | 不常用 |
| 同步梯度（普通） | 梯度 | ~参数量 | 全量梯度+全量优化器状态 | 经典 |
| 同步梯度 + distributed optimizer | 梯度分片 | reduce-scatter | 分片状态 | 省显存 |

```text
rank0 ─┐ grads
rank1 ─┼── all-reduce ──▶ 相同 grads ── ▶ 各自 optimizer.step()
rank2 ─┤
rank3 ─┘
```

#### 6. 拓展知识

- 与 T-B013 交叉：归约后的梯度用来算全局 grad norm，再裁剪；裁剪必须发生在归约之后才有意义。
- 与 T-B009 交叉：distributed optimizer 的 shard 布局要写进 checkpoint，否则恢复后分片对不上。
- 与 T-B015 交叉：DP 越大通信越多，扩展效率通常略低于强基线（论文报告 8.3B/512 GPU 约 74%，非本地复现）。

#### 7. 常见追问及回答要点

1. **all-reduce 和 reduce-scatter 有什么区别、何时用哪个？** ——all-reduce 让每个 rank 都得到完整和，reduce-scatter 只把和的对应分片给每个 rank；distributed optimizer 只更新分片，所以 reduce-scatter 足够。
2. **为什么梯度要除以 micro-batch 数？** ——梯度是求和累积的，不缩放会让等效学习率随累积步数线性放大。
3. **DP 会增加显存吗？** ——不省激活、不省权重，反而因副本复制使优化器状态总量升高；省显存要靠 TP/PP/distributed optimizer。

#### 8. 易错点与反例

- 反例：在梯度 all-reduce 之前执行 `optimizer.step()`，各 rank 用局部梯度更新，参数发散。
- 反例：忘记 scale 累积梯度，（micro_batch=4 时）等效学习率放大 4 倍，训练震荡。
- 易错：把 DP 通信写成在 global group 上做，混入 TP rank，导致跨副本错误求和。

#### 9. 实践与验证

`[待验证]` 最小复现：DP=2、其余为 1，两步训练后断言两 rank 的参数逐元素相等（容忍浮点误差）；oracle 为：参数 diff≈0、loss 曲线与 DP=1 同 batch 设置近似。可再对比 all-reduce 与 reduce-scatter 的通信 trace。当前未执行多卡 GPU 实验。

#### 10. 评分标准

- 不合格：只说「DP 用 all-reduce 同步」。
- 合格：能说明同步梯度而非参数的原因，指出归约时机与缩放。
- 良好：能区分 all-reduce/reduce-scatter、说明 bucket 与 distributed optimizer 的收益。
- 优秀/专家：能结合通信重叠、扩展效率口径（weak/strong scaling）评估 DP 扩展性。

**证据与来源：** `insights/05-megatron-architecture.md:476,488,495,1012`（DP 原理、bucket、distributed optimizer，`C`）；`megatron-lm/01-modules/M02-parallelism/README.md:40`（DP 组定义，`C`）。版本锚点同上 commit；扩展性与带宽数字为论文/官方报告，非本地复现。

### T-B004. 张量并行是怎么切线性层的，为什么 MLP 要「先列切再行切」？

- 难度：入门级
- 知识域：T04 张量并行
- 考察能力：矩阵切分方向、非线性约束与 f/g 算子
- 题型：原理/推导
- 面试频率：高频
- 前置知识：矩阵乘法维度、GeLU/softmax 非线性、all-reduce
- 关联题目：T-B002（TP 组）、T-B011（forward 契约）、T-B012（activation checkpoint）
- 事实状态：`[已确认]`（切分方案与 f/g）；实际吞吐 `[待验证]`

#### 1. 背景知识

TP 把单个权重矩阵切到多张卡，关键约束来自非线性函数：

- **列切 A**：`Y = GeLU(XA)`，把 A 按列切成 `[A1,A2]`，得 `[Y1,Y2]`，GeLU 可对各分片独立施加、无中间同步（论文，`C`，`05-megatron-architecture.md:535-540`）。
- **行切为何不行（第一步）**：按 A 的行切会让 `Y = GeLU(X1A1+X2A2)`，而 `GeLU` 非线性，`GeLU(X1A1)+GeLU(X2A2) ≠ Y`，必须在 GeLU 前 all-reduce，多一个同步点（论文，`C`，同上）。
- **MLP 配方**：第一个 GEMM 列切、第二个 GEMM 行切，第二个 GEMM 的输出后做一次 all-reduce（`g`）（论文，`C`，`:540,544-554`）。
- **Attention 配方**：QKV 投影按列切（head 维），本地 attention，输出投影按行切，最后一次 all-reduce（论文，`C`，`:558-566`）。
- **f/g 算子**：`f` forward 恒等、backward all-reduce；`g` forward all-reduce、backward 恒等（论文，`C`，`:570-585`）。每层 forward 2 次、backward 2 次，合计 4 次 all-reduce，与 TP 规模 t 无关（`C`，`:589-599`）。

#### 2. 问题分析

要抓住「切分方向由非线性决定」这一主线：

1. **划定边界**：TP 切权重与激活，不切数据；DP 才切 batch。
2. **不变量 A**：所有需要跨分片求和的中间量，必须在施加非线性之前完成归约。
3. **不变量 B**：一个 Transformer 层 TP 通信次数为常数 4（不随 t 增长），体积随 t 变化。
4. **边界反例**：把第一个 GEMM 也按行切并在 GeLU 前不归约，各 rank 对局部和施加 GeLU，结果与全量 GeLU 不等，数值错但不报错。

#### 3. 具体答案

面试中可以这样回答：

> 张量并行是在一个 Transformer 层内部把 GEMM 切开。关键不是随便切，而是看非线性在哪。以 MLP 为例：第一层 `Y=GeLU(XA)` 把 A 按列切成 `[A1,A2]`，每个 rank 算自己的 `GeLU(XAi)`，因为 GeLU 是逐元素的，列切后可以本地做、不需要中间通信。第二层按行切，它的输入正好是第一层的列分片，不用重排；第二层输出是各分片的和，所以在这里做一次 all-reduce 就得到完整的 hidden。
>
> Attention 同理：QKV 投影按 head 维列切，每个 head 的计算都在同一张卡上，softmax 只在序列维做、无需通信；输出投影按行切，最后一次 all-reduce。整个层 forward 两次、backward 两次 all-reduce，一共四次，而且这个次数和 TP 大小无关。
>
> 判据是：任何要在非线性前求和的地方都必须先归约；把方向搞反，就会在 GeLU 或 softmax 两边算出不一样的结果。

#### 4. 技术洞察

MLP 切分与 f/g：

```text
X ──列切──▶ [X1 X2]               # 输入按列切
A = [A1 A2]                        # 权重按列切，h×4h 每卡 4h/t 列
Y_i = GeLU(X_i A_i)                # 本地，无通信
B = [B1; B2]                       # 权重按行切，4h×h 每卡 4h/t 行
Z = Σ B_i Y_i                      # g: all_reduce 得完整输出
```

```python
class f(torch.autograd.Function):   # forward 恒等，backward all_reduce
    def forward(ctx, x): return x
    def backward(ctx, g): dist.all_reduce(g); return g
```

#### 5. 图文说明

| 层内位置 | 切分 | 通信 | 原因 |
|---|---|---|---|
| MLP fc1 (h→4h) | 列切 | 无 | GeLU 逐元素，可本地 |
| MLP fc2 (4h→h) | 行切 | all-reduce (g) | 输出为分片和 |
| QKV proj | 列切（head 维） | 无 | 每 head 计算本地 |
| attn out proj | 行切 | all-reduce (g) | 输出为分片和 |

```text
每层通信: forward g×2 (attn+mlp) + backward f×2 = 4 次，与 t 无关
```

#### 6. 拓展知识

- 与 sequence parallel 交叉：SP 把 LN/Dropout 的激活按序列维切，使原本的 all-reduce 拆成 all-gather + reduce-scatter，带宽等价但省激活（`C`，`:698`）。
- 与 GQA/MLA 交叉：KV head 数不能被 t 整除时需要复制 KV 投影或 padding（`C/I`，`:626-631`）。
- 与 T-B012 交叉：列切带来的大激活是 recompute 的主要对象。

#### 7. 常见追问及回答要点

1. **为什么第一个 GEMM 不按行切？** ——行切会要求在 GeLU 前归约，等于在非线性前加同步点，通信更早、更难 overlap。
2. **TP 通信次数会随 t 增长吗？** ——不会，每层固定 4 次 all-reduce；增长的是每次通信的体积与 GEMM 碎片化代价。
3. **Attention 的 softmax 需要通信吗？** ——不需要，head 维切分后每个 head 完整落在一张卡，softmax 只在序列维归一化。

#### 8. 易错点与反例

- 反例：MLP 第一层按行切，GeLU 前不归约，各 rank 对局部和取 GeLU，输出与全量不一致。
- 反例：把 all-reduce 放在第二层 GEMM 之前，得到的是「先和对再乘」的错误组合。
- 易错：以为 TP 越大通信越多——次数恒定，增多的是体积和碎片化。

#### 9. 实践与验证

`[待验证]` 最小复现：同一随机权重，TP=1 与 TP=2 跑同一步，比较输出 tensor 的最大逐元素误差；oracle 为：误差在浮点容差内、每层 all-reduce 计数为 4。需在具备 NCCL 的目标环境执行。当前未运行 GPU 实验。

#### 10. 评分标准

- 不合格：只说「TP 把权重切开」。
- 合格：能说明列切/行切与非线性位置的关系，指出 4 次 all-reduce。
- 良好：能推导 f/g 的 backward 方向并解释为什么次数与 t 无关。
- 优秀/专家：能结合 SP 的带宽等价性、GQA 整除约束与通信重叠分析 TP 适用边界。

**证据与来源：** `insights/05-megatron-architecture.md:535-599,626-646,698`（MLP/Attention 切分、f/g、4 次 all-reduce、SP，`C`）。论文锚点 Megatron-LM arXiv:1909.08053；吞吐数字为论文报告。

### T-B005. 流水线并行里 microbatch 是怎么流动的，1F1B 调度解决了什么问题？

- 难度：入门级
- 知识域：T05 流水线并行与调度
- 考察能力：流水线气泡、调度策略与激活驻留
- 题型：流程/对比
- 面试频率：高频
- 前置知识：层堆叠、P2P send/recv、schedule 概念
- 关联题目：T-B006（microbatch 计算）、T-B011（模型输出契约）
- 事实状态：`[已确认]`（调度与气泡公式）；时序细节 `[推断]`、实测 `[待验证]`

#### 1. 背景知识

PP 把模型的层分成多个 stage，每个 stage 放在一个（或多个）rank 上，边界用 P2P 传递激活（前向）和梯度（反向）（论文，`C`，`05-megatron-architecture.md:722`）：

- **气泡来源**：朴素流水（整批先全部 forward 再全部 backward）气泡很大，最坏效率趋近 `1/PP`（论文，`C`，`:726`）。
- **microbatch 的作用**：把 batch 切成多个 micro-batch 并在相邻 stage 间流水化，一部分 stage 在算、另一部分在传，从而填掉气泡（`C`，`:726`）。
- **1F1B**：稳定阶段每个 rank 交替做一个 forward 和一个 backward；在飞 microbatch 数被限制在 stage 数量级，而不是总 micro-batch 数（论文，`C`，`:730`）。
- **气泡公式**：`efficiency ≈ 1 - ((PP-1)/(MB+PP-1))`；`MB=4·PP` 约 81%，`MB=8·PP` 约 90%（论文，`C`，`:745-753`）。
- **显存代价**：PP 切分权重/梯度/优化器状态，但每个 stage 的激活不减少；每个 worker 要保存所有在飞 microbatch 的激活（论文，`C`，`:722,780`）。

#### 2. 问题分析

要同时解释「为什么需要 microbatch」「1F1B 为什么省显存」：

1. **划定边界**：PP 切层，P2P 传激活/梯度；microbatch 是 schedule 层的切分单位，不改变模型数学。
2. **不变量 A**：microbatch 数 MB 必须远大于 PP，否则气泡比例高。
3. **不变量 B**：一个 forward 只能消费对应序号 microbatch 的激活，收发必须按序遍历。
4. **边界反例**：MB < PP 时流水线填不满，大量时间片空闲，吞吐退化为接近单 stage 串行，性能远低于期望但不报错。

#### 3. 具体答案

面试中可以这样回答：

> 流水线把层切成 stage，stage 之间只传激活。如果按整批先全 forward 再全 backward，除第一个和最后一个 stage，其它 stage 大部分时间在等，气泡很大，最坏效率只有 1/PP。解决办法是把 batch 切成多个 micro-batch，让相邻 stage 同时忙：上游算完一个 micro-batch 就传给下游，下游一边收一边算下一步。
>
> Megatron 用的是 1F1B 调度：稳定阶段每个 rank 交替执行一个 forward 和一个 backward。它的关键是让在飞的 micro-batch 数量被限制在 stage 数量级，而不是总 micro-batch 数——这样显存里只需要保存有限个 micro-batch 的激活。气泡比例近似 `1-(PP-1)/(MB+PP-1)`，所以 MB 要远大于 PP。
>
> 判据：MB 必须明显大于 PP；PP 省的是权重和优化器状态，不是激活，激活要靠 microbatch 数和 recompute 控制。

#### 4. 技术洞察

1F1B 伪代码（推断，基于论文描述）：

```text
for step in range(num_microbatches + num_stages - 1):
    if should_forward(step):
        forward(microbatch(step))        # warmup / 稳态
    if should_backward(step):
        backward(microbatch(step - num_stages))
        finalize_grads_if_needed()
```

气泡近似：`bubble_fraction ≈ (PP-1)/(MB+PP-1)`；在飞激活 ≤ `PP × 单 stage 激活`。

#### 5. 图文说明

PP=4、MB=8 的时序示意（`[推断]`教学图）：

```text
time ->  0  1  2  3  4  5  6  7  8  9 10 11
rank0:   F0 F1 F2 F3 B0 F4 B1 F5 B2 F6 B3 F7
rank1:   .  F0 F1 F2 F3 B0 F4 B1 F5 B2 F6 ...
rank2:   .  .  F0 F1 F2 F3 B0 F4 B1 F5 ...
rank3:   .  .  .  F0 F1 F2 F3 B0 F4 ...
```

| 区间 | 行为 | 气泡 |
|---|---|---|
| warmup | 逐 stage 首次 forward | 有，∝ PP-1 |
| 稳态 | 交替 F/B（1F1B） | 基本无 |
| cooldown | 逐 stage 收尾 backward | 有，∝ PP-1 |

#### 6. 拓展知识

- 与 interleaved/VP 交叉：把层切成更多虚拟 chunk，进一步降低单 stage 层数与在飞激活，代价是 P2P 次数增多（`C`，`:776`）。
- 与 GPipe 对比：GPipe 整批先 F 后 B，气泡更宽；1F1B 交错收发、节奏更短（`C`，`:772`）。
- 与 T-B012 crossover：在飞激活是 recompute 的主要预算对象。

#### 7. 常见追问及回答要点

1. **1F1B 和 GPipe 的区别？** ——GPipe 一个 batch 全部 forward 再全部 backward，激活驻留 Whole-batch；1F1B 交替，在飞数受限到 stage 数。
2. **气泡怎么算？** ——`(PP-1)/(MB+PP-1)`，MB 越大气泡越小，但 MB 增大也抬高在飞激活。
3. **PP 省显存吗？** ——省权重/优化器状态，不省激活；激活要另用 microbatch 与 recompute 控。

#### 8. 易错点与反例

- 反例：MB 设得比 PP 还小，流水线几乎填不满，吞吐远低但无报错。
- 反例：stage 层数不均衡（如首尾 stages 多背 embedding/output），慢 stage 拉长整个 pipeline。
- 易错：以为 PP 能省激活——实际每个在飞 microbatch 的激活都得驻留。

#### 9. 实践与验证

`[待验证]` 最小复现：PP=2、MB=4 与 PP=2、MB=16 两组，比较每 step 时间与气泡占比；oracle 为：MB 增大时吞吐上升且趋近公式预测。需多卡 + NCCL 环境；当前未执行。

#### 10. 评分标准

- 不合格：只说「PP 把层切开」。
- 合格：能解释 microbatch 与气泡，给出 1F1B 的基本行为。
- 良好：能写气泡公式、说明在飞激活受 stage 数限制。
- 优秀/专家：能比较 1F1B/GPipe/interleaved，结合 stage 负载均衡与 P2P 重叠给出调优方向。

**证据与来源：** `insights/05-megatron-architecture.md:718-797`（PP 原理、1F1B、气泡公式、显存，`C`）；其中时序图与伪代码标 `[推断]`。论文锚点 MT-NLG arXiv:2201.11990。

### T-B006. 给定 global batch、micro batch 和 DP，microbatch 数量怎么算，和 PP 有什么关系？

- 难度：入门级
- 知识域：T05 流水线并行与调度
- 考察能力：批量维度分解、整除约束与 schedule 匹配
- 题型：计算/推导
- 面试频率：高频
- 前置知识：DP/PP 概念、整除、数据切分
- 关联题目：T-B003（DP 梯度）、T-B005（PP microbatch）、T-B010（sampler）
- 事实状态：`[已确认]`（关系式）；具体配置校验 `[待验证]`

#### 1. 背景知识

训练里有三层 batch 概念，面试极易混淆：

- **global batch（全局批）**：一次参数更新跨所有 rank 消费的样本总数。
- **micro batch（微批）**：每个 DP rank 在一次 forward-backward 里处理的样本数；受单卡显存约束（`C`）。
- **microbatch count（切割数）**：每个 DP rank 在一次 step 内、为累积到目标批量要跑的 forward-backward 次数。

关系（推断/工程常识，`I`，见 `05-megatron-architecture.md:317-319,1235`）：

```text
global_batch = micro_batch × DP × num_microbatches
```

- global batch 先被 DP 切到各 rank，再在 rank 内按 PP/schedule 切成 microbatch（`C/I`，`:1235`）。
- schedule 所需的 data iterator 数量必须与所选调度类型匹配（`C`，M02 不变量）。
- “microbatch” 在不同框架里既可能指 micro batch size，也可能指切割数；回答时必须点明。

#### 2. 问题分析

要严格分清三个量并给出整除约束：

1. **划定边界**：global batch 是「逻辑批量」，micro batch 是「单卡单次物理批量」，切割数是「累积次数」。
2. **不变量 A**：`global_batch % (micro_batch × DP) == 0`，否则不能整除成整数个 microbatch。
3. **不变量 B**：切割数必须 ≥ PP-1（通常需远大于 PP）才能填满流水线。
4. **边界反例**：把 global batch 写大而 micro batch 不变、DP 不变，切割数下降；当切割数 < PP 时流水线填不满，吞吐骤降。

#### 3. 具体答案

面试中可以这样回答：

> 先把三个量分清：global batch 是这一步总共消化的样本数；micro batch 是每张卡一次 forward-backward 吃的样本数，由显存决定；切割数是每个 DP rank 在这一步里要跑多少次 forward-backward。
>
> 它们的关系是 `global_batch = micro_batch × DP × num_microbatches`。也就是 global batch 先按 DP 均分到各 rank，每个 rank 再把它切成若干 micro batch 累积。举个数：global=512、micro=8、DP=16，那么每 rank 本地批=32，切割数=4。
>
> 判据：`global_batch` 要能被 `micro_batch × DP` 整除；切割数通常要远大于 PP，否则流水线气泡大。切割数还决定梯度缩放——累积梯度要除以切割数。

#### 4. 技术洞察

计算与约束：

```text
local_batch    = global_batch / DP
num_microbatches = local_batch / micro_batch
                 = global_batch / (micro_batch × DP)

约束1: global_batch % (micro_batch × DP) == 0
约束2: num_microbatches >= PP-1   (实用上 >= 4×PP)
约束3: data iterator 数量与 schedule 匹配
```

梯度缩放：`grad /= num_microbatches`（累积不同 micro-batch 的梯度求和后归一）。

#### 5. 图文说明

| 量 | 含义 | 约束来源 |
|---|---|---|
| global batch | 一步的样本总数 | 收敛目标 |
| micro batch | 单卡单次样本数 | 显存 |
| DP | 数据副本数 | world/TP/PP |
| num_microbatches | 累积次数 | `global/(micro×DP)` |

```text
global(512) ──/DP(16)──▶ local(32) ──/micro(8)──▶ 4 个 micro-batch
```

#### 6. 拓展知识

- 与 T-B010 交叉：sampler 必须保证每个 DP rank 的 micro-batch 彼此相异且数量匹配 schedule。
- 与 T-B013 交叉：裁剪用的 grad norm 在累积缩放之后计算。
- 与 T-B015 交叉：global batch、seq 与 step 时间共同决定 tokens/s 与 MFU。

#### 7. 常见追问及回答要点

1. **micro batch 和 num_microbatches 是一回事吗？** ——不是；前者是单次样本数，后者是累积次数，量纲不同。
2. **为什么切割数要大于 PP？** ——切割数太少流水线填不满，气泡按 `(PP-1)/(MB+PP-1)` 放大。
3. **改大 global batch 会怎样？** ——单步成本上升、更新频率下降；有效学习率通常需相应调整（`I`）。

#### 8. 易错点与反例

- 反例：`global=100, micro=8, DP=4` 无法整除，实际会丢弃/补齐样本，有效 batch 与期望不符。
- 反例：把 num_microbatches 当作 microbatch size 写进公式，量纲错误。
- 易错：忘记梯度按切割数缩放，等效学习率随累积次数放大。

#### 9. 实践与验证

`[待验证]` 最小复现：固定 global batch，改 DP 与 micro batch，打印每步消费的样本数与切割数，断言等于公式；oracle 为：累计 token 数与 `global_batch × seq × steps` 一致。当前未执行运行实验。

#### 10. 评分标准

- 不合格：混淆 micro batch 与切割数。
- 合格：能写出 `global = micro × DP × 切割数` 并给出整除约束。
- 良好：能指出切割数与 PP 的气泡关系、梯度缩放。
- 优秀/专家：能结合动态 batch、packed sequence 与实际 token 数（非样本数）讨论批量的口径差异。

**证据与来源：** `insights/05-megatron-architecture.md:317-319,1235`（global→DP→microbatch 切分、schedule 匹配，`C/I`）；`megatron-lm/01-modules/M02-parallelism/README.md:51`（iterator 数量不变量，`C`）。运行数值标 `[待验证]`。

### T-B007. optimizer 在混合精度下更新哪些状态，一次 step 到底改了什么？

- 难度：入门级
- 知识域：T08 优化器与梯度更新
- 考察能力：优化器状态构成、更新时序与精度关系
- 题型：概念/流程
- 面试频率：高频
- 前置知识：SGD/Adam、动量、权重衰减、混合精度
- 关联题目：T-B003（梯度同步）、T-B008（混合精度）、T-B009（checkpoint 状态）
- 事实状态：`[已确认]`（组件与时序）；具体 kernel 选择 `[待验证]`

#### 1. 背景知识

一次 `optimizer.step()` 远不止「参数 -= lr × grad」：

- **参数组**：按参数属性构造 learning-rate、weight-decay、decoupled-LR 等组，并把并行属性传给 optimizer factory（源码，`C`，`M05/implementation.md:14-17`）。
- **状态构成**：Adam 需要参数、梯度、一阶动量、二阶动量，可能还有 fp32 master 参数，共约参数量 16–20 倍的状态（论文/工程，`C`，`05-megatron-architecture.md:1016`）。
- **混合精度**：`MixedPrecisionOptimizer` 维护低精度参数/主参数、loss scaling 与溢出处理；`FP32Optimizer` 走普通精度路径（源码，`C`，`M05/implementation.md:19-21`）。
- **典型时序**：scaler 检查 → fp32 master 准备 → 梯度 gather/reduce-scatter → Adam 更新分片 →（可选）all-gather → 记录 grad norm（`I`，同文 `:1038-1047`）。
- **scheduler 推进条件**：只有 `optimizer.step()` 报告 update successful 时才推进学习率/迭代计数（源码，`C`，`M03/implementation.md:23-25`）。

#### 2. 问题分析

要把「改了哪些状态」「何时安全」「跳过怎么办」说清：

1. **划定边界**：optimizer 负责参数与自身状态更新；梯度同步归 T-B003，checkpoint 归 T-B009。
2. **不变量 A**：只有 update successful（无 inf/nan）才推进 scheduler 与迭代计数。
3. **不变量 B**：distributed optimizer 的 shard 所有权不能与普通 model state 混淆。
4. **边界反例**：scaler 检测到 inf/nan 时仍强行 step，坏梯度写进主参数，loss 立即 NaN 且可复现。

#### 3. 具体答案

面试中可以这样回答：

> optimizer 更新的不只是参数。以 Adam 为例，每个参数配套有梯度、一阶动量、二阶动量，混合精度下还有一份 fp32 的 master 参数——这些加起来大概是参数量的十几到二十倍显存。一次 step 的顺序大致是：先看 loss scaler 有没有检测到 inf/nan，有就跳过这次更新并调整 scale；然后把梯度归约好；用 fp32 master 做 Adam 更新；如果开了 distributed optimizer，梯度按 rank 分片、用 reduce-scatter，只更新自己负责的那片；最后记录 grad norm 用于裁剪和日志。
>
> 关键点是「update successful 才推进 scheduler」——一旦这步被判为坏，学习率调度和迭代计数都不动，训练状态保持一致。distributed optimizer 的分片布局是它自己的一层状态，不能和模型参数混为一谈，checkpoint 时要一起存。

#### 4. 技术洞察

一次 step 的时序与状态：

```text
1. scaler: if inf/nan -> skip update, adjust scale
2. prepare fp32 master params (mixed precision)
3. gather grads:
     normal: all-reduce
     distributed: reduce-scatter -> 本 rank 分片
4. Adam: m = b1*m + (1-b1)*g ; v = b2*v + (1-b2)*g^2
        p = p - lr * m_hat / (sqrt(v_hat) + eps)   (对负责分片)
5. (optional) all-gather 参数分片
6. record grad_norm -> clip / log
```

状态集合：`{param, grad, m, v, fp32 master?, shard metadata}`。

#### 5. 图文说明

| 状态 | 精度 | 是否分片 | 是否入库 |
|---|---|---|---|
| 参数 | bf16/fp16 | TP/PP 切 | 是 |
| fp32 master | fp32 | 随分片 | 是 |
| 一阶/二阶动量 | fp32 | 随分片 | 是 |
| 梯度 | 低精度 | 临时 | 否 |
| grad norm | 标量 | 全局 | 日志 |

```text
grads ─▶ reduce_scatter ─▶ Adam(params_shard) ─▶ all_gather ─▶ next fwd
                        └─ master/m/v 存在同一分片
```

#### 6. 拓展知识

- 与 T-B008 交叉：fp16 需要 loss scaling，bf16 不需要但仍在 fp32 更新主参数。
- 与 T-B009 交叉：save 必须在 step 完成后读取一致的参数/优化器分片（`I`）。
- 与 T-B003 交叉：裁剪发生在梯度归约完成之后。

#### 7. 常见追问及回答要点

1. **为什么要有 fp32 master？** ——低精度累加更新会丢精度，用 fp32 存主参数、更新后再降精度，稳定 Adam。
2. **skip 一次 step 会不会损坏状态？** ——不会，前提是 scheduler 和迭代计数不推进，状态原子地保持在上一步。
3. **distributed optimizer 省什么？** ——把优化器状态按 DP rank 分片，省显存，代价是多一次 all-gather 还原参数。

#### 8. 易错点与反例

- 反例：忽略 scaler 的 inf/nan，坏梯度写进主参数，loss 立刻 NaN。
- 反例：把 distributed optimizer 分片状态当普通参数存/读，恢复后分片错位、loss 漂移。
- 易错：先 step 后归约梯度，更新基于不完整梯度，参数发散。

#### 9. 实践与验证

`[待验证]` 最小复现：DP=2 + 普通优化器与 distributed optimizer 各跑若干步，断言两种方式参数一致（容差内）；oracle 为：参数 diff≈0、优化器状态形状符合分片预期。需多卡环境；当前未执行。

#### 10. 评分标准

- 不合格：认为 optimizer 只更新参数。
- 合格：能列出 Adam 状态、混合精度主参数与 update successful 条件。
- 良好：能写出 reduce-scatter→Adam→all-gather 的 distributed 时序。
- 优秀/专家：能结合 scaler、master 分片粒度与 checkpoint 一致性讨论状态集合点。

**证据与来源：** `megatron-lm/01-modules/M05-optimizer-checkpointing/implementation.md:14-29`（参数组、混合精度、distributed、checkpoint 两层，`C`）；`M03-training-runtime/implementation.md:23-25`（step 与 scheduler 条件，`C`）；`insights/05-megatron-architecture.md:1012-1050`（状态量与时序，`I`）。

### T-B008. 混合精度训练里 BF16 和 FP16 有什么区别，为什么 BF16 不需要 loss scaling？

- 难度：入门级
- 知识域：T09 混合精度与数值稳定
- 考察能力：浮点格式、数值范围与缩放机制
- 题型：原理/对比
- 面试频率：高频
- 前置知识：IEEE 浮点（符号/指数/尾数）、下溢/上溢、梯度
- 关联题目：T-B007（optimizer 状态）、T-B013（NaN 传播）
- 事实状态：`[已确认]`（格式与方案）；FP8/硬件收益 `[待验证]`

#### 1. 背景知识

两种 16 位格式的取舍在「范围」与「精度」之间：

- **格式**：fp16 有 5 位指数、10 位尾数；bf16 有 8 位指数、7 位尾数（论文/标准，`C`，`05-megatron-architecture.md:1063`）。bf16 的动态范围与 fp32 相近，但有效数字更少。
- **fp16 的问题**：梯度往往很小，fp16 动态范围不足会让小梯度下溢成 0（`C`，同上）。
- **loss scaling**：fp16 方案在 loss 上乘一个 scale，前向后梯度被同比放大，避免下溢；更新前再除回来，scaler 监测 inf/nan 并动态调整 scale（`C`，`:1068`）。
- **bf16 方案**：范围够大，无需 loss scaling，前向/反向直接用 bf16，但参数与优化器仍以 fp32 精度更新（`C`，`:1069`）。
- **FP8**：逐 tensor scale、延迟缩放（delayed scaling），需额外传播 scale 元数据（`C/I`，`:1074-1076`）。

#### 2. 问题分析

要区分「范围」与「精度」两件事：

1. **划定边界**：混合精度决定计算与存储的精度；loss scaling 是 fp16 的下溢补丁，bf16 不需要。
2. **不变量 A**：主参数与优化器状态保持 fp32 更新。
3. **不变量 B**：fp16 下 scale 必须成对地乘（loss）与除（梯度），否则等效步长错。
4. **边界反例**：fp16 不用 loss scaling，小梯度下溢为 0，模型局部永不更新，收敛停滞但不报错。

#### 3. 具体答案

面试中可以这样回答：

> bf16 和 fp16 都是 16 位，但分配方式不同：fp16 指数 5 位、尾数 10 位，精度高但能表示的范围窄；bf16 指数 8 位、尾数 7 位，范围和 fp32 差不多但有效数字少。
>
> 训练里梯度经常非常小。fp16 范围窄，小梯度会下溢成 0，所以要 loss scaling——把 loss 乘一个大 scale，反向后梯度同比放大，落在 fp16 能表示的范围里，更新前再除回去，scaler 还会盯着有没有 inf/nan 并动态调整 scale。bf16 范围够大，不需要这套机制；但两者都保持 fp32 的主参数和优化器状态来保证更新精度。
>
> 判据：fp16 必须配 loss scaling，且乘和除成对；bf16 可以省掉这一层，但显存与算力收益取决于硬件是否支持 bf16 kernel。

#### 4. 技术洞察

数值范围对比与流程：

```text
fp16:  sign(1) exp(5)  mantissa(10)   范围窄，精度高 -> 需 loss scaling
bf16:  sign(1) exp(8)  mantissa(7)    范围宽，精度低 -> 无需 scaling
```

```text
fp16 step:
  loss' = loss * scale
  loss'.backward()            # 梯度被放大
  if scaler.has_inf_nan(): scale /= 2 ; skip
  grad /= scale               # 除回
  optimizer.step()            # fp32 master 更新
```

#### 5. 图文说明

| 维度 | fp16 | bf16 |
|---|---|---|
| 指数位 | 5 | 8 |
| 尾数位 | 10 | 7 |
| 动态范围 | 窄 | ≈fp32 |
| loss scaling | 需要 | 不需要 |
| 主参数精度 | fp32 | fp32 |

```text
范围:  fp16 ▏■■■■▏            bf16 ▏■■■■■■■■▏(≈fp32)
精度:  fp16 ▏■■■■■■■■■■▏        bf16 ▏■■■■■■■▏
```

#### 6. 拓展知识

- 与 T-B007 交叉：`MixedPrecisionOptimizer` 封装低精度参数/主参数与溢出处理。
- 与 T-B013 交叉：scaler 报 inf/nan 时跳过更新并调 scale，是 NaN 防线的第一道。
- 与 T-B004 交叉：TP 下各 rank 的 scale/统计需一致，否则梯度数值不一致（`I`）。

#### 7. 常见追问及回答要点

1. **为什么 bf16 精度低却更常用？** ——范围大、和下溢无关，训练更稳；现代硬件 bf16 吞吐也高。
2. **loss scaling 的 scale 怎么定？** ——从一个大值起步，遇 inf/nan 减半，稳定后偶尔上调，动态搜索（`I`）。
3. **FP8 和这两者差在哪？** ——scale 逐 tensor、延迟缩放，需要传播 scale 元数据，收益依赖 Hopper+ 硬件（`C/待验证`）。

#### 8. 易错点与反例

- 反例：fp16 忘记 loss scaling，小梯度下溢为 0，局部参数不更新。
- 反例：只乘不除，等效学习率被放大 scale 倍，训练发散。
- 易错：以为混合精度会把参数也存成 bf16——主参数仍是 fp32。

#### 9. 实践与验证

`[待验证]` 最小复现：同一初始化下 fp16(带 scaling) 与 bf16 各训练若干步，比较 loss 曲线与 grad norm 量级；oracle 为：两者曲线趋势接近、fp16 scaler 不频繁触发 inf/nan。需 GPU；当前未执行。

#### 10. 评分标准

- 不合格：只说「bf16 范围大」。
- 合格：能给出两种格式的指数/尾数差异与 scaling 机制。
- 良好：能说明 fp16 下溢与 scaling 成对使用、主参数 fp32。
- 优秀/专家：能延伸到 FP8 的逐 tensor scaling、scale 元数据传播与并行下的统计一致性。

**证据与来源：** `insights/05-megatron-architecture.md:1054-1083`（格式、loss scaling、bf16、FP8，`C/I`）；`megatron-lm/01-modules/M05-optimizer-checkpointing/implementation.md:19-21`（MixedPrecisionOptimizer，`C`）。FP8 与硬件收益标 `[待验证]`。

### T-B009. 训练 checkpoint 里到底保存了哪些状态，少存一样会怎样？

- 难度：入门级
- 知识域：T10 检查点、恢复与容错
- 考察能力：状态清单、恢复一致性与并行兼容
- 题型：概念/清单
- 面试频率：中高频
- 前置知识：模型/优化器状态、RNG、数据迭代
- 关联题目：T-B007（optimizer 状态）、T-B014（数据集对齐）、T-B001（恢复训练）
- 事实状态：`[已确认]`（状态清单与两层结构）；round-trip 实测 `[待验证]`

#### 1. 背景知识

可靠的恢复要覆盖「计算状态」与「控制状态」两类：

- **两层结构**：训练 checkpointing 负责命名、tracker、参数校验与全局训练状态；`dist_checkpointing` 负责 sharded mapping 的存储/加载（源码，`C`，`M05/implementation.md:27-29`）。
- **保存的集合**：save/load 同时涉及模型、optimizer、scheduler、RNG、tokenizer、数据加载状态和 tracker（源码，`C`，`M05/README.md:41`）。
- **传统 vs 分布式**：传统 checkpoint 按 iteration、TP/PP/EP rank 与 distributed optimizer 形成路径；distributed checkpoint 用 sharded state dict 与 metadata，并支持并行度变化时 reshard（源码，`C`，同页）。
- **兼容性校验**：`check_checkpoint_args` 会拒绝或警告关键结构参数不匹配（源码，`C`，`:45`）。
- **模型适配边界**：`GPTModel.sharded_state_dict` 是模型到 checkpoint 层的适配点（源码，`C`，`M01/interfaces.md:22`）。

#### 2. 问题分析

要把「存什么」「为什么每样都要」和「并行变化怎么处理」讲清：

1. **划定边界**：checkpoint 是训练状态的完整快照，不只是权重。
2. **不变量 A**：恢复后的迭代数、样本消费位置、学习率、RNG 必须与保存点严格衔接。
3. **不变量 B**：并行拓扑变化时 sharded state dict 需要 reshard 成新布局。
4. **边界反例**：只存模型权重、不存 RNG 与数据位置，恢复后 loss 出现跳变（不是模型错，而是随机流与数据顺序重置），难以定位。

#### 3. 具体答案

面试中可以这样回答：

> checkpoint 不只是权重。要能无损续训，至少要存：模型参数、optimizer 状态（含动量和 fp32 master）、学习率 scheduler 状态、RNG 状态、数据加载/迭代位置，以及 tokenizer 和 tracker 这类元数据。少了哪样，恢复后的行为都会和原来不一致。
>
> 结构上分两层：训练层负责命名、参数校验、全局训练状态；`dist_checkpointing` 负责把 sharded state dict 按分片存下来。好处是并行度可以变——比如 8 卡存的 checkpoint，换到 16 卡继续训时用 reshard 把分片重新映射。
>
> 判据：恢复后「迭代数、样本位置、学习率、随机流」要连续；`check_checkpoint_args` 会拦下模型结构或并行参数不匹配的情况。只存权重、丢掉 RNG 和数据位置，是典型的伪恢复。

#### 4. 技术洞察

保存与恢复链：

```text
save:
  save_checkpoint -> generate model/optimizer/training state
    model.sharded_state_dict()      # 模型分片
    optimizer state (m, v, master)  # 优化器
    scheduler / RNG / data iterator / tokenizer / tracker
  -> dist_checkpointing.save(sharded_state_dict, metadata)

load:
  load_checkpoint -> resolve iteration/path
    check_checkpoint_args()
    -> load 模型 + optimizer + scheduler/RNG/data state
    -> 返回 iteration 与 consumed samples
```

#### 5. 图文说明

| 状态 | 类别 | 缺失后果 |
|---|---|---|
| 模型参数 | 计算 | 从头开始，等价未训练 |
| optimizer(m,v,master) | 计算 | 动量丢失，loss 抖动 |
| scheduler | 控制 | 学习率跳变 |
| RNG | 控制 | dropout/采样流错位 |
| 数据迭代位置 | 控制 | 重复或跳过样本 |
| tokenizer/tracker | 元数据 | 校验失败或指标断裂 |

```text
[模型] + [优化器] + [调度器] + [RNG] + [数据位置] + [元数据] = 可续训快照
```

#### 6. 拓展知识

- 与 T-B014 交叉：数据位置与 tokenizer 必须配套，换了 tokenizer 的 checkpoint 无法续训。
- 与 T-B007 交叉：save 的时机应落在 optimizer.step 后的状态集合点。
- 与 T-B002 交叉：并行拓扑决定 shard 布局，reshard 在跨并行度恢复时生效。

#### 7. 常见追问及回答要点

1. **为什么 RNG 也要存？** ——dropout、数据 shuffle、采样都依赖随机流；不存则恢复后这些序列重置，loss 不一致。
2. **并行度变了能恢复吗？** ——能，前提是用 distributed checkpoint 的 sharded state dict 并在加载时 reshard。
3. **结构改了还能加载吗？** ——`check_checkpoint_args` 会拦截关键参数不匹配，通常直接拒绝或警告。

#### 8. 易错点与反例

- 反例：只存权重恢复，loss 曲线在恢复点跳变，误判为模型 bug。
- 反例：把 distributed optimizer 的分片状态按普通参数存，跨并行度恢复后分片错位。
- 易错：存了数据位置但没存 tokenizer，换了词表后编码不一致。

#### 9. 实践与验证

`[待验证]` round-trip 最小复现：训练 N 步存 checkpoint，重启加载后继续训练 M 步，与不中断训练的 loss 曲线比对；oracle 为：恢复点前后 loss 连续，无跳变。需真实数据/多卡；当前未运行 checkpoint round-trip（源码分析亦标注未测）。

#### 10. 评分标准

- 不合格：认为 checkpoint 就是模型权重。
- 合格：能列出模型/优化器/调度器/RNG/数据/tokenizer 并说明两层结构。
- 良好：能解释每种状态缺失的具体后果与 reshard 条件。
- 优秀/专家：能讨论异步保存、non-persistent checkpoint、状态集合点与一致性边界。

**证据与来源：** `megatron-lm/01-modules/M05-optimizer-checkpointing/README.md:16-45`（主链、checkpoint 集合、兼容性，`C`）；`execution-flows.md:27-52`（save/load 与接口约束，`C`）；`M01-core-models/interfaces.md:22`（`sharded_state_dict`，`C`）。round-trip 与异步语义标 `[待验证]`。

### T-B010. 数据加载阶段 sampler 决定什么，DP rank 之间怎么保证不重复样本？

- 难度：入门级
- 知识域：T12 数据管线与交付
- 考察能力：采样语义、DP 分片与可复现性
- 题型：概念/机制
- 面试频率：中高频
- 前置知识：Dataset/DataLoader、epoch、随机种子
- 关联题目：T-B006（batch 计算）、T-B014（tokenizer 对齐）、T-B009（数据位置）
- 事实状态：`[已确认]`（sampler 类型与职责）；真实数据行为 `[待验证]`

#### 1. 背景知识

数据管线把「文档存储」映射为「rank-local batch」，sampler 是关键一环：

- **sample 语义**：`GPTDataset` 用 document index、sample index、shuffle index 把文档级存储映射为训练样本；builder 按 train/valid/test split 与 blended 配置分配样本（源码，`C`，`M04/implementation.md:15-17`）。
- **sampler 家族**：普通 sampler、random sampler、hybrid CP sampler、full validation sampler 把 dataset 样本变为 rank-local batch；`build_pretraining_data_loader` 负责选择 loader（源码，`C`，`:23-25`）。
- **随机性隔离**：data loader 使用独立 seed 控制 shuffle，使不同 DP rank 的数据分片在每次 epoch 不同（`C/I`，`05-megatron-architecture.md:434-436`）。
- **DP 切分**：global batch 每次迭代先被 DP 切到各 rank、再在 rank 内按 PP 切 microbatch；sampler 必须保证各 DP rank 的 micro-batch 彼此相异且数量匹配 schedule（`C/I`，`:1235`）。
- **mask/position 可缓存**：`_get_ltor_masks_and_position_ids` 按配置创建 causal mask、loss mask 与 position ids（源码，`C`，同页 `:19-21`）。

#### 2. 问题分析

要把「sampler 决定什么」「DP 不重复」讲清：

1. **划定边界**：sampler 决定每个 rank 每步取哪些样本；tokenizer/mask 生成归 T-B014 与 dataset 层。
2. **不变量 A**：同一 step 内各 DP rank 的样本集合互不相交（否则有效 batch 缩小）。
3. **不变量 B**：一个 epoch 内样本被消费的次数、顺序由 sampler + shuffle seed 决定，可复现。
4. **边界反例**：DP rank 各自用相同 seed 且不从 rank 偏移，两个 rank 取到同一批样本，global batch 里一半重复，收敛变慢但不报错。

#### 3. 具体答案

面试中可以这样回答：

> sampler 决定「这一步每个 rank 取哪些样」。数据先被 DP 切成互不相交的分片，每个 rank 拿自己那份，再在 rank 内按 microbatch 切。如果 sampler 做错——比如各 rank 用了同样的种子且没有 rank 偏移——两个 rank 会取到同一批样本，global batch 里出现重复，有效批量打折，收敛变慢却不报错。
>
> 除了不重复，sampler 还要做两件事：一是让 shuffle 每次 epoch 不同但可复现，通常靠独立 seed；二是保证每个 rank 的 micro-batch 数量与 schedule 匹配，否则流水线会缺料。
>
> 判据：同一 step 内各 rank 的样本索引集合互不相交；同一配置、同 seed 下重跑能复现同样的样本顺序。

#### 4. 技术洞察

采样链：

```text
GPTDatasetConfig
  -> Indexed/Mock dataset
  -> GPTDataset(document_index, sample_index, shuffle_index)
  -> BlendedMegatronDatasetBuilder(split, weights)
  -> sampler(seed, DP rank/size)
  -> build_pretraining_data_loader -> DataLoader
  -> get_batch -> device batch
```

不变量：`rank 的样本集 ∩ 其它 rank = ∅`；`每 rank microbatch 数 = global/(micro×DP)`。

#### 5. 图文说明

| sampler | 用途 | 关键参数 |
|---|---|---|
| 普通 | 顺序/简单切分 | DP rank/size |
| random | epoch 随机 shuffle | seed |
| hybrid CP | 配合 context parallel | CP rank |
| full validation | 验证集不切/全量 | split |

```text
dataset(全部样本)
  ──DP 切成 N 片(互斥)──▶ rank0[0..k) rank1[k..2k) ...
      每片 ──microbatch 切──▶ 4 个 micro-batch
```

#### 6. 拓展知识

- 与 T-B006 交叉：sampler 输出数量必须等于测算的 microbatch 数。
- 与 T-B009 交叉：数据迭代位置要进 checkpoint，否则恢复后重复/跳过样本。
- 与 T-B014 交叉：tokenizer 决定样本编码，sampler 只决定「取哪个样本」。

#### 7. 常见追问及回答要点

1. **sampler 和 tokenizer 的分工？** ——sampler 选样本索引，tokenizer 把文本编码成 token；前者管顺序，后者管内容。
2. **怎么保证可复现？** ——固定 dataset 构建与 shuffle seed，且 seed 逻辑只依赖样本/epoch 而非物理 rank（`I`）。
3. **packed sequence 影响 sampler 吗？** ——packing 改变样本到 token 的映射，sampler 仍需保证跨 rank 不重复（`C/I`）。

#### 8. 易错点与反例

- 反例：各 DP rank 同 seed 无偏移，样本重复，global batch 减半。
- 反例：epoch 切换时忘记重新 shuffle/reset，每轮顺序相同，降低取样多样性。
- 易错：把「样本数」与「token 数」混为一谈，packed 数据下每样本 token 数不等。

#### 9. 实践与验证

`[待验证]` 最小复现：DP=2 跑若干步，记录各 rank 取到的样本索引，断言两集合无交集；oracle 为：并集大小 = global batch × steps。需真实或 mock 数据集；当前未运行真实数据构建（M04 标注未测）。

#### 10. 评分标准

- 不合格：认为 sampler 只是随机打乱。
- 合格：能说明 DP 切分与不重复约束。
- 良好：能指出 seed 隔离、microbatch 数量匹配与可复现条件。
- 优秀/专家：能结合 packed sequence、blended 权重与 hybrid CP sampler 讨论跨 rank 采样正确性。

**证据与来源：** `megatron-lm/01-modules/M04-data-pipeline/implementation.md:15-29`（dataset index、mask、sampler、get_batch，`C`）；`README.md:29-45`（样本语义、不变量，`C`）；`insights/05-megatron-architecture.md:434-436,1235`（seed 隔离、DP 切分，`C/I`）。

### T-B011. Transformer 的 forward 契约是什么，为什么 TP 下不能假设 logits 已经 gather？

- 难度：入门级
- 知识域：T13 模型结构与计算图
- 考察能力：输入输出契约、stage 分工与并行输出语义
- 题型：接口/流程
- 面试频率：高频
- 前置知识：embedding、attention、MLP、logits/CE loss
- 关联题目：T-B001（训练循环）、T-B004（TP）、T-B005（PP）
- 事实状态：`[已确认]`（签名与不变量）；具体 kernel 目标 `[待验证]`

#### 1. 背景知识

`GPTModel.forward` 是一条有明确契约的流水线：

- **签名与输入**：`GPTModel.forward` 接收 tokens、position_ids、attention_mask、labels 等，输出 hidden/logits/loss，且 stage、dtype、shape 必须与 config 匹配（源码，`C`，`M01/interfaces.md:21`）。
- **两段式职责**：`pre_process`/`post_process` 决定本 stage 是否拥有 embedding / output；GPTModel 据此条件创建子模块（源码，`C`，`M01/implementation.md:47`）。
- **TP 输出语义**：`parallel_output=True` 时 output logits 保持 TP 分片，调用方不能假定全词表已 gather（源码，`C`，`M01/implementation.md:48`）。
- **CE 融合**：logits 与交叉熵损失融合，先算局部 `[Y1,Y2]`，只对损失跨 rank all-reduce，通信量从 `b·s·v` 降到 `b·s`（论文，`C`，`05-megatron-architecture.md:616`）。
- **schedule 边界**：模型只输出 hidden states；stage 间收发与激活生命周期由 pipeline schedule 拥有（源码/设计，`C`，`M02/design.md:19`）。`forward_step_func` 通常返回 `(output_tensor, loss_func)`（源码，`C`，`M02/interfaces.md:24`）。

#### 2. 问题分析

要把「契约是什么」和「为什么不能假设 gather」讲清：

1. **划定边界**：forward 只做数学；microbatch 顺序、P2P、激活释放归 schedule。
2. **不变量 A**：shape/dtype/stage 必须与 config 一致（如 `hidden_size`、vocab、分片维度）。
3. **不变量 B**：`parallel_output=True` 时 logits 按词表/TP 分片，消费方需知道分片语义。
4. **边界反例**：外部代码直接对分片 logits 做全词表 softmax 或 argmax，把分片当全量，产生错误概率/错误 token，不报错。

#### 3. 具体答案

面试中可以这样回答：

> forward 的契约是：输入是 token、position、mask、label 这些张量，输出按本 stage 的职责给 hidden 或 logits，或者再算 loss；但这有几个约束——shape、dtype、以及这个 stage 是否拥有 embedding/output，都要和 config 对上。
>
> 关键一点是 TP 下 logits 默认是分片的：`parallel_output=True` 时每张卡只有一部分词表的 logits，代码不保证你拿到完整词表。这是为了省通信——Megatron 把 logits 和交叉熵融合，只在 loss 标量上跨 rank 通信，把通信量从 batch×seq×vocab 降到 batch×seq。所以外层代码不能假设 logits 已经 gather，要做全词表操作得显式 all-gather。
>
> 另外模型只管算，microbatch 的收发和激活释放由 schedule 负责，stage 之间传的也是 hidden。判据：输入 shape/dtype 匹配 config；分片 logits 交给知道分片语义的 loss 函数处理，而不是外部直接 argmax。

#### 4. 技术洞察

forward 与 loss 契约：

```text
GPTModel.forward(tokens, position_ids, mask, labels):
    if pre_process:  hidden = embed(tokens, position_ids)   # 词表列切 + allreduce
    hidden = transformer_block(hidden, mask, ...)
    if post_process:
        logits = output_layer(hidden)          # parallel_output -> 分片
        loss = cross_entropy(logits, labels)   # 融合，只通信 loss 标量
    return (output, loss_func?)                # schedule 期望 (output, loss_func)
```

不变量：`hidden` 维度 = `hidden_size / tp`（分片后）；`parallel_output=True` ⇒ logits 不 gather。

#### 5. 图文说明

| 张量 | 形状 | TP 语义 |
|---|---|---|
| tokens | `[b, s]` | 复制 |
| hidden | `[s, b, h/t]`（SP 下） | 序列/隐藏分片 |
| logits | `[b, s, v/t]` | 词表分片 |
| loss | 标量 | 跨 rank 归约 |

```text
tokens ─embed(列切)+allreduce─▶ hidden ─block×L─▶ hidden
        └─ post_process: logits(分片) ─CE融合─▶ loss(标量)
                     return (output, loss_func)
```

#### 6. 拓展知识

- 与 T-B004 交叉：embedding 词表列切后需要一次 all-reduce；输出投影行切后 all-reduce。
- 与 T-B005 交叉：非最终 stage 的 forward 只输出 hidden，不产生 logits/loss。
- 与 T-B012 交叉：block 内的大激活是 recompute 对象，但 forward 契约不变。

#### 7. 常见追问及回答要点

1. **为什么 logits 不直接 all-gather？** ——词表极大，全量通信体积是 `b·s·v`；融合 CE 后只通信 loss 标量，量级降到 `b·s`。
2. **`pre_process`/`post_process` 决定什么？** ——决定该 stage 是否创建 embedding / output 层，是 PP 下子模块条件创建的开关。
3. **forward 是否可以自己做 P2P？** ——不应；stage 间收发归 schedule，模型只输出 hidden，否则和 schedule 职责重叠。

#### 8. 易错点与反例

- 反例：外部对分片 logits 直接全词表 argmax，得到错误 token。
- 反例：非最终 stage 也去计算 loss，缺少 label/label 语义，结果无意义。
- 易错：以为 forward 输出一定是 logits——PP 下中间 stage 只输出 hidden。

#### 9. 实践与验证

`[待验证]` 最小复现：TP=1 与 TP=2 各跑一次 forward，检查 hidden 维度是否为 `h/t`、logits 是否分片、loss 是否一致（容差内）；oracle 为：TP=2 拼接分片 logits 后与 TP=1 结果接近。需 GPU/NCCL；当前未执行。

#### 10. 评分标准

- 不合格：只说「forward 算 logits」。
- 合格：能给出输入输出契约与 stage 分工。
- 良好：能解释 `parallel_output` 分片语义与 CE 融合的通信收益。
- 优秀/专家：能结合 SP 的 hidden layout、packed sequence 与 schedule 契约讨论跨并行度的一致性。

**证据与来源：** `megatron-lm/01-modules/M01-core-models/interfaces.md:16-26`（forward 签名与契约，`C`）；`implementation.md:47-49`（stage 与 parallel_output 不变量，`C`）；`M02-parallelism/interfaces.md:24`（forward_step 返回值，`C`）；`insights/05-megatron-architecture.md:616`（CE 融合，`C`）。

### T-B012. activation checkpoint（重计算）省了显存还是省了计算，怎么选重算哪些层？

- 难度：入门级
- 知识域：T11 显存、性能与吞吐
- 考察能力：重计算机制、选层策略与代价分析
- 题型：原理/权衡
- 面试频率：中高频
- 前置知识：激活、autograd、前向反向
- 关联题目：T-B005（激活驻留）、T-B004（TP 激活）、T-B015（MFU）
- 事实状态：`[已确认]`（机制与判据）；实际收益 `[待验证]`

#### 1. 背景知识

重计算用「算两遍」换「少存激活」：

- **机制**：前向时不保存中间激活，反向需要时再重新算一遍；本质是拿计算换显存（论文/工程，`C`，`05-megatron-architecture.md:1018-1020`）。
- **full recompute**：把整层的激活都重算，省显存最多，但也有 30–40% 的额外计算开销（论文，`C`，`:1028`）。
- **selective recompute**：只对「激活体积大、计算密度低」的区域（attention 的 QKᵀ、softmax、softmax dropout、V 乘）做 checkpoint，其余保留（论文，`C`，`:1020`）。
- **判据公式**：`5·a·s/h > 34` 时重算更划算（a=heads, s=seq, h=hidden）；GPT-3 约 80>34 省约 70% 激活，MT-NLG 约 64>34 省约 65%（论文，`C`，`:1022-1026`）。
- **额外 FLOPs**：selective 只增加约 2.7%（GPT-3）/1.6%（MT-NLG），远小于 full 的 30–40%（论文，`C`，`:1028`）。

#### 2. 问题分析

要区分「省显存为主」「计算代价可控」两个层面：

1. **划定边界**：重计算改变激活驻留与计算量，不改变模型数学结果。
2. **不变量 A**：重算区域必须能用保存的边界张量确定性地重建。
3. **不变量 B**：重算引入的额外 FLOPs 必须小于它换回的显存收益（否则不划算）。
4. **边界反例**：对计算密度高、激活小的层（如大 MLP 主体）做重算，显存没省多少、step 时间显著变长，得不偿失。

#### 3. 具体答案

面试中可以这样回答：

> activation checkpoint 是拿计算换显存：前向不保存某些中间激活，反向需要时再算一遍。如果整层都重算，显存最省，但要多花 30–40% 的计算；所以更常见的是 selective recompute——只重算那些「激活大、算起来便宜」的地方，典型是 attention 里的 QKᵀ、softmax、softmax dropout 和 V 乘。
>
> 判断哪些层值得重算有个近似判据：`5·a·s/h > 34`（a 是 head 数，s 是序列长，h 是 hidden），GPT-3 和 MT-NLG 都在这个区间，能省下六七成激活，而额外 FLOPs 只有百分之几。
>
> 判据：重算区域要能由保存的边界张量确定性重建；省下的激活显存要大于增加的计算代价。把计算密集但激活不大的层拿去重算，基本是净亏。

#### 4. 技术洞察

选择逻辑：

```text
if 5 * num_heads * seq_len / hidden_size > 34:
    对该区域做 selective recompute   # 省激活多、额外 FLOPs 小
else:
    保留激活, 不做 recompute
```

```text
full recompute:   激活 ↓↓, 计算 +30~40%
selective:        激活 ↓ ~65-70%, 计算 +1.6~2.7%
no recompute:     激活 = 基线, 计算 = 基线
```

#### 5. 图文说明

| 策略 | 激活显存 | 额外计算 | 适用 |
|---|---|---|---|
| 不重算 | 基线 | 基线 | 显存充裕 |
| full recompute | 最低 | 30–40% | 极度缺显存 |
| selective | 降约 65–70% | 约 2.7% | 常规推荐 |

```text
算力 ◀── selective ── 不重算 ── full ──▶ 显存节省
        (小代价大收益)              (大代价最大收益)
```

#### 6. 拓展知识

- 与 T-B004 交叉：TP 下 attention 激活本就按 head 切分，重算粒度需与 head 归属对齐。
- 与 T-B015 交叉：重算改动 step 时间，直接影响 MFU 口径。
- 与 SP 交叉：SP + selective recompute 可把激活显存降约 5 倍，并把重算时间开销大幅降低（论文报告，非本地复现）。

#### 7. 常见追问及回答要点

1. **重算会导致结果不同吗？** ——数学上等价；只要边界张量确定，重算是同值计算，浮点近似内一致。
2. **为什么 selective 只挑 attention 那几块？** ——这些块激活体积随 s、head 数增长快，而 FLOPs 相对小，重算性价比高。
3. **full recompute 的代价为什么大？** ——它把整层的稠密计算都重做一遍，额外 FLOPs 接近前向的一半量级，但省的显存也最多。

#### 8. 易错点与反例

- 反例：对计算密集的 MLP 主体全重算，显存收益有限，step 时间显著上升。
- 反例：重算区域依赖前向时丢弃的信息（未保存），重建结果与原始不一致。
- 易错：以为重算能既省显存又省计算——它一定是拿计算换显存。

#### 9. 实践与验证

`[待验证]` 最小复现：固定模型，对比「不重算 / selective / full」三组的峰值显存与 step 时间；oracle 为：selective 显存显著下降且 step 时间增幅远小于 full。需 GPU profiling；当前未测量真实激活占用与重算开销。

#### 10. 评分标准

- 不合格：认为重算省计算。
- 合格：能说明「用计算换显存」的机制与 selective/full 的区别。
- 良好：能给出 `5as/h > 34` 判据与量级。
- 优秀/专家：能结合 SP、TP head 归属与 MFU 讨论重算粒度和整体收益。

**证据与来源：** `insights/05-megatron-architecture.md:1018-1030,706`（重算机制、selective 判据、SP 协同，`C`）。论文锚点 arXiv:2205.05198；收益数字为论文报告，非本地复现。

### T-B013. 梯度裁剪是怎么做的，NaN 又是怎么在训练里传播的？

- 难度：入门级
- 知识域：T09 混合精度与数值稳定
- 考察能力：grad norm 语义、裁剪时机与 NaN 防线
- 题型：原理/排障
- 面试频率：中高频
- 前置知识：梯度范数、L2 范数、全局/局部范数、inf/nan
- 关联题目：T-B003（梯度归约）、T-B007（optimizer）、T-B008（loss scaling）
- 事实状态：`[已确认]`（机制）；具体阈值与传播路径 `[待验证]`

#### 1. 背景知识

数值稳定性有两层防线——裁剪与 NaN 检测：

- **grad norm**：`optimizer.step` 记录 grad norm，用于裁剪与日志（源码，`C`，`05-megatron-architecture.md:1047`；`M05/README.md:22`）。
- **裁剪语义**：常用的做法是「全局范数裁剪」——把所有参数的梯度当成一个大向量算 L2 范数，若超过阈值则整体缩放，使各参数方向的相对比例不变（工程常识/`I`）。
- **全局 vs 局部**：DP 下真正有意义的裁剪阈值应基于**归约后的全局梯度**；若先按局部梯度裁剪再归约，等价于对每个 rank 用不同阈值，破坏一致性（`I`）。
- **NaN 防线一**：fp16 的 loss scaler 监测 inf/nan，发现则跳过本次更新并调整 scale（`C`，`:1039-1040`）。
- **NaN 防线二**：`update successful` 只在无 inf/nan 时为真；为假时 scheduler 与迭代计数都不推进（源码，`C`，`M03/implementation.md:23-25`）。
- **典型 NaN 源**：全零 loss mask 导致除零、fp16 下溢/上溢、FP8 scale 失配、学习率过大（`I`，`:1258` 列举数值/收敛类错误）。

#### 2. 问题分析

要把「裁剪时点」「NaN 传播链」「防线位置」讲清：

1. **划定边界**：裁剪作用于归约后的梯度；NaN 检测发生在更新前与 loss 统计处。
2. **不变量 A**：裁剪阈值比较对象必须是全局（DP 归约后）grad norm。
3. **不变量 B**：只要 step 被判为不成功，参数、学习率、迭代计数都不得改动。
4. **边界反例**：一个 rank 的某个参数梯度为 NaN，all-reduce 后整组梯度都是 NaN；若 scaler 未拦截，NaN 写进主参数并在所有 rank 上永久传播。

#### 3. 具体答案

面试中可以这样回答：

> 梯度裁剪最常用的是全局范数裁剪：把所有参数的梯度拼成一个大向量算 L2 范数，超过阈值就把整体按比例缩小，这样不改变各方向的比例，只是压总步长。关键是这个范数要基于 DP 归约之后的全局梯度——如果先按每个 rank 的局部梯度裁剪再归约，各 rank 阈值不同，结果不一致。
>
> NaN 的传播是「一点污染、全局扩散」：某个 rank 某个梯度出现 NaN，all-reduce 一求和，整个 DP 组的梯度都变 NaN；如果这时还 step，NaN 就写进主参数，之后每一步都 NaN。所以有两道防线：fp16 的 scaler 检测到 inf/nan 就跳过更新并调 scale；optimizer 只有在报告 update successful 时才让 scheduler 推进。这两道拦住之后，坏步被丢弃，状态退回到上一步。
>
> 判据：裁剪在归约之后；任何 inf/nan 都不允许进入参数更新。

#### 4. 技术洞察

裁剪与 NaN 防线：

```text
# 1) 归约（DP 全局）
grads = all_reduce(grads)                     # 某 rank NaN -> 全组 NaN
# 2) 全局范数
total_norm = sqrt(sum(p.grad^2 for p in params))
clip_coef = max_norm / (total_norm + eps)
if clip_coef < 1: grads *= clip_coef           # 全局缩放
# 3) 更新前防线
if scaler.has_inf_nan(): skip; adjust_scale()
else: optimizer.step()  -> update_successful
if successful: scheduler.step()
```

#### 5. 图文说明

| 时点 | 动作 | 失败信号 |
|---|---|---|
| 归约后 | 算全局 grad norm | norm=inf/nan |
| 裁剪 | 超阈值则整体缩放 | — |
| 更新前 | scaler 查 inf/nan | 跳过 step |
| 更新后 | 校验 update successful | 不推进 scheduler |

```text
grad ─allreduce─▶ NaN? ──yes──▶ 全组 NaN ─scaler拦截─▶ skip
                     │no
                     └▶ global_norm ─clip─▶ step ─▶ params
```

#### 6. 拓展知识

- 与 T-B003 交叉：裁剪必须晚于梯度归约，否则阈值语义错。
- 与 T-B008 交叉：scaler 是 fp16 下 NaN 的第一道防线，bf16 下同样需要显式检测。
- 与 T-B009 交叉：坏步被丢弃后，从上一个好 checkpoint 恢复是止损策略。

#### 7. 常见追问及回答要点

1. **全局范数裁剪和逐参数裁剪区别？** ——全局裁剪保持方向比例、只压总步长；逐参数裁剪会改变梯度方向。
2. **一个 rank NaN 会怎样？** ——all-reduce 后整个组变 NaN，必须靠 scaler/成功标志拦截，否则永久污染。
3. **裁剪阈值怎么定？** ——一般设一个上限（如 1.0），观察 grad norm 分布调整（`I`）。

#### 8. 易错点与反例

- 反例：在 DP 归约前按局部梯度裁剪，各 rank 阈值不同，聚合后行为不可预期。
- 反例：检测到 NaN 仍 step，NaN 写入主参数，之后每步都 NaN。
- 易错：以为 bf16 不需要 NaN 检测——范围大不等于不会出 NaN（除零、坏数据仍会产生）。

#### 9. 实践与验证

`[待验证]` 最小复现：人工注入一个 NaN 到某 rank 梯度，观察是否被 scaler 拦截、参数是否保持不变、bad step 是否被跳过；oracle 为：参数与注入前一致、scheduler 未推进、日志报 skip。需多卡环境；当前未执行。

#### 10. 评分标准

- 不合格：只说「超过阈值就裁剪」。
- 合格：能说明全局范数裁剪与归约时点的关系。
- 良好：能描述 NaN 从单 rank 到全组再到参数的传播链与两道防线。
- 优秀/专家：能结合 scaler 动态策略、FP8 scale 失配与故障恢复讨论数值稳定性的系统设计。

**证据与来源：** `megatron-lm/01-modules/M05-optimizer-checkpointing/README.md:22,37`（grad norm、更新不变量，`C`）；`M03-training-runtime/implementation.md:23-25`（update successful 与 scheduler，`C`）；`insights/05-megatron-architecture.md:1038-1047,1258`（scaler、grad norm、数值错误分类，`C/I`）。

### T-B014. tokenizer 和数据集是怎么对齐的，词表和 vocab padding 为什么重要？

- 难度：入门级
- 知识域：T12 数据管线与交付
- 考察能力：编码一致性、词表结构与并行对齐
- 题型：概念/排障
- 面试频率：中频
- 前置知识：tokenizer（BPE/词表）、embedding 维度、整除
- 关联题目：T-B010（sampler）、T-B004（TP embedding 列切）、T-B009（checkpoint tokenizer）
- 事实状态：`[已确认]`（词表切分与样本语义）；真实数据行为 `[待验证]`

#### 1. 背景知识

tokenizer 与数据集必须在「词表」和「编码方式」两层对齐：

- **样本语义**：`GPTDataset.__getitem__` 把文本转为 long tensor；配置 `add_extra_token_to_sequence` 时用 `text[:-1]`/`text[1:]`，否则 labels 是左移后的 text，最后位置填 pad token（源码，`C`，`M04/README.md:31`）。
- **mask 对齐**：pad 位置从 loss mask 排除，输入中的 pad token 映射为安全 token；mask 与 position ids 可缓存（源码，`C`，同页）。
- **词表列切**：TP 输入 embedding 按词表维列切 `E=[E1,E2]`，每 rank 持一块词表子集；为了每块等大，词表要 padding 到 `N·t`（论文，`C`，`05-megatron-architecture.md:635-639`）。
- **GEMM 对齐示例**：论文为对齐把 GPT-2 词表 50,257 padding 到 128 的倍数再乘 8 路并行，得到 51,200（论文，`C`，`:110`）。
- **数据检查**：数据集要与 tokenizer 匹配、最长序列不得超过 max seq len（`C/I`，`:333`）。

#### 2. 问题分析

要把「对齐的两个层次」和「padding 为什么必要」讲清：

1. **划定边界**：tokenizer 决定 token id 与词表；dataset 决定 token 序列与 label/mask 对齐；TP 切分决定词表如何分片。
2. **不变量 A**：训练与推理、checkpoint 与续训必须使用**同一 tokenizer**，否则同一文本编码不同。
3. **不变量 B**：词表大小按 TP 切分时需 padding 到可整除，且各 rank 子块等大。
4. **边界反例**：建数据集时用 tokenizer A、训练时用 tokenizer B，token id 语义错位，模型学到噪声，loss 下降缓慢但无报错。

#### 3. 具体答案

面试中可以这样回答：

> 对齐分两层。第一层是编码：tokenizer 把文本变成 token id，数据集的 tokens 和 labels 就是根据这个 tokenizer 生成的——labels 通常是左移一位、最后用 pad 填。如果建数据集和训练用了不同 tokenizer，同一个 token id 代表的字就变了，模型等于在噪声上学，loss 不报错但收敛很差。而且 loss mask 要把 pad 位置排除，输入里的 pad 还要映射成安全 token，否则 padding 会被当真实标签算进 loss。
>
> 第二层是词表结构：TP 会把 embedding 按词表维列切，每张卡拿一块词表。为了每块大小相等、能被 TP 整除，词表通常要 padding 到 TP 的倍数——论文里 GPT-2 的词表就从 50257 padding 到 51200。padding 出来的位置是占位，不参与真实语义。
>
> 判据：训练/推理/续训用同一 tokenizer；词表大小能按 TP 整除、pad 位置从 loss 里排除。

#### 4. 技术洞察

样本构造与词表切分：

```text
text --tokenizer--> token_ids
labels: shift-left
  add_extra_token_to_sequence: text[:-1] / text[1:]
  else: labels[-1] = pad; pad 从 loss_mask 排除
```

```text
词表对齐:
  vocab 50257 --pad to 128*k--> 51200
  51200 / TP(8) = 6400 per rank       # 列切每块等大
```

#### 5. 图文说明

| 检查项 | 不一致后果 |
|---|---|
| tokenizer 版本 | token id 语义错位 |
| 词表大小 vs TP | 列切不等长，建组/分片失败 |
| pad 与 loss mask | pad 被计入 loss |
| max seq len | 样本截断/越界 |

```text
文本 ─tokenizer(A)─▶ ids ─dataset─▶ tokens/labels/mask
                              │
                    词表 pad 到 N·TP ─▶ TP 列切等大
```

#### 6. 拓展知识

- 与 T-B004 交叉：embedding 词表列切后需一次 all-reduce 汇总 token 的 embedding。
- 与 T-B009 交叉：checkpoint 里存 tokenizer，是为了续训时编码一致。
- 与 T-B011 交叉：`parallel_output` 下 logits 也是词表分片，与词表 padding 口径一致。

#### 7. 常见追问及回答要点

1. **为什么词表要 padding？** ——TP 按词表列切，只有整除才能让每卡词表子块等大；padding 是占位、不参与语义。
2. **pad token 会污染 loss 吗？** ——不会，前提是 loss mask 排除了 pad 位置；忘记排除才会污染。
3. **换 tokenizer 能续训吗？** ——不能，token id 语义变了；要续训必须沿用原 tokenizer 和词表。

#### 8. 易错点与反例

- 反例：数据集用 tokenizer A、训练用 B，token id 错位，loss 缓慢下降。
- 反例：pad 未从 loss mask 排除，模型学习预测 pad，loss 被稀释。
- 易错：词表不做 padding，TP=8 时无法等分，分片越界。

#### 9. 实践与验证

`[待验证]` 最小复现：用同一 tokenizer 建小数据集，检查 `tokens[:-1] == labels[1:]`（按配置）、pad 位置的 loss mask 为 0、词表大小 `% TP == 0`；oracle 为：三项断言通过。需真实 tokenizer/数据；当前未运行真实数据构建。

#### 10. 评分标准

- 不合格：认为 tokenizer 只影响分词。
- 合格：能说明编码对齐与 pad/mask 处理。
- 良好：能解释词表 padding 与 TP 列切的关系及不一致后果。
- 优秀/专家：能结合 packed sequence、跨 checkpoint tokenizer 一致性与 in-vocab/logits 分片讨论端到端对齐。

**证据与来源：** `megatron-lm/01-modules/M04-data-pipeline/README.md:29-45`（样本语义、mask、不变量，`C`）；`implementation.md:15-29`（dataset index、mask/position、get_batch，`C`）；`insights/05-megatron-architecture.md:110,333,635-639`（词表 padding、数据检查，`C`）。

### T-B015. MFU 是怎么算的，为什么它比「每秒样本数」更能反映训练效率？

- 难度：入门级
- 知识域：T11 显存、性能与吞吐
- 考察能力：性能指标定义、口径与扩展性度量
- 题型：计算/概念
- 面试频率：中高频
- 前置知识：FLOPs、单卡峰值算力、step 时间
- 关联题目：T-B005（气泡）、T-B012（重算）、T-B006（batch）
- 事实状态：`[已确认]`（公式）；任何具体 MFU 数字 `[待验证]`/论文报告

#### 1. 背景知识

MFU 把硬件理论峰值和实际模型吞吐挂钩：

- **定义**：`MFU = 实际模型 FLOPs / (GPU数 × 单卡峰值FLOPs × 运行时长)`（工程/论文，`C`，`05-megatron-architecture.md:1105`）。
- **step 时间分解**：`step_time ≈ compute + communication + idle`，其中 `compute = 总FLOPs/(GPU数×峰值×MFU)`，`idle = 气泡 + 未重叠等待 + kernel 启动开销`（`I`，`:1093-1097`）。
- **为什么不用样本数**：样本大小不一（packed sequence 每样本 token 数不同），样本/秒掩盖了「每个 token 做了多少有效计算」；MFU 以模型 FLOPs 为分子，跨模型规模可比（`I`）。
- **模型 FLOPs 估算**：前向约 `2 × 参数量` FLOPs/token（乘加各计一次），训练约 3 倍前向量级（`I`，`:1129`）。
- **论文报告示例**：Megatron-LM 单卡 baseline 39 TFLOPS（约峰值 30%）；530B 在 2240 A100 上 MFU 54.2%（对比启用 recompute 的 42.1%）（论文，`C`，`:1109-1110`）。均为作者报告，非本地复现。

#### 2. 问题分析

要抓住「分子口径」「分母口径」「为什么可比」：

1. **划定边界**：MFU 度量「实际模型计算占硬件峰值的比例」，不直接等价于硬件利用率（通信、气泡、重算不在分子里）。
2. **不变量 A**：分子必须是**模型** FLOPs（按参数量与 token 数估），不是硬件执行的裸 FLOPs。
3. **不变量 B**：分母必须是「GPU数 × 单卡峰值 × 墙钟时长」，峰值口径（精度、稀疏）要一致。
4. **边界反例**：把重算、通信等额外计算算进分子，MFU 会被夸大；用超出实际时长的分母或错精度峰值，数字不可比。

#### 3. 具体答案

面试中可以这样回答：

> MFU 是「模型 FLOPs 利用率」，分子是实际模型计算量，分母是 GPU 数量乘单卡峰值算力再乘墙钟时间。模型 FLOPs 一般按「前向约 2 倍参数量每 token，训练再乘一个系数」来估；分母里用实际训练耗时。
>
> 它比每秒样本数更能说明问题，因为样本大小不统一——packed sequence 下每个样本的 token 数差很多，样本/秒会把「长样本多」误判成「效率高」。MFU 的分子是模型 token 级计算量，不同长度、不同模型都落在同一口径，跨配置可比。
>
> 判据：分子只算模型 FLOPs，不把重算和通信算进去；分母用一致的峰值口径和真实时长。这样看 step_time 的构成——compute、communication、idle——就能知道时间到底花在哪。

#### 4. 技术洞察

公式与分解：

```text
训练 FLOPs/token(前向) ≈ 2 × 参数量
总 FLOPs ≈ 2 × 参数 × token数 × (前向系数, 训练≥3×)
MFU = 模型FLOPs / (GPU数 × 单卡峰值FLOPs × 时长)

step_time ≈ compute + communication + idle
compute = 总FLOPs / (GPU数 × 峰值 × MFU)
idle = bubble(PP) + 未重叠等待 + kernel 启动
```

测量口径：固定 global batch、用每 step 时间而非样本/秒、warmup≥若干轮、重复取中位数（`C/I`，`:1118-1122`）。

#### 5. 图文说明

| 指标 | 分子 | 缺点 |
|---|---|---|
| 样本/秒 | 样本数 | 样本大小不一，不可比 |
| token/秒 | token 数 | 与模型规模无关，仍不可比 |
| MFU | 模型 FLOPs | 需固定的峰值与 FLOPs 口径 |

```text
MFU = 有效模型计算 / (硬件峰值 × 时间)
       └ 不含重算/通信     └ 峰值口径需一致
```

#### 6. 拓展知识

- 与 T-B005 交叉：PP 气泡落入 idle，压低 MFU。
- 与 T-B012 交叉：重算增加实际执行 FLOPs 但不进分子，同样压低 MFU。
- 与 T-B006 交叉：batch 与 seq 决定每 step token 数与总 FLOPs。

#### 7. 常见追问及回答要点

1. **MFU 高就一定快吗？** ——不一定，它相对模型计算量；跨模型只能说「单位模型计算的硬件效率」，绝对速度还要看 tokens/s。
2. **重算会降低 MFU 吗？** ——会，重算的额外 FLOPs 不在分子里，但占用了墙钟时间。
3. **扩展效率怎么测？** ——固定 global batch（说清 weak/strong scaling）、测每 step 时间、warmup 后重复取中位数（`I`）。

#### 8. 易错点与反例

- 反例：把重算/通信 FLOPs 计入分子，MFU 被系统性夸大。
- 反例：不同用例用不同精度峰值做分母，数字不可比。
- 易错：用样本/秒比较不同序列长度的配置，得出相反结论。

#### 9. 实践与验证

`[待验证]` 最小复现：同一模型固定 global batch，记录 tokens/s 与每 step 时间，按公式估算 MFU；oracle 为：MFU 落在合理区间、且打开 PP/重算后 MFU 下降与气泡/重算占比一致。需 GPU 与 profiler；当前未测量任何本地 MFU（仓库明确不做本地复现）。

#### 10. 评分标准

- 不合格：把 MFU 等同于「GPU 利用率」。
- 合格：能写出 MFU 公式与分子分母口径。
- 良好：能解释为什么样本/秒不可比、以及 step 时间三分解。
- 优秀/专家：能结合 weak/strong scaling、气泡公式与重算开销系统性评估吞吐与扩展效率。

**证据与来源：** `insights/05-megatron-architecture.md:1087-1139`（step 时间分解、MFU 定义、扩展测量口径，`C/I`）；`megatron-lm/01-modules/M03-training-runtime/README.md:50`（`--log-interval` 打印吞吐/MFU）。所有 MFU 数字为论文/官方报告，非本地复现，标 `[待验证]`。
