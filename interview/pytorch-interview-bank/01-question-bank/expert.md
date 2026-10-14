# 专家级 PyTorch 面试题

本文件包含 `E001`–`E100`，共 100 题；统一十类题型各 10 题，GPU、通信和性能结果未经目标环境运行时标为 [待验证]。

### E001. 如何从 autograd 语义审查一个复杂训练系统的正确性边界？

- 难度：专家级
- 知识域：K02 autograd、计算图与梯度
- 考察能力：概念边界、适用条件与反例
- 题型：概念解释与辨析
- 面试频率：开放题
- 前置知识：autograd、视图/原地操作、checkpoint、分布式训练
- 关联题目：B003、I002、A002
- 事实状态：[已确认][推断]

#### 1. 背景知识

本题考察能否用 autograd 语义为一套同时使用 view、in-place、activation checkpoint、自定义 autograd、AMP、torch.compile 与分布式通信的训练系统划定正确性边界。合法性问题不能只看最终 loss：要分别定义梯度语义、状态所有权与更新语义三条边界。

#### 2. 问题分析
1. 列出所有改变 Tensor 版本或存储的操作：view/reshape、原地写、copy_、checkpoint 重算。
2. 对每个参数声明梯度契约：应与 eager 参考一致，且 .grad 为累加语义而非覆盖。
3. 检查 in-place 是否落在反向需要的张量上：若命中，version counter 与 saved tensor 冲突会抛 'modified by an inplace operation'。
4. 对 AMP、compile、DDP 分别做单变量替换，与 eager 基线逐张量比对梯度。

#### 3. 具体答案
面试中可以这样回答：

> 我会把训练系统拆成三条边界：梯度边界（grad_fn 链与 version counter）、状态边界（Module/optimizer/RNG 的所有权）、更新边界（zero_grad→backward→clip→step 的顺序）。
> 每条边界给出一个必须成立的不变量：参数梯度在关闭改动的路径上应与 eager 参考在容差内一致；对需要反向的张量做 in-place 必须导致显式报错而非静默错误。
> 可验证事实：torch.autograd.gradcheck 在 float64 下以 eps=1e-6、rtol=1.3e-6、atol=1e-5 校验解析梯度，可作为最小正确性 oracle。

#### 4. 技术洞察
不变量：反向链 g_i=Σ_j g_j·(∂f_j/∂x_i) 且 .grad 累加；version counter 单调递增，任何被反向引用的张量一旦 in-place 其版本即失配。边界判定的核心是版本一致性与所有权唯一性。

#### 5. 图文说明
| 层次 | autograd 正确性边界 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | requires_grad 状态、叶张量标志、是否原地写 | 误把非叶张量当叶、图已释放 |
| 执行与状态 | 遍历 grad_fn 链、version counter、saved tensor 所有权 | 图重复释放、原地写破坏反向、梯度断链 |
| 输出与 oracle | 比对解析梯度与数值差分、.grad 累加结果 | 梯度为零/None、误差超容差、二阶导缺失 |

图示解释：沿前向图标注每个张量的 owner、version counter 与是否被 save，再沿反向链检查访问的版本是否仍新鲜。

#### 6. 拓展知识
相邻边界：checkpoint 的重算语义、AMP 的 GradScaler 状态、compile 的 guard 与 DDP 的 bucket 顺序，都会改变梯度边界，需单独验证。

#### 7. 常见追问及回答要点
1. activation checkpoint 的重算与 in-place 结合时，哪一步先违反 version counter 契约，如何检测？
2. 自定义 autograd.Function 的 backward 返回 grad 数量与输入是否一致，不一致时 PyTorch 报什么错？
3. DDP 的梯度分桶与 compile 的图捕获顺序冲突时，如何证明梯度仍与单卡一致？

#### 8. 易错点与反例
1. 反例：用 detach 切断图后仍断言参数更新，实际 .grad 为 None 却未断言。
2. 反例：把 loss 数值接近就判定正确，忽略某个参数梯度因 in-place 冲突被静默跳过。
3. 反例：AMP 下只比较 loss，未发现 GradScaler 因 NaN 跳过 step 导致实际未更新。

#### 9. 实践与验证
验证方案：构造含 in-place 与 checkpoint 模块，对 eager/AMP/compile/DDP 四种组合，用 gradcheck 与逐参数梯度比对，并断言每个参数的 .grad 非 None；oracle：相对误差在 rtol=1e-3、atol=1e-5 内且无跳过 step。

#### 10. 评分标准
E001 评分：
- 不合格：只谈 loss 收敛，说不出梯度边界；
- 合格：能定义梯度与状态两条边界并给出一个不变量；
- 良好：能用 gradcheck 与逐参数比对给出反例和验证 oracle；
- 优秀/专家：能覆盖 AMP/compile/DDP 的交互、版本一致性与残余风险。
### E002. 分布式训练中如何定义并验证“正确收敛”，尤其面对部分失败？

- 难度：专家级
- 知识域：K07 分布式训练与通信
- 考察能力：概念边界、适用条件与反例
- 题型：概念解释与辨析
- 面试频率：开放题
- 前置知识：DDP/FSDP、collective、checkpoint、容错
- 关联题目：I004、A001、E005
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

分布式训练的“正确收敛”包含数据覆盖、梯度同步、参数更新、随机性与恢复语义，而非所有进程退出码为 0。部分 rank hang、网络错误、OOM 或 checkpoint 不一致都可能让“看起来收敛”的模型与单卡基线偏离。

#### 2. 问题分析
1. 定义收敛契约：与单卡基线的参数轨迹在给定容差内一致，且每个 epoch 的样本并集完整。
2. 检查梯度归约：DDP 是否 all_reduce(sum)/world_size，全局有效批 = 本地批×world_size×累积步。
3. 注入部分失败：让一个 rank 延迟、OOM 或退出，观察是否 hang、是否有 rank 静默跳过更新。
4. 校验恢复：从 checkpoint 重启后，RNG、optimizer、sampler 状态是否闭合。

#### 3. 具体答案
面试中可以这样回答：

> 我会把“正确收敛”定义为三个可验证子句：所有 rank 参与相同次数的更新、每步梯度等于单卡梯度按 world_size 归一后的结果、恢复后轨迹可续。
> DDP 默认 all_reduce(sum) 后除以 world_size，ring all_reduce 单步通信量约 2·(N−1)/N·S，这是判断通信是否异常的基准。
> 可验证事实：用 torchrun 起 2 进程、固定初始化，比较单卡梯度与 DDP 梯度（除以 world_size）的相对误差应 < 1e-6。

#### 4. 技术洞察
不变量：全局有效批 = 本地批×world_size×累积步；所有 rank 的集合通信调用序列必须逐一配对，否则 hang；梯度归一错误会等比改变等效学习率。

#### 5. 图文说明
| 层次 | 分布式正确收敛与部分失败 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 进程数、后端、张量是否 CUDA、参与反向的参数集合 | 后端不匹配、参数未参与、集合调用不一致 |
| 执行与状态 | 追踪 rank、bucket、通信算子与反向重叠 | hang、梯度未归约、通信未重叠 |
| 输出与 oracle | 对比单卡/DDP 梯度、通信带宽与 step 方差 | 梯度差超容差、超时、吞吐不随卡数提升 |

图示解释：把每个 rank 的计算、all_reduce、更新画成对齐的泳道，未对齐处即 hang 风险点。

#### 6. 拓展知识
相邻机制：NCCL watchdog 超时、find_unused_parameters、FSDP 的分片与混合精度通信，均改变收敛语义，需分别验证。

#### 7. 常见追问及回答要点
1. 一个 rank 因 OOM 退出而其余 rank 在 all_reduce 处挂起时，watchdog 多久触发、日志里出现什么？
2. find_unused_parameters=True 会不会掩盖真实的梯度不同步，如何检测？
3. 数据并行下每个 rank 的随机增强不同，如何证明数据覆盖完整且可复现？

#### 8. 易错点与反例
1. 反例：只核对最终 loss 曲线，未发现某 rank 少跑若干 step 导致样本覆盖不全。
2. 反例：world_size 变化后未重新标定 lr，等效学习率随卡数放大却归因于数据。
3. 反例：resume 只加载模型权重，优化器动量缺失使损失曲线在重启处跳变。

#### 9. 实践与验证
验证方案：torchrun 2/4 进程训练固定步数，逐步比对单卡与分布式梯度、记录每 rank 的 step 计数与样本哈希；注入 rank 退出观测 watchdog；oracle：梯度相对误差 < 1e-6、step 计数一致、恢复后参数差 < 1e-5。

#### 10. 评分标准
E002 评分：
- 不合格：把退出码 0 当收敛；
- 合格：能定义梯度同步与数据覆盖两个子句；
- 良好：能给出 world_size 归一、集合通信配对与恢复闭包的验证；
- 优秀/专家：能覆盖部分失败的检测、watchdog、幂等恢复与残余风险。
### E003. 如何设计 `torch.compile` 的长期演进和回退策略？

- 难度：专家级
- 知识域：K08 `torch.compile` 与编译栈
- 考察能力：概念边界、适用条件与反例
- 题型：概念解释与辨析
- 面试频率：开放题
- 前置知识：Dynamo、AOTAutograd、Inductor、动态 shape、部署
- 关联题目：A002、A004、E001
- 事实状态：[已确认][推断][建议]

#### 1. 背景知识

torch.compile 的长期演进涉及 Dynamo 捕获、guard、图分区、Inductor 代码生成与缓存；PyTorch、后端、驱动与硬件版本变化都可能改变行为。生产启用需要可观测、可灰度和可回退，而不是一次性开关。

#### 2. 问题分析
1. 标注契约面：可观测的数值一致性、重编译次数上限与断点计数；实现面允许随版本变化。
2. 建立观测：TORCH_LOGS=graph_breaks,guards,recompiles 与 torch._dynamo.explain。
3. 定义回退：从 compiled 回 eager 的开关与触发阈值（如重编译 > K 或数值偏差超容差）。
4. 灰度：按流量/模型维度放量，保留最新可回退版本。

#### 3. 具体答案
面试中可以这样回答：

> 我会把 compile 当可回退的加速层，而不是正确性来源：契约是 compiled 输出与 eager 在容差内一致、重编译率受控、断点可观测。
> guard 一旦失效会触发重编译，默认累计重编译上限约 8；动态 shape 用 mark_dynamic 或 dynamic=True 收敛。
> 可验证事实：torch._dynamo.explain(fn)(*args) 返回 graph break 数与原因，可作为演进回归的量化指标。

#### 4. 技术洞察
不变量：同一编译单元在其 guard 覆盖的输入集合上保持有效；guard 失效→重编译，超过上限→回退 eager。演进安全性等价于 guard 集合与输入分布的匹配度。

#### 5. 图文说明
| 层次 | torch.compile 演进与回退 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 函数代码、示例输入、shape/dtype/device 与 dynamic 声明 | 守卫过严、断点隐藏、动态维度未声明 |
| 执行与状态 | 追踪 Dynamo 捕获、guard 集合、子图与 Inductor 缓存 | 重编译风暴、断点回退 eager、缓存未命中 |
| 输出与 oracle | 对比 compiled/eager 数值、重编译与断点计数 | 数值不一致、加速比 < 1、重编译失控 |

图示解释：把捕获→guard→子图→缓存→运行时索引画成管线，标注版本升级会改动哪一环以及回退开关位置。

#### 6. 拓展知识
相邻面：AOTAutograd 的功能化、Inductor 的 CUDAGraphs、跨版本 torch 缓存格式，都影响迁移成本，需版本化验证。

#### 7. 常见追问及回答要点
1. 升级 PyTorch 后如何量化 compile 的行为漂移，选哪些指标做门禁？
2. 动态 shape 场景下如何避免重编译风暴并保留回退？
3. 如何证明 compiled 与 eager 数值一致，容差如何设定？

#### 8. 易错点与反例
1. 反例：只看平均加速比，忽略长尾输入触发重编译导致 p99 恶化。
2. 反例：无回退开关直接全量启用，遇到编译后端 bug 只能停机。
3. 反例：把 .item() 引入图中造成断点，却误判为后端性能问题。

#### 9. 实践与验证
验证方案：固定一组 shape 与模型，跨两个 torch 版本采集 explain 断点数、重编译次数与 compiled/eager 最大差；oracle：断点与重编译不回归、数值最大差 < 1e-4、回退开关可在 1 分钟内生效。

#### 10. 评分标准
E003 评分：
- 不合格：把 compile 当黑白开关；
- 合格：能说明 guard/重编译机制与回退需求；
- 良好：能给出观测指标、数值一致性 oracle 与灰度回退；
- 优秀/专家：能覆盖跨版本漂移、缓存迁移、长尾重编译与残余风险。
### E004. 如何建立 PyTorch 性能治理，而不是一次性优化？

- 难度：专家级
- 知识域：K09 性能、显存与算子分析
- 考察能力：概念边界、适用条件与反例
- 题型：概念解释与辨析
- 面试频率：开放题
- 前置知识：profiler、分布式、compile、成本模型
- 关联题目：I005、A003、E003
- 事实状态：[已确认][推断][建议]

#### 1. 背景知识

性能治理是把一次优化变成可持续能力：定义指标基线、回归门禁、容量模型与归因流程。roofline 给出算力/带宽上界，profiler 给出证据链；没有协议的数字不能进入决策。

#### 2. 问题分析
1. 定义基线：固定版本、负载、warmup 与分位数，采集 p50/p99、有效带宽与 MFU。
2. 归因：用 torch.profiler 与 ncu 按算子聚合 self/total CUDA time，roofline 判定 compute/memory bound。
3. 门禁：把回归阈值接入 CI，超过阈值阻断或要求归因。
4. 容量：建立显存/吞吐随规模的模型，预测扩容点。

#### 3. 具体答案
面试中可以这样回答：

> 性能治理的核心是可复现的测量协议：warmup、同步、repeat、分位数，缺一不可；异步未同步的计时会系统性偏短。
> 用 I=FLOPs/Bytes 与上界 min(P, I×BW) 判定瓶颈，再决定融合、分块还是换算法。
> 可验证事实：torch.profiler 记录 CUDA event，首次迭代含 context 初始化与自动调优，必须先 warmup 再计时。

#### 4. 技术洞察
不变量：有效带宽 = Bytes/time ≤ 峰值带宽；任何测得的有效带宽超过峰值即说明测量协议有误（未同步或 warmup 缺失）。

#### 5. 图文说明
| 层次 | PyTorch 性能治理 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 算子/规模、dtype、warmup 与 repeat、计时方式 | 未同步、未 warmup、规模误导 |
| 执行与状态 | 追踪 kernel 时长、显存曲线、launch 与同步点 | 计时偏短、碎片抖动、把 launch 开销当算力 |
| 输出与 oracle | 对比 event/profiler、roofline 定位与分位数 | 加速比不稳定、有效带宽超理论值、尾延迟超标 |

图示解释：把负载→算子剖析→roofline 定位→优化→门禁回归串成闭环，标出基线冻结点。

#### 6. 拓展知识
相邻手段：kernel 融合、CUDAGraphs、批处理与量化；每种都要回到同一测量协议比较，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 如何区分算力受限与带宽受限，需要哪些实测数据？
2. 回归门禁的阈值定多少才既灵敏又稳定，如何抑制测量噪声？
3. 如何证明优化不是把成本转移给初始化或首次迭代？

#### 8. 易错点与反例
1. 反例：用单次计时且不同步，得到虚假的 2× 加速。
2. 反例：把 profiler 自身开销算入，误判某算子为瓶颈。
3. 反例：只看平均吞吐，忽略 p99 尾延迟在批处理变化后恶化。

#### 9. 实践与验证
验证方案：对目标负载建立 warmup=10、repeat=100、取中位数的基线，接入 CI 回归门禁，并用 torch.profiler 与 torch.cuda.Event 交叉验证；oracle：两种计时偏差 < 5%，回归超阈值时门禁阻断。

#### 10. 评分标准
E004 评分：
- 不合格：只报一个加速数字，无协议；
- 合格：能给出基线协议与一个归因指标；
- 良好：能用 roofline 定位并接入回归门禁；
- 优秀/专家：能补充容量模型、测量不确定性、成本与残余风险。
### E005. 如何复盘一次“训练能跑但结果不可信”的生产事故？

- 难度：专家级
- 知识域：K12 测试、调试、故障排查与工程治理
- 考察能力：概念边界、适用条件与反例
- 题型：概念解释与辨析
- 面试频率：开放题
- 前置知识：训练循环、分布式、checkpoint、可观测性
- 关联题目：I001、I002、E002、E004
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

“训练能跑但结果不可信”通常源于静默的数值或语义缺陷：梯度被原地写破坏、AMP 跳过 step、checkpoint 状态不闭合、数据泄漏或评估口径错误。复盘要把现象、直接原因、根因、验证分层。

#### 2. 问题分析
1. 固定现场：代码版本、配置、数据版本、随机种子与日志/trace。
2. 建立对照：用同种子跑一份可信基线，逐 epoch 比对中间量（梯度范数、lr、数据哈希）。
3. 二分定位：以单变量替换训练/评估组件，找出首个偏离点。
4. 修复并加回归：把该不变量写成门禁测试。

#### 3. 具体答案
面试中可以这样回答：

> 我会先证明“结果不可信”可复现，再沿梯度、状态、数据三条链二分：哪一条的中间不变量最先偏离基线。
>
> 常见根因包括：对需反向张量原地写、GradScaler 因 NaN 跳过更新、resume 丢失优化器/RNG 状态、评估集混入训练数据。
> 可验证事实：训练 50 步保存完整 checkpoint（model+optimizer+scaler+CPU/CUDA RNG），重启续训与基线参数最大差应 < 1e-5。

#### 4. 技术洞察
不变量：给定相同版本、种子与数据顺序，训练轨迹应可逐 epoch 复现；任何不可复现的偏差都指向未闭合的状态（RNG、优化器、数据、AMP 缩放）。

#### 5. 图文说明
| 层次 | 训练结果不可信复盘 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 被测模块、参数组合、种子、容差与基线 | 容差过宽、随机未固定、基线过期 |
| 执行与状态 | 追踪 fixture 生命周期、全局状态与断言点 | 顺序相关、静默 skip、只测终值 |
| 输出与 oracle | 核对断言、失败可定位性与 flaky 统计 | 假绿、不可复现失败、回归漏检 |

图示解释：把症状→直接原因→根因→验证四层展开，标注每层的证据来源与是否可复现。

#### 6. 拓展知识
相邻排查手段：anomaly_mode、梯度范数监控、数据哈希、确定性算法开关；需先隔离再归因。

#### 7. 常见追问及回答要点
1. 如何在不停训的情况下判断是数据问题还是数值问题？
2. 哪些状态必须进入 checkpoint 才能保证续训等价？
3. 如何构造最小复现并保证修复后不会复发？

#### 8. 易错点与反例
1. 反例：直接调大 lr 或换优化器“碰运气”，掩盖真正的状态丢失。
2. 反例：把不可复现归因于硬件噪声，未检查 cudnn.benchmark 与原子操作。
3. 反例：只比对最终指标，未比对中间梯度范数与数据哈希，错过偏离点。

#### 9. 实践与验证
验证方案：用同种子跑基线与故障版本，逐步记录梯度范数、lr、数据哈希与参数；用 anomaly_mode 捕获原地写；oracle：中间量与参数在容差内一致，修复后回归测试稳定通过。

#### 10. 评分标准
E005 评分：
- 不合格：只给结论或凭感觉换配置；
- 合格：能分层描述症状与直接原因；
- 良好：能二分定位根因并给出可复现证据；
- 优秀/专家：能覆盖状态闭包、确定性、回归门禁与残余风险。
### E006. 如何设计跨版本、跨硬件的 PyTorch 训练可复现性契约？

- 难度：专家级
- 知识域：K06 AMP、Checkpoint 与可复现性
- 考察能力：概念边界、适用条件与反例
- 题型：概念解释与辨析
- 面试频率：开放题
- 前置知识：checkpoint、随机数、分布式、dtype、版本治理
- 关联题目：B010、I006、A004、E005
- 事实状态：[已确认][推断][建议]

#### 1. 背景知识

跨版本、跨硬件的可复现性契约要求明确定义“相同结果”的含义与容差：bit 级还是统计等价。涉及随机种子、确定性算子、混合精度、通信归约顺序与硬件差异，需要在契约中逐条声明。

#### 2. 问题分析
1. 定义复现等级：bit-exact、容差一致、统计等价，并给出对应容差。
2. 列出状态闭包：Python/numpy/torch CPU/CUDA RNG、optimizer、scaler、sampler、cudnn 状态。
3. 开启确定性：torch.use_deterministic_algorithms(True)、CUBLAS_WORKSPACE_CONFIG，关闭 benchmark。
4. 跨硬件验证：记录并对比不同设备上的参数轨迹与误差。

#### 3. 具体答案
面试中可以这样回答：

> 我会把可复现性写成等级契约：同一版本同一硬件追求 bit-exact；跨硬件只承诺容差一致或统计等价，并写明容差数值。
> 完整状态闭包必须含 model、optimizer、GradScaler、epoch、CPU/CUDA RNG 与 sampler；缺一即不可复现。
> 可验证事实：torch.use_deterministic_algorithms(True) 会让部分算子回退确定性实现或报错，而 cudnn.benchmark=True 的自动调优会破坏逐步复现。

#### 4. 技术洞察
不变量：复现 = 状态闭包相同 ∧ 算子确定性 ∧ 归约顺序固定。任一条件缺失时，误差下界由浮点非结合性（Δ≈ε·Σ|partial|）决定，只能放宽到统计等价。

#### 5. 图文说明
| 层次 | 可复现性契约 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | dtype、autocast 区域、scaler 配置、checkpoint 内容 | 状态不闭合、精度选择错误 |
| 执行与状态 | 追踪 scale factor、RNG、优化器状态与 nan/inf 分支 | 下溢、NaN、恢复后状态漂移 |
| 输出与 oracle | 比对续训与基线参数、scale 曲线、依赖确定性开关 | 曲线跳变、结果不可复现、梯度全零 |

图示解释：把环境（版本/硬件）、状态（RNG/优化器）、算法（确定性/归约）三轴画成复现立方体，标明各等级覆盖的顶点。

#### 6. 拓展知识
相邻机制：bf16 与 fp16 的精度差、NCCL 归约算法、cuDNN 算法选择，都会注入不确定性，需要版本化记录。

#### 7. 常见追问及回答要点
1. 跨硬件时 bit-exact 是否可行，误差下界由什么决定？
2. 哪些随机源容易被漏掉，如何在契约中强制覆盖？
3. 如何验证确定性开关没有显著拖慢训练？

#### 8. 易错点与反例
1. 反例：只固定 torch.manual_seed，未固定 numpy 与 DataLoader worker 的随机源。
2. 反例：开启 benchmark 后同输入结果不同，却归因于数据。
3. 反例：把跨硬件 bit-exact 写进契约，导致 CI 长期红。

#### 9. 实践与验证
验证方案：固定完整状态闭包训练固定步数，同机重复两次比对参数最大差；再换设备/后端比较容差；oracle：同机 bit-exact（差=0）或 < 1e-6，跨机容差一致且记录实测值。

#### 10. 评分标准
E006 评分：
- 不合格：只说固定 seed；
- 合格：能列出状态闭包的主要项；
- 良好：能定义复现等级与容差并给出验证；
- 优秀/专家：能分析跨硬件误差下界、确定性开销与契约演进。
### E007. 如何评审 PyTorch 训练平台的资源所有权和生命周期？

- 难度：专家级
- 知识域：K12 测试、调试、故障排查与工程治理
- 考察能力：概念边界、适用条件与反例
- 题型：概念解释与辨析
- 面试频率：中频
- 前置知识：Tensor 生命周期、DataLoader、分布式、checkpoint、异步执行
- 关联题目：B008、I010、A001、E002
- 事实状态：[推断][建议]

#### 1. 背景知识

训练平台的资源所有权与生命周期评审关注 GPU、显存、进程、数据句柄与 checkpoint 的归属：谁创建、谁释放、失败时如何回收。所有权不清会导致泄漏、悬垂引用与不可解释的显存增长。

#### 2. 问题分析
1. 画出资源 O(owner) 与生命周期 [create, use, release] 的责任矩阵。
2. 检查异常路径：worker 崩溃、训练中断、重试时资源是否幂等释放。
3. 区分显存所有权：caching allocator 的 reserved 与 tensor 引用计数。
4. 定义泄漏检测与回收门槛（reserved 增长、句柄数）。

#### 3. 具体答案
面试中可以这样回答：

> 我会为每类资源列 owner 与释放者，并保证异常路径与正常路径都走同一 release 逻辑；所有权唯一是核心不变量。
> 显存释放依赖引用计数归零后回到 caching allocator，故“未释放”常是 Python 侧仍持有引用的张量或闭包。
> 可验证事实：torch.cuda.memory_summary() 可区分 allocated 与 reserved，泄漏时 reserved 单调增长而 allocated 回落。

#### 4. 技术洞察
不变量：每个资源在任一时刻恰有一个逻辑 owner，且 create/release 配对；reserved − allocated 反映缓存池，持续增长说明有引用未释放或碎片累积。

#### 5. 图文说明
| 层次 | 资源所有权与生命周期 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 被测模块、参数组合、种子、容差与基线 | 容差过宽、随机未固定、基线过期 |
| 执行与状态 | 追踪 fixture 生命周期、全局状态与断言点 | 顺序相关、静默 skip、只测终值 |
| 输出与 oracle | 核对断言、失败可定位性与 flaky 统计 | 假绿、不可复现失败、回归漏检 |

图示解释：把资源类别×生命周期画成矩阵，标出每个格子的 owner、释放动作与异常处理入口。

#### 6. 拓展知识
相邻面：DataLoader worker 进程、NCCL 通信缓冲、临时文件与 checkpoint 句柄，都需纳入同一所有权模型。

#### 7. 常见追问及回答要点
1. 训练中断后如何证明所有 GPU 显存和进程都已回收？
2. 哪些资源在重试路径上最容易被重复创建导致泄漏？
3. 如何区分显存碎片与真实泄漏？

#### 8. 易错点与反例
1. 反例：只靠进程退出回收资源，遇到长驻服务时显存持续增长。
2. 反例：在异常分支跳过 release，正常路径看似无泄漏。
3. 反例：把 reserved 增长当泄漏，忽略 allocator 缓存的正常行为。

#### 9. 实践与验证
验证方案：反复创建/销毁训练任务，记录 allocated/reserved、进程数与句柄数的稳态；注入异常中断验证回收；oracle：多轮后 allocated 与句柄数回到基线，reserved 增长有上界。

#### 10. 评分标准
E007 评分：
- 不合格：只列资源清单，无所有权；
- 合格：能给出一个资源的 owner 与释放路径；
- 良好：能覆盖异常路径与泄漏检测；
- 优秀/专家：能区分碎片与泄漏、给出回收门槛与残余风险。
### E008. 如何决定一个性能优化应进入 PyTorch 上游、内部扩展还是业务代码？

- 难度：专家级
- 知识域：K11 C++/CUDA 扩展与自定义算子
- 考察能力：概念边界、适用条件与反例
- 题型：概念解释与辨析
- 面试频率：开放题
- 前置知识：自定义算子、compile、性能模型、版本治理
- 关联题目：A005、A003、E003、E004
- 事实状态：[推断][建议]

#### 1. 背景知识

决定性能优化进入 PyTorch 上游、内部扩展还是业务代码，是分层决策问题：取决于通用性、维护成本、版本耦合与风险。上游改动收益广但周期长、受兼容约束；内部扩展可控但需自维护；业务代码最快但复用性差。

#### 2. 问题分析
1. 评估通用性：改动是否对多数工作负载有价值，还是仅业务特化。
2. 评估耦合与风险：接口稳定性、回归面、是否需要改 dispatcher/后端。
3. 评估维护成本：自维护版本升级的迁移负担。
4. 用可逆性排序：优先低风险高收益，其余走影子验证。

#### 3. 具体答案
面试中可以这样回答：

> 我会用三维判据：通用性、维护成本、版本耦合。通用且需长期维护的进上游；与框架机制强耦合但业务专用的做内部扩展；一次性、业务逻辑强的留业务代码。
> 自定义算子的接入点是 torch.library / dispatcher 与 autograd 公式；上游改动需遵守 BC 与 opset 约束。
> 可验证事实：torch.utils.cpp_extension.load/load_inline 会在首次调用编译并缓存扩展，是内部扩展的最低成本路径。

#### 4. 技术洞察
决策不变量：优化的收益/维护成本比在不同层差异显著；越靠上游通用性越高但迭代越慢，越靠业务越灵活但复用越低。选择层位即选择成本曲线。

#### 5. 图文说明
| 层次 | 优化的分层落点 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 张量 shape/dtype/device、线程与 grid 配置、边界条件 | 越界、设备不匹配、dtype 未分派 |
| 执行与状态 | 追踪 kernel 启动、CUDAGuard、dispatcher 分派与归约 | 越界写、跑错设备、梯度断链 |
| 输出与 oracle | 对比参考实现、gradcheck 与 sanitizer 报告 | 误差超标、梯度为零、随机错误 |

图示解释：把候选改动放在“通用性×维护成本×版本耦合”坐标系，标出上游/内部/业务三层的典型区域与迁移路径。

#### 6. 拓展知识
相邻考虑：算子融合可写在 Inductor pass（上游）、自定义 Triton kernel（内部）或模块层组合（业务），三者可迁移但成本不同。

#### 7. 常见追问及回答要点
1. 什么信号说明内部扩展应回流上游？
2. 业务专用优化如何避免污染公共代码与测试？
3. 上游改动的 BC 成本如何评估与补偿？

#### 8. 易错点与反例
1. 反例：为单业务把特化逻辑塞进公共库，增加全量回归负担。
2. 反例：把通用优化留在业务代码，重复实现且行为不一致。
3. 反例：内部扩展缺少 gradcheck 与 sanitizer，越界错误长期潜伏。

#### 9. 实践与验证
验证方案：为三种落点各写最小原型，测量收益、维护与升级成本（跨一个 torch 版本），用影子流量验证；oracle：选择的落点在收益/成本上帕累托占优且回归面可控。

#### 10. 评分标准
E008 评分：
- 不合格：只凭偏好选层；
- 合格：能给出一个分层判据；
- 良好：能比较三层成本并给出迁移与验证计划；
- 优秀/专家：能补充上游 BC、内部维护负担与回流机制。
### E009. 如何设计从 eager 原型到生产训练/推理的 PyTorch 迁移路线？

- 难度：专家级
- 知识域：K10 序列化、导出、部署与兼容性
- 考察能力：概念边界、适用条件与反例
- 题型：概念解释与辨析
- 面试频率：中频
- 前置知识：eager、compile、checkpoint、分布式、部署
- 关联题目：A004、A010、E003、E004
- 事实状态：[推断][建议]

#### 1. 背景知识

从 eager 原型到生产迁移需要冻结输入输出契约、消除动态行为、明确精度与性能目标。eager 依赖 Python 控制流与副作用，生产需要可导出、可确定、可回退的图。

#### 2. 问题分析
1. 定义契约：图签名、dtype/shape 范围、动态维度与容差。
2. 消除动态：把数据相关控制流改写为导出友好形式，声明 dynamic_shapes。
3. 对齐数值：导出图与 eager 在目标 runtime 逐输出比对。
4. 规划回退：保留 eager 路径与影子流量比对。

#### 3. 具体答案
面试中可以这样回答：

> 迁移分三步：先冻结 eager 的输入输出契约，再用 torch.export 得到函数化图并声明动态维度，最后在目标 runtime 上与 eager 数值对齐并保留回退。
> trace 会固化控制流分支，含数据相关分支的模型应改用 torch.export 或显式条件算子。
> 可验证事实：torch.export 需要示例输入，返回含 graph_module、range_constraints 与 signature 的 ExportedProgram。

#### 4. 技术洞察
不变量：导出图必须函数化（无副作用），其 graph_signature 是跨 runtime 的契约；正确性 = 导出输出与 eager 在容差内一致，兼容性 = 签名与 opset 可被目标消费。

#### 5. 图文说明
| 层次 | eager 到生产迁移 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 模型、示例输入、dynamic 声明与目标 opset/runtime | 控制流未覆盖、动态维度缺声明 |
| 执行与状态 | 追踪图捕获、功能化改写、签名与序列化 | 分支固化、opset 缺算子、加载执行任意代码 |
| 输出与 oracle | 对比导出/eager 输出、校验签名与 checker | 数值偏差、改变维度即失效、导出被拒绝 |

图示解释：把 eager 原型→契约冻结→导出→数值对齐→影子流量→灰度替换画成阶段流水线，标出回退点。

#### 6. 拓展知识
相邻路径：TorchScript、ONNX、AOTInductor 各有覆盖度与 runtime 差异，需按目标部署栈选择。

#### 7. 常见追问及回答要点
1. 如何声明动态维度并验证改变维度无需重导？
2. 哪些 eager 行为无法导出，如何改写？
3. 如何用影子流量证明生产迁移的数值与性能都达标？

#### 8. 易错点与反例
1. 反例：用 trace 处理带分支的模型，导出的图只保留了一条路径。
2. 反例：未声明动态 shape，上线后输入维度变化导致失败。
3. 反例：只比对部分输出，遗漏辅助输出与 dtype 差异。

#### 9. 实践与验证
验证方案：对模型分别 torch.export 与 jit.trace，跑固定输入比对全部输出；改变声明为 dynamic 的维度验证无需重导；用 onnx.checker 校验；oracle：导出/eager 最大差 < 1e-5，动态维度变化可用。

#### 10. 评分标准
E009 评分：
- 不合格：直接 trace 上线；
- 合格：能说明导出契约与一处风险；
- 良好：能处理动态 shape、数值对齐与回退；
- 优秀/专家：能比较多种导出路径并给出兼容性与灰度方案。
### E010. 如何建立 PyTorch 版本升级的证据驱动流程？

- 难度：专家级
- 知识域：K12 测试、调试、故障排查与工程治理
- 考察能力：概念边界、适用条件与反例
- 题型：概念解释与辨析
- 面试频率：中频
- 前置知识：版本管理、性能基线、checkpoint、分布式和编译
- 关联题目：A002、A004、E003、E004、E006
- 事实状态：[已确认][建议][待验证]

#### 1. 背景知识

版本升级的证据驱动流程要求把“能否升级”变成可判定的门禁：基线冻结、回归指标、灰度与回退。规则改动、API 弃用与性能漂移都需要证据而非经验。

#### 2. 问题分析
1. 冻结基线：版本、模型、数据、协议与关键指标（正确性、性能、显存）。
2. 列回归项：数值一致性、重编译/断点、通信、导出兼容与弃用告警。
3. 分级门禁：阻断项与告警项分开，给出阈值。
4. 灰度与回退：按流量放量，保留可一键回退的上一个版本。

#### 3. 具体答案
面试中可以这样回答：

> 我会把升级做成证据流程：先在冻结基线上跑回归套件，正确性用容差、性能用协议分位数、兼容性用导出与弃用检查，全部留证后灰度放量并保留回退。
> 证据等级要区分 [已确认]、[推断]、[待验证]；未跑过的结论不进决策。
> 可验证事实：CI 中未设种子的随机测试与无 GPU 时静默 skip 会造成假绿，升级回归必须显式声明依赖与环境。

#### 4. 技术洞察
不变量：升级安全性 = 所有阻断项门禁通过 ∧ 灰度期指标不劣于基线 ∧ 存在可回退版本。任一缺失都只能标注 [待验证]。

#### 5. 图文说明
| 层次 | 版本升级证据流程 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 被测模块、参数组合、种子、容差与基线 | 容差过宽、随机未固定、基线过期 |
| 执行与状态 | 追踪 fixture 生命周期、全局状态与断言点 | 顺序相关、静默 skip、只测终值 |
| 输出与 oracle | 核对断言、失败可定位性与 flaky 统计 | 假绿、不可复现失败、回归漏检 |

图示解释：把基线冻结→回归套件→门禁判定→灰度→回退画成流程，标出每个门禁的指标与阈值。

#### 6. 拓展知识
相邻机制：BC-breaking 变更清单、deprecation 周期、编译缓存格式迁移，都需纳入升级检查表。

#### 7. 常见追问及回答要点
1. 哪些指标应设为阻断项，哪些只告警？
2. 灰度期如何区分升级导致的劣化与自然波动？
3. 回退需要保留哪些状态才能无缝？

#### 8. 易错点与反例
1. 反例：CI 中 GPU 用例静默 skip，升级后假绿放量。
2. 反例：只跑功能测试不跑性能协议，忽略回归。
3. 反例：把 [推断] 结论当 [已确认] 写入门禁，误判。

#### 9. 实践与验证
验证方案：用固定基线与回归套件跑新旧版本，采集正确性容差、性能分位数、编译指标与弃用清单，灰度 1%→10%→100%；oracle：阻断项全过、灰度期指标不劣化、回退可在分钟级生效。

#### 10. 评分标准
E010 评分：
- 不合格：凭经验升级，无基线；
- 合格：能给出回归套件与一个门禁；
- 良好：能分级门禁并设计灰度回退；
- 优秀/专家：能覆盖 BC 变更、证据分级、回退状态与残余风险。
### E011. 第一性原理：张量/设备

- 难度：专家级
- 知识域：K01 张量/设备
- 考察能力：变量、因果链、复杂度与容量上限
- 题型：原理推导与复杂度
- 面试频率：开放题
- 前置知识：shape、stride、dtype、device
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

本题从第一性原理审查 张量布局、视图语义与设备放置，要求把结论还原到不可再分的量与守恒关系。需要先明确的量包括：stride 与 storage_offset 构成的地址映射、view/reshape/contiguous 的复制边界、memory_format 与 channels_last、pin_memory 与 non_blocking 拷贝。torch.Tensor.transpose 返回共享存储的视图（只互换 stride，不影响 storage），而 .reshape 在不可视图化时返回副本，可用 data_ptr() 是否相同判定。原理推导题的任何结论都必须能由这些基本量之间的等式或复杂度关系推出，并给出可证伪的预言。

#### 2. 问题分析
1. 列出基本量与单位：stride 与 storage_offset 构成的地址映射、view/reshape/contiguous 的复制边界、memory_format 与 channels_last、pin_memory 与 non_blocking 拷贝、CUDA caching allocator 的块复用与碎片。
2. 写出支配等式或复杂度关系：对连续张量有 stride[i]=∏_{j>i} shape[j]；view 只在目标 shape 能由既有 stride 整除分解时成立，否则 reshape 触发复制；实数占用满足 numel·itemsize ≤ storage.nbytes − storage_offset·itemsize。
3. 推导适用边界：在什么输入规模、精度、设备或并发下该关系成立、在何处失效。
4. 给出可证伪预言：预测某个可测量（如 有效带宽 GB/s）随 size、rank 或位宽的变化趋势，并说明测量方法。

#### 3. 具体答案
面试中可以这样回答：

> 张量的本质是 (storage, size, stride, storage_offset) 四元组对一段连续内存的解释；视图与副本的区别决定是否共享存储、是否可安全原地写。设备放置是显式拓扑决策，.to() 按需搬运并可能触发隐式同步。
>
> 支配关系：对连续张量有 stride[i]=∏_{j>i} shape[j]；view 只在目标 shape 能由既有 stride 整除分解时成立，否则 reshape 触发复制；实数占用满足 numel·itemsize ≤ storage.nbytes − storage_offset·itemsize。
>
> 可验证事实：torch.Tensor.transpose 返回共享存储的视图（只互换 stride，不影响 storage），而 .reshape 在不可视图化时返回副本，可用 data_ptr() 是否相同判定。

#### 4. 技术洞察
核心不变量：对连续张量有 stride[i]=∏_{j>i} shape[j]；view 只在目标 shape 能由既有 stride 整除分解时成立，否则 reshape 触发复制；实数占用满足 numel·itemsize ≤ storage.nbytes − storage_offset·itemsize。 推导题的难点是识别等式成立的前提；一旦去掉任一前提（例如 expand 产生的零 stride 张量可视图化但不能原位写；转置后的张量不可直接 view，必须先 contiguous()。），原关系不再成立，必须重新推导。

#### 5. 图文说明
| 层次 | 张量布局、视图语义与设备放置 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 张量、目标 shape/stride、device/dtype 与 memory_format | 约束缺失会误判视图可行性或越界读 |
| 执行与状态 | 追踪 storage、stride、offset 与 allocator 块的 owner 和生命周期 | 共享存储被原地写、碎片泄漏或隐式同步 |
| 输出与 oracle | 对照 data_ptr、is_contiguous 与显存曲线 | 把副本误当视图、峰值显存与理论值严重偏离 |

图示解释：围绕 张量布局、视图语义与设备放置 把基本量、支配关系与可测量串成一条链，任一环缺失都无法从原理推出结论。

#### 6. 拓展知识
相邻知识点：tensor.stride()/is_contiguous()/storage_offset()/data_ptr()、torch.cuda.memory_summary()/memory_snapshot()；比较不同精度/规模下的替代实现与误差，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 把 stride 与 storage_offset 构成的地址映射 的规模放大两个数量级，等式中的哪一项先成为主导，如何用 有效带宽 GB/s 量化？
2. 用什么实验能证伪该支配关系，例如改变 view/reshape/contiguous 的复制边界 后预测哪个指标如何变化？
3. 该关系依赖哪些前提（精度、设备、并发），去掉某一前提后结论如何改写？

#### 8. 易错点与反例
1. 反例：把 张量布局、视图语义与设备放置 的结论当作与规模无关的定律，忽略在极端参数下 对非连续张量调用 view 抛 RuntimeError: view size is not compatible with input tensor's size and stride。
2. 反例：用最终数值一致替代推导，忽略 在未同步的跨界 .to() 后立即读数据得到脏值。
3. 反例：只验证一个参数点就外推，遇到 pin_memory 张量长期驻留耗尽锁页内存 时结论崩塌。

#### 9. 实践与验证
验证方案：对同一逻辑张量构造 contiguous、转置、expand 三种布局，断言 is_contiguous/stride/storage_offset，并用 data_ptr 判定 reshape 是否复制。 用 tensor.stride()/is_contiguous()/storage_offset()/data_ptr() 与 PYTORCH_CUDA_ALLOC_CONF 环境变量 采集 有效带宽 GB/s、碎片率 reserved/allocated，把实测与 对连续张量有 stride[i]=∏_{j>i} shape[j] 的理论值对比；oracle：data_ptr 相同判为视图、不同判为副本；峰值显存与理论 numel·itemsize 的偏差落在分配器块对齐范围内。

#### 10. 评分标准
E011 评分：
- 不合格：只会复述术语，说不出支配等式与适用条件；
- 合格：能写出核心关系并说明 stride 与 storage_offset 构成的地址映射 与 view/reshape/contiguous 的复制边界 的因果；
- 良好：能给出边界反例（对非连续张量调用 view 抛 RuntimeError: view size is not compatible with input tensor's size and stride）、推导步骤与验证 oracle（data_ptr 相同判为视图、不同判为副本；峰值显存与理论 numel·itemsize 的偏差落在分配器块对齐范围内。）；
- 优秀/专家：能补充误差模型、复杂度上界、跨精度/规模外推与残余风险。
### E012. 第一性原理：autograd

- 难度：专家级
- 知识域：K02 autograd
- 考察能力：变量、因果链、复杂度与容量上限
- 题型：原理推导与复杂度
- 面试频率：开放题
- 前置知识：requires_grad、leaf、view、detach、反向
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

本题从第一性原理审查 动态计算图、梯度累积与高阶微分，要求把结论还原到不可再分的量与守恒关系。需要先明确的量包括：动态图与 grad_fn 链、叶张量与 .grad 累加、retain_graph/create_graph 生命周期、in-place 的 version counter。torch.autograd.gradcheck 在 float64 下用有限差分（默认 eps=1e-6、rtol=1.3e-6、atol=1e-5）校验解析梯度，要求输入 requires_grad=True 且函数确定性。原理推导题的任何结论都必须能由这些基本量之间的等式或复杂度关系推出，并给出可证伪的预言。

#### 2. 问题分析
1. 列出基本量与单位：动态图与 grad_fn 链、叶张量与 .grad 累加、retain_graph/create_graph 生命周期、in-place 的 version counter、saved tensor 与重算。
2. 写出支配等式或复杂度关系：反向传播沿 grad_fn 链执行链式法则 g_i=Σ_j g_j·(∂f_j/∂x_i)，梯度以累加方式写入 .grad（不清零则叠加）；对需要梯度的张量做 in-place 会使其 version counter 加一，若该版本被 backward 引用则抛 RuntimeError: a variable needed for gradient computation has been modified by an inplace operation。
3. 推导适用边界：在什么输入规模、精度、设备或并发下该关系成立、在何处失效。
4. 给出可证伪预言：预测某个可测量（如 梯度相对误差）随 size、rank 或位宽的变化趋势，并说明测量方法。

#### 3. 具体答案
面试中可以这样回答：

> autograd 记录前向操作序列构成的动态图，反向时按逆序调用 grad_fn，用链式法则把上游梯度乘上局部雅可比后累加到叶张量。图的生命周期由 retain_graph 控制，原地写通过 version counter 保证数值一致。
>
> 支配关系：反向传播沿 grad_fn 链执行链式法则 g_i=Σ_j g_j·(∂f_j/∂x_i)，梯度以累加方式写入 .grad（不清零则叠加）；对需要梯度的张量做 in-place 会使其 version counter 加一，若该版本被 backward 引用则抛 RuntimeError: a variable needed for gradient computation has been modified by an inplace operation。
>
> 可验证事实：torch.autograd.gradcheck 在 float64 下用有限差分（默认 eps=1e-6、rtol=1.3e-6、atol=1e-5）校验解析梯度，要求输入 requires_grad=True 且函数确定性。

#### 4. 技术洞察
核心不变量：反向传播沿 grad_fn 链执行链式法则 g_i=Σ_j g_j·(∂f_j/∂x_i)，梯度以累加方式写入 .grad（不清零则叠加）；对需要梯度的张量做 in-place 会使其 version counter 加一，若该版本被 backward 引用则抛 RuntimeError: a variable needed for gradient computation has been modified by an inplace operation。 推导题的难点是识别等式成立的前提；一旦去掉任一前提（例如 对同一张量第二次 backward 前必须 retain_graph=True；参数更新后再引用旧图会因版本不一致报错。），原关系不再成立，必须重新推导。

#### 5. 图文说明
| 层次 | 动态计算图、梯度累积与高阶微分 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | requires_grad 状态、叶张量标志、是否原地写 | 误把非叶张量当叶、图已释放 |
| 执行与状态 | 遍历 grad_fn 链、version counter、saved tensor 所有权 | 图重复释放、原地写破坏反向、梯度断链 |
| 输出与 oracle | 比对解析梯度与数值差分、.grad 累加结果 | 梯度为零/None、误差超容差、二阶导缺失 |

图示解释：围绕 动态计算图、梯度累积与高阶微分 把基本量、支配关系与可测量串成一条链，任一环缺失都无法从原理推出结论。

#### 6. 拓展知识
相邻知识点：tensor.grad_fn 与 next_functions 遍历、torch.autograd.anomaly_mode/set_detect_anomaly；比较不同精度/规模下的替代实现与误差，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 把 动态图与 grad_fn 链 的规模放大两个数量级，等式中的哪一项先成为主导，如何用 梯度相对误差 量化？
2. 用什么实验能证伪该支配关系，例如改变 叶张量与 .grad 累加 后预测哪个指标如何变化？
3. 该关系依赖哪些前提（精度、设备、并发），去掉某一前提后结论如何改写？

#### 8. 易错点与反例
1. 反例：把 动态计算图、梯度累积与高阶微分 的结论当作与规模无关的定律，忽略在极端参数下 detach 后仍期望梯度回传导致叶张量 .grad 为 None。
2. 反例：用最终数值一致替代推导，忽略 第二次 backward 未设 retain_graph=True 触发 Trying to backward through the graph a second time。
3. 反例：只验证一个参数点就外推，遇到 对参与反向的张量原地写触发 version counter 报错 时结论崩塌。

#### 9. 实践与验证
验证方案：构造 y=x·sin(x)，用 torch.autograd.grad 求一阶导并与有限差分对比；再开 create_graph=True 求二阶导，验证与解析式 cos(x)−x·sin(x) 一致。 用 tensor.grad_fn 与 next_functions 遍历 与 torch.autograd.gradcheck/gradgradcheck 采集 梯度相对误差、计算图节点数，把实测与 反向传播沿 grad_fn 链执行链式法则 g_i=Σ_j g_j·(∂f_j/∂x_i)，梯度以累加方式写入 .grad（不清零则叠加） 的理论值对比；oracle：gradcheck 返回 True（误差在 rtol=1e-3/atol=1e-5 内），二阶导相对误差 < 1e-4。

#### 10. 评分标准
E012 评分：
- 不合格：只会复述术语，说不出支配等式与适用条件；
- 合格：能写出核心关系并说明 动态图与 grad_fn 链 与 叶张量与 .grad 累加 的因果；
- 良好：能给出边界反例（detach 后仍期望梯度回传导致叶张量 .grad 为 None）、推导步骤与验证 oracle（gradcheck 返回 True（误差在 rtol=1e-3/atol=1e-5 内），二阶导相对误差 < 1e-4。）；
- 优秀/专家：能补充误差模型、复杂度上界、跨精度/规模外推与残余风险。
### E013. 第一性原理：Module/状态

- 难度：专家级
- 知识域：K03 Module/状态
- 考察能力：变量、因果链、复杂度与容量上限
- 题型：原理推导与复杂度
- 面试频率：开放题
- 前置知识：参数、buffer、模式和 state_dict
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

本题从第一性原理审查 模块树、参数字典与状态持久化，要求把结论还原到不可再分的量与守恒关系。需要先明确的量包括：Module 树与命名参数、parameter 与 buffer 的区别、state_dict 键空间、train/eval 模式。model.eval() 只切换 dropout/BatchNorm 等模块的 training 标志，不关闭 autograd；省显存必须叠加 torch.no_grad() 或 inference_mode()。原理推导题的任何结论都必须能由这些基本量之间的等式或复杂度关系推出，并给出可证伪的预言。

#### 2. 问题分析
1. 列出基本量与单位：Module 树与命名参数、parameter 与 buffer 的区别、state_dict 键空间、train/eval 模式、hook 与 forward 拦截。
2. 写出支配等式或复杂度关系：state_dict 的键等于模块路径拼接参数/缓冲区名（如 layer1.0.conv.weight）；load_state_dict(strict=True) 要求键集合完全一致，否则报 missing/unexpected keys；buffer 不参与 optimizer 更新但会被 .to() 搬运。
3. 推导适用边界：在什么输入规模、精度、设备或并发下该关系成立、在何处失效。
4. 给出可证伪预言：预测某个可测量（如 参数量）随 size、rank 或位宽的变化趋势，并说明测量方法。

#### 3. 具体答案
面试中可以这样回答：

> nn.Module 是一棵注册树，参数与缓冲区经命名空间形成扁平键空间；state_dict 是这棵树的快照契约，.to() 递归搬运所有注册张量。模式切换与注册完整性共同决定训练/推理一致性。
>
> 支配关系：state_dict 的键等于模块路径拼接参数/缓冲区名（如 layer1.0.conv.weight）；load_state_dict(strict=True) 要求键集合完全一致，否则报 missing/unexpected keys；buffer 不参与 optimizer 更新但会被 .to() 搬运。
>
> 可验证事实：model.eval() 只切换 dropout/BatchNorm 等模块的 training 标志，不关闭 autograd；省显存必须叠加 torch.no_grad() 或 inference_mode()。

#### 4. 技术洞察
核心不变量：state_dict 的键等于模块路径拼接参数/缓冲区名（如 layer1.0.conv.weight）；load_state_dict(strict=True) 要求键集合完全一致，否则报 missing/unexpected keys；buffer 不参与 optimizer 更新但会被 .to() 搬运。 推导题的难点是识别等式成立的前提；一旦去掉任一前提（例如 多层共享同一 Parameter 时 state_dict 出现多个键指向同一存储，加载需处理别名与顺序。），原关系不再成立，必须重新推导。

#### 5. 图文说明
| 层次 | 模块树、参数字典与状态持久化 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 模块结构、注册的参数/缓冲区、目标 device/dtype | 漏注册 buffer 或键空间不一致 |
| 执行与状态 | 遍历模块树、加载/保存快照、切换 training 标志 | 键缺失被静默、模式与图不匹配、别名错乱 |
| 输出与 oracle | 逐张量比对 state_dict、核对模式相关的统计 | 权重随机初始化、BN 统计漂移、显存未释放 |

图示解释：围绕 模块树、参数字典与状态持久化 把基本量、支配关系与可测量串成一条链，任一环缺失都无法从原理推出结论。

#### 6. 拓展知识
相邻知识点：named_parameters()/named_buffers()/recurse、state_dict()/load_state_dict()；比较不同精度/规模下的替代实现与误差，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 把 Module 树与命名参数 的规模放大两个数量级，等式中的哪一项先成为主导，如何用 参数量 量化？
2. 用什么实验能证伪该支配关系，例如改变 parameter 与 buffer 的区别 后预测哪个指标如何变化？
3. 该关系依赖哪些前提（精度、设备、并发），去掉某一前提后结论如何改写？

#### 8. 易错点与反例
1. 反例：把 模块树、参数字典与状态持久化 的结论当作与规模无关的定律，忽略在极端参数下 buffer 未 register_buffer 导致不随 .to() 和 state_dict 迁移。
2. 反例：用最终数值一致替代推导，忽略 共享参数被重复注册改变键空间与加载行为。
3. 反例：只验证一个参数点就外推，遇到 strict=False 静默吞掉缺失键使部分权重随机初始化 时结论崩塌。

#### 9. 实践与验证
验证方案：构造含卷积、BN、自定义 buffer 的模块，round-trip state_dict 到新实例并 assert_close 全部张量；删掉一个 buffer 验证 strict=True 报 missing keys。 用 named_parameters()/named_buffers()/recurse 与 register_buffer/register_parameter 采集 参数量、状态字典字节数，把实测与 state_dict 的键等于模块路径拼接参数/缓冲区名（如 layer1.0.conv.weight） 的理论值对比；oracle：load_state_dict 返回 missing=[]、unexpected=[]；train 模式两次 forward 后 BN running_mean 发生变化。

#### 10. 评分标准
E013 评分：
- 不合格：只会复述术语，说不出支配等式与适用条件；
- 合格：能写出核心关系并说明 Module 树与命名参数 与 parameter 与 buffer 的区别 的因果；
- 良好：能给出边界反例（buffer 未 register_buffer 导致不随 .to() 和 state_dict 迁移）、推导步骤与验证 oracle（load_state_dict 返回 missing=[]、unexpected=[]；train 模式两次 forward 后 BN running_mean 发生变化。）；
- 优秀/专家：能补充误差模型、复杂度上界、跨精度/规模外推与残余风险。
### E014. 第一性原理：数据管道

- 难度：专家级
- 知识域：K04 数据管道
- 考察能力：变量、因果链、复杂度与容量上限
- 题型：原理推导与复杂度
- 面试频率：开放题
- 前置知识：Dataset、Sampler、collate、padding、版本
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

本题从第一性原理审查 DataLoader 的并行、采样与预取，要求把结论还原到不可再分的量与守恒关系。需要先明确的量包括：map-style 与 iterable-style Dataset、num_workers 与进程启动方式、sampler 与 shuffle 语义、pin_memory 与 non_blocking 重叠。pin_memory=True 配合 non_blocking=True 能把 H2D 拷贝与计算重叠；persistent_workers=True 在 epoch 间保留 worker 进程，避免反复 fork 开销。原理推导题的任何结论都必须能由这些基本量之间的等式或复杂度关系推出，并给出可证伪的预言。

#### 2. 问题分析
1. 列出基本量与单位：map-style 与 iterable-style Dataset、num_workers 与进程启动方式、sampler 与 shuffle 语义、pin_memory 与 non_blocking 重叠、persistent_workers/prefetch_factor。
2. 写出支配等式或复杂度关系：map-style 数据集每 epoch 批数 = ⌈N/batch_size⌉（drop_last 时为 ⌊N/batch_size⌋）；worker 是独立进程并各自持有 RNG，随机变换必须经 worker_init_fn 播种才可复现。
3. 推导适用边界：在什么输入规模、精度、设备或并发下该关系成立、在何处失效。
4. 给出可证伪预言：预测某个可测量（如 吞吐 samples/s）随 size、rank 或位宽的变化趋势，并说明测量方法。

#### 3. 具体答案
面试中可以这样回答：

> 数据管道把采样、取数、批处理和预取解耦到多 worker 进程；正确性取决于索引覆盖与随机性可复现，性能取决于预取深度与 H2D 重叠。
>
> 支配关系：map-style 数据集每 epoch 批数 = ⌈N/batch_size⌉（drop_last 时为 ⌊N/batch_size⌋）；worker 是独立进程并各自持有 RNG，随机变换必须经 worker_init_fn 播种才可复现。
>
> 可验证事实：pin_memory=True 配合 non_blocking=True 能把 H2D 拷贝与计算重叠；persistent_workers=True 在 epoch 间保留 worker 进程，避免反复 fork 开销。

#### 4. 技术洞察
核心不变量：map-style 数据集每 epoch 批数 = ⌈N/batch_size⌉（drop_last 时为 ⌊N/batch_size⌋）；worker 是独立进程并各自持有 RNG，随机变换必须经 worker_init_fn 播种才可复现。 推导题的难点是识别等式成立的前提；一旦去掉任一前提（例如 IterableDataset 配 num_workers>1 时必须手动分片，否则每个 worker 读全量导致样本重复。），原关系不再成立，必须重新推导。

#### 5. 图文说明
| 层次 | DataLoader 的并行、采样与预取 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 数据集大小、batch_size、worker 数、采样方式 | 索引不覆盖、随机性不可复现 |
| 执行与状态 | 追踪 worker 进程、预取队列、pin 缓冲与 sampler 状态 | CUDA-in-fork 崩溃、句柄泄漏、顺序错乱 |
| 输出与 oracle | 核对样本集合、shape、吞吐与首 batch 延迟 | 重复/丢失样本、collate 形状错误、吞吐饱和异常 |

图示解释：围绕 DataLoader 的并行、采样与预取 把基本量、支配关系与可测量串成一条链，任一环缺失都无法从原理推出结论。

#### 6. 拓展知识
相邻知识点：DataLoader/worker_init_fn/generator、torch.utils.data.get_worker_info；比较不同精度/规模下的替代实现与误差，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 把 map-style 与 iterable-style Dataset 的规模放大两个数量级，等式中的哪一项先成为主导，如何用 吞吐 samples/s 量化？
2. 用什么实验能证伪该支配关系，例如改变 num_workers 与进程启动方式 后预测哪个指标如何变化？
3. 该关系依赖哪些前提（精度、设备、并发），去掉某一前提后结论如何改写？

#### 8. 易错点与反例
1. 反例：把 DataLoader 的并行、采样与预取 的结论当作与规模无关的定律，忽略在极端参数下 在 Dataset.__init__ 或 __getitem__ 中初始化 CUDA 导致 fork 后 worker 崩溃。
2. 反例：用最终数值一致替代推导，忽略 worker 数过大引发内存或文件句柄耗尽。
3. 反例：只验证一个参数点就外推，遇到 collate_fn 对不定长序列堆叠得到错误 shape 时结论崩塌。

#### 9. 实践与验证
验证方案：固定 seed 与 worker_init_fn，连续两 epoch 记录样本索引序列验证覆盖完整且可复现；测量 num_workers∈{0,2,4,8} 的吞吐拐点。 用 DataLoader/worker_init_fn/generator 与 IterableDataset 与分片 采集 吞吐 samples/s、GPU 利用率，把实测与 map-style 数据集每 epoch 批数 = ⌈N/batch_size⌉（drop_last 时为 ⌊N/batch_size⌋） 的理论值对比；oracle：两轮索引并集等于 0..N−1 且无重复，吞吐在某一 worker 数后饱和。

#### 10. 评分标准
E014 评分：
- 不合格：只会复述术语，说不出支配等式与适用条件；
- 合格：能写出核心关系并说明 map-style 与 iterable-style Dataset 与 num_workers 与进程启动方式 的因果；
- 良好：能给出边界反例（在 Dataset.__init__ 或 __getitem__ 中初始化 CUDA 导致 fork 后 worker 崩溃）、推导步骤与验证 oracle（两轮索引并集等于 0..N−1 且无重复，吞吐在某一 worker 数后饱和。）；
- 优秀/专家：能补充误差模型、复杂度上界、跨精度/规模外推与残余风险。
### E015. 第一性原理：训练/优化器

- 难度：专家级
- 知识域：K05 训练/优化器
- 考察能力：变量、因果链、复杂度与容量上限
- 题型：原理推导与复杂度
- 面试频率：开放题
- 前置知识：forward、backward、累积、optimizer、scheduler
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

本题从第一性原理审查 优化器更新、学习率调度与梯度处理，要求把结论还原到不可再分的量与守恒关系。需要先明确的量包括：优化器状态（Adam 的 m/v）、zero_grad 语义、梯度累积与有效批、clip 与 scaler 顺序。zero_grad(set_to_none=True) 把 .grad 置 None（自 PyTorch 2.0 起为默认行为），比置零更省内存；optimizer.state_dict() 按参数索引保存 momentum/m/v 缓冲区。原理推导题的任何结论都必须能由这些基本量之间的等式或复杂度关系推出，并给出可证伪的预言。

#### 2. 问题分析
1. 列出基本量与单位：优化器状态（Adam 的 m/v）、zero_grad 语义、梯度累积与有效批、clip 与 scaler 顺序、scheduler 步进时机。
2. 写出支配等式或复杂度关系：Adam：m_t=β1·m_{t−1}+(1−β1)·g_t，v_t=β2·v_{t−1}+(1−β2)·g_t²，偏差校正 m̂=m/(1−β1^t)、v̂=v/(1−β2^t)，θ←θ−lr·m̂/(√v̂+ε)；梯度累积 k 步等价于把有效批放大 k 倍，但必须对 loss 除以 k 或按 k 归约，否则梯度被放大 k 倍。
3. 推导适用边界：在什么输入规模、精度、设备或并发下该关系成立、在何处失效。
4. 给出可证伪预言：预测某个可测量（如 loss 曲线）随 size、rank 或位宽的变化趋势，并说明测量方法。

#### 3. 具体答案
面试中可以这样回答：

> 训练步是 zero_grad→forward→loss→scaled backward→unscale→clip→step→scheduler 的固定顺序；顺序错位会静默改变有效学习率或裁剪幅度。
>
> 支配关系：Adam：m_t=β1·m_{t−1}+(1−β1)·g_t，v_t=β2·v_{t−1}+(1−β2)·g_t²，偏差校正 m̂=m/(1−β1^t)、v̂=v/(1−β2^t)，θ←θ−lr·m̂/(√v̂+ε)；梯度累积 k 步等价于把有效批放大 k 倍，但必须对 loss 除以 k 或按 k 归约，否则梯度被放大 k 倍。
>
> 可验证事实：zero_grad(set_to_none=True) 把 .grad 置 None（自 PyTorch 2.0 起为默认行为），比置零更省内存；optimizer.state_dict() 按参数索引保存 momentum/m/v 缓冲区。

#### 4. 技术洞察
核心不变量：Adam：m_t=β1·m_{t−1}+(1−β1)·g_t，v_t=β2·v_{t−1}+(1−β2)·g_t²，偏差校正 m̂=m/(1−β1^t)、v̂=v/(1−β2^t)，θ←θ−lr·m̂/(√v̂+ε)；梯度累积 k 步等价于把有效批放大 k 倍，但必须对 loss 除以 k 或按 k 归约，否则梯度被放大 k 倍。 推导题的难点是识别等式成立的前提；一旦去掉任一前提（例如 混合精度下先 clip 后 unscale，阈值按缩放后梯度计算，等价阈值被放大 S 倍。），原关系不再成立，必须重新推导。

#### 5. 图文说明
| 层次 | 优化器更新、学习率调度与梯度处理 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 初始参数、优化器配置、损失缩放与累积步 k | 顺序错位、未缩放 loss |
| 执行与状态 | 追踪 .grad、优化器状态缓冲区、lr 调度历史 | 梯度残留、动量错乱、裁剪幅度被缩放 |
| 输出与 oracle | 核对 loss 单调性、参数更新与 lr 序列 | loss 震荡、等效 lr 漂移、累积实现不等价 |

图示解释：围绕 优化器更新、学习率调度与梯度处理 把基本量、支配关系与可测量串成一条链，任一环缺失都无法从原理推出结论。

#### 6. 拓展知识
相邻知识点：optimizer.state_dict()/param_groups、scheduler.get_last_lr()；比较不同精度/规模下的替代实现与误差，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 把 优化器状态（Adam 的 m/v） 的规模放大两个数量级，等式中的哪一项先成为主导，如何用 loss 曲线 量化？
2. 用什么实验能证伪该支配关系，例如改变 zero_grad 语义 后预测哪个指标如何变化？
3. 该关系依赖哪些前提（精度、设备、并发），去掉某一前提后结论如何改写？

#### 8. 易错点与反例
1. 反例：把 优化器更新、学习率调度与梯度处理 的结论当作与规模无关的定律，忽略在极端参数下 忘记 zero_grad 使梯度跨 step 累加，等效 lr 漂移。
2. 反例：用最终数值一致替代推导，忽略 scheduler.step 放错位置（每 batch 而非每 epoch）改变 lr 曲线。
3. 反例：只验证一个参数点就外推，遇到 clip_grad_norm_ 在 GradScaler.unscale_ 之前调用使裁剪阈值被放大 S 倍 时结论崩塌。

#### 9. 实践与验证
验证方案：在单个 batch 上跑 200 步验证 loss 单调下降到过拟合；打印 get_last_lr 序列核对调度曲线；对同一初始点比较累积实现与放大批实现的参数更新。 用 optimizer.state_dict()/param_groups 与 clip_grad_norm_ 返回值 采集 loss 曲线、全局梯度范数，把实测与 Adam：m_t=β1·m_{t−1}+(1−β1)·g_t，v_t=β2·v_{t−1}+(1−β2)·g_t²，偏差校正 m̂=m/(1−β1^t)、v̂=v/(1−β2^t)，θ←θ−lr·m̂/(√v̂+ε) 的理论值对比；oracle：单批过拟合 loss < 0.01；累积与大批实现的参数差在 1e-6 内。

#### 10. 评分标准
E015 评分：
- 不合格：只会复述术语，说不出支配等式与适用条件；
- 合格：能写出核心关系并说明 优化器状态（Adam 的 m/v） 与 zero_grad 语义 的因果；
- 良好：能给出边界反例（忘记 zero_grad 使梯度跨 step 累加，等效 lr 漂移）、推导步骤与验证 oracle（单批过拟合 loss < 0.01；累积与大批实现的参数差在 1e-6 内。）；
- 优秀/专家：能补充误差模型、复杂度上界、跨精度/规模外推与残余风险。
### E016. 第一性原理：AMP/恢复

- 难度：专家级
- 知识域：K06 AMP/恢复
- 考察能力：变量、因果链、复杂度与容量上限
- 题型：原理推导与复杂度
- 面试频率：开放题
- 前置知识：autocast、scaler、快照、随机状态
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

本题从第一性原理审查 混合精度、检查点与确定性，要求把结论还原到不可再分的量与守恒关系。需要先明确的量包括：autocast 的算子白名单、GradScaler 动态缩放、bf16 与 fp16 数值范围、RNG/optimizer 状态恢复。torch.use_deterministic_algorithms(True) 会让部分算子回退到确定性实现或直接报错；cudnn.benchmark=True 的自动调优会破坏逐步可复现性。完整恢复需保存 model、optimizer、scaler、epoch、CPU 与 CUDA RNG 及 sampler 状态。原理推导题的任何结论都必须能由这些基本量之间的等式或复杂度关系推出，并给出可证伪的预言。

#### 2. 问题分析
1. 列出基本量与单位：autocast 的算子白名单、GradScaler 动态缩放、bf16 与 fp16 数值范围、RNG/optimizer 状态恢复、确定性算法开关。
2. 写出支配等式或复杂度关系：GradScaler 先用 scale S 放大 loss 再反向，unscale_ 后若检测到 inf/nan 则跳过该 step 并把 S 减半；fp16 最正规数约 6.1e-5、次正规至约 6e-8，小于此量级的梯度下溢，故需缩放；bf16 指数位与 fp32 同为 8 位因此不需要缩放。
3. 推导适用边界：在什么输入规模、精度、设备或并发下该关系成立、在何处失效。
4. 给出可证伪预言：预测某个可测量（如 scale factor 曲线）随 size、rank 或位宽的变化趋势，并说明测量方法。

#### 3. 具体答案
面试中可以这样回答：

> AMP 通过 autocast 选低精度算子、GradScaler 维持梯度动态范围；恢复的正确性是状态闭包问题——模型、优化器、缩放与随机数状态缺一不可。
>
> 支配关系：GradScaler 先用 scale S 放大 loss 再反向，unscale_ 后若检测到 inf/nan 则跳过该 step 并把 S 减半；fp16 最正规数约 6.1e-5、次正规至约 6e-8，小于此量级的梯度下溢，故需缩放；bf16 指数位与 fp32 同为 8 位因此不需要缩放。
>
> 可验证事实：torch.use_deterministic_algorithms(True) 会让部分算子回退到确定性实现或直接报错；cudnn.benchmark=True 的自动调优会破坏逐步可复现性。完整恢复需保存 model、optimizer、scaler、epoch、CPU 与 CUDA RNG 及 sampler 状态。

#### 4. 技术洞察
核心不变量：GradScaler 先用 scale S 放大 loss 再反向，unscale_ 后若检测到 inf/nan 则跳过该 step 并把 S 减半；fp16 最正规数约 6.1e-5、次正规至约 6e-8，小于此量级的梯度下溢，故需缩放；bf16 指数位与 fp32 同为 8 位因此不需要缩放。 推导题的难点是识别等式成立的前提；一旦去掉任一前提（例如 梯度累积下 GradScaler 必须在累积窗口结束才 step/update，窗口内重复 unscale 会破坏缩放语义。），原关系不再成立，必须重新推导。

#### 5. 图文说明
| 层次 | 混合精度、检查点与确定性 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | dtype、autocast 区域、scaler 配置、checkpoint 内容 | 状态不闭合、精度选择错误 |
| 执行与状态 | 追踪 scale factor、RNG、优化器状态与 nan/inf 分支 | 下溢、NaN、恢复后状态漂移 |
| 输出与 oracle | 比对续训与基线参数、scale 曲线、依赖确定性开关 | 曲线跳变、结果不可复现、梯度全零 |

图示解释：围绕 混合精度、检查点与确定性 把基本量、支配关系与可测量串成一条链，任一环缺失都无法从原理推出结论。

#### 6. 拓展知识
相邻知识点：torch.cuda.amp.GradScaler 与 autocast、torch.get_rng_state/get_cuda_rng_state/numpy.random.get_state；比较不同精度/规模下的替代实现与误差，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 把 autocast 的算子白名单 的规模放大两个数量级，等式中的哪一项先成为主导，如何用 scale factor 曲线 量化？
2. 用什么实验能证伪该支配关系，例如改变 GradScaler 动态缩放 后预测哪个指标如何变化？
3. 该关系依赖哪些前提（精度、设备、并发），去掉某一前提后结论如何改写？

#### 8. 易错点与反例
1. 反例：把 混合精度、检查点与确定性 的结论当作与规模无关的定律，忽略在极端参数下 fp16 无 scaler 导致梯度下溢为 0 或 NaN。
2. 反例：用最终数值一致替代推导，忽略 resume 只存模型权重丢失优化器动量使曲线跳变。
3. 反例：只验证一个参数点就外推，遇到 只存 torch RNG 未存 numpy/random 状态导致数据顺序不同 时结论崩塌。

#### 9. 实践与验证
验证方案：训练 50 步保存完整 checkpoint，重启加载后再跑 50 步与不中断基线逐参数比较；fp16 下注入大梯度观测 S 是否减半。 用 torch.cuda.amp.GradScaler 与 autocast 与 torch.use_deterministic_algorithms 采集 scale factor 曲线、NaN/inf 次数，把实测与 GradScaler 先用 scale S 放大 loss 再反向，unscale_ 后若检测到 inf/nan 则跳过该 step 并把 S 减半 的理论值对比；oracle：续训与基线参数最大差 < 1e-5（bit 级取决于确定性开关），scale factor 与跳过计数一致。

#### 10. 评分标准
E016 评分：
- 不合格：只会复述术语，说不出支配等式与适用条件；
- 合格：能写出核心关系并说明 autocast 的算子白名单 与 GradScaler 动态缩放 的因果；
- 良好：能给出边界反例（fp16 无 scaler 导致梯度下溢为 0 或 NaN）、推导步骤与验证 oracle（续训与基线参数最大差 < 1e-5（bit 级取决于确定性开关），scale factor 与跳过计数一致。）；
- 优秀/专家：能补充误差模型、复杂度上界、跨精度/规模外推与残余风险。
### E017. 第一性原理：分布式

- 难度：专家级
- 知识域：K07 分布式
- 考察能力：变量、因果链、复杂度与容量上限
- 题型：原理推导与复杂度
- 面试频率：开放题
- 前置知识：rank、DDP、FSDP、collective、容错
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

本题从第一性原理审查 进程组、集合通信与梯度同步，要求把结论还原到不可再分的量与守恒关系。需要先明确的量包括：进程组与 rank/world_size、all_reduce/reduce_scatter/all_gather、NCCL 与 gloo 后端、梯度分桶与通信重叠。NCCL 只支持 CUDA 张量、gloo 支持 CPU；DDP 默认把梯度分成约 25 MiB 的桶以便与反向计算重叠通信。原理推导题的任何结论都必须能由这些基本量之间的等式或复杂度关系推出，并给出可证伪的预言。

#### 2. 问题分析
1. 列出基本量与单位：进程组与 rank/world_size、all_reduce/reduce_scatter/all_gather、NCCL 与 gloo 后端、梯度分桶与通信重叠、DDP/FSDP 语义。
2. 写出支配等式或复杂度关系：DDP 默认 all_reduce(sum) 后除以 world_size 得平均梯度，故全局有效批 = 本地批 × world_size × 累积步数；ring all_reduce 单步通信量 ≈ 2·(N−1)/N·S（S 为梯度字节数），大 N 时趋近 2S。
3. 推导适用边界：在什么输入规模、精度、设备或并发下该关系成立、在何处失效。
4. 给出可证伪预言：预测某个可测量（如 all_reduce 带宽 GB/s）随 size、rank 或位宽的变化趋势，并说明测量方法。

#### 3. 具体答案
面试中可以这样回答：

> 分布式训练把单机步拆成计算→梯度归约→更新三段；正确性在于所有 rank 以相同顺序进入集合通信且梯度按 world_size 归一，性能在于把通信藏进反向的尾部。
>
> 支配关系：DDP 默认 all_reduce(sum) 后除以 world_size 得平均梯度，故全局有效批 = 本地批 × world_size × 累积步数；ring all_reduce 单步通信量 ≈ 2·(N−1)/N·S（S 为梯度字节数），大 N 时趋近 2S。
>
> 可验证事实：NCCL 只支持 CUDA 张量、gloo 支持 CPU；DDP 默认把梯度分成约 25 MiB 的桶以便与反向计算重叠通信。

#### 4. 技术洞察
核心不变量：DDP 默认 all_reduce(sum) 后除以 world_size 得平均梯度，故全局有效批 = 本地批 × world_size × 累积步数；ring all_reduce 单步通信量 ≈ 2·(N−1)/N·S（S 为梯度字节数），大 N 时趋近 2S。 推导题的难点是识别等式成立的前提；一旦去掉任一前提（例如 find_unused_parameters=True 会在反向前后额外遍历图，掩盖未参与反向的参数但增加开销。），原关系不再成立，必须重新推导。

#### 5. 图文说明
| 层次 | 进程组、集合通信与梯度同步 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 进程数、后端、张量是否 CUDA、参与反向的参数集合 | 后端不匹配、参数未参与、集合调用不一致 |
| 执行与状态 | 追踪 rank、bucket、通信算子与反向重叠 | hang、梯度未归约、通信未重叠 |
| 输出与 oracle | 对比单卡/DDP 梯度、通信带宽与 step 方差 | 梯度差超容差、超时、吞吐不随卡数提升 |

图示解释：围绕 进程组、集合通信与梯度同步 把基本量、支配关系与可测量串成一条链，任一环缺失都无法从原理推出结论。

#### 6. 拓展知识
相邻知识点：torch.distributed.init_process_group/barrier、dist.all_reduce/all_gather/reduce_scatter；比较不同精度/规模下的替代实现与误差，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 把 进程组与 rank/world_size 的规模放大两个数量级，等式中的哪一项先成为主导，如何用 all_reduce 带宽 GB/s 量化？
2. 用什么实验能证伪该支配关系，例如改变 all_reduce/reduce_scatter/all_gather 后预测哪个指标如何变化？
3. 该关系依赖哪些前提（精度、设备、并发），去掉某一前提后结论如何改写？

#### 8. 易错点与反例
1. 反例：把 进程组、集合通信与梯度同步 的结论当作与规模无关的定律，忽略在极端参数下 各 rank 集合通信调用不匹配导致 hang 直到 watchdog 超时。
2. 反例：用最终数值一致替代推导，忽略 未按 world_size 归一梯度使等效 lr 随卡数变化。
3. 反例：只验证一个参数点就外推，遇到 NCCL/驱动版本不一致引发通信超时或崩溃 时结论崩塌。

#### 9. 实践与验证
验证方案：用 torchrun 起 2 进程，固定初始化使单卡与 DDP 梯度可对齐，比较 all_reduce 前后梯度；人为让一个 rank 提前退出观测 NCCL watchdog 超时。 用 torch.distributed.init_process_group/barrier 与 NCCL_DEBUG=INFO 与 TORCH_DISTRIBUTED_DEBUG 采集 all_reduce 带宽 GB/s、通信时间占比，把实测与 DDP 默认 all_reduce(sum) 后除以 world_size 得平均梯度，故全局有效批 = 本地批 × world_size × 累积步数 的理论值对比；oracle：DDP 与单卡初始梯度（除以 world_size 后）相对误差 < 1e-6；NCCL_DEBUG 显示预期的 ring/tree 算法。

#### 10. 评分标准
E017 评分：
- 不合格：只会复述术语，说不出支配等式与适用条件；
- 合格：能写出核心关系并说明 进程组与 rank/world_size 与 all_reduce/reduce_scatter/all_gather 的因果；
- 良好：能给出边界反例（各 rank 集合通信调用不匹配导致 hang 直到 watchdog 超时）、推导步骤与验证 oracle（DDP 与单卡初始梯度（除以 world_size 后）相对误差 < 1e-6；NCCL_DEBUG 显示预期的 ring/tree 算法。）；
- 优秀/专家：能补充误差模型、复杂度上界、跨精度/规模外推与残余风险。
### E018. 第一性原理：编译栈

- 难度：专家级
- 知识域：K08 编译栈
- 考察能力：变量、因果链、复杂度与容量上限
- 题型：原理推导与复杂度
- 面试频率：开放题
- 前置知识：Dynamo、AOTAutograd、Inductor、guard、缓存
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

本题从第一性原理审查 Dynamo 捕获、guard 与 Inductor 代码生成，要求把结论还原到不可再分的量与守恒关系。需要先明确的量包括：TorchDynamo 的字节码捕获、guard 与 specialization、graph break 与子图、AOTAutograd 与 Inductor。设置 TORCH_LOGS=graph_breaks,guards,recompiles 可打印断点与重编译原因；torch._dynamo.explain(fn)(*args) 返回断点数量与原因；dynamic=True 或 torch._dynamo.mark_dynamic 可放宽形状守卫。原理推导题的任何结论都必须能由这些基本量之间的等式或复杂度关系推出，并给出可证伪的预言。

#### 2. 问题分析
1. 列出基本量与单位：TorchDynamo 的字节码捕获、guard 与 specialization、graph break 与子图、AOTAutograd 与 Inductor、动态 shape 与重编译。
2. 写出支配等式或复杂度关系：一个编译单元的 guard 绑定在张量的 shape/dtype/device 与 Python 常量上；任一 guard 失效即触发重编译，默认累计重编译上限约 8 次（torch._dynamo.config.accumulated_recompile_limit）。图被 graph break 切成多个子图，未捕获区间回退 eager。
3. 推导适用边界：在什么输入规模、精度、设备或并发下该关系成立、在何处失效。
4. 给出可证伪预言：预测某个可测量（如 重编译次数）随 size、rank 或位宽的变化趋势，并说明测量方法。

#### 3. 具体答案
面试中可以这样回答：

> 编译栈把 Python 执行轨迹捕获成 FX 图，用 guard 保证图对后续输入仍有效；失效即重编译、无法捕获即断图。稳定收益来自低断点数与低重编译率而非一次加速。
>
> 支配关系：一个编译单元的 guard 绑定在张量的 shape/dtype/device 与 Python 常量上；任一 guard 失效即触发重编译，默认累计重编译上限约 8 次（torch._dynamo.config.accumulated_recompile_limit）。图被 graph break 切成多个子图，未捕获区间回退 eager。
>
> 可验证事实：设置 TORCH_LOGS=graph_breaks,guards,recompiles 可打印断点与重编译原因；torch._dynamo.explain(fn)(*args) 返回断点数量与原因；dynamic=True 或 torch._dynamo.mark_dynamic 可放宽形状守卫。

#### 4. 技术洞察
核心不变量：一个编译单元的 guard 绑定在张量的 shape/dtype/device 与 Python 常量上；任一 guard 失效即触发重编译，默认累计重编译上限约 8 次（torch._dynamo.config.accumulated_recompile_limit）。图被 graph break 切成多个子图，未捕获区间回退 eager。 推导题的难点是识别等式成立的前提；一旦去掉任一前提（例如 把张量属性（如 .shape 的某维）当 Python 常量使用会特化成守卫，改变该维度即重编译。），原关系不再成立，必须重新推导。

#### 5. 图文说明
| 层次 | Dynamo 捕获、guard 与 Inductor 代码生成 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 函数代码、示例输入、shape/dtype/device 与 dynamic 声明 | 守卫过严、断点隐藏、动态维度未声明 |
| 执行与状态 | 追踪 Dynamo 捕获、guard 集合、子图与 Inductor 缓存 | 重编译风暴、断点回退 eager、缓存未命中 |
| 输出与 oracle | 对比 compiled/eager 数值、重编译与断点计数 | 数值不一致、加速比 < 1、重编译失控 |

图示解释：围绕 Dynamo 捕获、guard 与 Inductor 代码生成 把基本量、支配关系与可测量串成一条链，任一环缺失都无法从原理推出结论。

#### 6. 拓展知识
相邻知识点：TORCH_LOGS=graph_breaks,guards,recompiles、torch._dynamo.explain；比较不同精度/规模下的替代实现与误差，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 把 TorchDynamo 的字节码捕获 的规模放大两个数量级，等式中的哪一项先成为主导，如何用 重编译次数 量化？
2. 用什么实验能证伪该支配关系，例如改变 guard 与 specialization 后预测哪个指标如何变化？
3. 该关系依赖哪些前提（精度、设备、并发），去掉某一前提后结论如何改写？

#### 8. 易错点与反例
1. 反例：把 Dynamo 捕获、guard 与 Inductor 代码生成 的结论当作与规模无关的定律，忽略在极端参数下 在图中调用 .item()/print/依赖数据的 Python 分支触发 graph break。
2. 反例：用最终数值一致替代推导，忽略 动态 shape 引发重编译风暴使性能倒挂。
3. 反例：只验证一个参数点就外推，遇到 别名或原地写使编译结果与 eager 数值不一致 时结论崩塌。

#### 9. 实践与验证
验证方案：对同一函数用固定 shape 与变化 shape 各跑 20 次，用 explain 统计 graph break 与重编译；对比 compiled 与 eager 输出最大差。 用 TORCH_LOGS=graph_breaks,guards,recompiles 与 torch.compile(mode=...)/mark_dynamic 采集 重编译次数、graph break 数，把实测与 一个编译单元的 guard 绑定在张量的 shape/dtype/device 与 Python 常量上 的理论值对比；oracle：固定 shape 下重编译次数为 1、输出与 eager 最大差 < 1e-4；变化 shape 下开启 dynamic 后重编译显著下降。

#### 10. 评分标准
E018 评分：
- 不合格：只会复述术语，说不出支配等式与适用条件；
- 合格：能写出核心关系并说明 TorchDynamo 的字节码捕获 与 guard 与 specialization 的因果；
- 良好：能给出边界反例（在图中调用 .item()/print/依赖数据的 Python 分支触发 graph break）、推导步骤与验证 oracle（固定 shape 下重编译次数为 1、输出与 eager 最大差 < 1e-4；变化 shape 下开启 dynamic 后重编译显著下降。）；
- 优秀/专家：能补充误差模型、复杂度上界、跨精度/规模外推与残余风险。
### E019. 第一性原理：性能

- 难度：专家级
- 知识域：K09 性能
- 考察能力：变量、因果链、复杂度与容量上限
- 题型：原理推导与复杂度
- 面试频率：开放题
- 前置知识：关键路径、带宽、算力、通信、显存、统计
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

本题从第一性原理审查 性能剖析、roofline 与显存容量，要求把结论还原到不可再分的量与守恒关系。需要先明确的量包括：CUDA event 计时与同步、roofline 与算术强度、kernel 融合与 launch 开销、显存占用与碎片。torch.profiler.profile(activities=[ProfilerActivity.CUDA]) 会插入 CUDA 事件并输出按算子聚合的 self/total CUDA time；首次迭代含 cuBLAS/context 初始化与自动调优，必须先 warmup 再计时。原理推导题的任何结论都必须能由这些基本量之间的等式或复杂度关系推出，并给出可证伪的预言。

#### 2. 问题分析
1. 列出基本量与单位：CUDA event 计时与同步、roofline 与算术强度、kernel 融合与 launch 开销、显存占用与碎片、profiler 证据链。
2. 写出支配等式或复杂度关系：算术强度 I=FLOPs/Bytes，roofline 上界 = min(峰值算力 P, I×峰值带宽 BW)，据此判定 compute-bound 或 memory-bound；CUDA event 计时为 t=elapsed_time(start,stop)，异步操作必须 torch.cuda.synchronize() 后计时才可信。
3. 推导适用边界：在什么输入规模、精度、设备或并发下该关系成立、在何处失效。
4. 给出可证伪预言：预测某个可测量（如 p50/p99 延迟）随 size、rank 或位宽的变化趋势，并说明测量方法。

#### 3. 具体答案
面试中可以这样回答：

> 性能结论必须绑定可复现的测量协议：warmup、同步、重复次数、分位数；再用 roofline 判定算力受限还是带宽受限，最后才谈优化手段。
>
> 支配关系：算术强度 I=FLOPs/Bytes，roofline 上界 = min(峰值算力 P, I×峰值带宽 BW)，据此判定 compute-bound 或 memory-bound；CUDA event 计时为 t=elapsed_time(start,stop)，异步操作必须 torch.cuda.synchronize() 后计时才可信。
>
> 可验证事实：torch.profiler.profile(activities=[ProfilerActivity.CUDA]) 会插入 CUDA 事件并输出按算子聚合的 self/total CUDA time；首次迭代含 cuBLAS/context 初始化与自动调优，必须先 warmup 再计时。

#### 4. 技术洞察
核心不变量：算术强度 I=FLOPs/Bytes，roofline 上界 = min(峰值算力 P, I×峰值带宽 BW)，据此判定 compute-bound 或 memory-bound；CUDA event 计时为 t=elapsed_time(start,stop)，异步操作必须 torch.cuda.synchronize() 后计时才可信。 推导题的难点是识别等式成立的前提；一旦去掉任一前提（例如 D2D 拷贝带宽受 ECC 与读写混合影响通常显著低于理论峰值，小 size 下 launch 开销占主导。），原关系不再成立，必须重新推导。

#### 5. 图文说明
| 层次 | 性能剖析、roofline 与显存容量 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 算子/规模、dtype、warmup 与 repeat、计时方式 | 未同步、未 warmup、规模误导 |
| 执行与状态 | 追踪 kernel 时长、显存曲线、launch 与同步点 | 计时偏短、碎片抖动、把 launch 开销当算力 |
| 输出与 oracle | 对比 event/profiler、roofline 定位与分位数 | 加速比不稳定、有效带宽超理论值、尾延迟超标 |

图示解释：围绕 性能剖析、roofline 与显存容量 把基本量、支配关系与可测量串成一条链，任一环缺失都无法从原理推出结论。

#### 6. 拓展知识
相邻知识点：torch.profiler、torch.cuda.Event/elapsed_time；比较不同精度/规模下的替代实现与误差，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 把 CUDA event 计时与同步 的规模放大两个数量级，等式中的哪一项先成为主导，如何用 p50/p99 延迟 量化？
2. 用什么实验能证伪该支配关系，例如改变 roofline 与算术强度 后预测哪个指标如何变化？
3. 该关系依赖哪些前提（精度、设备、并发），去掉某一前提后结论如何改写？

#### 8. 易错点与反例
1. 反例：把 性能剖析、roofline 与显存容量 的结论当作与规模无关的定律，忽略在极端参数下 异步未同步导致计时偏短，得出虚假加速。
2. 反例：用最终数值一致替代推导，忽略 把 profiler 自身开销计入结论。
3. 反例：只验证一个参数点就外推，遇到 只测单次忽略冷启动与首次调优 时结论崩塌。

#### 9. 实践与验证
验证方案：对 matmul、逐元素、拷贝分别用 CUDA event 与 profiler 计时，warmup=10、repeat=100、取中位数；用 bytes/time 反推有效带宽并与理论比较。 用 torch.profiler 与 ncu/nsys 采集 p50/p99 延迟、有效带宽 GB/s，把实测与 算术强度 I=FLOPs/Bytes，roofline 上界 = min(峰值算力 P, I×峰值带宽 BW)，据此判定 compute-bound 或 memory-bound 的理论值对比；oracle：event 计时与 profiler 的 CUDA time 偏差 < 5%，大 size 下有效带宽落在理论峰值的可解释比例内。

#### 10. 评分标准
E019 评分：
- 不合格：只会复述术语，说不出支配等式与适用条件；
- 合格：能写出核心关系并说明 CUDA event 计时与同步 与 roofline 与算术强度 的因果；
- 良好：能给出边界反例（异步未同步导致计时偏短，得出虚假加速）、推导步骤与验证 oracle（event 计时与 profiler 的 CUDA time 偏差 < 5%，大 size 下有效带宽落在理论峰值的可解释比例内。）；
- 优秀/专家：能补充误差模型、复杂度上界、跨精度/规模外推与残余风险。
### E020. 第一性原理：导出部署

- 难度：专家级
- 知识域：K10 导出部署
- 考察能力：变量、因果链、复杂度与容量上限
- 题型：原理推导与复杂度
- 面试频率：开放题
- 前置知识：state_dict、Export、runtime、schema、迁移
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

本题从第一性原理审查 图导出、图签名与跨版本加载，要求把结论还原到不可再分的量与守恒关系。需要先明确的量包括：torch.export/ExportedProgram、torch.jit.trace/script、ONNX opset 与 dynamic_axes、graph signature 与功能化。torch.export 需要示例输入，返回含 graph_module、range_constraints 与 signature 的 ExportedProgram；torch.jit.trace 只记录执行过的算子，遇控制流会固化当时走的分支。原理推导题的任何结论都必须能由这些基本量之间的等式或复杂度关系推出，并给出可证伪的预言。

#### 2. 问题分析
1. 列出基本量与单位：torch.export/ExportedProgram、torch.jit.trace/script、ONNX opset 与 dynamic_axes、graph signature 与功能化、序列化安全与反序列化兼容。
2. 写出支配等式或复杂度关系：导出图是函数式的：无 Python 副作用，输入输出由 graph_signature 声明；torch.export 的功能化把原地写改写为显式输出。ONNX 的算子覆盖度由 opset 版本决定，超出该 opset 的算子导出即失败。
3. 推导适用边界：在什么输入规模、精度、设备或并发下该关系成立、在何处失效。
4. 给出可证伪预言：预测某个可测量（如 导出成功率）随 size、rank 或位宽的变化趋势，并说明测量方法。

#### 3. 具体答案
面试中可以这样回答：

> 导出把可变的 Python 模块冻结成只含 aten 算子的静态图，并用水印式签名声明输入输出契约；正确性等于导出图在目标 runtime 与 eager 数值一致，兼容性等于签名与 opset 能跨版本被消费。
>
> 支配关系：导出图是函数式的：无 Python 副作用，输入输出由 graph_signature 声明；torch.export 的功能化把原地写改写为显式输出。ONNX 的算子覆盖度由 opset 版本决定，超出该 opset 的算子导出即失败。
>
> 可验证事实：torch.export 需要示例输入，返回含 graph_module、range_constraints 与 signature 的 ExportedProgram；torch.jit.trace 只记录执行过的算子，遇控制流会固化当时走的分支。

#### 4. 技术洞察
核心不变量：导出图是函数式的：无 Python 副作用，输入输出由 graph_signature 声明；torch.export 的功能化把原地写改写为显式输出。ONNX 的算子覆盖度由 opset 版本决定，超出该 opset 的算子导出即失败。 推导题的难点是识别等式成立的前提；一旦去掉任一前提（例如 含数据相关控制流的模型用 trace 会丢分支，需要 script 或改写为导出友好的条件算子。），原关系不再成立，必须重新推导。

#### 5. 图文说明
| 层次 | 图导出、图签名与跨版本加载 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 模型、示例输入、dynamic 声明与目标 opset/runtime | 控制流未覆盖、动态维度缺声明 |
| 执行与状态 | 追踪图捕获、功能化改写、签名与序列化 | 分支固化、opset 缺算子、加载执行任意代码 |
| 输出与 oracle | 对比导出/eager 输出、校验签名与 checker | 数值偏差、改变维度即失效、导出被拒绝 |

图示解释：围绕 图导出、图签名与跨版本加载 把基本量、支配关系与可测量串成一条链，任一环缺失都无法从原理推出结论。

#### 6. 拓展知识
相邻知识点：torch.export/exported.run_decompositions、torch.onnx.export；比较不同精度/规模下的替代实现与误差，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 把 torch.export/ExportedProgram 的规模放大两个数量级，等式中的哪一项先成为主导，如何用 导出成功率 量化？
2. 用什么实验能证伪该支配关系，例如改变 torch.jit.trace/script 后预测哪个指标如何变化？
3. 该关系依赖哪些前提（精度、设备、并发），去掉某一前提后结论如何改写？

#### 8. 易错点与反例
1. 反例：把 图导出、图签名与跨版本加载 的结论当作与规模无关的定律，忽略在极端参数下 trace 未覆盖控制流分支导致逻辑被固化到单一分支。
2. 反例：用最终数值一致替代推导，忽略 dynamic shape 未显式声明使其被编译成常量。
3. 反例：只验证一个参数点就外推，遇到 opset 过低缺失算子导致导出失败 时结论崩塌。

#### 9. 实践与验证
验证方案：对同一模型分别 torch.export 与 jit.trace，跑固定输入比对输出；改变一个输入维度检验 dynamic_shapes 是否被尊重；用 onnx.checker 校验导出图。 用 torch.export/exported.run_decompositions 与 torch.jit.trace 采集 导出成功率、图算子数，把实测与 导出图是函数式的：无 Python 副作用，输入输出由 graph_signature 声明 的理论值对比；oracle：导出输出与 eager 最大差 < 1e-5，声明为 dynamic 的维度改变时无需重导。

#### 10. 评分标准
E020 评分：
- 不合格：只会复述术语，说不出支配等式与适用条件；
- 合格：能写出核心关系并说明 torch.export/ExportedProgram 与 torch.jit.trace/script 的因果；
- 良好：能给出边界反例（trace 未覆盖控制流分支导致逻辑被固化到单一分支）、推导步骤与验证 oracle（导出输出与 eager 最大差 < 1e-5，声明为 dynamic 的维度改变时无需重导。）；
- 优秀/专家：能补充误差模型、复杂度上界、跨精度/规模外推与残余风险。
### E021. 第一性原理：自定义算子

- 难度：专家级
- 知识域：K11 自定义算子
- 考察能力：事件顺序、状态迁移、所有权与观测点
- 题型：流程/调用链/数据流
- 面试频率：开放题
- 前置知识：schema、dispatcher、stream、autograd、ABI
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

本题从第一性原理审查 算子注册、调度与数值正确性，要求把结论还原到不可再分的量与守恒关系。需要先明确的量包括：torch.library/TORCH_LIBRARY 注册、autograd.Function 与双反向、AT_DISPATCH 与设备守卫、launch 配置与边界检查。torch.utils.cpp_extension.load/load_inline 首次调用时编译并缓存扩展；缺少 TORCH_CHECK 的越界访问可能不报错而返回垃圾数据或破坏相邻内存。原理推导题的任何结论都必须能由这些基本量之间的等式或复杂度关系推出，并给出可证伪的预言。

#### 2. 问题分析
1. 列出基本量与单位：torch.library/TORCH_LIBRARY 注册、autograd.Function 与双反向、AT_DISPATCH 与设备守卫、launch 配置与边界检查、meta/fake 实现与 dispatcher。
2. 写出支配等式或复杂度关系：CUDA kernel 启动取 blocks=⌈N/threads⌉、threads≤1024 且为 32 的倍数；自定义算子若参与训练必须同时提供 forward 与 backward（或注册 autograd 公式），否则梯度断链；访问需满足 0≤idx<N 防越界。
3. 推导适用边界：在什么输入规模、精度、设备或并发下该关系成立、在何处失效。
4. 给出可证伪预言：预测某个可测量（如 相对误差）随 size、rank 或位宽的变化趋势，并说明测量方法。

#### 3. 具体答案
面试中可以这样回答：

> 自定义算子是 dispatcher 上的一个实现对：前向 kernel 负责数值、反向公式负责梯度；正确性由边界条件、设备守卫与 gradcheck 共同保证。
>
> 支配关系：CUDA kernel 启动取 blocks=⌈N/threads⌉、threads≤1024 且为 32 的倍数；自定义算子若参与训练必须同时提供 forward 与 backward（或注册 autograd 公式），否则梯度断链；访问需满足 0≤idx<N 防越界。
>
> 可验证事实：torch.utils.cpp_extension.load/load_inline 首次调用时编译并缓存扩展；缺少 TORCH_CHECK 的越界访问可能不报错而返回垃圾数据或破坏相邻内存。

#### 4. 技术洞察
核心不变量：CUDA kernel 启动取 blocks=⌈N/threads⌉、threads≤1024 且为 32 的倍数；自定义算子若参与训练必须同时提供 forward 与 backward（或注册 autograd 公式），否则梯度断链；访问需满足 0≤idx<N 防越界。 推导题的难点是识别等式成立的前提；一旦去掉任一前提（例如 归约类算子用 atomicAdd 会产生非确定性浮点累加顺序，需固定归约树或接受容差。），原关系不再成立，必须重新推导。

#### 5. 图文说明
| 层次 | 算子注册、调度与数值正确性 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 张量 shape/dtype/device、线程与 grid 配置、边界条件 | 越界、设备不匹配、dtype 未分派 |
| 执行与状态 | 追踪 kernel 启动、CUDAGuard、dispatcher 分派与归约 | 越界写、跑错设备、梯度断链 |
| 输出与 oracle | 对比参考实现、gradcheck 与 sanitizer 报告 | 误差超标、梯度为零、随机错误 |

图示解释：围绕 算子注册、调度与数值正确性 把基本量、支配关系与可测量串成一条链，任一环缺失都无法从原理推出结论。

#### 6. 拓展知识
相邻知识点：torch.utils.cpp_extension.load/load_inline、AT_DISPATCH_FLOATING_TYPES；比较不同精度/规模下的替代实现与误差，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 把 torch.library/TORCH_LIBRARY 注册 的规模放大两个数量级，等式中的哪一项先成为主导，如何用 相对误差 量化？
2. 用什么实验能证伪该支配关系，例如改变 autograd.Function 与双反向 后预测哪个指标如何变化？
3. 该关系依赖哪些前提（精度、设备、并发），去掉某一前提后结论如何改写？

#### 8. 易错点与反例
1. 反例：把 算子注册、调度与数值正确性 的结论当作与规模无关的定律，忽略在极端参数下 只写 forward 未注册 backward 使梯度静默为零。
2. 反例：用最终数值一致替代推导，忽略 kernel 越界写破坏相邻显存造成随机错误。
3. 反例：只验证一个参数点就外推，遇到 缺少 CUDAGuard 在多卡环境跑错设备 时结论崩塌。

#### 9. 实践与验证
验证方案：实现逐元素算子并与 eager 参考比较，用 gradcheck 校验反向；把 N 设为非 warp 倍数检验尾块处理，跑 compute-sanitizer。 用 torch.utils.cpp_extension.load/load_inline 与 CUDA_LAUNCH_BLOCKING=1 采集 相对误差、kernel 耗时，把实测与 CUDA kernel 启动取 blocks=⌈N/threads⌉、threads≤1024 且为 32 的倍数 的理论值对比；oracle：fp32 最大绝对误差 < 1e-6、gradcheck 通过，N=线程数±1 时无越界且结果正确。

#### 10. 评分标准
E021 评分：
- 不合格：只会复述术语，说不出支配等式与适用条件；
- 合格：能写出核心关系并说明 torch.library/TORCH_LIBRARY 注册 与 autograd.Function 与双反向 的因果；
- 良好：能给出边界反例（只写 forward 未注册 backward 使梯度静默为零）、推导步骤与验证 oracle（fp32 最大绝对误差 < 1e-6、gradcheck 通过，N=线程数±1 时无越界且结果正确。）；
- 优秀/专家：能补充误差模型、复杂度上界、跨精度/规模外推与残余风险。
### E022. 第一性原理：测试治理

- 难度：专家级
- 知识域：K12 测试治理
- 考察能力：事件顺序、状态迁移、所有权与观测点
- 题型：流程/调用链/数据流
- 面试频率：开放题
- 前置知识：oracle、证据、日志、回滚、门禁、复盘
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

本题从第一性原理审查 测试契约、容差与证据化调试，要求把结论还原到不可再分的量与守恒关系。需要先明确的量包括：pytest 参数化与 fixture、assert_close 的 rtol/atol、确定性种子与测试隔离、CI 门禁与 flaky 治理。pytest -k 按名筛选、-x 首次失败即停、--reruns 需 pytest-rerunfailures 插件；未设种子的随机测试在 CI 中会产生不可复现失败。原理推导题的任何结论都必须能由这些基本量之间的等式或复杂度关系推出，并给出可证伪的预言。

#### 2. 问题分析
1. 列出基本量与单位：pytest 参数化与 fixture、assert_close 的 rtol/atol、确定性种子与测试隔离、CI 门禁与 flaky 治理、golden/回归基线。
2. 写出支配等式或复杂度关系：torch.testing.assert_close 在 |a−b| ≤ atol + rtol·|b| 时通过，float32 默认 rtol≈1.3e-6、atol≈1e-5；flaky 率 = 失败重跑数/总重跑数，需按阈值门禁。
3. 推导适用边界：在什么输入规模、精度、设备或并发下该关系成立、在何处失效。
4. 给出可证伪预言：预测某个可测量（如 通过率）随 size、rank 或位宽的变化趋势，并说明测量方法。

#### 3. 具体答案
面试中可以这样回答：

> 测试是把需求写成可执行的契约；有效的门禁需要确定性、隔离、明确容差和可定位失败的证据，而不是只断言最终数值。
>
> 支配关系：torch.testing.assert_close 在 |a−b| ≤ atol + rtol·|b| 时通过，float32 默认 rtol≈1.3e-6、atol≈1e-5；flaky 率 = 失败重跑数/总重跑数，需按阈值门禁。
>
> 可验证事实：pytest -k 按名筛选、-x 首次失败即停、--reruns 需 pytest-rerunfailures 插件；未设种子的随机测试在 CI 中会产生不可复现失败。

#### 4. 技术洞察
核心不变量：torch.testing.assert_close 在 |a−b| ≤ atol + rtol·|b| 时通过，float32 默认 rtol≈1.3e-6、atol≈1e-5；flaky 率 = 失败重跑数/总重跑数，需按阈值门禁。 推导题的难点是识别等式成立的前提；一旦去掉任一前提（例如 对随机初始化模型直接比数值会误报，需先固定权重或用对初始化不敏感的不变量（shape/单调性/守恒量）。），原关系不再成立，必须重新推导。

#### 5. 图文说明
| 层次 | 测试契约、容差与证据化调试 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 被测模块、参数组合、种子、容差与基线 | 容差过宽、随机未固定、基线过期 |
| 执行与状态 | 追踪 fixture 生命周期、全局状态与断言点 | 顺序相关、静默 skip、只测终值 |
| 输出与 oracle | 核对断言、失败可定位性与 flaky 统计 | 假绿、不可复现失败、回归漏检 |

图示解释：围绕 测试契约、容差与证据化调试 把基本量、支配关系与可测量串成一条链，任一环缺失都无法从原理推出结论。

#### 6. 拓展知识
相邻知识点：pytest/parametrize/fixture、torch.testing.assert_close；比较不同精度/规模下的替代实现与误差，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 把 pytest 参数化与 fixture 的规模放大两个数量级，等式中的哪一项先成为主导，如何用 通过率 量化？
2. 用什么实验能证伪该支配关系，例如改变 assert_close 的 rtol/atol 后预测哪个指标如何变化？
3. 该关系依赖哪些前提（精度、设备、并发），去掉某一前提后结论如何改写？

#### 8. 易错点与反例
1. 反例：把 测试契约、容差与证据化调试 的结论当作与规模无关的定律，忽略在极端参数下 容差过宽掩盖真实数值回归。
2. 反例：用最终数值一致替代推导，忽略 测试间共享全局状态导致顺序相关失败。
3. 反例：只验证一个参数点就外推，遇到 无 GPU 时静默 skip 使 CI 假绿 时结论崩塌。

#### 9. 实践与验证
验证方案：为某模块写参数化测试（多 shape/dtype），固定种子并用加严容差的 assert_close；注入一个已知 bug 验证测试能失败。 用 pytest/parametrize/fixture 与 种子固定与 test 隔离 采集 通过率、flaky 率，把实测与 torch.testing.assert_close 在 |a−b| ≤ atol + rtol·|b| 时通过，float32 默认 rtol≈1.3e-6、atol≈1e-5 的理论值对比；oracle：测试在正确实现下稳定通过、在注入 bug 下必然失败，多次运行 flaky 率为 0。

#### 10. 评分标准
E022 评分：
- 不合格：只会复述术语，说不出支配等式与适用条件；
- 合格：能写出核心关系并说明 pytest 参数化与 fixture 与 assert_close 的 rtol/atol 的因果；
- 良好：能给出边界反例（容差过宽掩盖真实数值回归）、推导步骤与验证 oracle（测试在正确实现下稳定通过、在注入 bug 下必然失败，多次运行 flaky 率为 0。）；
- 优秀/专家：能补充误差模型、复杂度上界、跨精度/规模外推与残余风险。
### E023. 极端场景：张量/设备

- 难度：专家级
- 知识域：K01 张量/设备
- 考察能力：事件顺序、状态迁移、所有权与观测点
- 题型：流程/调用链/数据流
- 面试频率：开放题
- 前置知识：shape、stride、dtype、device
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

极端场景把 张量布局、视图语义与设备放置 的常规假设推向边界：规模、位宽、rank 数、序列长度、并发或数值范围。多数故障不在均值附近暴露，而在退化点（例如 expand 产生的零 stride 张量可视图化但不能原位写；转置后的张量不可直接 view，必须先 contiguous()。）出现。审题时要先明确把哪个变量推向极值，以及该极值下哪条假设先失效。

#### 2. 问题分析
1. 选定被推向极端的变量：stride 与 storage_offset 构成的地址映射、view/reshape/contiguous 的复制边界、memory_format 与 channels_last 中的哪一个，取值范围如何。
2. 在极值处重写支配关系：对连续张量有 stride[i]=∏_{j>i} shape[j]；view 只在目标 shape 能由既有 stride 整除分解时成立，否则 reshape 触发复制；实数占用满足 numel·itemsize ≤ storage.nbytes − storage_offset·itemsize。
3. 找出最先失效的假设并写出可观测症状，例如 对非连续张量调用 view 抛 RuntimeError: view size is not compatible with input tensor's size and stride。
4. 设计退化输入（size=0/1、非对齐、超长、单 rank、超大 batch）验证系统是否仍满足契约。

#### 3. 具体答案
面试中可以这样回答：

> 张量的本质是 (storage, size, stride, storage_offset) 四元组对一段连续内存的解释；视图与副本的区别决定是否共享存储、是否可安全原地写。设备放置是显式拓扑决策，.to() 按需搬运并可能触发隐式同步。
>
> 退化点：expand 产生的零 stride 张量可视图化但不能原位写；转置后的张量不可直接 view，必须先 contiguous()。
>
> 可验证事实：torch.Tensor.transpose 返回共享存储的视图（只互换 stride，不影响 storage），而 .reshape 在不可视图化时返回副本，可用 data_ptr() 是否相同判定。

#### 4. 技术洞察
极端场景的判据是条件数而非平均值：对连续张量有 stride[i]=∏_{j>i} shape[j]；view 只在目标 shape 能由既有 stride 整除分解时成立，否则 reshape 触发复制；实数占用满足 numel·itemsize ≤ storage.nbytes − storage_offset·itemsize。 在极端参数下需重新检验；必须先定位最先失效的前提（expand 产生的零 stride 张量可视图化但不能原位写；转置后的张量不可直接 view，必须先 contiguous()。）再谈缓解。

#### 5. 图文说明
| 层次 | 张量布局、视图语义与设备放置 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 张量、目标 shape/stride、device/dtype 与 memory_format | 约束缺失会误判视图可行性或越界读 |
| 执行与状态 | 追踪 storage、stride、offset 与 allocator 块的 owner 和生命周期 | 共享存储被原地写、碎片泄漏或隐式同步 |
| 输出与 oracle | 对照 data_ptr、is_contiguous 与显存曲线 | 把副本误当视图、峰值显存与理论值严重偏离 |

图示解释：把 张量布局、视图语义与设备放置 的输入量沿轴推到极值，观察 有效带宽 GB/s 是否出现阶跃或发散，从而定位退化边界。

#### 6. 拓展知识
相关容错手段：tensor.stride()/is_contiguous()/storage_offset()/data_ptr()、torch.cuda.memory_summary()/memory_snapshot()；对比正常与退化输入的恢复路径，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 当 stride 与 storage_offset 构成的地址映射 达到极值时，系统先出现错误、变慢还是静默错误，如何用 tensor.stride()/is_contiguous()/storage_offset()/data_ptr() 区分？
2. 退化输入下 有效带宽 GB/s 的预期拐点在哪里，如何设计最小复现？
3. 如果无法避免该极值，采取何种降级或保护（限幅、分块、回退 eager）最有效？

#### 8. 易错点与反例
1. 反例：只在正常规模测试，直到 对非连续张量调用 view 抛 RuntimeError: view size is not compatible with input tensor's size and stride 才发现退化。
2. 反例：用平均指标判断稳定性，忽略 在未同步的跨界 .to() 后立即读数据得到脏值。
3. 反例：把退化点的报错当作偶发而重试，掩盖 pin_memory 张量长期驻留耗尽锁页内存 的系统性缺陷。

#### 9. 实践与验证
验证方案：对同一逻辑张量构造 contiguous、转置、expand 三种布局，断言 is_contiguous/stride/storage_offset，并用 data_ptr 判定 reshape 是否复制。 额外注入退化输入（非对齐/极值/单 rank），用 tensor.stride()/is_contiguous()/storage_offset()/data_ptr() 与 nsys/ncu 的内存搬运与带宽视图 采集错误类别与 峰值显存 GiB；oracle：data_ptr 相同判为视图、不同判为副本；峰值显存与理论 numel·itemsize 的偏差落在分配器块对齐范围内。

#### 10. 评分标准
E023 评分：
- 不合格：只描述正常路径，说不出退化点在哪里；
- 合格：能指出一个极值变量与其失效症状（对非连续张量调用 view 抛 RuntimeError: view size is not compatible with input tensor's size and stride）；
- 良好：能给出退化边界的量化拐点、最小复现与保护手段；
- 优秀/专家：能补充条件数分析、分块/回退策略、监控阈值与残余风险。
### E024. 极端场景：autograd

- 难度：专家级
- 知识域：K02 autograd
- 考察能力：事件顺序、状态迁移、所有权与观测点
- 题型：流程/调用链/数据流
- 面试频率：开放题
- 前置知识：requires_grad、leaf、view、detach、反向
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

极端场景把 动态计算图、梯度累积与高阶微分 的常规假设推向边界：规模、位宽、rank 数、序列长度、并发或数值范围。多数故障不在均值附近暴露，而在退化点（例如 对同一张量第二次 backward 前必须 retain_graph=True；参数更新后再引用旧图会因版本不一致报错。）出现。审题时要先明确把哪个变量推向极值，以及该极值下哪条假设先失效。

#### 2. 问题分析
1. 选定被推向极端的变量：动态图与 grad_fn 链、叶张量与 .grad 累加、retain_graph/create_graph 生命周期 中的哪一个，取值范围如何。
2. 在极值处重写支配关系：反向传播沿 grad_fn 链执行链式法则 g_i=Σ_j g_j·(∂f_j/∂x_i)，梯度以累加方式写入 .grad（不清零则叠加）；对需要梯度的张量做 in-place 会使其 version counter 加一，若该版本被 backward 引用则抛 RuntimeError: a variable needed for gradient computation has been modified by an inplace operation。
3. 找出最先失效的假设并写出可观测症状，例如 detach 后仍期望梯度回传导致叶张量 .grad 为 None。
4. 设计退化输入（size=0/1、非对齐、超长、单 rank、超大 batch）验证系统是否仍满足契约。

#### 3. 具体答案
面试中可以这样回答：

> autograd 记录前向操作序列构成的动态图，反向时按逆序调用 grad_fn，用链式法则把上游梯度乘上局部雅可比后累加到叶张量。图的生命周期由 retain_graph 控制，原地写通过 version counter 保证数值一致。
>
> 退化点：对同一张量第二次 backward 前必须 retain_graph=True；参数更新后再引用旧图会因版本不一致报错。
>
> 可验证事实：torch.autograd.gradcheck 在 float64 下用有限差分（默认 eps=1e-6、rtol=1.3e-6、atol=1e-5）校验解析梯度，要求输入 requires_grad=True 且函数确定性。

#### 4. 技术洞察
极端场景的判据是条件数而非平均值：反向传播沿 grad_fn 链执行链式法则 g_i=Σ_j g_j·(∂f_j/∂x_i)，梯度以累加方式写入 .grad（不清零则叠加）；对需要梯度的张量做 in-place 会使其 version counter 加一，若该版本被 backward 引用则抛 RuntimeError: a variable needed for gradient computation has been modified by an inplace operation。 在极端参数下需重新检验；必须先定位最先失效的前提（对同一张量第二次 backward 前必须 retain_graph=True；参数更新后再引用旧图会因版本不一致报错。）再谈缓解。

#### 5. 图文说明
| 层次 | 动态计算图、梯度累积与高阶微分 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | requires_grad 状态、叶张量标志、是否原地写 | 误把非叶张量当叶、图已释放 |
| 执行与状态 | 遍历 grad_fn 链、version counter、saved tensor 所有权 | 图重复释放、原地写破坏反向、梯度断链 |
| 输出与 oracle | 比对解析梯度与数值差分、.grad 累加结果 | 梯度为零/None、误差超容差、二阶导缺失 |

图示解释：把 动态计算图、梯度累积与高阶微分 的输入量沿轴推到极值，观察 梯度相对误差 是否出现阶跃或发散，从而定位退化边界。

#### 6. 拓展知识
相关容错手段：tensor.grad_fn 与 next_functions 遍历、torch.autograd.anomaly_mode/set_detect_anomaly；对比正常与退化输入的恢复路径，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 当 动态图与 grad_fn 链 达到极值时，系统先出现错误、变慢还是静默错误，如何用 tensor.grad_fn 与 next_functions 遍历 区分？
2. 退化输入下 梯度相对误差 的预期拐点在哪里，如何设计最小复现？
3. 如果无法避免该极值，采取何种降级或保护（限幅、分块、回退 eager）最有效？

#### 8. 易错点与反例
1. 反例：只在正常规模测试，直到 detach 后仍期望梯度回传导致叶张量 .grad 为 None 才发现退化。
2. 反例：用平均指标判断稳定性，忽略 第二次 backward 未设 retain_graph=True 触发 Trying to backward through the graph a second time。
3. 反例：把退化点的报错当作偶发而重试，掩盖 对参与反向的张量原地写触发 version counter 报错 的系统性缺陷。

#### 9. 实践与验证
验证方案：构造 y=x·sin(x)，用 torch.autograd.grad 求一阶导并与有限差分对比；再开 create_graph=True 求二阶导，验证与解析式 cos(x)−x·sin(x) 一致。 额外注入退化输入（非对齐/极值/单 rank），用 tensor.grad_fn 与 next_functions 遍历 与 torch.autograd.grad 的 retain_graph/create_graph 采集错误类别与 峰值激活内存 MiB；oracle：gradcheck 返回 True（误差在 rtol=1e-3/atol=1e-5 内），二阶导相对误差 < 1e-4。

#### 10. 评分标准
E024 评分：
- 不合格：只描述正常路径，说不出退化点在哪里；
- 合格：能指出一个极值变量与其失效症状（detach 后仍期望梯度回传导致叶张量 .grad 为 None）；
- 良好：能给出退化边界的量化拐点、最小复现与保护手段；
- 优秀/专家：能补充条件数分析、分块/回退策略、监控阈值与残余风险。
### E025. 极端场景：Module/状态

- 难度：专家级
- 知识域：K03 Module/状态
- 考察能力：事件顺序、状态迁移、所有权与观测点
- 题型：流程/调用链/数据流
- 面试频率：开放题
- 前置知识：参数、buffer、模式和 state_dict
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

极端场景把 模块树、参数字典与状态持久化 的常规假设推向边界：规模、位宽、rank 数、序列长度、并发或数值范围。多数故障不在均值附近暴露，而在退化点（例如 多层共享同一 Parameter 时 state_dict 出现多个键指向同一存储，加载需处理别名与顺序。）出现。审题时要先明确把哪个变量推向极值，以及该极值下哪条假设先失效。

#### 2. 问题分析
1. 选定被推向极端的变量：Module 树与命名参数、parameter 与 buffer 的区别、state_dict 键空间 中的哪一个，取值范围如何。
2. 在极值处重写支配关系：state_dict 的键等于模块路径拼接参数/缓冲区名（如 layer1.0.conv.weight）；load_state_dict(strict=True) 要求键集合完全一致，否则报 missing/unexpected keys；buffer 不参与 optimizer 更新但会被 .to() 搬运。
3. 找出最先失效的假设并写出可观测症状，例如 buffer 未 register_buffer 导致不随 .to() 和 state_dict 迁移。
4. 设计退化输入（size=0/1、非对齐、超长、单 rank、超大 batch）验证系统是否仍满足契约。

#### 3. 具体答案
面试中可以这样回答：

> nn.Module 是一棵注册树，参数与缓冲区经命名空间形成扁平键空间；state_dict 是这棵树的快照契约，.to() 递归搬运所有注册张量。模式切换与注册完整性共同决定训练/推理一致性。
>
> 退化点：多层共享同一 Parameter 时 state_dict 出现多个键指向同一存储，加载需处理别名与顺序。
>
> 可验证事实：model.eval() 只切换 dropout/BatchNorm 等模块的 training 标志，不关闭 autograd；省显存必须叠加 torch.no_grad() 或 inference_mode()。

#### 4. 技术洞察
极端场景的判据是条件数而非平均值：state_dict 的键等于模块路径拼接参数/缓冲区名（如 layer1.0.conv.weight）；load_state_dict(strict=True) 要求键集合完全一致，否则报 missing/unexpected keys；buffer 不参与 optimizer 更新但会被 .to() 搬运。 在极端参数下需重新检验；必须先定位最先失效的前提（多层共享同一 Parameter 时 state_dict 出现多个键指向同一存储，加载需处理别名与顺序。）再谈缓解。

#### 5. 图文说明
| 层次 | 模块树、参数字典与状态持久化 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 模块结构、注册的参数/缓冲区、目标 device/dtype | 漏注册 buffer 或键空间不一致 |
| 执行与状态 | 遍历模块树、加载/保存快照、切换 training 标志 | 键缺失被静默、模式与图不匹配、别名错乱 |
| 输出与 oracle | 逐张量比对 state_dict、核对模式相关的统计 | 权重随机初始化、BN 统计漂移、显存未释放 |

图示解释：把 模块树、参数字典与状态持久化 的输入量沿轴推到极值，观察 参数量 是否出现阶跃或发散，从而定位退化边界。

#### 6. 拓展知识
相关容错手段：named_parameters()/named_buffers()/recurse、state_dict()/load_state_dict()；对比正常与退化输入的恢复路径，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 当 Module 树与命名参数 达到极值时，系统先出现错误、变慢还是静默错误，如何用 named_parameters()/named_buffers()/recurse 区分？
2. 退化输入下 参数量 的预期拐点在哪里，如何设计最小复现？
3. 如果无法避免该极值，采取何种降级或保护（限幅、分块、回退 eager）最有效？

#### 8. 易错点与反例
1. 反例：只在正常规模测试，直到 buffer 未 register_buffer 导致不随 .to() 和 state_dict 迁移 才发现退化。
2. 反例：用平均指标判断稳定性，忽略 共享参数被重复注册改变键空间与加载行为。
3. 反例：把退化点的报错当作偶发而重试，掩盖 strict=False 静默吞掉缺失键使部分权重随机初始化 的系统性缺陷。

#### 9. 实践与验证
验证方案：构造含卷积、BN、自定义 buffer 的模块，round-trip state_dict 到新实例并 assert_close 全部张量；删掉一个 buffer 验证 strict=True 报 missing keys。 额外注入退化输入（非对齐/极值/单 rank），用 named_parameters()/named_buffers()/recurse 与 module.apply/children/named_modules 采集错误类别与 键匹配率；oracle：load_state_dict 返回 missing=[]、unexpected=[]；train 模式两次 forward 后 BN running_mean 发生变化。

#### 10. 评分标准
E025 评分：
- 不合格：只描述正常路径，说不出退化点在哪里；
- 合格：能指出一个极值变量与其失效症状（buffer 未 register_buffer 导致不随 .to() 和 state_dict 迁移）；
- 良好：能给出退化边界的量化拐点、最小复现与保护手段；
- 优秀/专家：能补充条件数分析、分块/回退策略、监控阈值与残余风险。
### E026. 极端场景：数据管道

- 难度：专家级
- 知识域：K04 数据管道
- 考察能力：事件顺序、状态迁移、所有权与观测点
- 题型：流程/调用链/数据流
- 面试频率：开放题
- 前置知识：Dataset、Sampler、collate、padding、版本
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

极端场景把 DataLoader 的并行、采样与预取 的常规假设推向边界：规模、位宽、rank 数、序列长度、并发或数值范围。多数故障不在均值附近暴露，而在退化点（例如 IterableDataset 配 num_workers>1 时必须手动分片，否则每个 worker 读全量导致样本重复。）出现。审题时要先明确把哪个变量推向极值，以及该极值下哪条假设先失效。

#### 2. 问题分析
1. 选定被推向极端的变量：map-style 与 iterable-style Dataset、num_workers 与进程启动方式、sampler 与 shuffle 语义 中的哪一个，取值范围如何。
2. 在极值处重写支配关系：map-style 数据集每 epoch 批数 = ⌈N/batch_size⌉（drop_last 时为 ⌊N/batch_size⌋）；worker 是独立进程并各自持有 RNG，随机变换必须经 worker_init_fn 播种才可复现。
3. 找出最先失效的假设并写出可观测症状，例如 在 Dataset.__init__ 或 __getitem__ 中初始化 CUDA 导致 fork 后 worker 崩溃。
4. 设计退化输入（size=0/1、非对齐、超长、单 rank、超大 batch）验证系统是否仍满足契约。

#### 3. 具体答案
面试中可以这样回答：

> 数据管道把采样、取数、批处理和预取解耦到多 worker 进程；正确性取决于索引覆盖与随机性可复现，性能取决于预取深度与 H2D 重叠。
>
> 退化点：IterableDataset 配 num_workers>1 时必须手动分片，否则每个 worker 读全量导致样本重复。
>
> 可验证事实：pin_memory=True 配合 non_blocking=True 能把 H2D 拷贝与计算重叠；persistent_workers=True 在 epoch 间保留 worker 进程，避免反复 fork 开销。

#### 4. 技术洞察
极端场景的判据是条件数而非平均值：map-style 数据集每 epoch 批数 = ⌈N/batch_size⌉（drop_last 时为 ⌊N/batch_size⌋）；worker 是独立进程并各自持有 RNG，随机变换必须经 worker_init_fn 播种才可复现。 在极端参数下需重新检验；必须先定位最先失效的前提（IterableDataset 配 num_workers>1 时必须手动分片，否则每个 worker 读全量导致样本重复。）再谈缓解。

#### 5. 图文说明
| 层次 | DataLoader 的并行、采样与预取 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 数据集大小、batch_size、worker 数、采样方式 | 索引不覆盖、随机性不可复现 |
| 执行与状态 | 追踪 worker 进程、预取队列、pin 缓冲与 sampler 状态 | CUDA-in-fork 崩溃、句柄泄漏、顺序错乱 |
| 输出与 oracle | 核对样本集合、shape、吞吐与首 batch 延迟 | 重复/丢失样本、collate 形状错误、吞吐饱和异常 |

图示解释：把 DataLoader 的并行、采样与预取 的输入量沿轴推到极值，观察 吞吐 samples/s 是否出现阶跃或发散，从而定位退化边界。

#### 6. 拓展知识
相关容错手段：DataLoader/worker_init_fn/generator、torch.utils.data.get_worker_info；对比正常与退化输入的恢复路径，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 当 map-style 与 iterable-style Dataset 达到极值时，系统先出现错误、变慢还是静默错误，如何用 DataLoader/worker_init_fn/generator 区分？
2. 退化输入下 吞吐 samples/s 的预期拐点在哪里，如何设计最小复现？
3. 如果无法避免该极值，采取何种降级或保护（限幅、分块、回退 eager）最有效？

#### 8. 易错点与反例
1. 反例：只在正常规模测试，直到 在 Dataset.__init__ 或 __getitem__ 中初始化 CUDA 导致 fork 后 worker 崩溃 才发现退化。
2. 反例：用平均指标判断稳定性，忽略 worker 数过大引发内存或文件句柄耗尽。
3. 反例：把退化点的报错当作偶发而重试，掩盖 collate_fn 对不定长序列堆叠得到错误 shape 的系统性缺陷。

#### 9. 实践与验证
验证方案：固定 seed 与 worker_init_fn，连续两 epoch 记录样本索引序列验证覆盖完整且可复现；测量 num_workers∈{0,2,4,8} 的吞吐拐点。 额外注入退化输入（非对齐/极值/单 rank），用 DataLoader/worker_init_fn/generator 与 输入流水线 trace/cProfile 采集错误类别与 首 batch 延迟；oracle：两轮索引并集等于 0..N−1 且无重复，吞吐在某一 worker 数后饱和。

#### 10. 评分标准
E026 评分：
- 不合格：只描述正常路径，说不出退化点在哪里；
- 合格：能指出一个极值变量与其失效症状（在 Dataset.__init__ 或 __getitem__ 中初始化 CUDA 导致 fork 后 worker 崩溃）；
- 良好：能给出退化边界的量化拐点、最小复现与保护手段；
- 优秀/专家：能补充条件数分析、分块/回退策略、监控阈值与残余风险。
### E027. 极端场景：训练/优化器

- 难度：专家级
- 知识域：K05 训练/优化器
- 考察能力：事件顺序、状态迁移、所有权与观测点
- 题型：流程/调用链/数据流
- 面试频率：开放题
- 前置知识：forward、backward、累积、optimizer、scheduler
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

极端场景把 优化器更新、学习率调度与梯度处理 的常规假设推向边界：规模、位宽、rank 数、序列长度、并发或数值范围。多数故障不在均值附近暴露，而在退化点（例如 混合精度下先 clip 后 unscale，阈值按缩放后梯度计算，等价阈值被放大 S 倍。）出现。审题时要先明确把哪个变量推向极值，以及该极值下哪条假设先失效。

#### 2. 问题分析
1. 选定被推向极端的变量：优化器状态（Adam 的 m/v）、zero_grad 语义、梯度累积与有效批 中的哪一个，取值范围如何。
2. 在极值处重写支配关系：Adam：m_t=β1·m_{t−1}+(1−β1)·g_t，v_t=β2·v_{t−1}+(1−β2)·g_t²，偏差校正 m̂=m/(1−β1^t)、v̂=v/(1−β2^t)，θ←θ−lr·m̂/(√v̂+ε)；梯度累积 k 步等价于把有效批放大 k 倍，但必须对 loss 除以 k 或按 k 归约，否则梯度被放大 k 倍。
3. 找出最先失效的假设并写出可观测症状，例如 忘记 zero_grad 使梯度跨 step 累加，等效 lr 漂移。
4. 设计退化输入（size=0/1、非对齐、超长、单 rank、超大 batch）验证系统是否仍满足契约。

#### 3. 具体答案
面试中可以这样回答：

> 训练步是 zero_grad→forward→loss→scaled backward→unscale→clip→step→scheduler 的固定顺序；顺序错位会静默改变有效学习率或裁剪幅度。
>
> 退化点：混合精度下先 clip 后 unscale，阈值按缩放后梯度计算，等价阈值被放大 S 倍。
>
> 可验证事实：zero_grad(set_to_none=True) 把 .grad 置 None（自 PyTorch 2.0 起为默认行为），比置零更省内存；optimizer.state_dict() 按参数索引保存 momentum/m/v 缓冲区。

#### 4. 技术洞察
极端场景的判据是条件数而非平均值：Adam：m_t=β1·m_{t−1}+(1−β1)·g_t，v_t=β2·v_{t−1}+(1−β2)·g_t²，偏差校正 m̂=m/(1−β1^t)、v̂=v/(1−β2^t)，θ←θ−lr·m̂/(√v̂+ε)；梯度累积 k 步等价于把有效批放大 k 倍，但必须对 loss 除以 k 或按 k 归约，否则梯度被放大 k 倍。 在极端参数下需重新检验；必须先定位最先失效的前提（混合精度下先 clip 后 unscale，阈值按缩放后梯度计算，等价阈值被放大 S 倍。）再谈缓解。

#### 5. 图文说明
| 层次 | 优化器更新、学习率调度与梯度处理 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 初始参数、优化器配置、损失缩放与累积步 k | 顺序错位、未缩放 loss |
| 执行与状态 | 追踪 .grad、优化器状态缓冲区、lr 调度历史 | 梯度残留、动量错乱、裁剪幅度被缩放 |
| 输出与 oracle | 核对 loss 单调性、参数更新与 lr 序列 | loss 震荡、等效 lr 漂移、累积实现不等价 |

图示解释：把 优化器更新、学习率调度与梯度处理 的输入量沿轴推到极值，观察 loss 曲线 是否出现阶跃或发散，从而定位退化边界。

#### 6. 拓展知识
相关容错手段：optimizer.state_dict()/param_groups、scheduler.get_last_lr()；对比正常与退化输入的恢复路径，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 当 优化器状态（Adam 的 m/v） 达到极值时，系统先出现错误、变慢还是静默错误，如何用 optimizer.state_dict()/param_groups 区分？
2. 退化输入下 loss 曲线 的预期拐点在哪里，如何设计最小复现？
3. 如果无法避免该极值，采取何种降级或保护（限幅、分块、回退 eager）最有效？

#### 8. 易错点与反例
1. 反例：只在正常规模测试，直到 忘记 zero_grad 使梯度跨 step 累加，等效 lr 漂移 才发现退化。
2. 反例：用平均指标判断稳定性，忽略 scheduler.step 放错位置（每 batch 而非每 epoch）改变 lr 曲线。
3. 反例：把退化点的报错当作偶发而重试，掩盖 clip_grad_norm_ 在 GradScaler.unscale_ 之前调用使裁剪阈值被放大 S 倍 的系统性缺陷。

#### 9. 实践与验证
验证方案：在单个 batch 上跑 200 步验证 loss 单调下降到过拟合；打印 get_last_lr 序列核对调度曲线；对同一初始点比较累积实现与放大批实现的参数更新。 额外注入退化输入（非对齐/极值/单 rank），用 optimizer.state_dict()/param_groups 与 torch.optim.lr_scheduler 采集错误类别与 学习率序列；oracle：单批过拟合 loss < 0.01；累积与大批实现的参数差在 1e-6 内。

#### 10. 评分标准
E027 评分：
- 不合格：只描述正常路径，说不出退化点在哪里；
- 合格：能指出一个极值变量与其失效症状（忘记 zero_grad 使梯度跨 step 累加，等效 lr 漂移）；
- 良好：能给出退化边界的量化拐点、最小复现与保护手段；
- 优秀/专家：能补充条件数分析、分块/回退策略、监控阈值与残余风险。
### E028. 极端场景：AMP/恢复

- 难度：专家级
- 知识域：K06 AMP/恢复
- 考察能力：事件顺序、状态迁移、所有权与观测点
- 题型：流程/调用链/数据流
- 面试频率：开放题
- 前置知识：autocast、scaler、快照、随机状态
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

极端场景把 混合精度、检查点与确定性 的常规假设推向边界：规模、位宽、rank 数、序列长度、并发或数值范围。多数故障不在均值附近暴露，而在退化点（例如 梯度累积下 GradScaler 必须在累积窗口结束才 step/update，窗口内重复 unscale 会破坏缩放语义。）出现。审题时要先明确把哪个变量推向极值，以及该极值下哪条假设先失效。

#### 2. 问题分析
1. 选定被推向极端的变量：autocast 的算子白名单、GradScaler 动态缩放、bf16 与 fp16 数值范围 中的哪一个，取值范围如何。
2. 在极值处重写支配关系：GradScaler 先用 scale S 放大 loss 再反向，unscale_ 后若检测到 inf/nan 则跳过该 step 并把 S 减半；fp16 最正规数约 6.1e-5、次正规至约 6e-8，小于此量级的梯度下溢，故需缩放；bf16 指数位与 fp32 同为 8 位因此不需要缩放。
3. 找出最先失效的假设并写出可观测症状，例如 fp16 无 scaler 导致梯度下溢为 0 或 NaN。
4. 设计退化输入（size=0/1、非对齐、超长、单 rank、超大 batch）验证系统是否仍满足契约。

#### 3. 具体答案
面试中可以这样回答：

> AMP 通过 autocast 选低精度算子、GradScaler 维持梯度动态范围；恢复的正确性是状态闭包问题——模型、优化器、缩放与随机数状态缺一不可。
>
> 退化点：梯度累积下 GradScaler 必须在累积窗口结束才 step/update，窗口内重复 unscale 会破坏缩放语义。
>
> 可验证事实：torch.use_deterministic_algorithms(True) 会让部分算子回退到确定性实现或直接报错；cudnn.benchmark=True 的自动调优会破坏逐步可复现性。完整恢复需保存 model、optimizer、scaler、epoch、CPU 与 CUDA RNG 及 sampler 状态。

#### 4. 技术洞察
极端场景的判据是条件数而非平均值：GradScaler 先用 scale S 放大 loss 再反向，unscale_ 后若检测到 inf/nan 则跳过该 step 并把 S 减半；fp16 最正规数约 6.1e-5、次正规至约 6e-8，小于此量级的梯度下溢，故需缩放；bf16 指数位与 fp32 同为 8 位因此不需要缩放。 在极端参数下需重新检验；必须先定位最先失效的前提（梯度累积下 GradScaler 必须在累积窗口结束才 step/update，窗口内重复 unscale 会破坏缩放语义。）再谈缓解。

#### 5. 图文说明
| 层次 | 混合精度、检查点与确定性 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | dtype、autocast 区域、scaler 配置、checkpoint 内容 | 状态不闭合、精度选择错误 |
| 执行与状态 | 追踪 scale factor、RNG、优化器状态与 nan/inf 分支 | 下溢、NaN、恢复后状态漂移 |
| 输出与 oracle | 比对续训与基线参数、scale 曲线、依赖确定性开关 | 曲线跳变、结果不可复现、梯度全零 |

图示解释：把 混合精度、检查点与确定性 的输入量沿轴推到极值，观察 scale factor 曲线 是否出现阶跃或发散，从而定位退化边界。

#### 6. 拓展知识
相关容错手段：torch.cuda.amp.GradScaler 与 autocast、torch.get_rng_state/get_cuda_rng_state/numpy.random.get_state；对比正常与退化输入的恢复路径，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 当 autocast 的算子白名单 达到极值时，系统先出现错误、变慢还是静默错误，如何用 torch.cuda.amp.GradScaler 与 autocast 区分？
2. 退化输入下 scale factor 曲线 的预期拐点在哪里，如何设计最小复现？
3. 如果无法避免该极值，采取何种降级或保护（限幅、分块、回退 eager）最有效？

#### 8. 易错点与反例
1. 反例：只在正常规模测试，直到 fp16 无 scaler 导致梯度下溢为 0 或 NaN 才发现退化。
2. 反例：用平均指标判断稳定性，忽略 resume 只存模型权重丢失优化器动量使曲线跳变。
3. 反例：把退化点的报错当作偶发而重试，掩盖 只存 torch RNG 未存 numpy/random 状态导致数据顺序不同 的系统性缺陷。

#### 9. 实践与验证
验证方案：训练 50 步保存完整 checkpoint，重启加载后再跑 50 步与不中断基线逐参数比较；fp16 下注入大梯度观测 S 是否减半。 额外注入退化输入（非对齐/极值/单 rank），用 torch.cuda.amp.GradScaler 与 autocast 与 CUBLAS_WORKSPACE_CONFIG=:4096:8 采集错误类别与 resume 前后参数最大差；oracle：续训与基线参数最大差 < 1e-5（bit 级取决于确定性开关），scale factor 与跳过计数一致。

#### 10. 评分标准
E028 评分：
- 不合格：只描述正常路径，说不出退化点在哪里；
- 合格：能指出一个极值变量与其失效症状（fp16 无 scaler 导致梯度下溢为 0 或 NaN）；
- 良好：能给出退化边界的量化拐点、最小复现与保护手段；
- 优秀/专家：能补充条件数分析、分块/回退策略、监控阈值与残余风险。
### E029. 极端场景：分布式

- 难度：专家级
- 知识域：K07 分布式
- 考察能力：事件顺序、状态迁移、所有权与观测点
- 题型：流程/调用链/数据流
- 面试频率：开放题
- 前置知识：rank、DDP、FSDP、collective、容错
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

极端场景把 进程组、集合通信与梯度同步 的常规假设推向边界：规模、位宽、rank 数、序列长度、并发或数值范围。多数故障不在均值附近暴露，而在退化点（例如 find_unused_parameters=True 会在反向前后额外遍历图，掩盖未参与反向的参数但增加开销。）出现。审题时要先明确把哪个变量推向极值，以及该极值下哪条假设先失效。

#### 2. 问题分析
1. 选定被推向极端的变量：进程组与 rank/world_size、all_reduce/reduce_scatter/all_gather、NCCL 与 gloo 后端 中的哪一个，取值范围如何。
2. 在极值处重写支配关系：DDP 默认 all_reduce(sum) 后除以 world_size 得平均梯度，故全局有效批 = 本地批 × world_size × 累积步数；ring all_reduce 单步通信量 ≈ 2·(N−1)/N·S（S 为梯度字节数），大 N 时趋近 2S。
3. 找出最先失效的假设并写出可观测症状，例如 各 rank 集合通信调用不匹配导致 hang 直到 watchdog 超时。
4. 设计退化输入（size=0/1、非对齐、超长、单 rank、超大 batch）验证系统是否仍满足契约。

#### 3. 具体答案
面试中可以这样回答：

> 分布式训练把单机步拆成计算→梯度归约→更新三段；正确性在于所有 rank 以相同顺序进入集合通信且梯度按 world_size 归一，性能在于把通信藏进反向的尾部。
>
> 退化点：find_unused_parameters=True 会在反向前后额外遍历图，掩盖未参与反向的参数但增加开销。
>
> 可验证事实：NCCL 只支持 CUDA 张量、gloo 支持 CPU；DDP 默认把梯度分成约 25 MiB 的桶以便与反向计算重叠通信。

#### 4. 技术洞察
极端场景的判据是条件数而非平均值：DDP 默认 all_reduce(sum) 后除以 world_size 得平均梯度，故全局有效批 = 本地批 × world_size × 累积步数；ring all_reduce 单步通信量 ≈ 2·(N−1)/N·S（S 为梯度字节数），大 N 时趋近 2S。 在极端参数下需重新检验；必须先定位最先失效的前提（find_unused_parameters=True 会在反向前后额外遍历图，掩盖未参与反向的参数但增加开销。）再谈缓解。

#### 5. 图文说明
| 层次 | 进程组、集合通信与梯度同步 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 进程数、后端、张量是否 CUDA、参与反向的参数集合 | 后端不匹配、参数未参与、集合调用不一致 |
| 执行与状态 | 追踪 rank、bucket、通信算子与反向重叠 | hang、梯度未归约、通信未重叠 |
| 输出与 oracle | 对比单卡/DDP 梯度、通信带宽与 step 方差 | 梯度差超容差、超时、吞吐不随卡数提升 |

图示解释：把 进程组、集合通信与梯度同步 的输入量沿轴推到极值，观察 all_reduce 带宽 GB/s 是否出现阶跃或发散，从而定位退化边界。

#### 6. 拓展知识
相关容错手段：torch.distributed.init_process_group/barrier、dist.all_reduce/all_gather/reduce_scatter；对比正常与退化输入的恢复路径，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 当 进程组与 rank/world_size 达到极值时，系统先出现错误、变慢还是静默错误，如何用 torch.distributed.init_process_group/barrier 区分？
2. 退化输入下 all_reduce 带宽 GB/s 的预期拐点在哪里，如何设计最小复现？
3. 如果无法避免该极值，采取何种降级或保护（限幅、分块、回退 eager）最有效？

#### 8. 易错点与反例
1. 反例：只在正常规模测试，直到 各 rank 集合通信调用不匹配导致 hang 直到 watchdog 超时 才发现退化。
2. 反例：用平均指标判断稳定性，忽略 未按 world_size 归一梯度使等效 lr 随卡数变化。
3. 反例：把退化点的报错当作偶发而重试，掩盖 NCCL/驱动版本不一致引发通信超时或崩溃 的系统性缺陷。

#### 9. 实践与验证
验证方案：用 torchrun 起 2 进程，固定初始化使单卡与 DDP 梯度可对齐，比较 all_reduce 前后梯度；人为让一个 rank 提前退出观测 NCCL watchdog 超时。 额外注入退化输入（非对齐/极值/单 rank），用 torch.distributed.init_process_group/barrier 与 torchrun 采集错误类别与 step 时间方差；oracle：DDP 与单卡初始梯度（除以 world_size 后）相对误差 < 1e-6；NCCL_DEBUG 显示预期的 ring/tree 算法。

#### 10. 评分标准
E029 评分：
- 不合格：只描述正常路径，说不出退化点在哪里；
- 合格：能指出一个极值变量与其失效症状（各 rank 集合通信调用不匹配导致 hang 直到 watchdog 超时）；
- 良好：能给出退化边界的量化拐点、最小复现与保护手段；
- 优秀/专家：能补充条件数分析、分块/回退策略、监控阈值与残余风险。
### E030. 极端场景：编译栈

- 难度：专家级
- 知识域：K08 编译栈
- 考察能力：事件顺序、状态迁移、所有权与观测点
- 题型：流程/调用链/数据流
- 面试频率：开放题
- 前置知识：Dynamo、AOTAutograd、Inductor、guard、缓存
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

极端场景把 Dynamo 捕获、guard 与 Inductor 代码生成 的常规假设推向边界：规模、位宽、rank 数、序列长度、并发或数值范围。多数故障不在均值附近暴露，而在退化点（例如 把张量属性（如 .shape 的某维）当 Python 常量使用会特化成守卫，改变该维度即重编译。）出现。审题时要先明确把哪个变量推向极值，以及该极值下哪条假设先失效。

#### 2. 问题分析
1. 选定被推向极端的变量：TorchDynamo 的字节码捕获、guard 与 specialization、graph break 与子图 中的哪一个，取值范围如何。
2. 在极值处重写支配关系：一个编译单元的 guard 绑定在张量的 shape/dtype/device 与 Python 常量上；任一 guard 失效即触发重编译，默认累计重编译上限约 8 次（torch._dynamo.config.accumulated_recompile_limit）。图被 graph break 切成多个子图，未捕获区间回退 eager。
3. 找出最先失效的假设并写出可观测症状，例如 在图中调用 .item()/print/依赖数据的 Python 分支触发 graph break。
4. 设计退化输入（size=0/1、非对齐、超长、单 rank、超大 batch）验证系统是否仍满足契约。

#### 3. 具体答案
面试中可以这样回答：

> 编译栈把 Python 执行轨迹捕获成 FX 图，用 guard 保证图对后续输入仍有效；失效即重编译、无法捕获即断图。稳定收益来自低断点数与低重编译率而非一次加速。
>
> 退化点：把张量属性（如 .shape 的某维）当 Python 常量使用会特化成守卫，改变该维度即重编译。
>
> 可验证事实：设置 TORCH_LOGS=graph_breaks,guards,recompiles 可打印断点与重编译原因；torch._dynamo.explain(fn)(*args) 返回断点数量与原因；dynamic=True 或 torch._dynamo.mark_dynamic 可放宽形状守卫。

#### 4. 技术洞察
极端场景的判据是条件数而非平均值：一个编译单元的 guard 绑定在张量的 shape/dtype/device 与 Python 常量上；任一 guard 失效即触发重编译，默认累计重编译上限约 8 次（torch._dynamo.config.accumulated_recompile_limit）。图被 graph break 切成多个子图，未捕获区间回退 eager。 在极端参数下需重新检验；必须先定位最先失效的前提（把张量属性（如 .shape 的某维）当 Python 常量使用会特化成守卫，改变该维度即重编译。）再谈缓解。

#### 5. 图文说明
| 层次 | Dynamo 捕获、guard 与 Inductor 代码生成 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 函数代码、示例输入、shape/dtype/device 与 dynamic 声明 | 守卫过严、断点隐藏、动态维度未声明 |
| 执行与状态 | 追踪 Dynamo 捕获、guard 集合、子图与 Inductor 缓存 | 重编译风暴、断点回退 eager、缓存未命中 |
| 输出与 oracle | 对比 compiled/eager 数值、重编译与断点计数 | 数值不一致、加速比 < 1、重编译失控 |

图示解释：把 Dynamo 捕获、guard 与 Inductor 代码生成 的输入量沿轴推到极值，观察 重编译次数 是否出现阶跃或发散，从而定位退化边界。

#### 6. 拓展知识
相关容错手段：TORCH_LOGS=graph_breaks,guards,recompiles、torch._dynamo.explain；对比正常与退化输入的恢复路径，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 当 TorchDynamo 的字节码捕获 达到极值时，系统先出现错误、变慢还是静默错误，如何用 TORCH_LOGS=graph_breaks,guards,recompiles 区分？
2. 退化输入下 重编译次数 的预期拐点在哪里，如何设计最小复现？
3. 如果无法避免该极值，采取何种降级或保护（限幅、分块、回退 eager）最有效？

#### 8. 易错点与反例
1. 反例：只在正常规模测试，直到 在图中调用 .item()/print/依赖数据的 Python 分支触发 graph break 才发现退化。
2. 反例：用平均指标判断稳定性，忽略 动态 shape 引发重编译风暴使性能倒挂。
3. 反例：把退化点的报错当作偶发而重试，掩盖 别名或原地写使编译结果与 eager 数值不一致 的系统性缺陷。

#### 9. 实践与验证
验证方案：对同一函数用固定 shape 与变化 shape 各跑 20 次，用 explain 统计 graph break 与重编译；对比 compiled 与 eager 输出最大差。 额外注入退化输入（非对齐/极值/单 rank），用 TORCH_LOGS=graph_breaks,guards,recompiles 与 torch._inductor.config 采集错误类别与 compiled/eager 加速比；oracle：固定 shape 下重编译次数为 1、输出与 eager 最大差 < 1e-4；变化 shape 下开启 dynamic 后重编译显著下降。

#### 10. 评分标准
E030 评分：
- 不合格：只描述正常路径，说不出退化点在哪里；
- 合格：能指出一个极值变量与其失效症状（在图中调用 .item()/print/依赖数据的 Python 分支触发 graph break）；
- 良好：能给出退化边界的量化拐点、最小复现与保护手段；
- 优秀/专家：能补充条件数分析、分块/回退策略、监控阈值与残余风险。
### E031. 极端场景：性能

- 难度：专家级
- 知识域：K09 性能
- 考察能力：前置条件、后置条件、断言与错误处理
- 题型：代码/伪代码/配置分析
- 面试频率：开放题
- 前置知识：关键路径、带宽、算力、通信、显存、统计
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

极端场景把 性能剖析、roofline 与显存容量 的常规假设推向边界：规模、位宽、rank 数、序列长度、并发或数值范围。多数故障不在均值附近暴露，而在退化点（例如 D2D 拷贝带宽受 ECC 与读写混合影响通常显著低于理论峰值，小 size 下 launch 开销占主导。）出现。审题时要先明确把哪个变量推向极值，以及该极值下哪条假设先失效。

#### 2. 问题分析
1. 选定被推向极端的变量：CUDA event 计时与同步、roofline 与算术强度、kernel 融合与 launch 开销 中的哪一个，取值范围如何。
2. 在极值处重写支配关系：算术强度 I=FLOPs/Bytes，roofline 上界 = min(峰值算力 P, I×峰值带宽 BW)，据此判定 compute-bound 或 memory-bound；CUDA event 计时为 t=elapsed_time(start,stop)，异步操作必须 torch.cuda.synchronize() 后计时才可信。
3. 找出最先失效的假设并写出可观测症状，例如 异步未同步导致计时偏短，得出虚假加速。
4. 设计退化输入（size=0/1、非对齐、超长、单 rank、超大 batch）验证系统是否仍满足契约。

#### 3. 具体答案
面试中可以这样回答：

> 性能结论必须绑定可复现的测量协议：warmup、同步、重复次数、分位数；再用 roofline 判定算力受限还是带宽受限，最后才谈优化手段。
>
> 退化点：D2D 拷贝带宽受 ECC 与读写混合影响通常显著低于理论峰值，小 size 下 launch 开销占主导。
>
> 可验证事实：torch.profiler.profile(activities=[ProfilerActivity.CUDA]) 会插入 CUDA 事件并输出按算子聚合的 self/total CUDA time；首次迭代含 cuBLAS/context 初始化与自动调优，必须先 warmup 再计时。

#### 4. 技术洞察
极端场景的判据是条件数而非平均值：算术强度 I=FLOPs/Bytes，roofline 上界 = min(峰值算力 P, I×峰值带宽 BW)，据此判定 compute-bound 或 memory-bound；CUDA event 计时为 t=elapsed_time(start,stop)，异步操作必须 torch.cuda.synchronize() 后计时才可信。 在极端参数下需重新检验；必须先定位最先失效的前提（D2D 拷贝带宽受 ECC 与读写混合影响通常显著低于理论峰值，小 size 下 launch 开销占主导。）再谈缓解。

#### 5. 图文说明
| 层次 | 性能剖析、roofline 与显存容量 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 算子/规模、dtype、warmup 与 repeat、计时方式 | 未同步、未 warmup、规模误导 |
| 执行与状态 | 追踪 kernel 时长、显存曲线、launch 与同步点 | 计时偏短、碎片抖动、把 launch 开销当算力 |
| 输出与 oracle | 对比 event/profiler、roofline 定位与分位数 | 加速比不稳定、有效带宽超理论值、尾延迟超标 |

图示解释：把 性能剖析、roofline 与显存容量 的输入量沿轴推到极值，观察 p50/p99 延迟 是否出现阶跃或发散，从而定位退化边界。

#### 6. 拓展知识
相关容错手段：torch.profiler、torch.cuda.Event/elapsed_time；对比正常与退化输入的恢复路径，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 当 CUDA event 计时与同步 达到极值时，系统先出现错误、变慢还是静默错误，如何用 torch.profiler 区分？
2. 退化输入下 p50/p99 延迟 的预期拐点在哪里，如何设计最小复现？
3. 如果无法避免该极值，采取何种降级或保护（限幅、分块、回退 eager）最有效？

#### 8. 易错点与反例
1. 反例：只在正常规模测试，直到 异步未同步导致计时偏短，得出虚假加速 才发现退化。
2. 反例：用平均指标判断稳定性，忽略 把 profiler 自身开销计入结论。
3. 反例：把退化点的报错当作偶发而重试，掩盖 只测单次忽略冷启动与首次调优 的系统性缺陷。

#### 9. 实践与验证
验证方案：对 matmul、逐元素、拷贝分别用 CUDA event 与 profiler 计时，warmup=10、repeat=100、取中位数；用 bytes/time 反推有效带宽并与理论比较。 额外注入退化输入（非对齐/极值/单 rank），用 torch.profiler 与 torch.cuda.max_memory_allocated 采集错误类别与 MFU/TFLOPS；oracle：event 计时与 profiler 的 CUDA time 偏差 < 5%，大 size 下有效带宽落在理论峰值的可解释比例内。

#### 10. 评分标准
E031 评分：
- 不合格：只描述正常路径，说不出退化点在哪里；
- 合格：能指出一个极值变量与其失效症状（异步未同步导致计时偏短，得出虚假加速）；
- 良好：能给出退化边界的量化拐点、最小复现与保护手段；
- 优秀/专家：能补充条件数分析、分块/回退策略、监控阈值与残余风险。
### E032. 极端场景：导出部署

- 难度：专家级
- 知识域：K10 导出部署
- 考察能力：前置条件、后置条件、断言与错误处理
- 题型：代码/伪代码/配置分析
- 面试频率：开放题
- 前置知识：state_dict、Export、runtime、schema、迁移
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

极端场景把 图导出、图签名与跨版本加载 的常规假设推向边界：规模、位宽、rank 数、序列长度、并发或数值范围。多数故障不在均值附近暴露，而在退化点（例如 含数据相关控制流的模型用 trace 会丢分支，需要 script 或改写为导出友好的条件算子。）出现。审题时要先明确把哪个变量推向极值，以及该极值下哪条假设先失效。

#### 2. 问题分析
1. 选定被推向极端的变量：torch.export/ExportedProgram、torch.jit.trace/script、ONNX opset 与 dynamic_axes 中的哪一个，取值范围如何。
2. 在极值处重写支配关系：导出图是函数式的：无 Python 副作用，输入输出由 graph_signature 声明；torch.export 的功能化把原地写改写为显式输出。ONNX 的算子覆盖度由 opset 版本决定，超出该 opset 的算子导出即失败。
3. 找出最先失效的假设并写出可观测症状，例如 trace 未覆盖控制流分支导致逻辑被固化到单一分支。
4. 设计退化输入（size=0/1、非对齐、超长、单 rank、超大 batch）验证系统是否仍满足契约。

#### 3. 具体答案
面试中可以这样回答：

> 导出把可变的 Python 模块冻结成只含 aten 算子的静态图，并用水印式签名声明输入输出契约；正确性等于导出图在目标 runtime 与 eager 数值一致，兼容性等于签名与 opset 能跨版本被消费。
>
> 退化点：含数据相关控制流的模型用 trace 会丢分支，需要 script 或改写为导出友好的条件算子。
>
> 可验证事实：torch.export 需要示例输入，返回含 graph_module、range_constraints 与 signature 的 ExportedProgram；torch.jit.trace 只记录执行过的算子，遇控制流会固化当时走的分支。

#### 4. 技术洞察
极端场景的判据是条件数而非平均值：导出图是函数式的：无 Python 副作用，输入输出由 graph_signature 声明；torch.export 的功能化把原地写改写为显式输出。ONNX 的算子覆盖度由 opset 版本决定，超出该 opset 的算子导出即失败。 在极端参数下需重新检验；必须先定位最先失效的前提（含数据相关控制流的模型用 trace 会丢分支，需要 script 或改写为导出友好的条件算子。）再谈缓解。

#### 5. 图文说明
| 层次 | 图导出、图签名与跨版本加载 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 模型、示例输入、dynamic 声明与目标 opset/runtime | 控制流未覆盖、动态维度缺声明 |
| 执行与状态 | 追踪图捕获、功能化改写、签名与序列化 | 分支固化、opset 缺算子、加载执行任意代码 |
| 输出与 oracle | 对比导出/eager 输出、校验签名与 checker | 数值偏差、改变维度即失效、导出被拒绝 |

图示解释：把 图导出、图签名与跨版本加载 的输入量沿轴推到极值，观察 导出成功率 是否出现阶跃或发散，从而定位退化边界。

#### 6. 拓展知识
相关容错手段：torch.export/exported.run_decompositions、torch.onnx.export；对比正常与退化输入的恢复路径，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 当 torch.export/ExportedProgram 达到极值时，系统先出现错误、变慢还是静默错误，如何用 torch.export/exported.run_decompositions 区分？
2. 退化输入下 导出成功率 的预期拐点在哪里，如何设计最小复现？
3. 如果无法避免该极值，采取何种降级或保护（限幅、分块、回退 eager）最有效？

#### 8. 易错点与反例
1. 反例：只在正常规模测试，直到 trace 未覆盖控制流分支导致逻辑被固化到单一分支 才发现退化。
2. 反例：用平均指标判断稳定性，忽略 dynamic shape 未显式声明使其被编译成常量。
3. 反例：把退化点的报错当作偶发而重试，掩盖 opset 过低缺失算子导致导出失败 的系统性缺陷。

#### 9. 实践与验证
验证方案：对同一模型分别 torch.export 与 jit.trace，跑固定输入比对输出；改变一个输入维度检验 dynamic_shapes 是否被尊重；用 onnx.checker 校验导出图。 额外注入退化输入（非对齐/极值/单 rank），用 torch.export/exported.run_decompositions 与 onnx.checker 与 shape inference 采集错误类别与 eager 与导出输出误差；oracle：导出输出与 eager 最大差 < 1e-5，声明为 dynamic 的维度改变时无需重导。

#### 10. 评分标准
E032 评分：
- 不合格：只描述正常路径，说不出退化点在哪里；
- 合格：能指出一个极值变量与其失效症状（trace 未覆盖控制流分支导致逻辑被固化到单一分支）；
- 良好：能给出退化边界的量化拐点、最小复现与保护手段；
- 优秀/专家：能补充条件数分析、分块/回退策略、监控阈值与残余风险。
### E033. 极端场景：自定义算子

- 难度：专家级
- 知识域：K11 自定义算子
- 考察能力：前置条件、后置条件、断言与错误处理
- 题型：代码/伪代码/配置分析
- 面试频率：开放题
- 前置知识：schema、dispatcher、stream、autograd、ABI
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

极端场景把 算子注册、调度与数值正确性 的常规假设推向边界：规模、位宽、rank 数、序列长度、并发或数值范围。多数故障不在均值附近暴露，而在退化点（例如 归约类算子用 atomicAdd 会产生非确定性浮点累加顺序，需固定归约树或接受容差。）出现。审题时要先明确把哪个变量推向极值，以及该极值下哪条假设先失效。

#### 2. 问题分析
1. 选定被推向极端的变量：torch.library/TORCH_LIBRARY 注册、autograd.Function 与双反向、AT_DISPATCH 与设备守卫 中的哪一个，取值范围如何。
2. 在极值处重写支配关系：CUDA kernel 启动取 blocks=⌈N/threads⌉、threads≤1024 且为 32 的倍数；自定义算子若参与训练必须同时提供 forward 与 backward（或注册 autograd 公式），否则梯度断链；访问需满足 0≤idx<N 防越界。
3. 找出最先失效的假设并写出可观测症状，例如 只写 forward 未注册 backward 使梯度静默为零。
4. 设计退化输入（size=0/1、非对齐、超长、单 rank、超大 batch）验证系统是否仍满足契约。

#### 3. 具体答案
面试中可以这样回答：

> 自定义算子是 dispatcher 上的一个实现对：前向 kernel 负责数值、反向公式负责梯度；正确性由边界条件、设备守卫与 gradcheck 共同保证。
>
> 退化点：归约类算子用 atomicAdd 会产生非确定性浮点累加顺序，需固定归约树或接受容差。
>
> 可验证事实：torch.utils.cpp_extension.load/load_inline 首次调用时编译并缓存扩展；缺少 TORCH_CHECK 的越界访问可能不报错而返回垃圾数据或破坏相邻内存。

#### 4. 技术洞察
极端场景的判据是条件数而非平均值：CUDA kernel 启动取 blocks=⌈N/threads⌉、threads≤1024 且为 32 的倍数；自定义算子若参与训练必须同时提供 forward 与 backward（或注册 autograd 公式），否则梯度断链；访问需满足 0≤idx<N 防越界。 在极端参数下需重新检验；必须先定位最先失效的前提（归约类算子用 atomicAdd 会产生非确定性浮点累加顺序，需固定归约树或接受容差。）再谈缓解。

#### 5. 图文说明
| 层次 | 算子注册、调度与数值正确性 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 张量 shape/dtype/device、线程与 grid 配置、边界条件 | 越界、设备不匹配、dtype 未分派 |
| 执行与状态 | 追踪 kernel 启动、CUDAGuard、dispatcher 分派与归约 | 越界写、跑错设备、梯度断链 |
| 输出与 oracle | 对比参考实现、gradcheck 与 sanitizer 报告 | 误差超标、梯度为零、随机错误 |

图示解释：把 算子注册、调度与数值正确性 的输入量沿轴推到极值，观察 相对误差 是否出现阶跃或发散，从而定位退化边界。

#### 6. 拓展知识
相关容错手段：torch.utils.cpp_extension.load/load_inline、AT_DISPATCH_FLOATING_TYPES；对比正常与退化输入的恢复路径，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 当 torch.library/TORCH_LIBRARY 注册 达到极值时，系统先出现错误、变慢还是静默错误，如何用 torch.utils.cpp_extension.load/load_inline 区分？
2. 退化输入下 相对误差 的预期拐点在哪里，如何设计最小复现？
3. 如果无法避免该极值，采取何种降级或保护（限幅、分块、回退 eager）最有效？

#### 8. 易错点与反例
1. 反例：只在正常规模测试，直到 只写 forward 未注册 backward 使梯度静默为零 才发现退化。
2. 反例：用平均指标判断稳定性，忽略 kernel 越界写破坏相邻显存造成随机错误。
3. 反例：把退化点的报错当作偶发而重试，掩盖 缺少 CUDAGuard 在多卡环境跑错设备 的系统性缺陷。

#### 9. 实践与验证
验证方案：实现逐元素算子并与 eager 参考比较，用 gradcheck 校验反向；把 N 设为非 warp 倍数检验尾块处理，跑 compute-sanitizer。 额外注入退化输入（非对齐/极值/单 rank），用 torch.utils.cpp_extension.load/load_inline 与 torch.autograd.Function 与 gradcheck 采集错误类别与 compute-sanitizer 命中数；oracle：fp32 最大绝对误差 < 1e-6、gradcheck 通过，N=线程数±1 时无越界且结果正确。

#### 10. 评分标准
E033 评分：
- 不合格：只描述正常路径，说不出退化点在哪里；
- 合格：能指出一个极值变量与其失效症状（只写 forward 未注册 backward 使梯度静默为零）；
- 良好：能给出退化边界的量化拐点、最小复现与保护手段；
- 优秀/专家：能补充条件数分析、分块/回退策略、监控阈值与残余风险。
### E034. 极端场景：测试治理

- 难度：专家级
- 知识域：K12 测试治理
- 考察能力：前置条件、后置条件、断言与错误处理
- 题型：代码/伪代码/配置分析
- 面试频率：开放题
- 前置知识：oracle、证据、日志、回滚、门禁、复盘
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

极端场景把 测试契约、容差与证据化调试 的常规假设推向边界：规模、位宽、rank 数、序列长度、并发或数值范围。多数故障不在均值附近暴露，而在退化点（例如 对随机初始化模型直接比数值会误报，需先固定权重或用对初始化不敏感的不变量（shape/单调性/守恒量）。）出现。审题时要先明确把哪个变量推向极值，以及该极值下哪条假设先失效。

#### 2. 问题分析
1. 选定被推向极端的变量：pytest 参数化与 fixture、assert_close 的 rtol/atol、确定性种子与测试隔离 中的哪一个，取值范围如何。
2. 在极值处重写支配关系：torch.testing.assert_close 在 |a−b| ≤ atol + rtol·|b| 时通过，float32 默认 rtol≈1.3e-6、atol≈1e-5；flaky 率 = 失败重跑数/总重跑数，需按阈值门禁。
3. 找出最先失效的假设并写出可观测症状，例如 容差过宽掩盖真实数值回归。
4. 设计退化输入（size=0/1、非对齐、超长、单 rank、超大 batch）验证系统是否仍满足契约。

#### 3. 具体答案
面试中可以这样回答：

> 测试是把需求写成可执行的契约；有效的门禁需要确定性、隔离、明确容差和可定位失败的证据，而不是只断言最终数值。
>
> 退化点：对随机初始化模型直接比数值会误报，需先固定权重或用对初始化不敏感的不变量（shape/单调性/守恒量）。
>
> 可验证事实：pytest -k 按名筛选、-x 首次失败即停、--reruns 需 pytest-rerunfailures 插件；未设种子的随机测试在 CI 中会产生不可复现失败。

#### 4. 技术洞察
极端场景的判据是条件数而非平均值：torch.testing.assert_close 在 |a−b| ≤ atol + rtol·|b| 时通过，float32 默认 rtol≈1.3e-6、atol≈1e-5；flaky 率 = 失败重跑数/总重跑数，需按阈值门禁。 在极端参数下需重新检验；必须先定位最先失效的前提（对随机初始化模型直接比数值会误报，需先固定权重或用对初始化不敏感的不变量（shape/单调性/守恒量）。）再谈缓解。

#### 5. 图文说明
| 层次 | 测试契约、容差与证据化调试 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 被测模块、参数组合、种子、容差与基线 | 容差过宽、随机未固定、基线过期 |
| 执行与状态 | 追踪 fixture 生命周期、全局状态与断言点 | 顺序相关、静默 skip、只测终值 |
| 输出与 oracle | 核对断言、失败可定位性与 flaky 统计 | 假绿、不可复现失败、回归漏检 |

图示解释：把 测试契约、容差与证据化调试 的输入量沿轴推到极值，观察 通过率 是否出现阶跃或发散，从而定位退化边界。

#### 6. 拓展知识
相关容错手段：pytest/parametrize/fixture、torch.testing.assert_close；对比正常与退化输入的恢复路径，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 当 pytest 参数化与 fixture 达到极值时，系统先出现错误、变慢还是静默错误，如何用 pytest/parametrize/fixture 区分？
2. 退化输入下 通过率 的预期拐点在哪里，如何设计最小复现？
3. 如果无法避免该极值，采取何种降级或保护（限幅、分块、回退 eager）最有效？

#### 8. 易错点与反例
1. 反例：只在正常规模测试，直到 容差过宽掩盖真实数值回归 才发现退化。
2. 反例：用平均指标判断稳定性，忽略 测试间共享全局状态导致顺序相关失败。
3. 反例：把退化点的报错当作偶发而重试，掩盖 无 GPU 时静默 skip 使 CI 假绿 的系统性缺陷。

#### 9. 实践与验证
验证方案：为某模块写参数化测试（多 shape/dtype），固定种子并用加严容差的 assert_close；注入一个已知 bug 验证测试能失败。 额外注入退化输入（非对齐/极值/单 rank），用 pytest/parametrize/fixture 与 CI 报告与覆盖率 采集错误类别与 覆盖率；oracle：测试在正确实现下稳定通过、在注入 bug 下必然失败，多次运行 flaky 率为 0。

#### 10. 评分标准
E034 评分：
- 不合格：只描述正常路径，说不出退化点在哪里；
- 合格：能指出一个极值变量与其失效症状（容差过宽掩盖真实数值回归）；
- 良好：能给出退化边界的量化拐点、最小复现与保护手段；
- 优秀/专家：能补充条件数分析、分块/回退策略、监控阈值与残余风险。
### E035. 长期演进：张量/设备

- 难度：专家级
- 知识域：K01 张量/设备
- 考察能力：前置条件、后置条件、断言与错误处理
- 题型：代码/伪代码/配置分析
- 面试频率：开放题
- 前置知识：shape、stride、dtype、device
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

长期演进关注 张量布局、视图语义与设备放置 在版本、硬件与业务变化下的可持续性：PyTorch/驱动升级、dtype 与设备扩展、API 弃用都可能改变行为。需要把 stride 与 storage_offset 构成的地址映射 与 view/reshape/contiguous 的复制边界 视为对外契约，而不是一次性实现细节。

#### 2. 问题分析
1. 标注 张量布局、视图语义与设备放置 中哪些是可承诺的契约、哪些是实现细节。
2. 找出对版本/硬件敏感的面：对连续张量有 stride[i]=∏_{j>i} shape[j]；view 只在目标 shape 能由既有 stride 整除分解时成立，否则 reshape 触发复制；实数占用满足 numel·itemsize ≤ storage.nbytes − storage_offset·itemsize。
3. 定义演进不变量：升级后哪些量必须保持不变（如 有效带宽 GB/s）。
4. 设计回退路径与灰度，避免 对非连续张量调用 view 抛 RuntimeError: view size is not compatible with input tensor's size and stride 在生产放大。

#### 3. 具体答案
面试中可以这样回答：

> 张量的本质是 (storage, size, stride, storage_offset) 四元组对一段连续内存的解释；视图与副本的区别决定是否共享存储、是否可安全原地写。设备放置是显式拓扑决策，.to() 按需搬运并可能触发隐式同步。
>
> 演进不变量：对连续张量有 stride[i]=∏_{j>i} shape[j]；view 只在目标 shape 能由既有 stride 整除分解时成立，否则 reshape 触发复制；实数占用满足 numel·itemsize ≤ storage.nbytes − storage_offset·itemsize。
>
> 可验证事实：torch.Tensor.transpose 返回共享存储的视图（只互换 stride，不影响 storage），而 .reshape 在不可视图化时返回副本，可用 data_ptr() 是否相同判定。

#### 4. 技术洞察
演进的本质是契约稳定性：对连续张量有 stride[i]=∏_{j>i} shape[j]；view 只在目标 shape 能由既有 stride 整除分解时成立，否则 reshape 触发复制；实数占用满足 numel·itemsize ≤ storage.nbytes − storage_offset·itemsize。 中依赖版本/硬件的部分应被隔离并用抽象层收敛，任何未验证的升级结论都标注 [待验证]。

#### 5. 图文说明
| 层次 | 张量布局、视图语义与设备放置 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 张量、目标 shape/stride、device/dtype 与 memory_format | 约束缺失会误判视图可行性或越界读 |
| 执行与状态 | 追踪 storage、stride、offset 与 allocator 块的 owner 和生命周期 | 共享存储被原地写、碎片泄漏或隐式同步 |
| 输出与 oracle | 对照 data_ptr、is_contiguous 与显存曲线 | 把副本误当视图、峰值显存与理论值严重偏离 |

图示解释：把 张量布局、视图语义与设备放置 的契约面与实现面分离，沿版本轴观察契约面是否漂移，实现面可自由替换。

#### 6. 拓展知识
配套治理：版本兼容矩阵、PYTORCH_CUDA_ALLOC_CONF 环境变量 与回退开关；比较新旧实现的成本与失败恢复，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 升级 PyTorch 或驱动后，张量布局、视图语义与设备放置 的哪些行为可能变化，如何用 有效带宽 GB/s 回归检测？
2. 如何在不停机的情况下灰度新实现并支持快速回退，回退点选在哪里？
3. 哪些边界（memory_format 与 channels_last、pin_memory 与 non_blocking 拷贝）最可能在演进中被破坏，如何冻结？

#### 8. 易错点与反例
1. 反例：把版本相关行为硬编码，升级后 对非连续张量调用 view 抛 RuntimeError: view size is not compatible with input tensor's size and stride。
2. 反例：无回退开关直接全量上线，遇到 在未同步的跨界 .to() 后立即读数据得到脏值 无法止损。
3. 反例：只验证新功能不回归旧路径，直到 pin_memory 张量长期驻留耗尽锁页内存 才暴露兼容性缺陷。

#### 9. 实践与验证
验证方案：对同一逻辑张量构造 contiguous、转置、expand 三种布局，断言 is_contiguous/stride/storage_offset，并用 data_ptr 判定 reshape 是否复制。 在至少两个版本/设备上重复，用 tensor.stride()/is_contiguous()/storage_offset()/data_ptr() 与 torch.cuda.memory_summary()/memory_snapshot() 记录 有效带宽 GB/s、峰值显存 GiB 的历史曲线；oracle：data_ptr 相同判为视图、不同判为副本；峰值显存与理论 numel·itemsize 的偏差落在分配器块对齐范围内。

#### 10. 评分标准
E035 评分：
- 不合格：只讲当前实现，没有契约与版本概念；
- 合格：能区分契约面与实现面并指出一个版本敏感点；
- 良好：能给出演进不变量、灰度与回退方案（覆盖 对非连续张量调用 view 抛 RuntimeError: view size is not compatible with input tensor's size and stride）；
- 优秀/专家：能补充兼容矩阵、弃用迁移成本、长期维护与残余风险。
### E036. 长期演进：autograd

- 难度：专家级
- 知识域：K02 autograd
- 考察能力：前置条件、后置条件、断言与错误处理
- 题型：代码/伪代码/配置分析
- 面试频率：开放题
- 前置知识：requires_grad、leaf、view、detach、反向
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

长期演进关注 动态计算图、梯度累积与高阶微分 在版本、硬件与业务变化下的可持续性：PyTorch/驱动升级、dtype 与设备扩展、API 弃用都可能改变行为。需要把 动态图与 grad_fn 链 与 叶张量与 .grad 累加 视为对外契约，而不是一次性实现细节。

#### 2. 问题分析
1. 标注 动态计算图、梯度累积与高阶微分 中哪些是可承诺的契约、哪些是实现细节。
2. 找出对版本/硬件敏感的面：反向传播沿 grad_fn 链执行链式法则 g_i=Σ_j g_j·(∂f_j/∂x_i)，梯度以累加方式写入 .grad（不清零则叠加）；对需要梯度的张量做 in-place 会使其 version counter 加一，若该版本被 backward 引用则抛 RuntimeError: a variable needed for gradient computation has been modified by an inplace operation。
3. 定义演进不变量：升级后哪些量必须保持不变（如 梯度相对误差）。
4. 设计回退路径与灰度，避免 detach 后仍期望梯度回传导致叶张量 .grad 为 None 在生产放大。

#### 3. 具体答案
面试中可以这样回答：

> autograd 记录前向操作序列构成的动态图，反向时按逆序调用 grad_fn，用链式法则把上游梯度乘上局部雅可比后累加到叶张量。图的生命周期由 retain_graph 控制，原地写通过 version counter 保证数值一致。
>
> 演进不变量：反向传播沿 grad_fn 链执行链式法则 g_i=Σ_j g_j·(∂f_j/∂x_i)，梯度以累加方式写入 .grad（不清零则叠加）；对需要梯度的张量做 in-place 会使其 version counter 加一，若该版本被 backward 引用则抛 RuntimeError: a variable needed for gradient computation has been modified by an inplace operation。
>
> 可验证事实：torch.autograd.gradcheck 在 float64 下用有限差分（默认 eps=1e-6、rtol=1.3e-6、atol=1e-5）校验解析梯度，要求输入 requires_grad=True 且函数确定性。

#### 4. 技术洞察
演进的本质是契约稳定性：反向传播沿 grad_fn 链执行链式法则 g_i=Σ_j g_j·(∂f_j/∂x_i)，梯度以累加方式写入 .grad（不清零则叠加）；对需要梯度的张量做 in-place 会使其 version counter 加一，若该版本被 backward 引用则抛 RuntimeError: a variable needed for gradient computation has been modified by an inplace operation。 中依赖版本/硬件的部分应被隔离并用抽象层收敛，任何未验证的升级结论都标注 [待验证]。

#### 5. 图文说明
| 层次 | 动态计算图、梯度累积与高阶微分 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | requires_grad 状态、叶张量标志、是否原地写 | 误把非叶张量当叶、图已释放 |
| 执行与状态 | 遍历 grad_fn 链、version counter、saved tensor 所有权 | 图重复释放、原地写破坏反向、梯度断链 |
| 输出与 oracle | 比对解析梯度与数值差分、.grad 累加结果 | 梯度为零/None、误差超容差、二阶导缺失 |

图示解释：把 动态计算图、梯度累积与高阶微分 的契约面与实现面分离，沿版本轴观察契约面是否漂移，实现面可自由替换。

#### 6. 拓展知识
配套治理：版本兼容矩阵、torch.autograd.gradcheck/gradgradcheck 与回退开关；比较新旧实现的成本与失败恢复，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 升级 PyTorch 或驱动后，动态计算图、梯度累积与高阶微分 的哪些行为可能变化，如何用 梯度相对误差 回归检测？
2. 如何在不停机的情况下灰度新实现并支持快速回退，回退点选在哪里？
3. 哪些边界（retain_graph/create_graph 生命周期、in-place 的 version counter）最可能在演进中被破坏，如何冻结？

#### 8. 易错点与反例
1. 反例：把版本相关行为硬编码，升级后 detach 后仍期望梯度回传导致叶张量 .grad 为 None。
2. 反例：无回退开关直接全量上线，遇到 第二次 backward 未设 retain_graph=True 触发 Trying to backward through the graph a second time 无法止损。
3. 反例：只验证新功能不回归旧路径，直到 对参与反向的张量原地写触发 version counter 报错 才暴露兼容性缺陷。

#### 9. 实践与验证
验证方案：构造 y=x·sin(x)，用 torch.autograd.grad 求一阶导并与有限差分对比；再开 create_graph=True 求二阶导，验证与解析式 cos(x)−x·sin(x) 一致。 在至少两个版本/设备上重复，用 tensor.grad_fn 与 next_functions 遍历 与 torch.autograd.anomaly_mode/set_detect_anomaly 记录 梯度相对误差、峰值激活内存 MiB 的历史曲线；oracle：gradcheck 返回 True（误差在 rtol=1e-3/atol=1e-5 内），二阶导相对误差 < 1e-4。

#### 10. 评分标准
E036 评分：
- 不合格：只讲当前实现，没有契约与版本概念；
- 合格：能区分契约面与实现面并指出一个版本敏感点；
- 良好：能给出演进不变量、灰度与回退方案（覆盖 detach 后仍期望梯度回传导致叶张量 .grad 为 None）；
- 优秀/专家：能补充兼容矩阵、弃用迁移成本、长期维护与残余风险。
### E037. 长期演进：Module/状态

- 难度：专家级
- 知识域：K03 Module/状态
- 考察能力：前置条件、后置条件、断言与错误处理
- 题型：代码/伪代码/配置分析
- 面试频率：开放题
- 前置知识：参数、buffer、模式和 state_dict
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

长期演进关注 模块树、参数字典与状态持久化 在版本、硬件与业务变化下的可持续性：PyTorch/驱动升级、dtype 与设备扩展、API 弃用都可能改变行为。需要把 Module 树与命名参数 与 parameter 与 buffer 的区别 视为对外契约，而不是一次性实现细节。

#### 2. 问题分析
1. 标注 模块树、参数字典与状态持久化 中哪些是可承诺的契约、哪些是实现细节。
2. 找出对版本/硬件敏感的面：state_dict 的键等于模块路径拼接参数/缓冲区名（如 layer1.0.conv.weight）；load_state_dict(strict=True) 要求键集合完全一致，否则报 missing/unexpected keys；buffer 不参与 optimizer 更新但会被 .to() 搬运。
3. 定义演进不变量：升级后哪些量必须保持不变（如 参数量）。
4. 设计回退路径与灰度，避免 buffer 未 register_buffer 导致不随 .to() 和 state_dict 迁移 在生产放大。

#### 3. 具体答案
面试中可以这样回答：

> nn.Module 是一棵注册树，参数与缓冲区经命名空间形成扁平键空间；state_dict 是这棵树的快照契约，.to() 递归搬运所有注册张量。模式切换与注册完整性共同决定训练/推理一致性。
>
> 演进不变量：state_dict 的键等于模块路径拼接参数/缓冲区名（如 layer1.0.conv.weight）；load_state_dict(strict=True) 要求键集合完全一致，否则报 missing/unexpected keys；buffer 不参与 optimizer 更新但会被 .to() 搬运。
>
> 可验证事实：model.eval() 只切换 dropout/BatchNorm 等模块的 training 标志，不关闭 autograd；省显存必须叠加 torch.no_grad() 或 inference_mode()。

#### 4. 技术洞察
演进的本质是契约稳定性：state_dict 的键等于模块路径拼接参数/缓冲区名（如 layer1.0.conv.weight）；load_state_dict(strict=True) 要求键集合完全一致，否则报 missing/unexpected keys；buffer 不参与 optimizer 更新但会被 .to() 搬运。 中依赖版本/硬件的部分应被隔离并用抽象层收敛，任何未验证的升级结论都标注 [待验证]。

#### 5. 图文说明
| 层次 | 模块树、参数字典与状态持久化 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 模块结构、注册的参数/缓冲区、目标 device/dtype | 漏注册 buffer 或键空间不一致 |
| 执行与状态 | 遍历模块树、加载/保存快照、切换 training 标志 | 键缺失被静默、模式与图不匹配、别名错乱 |
| 输出与 oracle | 逐张量比对 state_dict、核对模式相关的统计 | 权重随机初始化、BN 统计漂移、显存未释放 |

图示解释：把 模块树、参数字典与状态持久化 的契约面与实现面分离，沿版本轴观察契约面是否漂移，实现面可自由替换。

#### 6. 拓展知识
配套治理：版本兼容矩阵、register_buffer/register_parameter 与回退开关；比较新旧实现的成本与失败恢复，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 升级 PyTorch 或驱动后，模块树、参数字典与状态持久化 的哪些行为可能变化，如何用 参数量 回归检测？
2. 如何在不停机的情况下灰度新实现并支持快速回退，回退点选在哪里？
3. 哪些边界（state_dict 键空间、train/eval 模式）最可能在演进中被破坏，如何冻结？

#### 8. 易错点与反例
1. 反例：把版本相关行为硬编码，升级后 buffer 未 register_buffer 导致不随 .to() 和 state_dict 迁移。
2. 反例：无回退开关直接全量上线，遇到 共享参数被重复注册改变键空间与加载行为 无法止损。
3. 反例：只验证新功能不回归旧路径，直到 strict=False 静默吞掉缺失键使部分权重随机初始化 才暴露兼容性缺陷。

#### 9. 实践与验证
验证方案：构造含卷积、BN、自定义 buffer 的模块，round-trip state_dict 到新实例并 assert_close 全部张量；删掉一个 buffer 验证 strict=True 报 missing keys。 在至少两个版本/设备上重复，用 named_parameters()/named_buffers()/recurse 与 state_dict()/load_state_dict() 记录 参数量、键匹配率 的历史曲线；oracle：load_state_dict 返回 missing=[]、unexpected=[]；train 模式两次 forward 后 BN running_mean 发生变化。

#### 10. 评分标准
E037 评分：
- 不合格：只讲当前实现，没有契约与版本概念；
- 合格：能区分契约面与实现面并指出一个版本敏感点；
- 良好：能给出演进不变量、灰度与回退方案（覆盖 buffer 未 register_buffer 导致不随 .to() 和 state_dict 迁移）；
- 优秀/专家：能补充兼容矩阵、弃用迁移成本、长期维护与残余风险。
### E038. 长期演进：数据管道

- 难度：专家级
- 知识域：K04 数据管道
- 考察能力：前置条件、后置条件、断言与错误处理
- 题型：代码/伪代码/配置分析
- 面试频率：开放题
- 前置知识：Dataset、Sampler、collate、padding、版本
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

长期演进关注 DataLoader 的并行、采样与预取 在版本、硬件与业务变化下的可持续性：PyTorch/驱动升级、dtype 与设备扩展、API 弃用都可能改变行为。需要把 map-style 与 iterable-style Dataset 与 num_workers 与进程启动方式 视为对外契约，而不是一次性实现细节。

#### 2. 问题分析
1. 标注 DataLoader 的并行、采样与预取 中哪些是可承诺的契约、哪些是实现细节。
2. 找出对版本/硬件敏感的面：map-style 数据集每 epoch 批数 = ⌈N/batch_size⌉（drop_last 时为 ⌊N/batch_size⌋）；worker 是独立进程并各自持有 RNG，随机变换必须经 worker_init_fn 播种才可复现。
3. 定义演进不变量：升级后哪些量必须保持不变（如 吞吐 samples/s）。
4. 设计回退路径与灰度，避免 在 Dataset.__init__ 或 __getitem__ 中初始化 CUDA 导致 fork 后 worker 崩溃 在生产放大。

#### 3. 具体答案
面试中可以这样回答：

> 数据管道把采样、取数、批处理和预取解耦到多 worker 进程；正确性取决于索引覆盖与随机性可复现，性能取决于预取深度与 H2D 重叠。
>
> 演进不变量：map-style 数据集每 epoch 批数 = ⌈N/batch_size⌉（drop_last 时为 ⌊N/batch_size⌋）；worker 是独立进程并各自持有 RNG，随机变换必须经 worker_init_fn 播种才可复现。
>
> 可验证事实：pin_memory=True 配合 non_blocking=True 能把 H2D 拷贝与计算重叠；persistent_workers=True 在 epoch 间保留 worker 进程，避免反复 fork 开销。

#### 4. 技术洞察
演进的本质是契约稳定性：map-style 数据集每 epoch 批数 = ⌈N/batch_size⌉（drop_last 时为 ⌊N/batch_size⌋）；worker 是独立进程并各自持有 RNG，随机变换必须经 worker_init_fn 播种才可复现。 中依赖版本/硬件的部分应被隔离并用抽象层收敛，任何未验证的升级结论都标注 [待验证]。

#### 5. 图文说明
| 层次 | DataLoader 的并行、采样与预取 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 数据集大小、batch_size、worker 数、采样方式 | 索引不覆盖、随机性不可复现 |
| 执行与状态 | 追踪 worker 进程、预取队列、pin 缓冲与 sampler 状态 | CUDA-in-fork 崩溃、句柄泄漏、顺序错乱 |
| 输出与 oracle | 核对样本集合、shape、吞吐与首 batch 延迟 | 重复/丢失样本、collate 形状错误、吞吐饱和异常 |

图示解释：把 DataLoader 的并行、采样与预取 的契约面与实现面分离，沿版本轴观察契约面是否漂移，实现面可自由替换。

#### 6. 拓展知识
配套治理：版本兼容矩阵、IterableDataset 与分片 与回退开关；比较新旧实现的成本与失败恢复，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 升级 PyTorch 或驱动后，DataLoader 的并行、采样与预取 的哪些行为可能变化，如何用 吞吐 samples/s 回归检测？
2. 如何在不停机的情况下灰度新实现并支持快速回退，回退点选在哪里？
3. 哪些边界（sampler 与 shuffle 语义、pin_memory 与 non_blocking 重叠）最可能在演进中被破坏，如何冻结？

#### 8. 易错点与反例
1. 反例：把版本相关行为硬编码，升级后 在 Dataset.__init__ 或 __getitem__ 中初始化 CUDA 导致 fork 后 worker 崩溃。
2. 反例：无回退开关直接全量上线，遇到 worker 数过大引发内存或文件句柄耗尽 无法止损。
3. 反例：只验证新功能不回归旧路径，直到 collate_fn 对不定长序列堆叠得到错误 shape 才暴露兼容性缺陷。

#### 9. 实践与验证
验证方案：固定 seed 与 worker_init_fn，连续两 epoch 记录样本索引序列验证覆盖完整且可复现；测量 num_workers∈{0,2,4,8} 的吞吐拐点。 在至少两个版本/设备上重复，用 DataLoader/worker_init_fn/generator 与 torch.utils.data.get_worker_info 记录 吞吐 samples/s、首 batch 延迟 的历史曲线；oracle：两轮索引并集等于 0..N−1 且无重复，吞吐在某一 worker 数后饱和。

#### 10. 评分标准
E038 评分：
- 不合格：只讲当前实现，没有契约与版本概念；
- 合格：能区分契约面与实现面并指出一个版本敏感点；
- 良好：能给出演进不变量、灰度与回退方案（覆盖 在 Dataset.__init__ 或 __getitem__ 中初始化 CUDA 导致 fork 后 worker 崩溃）；
- 优秀/专家：能补充兼容矩阵、弃用迁移成本、长期维护与残余风险。
### E039. 长期演进：训练/优化器

- 难度：专家级
- 知识域：K05 训练/优化器
- 考察能力：前置条件、后置条件、断言与错误处理
- 题型：代码/伪代码/配置分析
- 面试频率：开放题
- 前置知识：forward、backward、累积、optimizer、scheduler
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

长期演进关注 优化器更新、学习率调度与梯度处理 在版本、硬件与业务变化下的可持续性：PyTorch/驱动升级、dtype 与设备扩展、API 弃用都可能改变行为。需要把 优化器状态（Adam 的 m/v） 与 zero_grad 语义 视为对外契约，而不是一次性实现细节。

#### 2. 问题分析
1. 标注 优化器更新、学习率调度与梯度处理 中哪些是可承诺的契约、哪些是实现细节。
2. 找出对版本/硬件敏感的面：Adam：m_t=β1·m_{t−1}+(1−β1)·g_t，v_t=β2·v_{t−1}+(1−β2)·g_t²，偏差校正 m̂=m/(1−β1^t)、v̂=v/(1−β2^t)，θ←θ−lr·m̂/(√v̂+ε)；梯度累积 k 步等价于把有效批放大 k 倍，但必须对 loss 除以 k 或按 k 归约，否则梯度被放大 k 倍。
3. 定义演进不变量：升级后哪些量必须保持不变（如 loss 曲线）。
4. 设计回退路径与灰度，避免 忘记 zero_grad 使梯度跨 step 累加，等效 lr 漂移 在生产放大。

#### 3. 具体答案
面试中可以这样回答：

> 训练步是 zero_grad→forward→loss→scaled backward→unscale→clip→step→scheduler 的固定顺序；顺序错位会静默改变有效学习率或裁剪幅度。
>
> 演进不变量：Adam：m_t=β1·m_{t−1}+(1−β1)·g_t，v_t=β2·v_{t−1}+(1−β2)·g_t²，偏差校正 m̂=m/(1−β1^t)、v̂=v/(1−β2^t)，θ←θ−lr·m̂/(√v̂+ε)；梯度累积 k 步等价于把有效批放大 k 倍，但必须对 loss 除以 k 或按 k 归约，否则梯度被放大 k 倍。
>
> 可验证事实：zero_grad(set_to_none=True) 把 .grad 置 None（自 PyTorch 2.0 起为默认行为），比置零更省内存；optimizer.state_dict() 按参数索引保存 momentum/m/v 缓冲区。

#### 4. 技术洞察
演进的本质是契约稳定性：Adam：m_t=β1·m_{t−1}+(1−β1)·g_t，v_t=β2·v_{t−1}+(1−β2)·g_t²，偏差校正 m̂=m/(1−β1^t)、v̂=v/(1−β2^t)，θ←θ−lr·m̂/(√v̂+ε)；梯度累积 k 步等价于把有效批放大 k 倍，但必须对 loss 除以 k 或按 k 归约，否则梯度被放大 k 倍。 中依赖版本/硬件的部分应被隔离并用抽象层收敛，任何未验证的升级结论都标注 [待验证]。

#### 5. 图文说明
| 层次 | 优化器更新、学习率调度与梯度处理 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 初始参数、优化器配置、损失缩放与累积步 k | 顺序错位、未缩放 loss |
| 执行与状态 | 追踪 .grad、优化器状态缓冲区、lr 调度历史 | 梯度残留、动量错乱、裁剪幅度被缩放 |
| 输出与 oracle | 核对 loss 单调性、参数更新与 lr 序列 | loss 震荡、等效 lr 漂移、累积实现不等价 |

图示解释：把 优化器更新、学习率调度与梯度处理 的契约面与实现面分离，沿版本轴观察契约面是否漂移，实现面可自由替换。

#### 6. 拓展知识
配套治理：版本兼容矩阵、clip_grad_norm_ 返回值 与回退开关；比较新旧实现的成本与失败恢复，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 升级 PyTorch 或驱动后，优化器更新、学习率调度与梯度处理 的哪些行为可能变化，如何用 loss 曲线 回归检测？
2. 如何在不停机的情况下灰度新实现并支持快速回退，回退点选在哪里？
3. 哪些边界（梯度累积与有效批、clip 与 scaler 顺序）最可能在演进中被破坏，如何冻结？

#### 8. 易错点与反例
1. 反例：把版本相关行为硬编码，升级后 忘记 zero_grad 使梯度跨 step 累加，等效 lr 漂移。
2. 反例：无回退开关直接全量上线，遇到 scheduler.step 放错位置（每 batch 而非每 epoch）改变 lr 曲线 无法止损。
3. 反例：只验证新功能不回归旧路径，直到 clip_grad_norm_ 在 GradScaler.unscale_ 之前调用使裁剪阈值被放大 S 倍 才暴露兼容性缺陷。

#### 9. 实践与验证
验证方案：在单个 batch 上跑 200 步验证 loss 单调下降到过拟合；打印 get_last_lr 序列核对调度曲线；对同一初始点比较累积实现与放大批实现的参数更新。 在至少两个版本/设备上重复，用 optimizer.state_dict()/param_groups 与 scheduler.get_last_lr() 记录 loss 曲线、学习率序列 的历史曲线；oracle：单批过拟合 loss < 0.01；累积与大批实现的参数差在 1e-6 内。

#### 10. 评分标准
E039 评分：
- 不合格：只讲当前实现，没有契约与版本概念；
- 合格：能区分契约面与实现面并指出一个版本敏感点；
- 良好：能给出演进不变量、灰度与回退方案（覆盖 忘记 zero_grad 使梯度跨 step 累加，等效 lr 漂移）；
- 优秀/专家：能补充兼容矩阵、弃用迁移成本、长期维护与残余风险。
### E040. 长期演进：AMP/恢复

- 难度：专家级
- 知识域：K06 AMP/恢复
- 考察能力：前置条件、后置条件、断言与错误处理
- 题型：代码/伪代码/配置分析
- 面试频率：开放题
- 前置知识：autocast、scaler、快照、随机状态
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

长期演进关注 混合精度、检查点与确定性 在版本、硬件与业务变化下的可持续性：PyTorch/驱动升级、dtype 与设备扩展、API 弃用都可能改变行为。需要把 autocast 的算子白名单 与 GradScaler 动态缩放 视为对外契约，而不是一次性实现细节。

#### 2. 问题分析
1. 标注 混合精度、检查点与确定性 中哪些是可承诺的契约、哪些是实现细节。
2. 找出对版本/硬件敏感的面：GradScaler 先用 scale S 放大 loss 再反向，unscale_ 后若检测到 inf/nan 则跳过该 step 并把 S 减半；fp16 最正规数约 6.1e-5、次正规至约 6e-8，小于此量级的梯度下溢，故需缩放；bf16 指数位与 fp32 同为 8 位因此不需要缩放。
3. 定义演进不变量：升级后哪些量必须保持不变（如 scale factor 曲线）。
4. 设计回退路径与灰度，避免 fp16 无 scaler 导致梯度下溢为 0 或 NaN 在生产放大。

#### 3. 具体答案
面试中可以这样回答：

> AMP 通过 autocast 选低精度算子、GradScaler 维持梯度动态范围；恢复的正确性是状态闭包问题——模型、优化器、缩放与随机数状态缺一不可。
>
> 演进不变量：GradScaler 先用 scale S 放大 loss 再反向，unscale_ 后若检测到 inf/nan 则跳过该 step 并把 S 减半；fp16 最正规数约 6.1e-5、次正规至约 6e-8，小于此量级的梯度下溢，故需缩放；bf16 指数位与 fp32 同为 8 位因此不需要缩放。
>
> 可验证事实：torch.use_deterministic_algorithms(True) 会让部分算子回退到确定性实现或直接报错；cudnn.benchmark=True 的自动调优会破坏逐步可复现性。完整恢复需保存 model、optimizer、scaler、epoch、CPU 与 CUDA RNG 及 sampler 状态。

#### 4. 技术洞察
演进的本质是契约稳定性：GradScaler 先用 scale S 放大 loss 再反向，unscale_ 后若检测到 inf/nan 则跳过该 step 并把 S 减半；fp16 最正规数约 6.1e-5、次正规至约 6e-8，小于此量级的梯度下溢，故需缩放；bf16 指数位与 fp32 同为 8 位因此不需要缩放。 中依赖版本/硬件的部分应被隔离并用抽象层收敛，任何未验证的升级结论都标注 [待验证]。

#### 5. 图文说明
| 层次 | 混合精度、检查点与确定性 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | dtype、autocast 区域、scaler 配置、checkpoint 内容 | 状态不闭合、精度选择错误 |
| 执行与状态 | 追踪 scale factor、RNG、优化器状态与 nan/inf 分支 | 下溢、NaN、恢复后状态漂移 |
| 输出与 oracle | 比对续训与基线参数、scale 曲线、依赖确定性开关 | 曲线跳变、结果不可复现、梯度全零 |

图示解释：把 混合精度、检查点与确定性 的契约面与实现面分离，沿版本轴观察契约面是否漂移，实现面可自由替换。

#### 6. 拓展知识
配套治理：版本兼容矩阵、torch.use_deterministic_algorithms 与回退开关；比较新旧实现的成本与失败恢复，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 升级 PyTorch 或驱动后，混合精度、检查点与确定性 的哪些行为可能变化，如何用 scale factor 曲线 回归检测？
2. 如何在不停机的情况下灰度新实现并支持快速回退，回退点选在哪里？
3. 哪些边界（bf16 与 fp16 数值范围、RNG/optimizer 状态恢复）最可能在演进中被破坏，如何冻结？

#### 8. 易错点与反例
1. 反例：把版本相关行为硬编码，升级后 fp16 无 scaler 导致梯度下溢为 0 或 NaN。
2. 反例：无回退开关直接全量上线，遇到 resume 只存模型权重丢失优化器动量使曲线跳变 无法止损。
3. 反例：只验证新功能不回归旧路径，直到 只存 torch RNG 未存 numpy/random 状态导致数据顺序不同 才暴露兼容性缺陷。

#### 9. 实践与验证
验证方案：训练 50 步保存完整 checkpoint，重启加载后再跑 50 步与不中断基线逐参数比较；fp16 下注入大梯度观测 S 是否减半。 在至少两个版本/设备上重复，用 torch.cuda.amp.GradScaler 与 autocast 与 torch.get_rng_state/get_cuda_rng_state/numpy.random.get_state 记录 scale factor 曲线、resume 前后参数最大差 的历史曲线；oracle：续训与基线参数最大差 < 1e-5（bit 级取决于确定性开关），scale factor 与跳过计数一致。

#### 10. 评分标准
E040 评分：
- 不合格：只讲当前实现，没有契约与版本概念；
- 合格：能区分契约面与实现面并指出一个版本敏感点；
- 良好：能给出演进不变量、灰度与回退方案（覆盖 fp16 无 scaler 导致梯度下溢为 0 或 NaN）；
- 优秀/专家：能补充兼容矩阵、弃用迁移成本、长期维护与残余风险。
### E041. 长期演进：分布式

- 难度：专家级
- 知识域：K07 分布式
- 考察能力：正确性、性能、成本、运维与迁移条件
- 题型：对比与取舍
- 面试频率：开放题
- 前置知识：rank、DDP、FSDP、collective、容错
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

长期演进关注 进程组、集合通信与梯度同步 在版本、硬件与业务变化下的可持续性：PyTorch/驱动升级、dtype 与设备扩展、API 弃用都可能改变行为。需要把 进程组与 rank/world_size 与 all_reduce/reduce_scatter/all_gather 视为对外契约，而不是一次性实现细节。

#### 2. 问题分析
1. 标注 进程组、集合通信与梯度同步 中哪些是可承诺的契约、哪些是实现细节。
2. 找出对版本/硬件敏感的面：DDP 默认 all_reduce(sum) 后除以 world_size 得平均梯度，故全局有效批 = 本地批 × world_size × 累积步数；ring all_reduce 单步通信量 ≈ 2·(N−1)/N·S（S 为梯度字节数），大 N 时趋近 2S。
3. 定义演进不变量：升级后哪些量必须保持不变（如 all_reduce 带宽 GB/s）。
4. 设计回退路径与灰度，避免 各 rank 集合通信调用不匹配导致 hang 直到 watchdog 超时 在生产放大。

#### 3. 具体答案
面试中可以这样回答：

> 分布式训练把单机步拆成计算→梯度归约→更新三段；正确性在于所有 rank 以相同顺序进入集合通信且梯度按 world_size 归一，性能在于把通信藏进反向的尾部。
>
> 演进不变量：DDP 默认 all_reduce(sum) 后除以 world_size 得平均梯度，故全局有效批 = 本地批 × world_size × 累积步数；ring all_reduce 单步通信量 ≈ 2·(N−1)/N·S（S 为梯度字节数），大 N 时趋近 2S。
>
> 可验证事实：NCCL 只支持 CUDA 张量、gloo 支持 CPU；DDP 默认把梯度分成约 25 MiB 的桶以便与反向计算重叠通信。

#### 4. 技术洞察
演进的本质是契约稳定性：DDP 默认 all_reduce(sum) 后除以 world_size 得平均梯度，故全局有效批 = 本地批 × world_size × 累积步数；ring all_reduce 单步通信量 ≈ 2·(N−1)/N·S（S 为梯度字节数），大 N 时趋近 2S。 中依赖版本/硬件的部分应被隔离并用抽象层收敛，任何未验证的升级结论都标注 [待验证]。

#### 5. 图文说明
| 层次 | 进程组、集合通信与梯度同步 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 进程数、后端、张量是否 CUDA、参与反向的参数集合 | 后端不匹配、参数未参与、集合调用不一致 |
| 执行与状态 | 追踪 rank、bucket、通信算子与反向重叠 | hang、梯度未归约、通信未重叠 |
| 输出与 oracle | 对比单卡/DDP 梯度、通信带宽与 step 方差 | 梯度差超容差、超时、吞吐不随卡数提升 |

图示解释：把 进程组、集合通信与梯度同步 的契约面与实现面分离，沿版本轴观察契约面是否漂移，实现面可自由替换。

#### 6. 拓展知识
配套治理：版本兼容矩阵、NCCL_DEBUG=INFO 与 TORCH_DISTRIBUTED_DEBUG 与回退开关；比较新旧实现的成本与失败恢复，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 升级 PyTorch 或驱动后，进程组、集合通信与梯度同步 的哪些行为可能变化，如何用 all_reduce 带宽 GB/s 回归检测？
2. 如何在不停机的情况下灰度新实现并支持快速回退，回退点选在哪里？
3. 哪些边界（NCCL 与 gloo 后端、梯度分桶与通信重叠）最可能在演进中被破坏，如何冻结？

#### 8. 易错点与反例
1. 反例：把版本相关行为硬编码，升级后 各 rank 集合通信调用不匹配导致 hang 直到 watchdog 超时。
2. 反例：无回退开关直接全量上线，遇到 未按 world_size 归一梯度使等效 lr 随卡数变化 无法止损。
3. 反例：只验证新功能不回归旧路径，直到 NCCL/驱动版本不一致引发通信超时或崩溃 才暴露兼容性缺陷。

#### 9. 实践与验证
验证方案：用 torchrun 起 2 进程，固定初始化使单卡与 DDP 梯度可对齐，比较 all_reduce 前后梯度；人为让一个 rank 提前退出观测 NCCL watchdog 超时。 在至少两个版本/设备上重复，用 torch.distributed.init_process_group/barrier 与 dist.all_reduce/all_gather/reduce_scatter 记录 all_reduce 带宽 GB/s、step 时间方差 的历史曲线；oracle：DDP 与单卡初始梯度（除以 world_size 后）相对误差 < 1e-6；NCCL_DEBUG 显示预期的 ring/tree 算法。

#### 10. 评分标准
E041 评分：
- 不合格：只讲当前实现，没有契约与版本概念；
- 合格：能区分契约面与实现面并指出一个版本敏感点；
- 良好：能给出演进不变量、灰度与回退方案（覆盖 各 rank 集合通信调用不匹配导致 hang 直到 watchdog 超时）；
- 优秀/专家：能补充兼容矩阵、弃用迁移成本、长期维护与残余风险。
### E042. 长期演进：编译栈

- 难度：专家级
- 知识域：K08 编译栈
- 考察能力：正确性、性能、成本、运维与迁移条件
- 题型：对比与取舍
- 面试频率：开放题
- 前置知识：Dynamo、AOTAutograd、Inductor、guard、缓存
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

长期演进关注 Dynamo 捕获、guard 与 Inductor 代码生成 在版本、硬件与业务变化下的可持续性：PyTorch/驱动升级、dtype 与设备扩展、API 弃用都可能改变行为。需要把 TorchDynamo 的字节码捕获 与 guard 与 specialization 视为对外契约，而不是一次性实现细节。

#### 2. 问题分析
1. 标注 Dynamo 捕获、guard 与 Inductor 代码生成 中哪些是可承诺的契约、哪些是实现细节。
2. 找出对版本/硬件敏感的面：一个编译单元的 guard 绑定在张量的 shape/dtype/device 与 Python 常量上；任一 guard 失效即触发重编译，默认累计重编译上限约 8 次（torch._dynamo.config.accumulated_recompile_limit）。图被 graph break 切成多个子图，未捕获区间回退 eager。
3. 定义演进不变量：升级后哪些量必须保持不变（如 重编译次数）。
4. 设计回退路径与灰度，避免 在图中调用 .item()/print/依赖数据的 Python 分支触发 graph break 在生产放大。

#### 3. 具体答案
面试中可以这样回答：

> 编译栈把 Python 执行轨迹捕获成 FX 图，用 guard 保证图对后续输入仍有效；失效即重编译、无法捕获即断图。稳定收益来自低断点数与低重编译率而非一次加速。
>
> 演进不变量：一个编译单元的 guard 绑定在张量的 shape/dtype/device 与 Python 常量上；任一 guard 失效即触发重编译，默认累计重编译上限约 8 次（torch._dynamo.config.accumulated_recompile_limit）。图被 graph break 切成多个子图，未捕获区间回退 eager。
>
> 可验证事实：设置 TORCH_LOGS=graph_breaks,guards,recompiles 可打印断点与重编译原因；torch._dynamo.explain(fn)(*args) 返回断点数量与原因；dynamic=True 或 torch._dynamo.mark_dynamic 可放宽形状守卫。

#### 4. 技术洞察
演进的本质是契约稳定性：一个编译单元的 guard 绑定在张量的 shape/dtype/device 与 Python 常量上；任一 guard 失效即触发重编译，默认累计重编译上限约 8 次（torch._dynamo.config.accumulated_recompile_limit）。图被 graph break 切成多个子图，未捕获区间回退 eager。 中依赖版本/硬件的部分应被隔离并用抽象层收敛，任何未验证的升级结论都标注 [待验证]。

#### 5. 图文说明
| 层次 | Dynamo 捕获、guard 与 Inductor 代码生成 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 函数代码、示例输入、shape/dtype/device 与 dynamic 声明 | 守卫过严、断点隐藏、动态维度未声明 |
| 执行与状态 | 追踪 Dynamo 捕获、guard 集合、子图与 Inductor 缓存 | 重编译风暴、断点回退 eager、缓存未命中 |
| 输出与 oracle | 对比 compiled/eager 数值、重编译与断点计数 | 数值不一致、加速比 < 1、重编译失控 |

图示解释：把 Dynamo 捕获、guard 与 Inductor 代码生成 的契约面与实现面分离，沿版本轴观察契约面是否漂移，实现面可自由替换。

#### 6. 拓展知识
配套治理：版本兼容矩阵、torch.compile(mode=...)/mark_dynamic 与回退开关；比较新旧实现的成本与失败恢复，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 升级 PyTorch 或驱动后，Dynamo 捕获、guard 与 Inductor 代码生成 的哪些行为可能变化，如何用 重编译次数 回归检测？
2. 如何在不停机的情况下灰度新实现并支持快速回退，回退点选在哪里？
3. 哪些边界（graph break 与子图、AOTAutograd 与 Inductor）最可能在演进中被破坏，如何冻结？

#### 8. 易错点与反例
1. 反例：把版本相关行为硬编码，升级后 在图中调用 .item()/print/依赖数据的 Python 分支触发 graph break。
2. 反例：无回退开关直接全量上线，遇到 动态 shape 引发重编译风暴使性能倒挂 无法止损。
3. 反例：只验证新功能不回归旧路径，直到 别名或原地写使编译结果与 eager 数值不一致 才暴露兼容性缺陷。

#### 9. 实践与验证
验证方案：对同一函数用固定 shape 与变化 shape 各跑 20 次，用 explain 统计 graph break 与重编译；对比 compiled 与 eager 输出最大差。 在至少两个版本/设备上重复，用 TORCH_LOGS=graph_breaks,guards,recompiles 与 torch._dynamo.explain 记录 重编译次数、compiled/eager 加速比 的历史曲线；oracle：固定 shape 下重编译次数为 1、输出与 eager 最大差 < 1e-4；变化 shape 下开启 dynamic 后重编译显著下降。

#### 10. 评分标准
E042 评分：
- 不合格：只讲当前实现，没有契约与版本概念；
- 合格：能区分契约面与实现面并指出一个版本敏感点；
- 良好：能给出演进不变量、灰度与回退方案（覆盖 在图中调用 .item()/print/依赖数据的 Python 分支触发 graph break）；
- 优秀/专家：能补充兼容矩阵、弃用迁移成本、长期维护与残余风险。
### E043. 长期演进：性能

- 难度：专家级
- 知识域：K09 性能
- 考察能力：正确性、性能、成本、运维与迁移条件
- 题型：对比与取舍
- 面试频率：开放题
- 前置知识：关键路径、带宽、算力、通信、显存、统计
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

长期演进关注 性能剖析、roofline 与显存容量 在版本、硬件与业务变化下的可持续性：PyTorch/驱动升级、dtype 与设备扩展、API 弃用都可能改变行为。需要把 CUDA event 计时与同步 与 roofline 与算术强度 视为对外契约，而不是一次性实现细节。

#### 2. 问题分析
1. 标注 性能剖析、roofline 与显存容量 中哪些是可承诺的契约、哪些是实现细节。
2. 找出对版本/硬件敏感的面：算术强度 I=FLOPs/Bytes，roofline 上界 = min(峰值算力 P, I×峰值带宽 BW)，据此判定 compute-bound 或 memory-bound；CUDA event 计时为 t=elapsed_time(start,stop)，异步操作必须 torch.cuda.synchronize() 后计时才可信。
3. 定义演进不变量：升级后哪些量必须保持不变（如 p50/p99 延迟）。
4. 设计回退路径与灰度，避免 异步未同步导致计时偏短，得出虚假加速 在生产放大。

#### 3. 具体答案
面试中可以这样回答：

> 性能结论必须绑定可复现的测量协议：warmup、同步、重复次数、分位数；再用 roofline 判定算力受限还是带宽受限，最后才谈优化手段。
>
> 演进不变量：算术强度 I=FLOPs/Bytes，roofline 上界 = min(峰值算力 P, I×峰值带宽 BW)，据此判定 compute-bound 或 memory-bound；CUDA event 计时为 t=elapsed_time(start,stop)，异步操作必须 torch.cuda.synchronize() 后计时才可信。
>
> 可验证事实：torch.profiler.profile(activities=[ProfilerActivity.CUDA]) 会插入 CUDA 事件并输出按算子聚合的 self/total CUDA time；首次迭代含 cuBLAS/context 初始化与自动调优，必须先 warmup 再计时。

#### 4. 技术洞察
演进的本质是契约稳定性：算术强度 I=FLOPs/Bytes，roofline 上界 = min(峰值算力 P, I×峰值带宽 BW)，据此判定 compute-bound 或 memory-bound；CUDA event 计时为 t=elapsed_time(start,stop)，异步操作必须 torch.cuda.synchronize() 后计时才可信。 中依赖版本/硬件的部分应被隔离并用抽象层收敛，任何未验证的升级结论都标注 [待验证]。

#### 5. 图文说明
| 层次 | 性能剖析、roofline 与显存容量 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 算子/规模、dtype、warmup 与 repeat、计时方式 | 未同步、未 warmup、规模误导 |
| 执行与状态 | 追踪 kernel 时长、显存曲线、launch 与同步点 | 计时偏短、碎片抖动、把 launch 开销当算力 |
| 输出与 oracle | 对比 event/profiler、roofline 定位与分位数 | 加速比不稳定、有效带宽超理论值、尾延迟超标 |

图示解释：把 性能剖析、roofline 与显存容量 的契约面与实现面分离，沿版本轴观察契约面是否漂移，实现面可自由替换。

#### 6. 拓展知识
配套治理：版本兼容矩阵、ncu/nsys 与回退开关；比较新旧实现的成本与失败恢复，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 升级 PyTorch 或驱动后，性能剖析、roofline 与显存容量 的哪些行为可能变化，如何用 p50/p99 延迟 回归检测？
2. 如何在不停机的情况下灰度新实现并支持快速回退，回退点选在哪里？
3. 哪些边界（kernel 融合与 launch 开销、显存占用与碎片）最可能在演进中被破坏，如何冻结？

#### 8. 易错点与反例
1. 反例：把版本相关行为硬编码，升级后 异步未同步导致计时偏短，得出虚假加速。
2. 反例：无回退开关直接全量上线，遇到 把 profiler 自身开销计入结论 无法止损。
3. 反例：只验证新功能不回归旧路径，直到 只测单次忽略冷启动与首次调优 才暴露兼容性缺陷。

#### 9. 实践与验证
验证方案：对 matmul、逐元素、拷贝分别用 CUDA event 与 profiler 计时，warmup=10、repeat=100、取中位数；用 bytes/time 反推有效带宽并与理论比较。 在至少两个版本/设备上重复，用 torch.profiler 与 torch.cuda.Event/elapsed_time 记录 p50/p99 延迟、MFU/TFLOPS 的历史曲线；oracle：event 计时与 profiler 的 CUDA time 偏差 < 5%，大 size 下有效带宽落在理论峰值的可解释比例内。

#### 10. 评分标准
E043 评分：
- 不合格：只讲当前实现，没有契约与版本概念；
- 合格：能区分契约面与实现面并指出一个版本敏感点；
- 良好：能给出演进不变量、灰度与回退方案（覆盖 异步未同步导致计时偏短，得出虚假加速）；
- 优秀/专家：能补充兼容矩阵、弃用迁移成本、长期维护与残余风险。
### E044. 长期演进：导出部署

- 难度：专家级
- 知识域：K10 导出部署
- 考察能力：正确性、性能、成本、运维与迁移条件
- 题型：对比与取舍
- 面试频率：开放题
- 前置知识：state_dict、Export、runtime、schema、迁移
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

长期演进关注 图导出、图签名与跨版本加载 在版本、硬件与业务变化下的可持续性：PyTorch/驱动升级、dtype 与设备扩展、API 弃用都可能改变行为。需要把 torch.export/ExportedProgram 与 torch.jit.trace/script 视为对外契约，而不是一次性实现细节。

#### 2. 问题分析
1. 标注 图导出、图签名与跨版本加载 中哪些是可承诺的契约、哪些是实现细节。
2. 找出对版本/硬件敏感的面：导出图是函数式的：无 Python 副作用，输入输出由 graph_signature 声明；torch.export 的功能化把原地写改写为显式输出。ONNX 的算子覆盖度由 opset 版本决定，超出该 opset 的算子导出即失败。
3. 定义演进不变量：升级后哪些量必须保持不变（如 导出成功率）。
4. 设计回退路径与灰度，避免 trace 未覆盖控制流分支导致逻辑被固化到单一分支 在生产放大。

#### 3. 具体答案
面试中可以这样回答：

> 导出把可变的 Python 模块冻结成只含 aten 算子的静态图，并用水印式签名声明输入输出契约；正确性等于导出图在目标 runtime 与 eager 数值一致，兼容性等于签名与 opset 能跨版本被消费。
>
> 演进不变量：导出图是函数式的：无 Python 副作用，输入输出由 graph_signature 声明；torch.export 的功能化把原地写改写为显式输出。ONNX 的算子覆盖度由 opset 版本决定，超出该 opset 的算子导出即失败。
>
> 可验证事实：torch.export 需要示例输入，返回含 graph_module、range_constraints 与 signature 的 ExportedProgram；torch.jit.trace 只记录执行过的算子，遇控制流会固化当时走的分支。

#### 4. 技术洞察
演进的本质是契约稳定性：导出图是函数式的：无 Python 副作用，输入输出由 graph_signature 声明；torch.export 的功能化把原地写改写为显式输出。ONNX 的算子覆盖度由 opset 版本决定，超出该 opset 的算子导出即失败。 中依赖版本/硬件的部分应被隔离并用抽象层收敛，任何未验证的升级结论都标注 [待验证]。

#### 5. 图文说明
| 层次 | 图导出、图签名与跨版本加载 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 模型、示例输入、dynamic 声明与目标 opset/runtime | 控制流未覆盖、动态维度缺声明 |
| 执行与状态 | 追踪图捕获、功能化改写、签名与序列化 | 分支固化、opset 缺算子、加载执行任意代码 |
| 输出与 oracle | 对比导出/eager 输出、校验签名与 checker | 数值偏差、改变维度即失效、导出被拒绝 |

图示解释：把 图导出、图签名与跨版本加载 的契约面与实现面分离，沿版本轴观察契约面是否漂移，实现面可自由替换。

#### 6. 拓展知识
配套治理：版本兼容矩阵、torch.jit.trace 与回退开关；比较新旧实现的成本与失败恢复，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 升级 PyTorch 或驱动后，图导出、图签名与跨版本加载 的哪些行为可能变化，如何用 导出成功率 回归检测？
2. 如何在不停机的情况下灰度新实现并支持快速回退，回退点选在哪里？
3. 哪些边界（ONNX opset 与 dynamic_axes、graph signature 与功能化）最可能在演进中被破坏，如何冻结？

#### 8. 易错点与反例
1. 反例：把版本相关行为硬编码，升级后 trace 未覆盖控制流分支导致逻辑被固化到单一分支。
2. 反例：无回退开关直接全量上线，遇到 dynamic shape 未显式声明使其被编译成常量 无法止损。
3. 反例：只验证新功能不回归旧路径，直到 opset 过低缺失算子导致导出失败 才暴露兼容性缺陷。

#### 9. 实践与验证
验证方案：对同一模型分别 torch.export 与 jit.trace，跑固定输入比对输出；改变一个输入维度检验 dynamic_shapes 是否被尊重；用 onnx.checker 校验导出图。 在至少两个版本/设备上重复，用 torch.export/exported.run_decompositions 与 torch.onnx.export 记录 导出成功率、eager 与导出输出误差 的历史曲线；oracle：导出输出与 eager 最大差 < 1e-5，声明为 dynamic 的维度改变时无需重导。

#### 10. 评分标准
E044 评分：
- 不合格：只讲当前实现，没有契约与版本概念；
- 合格：能区分契约面与实现面并指出一个版本敏感点；
- 良好：能给出演进不变量、灰度与回退方案（覆盖 trace 未覆盖控制流分支导致逻辑被固化到单一分支）；
- 优秀/专家：能补充兼容矩阵、弃用迁移成本、长期维护与残余风险。
### E045. 长期演进：自定义算子

- 难度：专家级
- 知识域：K11 自定义算子
- 考察能力：正确性、性能、成本、运维与迁移条件
- 题型：对比与取舍
- 面试频率：开放题
- 前置知识：schema、dispatcher、stream、autograd、ABI
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

长期演进关注 算子注册、调度与数值正确性 在版本、硬件与业务变化下的可持续性：PyTorch/驱动升级、dtype 与设备扩展、API 弃用都可能改变行为。需要把 torch.library/TORCH_LIBRARY 注册 与 autograd.Function 与双反向 视为对外契约，而不是一次性实现细节。

#### 2. 问题分析
1. 标注 算子注册、调度与数值正确性 中哪些是可承诺的契约、哪些是实现细节。
2. 找出对版本/硬件敏感的面：CUDA kernel 启动取 blocks=⌈N/threads⌉、threads≤1024 且为 32 的倍数；自定义算子若参与训练必须同时提供 forward 与 backward（或注册 autograd 公式），否则梯度断链；访问需满足 0≤idx<N 防越界。
3. 定义演进不变量：升级后哪些量必须保持不变（如 相对误差）。
4. 设计回退路径与灰度，避免 只写 forward 未注册 backward 使梯度静默为零 在生产放大。

#### 3. 具体答案
面试中可以这样回答：

> 自定义算子是 dispatcher 上的一个实现对：前向 kernel 负责数值、反向公式负责梯度；正确性由边界条件、设备守卫与 gradcheck 共同保证。
>
> 演进不变量：CUDA kernel 启动取 blocks=⌈N/threads⌉、threads≤1024 且为 32 的倍数；自定义算子若参与训练必须同时提供 forward 与 backward（或注册 autograd 公式），否则梯度断链；访问需满足 0≤idx<N 防越界。
>
> 可验证事实：torch.utils.cpp_extension.load/load_inline 首次调用时编译并缓存扩展；缺少 TORCH_CHECK 的越界访问可能不报错而返回垃圾数据或破坏相邻内存。

#### 4. 技术洞察
演进的本质是契约稳定性：CUDA kernel 启动取 blocks=⌈N/threads⌉、threads≤1024 且为 32 的倍数；自定义算子若参与训练必须同时提供 forward 与 backward（或注册 autograd 公式），否则梯度断链；访问需满足 0≤idx<N 防越界。 中依赖版本/硬件的部分应被隔离并用抽象层收敛，任何未验证的升级结论都标注 [待验证]。

#### 5. 图文说明
| 层次 | 算子注册、调度与数值正确性 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 张量 shape/dtype/device、线程与 grid 配置、边界条件 | 越界、设备不匹配、dtype 未分派 |
| 执行与状态 | 追踪 kernel 启动、CUDAGuard、dispatcher 分派与归约 | 越界写、跑错设备、梯度断链 |
| 输出与 oracle | 对比参考实现、gradcheck 与 sanitizer 报告 | 误差超标、梯度为零、随机错误 |

图示解释：把 算子注册、调度与数值正确性 的契约面与实现面分离，沿版本轴观察契约面是否漂移，实现面可自由替换。

#### 6. 拓展知识
配套治理：版本兼容矩阵、CUDA_LAUNCH_BLOCKING=1 与回退开关；比较新旧实现的成本与失败恢复，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 升级 PyTorch 或驱动后，算子注册、调度与数值正确性 的哪些行为可能变化，如何用 相对误差 回归检测？
2. 如何在不停机的情况下灰度新实现并支持快速回退，回退点选在哪里？
3. 哪些边界（AT_DISPATCH 与设备守卫、launch 配置与边界检查）最可能在演进中被破坏，如何冻结？

#### 8. 易错点与反例
1. 反例：把版本相关行为硬编码，升级后 只写 forward 未注册 backward 使梯度静默为零。
2. 反例：无回退开关直接全量上线，遇到 kernel 越界写破坏相邻显存造成随机错误 无法止损。
3. 反例：只验证新功能不回归旧路径，直到 缺少 CUDAGuard 在多卡环境跑错设备 才暴露兼容性缺陷。

#### 9. 实践与验证
验证方案：实现逐元素算子并与 eager 参考比较，用 gradcheck 校验反向；把 N 设为非 warp 倍数检验尾块处理，跑 compute-sanitizer。 在至少两个版本/设备上重复，用 torch.utils.cpp_extension.load/load_inline 与 AT_DISPATCH_FLOATING_TYPES 记录 相对误差、compute-sanitizer 命中数 的历史曲线；oracle：fp32 最大绝对误差 < 1e-6、gradcheck 通过，N=线程数±1 时无越界且结果正确。

#### 10. 评分标准
E045 评分：
- 不合格：只讲当前实现，没有契约与版本概念；
- 合格：能区分契约面与实现面并指出一个版本敏感点；
- 良好：能给出演进不变量、灰度与回退方案（覆盖 只写 forward 未注册 backward 使梯度静默为零）；
- 优秀/专家：能补充兼容矩阵、弃用迁移成本、长期维护与残余风险。
### E046. 长期演进：测试治理

- 难度：专家级
- 知识域：K12 测试治理
- 考察能力：正确性、性能、成本、运维与迁移条件
- 题型：对比与取舍
- 面试频率：开放题
- 前置知识：oracle、证据、日志、回滚、门禁、复盘
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

长期演进关注 测试契约、容差与证据化调试 在版本、硬件与业务变化下的可持续性：PyTorch/驱动升级、dtype 与设备扩展、API 弃用都可能改变行为。需要把 pytest 参数化与 fixture 与 assert_close 的 rtol/atol 视为对外契约，而不是一次性实现细节。

#### 2. 问题分析
1. 标注 测试契约、容差与证据化调试 中哪些是可承诺的契约、哪些是实现细节。
2. 找出对版本/硬件敏感的面：torch.testing.assert_close 在 |a−b| ≤ atol + rtol·|b| 时通过，float32 默认 rtol≈1.3e-6、atol≈1e-5；flaky 率 = 失败重跑数/总重跑数，需按阈值门禁。
3. 定义演进不变量：升级后哪些量必须保持不变（如 通过率）。
4. 设计回退路径与灰度，避免 容差过宽掩盖真实数值回归 在生产放大。

#### 3. 具体答案
面试中可以这样回答：

> 测试是把需求写成可执行的契约；有效的门禁需要确定性、隔离、明确容差和可定位失败的证据，而不是只断言最终数值。
>
> 演进不变量：torch.testing.assert_close 在 |a−b| ≤ atol + rtol·|b| 时通过，float32 默认 rtol≈1.3e-6、atol≈1e-5；flaky 率 = 失败重跑数/总重跑数，需按阈值门禁。
>
> 可验证事实：pytest -k 按名筛选、-x 首次失败即停、--reruns 需 pytest-rerunfailures 插件；未设种子的随机测试在 CI 中会产生不可复现失败。

#### 4. 技术洞察
演进的本质是契约稳定性：torch.testing.assert_close 在 |a−b| ≤ atol + rtol·|b| 时通过，float32 默认 rtol≈1.3e-6、atol≈1e-5；flaky 率 = 失败重跑数/总重跑数，需按阈值门禁。 中依赖版本/硬件的部分应被隔离并用抽象层收敛，任何未验证的升级结论都标注 [待验证]。

#### 5. 图文说明
| 层次 | 测试契约、容差与证据化调试 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 被测模块、参数组合、种子、容差与基线 | 容差过宽、随机未固定、基线过期 |
| 执行与状态 | 追踪 fixture 生命周期、全局状态与断言点 | 顺序相关、静默 skip、只测终值 |
| 输出与 oracle | 核对断言、失败可定位性与 flaky 统计 | 假绿、不可复现失败、回归漏检 |

图示解释：把 测试契约、容差与证据化调试 的契约面与实现面分离，沿版本轴观察契约面是否漂移，实现面可自由替换。

#### 6. 拓展知识
配套治理：版本兼容矩阵、种子固定与 test 隔离 与回退开关；比较新旧实现的成本与失败恢复，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 升级 PyTorch 或驱动后，测试契约、容差与证据化调试 的哪些行为可能变化，如何用 通过率 回归检测？
2. 如何在不停机的情况下灰度新实现并支持快速回退，回退点选在哪里？
3. 哪些边界（确定性种子与测试隔离、CI 门禁与 flaky 治理）最可能在演进中被破坏，如何冻结？

#### 8. 易错点与反例
1. 反例：把版本相关行为硬编码，升级后 容差过宽掩盖真实数值回归。
2. 反例：无回退开关直接全量上线，遇到 测试间共享全局状态导致顺序相关失败 无法止损。
3. 反例：只验证新功能不回归旧路径，直到 无 GPU 时静默 skip 使 CI 假绿 才暴露兼容性缺陷。

#### 9. 实践与验证
验证方案：为某模块写参数化测试（多 shape/dtype），固定种子并用加严容差的 assert_close；注入一个已知 bug 验证测试能失败。 在至少两个版本/设备上重复，用 pytest/parametrize/fixture 与 torch.testing.assert_close 记录 通过率、覆盖率 的历史曲线；oracle：测试在正确实现下稳定通过、在注入 bug 下必然失败，多次运行 flaky 率为 0。

#### 10. 评分标准
E046 评分：
- 不合格：只讲当前实现，没有契约与版本概念；
- 合格：能区分契约面与实现面并指出一个版本敏感点；
- 良好：能给出演进不变量、灰度与回退方案（覆盖 容差过宽掩盖真实数值回归）；
- 优秀/专家：能补充兼容矩阵、弃用迁移成本、长期维护与残余风险。
### E047. 跨层权衡：张量/设备

- 难度：专家级
- 知识域：K01 张量/设备
- 考察能力：正确性、性能、成本、运维与迁移条件
- 题型：对比与取舍
- 面试频率：开放题
- 前置知识：shape、stride、dtype、device
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

跨层权衡要求把 张量布局、视图语义与设备放置 放回 PyTorch 的技术栈中，考察它与相邻层的耦合：算子层、编译层、通信层与存储层。关键抉择通常不是对错而是代价转移，例如 stride 与 storage_offset 构成的地址映射 与 view/reshape/contiguous 的复制边界 的取舍。

#### 2. 问题分析
1. 明确本层职责与不负责的相邻职责，列出候选方案。
2. 用统一量纲比较成本：对连续张量有 stride[i]=∏_{j>i} shape[j]；view 只在目标 shape 能由既有 stride 整除分解时成立，否则 reshape 触发复制；实数占用满足 numel·itemsize ≤ storage.nbytes − storage_offset·itemsize。
3. 找出被转移的代价（显存换时间、精度换吞吐、通信换计算）。
4. 用 有效带宽 GB/s 与 碎片率 reserved/allocated 划定决策边界，并给出反例。

#### 3. 具体答案
面试中可以这样回答：

> 张量的本质是 (storage, size, stride, storage_offset) 四元组对一段连续内存的解释；视图与副本的区别决定是否共享存储、是否可安全原地写。设备放置是显式拓扑决策，.to() 按需搬运并可能触发隐式同步。
>
> 权衡量纲：对连续张量有 stride[i]=∏_{j>i} shape[j]；view 只在目标 shape 能由既有 stride 整除分解时成立，否则 reshape 触发复制；实数占用满足 numel·itemsize ≤ storage.nbytes − storage_offset·itemsize。
>
> 可验证事实：torch.Tensor.transpose 返回共享存储的视图（只互换 stride，不影响 storage），而 .reshape 在不可视图化时返回副本，可用 data_ptr() 是否相同判定。

#### 4. 技术洞察
跨层权衡的核心是代价守恒：节省一处必然在另一处付出，对连续张量有 stride[i]=∏_{j>i} shape[j]；view 只在目标 shape 能由既有 stride 整除分解时成立，否则 reshape 触发复制；实数占用满足 numel·itemsize ≤ storage.nbytes − storage_offset·itemsize。 给出了可比较的量纲；脱离该量纲的比较都是空谈。

#### 5. 图文说明
| 层次 | 张量布局、视图语义与设备放置 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 张量、目标 shape/stride、device/dtype 与 memory_format | 约束缺失会误判视图可行性或越界读 |
| 执行与状态 | 追踪 storage、stride、offset 与 allocator 块的 owner 和生命周期 | 共享存储被原地写、碎片泄漏或隐式同步 |
| 输出与 oracle | 对照 data_ptr、is_contiguous 与显存曲线 | 把副本误当视图、峰值显存与理论值严重偏离 |

图示解释：把 张量布局、视图语义与设备放置 的相邻层画成接口，标出每层承担的 有效带宽 GB/s、峰值显存 GiB 成本，观察代价如何转移。

#### 6. 拓展知识
相邻层手段：tensor.stride()/is_contiguous()/storage_offset()/data_ptr()、nsys/ncu 的内存搬运与带宽视图；对比不同分层的实现复杂度与故障面，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 本层把成本推给相邻层时，碎片率 reserved/allocated 会如何变化，是否可接受？
2. 在什么条件下选择方案 A 而非 B，决策阈值如何用 tensor.stride()/is_contiguous()/storage_offset()/data_ptr() 测量？
3. 如果相邻层版本或负载改变，当前权衡是否反转，如何检测？

#### 8. 易错点与反例
1. 反例：只优化本层指标，忽略 对非连续张量调用 view 抛 RuntimeError: view size is not compatible with input tensor's size and stride 被转移到相邻层。
2. 反例：用不同量纲比较方案，忽略 在未同步的跨界 .to() 后立即读数据得到脏值。
3. 反例：忽略负载相关性，在低负载下选择的方案遇到 pin_memory 张量长期驻留耗尽锁页内存 失效。

#### 9. 实践与验证
验证方案：对同一逻辑张量构造 contiguous、转置、expand 三种布局，断言 is_contiguous/stride/storage_offset，并用 data_ptr 判定 reshape 是否复制。 分别测量两种分层下的 有效带宽 GB/s 与 碎片率 reserved/allocated，做帕累托对比；oracle：data_ptr 相同判为视图、不同判为副本；峰值显存与理论 numel·itemsize 的偏差落在分配器块对齐范围内。

#### 10. 评分标准
E047 评分：
- 不合格：只堆砌方案，不指出代价转移；
- 合格：能说明一个跨层权衡并给出量纲（对连续张量有 stride[i]=∏_{j>i} shape[j]）；
- 良好：能给出决策边界、帕累托证据与反例；
- 优秀/专家：能补充负载相关性、版本演进下的权衡反转与残余风险。
### E048. 跨层权衡：autograd

- 难度：专家级
- 知识域：K02 autograd
- 考察能力：正确性、性能、成本、运维与迁移条件
- 题型：对比与取舍
- 面试频率：开放题
- 前置知识：requires_grad、leaf、view、detach、反向
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

跨层权衡要求把 动态计算图、梯度累积与高阶微分 放回 PyTorch 的技术栈中，考察它与相邻层的耦合：算子层、编译层、通信层与存储层。关键抉择通常不是对错而是代价转移，例如 动态图与 grad_fn 链 与 叶张量与 .grad 累加 的取舍。

#### 2. 问题分析
1. 明确本层职责与不负责的相邻职责，列出候选方案。
2. 用统一量纲比较成本：反向传播沿 grad_fn 链执行链式法则 g_i=Σ_j g_j·(∂f_j/∂x_i)，梯度以累加方式写入 .grad（不清零则叠加）；对需要梯度的张量做 in-place 会使其 version counter 加一，若该版本被 backward 引用则抛 RuntimeError: a variable needed for gradient computation has been modified by an inplace operation。
3. 找出被转移的代价（显存换时间、精度换吞吐、通信换计算）。
4. 用 梯度相对误差 与 计算图节点数 划定决策边界，并给出反例。

#### 3. 具体答案
面试中可以这样回答：

> autograd 记录前向操作序列构成的动态图，反向时按逆序调用 grad_fn，用链式法则把上游梯度乘上局部雅可比后累加到叶张量。图的生命周期由 retain_graph 控制，原地写通过 version counter 保证数值一致。
>
> 权衡量纲：反向传播沿 grad_fn 链执行链式法则 g_i=Σ_j g_j·(∂f_j/∂x_i)，梯度以累加方式写入 .grad（不清零则叠加）；对需要梯度的张量做 in-place 会使其 version counter 加一，若该版本被 backward 引用则抛 RuntimeError: a variable needed for gradient computation has been modified by an inplace operation。
>
> 可验证事实：torch.autograd.gradcheck 在 float64 下用有限差分（默认 eps=1e-6、rtol=1.3e-6、atol=1e-5）校验解析梯度，要求输入 requires_grad=True 且函数确定性。

#### 4. 技术洞察
跨层权衡的核心是代价守恒：节省一处必然在另一处付出，反向传播沿 grad_fn 链执行链式法则 g_i=Σ_j g_j·(∂f_j/∂x_i)，梯度以累加方式写入 .grad（不清零则叠加）；对需要梯度的张量做 in-place 会使其 version counter 加一，若该版本被 backward 引用则抛 RuntimeError: a variable needed for gradient computation has been modified by an inplace operation。 给出了可比较的量纲；脱离该量纲的比较都是空谈。

#### 5. 图文说明
| 层次 | 动态计算图、梯度累积与高阶微分 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | requires_grad 状态、叶张量标志、是否原地写 | 误把非叶张量当叶、图已释放 |
| 执行与状态 | 遍历 grad_fn 链、version counter、saved tensor 所有权 | 图重复释放、原地写破坏反向、梯度断链 |
| 输出与 oracle | 比对解析梯度与数值差分、.grad 累加结果 | 梯度为零/None、误差超容差、二阶导缺失 |

图示解释：把 动态计算图、梯度累积与高阶微分 的相邻层画成接口，标出每层承担的 梯度相对误差、峰值激活内存 MiB 成本，观察代价如何转移。

#### 6. 拓展知识
相邻层手段：tensor.grad_fn 与 next_functions 遍历、torch.autograd.grad 的 retain_graph/create_graph；对比不同分层的实现复杂度与故障面，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 本层把成本推给相邻层时，计算图节点数 会如何变化，是否可接受？
2. 在什么条件下选择方案 A 而非 B，决策阈值如何用 tensor.grad_fn 与 next_functions 遍历 测量？
3. 如果相邻层版本或负载改变，当前权衡是否反转，如何检测？

#### 8. 易错点与反例
1. 反例：只优化本层指标，忽略 detach 后仍期望梯度回传导致叶张量 .grad 为 None 被转移到相邻层。
2. 反例：用不同量纲比较方案，忽略 第二次 backward 未设 retain_graph=True 触发 Trying to backward through the graph a second time。
3. 反例：忽略负载相关性，在低负载下选择的方案遇到 对参与反向的张量原地写触发 version counter 报错 失效。

#### 9. 实践与验证
验证方案：构造 y=x·sin(x)，用 torch.autograd.grad 求一阶导并与有限差分对比；再开 create_graph=True 求二阶导，验证与解析式 cos(x)−x·sin(x) 一致。 分别测量两种分层下的 梯度相对误差 与 计算图节点数，做帕累托对比；oracle：gradcheck 返回 True（误差在 rtol=1e-3/atol=1e-5 内），二阶导相对误差 < 1e-4。

#### 10. 评分标准
E048 评分：
- 不合格：只堆砌方案，不指出代价转移；
- 合格：能说明一个跨层权衡并给出量纲（反向传播沿 grad_fn 链执行链式法则 g_i=Σ_j g_j·(∂f_j/∂x_i)，梯度以累加方式写入 .grad（不清零则叠加））；
- 良好：能给出决策边界、帕累托证据与反例；
- 优秀/专家：能补充负载相关性、版本演进下的权衡反转与残余风险。
### E049. 跨层权衡：Module/状态

- 难度：专家级
- 知识域：K03 Module/状态
- 考察能力：正确性、性能、成本、运维与迁移条件
- 题型：对比与取舍
- 面试频率：开放题
- 前置知识：参数、buffer、模式和 state_dict
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

跨层权衡要求把 模块树、参数字典与状态持久化 放回 PyTorch 的技术栈中，考察它与相邻层的耦合：算子层、编译层、通信层与存储层。关键抉择通常不是对错而是代价转移，例如 Module 树与命名参数 与 parameter 与 buffer 的区别 的取舍。

#### 2. 问题分析
1. 明确本层职责与不负责的相邻职责，列出候选方案。
2. 用统一量纲比较成本：state_dict 的键等于模块路径拼接参数/缓冲区名（如 layer1.0.conv.weight）；load_state_dict(strict=True) 要求键集合完全一致，否则报 missing/unexpected keys；buffer 不参与 optimizer 更新但会被 .to() 搬运。
3. 找出被转移的代价（显存换时间、精度换吞吐、通信换计算）。
4. 用 参数量 与 状态字典字节数 划定决策边界，并给出反例。

#### 3. 具体答案
面试中可以这样回答：

> nn.Module 是一棵注册树，参数与缓冲区经命名空间形成扁平键空间；state_dict 是这棵树的快照契约，.to() 递归搬运所有注册张量。模式切换与注册完整性共同决定训练/推理一致性。
>
> 权衡量纲：state_dict 的键等于模块路径拼接参数/缓冲区名（如 layer1.0.conv.weight）；load_state_dict(strict=True) 要求键集合完全一致，否则报 missing/unexpected keys；buffer 不参与 optimizer 更新但会被 .to() 搬运。
>
> 可验证事实：model.eval() 只切换 dropout/BatchNorm 等模块的 training 标志，不关闭 autograd；省显存必须叠加 torch.no_grad() 或 inference_mode()。

#### 4. 技术洞察
跨层权衡的核心是代价守恒：节省一处必然在另一处付出，state_dict 的键等于模块路径拼接参数/缓冲区名（如 layer1.0.conv.weight）；load_state_dict(strict=True) 要求键集合完全一致，否则报 missing/unexpected keys；buffer 不参与 optimizer 更新但会被 .to() 搬运。 给出了可比较的量纲；脱离该量纲的比较都是空谈。

#### 5. 图文说明
| 层次 | 模块树、参数字典与状态持久化 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 模块结构、注册的参数/缓冲区、目标 device/dtype | 漏注册 buffer 或键空间不一致 |
| 执行与状态 | 遍历模块树、加载/保存快照、切换 training 标志 | 键缺失被静默、模式与图不匹配、别名错乱 |
| 输出与 oracle | 逐张量比对 state_dict、核对模式相关的统计 | 权重随机初始化、BN 统计漂移、显存未释放 |

图示解释：把 模块树、参数字典与状态持久化 的相邻层画成接口，标出每层承担的 参数量、键匹配率 成本，观察代价如何转移。

#### 6. 拓展知识
相邻层手段：named_parameters()/named_buffers()/recurse、module.apply/children/named_modules；对比不同分层的实现复杂度与故障面，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 本层把成本推给相邻层时，状态字典字节数 会如何变化，是否可接受？
2. 在什么条件下选择方案 A 而非 B，决策阈值如何用 named_parameters()/named_buffers()/recurse 测量？
3. 如果相邻层版本或负载改变，当前权衡是否反转，如何检测？

#### 8. 易错点与反例
1. 反例：只优化本层指标，忽略 buffer 未 register_buffer 导致不随 .to() 和 state_dict 迁移 被转移到相邻层。
2. 反例：用不同量纲比较方案，忽略 共享参数被重复注册改变键空间与加载行为。
3. 反例：忽略负载相关性，在低负载下选择的方案遇到 strict=False 静默吞掉缺失键使部分权重随机初始化 失效。

#### 9. 实践与验证
验证方案：构造含卷积、BN、自定义 buffer 的模块，round-trip state_dict 到新实例并 assert_close 全部张量；删掉一个 buffer 验证 strict=True 报 missing keys。 分别测量两种分层下的 参数量 与 状态字典字节数，做帕累托对比；oracle：load_state_dict 返回 missing=[]、unexpected=[]；train 模式两次 forward 后 BN running_mean 发生变化。

#### 10. 评分标准
E049 评分：
- 不合格：只堆砌方案，不指出代价转移；
- 合格：能说明一个跨层权衡并给出量纲（state_dict 的键等于模块路径拼接参数/缓冲区名（如 layer1.0.conv.weight））；
- 良好：能给出决策边界、帕累托证据与反例；
- 优秀/专家：能补充负载相关性、版本演进下的权衡反转与残余风险。
### E050. 跨层权衡：数据管道

- 难度：专家级
- 知识域：K04 数据管道
- 考察能力：正确性、性能、成本、运维与迁移条件
- 题型：对比与取舍
- 面试频率：开放题
- 前置知识：Dataset、Sampler、collate、padding、版本
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

跨层权衡要求把 DataLoader 的并行、采样与预取 放回 PyTorch 的技术栈中，考察它与相邻层的耦合：算子层、编译层、通信层与存储层。关键抉择通常不是对错而是代价转移，例如 map-style 与 iterable-style Dataset 与 num_workers 与进程启动方式 的取舍。

#### 2. 问题分析
1. 明确本层职责与不负责的相邻职责，列出候选方案。
2. 用统一量纲比较成本：map-style 数据集每 epoch 批数 = ⌈N/batch_size⌉（drop_last 时为 ⌊N/batch_size⌋）；worker 是独立进程并各自持有 RNG，随机变换必须经 worker_init_fn 播种才可复现。
3. 找出被转移的代价（显存换时间、精度换吞吐、通信换计算）。
4. 用 吞吐 samples/s 与 GPU 利用率 划定决策边界，并给出反例。

#### 3. 具体答案
面试中可以这样回答：

> 数据管道把采样、取数、批处理和预取解耦到多 worker 进程；正确性取决于索引覆盖与随机性可复现，性能取决于预取深度与 H2D 重叠。
>
> 权衡量纲：map-style 数据集每 epoch 批数 = ⌈N/batch_size⌉（drop_last 时为 ⌊N/batch_size⌋）；worker 是独立进程并各自持有 RNG，随机变换必须经 worker_init_fn 播种才可复现。
>
> 可验证事实：pin_memory=True 配合 non_blocking=True 能把 H2D 拷贝与计算重叠；persistent_workers=True 在 epoch 间保留 worker 进程，避免反复 fork 开销。

#### 4. 技术洞察
跨层权衡的核心是代价守恒：节省一处必然在另一处付出，map-style 数据集每 epoch 批数 = ⌈N/batch_size⌉（drop_last 时为 ⌊N/batch_size⌋）；worker 是独立进程并各自持有 RNG，随机变换必须经 worker_init_fn 播种才可复现。 给出了可比较的量纲；脱离该量纲的比较都是空谈。

#### 5. 图文说明
| 层次 | DataLoader 的并行、采样与预取 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 数据集大小、batch_size、worker 数、采样方式 | 索引不覆盖、随机性不可复现 |
| 执行与状态 | 追踪 worker 进程、预取队列、pin 缓冲与 sampler 状态 | CUDA-in-fork 崩溃、句柄泄漏、顺序错乱 |
| 输出与 oracle | 核对样本集合、shape、吞吐与首 batch 延迟 | 重复/丢失样本、collate 形状错误、吞吐饱和异常 |

图示解释：把 DataLoader 的并行、采样与预取 的相邻层画成接口，标出每层承担的 吞吐 samples/s、首 batch 延迟 成本，观察代价如何转移。

#### 6. 拓展知识
相邻层手段：DataLoader/worker_init_fn/generator、输入流水线 trace/cProfile；对比不同分层的实现复杂度与故障面，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 本层把成本推给相邻层时，GPU 利用率 会如何变化，是否可接受？
2. 在什么条件下选择方案 A 而非 B，决策阈值如何用 DataLoader/worker_init_fn/generator 测量？
3. 如果相邻层版本或负载改变，当前权衡是否反转，如何检测？

#### 8. 易错点与反例
1. 反例：只优化本层指标，忽略 在 Dataset.__init__ 或 __getitem__ 中初始化 CUDA 导致 fork 后 worker 崩溃 被转移到相邻层。
2. 反例：用不同量纲比较方案，忽略 worker 数过大引发内存或文件句柄耗尽。
3. 反例：忽略负载相关性，在低负载下选择的方案遇到 collate_fn 对不定长序列堆叠得到错误 shape 失效。

#### 9. 实践与验证
验证方案：固定 seed 与 worker_init_fn，连续两 epoch 记录样本索引序列验证覆盖完整且可复现；测量 num_workers∈{0,2,4,8} 的吞吐拐点。 分别测量两种分层下的 吞吐 samples/s 与 GPU 利用率，做帕累托对比；oracle：两轮索引并集等于 0..N−1 且无重复，吞吐在某一 worker 数后饱和。

#### 10. 评分标准
E050 评分：
- 不合格：只堆砌方案，不指出代价转移；
- 合格：能说明一个跨层权衡并给出量纲（map-style 数据集每 epoch 批数 = ⌈N/batch_size⌉（drop_last 时为 ⌊N/batch_size⌋））；
- 良好：能给出决策边界、帕累托证据与反例；
- 优秀/专家：能补充负载相关性、版本演进下的权衡反转与残余风险。
### E051. 跨层权衡：训练/优化器

- 难度：专家级
- 知识域：K05 训练/优化器
- 考察能力：输入矩阵、oracle、故障注入与指标
- 题型：测试/实验与验证
- 面试频率：开放题
- 前置知识：forward、backward、累积、optimizer、scheduler
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

跨层权衡要求把 优化器更新、学习率调度与梯度处理 放回 PyTorch 的技术栈中，考察它与相邻层的耦合：算子层、编译层、通信层与存储层。关键抉择通常不是对错而是代价转移，例如 优化器状态（Adam 的 m/v） 与 zero_grad 语义 的取舍。

#### 2. 问题分析
1. 明确本层职责与不负责的相邻职责，列出候选方案。
2. 用统一量纲比较成本：Adam：m_t=β1·m_{t−1}+(1−β1)·g_t，v_t=β2·v_{t−1}+(1−β2)·g_t²，偏差校正 m̂=m/(1−β1^t)、v̂=v/(1−β2^t)，θ←θ−lr·m̂/(√v̂+ε)；梯度累积 k 步等价于把有效批放大 k 倍，但必须对 loss 除以 k 或按 k 归约，否则梯度被放大 k 倍。
3. 找出被转移的代价（显存换时间、精度换吞吐、通信换计算）。
4. 用 loss 曲线 与 全局梯度范数 划定决策边界，并给出反例。

#### 3. 具体答案
面试中可以这样回答：

> 训练步是 zero_grad→forward→loss→scaled backward→unscale→clip→step→scheduler 的固定顺序；顺序错位会静默改变有效学习率或裁剪幅度。
>
> 权衡量纲：Adam：m_t=β1·m_{t−1}+(1−β1)·g_t，v_t=β2·v_{t−1}+(1−β2)·g_t²，偏差校正 m̂=m/(1−β1^t)、v̂=v/(1−β2^t)，θ←θ−lr·m̂/(√v̂+ε)；梯度累积 k 步等价于把有效批放大 k 倍，但必须对 loss 除以 k 或按 k 归约，否则梯度被放大 k 倍。
>
> 可验证事实：zero_grad(set_to_none=True) 把 .grad 置 None（自 PyTorch 2.0 起为默认行为），比置零更省内存；optimizer.state_dict() 按参数索引保存 momentum/m/v 缓冲区。

#### 4. 技术洞察
跨层权衡的核心是代价守恒：节省一处必然在另一处付出，Adam：m_t=β1·m_{t−1}+(1−β1)·g_t，v_t=β2·v_{t−1}+(1−β2)·g_t²，偏差校正 m̂=m/(1−β1^t)、v̂=v/(1−β2^t)，θ←θ−lr·m̂/(√v̂+ε)；梯度累积 k 步等价于把有效批放大 k 倍，但必须对 loss 除以 k 或按 k 归约，否则梯度被放大 k 倍。 给出了可比较的量纲；脱离该量纲的比较都是空谈。

#### 5. 图文说明
| 层次 | 优化器更新、学习率调度与梯度处理 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 初始参数、优化器配置、损失缩放与累积步 k | 顺序错位、未缩放 loss |
| 执行与状态 | 追踪 .grad、优化器状态缓冲区、lr 调度历史 | 梯度残留、动量错乱、裁剪幅度被缩放 |
| 输出与 oracle | 核对 loss 单调性、参数更新与 lr 序列 | loss 震荡、等效 lr 漂移、累积实现不等价 |

图示解释：把 优化器更新、学习率调度与梯度处理 的相邻层画成接口，标出每层承担的 loss 曲线、学习率序列 成本，观察代价如何转移。

#### 6. 拓展知识
相邻层手段：optimizer.state_dict()/param_groups、torch.optim.lr_scheduler；对比不同分层的实现复杂度与故障面，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 本层把成本推给相邻层时，全局梯度范数 会如何变化，是否可接受？
2. 在什么条件下选择方案 A 而非 B，决策阈值如何用 optimizer.state_dict()/param_groups 测量？
3. 如果相邻层版本或负载改变，当前权衡是否反转，如何检测？

#### 8. 易错点与反例
1. 反例：只优化本层指标，忽略 忘记 zero_grad 使梯度跨 step 累加，等效 lr 漂移 被转移到相邻层。
2. 反例：用不同量纲比较方案，忽略 scheduler.step 放错位置（每 batch 而非每 epoch）改变 lr 曲线。
3. 反例：忽略负载相关性，在低负载下选择的方案遇到 clip_grad_norm_ 在 GradScaler.unscale_ 之前调用使裁剪阈值被放大 S 倍 失效。

#### 9. 实践与验证
验证方案：在单个 batch 上跑 200 步验证 loss 单调下降到过拟合；打印 get_last_lr 序列核对调度曲线；对同一初始点比较累积实现与放大批实现的参数更新。 分别测量两种分层下的 loss 曲线 与 全局梯度范数，做帕累托对比；oracle：单批过拟合 loss < 0.01；累积与大批实现的参数差在 1e-6 内。

#### 10. 评分标准
E051 评分：
- 不合格：只堆砌方案，不指出代价转移；
- 合格：能说明一个跨层权衡并给出量纲（Adam：m_t=β1·m_{t−1}+(1−β1)·g_t，v_t=β2·v_{t−1}+(1−β2)·g_t²，偏差校正 m̂=m/(1−β1^t)、v̂=v/(1−β2^t)，θ←θ−lr·m̂/(√v̂+ε)）；
- 良好：能给出决策边界、帕累托证据与反例；
- 优秀/专家：能补充负载相关性、版本演进下的权衡反转与残余风险。
### E052. 跨层权衡：AMP/恢复

- 难度：专家级
- 知识域：K06 AMP/恢复
- 考察能力：输入矩阵、oracle、故障注入与指标
- 题型：测试/实验与验证
- 面试频率：开放题
- 前置知识：autocast、scaler、快照、随机状态
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

跨层权衡要求把 混合精度、检查点与确定性 放回 PyTorch 的技术栈中，考察它与相邻层的耦合：算子层、编译层、通信层与存储层。关键抉择通常不是对错而是代价转移，例如 autocast 的算子白名单 与 GradScaler 动态缩放 的取舍。

#### 2. 问题分析
1. 明确本层职责与不负责的相邻职责，列出候选方案。
2. 用统一量纲比较成本：GradScaler 先用 scale S 放大 loss 再反向，unscale_ 后若检测到 inf/nan 则跳过该 step 并把 S 减半；fp16 最正规数约 6.1e-5、次正规至约 6e-8，小于此量级的梯度下溢，故需缩放；bf16 指数位与 fp32 同为 8 位因此不需要缩放。
3. 找出被转移的代价（显存换时间、精度换吞吐、通信换计算）。
4. 用 scale factor 曲线 与 NaN/inf 次数 划定决策边界，并给出反例。

#### 3. 具体答案
面试中可以这样回答：

> AMP 通过 autocast 选低精度算子、GradScaler 维持梯度动态范围；恢复的正确性是状态闭包问题——模型、优化器、缩放与随机数状态缺一不可。
>
> 权衡量纲：GradScaler 先用 scale S 放大 loss 再反向，unscale_ 后若检测到 inf/nan 则跳过该 step 并把 S 减半；fp16 最正规数约 6.1e-5、次正规至约 6e-8，小于此量级的梯度下溢，故需缩放；bf16 指数位与 fp32 同为 8 位因此不需要缩放。
>
> 可验证事实：torch.use_deterministic_algorithms(True) 会让部分算子回退到确定性实现或直接报错；cudnn.benchmark=True 的自动调优会破坏逐步可复现性。完整恢复需保存 model、optimizer、scaler、epoch、CPU 与 CUDA RNG 及 sampler 状态。

#### 4. 技术洞察
跨层权衡的核心是代价守恒：节省一处必然在另一处付出，GradScaler 先用 scale S 放大 loss 再反向，unscale_ 后若检测到 inf/nan 则跳过该 step 并把 S 减半；fp16 最正规数约 6.1e-5、次正规至约 6e-8，小于此量级的梯度下溢，故需缩放；bf16 指数位与 fp32 同为 8 位因此不需要缩放。 给出了可比较的量纲；脱离该量纲的比较都是空谈。

#### 5. 图文说明
| 层次 | 混合精度、检查点与确定性 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | dtype、autocast 区域、scaler 配置、checkpoint 内容 | 状态不闭合、精度选择错误 |
| 执行与状态 | 追踪 scale factor、RNG、优化器状态与 nan/inf 分支 | 下溢、NaN、恢复后状态漂移 |
| 输出与 oracle | 比对续训与基线参数、scale 曲线、依赖确定性开关 | 曲线跳变、结果不可复现、梯度全零 |

图示解释：把 混合精度、检查点与确定性 的相邻层画成接口，标出每层承担的 scale factor 曲线、resume 前后参数最大差 成本，观察代价如何转移。

#### 6. 拓展知识
相邻层手段：torch.cuda.amp.GradScaler 与 autocast、CUBLAS_WORKSPACE_CONFIG=:4096:8；对比不同分层的实现复杂度与故障面，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 本层把成本推给相邻层时，NaN/inf 次数 会如何变化，是否可接受？
2. 在什么条件下选择方案 A 而非 B，决策阈值如何用 torch.cuda.amp.GradScaler 与 autocast 测量？
3. 如果相邻层版本或负载改变，当前权衡是否反转，如何检测？

#### 8. 易错点与反例
1. 反例：只优化本层指标，忽略 fp16 无 scaler 导致梯度下溢为 0 或 NaN 被转移到相邻层。
2. 反例：用不同量纲比较方案，忽略 resume 只存模型权重丢失优化器动量使曲线跳变。
3. 反例：忽略负载相关性，在低负载下选择的方案遇到 只存 torch RNG 未存 numpy/random 状态导致数据顺序不同 失效。

#### 9. 实践与验证
验证方案：训练 50 步保存完整 checkpoint，重启加载后再跑 50 步与不中断基线逐参数比较；fp16 下注入大梯度观测 S 是否减半。 分别测量两种分层下的 scale factor 曲线 与 NaN/inf 次数，做帕累托对比；oracle：续训与基线参数最大差 < 1e-5（bit 级取决于确定性开关），scale factor 与跳过计数一致。

#### 10. 评分标准
E052 评分：
- 不合格：只堆砌方案，不指出代价转移；
- 合格：能说明一个跨层权衡并给出量纲（GradScaler 先用 scale S 放大 loss 再反向，unscale_ 后若检测到 inf/nan 则跳过该 step 并把 S 减半）；
- 良好：能给出决策边界、帕累托证据与反例；
- 优秀/专家：能补充负载相关性、版本演进下的权衡反转与残余风险。
### E053. 跨层权衡：分布式

- 难度：专家级
- 知识域：K07 分布式
- 考察能力：输入矩阵、oracle、故障注入与指标
- 题型：测试/实验与验证
- 面试频率：开放题
- 前置知识：rank、DDP、FSDP、collective、容错
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

跨层权衡要求把 进程组、集合通信与梯度同步 放回 PyTorch 的技术栈中，考察它与相邻层的耦合：算子层、编译层、通信层与存储层。关键抉择通常不是对错而是代价转移，例如 进程组与 rank/world_size 与 all_reduce/reduce_scatter/all_gather 的取舍。

#### 2. 问题分析
1. 明确本层职责与不负责的相邻职责，列出候选方案。
2. 用统一量纲比较成本：DDP 默认 all_reduce(sum) 后除以 world_size 得平均梯度，故全局有效批 = 本地批 × world_size × 累积步数；ring all_reduce 单步通信量 ≈ 2·(N−1)/N·S（S 为梯度字节数），大 N 时趋近 2S。
3. 找出被转移的代价（显存换时间、精度换吞吐、通信换计算）。
4. 用 all_reduce 带宽 GB/s 与 通信时间占比 划定决策边界，并给出反例。

#### 3. 具体答案
面试中可以这样回答：

> 分布式训练把单机步拆成计算→梯度归约→更新三段；正确性在于所有 rank 以相同顺序进入集合通信且梯度按 world_size 归一，性能在于把通信藏进反向的尾部。
>
> 权衡量纲：DDP 默认 all_reduce(sum) 后除以 world_size 得平均梯度，故全局有效批 = 本地批 × world_size × 累积步数；ring all_reduce 单步通信量 ≈ 2·(N−1)/N·S（S 为梯度字节数），大 N 时趋近 2S。
>
> 可验证事实：NCCL 只支持 CUDA 张量、gloo 支持 CPU；DDP 默认把梯度分成约 25 MiB 的桶以便与反向计算重叠通信。

#### 4. 技术洞察
跨层权衡的核心是代价守恒：节省一处必然在另一处付出，DDP 默认 all_reduce(sum) 后除以 world_size 得平均梯度，故全局有效批 = 本地批 × world_size × 累积步数；ring all_reduce 单步通信量 ≈ 2·(N−1)/N·S（S 为梯度字节数），大 N 时趋近 2S。 给出了可比较的量纲；脱离该量纲的比较都是空谈。

#### 5. 图文说明
| 层次 | 进程组、集合通信与梯度同步 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 进程数、后端、张量是否 CUDA、参与反向的参数集合 | 后端不匹配、参数未参与、集合调用不一致 |
| 执行与状态 | 追踪 rank、bucket、通信算子与反向重叠 | hang、梯度未归约、通信未重叠 |
| 输出与 oracle | 对比单卡/DDP 梯度、通信带宽与 step 方差 | 梯度差超容差、超时、吞吐不随卡数提升 |

图示解释：把 进程组、集合通信与梯度同步 的相邻层画成接口，标出每层承担的 all_reduce 带宽 GB/s、step 时间方差 成本，观察代价如何转移。

#### 6. 拓展知识
相邻层手段：torch.distributed.init_process_group/barrier、torchrun；对比不同分层的实现复杂度与故障面，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 本层把成本推给相邻层时，通信时间占比 会如何变化，是否可接受？
2. 在什么条件下选择方案 A 而非 B，决策阈值如何用 torch.distributed.init_process_group/barrier 测量？
3. 如果相邻层版本或负载改变，当前权衡是否反转，如何检测？

#### 8. 易错点与反例
1. 反例：只优化本层指标，忽略 各 rank 集合通信调用不匹配导致 hang 直到 watchdog 超时 被转移到相邻层。
2. 反例：用不同量纲比较方案，忽略 未按 world_size 归一梯度使等效 lr 随卡数变化。
3. 反例：忽略负载相关性，在低负载下选择的方案遇到 NCCL/驱动版本不一致引发通信超时或崩溃 失效。

#### 9. 实践与验证
验证方案：用 torchrun 起 2 进程，固定初始化使单卡与 DDP 梯度可对齐，比较 all_reduce 前后梯度；人为让一个 rank 提前退出观测 NCCL watchdog 超时。 分别测量两种分层下的 all_reduce 带宽 GB/s 与 通信时间占比，做帕累托对比；oracle：DDP 与单卡初始梯度（除以 world_size 后）相对误差 < 1e-6；NCCL_DEBUG 显示预期的 ring/tree 算法。

#### 10. 评分标准
E053 评分：
- 不合格：只堆砌方案，不指出代价转移；
- 合格：能说明一个跨层权衡并给出量纲（DDP 默认 all_reduce(sum) 后除以 world_size 得平均梯度，故全局有效批 = 本地批 × world_size × 累积步数）；
- 良好：能给出决策边界、帕累托证据与反例；
- 优秀/专家：能补充负载相关性、版本演进下的权衡反转与残余风险。
### E054. 跨层权衡：编译栈

- 难度：专家级
- 知识域：K08 编译栈
- 考察能力：输入矩阵、oracle、故障注入与指标
- 题型：测试/实验与验证
- 面试频率：开放题
- 前置知识：Dynamo、AOTAutograd、Inductor、guard、缓存
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

跨层权衡要求把 Dynamo 捕获、guard 与 Inductor 代码生成 放回 PyTorch 的技术栈中，考察它与相邻层的耦合：算子层、编译层、通信层与存储层。关键抉择通常不是对错而是代价转移，例如 TorchDynamo 的字节码捕获 与 guard 与 specialization 的取舍。

#### 2. 问题分析
1. 明确本层职责与不负责的相邻职责，列出候选方案。
2. 用统一量纲比较成本：一个编译单元的 guard 绑定在张量的 shape/dtype/device 与 Python 常量上；任一 guard 失效即触发重编译，默认累计重编译上限约 8 次（torch._dynamo.config.accumulated_recompile_limit）。图被 graph break 切成多个子图，未捕获区间回退 eager。
3. 找出被转移的代价（显存换时间、精度换吞吐、通信换计算）。
4. 用 重编译次数 与 graph break 数 划定决策边界，并给出反例。

#### 3. 具体答案
面试中可以这样回答：

> 编译栈把 Python 执行轨迹捕获成 FX 图，用 guard 保证图对后续输入仍有效；失效即重编译、无法捕获即断图。稳定收益来自低断点数与低重编译率而非一次加速。
>
> 权衡量纲：一个编译单元的 guard 绑定在张量的 shape/dtype/device 与 Python 常量上；任一 guard 失效即触发重编译，默认累计重编译上限约 8 次（torch._dynamo.config.accumulated_recompile_limit）。图被 graph break 切成多个子图，未捕获区间回退 eager。
>
> 可验证事实：设置 TORCH_LOGS=graph_breaks,guards,recompiles 可打印断点与重编译原因；torch._dynamo.explain(fn)(*args) 返回断点数量与原因；dynamic=True 或 torch._dynamo.mark_dynamic 可放宽形状守卫。

#### 4. 技术洞察
跨层权衡的核心是代价守恒：节省一处必然在另一处付出，一个编译单元的 guard 绑定在张量的 shape/dtype/device 与 Python 常量上；任一 guard 失效即触发重编译，默认累计重编译上限约 8 次（torch._dynamo.config.accumulated_recompile_limit）。图被 graph break 切成多个子图，未捕获区间回退 eager。 给出了可比较的量纲；脱离该量纲的比较都是空谈。

#### 5. 图文说明
| 层次 | Dynamo 捕获、guard 与 Inductor 代码生成 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 函数代码、示例输入、shape/dtype/device 与 dynamic 声明 | 守卫过严、断点隐藏、动态维度未声明 |
| 执行与状态 | 追踪 Dynamo 捕获、guard 集合、子图与 Inductor 缓存 | 重编译风暴、断点回退 eager、缓存未命中 |
| 输出与 oracle | 对比 compiled/eager 数值、重编译与断点计数 | 数值不一致、加速比 < 1、重编译失控 |

图示解释：把 Dynamo 捕获、guard 与 Inductor 代码生成 的相邻层画成接口，标出每层承担的 重编译次数、compiled/eager 加速比 成本，观察代价如何转移。

#### 6. 拓展知识
相邻层手段：TORCH_LOGS=graph_breaks,guards,recompiles、torch._inductor.config；对比不同分层的实现复杂度与故障面，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 本层把成本推给相邻层时，graph break 数 会如何变化，是否可接受？
2. 在什么条件下选择方案 A 而非 B，决策阈值如何用 TORCH_LOGS=graph_breaks,guards,recompiles 测量？
3. 如果相邻层版本或负载改变，当前权衡是否反转，如何检测？

#### 8. 易错点与反例
1. 反例：只优化本层指标，忽略 在图中调用 .item()/print/依赖数据的 Python 分支触发 graph break 被转移到相邻层。
2. 反例：用不同量纲比较方案，忽略 动态 shape 引发重编译风暴使性能倒挂。
3. 反例：忽略负载相关性，在低负载下选择的方案遇到 别名或原地写使编译结果与 eager 数值不一致 失效。

#### 9. 实践与验证
验证方案：对同一函数用固定 shape 与变化 shape 各跑 20 次，用 explain 统计 graph break 与重编译；对比 compiled 与 eager 输出最大差。 分别测量两种分层下的 重编译次数 与 graph break 数，做帕累托对比；oracle：固定 shape 下重编译次数为 1、输出与 eager 最大差 < 1e-4；变化 shape 下开启 dynamic 后重编译显著下降。

#### 10. 评分标准
E054 评分：
- 不合格：只堆砌方案，不指出代价转移；
- 合格：能说明一个跨层权衡并给出量纲（一个编译单元的 guard 绑定在张量的 shape/dtype/device 与 Python 常量上）；
- 良好：能给出决策边界、帕累托证据与反例；
- 优秀/专家：能补充负载相关性、版本演进下的权衡反转与残余风险。
### E055. 跨层权衡：性能

- 难度：专家级
- 知识域：K09 性能
- 考察能力：输入矩阵、oracle、故障注入与指标
- 题型：测试/实验与验证
- 面试频率：开放题
- 前置知识：关键路径、带宽、算力、通信、显存、统计
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

跨层权衡要求把 性能剖析、roofline 与显存容量 放回 PyTorch 的技术栈中，考察它与相邻层的耦合：算子层、编译层、通信层与存储层。关键抉择通常不是对错而是代价转移，例如 CUDA event 计时与同步 与 roofline 与算术强度 的取舍。

#### 2. 问题分析
1. 明确本层职责与不负责的相邻职责，列出候选方案。
2. 用统一量纲比较成本：算术强度 I=FLOPs/Bytes，roofline 上界 = min(峰值算力 P, I×峰值带宽 BW)，据此判定 compute-bound 或 memory-bound；CUDA event 计时为 t=elapsed_time(start,stop)，异步操作必须 torch.cuda.synchronize() 后计时才可信。
3. 找出被转移的代价（显存换时间、精度换吞吐、通信换计算）。
4. 用 p50/p99 延迟 与 有效带宽 GB/s 划定决策边界，并给出反例。

#### 3. 具体答案
面试中可以这样回答：

> 性能结论必须绑定可复现的测量协议：warmup、同步、重复次数、分位数；再用 roofline 判定算力受限还是带宽受限，最后才谈优化手段。
>
> 权衡量纲：算术强度 I=FLOPs/Bytes，roofline 上界 = min(峰值算力 P, I×峰值带宽 BW)，据此判定 compute-bound 或 memory-bound；CUDA event 计时为 t=elapsed_time(start,stop)，异步操作必须 torch.cuda.synchronize() 后计时才可信。
>
> 可验证事实：torch.profiler.profile(activities=[ProfilerActivity.CUDA]) 会插入 CUDA 事件并输出按算子聚合的 self/total CUDA time；首次迭代含 cuBLAS/context 初始化与自动调优，必须先 warmup 再计时。

#### 4. 技术洞察
跨层权衡的核心是代价守恒：节省一处必然在另一处付出，算术强度 I=FLOPs/Bytes，roofline 上界 = min(峰值算力 P, I×峰值带宽 BW)，据此判定 compute-bound 或 memory-bound；CUDA event 计时为 t=elapsed_time(start,stop)，异步操作必须 torch.cuda.synchronize() 后计时才可信。 给出了可比较的量纲；脱离该量纲的比较都是空谈。

#### 5. 图文说明
| 层次 | 性能剖析、roofline 与显存容量 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 算子/规模、dtype、warmup 与 repeat、计时方式 | 未同步、未 warmup、规模误导 |
| 执行与状态 | 追踪 kernel 时长、显存曲线、launch 与同步点 | 计时偏短、碎片抖动、把 launch 开销当算力 |
| 输出与 oracle | 对比 event/profiler、roofline 定位与分位数 | 加速比不稳定、有效带宽超理论值、尾延迟超标 |

图示解释：把 性能剖析、roofline 与显存容量 的相邻层画成接口，标出每层承担的 p50/p99 延迟、MFU/TFLOPS 成本，观察代价如何转移。

#### 6. 拓展知识
相邻层手段：torch.profiler、torch.cuda.max_memory_allocated；对比不同分层的实现复杂度与故障面，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 本层把成本推给相邻层时，有效带宽 GB/s 会如何变化，是否可接受？
2. 在什么条件下选择方案 A 而非 B，决策阈值如何用 torch.profiler 测量？
3. 如果相邻层版本或负载改变，当前权衡是否反转，如何检测？

#### 8. 易错点与反例
1. 反例：只优化本层指标，忽略 异步未同步导致计时偏短，得出虚假加速 被转移到相邻层。
2. 反例：用不同量纲比较方案，忽略 把 profiler 自身开销计入结论。
3. 反例：忽略负载相关性，在低负载下选择的方案遇到 只测单次忽略冷启动与首次调优 失效。

#### 9. 实践与验证
验证方案：对 matmul、逐元素、拷贝分别用 CUDA event 与 profiler 计时，warmup=10、repeat=100、取中位数；用 bytes/time 反推有效带宽并与理论比较。 分别测量两种分层下的 p50/p99 延迟 与 有效带宽 GB/s，做帕累托对比；oracle：event 计时与 profiler 的 CUDA time 偏差 < 5%，大 size 下有效带宽落在理论峰值的可解释比例内。

#### 10. 评分标准
E055 评分：
- 不合格：只堆砌方案，不指出代价转移；
- 合格：能说明一个跨层权衡并给出量纲（算术强度 I=FLOPs/Bytes，roofline 上界 = min(峰值算力 P, I×峰值带宽 BW)，据此判定 compute-bound 或 memory-bound）；
- 良好：能给出决策边界、帕累托证据与反例；
- 优秀/专家：能补充负载相关性、版本演进下的权衡反转与残余风险。
### E056. 跨层权衡：导出部署

- 难度：专家级
- 知识域：K10 导出部署
- 考察能力：输入矩阵、oracle、故障注入与指标
- 题型：测试/实验与验证
- 面试频率：开放题
- 前置知识：state_dict、Export、runtime、schema、迁移
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

跨层权衡要求把 图导出、图签名与跨版本加载 放回 PyTorch 的技术栈中，考察它与相邻层的耦合：算子层、编译层、通信层与存储层。关键抉择通常不是对错而是代价转移，例如 torch.export/ExportedProgram 与 torch.jit.trace/script 的取舍。

#### 2. 问题分析
1. 明确本层职责与不负责的相邻职责，列出候选方案。
2. 用统一量纲比较成本：导出图是函数式的：无 Python 副作用，输入输出由 graph_signature 声明；torch.export 的功能化把原地写改写为显式输出。ONNX 的算子覆盖度由 opset 版本决定，超出该 opset 的算子导出即失败。
3. 找出被转移的代价（显存换时间、精度换吞吐、通信换计算）。
4. 用 导出成功率 与 图算子数 划定决策边界，并给出反例。

#### 3. 具体答案
面试中可以这样回答：

> 导出把可变的 Python 模块冻结成只含 aten 算子的静态图，并用水印式签名声明输入输出契约；正确性等于导出图在目标 runtime 与 eager 数值一致，兼容性等于签名与 opset 能跨版本被消费。
>
> 权衡量纲：导出图是函数式的：无 Python 副作用，输入输出由 graph_signature 声明；torch.export 的功能化把原地写改写为显式输出。ONNX 的算子覆盖度由 opset 版本决定，超出该 opset 的算子导出即失败。
>
> 可验证事实：torch.export 需要示例输入，返回含 graph_module、range_constraints 与 signature 的 ExportedProgram；torch.jit.trace 只记录执行过的算子，遇控制流会固化当时走的分支。

#### 4. 技术洞察
跨层权衡的核心是代价守恒：节省一处必然在另一处付出，导出图是函数式的：无 Python 副作用，输入输出由 graph_signature 声明；torch.export 的功能化把原地写改写为显式输出。ONNX 的算子覆盖度由 opset 版本决定，超出该 opset 的算子导出即失败。 给出了可比较的量纲；脱离该量纲的比较都是空谈。

#### 5. 图文说明
| 层次 | 图导出、图签名与跨版本加载 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 模型、示例输入、dynamic 声明与目标 opset/runtime | 控制流未覆盖、动态维度缺声明 |
| 执行与状态 | 追踪图捕获、功能化改写、签名与序列化 | 分支固化、opset 缺算子、加载执行任意代码 |
| 输出与 oracle | 对比导出/eager 输出、校验签名与 checker | 数值偏差、改变维度即失效、导出被拒绝 |

图示解释：把 图导出、图签名与跨版本加载 的相邻层画成接口，标出每层承担的 导出成功率、eager 与导出输出误差 成本，观察代价如何转移。

#### 6. 拓展知识
相邻层手段：torch.export/exported.run_decompositions、onnx.checker 与 shape inference；对比不同分层的实现复杂度与故障面，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 本层把成本推给相邻层时，图算子数 会如何变化，是否可接受？
2. 在什么条件下选择方案 A 而非 B，决策阈值如何用 torch.export/exported.run_decompositions 测量？
3. 如果相邻层版本或负载改变，当前权衡是否反转，如何检测？

#### 8. 易错点与反例
1. 反例：只优化本层指标，忽略 trace 未覆盖控制流分支导致逻辑被固化到单一分支 被转移到相邻层。
2. 反例：用不同量纲比较方案，忽略 dynamic shape 未显式声明使其被编译成常量。
3. 反例：忽略负载相关性，在低负载下选择的方案遇到 opset 过低缺失算子导致导出失败 失效。

#### 9. 实践与验证
验证方案：对同一模型分别 torch.export 与 jit.trace，跑固定输入比对输出；改变一个输入维度检验 dynamic_shapes 是否被尊重；用 onnx.checker 校验导出图。 分别测量两种分层下的 导出成功率 与 图算子数，做帕累托对比；oracle：导出输出与 eager 最大差 < 1e-5，声明为 dynamic 的维度改变时无需重导。

#### 10. 评分标准
E056 评分：
- 不合格：只堆砌方案，不指出代价转移；
- 合格：能说明一个跨层权衡并给出量纲（导出图是函数式的：无 Python 副作用，输入输出由 graph_signature 声明）；
- 良好：能给出决策边界、帕累托证据与反例；
- 优秀/专家：能补充负载相关性、版本演进下的权衡反转与残余风险。
### E057. 跨层权衡：自定义算子

- 难度：专家级
- 知识域：K11 自定义算子
- 考察能力：输入矩阵、oracle、故障注入与指标
- 题型：测试/实验与验证
- 面试频率：开放题
- 前置知识：schema、dispatcher、stream、autograd、ABI
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

跨层权衡要求把 算子注册、调度与数值正确性 放回 PyTorch 的技术栈中，考察它与相邻层的耦合：算子层、编译层、通信层与存储层。关键抉择通常不是对错而是代价转移，例如 torch.library/TORCH_LIBRARY 注册 与 autograd.Function 与双反向 的取舍。

#### 2. 问题分析
1. 明确本层职责与不负责的相邻职责，列出候选方案。
2. 用统一量纲比较成本：CUDA kernel 启动取 blocks=⌈N/threads⌉、threads≤1024 且为 32 的倍数；自定义算子若参与训练必须同时提供 forward 与 backward（或注册 autograd 公式），否则梯度断链；访问需满足 0≤idx<N 防越界。
3. 找出被转移的代价（显存换时间、精度换吞吐、通信换计算）。
4. 用 相对误差 与 kernel 耗时 划定决策边界，并给出反例。

#### 3. 具体答案
面试中可以这样回答：

> 自定义算子是 dispatcher 上的一个实现对：前向 kernel 负责数值、反向公式负责梯度；正确性由边界条件、设备守卫与 gradcheck 共同保证。
>
> 权衡量纲：CUDA kernel 启动取 blocks=⌈N/threads⌉、threads≤1024 且为 32 的倍数；自定义算子若参与训练必须同时提供 forward 与 backward（或注册 autograd 公式），否则梯度断链；访问需满足 0≤idx<N 防越界。
>
> 可验证事实：torch.utils.cpp_extension.load/load_inline 首次调用时编译并缓存扩展；缺少 TORCH_CHECK 的越界访问可能不报错而返回垃圾数据或破坏相邻内存。

#### 4. 技术洞察
跨层权衡的核心是代价守恒：节省一处必然在另一处付出，CUDA kernel 启动取 blocks=⌈N/threads⌉、threads≤1024 且为 32 的倍数；自定义算子若参与训练必须同时提供 forward 与 backward（或注册 autograd 公式），否则梯度断链；访问需满足 0≤idx<N 防越界。 给出了可比较的量纲；脱离该量纲的比较都是空谈。

#### 5. 图文说明
| 层次 | 算子注册、调度与数值正确性 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 张量 shape/dtype/device、线程与 grid 配置、边界条件 | 越界、设备不匹配、dtype 未分派 |
| 执行与状态 | 追踪 kernel 启动、CUDAGuard、dispatcher 分派与归约 | 越界写、跑错设备、梯度断链 |
| 输出与 oracle | 对比参考实现、gradcheck 与 sanitizer 报告 | 误差超标、梯度为零、随机错误 |

图示解释：把 算子注册、调度与数值正确性 的相邻层画成接口，标出每层承担的 相对误差、compute-sanitizer 命中数 成本，观察代价如何转移。

#### 6. 拓展知识
相邻层手段：torch.utils.cpp_extension.load/load_inline、torch.autograd.Function 与 gradcheck；对比不同分层的实现复杂度与故障面，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 本层把成本推给相邻层时，kernel 耗时 会如何变化，是否可接受？
2. 在什么条件下选择方案 A 而非 B，决策阈值如何用 torch.utils.cpp_extension.load/load_inline 测量？
3. 如果相邻层版本或负载改变，当前权衡是否反转，如何检测？

#### 8. 易错点与反例
1. 反例：只优化本层指标，忽略 只写 forward 未注册 backward 使梯度静默为零 被转移到相邻层。
2. 反例：用不同量纲比较方案，忽略 kernel 越界写破坏相邻显存造成随机错误。
3. 反例：忽略负载相关性，在低负载下选择的方案遇到 缺少 CUDAGuard 在多卡环境跑错设备 失效。

#### 9. 实践与验证
验证方案：实现逐元素算子并与 eager 参考比较，用 gradcheck 校验反向；把 N 设为非 warp 倍数检验尾块处理，跑 compute-sanitizer。 分别测量两种分层下的 相对误差 与 kernel 耗时，做帕累托对比；oracle：fp32 最大绝对误差 < 1e-6、gradcheck 通过，N=线程数±1 时无越界且结果正确。

#### 10. 评分标准
E057 评分：
- 不合格：只堆砌方案，不指出代价转移；
- 合格：能说明一个跨层权衡并给出量纲（CUDA kernel 启动取 blocks=⌈N/threads⌉、threads≤1024 且为 32 的倍数）；
- 良好：能给出决策边界、帕累托证据与反例；
- 优秀/专家：能补充负载相关性、版本演进下的权衡反转与残余风险。
### E058. 跨层权衡：测试治理

- 难度：专家级
- 知识域：K12 测试治理
- 考察能力：输入矩阵、oracle、故障注入与指标
- 题型：测试/实验与验证
- 面试频率：开放题
- 前置知识：oracle、证据、日志、回滚、门禁、复盘
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

跨层权衡要求把 测试契约、容差与证据化调试 放回 PyTorch 的技术栈中，考察它与相邻层的耦合：算子层、编译层、通信层与存储层。关键抉择通常不是对错而是代价转移，例如 pytest 参数化与 fixture 与 assert_close 的 rtol/atol 的取舍。

#### 2. 问题分析
1. 明确本层职责与不负责的相邻职责，列出候选方案。
2. 用统一量纲比较成本：torch.testing.assert_close 在 |a−b| ≤ atol + rtol·|b| 时通过，float32 默认 rtol≈1.3e-6、atol≈1e-5；flaky 率 = 失败重跑数/总重跑数，需按阈值门禁。
3. 找出被转移的代价（显存换时间、精度换吞吐、通信换计算）。
4. 用 通过率 与 flaky 率 划定决策边界，并给出反例。

#### 3. 具体答案
面试中可以这样回答：

> 测试是把需求写成可执行的契约；有效的门禁需要确定性、隔离、明确容差和可定位失败的证据，而不是只断言最终数值。
>
> 权衡量纲：torch.testing.assert_close 在 |a−b| ≤ atol + rtol·|b| 时通过，float32 默认 rtol≈1.3e-6、atol≈1e-5；flaky 率 = 失败重跑数/总重跑数，需按阈值门禁。
>
> 可验证事实：pytest -k 按名筛选、-x 首次失败即停、--reruns 需 pytest-rerunfailures 插件；未设种子的随机测试在 CI 中会产生不可复现失败。

#### 4. 技术洞察
跨层权衡的核心是代价守恒：节省一处必然在另一处付出，torch.testing.assert_close 在 |a−b| ≤ atol + rtol·|b| 时通过，float32 默认 rtol≈1.3e-6、atol≈1e-5；flaky 率 = 失败重跑数/总重跑数，需按阈值门禁。 给出了可比较的量纲；脱离该量纲的比较都是空谈。

#### 5. 图文说明
| 层次 | 测试契约、容差与证据化调试 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 被测模块、参数组合、种子、容差与基线 | 容差过宽、随机未固定、基线过期 |
| 执行与状态 | 追踪 fixture 生命周期、全局状态与断言点 | 顺序相关、静默 skip、只测终值 |
| 输出与 oracle | 核对断言、失败可定位性与 flaky 统计 | 假绿、不可复现失败、回归漏检 |

图示解释：把 测试契约、容差与证据化调试 的相邻层画成接口，标出每层承担的 通过率、覆盖率 成本，观察代价如何转移。

#### 6. 拓展知识
相邻层手段：pytest/parametrize/fixture、CI 报告与覆盖率；对比不同分层的实现复杂度与故障面，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 本层把成本推给相邻层时，flaky 率 会如何变化，是否可接受？
2. 在什么条件下选择方案 A 而非 B，决策阈值如何用 pytest/parametrize/fixture 测量？
3. 如果相邻层版本或负载改变，当前权衡是否反转，如何检测？

#### 8. 易错点与反例
1. 反例：只优化本层指标，忽略 容差过宽掩盖真实数值回归 被转移到相邻层。
2. 反例：用不同量纲比较方案，忽略 测试间共享全局状态导致顺序相关失败。
3. 反例：忽略负载相关性，在低负载下选择的方案遇到 无 GPU 时静默 skip 使 CI 假绿 失效。

#### 9. 实践与验证
验证方案：为某模块写参数化测试（多 shape/dtype），固定种子并用加严容差的 assert_close；注入一个已知 bug 验证测试能失败。 分别测量两种分层下的 通过率 与 flaky 率，做帕累托对比；oracle：测试在正确实现下稳定通过、在注入 bug 下必然失败，多次运行 flaky 率为 0。

#### 10. 评分标准
E058 评分：
- 不合格：只堆砌方案，不指出代价转移；
- 合格：能说明一个跨层权衡并给出量纲（torch.testing.assert_close 在 |a−b| ≤ atol + rtol·|b| 时通过，float32 默认 rtol≈1.3e-6、atol≈1e-5）；
- 良好：能给出决策边界、帕累托证据与反例；
- 优秀/专家：能补充负载相关性、版本演进下的权衡反转与残余风险。
### E059. 证据审计：张量/设备

- 难度：专家级
- 知识域：K01 张量/设备
- 考察能力：输入矩阵、oracle、故障注入与指标
- 题型：测试/实验与验证
- 面试频率：开放题
- 前置知识：shape、stride、dtype、device
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

证据审计要求对 张量布局、视图语义与设备放置 的每个结论追问证据来源与可复现性：是框架返回、日志推断还是实测。审计对象包括 stride 与 storage_offset 构成的地址映射、view/reshape/contiguous 的复制边界 等断言，以及 有效带宽 GB/s 的采集方式。

#### 2. 问题分析
1. 把结论拆成可判定子命题，标注每条的证据等级（源码/文档/实测/推断）。
2. 定位可证伪点：对连续张量有 stride[i]=∏_{j>i} shape[j]；view 只在目标 shape 能由既有 stride 整除分解时成立，否则 reshape 触发复制；实数占用满足 numel·itemsize ≤ storage.nbytes − storage_offset·itemsize。
3. 检查证据链完整性，例如 torch.Tensor.transpose 返回共享存储的视图（只互换 stride，不影响 storage），而 .reshape 在不可视图化时返回副本，可用 data_ptr() 是否相同判定。
4. 设计能推翻结论的最小实验，记录 有效带宽 GB/s、峰值显存 GiB 与错误类别。

#### 3. 具体答案
面试中可以这样回答：

> 张量的本质是 (storage, size, stride, storage_offset) 四元组对一段连续内存的解释；视图与副本的区别决定是否共享存储、是否可安全原地写。设备放置是显式拓扑决策，.to() 按需搬运并可能触发隐式同步。
>
> 可证伪判据：对连续张量有 stride[i]=∏_{j>i} shape[j]；view 只在目标 shape 能由既有 stride 整除分解时成立，否则 reshape 触发复制；实数占用满足 numel·itemsize ≤ storage.nbytes − storage_offset·itemsize。
>
> 可验证事实：torch.Tensor.transpose 返回共享存储的视图（只互换 stride，不影响 storage），而 .reshape 在不可视图化时返回副本，可用 data_ptr() 是否相同判定。

#### 4. 技术洞察
审计的依据是可证伪性：对连续张量有 stride[i]=∏_{j>i} shape[j]；view 只在目标 shape 能由既有 stride 整除分解时成立，否则 reshape 触发复制；实数占用满足 numel·itemsize ≤ storage.nbytes − storage_offset·itemsize。 只有在给定输入、版本与测量协议下才成立；脱离协议的数字是传闻而非证据。

#### 5. 图文说明
| 层次 | 张量布局、视图语义与设备放置 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 张量、目标 shape/stride、device/dtype 与 memory_format | 约束缺失会误判视图可行性或越界读 |
| 执行与状态 | 追踪 storage、stride、offset 与 allocator 块的 owner 和生命周期 | 共享存储被原地写、碎片泄漏或隐式同步 |
| 输出与 oracle | 对照 data_ptr、is_contiguous 与显存曲线 | 把副本误当视图、峰值显存与理论值严重偏离 |

图示解释：把 张量布局、视图语义与设备放置 的结论、证据来源与采集工具连成链，任何断链处都必须标注 [待验证]。

#### 6. 拓展知识
增强证据的工具：tensor.stride()/is_contiguous()/storage_offset()/data_ptr()、PYTORCH_CUDA_ALLOC_CONF 环境变量；比较不同证据等级的可信度与成本，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 哪条证据足以把 张量布局、视图语义与设备放置 的结论从推断升级为已确认，缺少它会怎样？
2. 如何独立复现该证据，需要固定哪些版本、seed 与测量协议？
3. 如果证据与结论冲突，优先怀疑测量、代码还是环境，如何用 tensor.stride()/is_contiguous()/storage_offset()/data_ptr() 二分？

#### 8. 易错点与反例
1. 反例：把模型/框架返回成功当成正确证据，忽略 在未同步的跨界 .to() 后立即读数据得到脏值。
2. 反例：用一次测量代替协议，忽略 pin_memory 张量长期驻留耗尽锁页内存。
3. 反例：把 [推断] 写成 [已确认]，直到 对非连续张量调用 view 抛 RuntimeError: view size is not compatible with input tensor's size and stride 才暴露证据不足。

#### 9. 实践与验证
验证方案：对同一逻辑张量构造 contiguous、转置、expand 三种布局，断言 is_contiguous/stride/storage_offset，并用 data_ptr 判定 reshape 是否复制。 全程用 tensor.stride()/is_contiguous()/storage_offset()/data_ptr() 与 PYTORCH_CUDA_ALLOC_CONF 环境变量 留存原始 trace 与版本信息，形成可复现证据包；oracle：data_ptr 相同判为视图、不同判为副本；峰值显存与理论 numel·itemsize 的偏差落在分配器块对齐范围内。

#### 10. 评分标准
E059 评分：
- 不合格：只给结论，不区分证据等级；
- 合格：能标注证据来源并给出一个可证伪点；
- 良好：能构建可复现证据链并设计推翻性实验；
- 优秀/专家：能给出证据完整性的量化门槛、版本绑定与残余不确定性。
### E060. 证据审计：autograd

- 难度：专家级
- 知识域：K02 autograd
- 考察能力：输入矩阵、oracle、故障注入与指标
- 题型：测试/实验与验证
- 面试频率：开放题
- 前置知识：requires_grad、leaf、view、detach、反向
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

证据审计要求对 动态计算图、梯度累积与高阶微分 的每个结论追问证据来源与可复现性：是框架返回、日志推断还是实测。审计对象包括 动态图与 grad_fn 链、叶张量与 .grad 累加 等断言，以及 梯度相对误差 的采集方式。

#### 2. 问题分析
1. 把结论拆成可判定子命题，标注每条的证据等级（源码/文档/实测/推断）。
2. 定位可证伪点：反向传播沿 grad_fn 链执行链式法则 g_i=Σ_j g_j·(∂f_j/∂x_i)，梯度以累加方式写入 .grad（不清零则叠加）；对需要梯度的张量做 in-place 会使其 version counter 加一，若该版本被 backward 引用则抛 RuntimeError: a variable needed for gradient computation has been modified by an inplace operation。
3. 检查证据链完整性，例如 torch.autograd.gradcheck 在 float64 下用有限差分（默认 eps=1e-6、rtol=1.3e-6、atol=1e-5）校验解析梯度，要求输入 requires_grad=True 且函数确定性。
4. 设计能推翻结论的最小实验，记录 梯度相对误差、峰值激活内存 MiB 与错误类别。

#### 3. 具体答案
面试中可以这样回答：

> autograd 记录前向操作序列构成的动态图，反向时按逆序调用 grad_fn，用链式法则把上游梯度乘上局部雅可比后累加到叶张量。图的生命周期由 retain_graph 控制，原地写通过 version counter 保证数值一致。
>
> 可证伪判据：反向传播沿 grad_fn 链执行链式法则 g_i=Σ_j g_j·(∂f_j/∂x_i)，梯度以累加方式写入 .grad（不清零则叠加）；对需要梯度的张量做 in-place 会使其 version counter 加一，若该版本被 backward 引用则抛 RuntimeError: a variable needed for gradient computation has been modified by an inplace operation。
>
> 可验证事实：torch.autograd.gradcheck 在 float64 下用有限差分（默认 eps=1e-6、rtol=1.3e-6、atol=1e-5）校验解析梯度，要求输入 requires_grad=True 且函数确定性。

#### 4. 技术洞察
审计的依据是可证伪性：反向传播沿 grad_fn 链执行链式法则 g_i=Σ_j g_j·(∂f_j/∂x_i)，梯度以累加方式写入 .grad（不清零则叠加）；对需要梯度的张量做 in-place 会使其 version counter 加一，若该版本被 backward 引用则抛 RuntimeError: a variable needed for gradient computation has been modified by an inplace operation。 只有在给定输入、版本与测量协议下才成立；脱离协议的数字是传闻而非证据。

#### 5. 图文说明
| 层次 | 动态计算图、梯度累积与高阶微分 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | requires_grad 状态、叶张量标志、是否原地写 | 误把非叶张量当叶、图已释放 |
| 执行与状态 | 遍历 grad_fn 链、version counter、saved tensor 所有权 | 图重复释放、原地写破坏反向、梯度断链 |
| 输出与 oracle | 比对解析梯度与数值差分、.grad 累加结果 | 梯度为零/None、误差超容差、二阶导缺失 |

图示解释：把 动态计算图、梯度累积与高阶微分 的结论、证据来源与采集工具连成链，任何断链处都必须标注 [待验证]。

#### 6. 拓展知识
增强证据的工具：tensor.grad_fn 与 next_functions 遍历、torch.autograd.gradcheck/gradgradcheck；比较不同证据等级的可信度与成本，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 哪条证据足以把 动态计算图、梯度累积与高阶微分 的结论从推断升级为已确认，缺少它会怎样？
2. 如何独立复现该证据，需要固定哪些版本、seed 与测量协议？
3. 如果证据与结论冲突，优先怀疑测量、代码还是环境，如何用 tensor.grad_fn 与 next_functions 遍历 二分？

#### 8. 易错点与反例
1. 反例：把模型/框架返回成功当成正确证据，忽略 第二次 backward 未设 retain_graph=True 触发 Trying to backward through the graph a second time。
2. 反例：用一次测量代替协议，忽略 对参与反向的张量原地写触发 version counter 报错。
3. 反例：把 [推断] 写成 [已确认]，直到 detach 后仍期望梯度回传导致叶张量 .grad 为 None 才暴露证据不足。

#### 9. 实践与验证
验证方案：构造 y=x·sin(x)，用 torch.autograd.grad 求一阶导并与有限差分对比；再开 create_graph=True 求二阶导，验证与解析式 cos(x)−x·sin(x) 一致。 全程用 tensor.grad_fn 与 next_functions 遍历 与 torch.autograd.gradcheck/gradgradcheck 留存原始 trace 与版本信息，形成可复现证据包；oracle：gradcheck 返回 True（误差在 rtol=1e-3/atol=1e-5 内），二阶导相对误差 < 1e-4。

#### 10. 评分标准
E060 评分：
- 不合格：只给结论，不区分证据等级；
- 合格：能标注证据来源并给出一个可证伪点；
- 良好：能构建可复现证据链并设计推翻性实验；
- 优秀/专家：能给出证据完整性的量化门槛、版本绑定与残余不确定性。
### E061. 证据审计：Module/状态

- 难度：专家级
- 知识域：K03 Module/状态
- 考察能力：现象分层、假设树、止损、根因与复发防护
- 题型：故障排查与事故复盘
- 面试频率：开放题
- 前置知识：参数、buffer、模式和 state_dict
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

证据审计要求对 模块树、参数字典与状态持久化 的每个结论追问证据来源与可复现性：是框架返回、日志推断还是实测。审计对象包括 Module 树与命名参数、parameter 与 buffer 的区别 等断言，以及 参数量 的采集方式。

#### 2. 问题分析
1. 把结论拆成可判定子命题，标注每条的证据等级（源码/文档/实测/推断）。
2. 定位可证伪点：state_dict 的键等于模块路径拼接参数/缓冲区名（如 layer1.0.conv.weight）；load_state_dict(strict=True) 要求键集合完全一致，否则报 missing/unexpected keys；buffer 不参与 optimizer 更新但会被 .to() 搬运。
3. 检查证据链完整性，例如 model.eval() 只切换 dropout/BatchNorm 等模块的 training 标志，不关闭 autograd；省显存必须叠加 torch.no_grad() 或 inference_mode()。
4. 设计能推翻结论的最小实验，记录 参数量、键匹配率 与错误类别。

#### 3. 具体答案
面试中可以这样回答：

> nn.Module 是一棵注册树，参数与缓冲区经命名空间形成扁平键空间；state_dict 是这棵树的快照契约，.to() 递归搬运所有注册张量。模式切换与注册完整性共同决定训练/推理一致性。
>
> 可证伪判据：state_dict 的键等于模块路径拼接参数/缓冲区名（如 layer1.0.conv.weight）；load_state_dict(strict=True) 要求键集合完全一致，否则报 missing/unexpected keys；buffer 不参与 optimizer 更新但会被 .to() 搬运。
>
> 可验证事实：model.eval() 只切换 dropout/BatchNorm 等模块的 training 标志，不关闭 autograd；省显存必须叠加 torch.no_grad() 或 inference_mode()。

#### 4. 技术洞察
审计的依据是可证伪性：state_dict 的键等于模块路径拼接参数/缓冲区名（如 layer1.0.conv.weight）；load_state_dict(strict=True) 要求键集合完全一致，否则报 missing/unexpected keys；buffer 不参与 optimizer 更新但会被 .to() 搬运。 只有在给定输入、版本与测量协议下才成立；脱离协议的数字是传闻而非证据。

#### 5. 图文说明
| 层次 | 模块树、参数字典与状态持久化 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 模块结构、注册的参数/缓冲区、目标 device/dtype | 漏注册 buffer 或键空间不一致 |
| 执行与状态 | 遍历模块树、加载/保存快照、切换 training 标志 | 键缺失被静默、模式与图不匹配、别名错乱 |
| 输出与 oracle | 逐张量比对 state_dict、核对模式相关的统计 | 权重随机初始化、BN 统计漂移、显存未释放 |

图示解释：把 模块树、参数字典与状态持久化 的结论、证据来源与采集工具连成链，任何断链处都必须标注 [待验证]。

#### 6. 拓展知识
增强证据的工具：named_parameters()/named_buffers()/recurse、register_buffer/register_parameter；比较不同证据等级的可信度与成本，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 哪条证据足以把 模块树、参数字典与状态持久化 的结论从推断升级为已确认，缺少它会怎样？
2. 如何独立复现该证据，需要固定哪些版本、seed 与测量协议？
3. 如果证据与结论冲突，优先怀疑测量、代码还是环境，如何用 named_parameters()/named_buffers()/recurse 二分？

#### 8. 易错点与反例
1. 反例：把模型/框架返回成功当成正确证据，忽略 共享参数被重复注册改变键空间与加载行为。
2. 反例：用一次测量代替协议，忽略 strict=False 静默吞掉缺失键使部分权重随机初始化。
3. 反例：把 [推断] 写成 [已确认]，直到 buffer 未 register_buffer 导致不随 .to() 和 state_dict 迁移 才暴露证据不足。

#### 9. 实践与验证
验证方案：构造含卷积、BN、自定义 buffer 的模块，round-trip state_dict 到新实例并 assert_close 全部张量；删掉一个 buffer 验证 strict=True 报 missing keys。 全程用 named_parameters()/named_buffers()/recurse 与 register_buffer/register_parameter 留存原始 trace 与版本信息，形成可复现证据包；oracle：load_state_dict 返回 missing=[]、unexpected=[]；train 模式两次 forward 后 BN running_mean 发生变化。

#### 10. 评分标准
E061 评分：
- 不合格：只给结论，不区分证据等级；
- 合格：能标注证据来源并给出一个可证伪点；
- 良好：能构建可复现证据链并设计推翻性实验；
- 优秀/专家：能给出证据完整性的量化门槛、版本绑定与残余不确定性。
### E062. 证据审计：数据管道

- 难度：专家级
- 知识域：K04 数据管道
- 考察能力：现象分层、假设树、止损、根因与复发防护
- 题型：故障排查与事故复盘
- 面试频率：开放题
- 前置知识：Dataset、Sampler、collate、padding、版本
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

证据审计要求对 DataLoader 的并行、采样与预取 的每个结论追问证据来源与可复现性：是框架返回、日志推断还是实测。审计对象包括 map-style 与 iterable-style Dataset、num_workers 与进程启动方式 等断言，以及 吞吐 samples/s 的采集方式。

#### 2. 问题分析
1. 把结论拆成可判定子命题，标注每条的证据等级（源码/文档/实测/推断）。
2. 定位可证伪点：map-style 数据集每 epoch 批数 = ⌈N/batch_size⌉（drop_last 时为 ⌊N/batch_size⌋）；worker 是独立进程并各自持有 RNG，随机变换必须经 worker_init_fn 播种才可复现。
3. 检查证据链完整性，例如 pin_memory=True 配合 non_blocking=True 能把 H2D 拷贝与计算重叠；persistent_workers=True 在 epoch 间保留 worker 进程，避免反复 fork 开销。
4. 设计能推翻结论的最小实验，记录 吞吐 samples/s、首 batch 延迟 与错误类别。

#### 3. 具体答案
面试中可以这样回答：

> 数据管道把采样、取数、批处理和预取解耦到多 worker 进程；正确性取决于索引覆盖与随机性可复现，性能取决于预取深度与 H2D 重叠。
>
> 可证伪判据：map-style 数据集每 epoch 批数 = ⌈N/batch_size⌉（drop_last 时为 ⌊N/batch_size⌋）；worker 是独立进程并各自持有 RNG，随机变换必须经 worker_init_fn 播种才可复现。
>
> 可验证事实：pin_memory=True 配合 non_blocking=True 能把 H2D 拷贝与计算重叠；persistent_workers=True 在 epoch 间保留 worker 进程，避免反复 fork 开销。

#### 4. 技术洞察
审计的依据是可证伪性：map-style 数据集每 epoch 批数 = ⌈N/batch_size⌉（drop_last 时为 ⌊N/batch_size⌋）；worker 是独立进程并各自持有 RNG，随机变换必须经 worker_init_fn 播种才可复现。 只有在给定输入、版本与测量协议下才成立；脱离协议的数字是传闻而非证据。

#### 5. 图文说明
| 层次 | DataLoader 的并行、采样与预取 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 数据集大小、batch_size、worker 数、采样方式 | 索引不覆盖、随机性不可复现 |
| 执行与状态 | 追踪 worker 进程、预取队列、pin 缓冲与 sampler 状态 | CUDA-in-fork 崩溃、句柄泄漏、顺序错乱 |
| 输出与 oracle | 核对样本集合、shape、吞吐与首 batch 延迟 | 重复/丢失样本、collate 形状错误、吞吐饱和异常 |

图示解释：把 DataLoader 的并行、采样与预取 的结论、证据来源与采集工具连成链，任何断链处都必须标注 [待验证]。

#### 6. 拓展知识
增强证据的工具：DataLoader/worker_init_fn/generator、IterableDataset 与分片；比较不同证据等级的可信度与成本，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 哪条证据足以把 DataLoader 的并行、采样与预取 的结论从推断升级为已确认，缺少它会怎样？
2. 如何独立复现该证据，需要固定哪些版本、seed 与测量协议？
3. 如果证据与结论冲突，优先怀疑测量、代码还是环境，如何用 DataLoader/worker_init_fn/generator 二分？

#### 8. 易错点与反例
1. 反例：把模型/框架返回成功当成正确证据，忽略 worker 数过大引发内存或文件句柄耗尽。
2. 反例：用一次测量代替协议，忽略 collate_fn 对不定长序列堆叠得到错误 shape。
3. 反例：把 [推断] 写成 [已确认]，直到 在 Dataset.__init__ 或 __getitem__ 中初始化 CUDA 导致 fork 后 worker 崩溃 才暴露证据不足。

#### 9. 实践与验证
验证方案：固定 seed 与 worker_init_fn，连续两 epoch 记录样本索引序列验证覆盖完整且可复现；测量 num_workers∈{0,2,4,8} 的吞吐拐点。 全程用 DataLoader/worker_init_fn/generator 与 IterableDataset 与分片 留存原始 trace 与版本信息，形成可复现证据包；oracle：两轮索引并集等于 0..N−1 且无重复，吞吐在某一 worker 数后饱和。

#### 10. 评分标准
E062 评分：
- 不合格：只给结论，不区分证据等级；
- 合格：能标注证据来源并给出一个可证伪点；
- 良好：能构建可复现证据链并设计推翻性实验；
- 优秀/专家：能给出证据完整性的量化门槛、版本绑定与残余不确定性。
### E063. 证据审计：训练/优化器

- 难度：专家级
- 知识域：K05 训练/优化器
- 考察能力：现象分层、假设树、止损、根因与复发防护
- 题型：故障排查与事故复盘
- 面试频率：开放题
- 前置知识：forward、backward、累积、optimizer、scheduler
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

证据审计要求对 优化器更新、学习率调度与梯度处理 的每个结论追问证据来源与可复现性：是框架返回、日志推断还是实测。审计对象包括 优化器状态（Adam 的 m/v）、zero_grad 语义 等断言，以及 loss 曲线 的采集方式。

#### 2. 问题分析
1. 把结论拆成可判定子命题，标注每条的证据等级（源码/文档/实测/推断）。
2. 定位可证伪点：Adam：m_t=β1·m_{t−1}+(1−β1)·g_t，v_t=β2·v_{t−1}+(1−β2)·g_t²，偏差校正 m̂=m/(1−β1^t)、v̂=v/(1−β2^t)，θ←θ−lr·m̂/(√v̂+ε)；梯度累积 k 步等价于把有效批放大 k 倍，但必须对 loss 除以 k 或按 k 归约，否则梯度被放大 k 倍。
3. 检查证据链完整性，例如 zero_grad(set_to_none=True) 把 .grad 置 None（自 PyTorch 2.0 起为默认行为），比置零更省内存；optimizer.state_dict() 按参数索引保存 momentum/m/v 缓冲区。
4. 设计能推翻结论的最小实验，记录 loss 曲线、学习率序列 与错误类别。

#### 3. 具体答案
面试中可以这样回答：

> 训练步是 zero_grad→forward→loss→scaled backward→unscale→clip→step→scheduler 的固定顺序；顺序错位会静默改变有效学习率或裁剪幅度。
>
> 可证伪判据：Adam：m_t=β1·m_{t−1}+(1−β1)·g_t，v_t=β2·v_{t−1}+(1−β2)·g_t²，偏差校正 m̂=m/(1−β1^t)、v̂=v/(1−β2^t)，θ←θ−lr·m̂/(√v̂+ε)；梯度累积 k 步等价于把有效批放大 k 倍，但必须对 loss 除以 k 或按 k 归约，否则梯度被放大 k 倍。
>
> 可验证事实：zero_grad(set_to_none=True) 把 .grad 置 None（自 PyTorch 2.0 起为默认行为），比置零更省内存；optimizer.state_dict() 按参数索引保存 momentum/m/v 缓冲区。

#### 4. 技术洞察
审计的依据是可证伪性：Adam：m_t=β1·m_{t−1}+(1−β1)·g_t，v_t=β2·v_{t−1}+(1−β2)·g_t²，偏差校正 m̂=m/(1−β1^t)、v̂=v/(1−β2^t)，θ←θ−lr·m̂/(√v̂+ε)；梯度累积 k 步等价于把有效批放大 k 倍，但必须对 loss 除以 k 或按 k 归约，否则梯度被放大 k 倍。 只有在给定输入、版本与测量协议下才成立；脱离协议的数字是传闻而非证据。

#### 5. 图文说明
| 层次 | 优化器更新、学习率调度与梯度处理 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 初始参数、优化器配置、损失缩放与累积步 k | 顺序错位、未缩放 loss |
| 执行与状态 | 追踪 .grad、优化器状态缓冲区、lr 调度历史 | 梯度残留、动量错乱、裁剪幅度被缩放 |
| 输出与 oracle | 核对 loss 单调性、参数更新与 lr 序列 | loss 震荡、等效 lr 漂移、累积实现不等价 |

图示解释：把 优化器更新、学习率调度与梯度处理 的结论、证据来源与采集工具连成链，任何断链处都必须标注 [待验证]。

#### 6. 拓展知识
增强证据的工具：optimizer.state_dict()/param_groups、clip_grad_norm_ 返回值；比较不同证据等级的可信度与成本，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 哪条证据足以把 优化器更新、学习率调度与梯度处理 的结论从推断升级为已确认，缺少它会怎样？
2. 如何独立复现该证据，需要固定哪些版本、seed 与测量协议？
3. 如果证据与结论冲突，优先怀疑测量、代码还是环境，如何用 optimizer.state_dict()/param_groups 二分？

#### 8. 易错点与反例
1. 反例：把模型/框架返回成功当成正确证据，忽略 scheduler.step 放错位置（每 batch 而非每 epoch）改变 lr 曲线。
2. 反例：用一次测量代替协议，忽略 clip_grad_norm_ 在 GradScaler.unscale_ 之前调用使裁剪阈值被放大 S 倍。
3. 反例：把 [推断] 写成 [已确认]，直到 忘记 zero_grad 使梯度跨 step 累加，等效 lr 漂移 才暴露证据不足。

#### 9. 实践与验证
验证方案：在单个 batch 上跑 200 步验证 loss 单调下降到过拟合；打印 get_last_lr 序列核对调度曲线；对同一初始点比较累积实现与放大批实现的参数更新。 全程用 optimizer.state_dict()/param_groups 与 clip_grad_norm_ 返回值 留存原始 trace 与版本信息，形成可复现证据包；oracle：单批过拟合 loss < 0.01；累积与大批实现的参数差在 1e-6 内。

#### 10. 评分标准
E063 评分：
- 不合格：只给结论，不区分证据等级；
- 合格：能标注证据来源并给出一个可证伪点；
- 良好：能构建可复现证据链并设计推翻性实验；
- 优秀/专家：能给出证据完整性的量化门槛、版本绑定与残余不确定性。
### E064. 证据审计：AMP/恢复

- 难度：专家级
- 知识域：K06 AMP/恢复
- 考察能力：现象分层、假设树、止损、根因与复发防护
- 题型：故障排查与事故复盘
- 面试频率：开放题
- 前置知识：autocast、scaler、快照、随机状态
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

证据审计要求对 混合精度、检查点与确定性 的每个结论追问证据来源与可复现性：是框架返回、日志推断还是实测。审计对象包括 autocast 的算子白名单、GradScaler 动态缩放 等断言，以及 scale factor 曲线 的采集方式。

#### 2. 问题分析
1. 把结论拆成可判定子命题，标注每条的证据等级（源码/文档/实测/推断）。
2. 定位可证伪点：GradScaler 先用 scale S 放大 loss 再反向，unscale_ 后若检测到 inf/nan 则跳过该 step 并把 S 减半；fp16 最正规数约 6.1e-5、次正规至约 6e-8，小于此量级的梯度下溢，故需缩放；bf16 指数位与 fp32 同为 8 位因此不需要缩放。
3. 检查证据链完整性，例如 torch.use_deterministic_algorithms(True) 会让部分算子回退到确定性实现或直接报错；cudnn.benchmark=True 的自动调优会破坏逐步可复现性。完整恢复需保存 model、optimizer、scaler、epoch、CPU 与 CUDA RNG 及 sampler 状态。
4. 设计能推翻结论的最小实验，记录 scale factor 曲线、resume 前后参数最大差 与错误类别。

#### 3. 具体答案
面试中可以这样回答：

> AMP 通过 autocast 选低精度算子、GradScaler 维持梯度动态范围；恢复的正确性是状态闭包问题——模型、优化器、缩放与随机数状态缺一不可。
>
> 可证伪判据：GradScaler 先用 scale S 放大 loss 再反向，unscale_ 后若检测到 inf/nan 则跳过该 step 并把 S 减半；fp16 最正规数约 6.1e-5、次正规至约 6e-8，小于此量级的梯度下溢，故需缩放；bf16 指数位与 fp32 同为 8 位因此不需要缩放。
>
> 可验证事实：torch.use_deterministic_algorithms(True) 会让部分算子回退到确定性实现或直接报错；cudnn.benchmark=True 的自动调优会破坏逐步可复现性。完整恢复需保存 model、optimizer、scaler、epoch、CPU 与 CUDA RNG 及 sampler 状态。

#### 4. 技术洞察
审计的依据是可证伪性：GradScaler 先用 scale S 放大 loss 再反向，unscale_ 后若检测到 inf/nan 则跳过该 step 并把 S 减半；fp16 最正规数约 6.1e-5、次正规至约 6e-8，小于此量级的梯度下溢，故需缩放；bf16 指数位与 fp32 同为 8 位因此不需要缩放。 只有在给定输入、版本与测量协议下才成立；脱离协议的数字是传闻而非证据。

#### 5. 图文说明
| 层次 | 混合精度、检查点与确定性 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | dtype、autocast 区域、scaler 配置、checkpoint 内容 | 状态不闭合、精度选择错误 |
| 执行与状态 | 追踪 scale factor、RNG、优化器状态与 nan/inf 分支 | 下溢、NaN、恢复后状态漂移 |
| 输出与 oracle | 比对续训与基线参数、scale 曲线、依赖确定性开关 | 曲线跳变、结果不可复现、梯度全零 |

图示解释：把 混合精度、检查点与确定性 的结论、证据来源与采集工具连成链，任何断链处都必须标注 [待验证]。

#### 6. 拓展知识
增强证据的工具：torch.cuda.amp.GradScaler 与 autocast、torch.use_deterministic_algorithms；比较不同证据等级的可信度与成本，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 哪条证据足以把 混合精度、检查点与确定性 的结论从推断升级为已确认，缺少它会怎样？
2. 如何独立复现该证据，需要固定哪些版本、seed 与测量协议？
3. 如果证据与结论冲突，优先怀疑测量、代码还是环境，如何用 torch.cuda.amp.GradScaler 与 autocast 二分？

#### 8. 易错点与反例
1. 反例：把模型/框架返回成功当成正确证据，忽略 resume 只存模型权重丢失优化器动量使曲线跳变。
2. 反例：用一次测量代替协议，忽略 只存 torch RNG 未存 numpy/random 状态导致数据顺序不同。
3. 反例：把 [推断] 写成 [已确认]，直到 fp16 无 scaler 导致梯度下溢为 0 或 NaN 才暴露证据不足。

#### 9. 实践与验证
验证方案：训练 50 步保存完整 checkpoint，重启加载后再跑 50 步与不中断基线逐参数比较；fp16 下注入大梯度观测 S 是否减半。 全程用 torch.cuda.amp.GradScaler 与 autocast 与 torch.use_deterministic_algorithms 留存原始 trace 与版本信息，形成可复现证据包；oracle：续训与基线参数最大差 < 1e-5（bit 级取决于确定性开关），scale factor 与跳过计数一致。

#### 10. 评分标准
E064 评分：
- 不合格：只给结论，不区分证据等级；
- 合格：能标注证据来源并给出一个可证伪点；
- 良好：能构建可复现证据链并设计推翻性实验；
- 优秀/专家：能给出证据完整性的量化门槛、版本绑定与残余不确定性。
### E065. 证据审计：分布式

- 难度：专家级
- 知识域：K07 分布式
- 考察能力：现象分层、假设树、止损、根因与复发防护
- 题型：故障排查与事故复盘
- 面试频率：开放题
- 前置知识：rank、DDP、FSDP、collective、容错
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

证据审计要求对 进程组、集合通信与梯度同步 的每个结论追问证据来源与可复现性：是框架返回、日志推断还是实测。审计对象包括 进程组与 rank/world_size、all_reduce/reduce_scatter/all_gather 等断言，以及 all_reduce 带宽 GB/s 的采集方式。

#### 2. 问题分析
1. 把结论拆成可判定子命题，标注每条的证据等级（源码/文档/实测/推断）。
2. 定位可证伪点：DDP 默认 all_reduce(sum) 后除以 world_size 得平均梯度，故全局有效批 = 本地批 × world_size × 累积步数；ring all_reduce 单步通信量 ≈ 2·(N−1)/N·S（S 为梯度字节数），大 N 时趋近 2S。
3. 检查证据链完整性，例如 NCCL 只支持 CUDA 张量、gloo 支持 CPU；DDP 默认把梯度分成约 25 MiB 的桶以便与反向计算重叠通信。
4. 设计能推翻结论的最小实验，记录 all_reduce 带宽 GB/s、step 时间方差 与错误类别。

#### 3. 具体答案
面试中可以这样回答：

> 分布式训练把单机步拆成计算→梯度归约→更新三段；正确性在于所有 rank 以相同顺序进入集合通信且梯度按 world_size 归一，性能在于把通信藏进反向的尾部。
>
> 可证伪判据：DDP 默认 all_reduce(sum) 后除以 world_size 得平均梯度，故全局有效批 = 本地批 × world_size × 累积步数；ring all_reduce 单步通信量 ≈ 2·(N−1)/N·S（S 为梯度字节数），大 N 时趋近 2S。
>
> 可验证事实：NCCL 只支持 CUDA 张量、gloo 支持 CPU；DDP 默认把梯度分成约 25 MiB 的桶以便与反向计算重叠通信。

#### 4. 技术洞察
审计的依据是可证伪性：DDP 默认 all_reduce(sum) 后除以 world_size 得平均梯度，故全局有效批 = 本地批 × world_size × 累积步数；ring all_reduce 单步通信量 ≈ 2·(N−1)/N·S（S 为梯度字节数），大 N 时趋近 2S。 只有在给定输入、版本与测量协议下才成立；脱离协议的数字是传闻而非证据。

#### 5. 图文说明
| 层次 | 进程组、集合通信与梯度同步 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 进程数、后端、张量是否 CUDA、参与反向的参数集合 | 后端不匹配、参数未参与、集合调用不一致 |
| 执行与状态 | 追踪 rank、bucket、通信算子与反向重叠 | hang、梯度未归约、通信未重叠 |
| 输出与 oracle | 对比单卡/DDP 梯度、通信带宽与 step 方差 | 梯度差超容差、超时、吞吐不随卡数提升 |

图示解释：把 进程组、集合通信与梯度同步 的结论、证据来源与采集工具连成链，任何断链处都必须标注 [待验证]。

#### 6. 拓展知识
增强证据的工具：torch.distributed.init_process_group/barrier、NCCL_DEBUG=INFO 与 TORCH_DISTRIBUTED_DEBUG；比较不同证据等级的可信度与成本，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 哪条证据足以把 进程组、集合通信与梯度同步 的结论从推断升级为已确认，缺少它会怎样？
2. 如何独立复现该证据，需要固定哪些版本、seed 与测量协议？
3. 如果证据与结论冲突，优先怀疑测量、代码还是环境，如何用 torch.distributed.init_process_group/barrier 二分？

#### 8. 易错点与反例
1. 反例：把模型/框架返回成功当成正确证据，忽略 未按 world_size 归一梯度使等效 lr 随卡数变化。
2. 反例：用一次测量代替协议，忽略 NCCL/驱动版本不一致引发通信超时或崩溃。
3. 反例：把 [推断] 写成 [已确认]，直到 各 rank 集合通信调用不匹配导致 hang 直到 watchdog 超时 才暴露证据不足。

#### 9. 实践与验证
验证方案：用 torchrun 起 2 进程，固定初始化使单卡与 DDP 梯度可对齐，比较 all_reduce 前后梯度；人为让一个 rank 提前退出观测 NCCL watchdog 超时。 全程用 torch.distributed.init_process_group/barrier 与 NCCL_DEBUG=INFO 与 TORCH_DISTRIBUTED_DEBUG 留存原始 trace 与版本信息，形成可复现证据包；oracle：DDP 与单卡初始梯度（除以 world_size 后）相对误差 < 1e-6；NCCL_DEBUG 显示预期的 ring/tree 算法。

#### 10. 评分标准
E065 评分：
- 不合格：只给结论，不区分证据等级；
- 合格：能标注证据来源并给出一个可证伪点；
- 良好：能构建可复现证据链并设计推翻性实验；
- 优秀/专家：能给出证据完整性的量化门槛、版本绑定与残余不确定性。
### E066. 证据审计：编译栈

- 难度：专家级
- 知识域：K08 编译栈
- 考察能力：现象分层、假设树、止损、根因与复发防护
- 题型：故障排查与事故复盘
- 面试频率：开放题
- 前置知识：Dynamo、AOTAutograd、Inductor、guard、缓存
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

证据审计要求对 Dynamo 捕获、guard 与 Inductor 代码生成 的每个结论追问证据来源与可复现性：是框架返回、日志推断还是实测。审计对象包括 TorchDynamo 的字节码捕获、guard 与 specialization 等断言，以及 重编译次数 的采集方式。

#### 2. 问题分析
1. 把结论拆成可判定子命题，标注每条的证据等级（源码/文档/实测/推断）。
2. 定位可证伪点：一个编译单元的 guard 绑定在张量的 shape/dtype/device 与 Python 常量上；任一 guard 失效即触发重编译，默认累计重编译上限约 8 次（torch._dynamo.config.accumulated_recompile_limit）。图被 graph break 切成多个子图，未捕获区间回退 eager。
3. 检查证据链完整性，例如 设置 TORCH_LOGS=graph_breaks,guards,recompiles 可打印断点与重编译原因；torch._dynamo.explain(fn)(*args) 返回断点数量与原因；dynamic=True 或 torch._dynamo.mark_dynamic 可放宽形状守卫。
4. 设计能推翻结论的最小实验，记录 重编译次数、compiled/eager 加速比 与错误类别。

#### 3. 具体答案
面试中可以这样回答：

> 编译栈把 Python 执行轨迹捕获成 FX 图，用 guard 保证图对后续输入仍有效；失效即重编译、无法捕获即断图。稳定收益来自低断点数与低重编译率而非一次加速。
>
> 可证伪判据：一个编译单元的 guard 绑定在张量的 shape/dtype/device 与 Python 常量上；任一 guard 失效即触发重编译，默认累计重编译上限约 8 次（torch._dynamo.config.accumulated_recompile_limit）。图被 graph break 切成多个子图，未捕获区间回退 eager。
>
> 可验证事实：设置 TORCH_LOGS=graph_breaks,guards,recompiles 可打印断点与重编译原因；torch._dynamo.explain(fn)(*args) 返回断点数量与原因；dynamic=True 或 torch._dynamo.mark_dynamic 可放宽形状守卫。

#### 4. 技术洞察
审计的依据是可证伪性：一个编译单元的 guard 绑定在张量的 shape/dtype/device 与 Python 常量上；任一 guard 失效即触发重编译，默认累计重编译上限约 8 次（torch._dynamo.config.accumulated_recompile_limit）。图被 graph break 切成多个子图，未捕获区间回退 eager。 只有在给定输入、版本与测量协议下才成立；脱离协议的数字是传闻而非证据。

#### 5. 图文说明
| 层次 | Dynamo 捕获、guard 与 Inductor 代码生成 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 函数代码、示例输入、shape/dtype/device 与 dynamic 声明 | 守卫过严、断点隐藏、动态维度未声明 |
| 执行与状态 | 追踪 Dynamo 捕获、guard 集合、子图与 Inductor 缓存 | 重编译风暴、断点回退 eager、缓存未命中 |
| 输出与 oracle | 对比 compiled/eager 数值、重编译与断点计数 | 数值不一致、加速比 < 1、重编译失控 |

图示解释：把 Dynamo 捕获、guard 与 Inductor 代码生成 的结论、证据来源与采集工具连成链，任何断链处都必须标注 [待验证]。

#### 6. 拓展知识
增强证据的工具：TORCH_LOGS=graph_breaks,guards,recompiles、torch.compile(mode=...)/mark_dynamic；比较不同证据等级的可信度与成本，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 哪条证据足以把 Dynamo 捕获、guard 与 Inductor 代码生成 的结论从推断升级为已确认，缺少它会怎样？
2. 如何独立复现该证据，需要固定哪些版本、seed 与测量协议？
3. 如果证据与结论冲突，优先怀疑测量、代码还是环境，如何用 TORCH_LOGS=graph_breaks,guards,recompiles 二分？

#### 8. 易错点与反例
1. 反例：把模型/框架返回成功当成正确证据，忽略 动态 shape 引发重编译风暴使性能倒挂。
2. 反例：用一次测量代替协议，忽略 别名或原地写使编译结果与 eager 数值不一致。
3. 反例：把 [推断] 写成 [已确认]，直到 在图中调用 .item()/print/依赖数据的 Python 分支触发 graph break 才暴露证据不足。

#### 9. 实践与验证
验证方案：对同一函数用固定 shape 与变化 shape 各跑 20 次，用 explain 统计 graph break 与重编译；对比 compiled 与 eager 输出最大差。 全程用 TORCH_LOGS=graph_breaks,guards,recompiles 与 torch.compile(mode=...)/mark_dynamic 留存原始 trace 与版本信息，形成可复现证据包；oracle：固定 shape 下重编译次数为 1、输出与 eager 最大差 < 1e-4；变化 shape 下开启 dynamic 后重编译显著下降。

#### 10. 评分标准
E066 评分：
- 不合格：只给结论，不区分证据等级；
- 合格：能标注证据来源并给出一个可证伪点；
- 良好：能构建可复现证据链并设计推翻性实验；
- 优秀/专家：能给出证据完整性的量化门槛、版本绑定与残余不确定性。
### E067. 证据审计：性能

- 难度：专家级
- 知识域：K09 性能
- 考察能力：现象分层、假设树、止损、根因与复发防护
- 题型：故障排查与事故复盘
- 面试频率：开放题
- 前置知识：关键路径、带宽、算力、通信、显存、统计
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

证据审计要求对 性能剖析、roofline 与显存容量 的每个结论追问证据来源与可复现性：是框架返回、日志推断还是实测。审计对象包括 CUDA event 计时与同步、roofline 与算术强度 等断言，以及 p50/p99 延迟 的采集方式。

#### 2. 问题分析
1. 把结论拆成可判定子命题，标注每条的证据等级（源码/文档/实测/推断）。
2. 定位可证伪点：算术强度 I=FLOPs/Bytes，roofline 上界 = min(峰值算力 P, I×峰值带宽 BW)，据此判定 compute-bound 或 memory-bound；CUDA event 计时为 t=elapsed_time(start,stop)，异步操作必须 torch.cuda.synchronize() 后计时才可信。
3. 检查证据链完整性，例如 torch.profiler.profile(activities=[ProfilerActivity.CUDA]) 会插入 CUDA 事件并输出按算子聚合的 self/total CUDA time；首次迭代含 cuBLAS/context 初始化与自动调优，必须先 warmup 再计时。
4. 设计能推翻结论的最小实验，记录 p50/p99 延迟、MFU/TFLOPS 与错误类别。

#### 3. 具体答案
面试中可以这样回答：

> 性能结论必须绑定可复现的测量协议：warmup、同步、重复次数、分位数；再用 roofline 判定算力受限还是带宽受限，最后才谈优化手段。
>
> 可证伪判据：算术强度 I=FLOPs/Bytes，roofline 上界 = min(峰值算力 P, I×峰值带宽 BW)，据此判定 compute-bound 或 memory-bound；CUDA event 计时为 t=elapsed_time(start,stop)，异步操作必须 torch.cuda.synchronize() 后计时才可信。
>
> 可验证事实：torch.profiler.profile(activities=[ProfilerActivity.CUDA]) 会插入 CUDA 事件并输出按算子聚合的 self/total CUDA time；首次迭代含 cuBLAS/context 初始化与自动调优，必须先 warmup 再计时。

#### 4. 技术洞察
审计的依据是可证伪性：算术强度 I=FLOPs/Bytes，roofline 上界 = min(峰值算力 P, I×峰值带宽 BW)，据此判定 compute-bound 或 memory-bound；CUDA event 计时为 t=elapsed_time(start,stop)，异步操作必须 torch.cuda.synchronize() 后计时才可信。 只有在给定输入、版本与测量协议下才成立；脱离协议的数字是传闻而非证据。

#### 5. 图文说明
| 层次 | 性能剖析、roofline 与显存容量 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 算子/规模、dtype、warmup 与 repeat、计时方式 | 未同步、未 warmup、规模误导 |
| 执行与状态 | 追踪 kernel 时长、显存曲线、launch 与同步点 | 计时偏短、碎片抖动、把 launch 开销当算力 |
| 输出与 oracle | 对比 event/profiler、roofline 定位与分位数 | 加速比不稳定、有效带宽超理论值、尾延迟超标 |

图示解释：把 性能剖析、roofline 与显存容量 的结论、证据来源与采集工具连成链，任何断链处都必须标注 [待验证]。

#### 6. 拓展知识
增强证据的工具：torch.profiler、ncu/nsys；比较不同证据等级的可信度与成本，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 哪条证据足以把 性能剖析、roofline 与显存容量 的结论从推断升级为已确认，缺少它会怎样？
2. 如何独立复现该证据，需要固定哪些版本、seed 与测量协议？
3. 如果证据与结论冲突，优先怀疑测量、代码还是环境，如何用 torch.profiler 二分？

#### 8. 易错点与反例
1. 反例：把模型/框架返回成功当成正确证据，忽略 把 profiler 自身开销计入结论。
2. 反例：用一次测量代替协议，忽略 只测单次忽略冷启动与首次调优。
3. 反例：把 [推断] 写成 [已确认]，直到 异步未同步导致计时偏短，得出虚假加速 才暴露证据不足。

#### 9. 实践与验证
验证方案：对 matmul、逐元素、拷贝分别用 CUDA event 与 profiler 计时，warmup=10、repeat=100、取中位数；用 bytes/time 反推有效带宽并与理论比较。 全程用 torch.profiler 与 ncu/nsys 留存原始 trace 与版本信息，形成可复现证据包；oracle：event 计时与 profiler 的 CUDA time 偏差 < 5%，大 size 下有效带宽落在理论峰值的可解释比例内。

#### 10. 评分标准
E067 评分：
- 不合格：只给结论，不区分证据等级；
- 合格：能标注证据来源并给出一个可证伪点；
- 良好：能构建可复现证据链并设计推翻性实验；
- 优秀/专家：能给出证据完整性的量化门槛、版本绑定与残余不确定性。
### E068. 证据审计：导出部署

- 难度：专家级
- 知识域：K10 导出部署
- 考察能力：现象分层、假设树、止损、根因与复发防护
- 题型：故障排查与事故复盘
- 面试频率：开放题
- 前置知识：state_dict、Export、runtime、schema、迁移
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

证据审计要求对 图导出、图签名与跨版本加载 的每个结论追问证据来源与可复现性：是框架返回、日志推断还是实测。审计对象包括 torch.export/ExportedProgram、torch.jit.trace/script 等断言，以及 导出成功率 的采集方式。

#### 2. 问题分析
1. 把结论拆成可判定子命题，标注每条的证据等级（源码/文档/实测/推断）。
2. 定位可证伪点：导出图是函数式的：无 Python 副作用，输入输出由 graph_signature 声明；torch.export 的功能化把原地写改写为显式输出。ONNX 的算子覆盖度由 opset 版本决定，超出该 opset 的算子导出即失败。
3. 检查证据链完整性，例如 torch.export 需要示例输入，返回含 graph_module、range_constraints 与 signature 的 ExportedProgram；torch.jit.trace 只记录执行过的算子，遇控制流会固化当时走的分支。
4. 设计能推翻结论的最小实验，记录 导出成功率、eager 与导出输出误差 与错误类别。

#### 3. 具体答案
面试中可以这样回答：

> 导出把可变的 Python 模块冻结成只含 aten 算子的静态图，并用水印式签名声明输入输出契约；正确性等于导出图在目标 runtime 与 eager 数值一致，兼容性等于签名与 opset 能跨版本被消费。
>
> 可证伪判据：导出图是函数式的：无 Python 副作用，输入输出由 graph_signature 声明；torch.export 的功能化把原地写改写为显式输出。ONNX 的算子覆盖度由 opset 版本决定，超出该 opset 的算子导出即失败。
>
> 可验证事实：torch.export 需要示例输入，返回含 graph_module、range_constraints 与 signature 的 ExportedProgram；torch.jit.trace 只记录执行过的算子，遇控制流会固化当时走的分支。

#### 4. 技术洞察
审计的依据是可证伪性：导出图是函数式的：无 Python 副作用，输入输出由 graph_signature 声明；torch.export 的功能化把原地写改写为显式输出。ONNX 的算子覆盖度由 opset 版本决定，超出该 opset 的算子导出即失败。 只有在给定输入、版本与测量协议下才成立；脱离协议的数字是传闻而非证据。

#### 5. 图文说明
| 层次 | 图导出、图签名与跨版本加载 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 模型、示例输入、dynamic 声明与目标 opset/runtime | 控制流未覆盖、动态维度缺声明 |
| 执行与状态 | 追踪图捕获、功能化改写、签名与序列化 | 分支固化、opset 缺算子、加载执行任意代码 |
| 输出与 oracle | 对比导出/eager 输出、校验签名与 checker | 数值偏差、改变维度即失效、导出被拒绝 |

图示解释：把 图导出、图签名与跨版本加载 的结论、证据来源与采集工具连成链，任何断链处都必须标注 [待验证]。

#### 6. 拓展知识
增强证据的工具：torch.export/exported.run_decompositions、torch.jit.trace；比较不同证据等级的可信度与成本，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 哪条证据足以把 图导出、图签名与跨版本加载 的结论从推断升级为已确认，缺少它会怎样？
2. 如何独立复现该证据，需要固定哪些版本、seed 与测量协议？
3. 如果证据与结论冲突，优先怀疑测量、代码还是环境，如何用 torch.export/exported.run_decompositions 二分？

#### 8. 易错点与反例
1. 反例：把模型/框架返回成功当成正确证据，忽略 dynamic shape 未显式声明使其被编译成常量。
2. 反例：用一次测量代替协议，忽略 opset 过低缺失算子导致导出失败。
3. 反例：把 [推断] 写成 [已确认]，直到 trace 未覆盖控制流分支导致逻辑被固化到单一分支 才暴露证据不足。

#### 9. 实践与验证
验证方案：对同一模型分别 torch.export 与 jit.trace，跑固定输入比对输出；改变一个输入维度检验 dynamic_shapes 是否被尊重；用 onnx.checker 校验导出图。 全程用 torch.export/exported.run_decompositions 与 torch.jit.trace 留存原始 trace 与版本信息，形成可复现证据包；oracle：导出输出与 eager 最大差 < 1e-5，声明为 dynamic 的维度改变时无需重导。

#### 10. 评分标准
E068 评分：
- 不合格：只给结论，不区分证据等级；
- 合格：能标注证据来源并给出一个可证伪点；
- 良好：能构建可复现证据链并设计推翻性实验；
- 优秀/专家：能给出证据完整性的量化门槛、版本绑定与残余不确定性。
### E069. 证据审计：自定义算子

- 难度：专家级
- 知识域：K11 自定义算子
- 考察能力：现象分层、假设树、止损、根因与复发防护
- 题型：故障排查与事故复盘
- 面试频率：开放题
- 前置知识：schema、dispatcher、stream、autograd、ABI
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

证据审计要求对 算子注册、调度与数值正确性 的每个结论追问证据来源与可复现性：是框架返回、日志推断还是实测。审计对象包括 torch.library/TORCH_LIBRARY 注册、autograd.Function 与双反向 等断言，以及 相对误差 的采集方式。

#### 2. 问题分析
1. 把结论拆成可判定子命题，标注每条的证据等级（源码/文档/实测/推断）。
2. 定位可证伪点：CUDA kernel 启动取 blocks=⌈N/threads⌉、threads≤1024 且为 32 的倍数；自定义算子若参与训练必须同时提供 forward 与 backward（或注册 autograd 公式），否则梯度断链；访问需满足 0≤idx<N 防越界。
3. 检查证据链完整性，例如 torch.utils.cpp_extension.load/load_inline 首次调用时编译并缓存扩展；缺少 TORCH_CHECK 的越界访问可能不报错而返回垃圾数据或破坏相邻内存。
4. 设计能推翻结论的最小实验，记录 相对误差、compute-sanitizer 命中数 与错误类别。

#### 3. 具体答案
面试中可以这样回答：

> 自定义算子是 dispatcher 上的一个实现对：前向 kernel 负责数值、反向公式负责梯度；正确性由边界条件、设备守卫与 gradcheck 共同保证。
>
> 可证伪判据：CUDA kernel 启动取 blocks=⌈N/threads⌉、threads≤1024 且为 32 的倍数；自定义算子若参与训练必须同时提供 forward 与 backward（或注册 autograd 公式），否则梯度断链；访问需满足 0≤idx<N 防越界。
>
> 可验证事实：torch.utils.cpp_extension.load/load_inline 首次调用时编译并缓存扩展；缺少 TORCH_CHECK 的越界访问可能不报错而返回垃圾数据或破坏相邻内存。

#### 4. 技术洞察
审计的依据是可证伪性：CUDA kernel 启动取 blocks=⌈N/threads⌉、threads≤1024 且为 32 的倍数；自定义算子若参与训练必须同时提供 forward 与 backward（或注册 autograd 公式），否则梯度断链；访问需满足 0≤idx<N 防越界。 只有在给定输入、版本与测量协议下才成立；脱离协议的数字是传闻而非证据。

#### 5. 图文说明
| 层次 | 算子注册、调度与数值正确性 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 张量 shape/dtype/device、线程与 grid 配置、边界条件 | 越界、设备不匹配、dtype 未分派 |
| 执行与状态 | 追踪 kernel 启动、CUDAGuard、dispatcher 分派与归约 | 越界写、跑错设备、梯度断链 |
| 输出与 oracle | 对比参考实现、gradcheck 与 sanitizer 报告 | 误差超标、梯度为零、随机错误 |

图示解释：把 算子注册、调度与数值正确性 的结论、证据来源与采集工具连成链，任何断链处都必须标注 [待验证]。

#### 6. 拓展知识
增强证据的工具：torch.utils.cpp_extension.load/load_inline、CUDA_LAUNCH_BLOCKING=1；比较不同证据等级的可信度与成本，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 哪条证据足以把 算子注册、调度与数值正确性 的结论从推断升级为已确认，缺少它会怎样？
2. 如何独立复现该证据，需要固定哪些版本、seed 与测量协议？
3. 如果证据与结论冲突，优先怀疑测量、代码还是环境，如何用 torch.utils.cpp_extension.load/load_inline 二分？

#### 8. 易错点与反例
1. 反例：把模型/框架返回成功当成正确证据，忽略 kernel 越界写破坏相邻显存造成随机错误。
2. 反例：用一次测量代替协议，忽略 缺少 CUDAGuard 在多卡环境跑错设备。
3. 反例：把 [推断] 写成 [已确认]，直到 只写 forward 未注册 backward 使梯度静默为零 才暴露证据不足。

#### 9. 实践与验证
验证方案：实现逐元素算子并与 eager 参考比较，用 gradcheck 校验反向；把 N 设为非 warp 倍数检验尾块处理，跑 compute-sanitizer。 全程用 torch.utils.cpp_extension.load/load_inline 与 CUDA_LAUNCH_BLOCKING=1 留存原始 trace 与版本信息，形成可复现证据包；oracle：fp32 最大绝对误差 < 1e-6、gradcheck 通过，N=线程数±1 时无越界且结果正确。

#### 10. 评分标准
E069 评分：
- 不合格：只给结论，不区分证据等级；
- 合格：能标注证据来源并给出一个可证伪点；
- 良好：能构建可复现证据链并设计推翻性实验；
- 优秀/专家：能给出证据完整性的量化门槛、版本绑定与残余不确定性。
### E070. 证据审计：测试治理

- 难度：专家级
- 知识域：K12 测试治理
- 考察能力：现象分层、假设树、止损、根因与复发防护
- 题型：故障排查与事故复盘
- 面试频率：开放题
- 前置知识：oracle、证据、日志、回滚、门禁、复盘
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

证据审计要求对 测试契约、容差与证据化调试 的每个结论追问证据来源与可复现性：是框架返回、日志推断还是实测。审计对象包括 pytest 参数化与 fixture、assert_close 的 rtol/atol 等断言，以及 通过率 的采集方式。

#### 2. 问题分析
1. 把结论拆成可判定子命题，标注每条的证据等级（源码/文档/实测/推断）。
2. 定位可证伪点：torch.testing.assert_close 在 |a−b| ≤ atol + rtol·|b| 时通过，float32 默认 rtol≈1.3e-6、atol≈1e-5；flaky 率 = 失败重跑数/总重跑数，需按阈值门禁。
3. 检查证据链完整性，例如 pytest -k 按名筛选、-x 首次失败即停、--reruns 需 pytest-rerunfailures 插件；未设种子的随机测试在 CI 中会产生不可复现失败。
4. 设计能推翻结论的最小实验，记录 通过率、覆盖率 与错误类别。

#### 3. 具体答案
面试中可以这样回答：

> 测试是把需求写成可执行的契约；有效的门禁需要确定性、隔离、明确容差和可定位失败的证据，而不是只断言最终数值。
>
> 可证伪判据：torch.testing.assert_close 在 |a−b| ≤ atol + rtol·|b| 时通过，float32 默认 rtol≈1.3e-6、atol≈1e-5；flaky 率 = 失败重跑数/总重跑数，需按阈值门禁。
>
> 可验证事实：pytest -k 按名筛选、-x 首次失败即停、--reruns 需 pytest-rerunfailures 插件；未设种子的随机测试在 CI 中会产生不可复现失败。

#### 4. 技术洞察
审计的依据是可证伪性：torch.testing.assert_close 在 |a−b| ≤ atol + rtol·|b| 时通过，float32 默认 rtol≈1.3e-6、atol≈1e-5；flaky 率 = 失败重跑数/总重跑数，需按阈值门禁。 只有在给定输入、版本与测量协议下才成立；脱离协议的数字是传闻而非证据。

#### 5. 图文说明
| 层次 | 测试契约、容差与证据化调试 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 被测模块、参数组合、种子、容差与基线 | 容差过宽、随机未固定、基线过期 |
| 执行与状态 | 追踪 fixture 生命周期、全局状态与断言点 | 顺序相关、静默 skip、只测终值 |
| 输出与 oracle | 核对断言、失败可定位性与 flaky 统计 | 假绿、不可复现失败、回归漏检 |

图示解释：把 测试契约、容差与证据化调试 的结论、证据来源与采集工具连成链，任何断链处都必须标注 [待验证]。

#### 6. 拓展知识
增强证据的工具：pytest/parametrize/fixture、种子固定与 test 隔离；比较不同证据等级的可信度与成本，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 哪条证据足以把 测试契约、容差与证据化调试 的结论从推断升级为已确认，缺少它会怎样？
2. 如何独立复现该证据，需要固定哪些版本、seed 与测量协议？
3. 如果证据与结论冲突，优先怀疑测量、代码还是环境，如何用 pytest/parametrize/fixture 二分？

#### 8. 易错点与反例
1. 反例：把模型/框架返回成功当成正确证据，忽略 测试间共享全局状态导致顺序相关失败。
2. 反例：用一次测量代替协议，忽略 无 GPU 时静默 skip 使 CI 假绿。
3. 反例：把 [推断] 写成 [已确认]，直到 容差过宽掩盖真实数值回归 才暴露证据不足。

#### 9. 实践与验证
验证方案：为某模块写参数化测试（多 shape/dtype），固定种子并用加严容差的 assert_close；注入一个已知 bug 验证测试能失败。 全程用 pytest/parametrize/fixture 与 种子固定与 test 隔离 留存原始 trace 与版本信息，形成可复现证据包；oracle：测试在正确实现下稳定通过、在注入 bug 下必然失败，多次运行 flaky 率为 0。

#### 10. 评分标准
E070 评分：
- 不合格：只给结论，不区分证据等级；
- 合格：能标注证据来源并给出一个可证伪点；
- 良好：能构建可复现证据链并设计推翻性实验；
- 优秀/专家：能给出证据完整性的量化门槛、版本绑定与残余不确定性。
### E071. 生产事故：张量/设备

- 难度：专家级
- 知识域：K01 张量/设备
- 考察能力：资源预算、吞吐、尾延迟、容量阈值与单位成本
- 题型：性能/容量/成本
- 面试频率：开放题
- 前置知识：shape、stride、dtype、device
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

生产事故视角关注 张量布局、视图语义与设备放置 的性能、容量与成本越界：吞吐下降、显存 OOM、通信拥塞或延迟长尾。排查要先固定现场（版本、配置、负载）再定位，避免把 stride 与 storage_offset 构成的地址映射 与 view/reshape/contiguous 的复制边界 的偶发现象当根因。

#### 2. 问题分析
1. 固定事故现场：版本、配置、负载画像、时间窗口与错误类别。
2. 用容量/性能模型定位瓶颈：对连续张量有 stride[i]=∏_{j>i} shape[j]；view 只在目标 shape 能由既有 stride 整除分解时成立，否则 reshape 触发复制；实数占用满足 numel·itemsize ≤ storage.nbytes − storage_offset·itemsize。
3. 区分根因与放大因素，例如 对非连续张量调用 view 抛 RuntimeError: view size is not compatible with input tensor's size and stride 是否为触发条件。
4. 给出止损、恢复与防复发措施，并用 有效带宽 GB/s、峰值显存 GiB 验证。

#### 3. 具体答案
面试中可以这样回答：

> 张量的本质是 (storage, size, stride, storage_offset) 四元组对一段连续内存的解释；视图与副本的区别决定是否共享存储、是否可安全原地写。设备放置是显式拓扑决策，.to() 按需搬运并可能触发隐式同步。
>
> 容量关系：对连续张量有 stride[i]=∏_{j>i} shape[j]；view 只在目标 shape 能由既有 stride 整除分解时成立，否则 reshape 触发复制；实数占用满足 numel·itemsize ≤ storage.nbytes − storage_offset·itemsize。
>
> 可验证事实：torch.Tensor.transpose 返回共享存储的视图（只互换 stride，不影响 storage），而 .reshape 在不可视图化时返回副本，可用 data_ptr() 是否相同判定。

#### 4. 技术洞察
事故的根因通常是容量或契约被突破：对连续张量有 stride[i]=∏_{j>i} shape[j]；view 只在目标 shape 能由既有 stride 整除分解时成立，否则 reshape 触发复制；实数占用满足 numel·itemsize ≤ storage.nbytes − storage_offset·itemsize。 被打破时表现为 在未同步的跨界 .to() 后立即读数据得到脏值；止损优先于根因，证据留存优先于重试。

#### 5. 图文说明
| 层次 | 张量布局、视图语义与设备放置 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 张量、目标 shape/stride、device/dtype 与 memory_format | 约束缺失会误判视图可行性或越界读 |
| 执行与状态 | 追踪 storage、stride、offset 与 allocator 块的 owner 和生命周期 | 共享存储被原地写、碎片泄漏或隐式同步 |
| 输出与 oracle | 对照 data_ptr、is_contiguous 与显存曲线 | 把副本误当视图、峰值显存与理论值严重偏离 |

图示解释：把 张量布局、视图语义与设备放置 的负载、容量与错误率画成时间线，标出越界点与止损动作，区分触发与放大因素。

#### 6. 拓展知识
相关诊断工具：tensor.stride()/is_contiguous()/storage_offset()/data_ptr()、torch.cuda.memory_summary()/memory_snapshot()；对比扩容、降级与回滚的成本，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 事故是容量不足、性能退化还是正确性问题，如何用 tensor.stride()/is_contiguous()/storage_offset()/data_ptr() 在 5 分钟内区分？
2. 止损应选择限流、降级还是回滚，依据哪个 有效带宽 GB/s 阈值？
3. 如何证明修复有效而非负载自然回落，需要多长时间窗与对照？

#### 8. 易错点与反例
1. 反例：先重试后查因，放大 对非连续张量调用 view 抛 RuntimeError: view size is not compatible with input tensor's size and stride 并丢失现场。
2. 反例：只扩容量不找根因，遇到 pin_memory 张量长期驻留耗尽锁页内存 后再次复发。
3. 反例：用平均吞吐判断恢复，忽略 pin_memory 张量长期驻留耗尽锁页内存 的尾延迟仍在恶化。

#### 9. 实践与验证
验证方案：对同一逻辑张量构造 contiguous、转置、expand 三种布局，断言 is_contiguous/stride/storage_offset，并用 data_ptr 判定 reshape 是否复制。 在受控负载下复现并采集 有效带宽 GB/s、碎片率 reserved/allocated、峰值显存 GiB 的时间序列；oracle：data_ptr 相同判为视图、不同判为副本；峰值显存与理论 numel·itemsize 的偏差落在分配器块对齐范围内。

#### 10. 评分标准
E071 评分：
- 不合格：直接给方案，无现场固定与容量分析；
- 合格：能定位一个瓶颈并给出止损动作；
- 良好：能区分触发/放大因素，给出容量模型与防复发措施；
- 优秀/专家：能补充成本权衡、灰度回滚、监控门禁与残余风险。
### E072. 生产事故：autograd

- 难度：专家级
- 知识域：K02 autograd
- 考察能力：资源预算、吞吐、尾延迟、容量阈值与单位成本
- 题型：性能/容量/成本
- 面试频率：开放题
- 前置知识：requires_grad、leaf、view、detach、反向
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

生产事故视角关注 动态计算图、梯度累积与高阶微分 的性能、容量与成本越界：吞吐下降、显存 OOM、通信拥塞或延迟长尾。排查要先固定现场（版本、配置、负载）再定位，避免把 动态图与 grad_fn 链 与 叶张量与 .grad 累加 的偶发现象当根因。

#### 2. 问题分析
1. 固定事故现场：版本、配置、负载画像、时间窗口与错误类别。
2. 用容量/性能模型定位瓶颈：反向传播沿 grad_fn 链执行链式法则 g_i=Σ_j g_j·(∂f_j/∂x_i)，梯度以累加方式写入 .grad（不清零则叠加）；对需要梯度的张量做 in-place 会使其 version counter 加一，若该版本被 backward 引用则抛 RuntimeError: a variable needed for gradient computation has been modified by an inplace operation。
3. 区分根因与放大因素，例如 detach 后仍期望梯度回传导致叶张量 .grad 为 None 是否为触发条件。
4. 给出止损、恢复与防复发措施，并用 梯度相对误差、峰值激活内存 MiB 验证。

#### 3. 具体答案
面试中可以这样回答：

> autograd 记录前向操作序列构成的动态图，反向时按逆序调用 grad_fn，用链式法则把上游梯度乘上局部雅可比后累加到叶张量。图的生命周期由 retain_graph 控制，原地写通过 version counter 保证数值一致。
>
> 容量关系：反向传播沿 grad_fn 链执行链式法则 g_i=Σ_j g_j·(∂f_j/∂x_i)，梯度以累加方式写入 .grad（不清零则叠加）；对需要梯度的张量做 in-place 会使其 version counter 加一，若该版本被 backward 引用则抛 RuntimeError: a variable needed for gradient computation has been modified by an inplace operation。
>
> 可验证事实：torch.autograd.gradcheck 在 float64 下用有限差分（默认 eps=1e-6、rtol=1.3e-6、atol=1e-5）校验解析梯度，要求输入 requires_grad=True 且函数确定性。

#### 4. 技术洞察
事故的根因通常是容量或契约被突破：反向传播沿 grad_fn 链执行链式法则 g_i=Σ_j g_j·(∂f_j/∂x_i)，梯度以累加方式写入 .grad（不清零则叠加）；对需要梯度的张量做 in-place 会使其 version counter 加一，若该版本被 backward 引用则抛 RuntimeError: a variable needed for gradient computation has been modified by an inplace operation。 被打破时表现为 第二次 backward 未设 retain_graph=True 触发 Trying to backward through the graph a second time；止损优先于根因，证据留存优先于重试。

#### 5. 图文说明
| 层次 | 动态计算图、梯度累积与高阶微分 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | requires_grad 状态、叶张量标志、是否原地写 | 误把非叶张量当叶、图已释放 |
| 执行与状态 | 遍历 grad_fn 链、version counter、saved tensor 所有权 | 图重复释放、原地写破坏反向、梯度断链 |
| 输出与 oracle | 比对解析梯度与数值差分、.grad 累加结果 | 梯度为零/None、误差超容差、二阶导缺失 |

图示解释：把 动态计算图、梯度累积与高阶微分 的负载、容量与错误率画成时间线，标出越界点与止损动作，区分触发与放大因素。

#### 6. 拓展知识
相关诊断工具：tensor.grad_fn 与 next_functions 遍历、torch.autograd.anomaly_mode/set_detect_anomaly；对比扩容、降级与回滚的成本，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 事故是容量不足、性能退化还是正确性问题，如何用 tensor.grad_fn 与 next_functions 遍历 在 5 分钟内区分？
2. 止损应选择限流、降级还是回滚，依据哪个 梯度相对误差 阈值？
3. 如何证明修复有效而非负载自然回落，需要多长时间窗与对照？

#### 8. 易错点与反例
1. 反例：先重试后查因，放大 detach 后仍期望梯度回传导致叶张量 .grad 为 None 并丢失现场。
2. 反例：只扩容量不找根因，遇到 对参与反向的张量原地写触发 version counter 报错 后再次复发。
3. 反例：用平均吞吐判断恢复，忽略 对参与反向的张量原地写触发 version counter 报错 的尾延迟仍在恶化。

#### 9. 实践与验证
验证方案：构造 y=x·sin(x)，用 torch.autograd.grad 求一阶导并与有限差分对比；再开 create_graph=True 求二阶导，验证与解析式 cos(x)−x·sin(x) 一致。 在受控负载下复现并采集 梯度相对误差、计算图节点数、峰值激活内存 MiB 的时间序列；oracle：gradcheck 返回 True（误差在 rtol=1e-3/atol=1e-5 内），二阶导相对误差 < 1e-4。

#### 10. 评分标准
E072 评分：
- 不合格：直接给方案，无现场固定与容量分析；
- 合格：能定位一个瓶颈并给出止损动作；
- 良好：能区分触发/放大因素，给出容量模型与防复发措施；
- 优秀/专家：能补充成本权衡、灰度回滚、监控门禁与残余风险。
### E073. 生产事故：Module/状态

- 难度：专家级
- 知识域：K03 Module/状态
- 考察能力：资源预算、吞吐、尾延迟、容量阈值与单位成本
- 题型：性能/容量/成本
- 面试频率：开放题
- 前置知识：参数、buffer、模式和 state_dict
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

生产事故视角关注 模块树、参数字典与状态持久化 的性能、容量与成本越界：吞吐下降、显存 OOM、通信拥塞或延迟长尾。排查要先固定现场（版本、配置、负载）再定位，避免把 Module 树与命名参数 与 parameter 与 buffer 的区别 的偶发现象当根因。

#### 2. 问题分析
1. 固定事故现场：版本、配置、负载画像、时间窗口与错误类别。
2. 用容量/性能模型定位瓶颈：state_dict 的键等于模块路径拼接参数/缓冲区名（如 layer1.0.conv.weight）；load_state_dict(strict=True) 要求键集合完全一致，否则报 missing/unexpected keys；buffer 不参与 optimizer 更新但会被 .to() 搬运。
3. 区分根因与放大因素，例如 buffer 未 register_buffer 导致不随 .to() 和 state_dict 迁移 是否为触发条件。
4. 给出止损、恢复与防复发措施，并用 参数量、键匹配率 验证。

#### 3. 具体答案
面试中可以这样回答：

> nn.Module 是一棵注册树，参数与缓冲区经命名空间形成扁平键空间；state_dict 是这棵树的快照契约，.to() 递归搬运所有注册张量。模式切换与注册完整性共同决定训练/推理一致性。
>
> 容量关系：state_dict 的键等于模块路径拼接参数/缓冲区名（如 layer1.0.conv.weight）；load_state_dict(strict=True) 要求键集合完全一致，否则报 missing/unexpected keys；buffer 不参与 optimizer 更新但会被 .to() 搬运。
>
> 可验证事实：model.eval() 只切换 dropout/BatchNorm 等模块的 training 标志，不关闭 autograd；省显存必须叠加 torch.no_grad() 或 inference_mode()。

#### 4. 技术洞察
事故的根因通常是容量或契约被突破：state_dict 的键等于模块路径拼接参数/缓冲区名（如 layer1.0.conv.weight）；load_state_dict(strict=True) 要求键集合完全一致，否则报 missing/unexpected keys；buffer 不参与 optimizer 更新但会被 .to() 搬运。 被打破时表现为 共享参数被重复注册改变键空间与加载行为；止损优先于根因，证据留存优先于重试。

#### 5. 图文说明
| 层次 | 模块树、参数字典与状态持久化 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 模块结构、注册的参数/缓冲区、目标 device/dtype | 漏注册 buffer 或键空间不一致 |
| 执行与状态 | 遍历模块树、加载/保存快照、切换 training 标志 | 键缺失被静默、模式与图不匹配、别名错乱 |
| 输出与 oracle | 逐张量比对 state_dict、核对模式相关的统计 | 权重随机初始化、BN 统计漂移、显存未释放 |

图示解释：把 模块树、参数字典与状态持久化 的负载、容量与错误率画成时间线，标出越界点与止损动作，区分触发与放大因素。

#### 6. 拓展知识
相关诊断工具：named_parameters()/named_buffers()/recurse、state_dict()/load_state_dict()；对比扩容、降级与回滚的成本，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 事故是容量不足、性能退化还是正确性问题，如何用 named_parameters()/named_buffers()/recurse 在 5 分钟内区分？
2. 止损应选择限流、降级还是回滚，依据哪个 参数量 阈值？
3. 如何证明修复有效而非负载自然回落，需要多长时间窗与对照？

#### 8. 易错点与反例
1. 反例：先重试后查因，放大 buffer 未 register_buffer 导致不随 .to() 和 state_dict 迁移 并丢失现场。
2. 反例：只扩容量不找根因，遇到 strict=False 静默吞掉缺失键使部分权重随机初始化 后再次复发。
3. 反例：用平均吞吐判断恢复，忽略 strict=False 静默吞掉缺失键使部分权重随机初始化 的尾延迟仍在恶化。

#### 9. 实践与验证
验证方案：构造含卷积、BN、自定义 buffer 的模块，round-trip state_dict 到新实例并 assert_close 全部张量；删掉一个 buffer 验证 strict=True 报 missing keys。 在受控负载下复现并采集 参数量、状态字典字节数、键匹配率 的时间序列；oracle：load_state_dict 返回 missing=[]、unexpected=[]；train 模式两次 forward 后 BN running_mean 发生变化。

#### 10. 评分标准
E073 评分：
- 不合格：直接给方案，无现场固定与容量分析；
- 合格：能定位一个瓶颈并给出止损动作；
- 良好：能区分触发/放大因素，给出容量模型与防复发措施；
- 优秀/专家：能补充成本权衡、灰度回滚、监控门禁与残余风险。
### E074. 生产事故：数据管道

- 难度：专家级
- 知识域：K04 数据管道
- 考察能力：资源预算、吞吐、尾延迟、容量阈值与单位成本
- 题型：性能/容量/成本
- 面试频率：开放题
- 前置知识：Dataset、Sampler、collate、padding、版本
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

生产事故视角关注 DataLoader 的并行、采样与预取 的性能、容量与成本越界：吞吐下降、显存 OOM、通信拥塞或延迟长尾。排查要先固定现场（版本、配置、负载）再定位，避免把 map-style 与 iterable-style Dataset 与 num_workers 与进程启动方式 的偶发现象当根因。

#### 2. 问题分析
1. 固定事故现场：版本、配置、负载画像、时间窗口与错误类别。
2. 用容量/性能模型定位瓶颈：map-style 数据集每 epoch 批数 = ⌈N/batch_size⌉（drop_last 时为 ⌊N/batch_size⌋）；worker 是独立进程并各自持有 RNG，随机变换必须经 worker_init_fn 播种才可复现。
3. 区分根因与放大因素，例如 在 Dataset.__init__ 或 __getitem__ 中初始化 CUDA 导致 fork 后 worker 崩溃 是否为触发条件。
4. 给出止损、恢复与防复发措施，并用 吞吐 samples/s、首 batch 延迟 验证。

#### 3. 具体答案
面试中可以这样回答：

> 数据管道把采样、取数、批处理和预取解耦到多 worker 进程；正确性取决于索引覆盖与随机性可复现，性能取决于预取深度与 H2D 重叠。
>
> 容量关系：map-style 数据集每 epoch 批数 = ⌈N/batch_size⌉（drop_last 时为 ⌊N/batch_size⌋）；worker 是独立进程并各自持有 RNG，随机变换必须经 worker_init_fn 播种才可复现。
>
> 可验证事实：pin_memory=True 配合 non_blocking=True 能把 H2D 拷贝与计算重叠；persistent_workers=True 在 epoch 间保留 worker 进程，避免反复 fork 开销。

#### 4. 技术洞察
事故的根因通常是容量或契约被突破：map-style 数据集每 epoch 批数 = ⌈N/batch_size⌉（drop_last 时为 ⌊N/batch_size⌋）；worker 是独立进程并各自持有 RNG，随机变换必须经 worker_init_fn 播种才可复现。 被打破时表现为 worker 数过大引发内存或文件句柄耗尽；止损优先于根因，证据留存优先于重试。

#### 5. 图文说明
| 层次 | DataLoader 的并行、采样与预取 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 数据集大小、batch_size、worker 数、采样方式 | 索引不覆盖、随机性不可复现 |
| 执行与状态 | 追踪 worker 进程、预取队列、pin 缓冲与 sampler 状态 | CUDA-in-fork 崩溃、句柄泄漏、顺序错乱 |
| 输出与 oracle | 核对样本集合、shape、吞吐与首 batch 延迟 | 重复/丢失样本、collate 形状错误、吞吐饱和异常 |

图示解释：把 DataLoader 的并行、采样与预取 的负载、容量与错误率画成时间线，标出越界点与止损动作，区分触发与放大因素。

#### 6. 拓展知识
相关诊断工具：DataLoader/worker_init_fn/generator、torch.utils.data.get_worker_info；对比扩容、降级与回滚的成本，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 事故是容量不足、性能退化还是正确性问题，如何用 DataLoader/worker_init_fn/generator 在 5 分钟内区分？
2. 止损应选择限流、降级还是回滚，依据哪个 吞吐 samples/s 阈值？
3. 如何证明修复有效而非负载自然回落，需要多长时间窗与对照？

#### 8. 易错点与反例
1. 反例：先重试后查因，放大 在 Dataset.__init__ 或 __getitem__ 中初始化 CUDA 导致 fork 后 worker 崩溃 并丢失现场。
2. 反例：只扩容量不找根因，遇到 collate_fn 对不定长序列堆叠得到错误 shape 后再次复发。
3. 反例：用平均吞吐判断恢复，忽略 collate_fn 对不定长序列堆叠得到错误 shape 的尾延迟仍在恶化。

#### 9. 实践与验证
验证方案：固定 seed 与 worker_init_fn，连续两 epoch 记录样本索引序列验证覆盖完整且可复现；测量 num_workers∈{0,2,4,8} 的吞吐拐点。 在受控负载下复现并采集 吞吐 samples/s、GPU 利用率、首 batch 延迟 的时间序列；oracle：两轮索引并集等于 0..N−1 且无重复，吞吐在某一 worker 数后饱和。

#### 10. 评分标准
E074 评分：
- 不合格：直接给方案，无现场固定与容量分析；
- 合格：能定位一个瓶颈并给出止损动作；
- 良好：能区分触发/放大因素，给出容量模型与防复发措施；
- 优秀/专家：能补充成本权衡、灰度回滚、监控门禁与残余风险。
### E075. 生产事故：训练/优化器

- 难度：专家级
- 知识域：K05 训练/优化器
- 考察能力：资源预算、吞吐、尾延迟、容量阈值与单位成本
- 题型：性能/容量/成本
- 面试频率：开放题
- 前置知识：forward、backward、累积、optimizer、scheduler
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

生产事故视角关注 优化器更新、学习率调度与梯度处理 的性能、容量与成本越界：吞吐下降、显存 OOM、通信拥塞或延迟长尾。排查要先固定现场（版本、配置、负载）再定位，避免把 优化器状态（Adam 的 m/v） 与 zero_grad 语义 的偶发现象当根因。

#### 2. 问题分析
1. 固定事故现场：版本、配置、负载画像、时间窗口与错误类别。
2. 用容量/性能模型定位瓶颈：Adam：m_t=β1·m_{t−1}+(1−β1)·g_t，v_t=β2·v_{t−1}+(1−β2)·g_t²，偏差校正 m̂=m/(1−β1^t)、v̂=v/(1−β2^t)，θ←θ−lr·m̂/(√v̂+ε)；梯度累积 k 步等价于把有效批放大 k 倍，但必须对 loss 除以 k 或按 k 归约，否则梯度被放大 k 倍。
3. 区分根因与放大因素，例如 忘记 zero_grad 使梯度跨 step 累加，等效 lr 漂移 是否为触发条件。
4. 给出止损、恢复与防复发措施，并用 loss 曲线、学习率序列 验证。

#### 3. 具体答案
面试中可以这样回答：

> 训练步是 zero_grad→forward→loss→scaled backward→unscale→clip→step→scheduler 的固定顺序；顺序错位会静默改变有效学习率或裁剪幅度。
>
> 容量关系：Adam：m_t=β1·m_{t−1}+(1−β1)·g_t，v_t=β2·v_{t−1}+(1−β2)·g_t²，偏差校正 m̂=m/(1−β1^t)、v̂=v/(1−β2^t)，θ←θ−lr·m̂/(√v̂+ε)；梯度累积 k 步等价于把有效批放大 k 倍，但必须对 loss 除以 k 或按 k 归约，否则梯度被放大 k 倍。
>
> 可验证事实：zero_grad(set_to_none=True) 把 .grad 置 None（自 PyTorch 2.0 起为默认行为），比置零更省内存；optimizer.state_dict() 按参数索引保存 momentum/m/v 缓冲区。

#### 4. 技术洞察
事故的根因通常是容量或契约被突破：Adam：m_t=β1·m_{t−1}+(1−β1)·g_t，v_t=β2·v_{t−1}+(1−β2)·g_t²，偏差校正 m̂=m/(1−β1^t)、v̂=v/(1−β2^t)，θ←θ−lr·m̂/(√v̂+ε)；梯度累积 k 步等价于把有效批放大 k 倍，但必须对 loss 除以 k 或按 k 归约，否则梯度被放大 k 倍。 被打破时表现为 scheduler.step 放错位置（每 batch 而非每 epoch）改变 lr 曲线；止损优先于根因，证据留存优先于重试。

#### 5. 图文说明
| 层次 | 优化器更新、学习率调度与梯度处理 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 初始参数、优化器配置、损失缩放与累积步 k | 顺序错位、未缩放 loss |
| 执行与状态 | 追踪 .grad、优化器状态缓冲区、lr 调度历史 | 梯度残留、动量错乱、裁剪幅度被缩放 |
| 输出与 oracle | 核对 loss 单调性、参数更新与 lr 序列 | loss 震荡、等效 lr 漂移、累积实现不等价 |

图示解释：把 优化器更新、学习率调度与梯度处理 的负载、容量与错误率画成时间线，标出越界点与止损动作，区分触发与放大因素。

#### 6. 拓展知识
相关诊断工具：optimizer.state_dict()/param_groups、scheduler.get_last_lr()；对比扩容、降级与回滚的成本，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 事故是容量不足、性能退化还是正确性问题，如何用 optimizer.state_dict()/param_groups 在 5 分钟内区分？
2. 止损应选择限流、降级还是回滚，依据哪个 loss 曲线 阈值？
3. 如何证明修复有效而非负载自然回落，需要多长时间窗与对照？

#### 8. 易错点与反例
1. 反例：先重试后查因，放大 忘记 zero_grad 使梯度跨 step 累加，等效 lr 漂移 并丢失现场。
2. 反例：只扩容量不找根因，遇到 clip_grad_norm_ 在 GradScaler.unscale_ 之前调用使裁剪阈值被放大 S 倍 后再次复发。
3. 反例：用平均吞吐判断恢复，忽略 clip_grad_norm_ 在 GradScaler.unscale_ 之前调用使裁剪阈值被放大 S 倍 的尾延迟仍在恶化。

#### 9. 实践与验证
验证方案：在单个 batch 上跑 200 步验证 loss 单调下降到过拟合；打印 get_last_lr 序列核对调度曲线；对同一初始点比较累积实现与放大批实现的参数更新。 在受控负载下复现并采集 loss 曲线、全局梯度范数、学习率序列 的时间序列；oracle：单批过拟合 loss < 0.01；累积与大批实现的参数差在 1e-6 内。

#### 10. 评分标准
E075 评分：
- 不合格：直接给方案，无现场固定与容量分析；
- 合格：能定位一个瓶颈并给出止损动作；
- 良好：能区分触发/放大因素，给出容量模型与防复发措施；
- 优秀/专家：能补充成本权衡、灰度回滚、监控门禁与残余风险。
### E076. 生产事故：AMP/恢复

- 难度：专家级
- 知识域：K06 AMP/恢复
- 考察能力：资源预算、吞吐、尾延迟、容量阈值与单位成本
- 题型：性能/容量/成本
- 面试频率：开放题
- 前置知识：autocast、scaler、快照、随机状态
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

生产事故视角关注 混合精度、检查点与确定性 的性能、容量与成本越界：吞吐下降、显存 OOM、通信拥塞或延迟长尾。排查要先固定现场（版本、配置、负载）再定位，避免把 autocast 的算子白名单 与 GradScaler 动态缩放 的偶发现象当根因。

#### 2. 问题分析
1. 固定事故现场：版本、配置、负载画像、时间窗口与错误类别。
2. 用容量/性能模型定位瓶颈：GradScaler 先用 scale S 放大 loss 再反向，unscale_ 后若检测到 inf/nan 则跳过该 step 并把 S 减半；fp16 最正规数约 6.1e-5、次正规至约 6e-8，小于此量级的梯度下溢，故需缩放；bf16 指数位与 fp32 同为 8 位因此不需要缩放。
3. 区分根因与放大因素，例如 fp16 无 scaler 导致梯度下溢为 0 或 NaN 是否为触发条件。
4. 给出止损、恢复与防复发措施，并用 scale factor 曲线、resume 前后参数最大差 验证。

#### 3. 具体答案
面试中可以这样回答：

> AMP 通过 autocast 选低精度算子、GradScaler 维持梯度动态范围；恢复的正确性是状态闭包问题——模型、优化器、缩放与随机数状态缺一不可。
>
> 容量关系：GradScaler 先用 scale S 放大 loss 再反向，unscale_ 后若检测到 inf/nan 则跳过该 step 并把 S 减半；fp16 最正规数约 6.1e-5、次正规至约 6e-8，小于此量级的梯度下溢，故需缩放；bf16 指数位与 fp32 同为 8 位因此不需要缩放。
>
> 可验证事实：torch.use_deterministic_algorithms(True) 会让部分算子回退到确定性实现或直接报错；cudnn.benchmark=True 的自动调优会破坏逐步可复现性。完整恢复需保存 model、optimizer、scaler、epoch、CPU 与 CUDA RNG 及 sampler 状态。

#### 4. 技术洞察
事故的根因通常是容量或契约被突破：GradScaler 先用 scale S 放大 loss 再反向，unscale_ 后若检测到 inf/nan 则跳过该 step 并把 S 减半；fp16 最正规数约 6.1e-5、次正规至约 6e-8，小于此量级的梯度下溢，故需缩放；bf16 指数位与 fp32 同为 8 位因此不需要缩放。 被打破时表现为 resume 只存模型权重丢失优化器动量使曲线跳变；止损优先于根因，证据留存优先于重试。

#### 5. 图文说明
| 层次 | 混合精度、检查点与确定性 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | dtype、autocast 区域、scaler 配置、checkpoint 内容 | 状态不闭合、精度选择错误 |
| 执行与状态 | 追踪 scale factor、RNG、优化器状态与 nan/inf 分支 | 下溢、NaN、恢复后状态漂移 |
| 输出与 oracle | 比对续训与基线参数、scale 曲线、依赖确定性开关 | 曲线跳变、结果不可复现、梯度全零 |

图示解释：把 混合精度、检查点与确定性 的负载、容量与错误率画成时间线，标出越界点与止损动作，区分触发与放大因素。

#### 6. 拓展知识
相关诊断工具：torch.cuda.amp.GradScaler 与 autocast、torch.get_rng_state/get_cuda_rng_state/numpy.random.get_state；对比扩容、降级与回滚的成本，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 事故是容量不足、性能退化还是正确性问题，如何用 torch.cuda.amp.GradScaler 与 autocast 在 5 分钟内区分？
2. 止损应选择限流、降级还是回滚，依据哪个 scale factor 曲线 阈值？
3. 如何证明修复有效而非负载自然回落，需要多长时间窗与对照？

#### 8. 易错点与反例
1. 反例：先重试后查因，放大 fp16 无 scaler 导致梯度下溢为 0 或 NaN 并丢失现场。
2. 反例：只扩容量不找根因，遇到 只存 torch RNG 未存 numpy/random 状态导致数据顺序不同 后再次复发。
3. 反例：用平均吞吐判断恢复，忽略 只存 torch RNG 未存 numpy/random 状态导致数据顺序不同 的尾延迟仍在恶化。

#### 9. 实践与验证
验证方案：训练 50 步保存完整 checkpoint，重启加载后再跑 50 步与不中断基线逐参数比较；fp16 下注入大梯度观测 S 是否减半。 在受控负载下复现并采集 scale factor 曲线、NaN/inf 次数、resume 前后参数最大差 的时间序列；oracle：续训与基线参数最大差 < 1e-5（bit 级取决于确定性开关），scale factor 与跳过计数一致。

#### 10. 评分标准
E076 评分：
- 不合格：直接给方案，无现场固定与容量分析；
- 合格：能定位一个瓶颈并给出止损动作；
- 良好：能区分触发/放大因素，给出容量模型与防复发措施；
- 优秀/专家：能补充成本权衡、灰度回滚、监控门禁与残余风险。
### E077. 生产事故：分布式

- 难度：专家级
- 知识域：K07 分布式
- 考察能力：资源预算、吞吐、尾延迟、容量阈值与单位成本
- 题型：性能/容量/成本
- 面试频率：开放题
- 前置知识：rank、DDP、FSDP、collective、容错
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

生产事故视角关注 进程组、集合通信与梯度同步 的性能、容量与成本越界：吞吐下降、显存 OOM、通信拥塞或延迟长尾。排查要先固定现场（版本、配置、负载）再定位，避免把 进程组与 rank/world_size 与 all_reduce/reduce_scatter/all_gather 的偶发现象当根因。

#### 2. 问题分析
1. 固定事故现场：版本、配置、负载画像、时间窗口与错误类别。
2. 用容量/性能模型定位瓶颈：DDP 默认 all_reduce(sum) 后除以 world_size 得平均梯度，故全局有效批 = 本地批 × world_size × 累积步数；ring all_reduce 单步通信量 ≈ 2·(N−1)/N·S（S 为梯度字节数），大 N 时趋近 2S。
3. 区分根因与放大因素，例如 各 rank 集合通信调用不匹配导致 hang 直到 watchdog 超时 是否为触发条件。
4. 给出止损、恢复与防复发措施，并用 all_reduce 带宽 GB/s、step 时间方差 验证。

#### 3. 具体答案
面试中可以这样回答：

> 分布式训练把单机步拆成计算→梯度归约→更新三段；正确性在于所有 rank 以相同顺序进入集合通信且梯度按 world_size 归一，性能在于把通信藏进反向的尾部。
>
> 容量关系：DDP 默认 all_reduce(sum) 后除以 world_size 得平均梯度，故全局有效批 = 本地批 × world_size × 累积步数；ring all_reduce 单步通信量 ≈ 2·(N−1)/N·S（S 为梯度字节数），大 N 时趋近 2S。
>
> 可验证事实：NCCL 只支持 CUDA 张量、gloo 支持 CPU；DDP 默认把梯度分成约 25 MiB 的桶以便与反向计算重叠通信。

#### 4. 技术洞察
事故的根因通常是容量或契约被突破：DDP 默认 all_reduce(sum) 后除以 world_size 得平均梯度，故全局有效批 = 本地批 × world_size × 累积步数；ring all_reduce 单步通信量 ≈ 2·(N−1)/N·S（S 为梯度字节数），大 N 时趋近 2S。 被打破时表现为 未按 world_size 归一梯度使等效 lr 随卡数变化；止损优先于根因，证据留存优先于重试。

#### 5. 图文说明
| 层次 | 进程组、集合通信与梯度同步 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 进程数、后端、张量是否 CUDA、参与反向的参数集合 | 后端不匹配、参数未参与、集合调用不一致 |
| 执行与状态 | 追踪 rank、bucket、通信算子与反向重叠 | hang、梯度未归约、通信未重叠 |
| 输出与 oracle | 对比单卡/DDP 梯度、通信带宽与 step 方差 | 梯度差超容差、超时、吞吐不随卡数提升 |

图示解释：把 进程组、集合通信与梯度同步 的负载、容量与错误率画成时间线，标出越界点与止损动作，区分触发与放大因素。

#### 6. 拓展知识
相关诊断工具：torch.distributed.init_process_group/barrier、dist.all_reduce/all_gather/reduce_scatter；对比扩容、降级与回滚的成本，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 事故是容量不足、性能退化还是正确性问题，如何用 torch.distributed.init_process_group/barrier 在 5 分钟内区分？
2. 止损应选择限流、降级还是回滚，依据哪个 all_reduce 带宽 GB/s 阈值？
3. 如何证明修复有效而非负载自然回落，需要多长时间窗与对照？

#### 8. 易错点与反例
1. 反例：先重试后查因，放大 各 rank 集合通信调用不匹配导致 hang 直到 watchdog 超时 并丢失现场。
2. 反例：只扩容量不找根因，遇到 NCCL/驱动版本不一致引发通信超时或崩溃 后再次复发。
3. 反例：用平均吞吐判断恢复，忽略 NCCL/驱动版本不一致引发通信超时或崩溃 的尾延迟仍在恶化。

#### 9. 实践与验证
验证方案：用 torchrun 起 2 进程，固定初始化使单卡与 DDP 梯度可对齐，比较 all_reduce 前后梯度；人为让一个 rank 提前退出观测 NCCL watchdog 超时。 在受控负载下复现并采集 all_reduce 带宽 GB/s、通信时间占比、step 时间方差 的时间序列；oracle：DDP 与单卡初始梯度（除以 world_size 后）相对误差 < 1e-6；NCCL_DEBUG 显示预期的 ring/tree 算法。

#### 10. 评分标准
E077 评分：
- 不合格：直接给方案，无现场固定与容量分析；
- 合格：能定位一个瓶颈并给出止损动作；
- 良好：能区分触发/放大因素，给出容量模型与防复发措施；
- 优秀/专家：能补充成本权衡、灰度回滚、监控门禁与残余风险。
### E078. 生产事故：编译栈

- 难度：专家级
- 知识域：K08 编译栈
- 考察能力：资源预算、吞吐、尾延迟、容量阈值与单位成本
- 题型：性能/容量/成本
- 面试频率：开放题
- 前置知识：Dynamo、AOTAutograd、Inductor、guard、缓存
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

生产事故视角关注 Dynamo 捕获、guard 与 Inductor 代码生成 的性能、容量与成本越界：吞吐下降、显存 OOM、通信拥塞或延迟长尾。排查要先固定现场（版本、配置、负载）再定位，避免把 TorchDynamo 的字节码捕获 与 guard 与 specialization 的偶发现象当根因。

#### 2. 问题分析
1. 固定事故现场：版本、配置、负载画像、时间窗口与错误类别。
2. 用容量/性能模型定位瓶颈：一个编译单元的 guard 绑定在张量的 shape/dtype/device 与 Python 常量上；任一 guard 失效即触发重编译，默认累计重编译上限约 8 次（torch._dynamo.config.accumulated_recompile_limit）。图被 graph break 切成多个子图，未捕获区间回退 eager。
3. 区分根因与放大因素，例如 在图中调用 .item()/print/依赖数据的 Python 分支触发 graph break 是否为触发条件。
4. 给出止损、恢复与防复发措施，并用 重编译次数、compiled/eager 加速比 验证。

#### 3. 具体答案
面试中可以这样回答：

> 编译栈把 Python 执行轨迹捕获成 FX 图，用 guard 保证图对后续输入仍有效；失效即重编译、无法捕获即断图。稳定收益来自低断点数与低重编译率而非一次加速。
>
> 容量关系：一个编译单元的 guard 绑定在张量的 shape/dtype/device 与 Python 常量上；任一 guard 失效即触发重编译，默认累计重编译上限约 8 次（torch._dynamo.config.accumulated_recompile_limit）。图被 graph break 切成多个子图，未捕获区间回退 eager。
>
> 可验证事实：设置 TORCH_LOGS=graph_breaks,guards,recompiles 可打印断点与重编译原因；torch._dynamo.explain(fn)(*args) 返回断点数量与原因；dynamic=True 或 torch._dynamo.mark_dynamic 可放宽形状守卫。

#### 4. 技术洞察
事故的根因通常是容量或契约被突破：一个编译单元的 guard 绑定在张量的 shape/dtype/device 与 Python 常量上；任一 guard 失效即触发重编译，默认累计重编译上限约 8 次（torch._dynamo.config.accumulated_recompile_limit）。图被 graph break 切成多个子图，未捕获区间回退 eager。 被打破时表现为 动态 shape 引发重编译风暴使性能倒挂；止损优先于根因，证据留存优先于重试。

#### 5. 图文说明
| 层次 | Dynamo 捕获、guard 与 Inductor 代码生成 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 函数代码、示例输入、shape/dtype/device 与 dynamic 声明 | 守卫过严、断点隐藏、动态维度未声明 |
| 执行与状态 | 追踪 Dynamo 捕获、guard 集合、子图与 Inductor 缓存 | 重编译风暴、断点回退 eager、缓存未命中 |
| 输出与 oracle | 对比 compiled/eager 数值、重编译与断点计数 | 数值不一致、加速比 < 1、重编译失控 |

图示解释：把 Dynamo 捕获、guard 与 Inductor 代码生成 的负载、容量与错误率画成时间线，标出越界点与止损动作，区分触发与放大因素。

#### 6. 拓展知识
相关诊断工具：TORCH_LOGS=graph_breaks,guards,recompiles、torch._dynamo.explain；对比扩容、降级与回滚的成本，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 事故是容量不足、性能退化还是正确性问题，如何用 TORCH_LOGS=graph_breaks,guards,recompiles 在 5 分钟内区分？
2. 止损应选择限流、降级还是回滚，依据哪个 重编译次数 阈值？
3. 如何证明修复有效而非负载自然回落，需要多长时间窗与对照？

#### 8. 易错点与反例
1. 反例：先重试后查因，放大 在图中调用 .item()/print/依赖数据的 Python 分支触发 graph break 并丢失现场。
2. 反例：只扩容量不找根因，遇到 别名或原地写使编译结果与 eager 数值不一致 后再次复发。
3. 反例：用平均吞吐判断恢复，忽略 别名或原地写使编译结果与 eager 数值不一致 的尾延迟仍在恶化。

#### 9. 实践与验证
验证方案：对同一函数用固定 shape 与变化 shape 各跑 20 次，用 explain 统计 graph break 与重编译；对比 compiled 与 eager 输出最大差。 在受控负载下复现并采集 重编译次数、graph break 数、compiled/eager 加速比 的时间序列；oracle：固定 shape 下重编译次数为 1、输出与 eager 最大差 < 1e-4；变化 shape 下开启 dynamic 后重编译显著下降。

#### 10. 评分标准
E078 评分：
- 不合格：直接给方案，无现场固定与容量分析；
- 合格：能定位一个瓶颈并给出止损动作；
- 良好：能区分触发/放大因素，给出容量模型与防复发措施；
- 优秀/专家：能补充成本权衡、灰度回滚、监控门禁与残余风险。
### E079. 生产事故：性能

- 难度：专家级
- 知识域：K09 性能
- 考察能力：资源预算、吞吐、尾延迟、容量阈值与单位成本
- 题型：性能/容量/成本
- 面试频率：开放题
- 前置知识：关键路径、带宽、算力、通信、显存、统计
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

生产事故视角关注 性能剖析、roofline 与显存容量 的性能、容量与成本越界：吞吐下降、显存 OOM、通信拥塞或延迟长尾。排查要先固定现场（版本、配置、负载）再定位，避免把 CUDA event 计时与同步 与 roofline 与算术强度 的偶发现象当根因。

#### 2. 问题分析
1. 固定事故现场：版本、配置、负载画像、时间窗口与错误类别。
2. 用容量/性能模型定位瓶颈：算术强度 I=FLOPs/Bytes，roofline 上界 = min(峰值算力 P, I×峰值带宽 BW)，据此判定 compute-bound 或 memory-bound；CUDA event 计时为 t=elapsed_time(start,stop)，异步操作必须 torch.cuda.synchronize() 后计时才可信。
3. 区分根因与放大因素，例如 异步未同步导致计时偏短，得出虚假加速 是否为触发条件。
4. 给出止损、恢复与防复发措施，并用 p50/p99 延迟、MFU/TFLOPS 验证。

#### 3. 具体答案
面试中可以这样回答：

> 性能结论必须绑定可复现的测量协议：warmup、同步、重复次数、分位数；再用 roofline 判定算力受限还是带宽受限，最后才谈优化手段。
>
> 容量关系：算术强度 I=FLOPs/Bytes，roofline 上界 = min(峰值算力 P, I×峰值带宽 BW)，据此判定 compute-bound 或 memory-bound；CUDA event 计时为 t=elapsed_time(start,stop)，异步操作必须 torch.cuda.synchronize() 后计时才可信。
>
> 可验证事实：torch.profiler.profile(activities=[ProfilerActivity.CUDA]) 会插入 CUDA 事件并输出按算子聚合的 self/total CUDA time；首次迭代含 cuBLAS/context 初始化与自动调优，必须先 warmup 再计时。

#### 4. 技术洞察
事故的根因通常是容量或契约被突破：算术强度 I=FLOPs/Bytes，roofline 上界 = min(峰值算力 P, I×峰值带宽 BW)，据此判定 compute-bound 或 memory-bound；CUDA event 计时为 t=elapsed_time(start,stop)，异步操作必须 torch.cuda.synchronize() 后计时才可信。 被打破时表现为 把 profiler 自身开销计入结论；止损优先于根因，证据留存优先于重试。

#### 5. 图文说明
| 层次 | 性能剖析、roofline 与显存容量 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 算子/规模、dtype、warmup 与 repeat、计时方式 | 未同步、未 warmup、规模误导 |
| 执行与状态 | 追踪 kernel 时长、显存曲线、launch 与同步点 | 计时偏短、碎片抖动、把 launch 开销当算力 |
| 输出与 oracle | 对比 event/profiler、roofline 定位与分位数 | 加速比不稳定、有效带宽超理论值、尾延迟超标 |

图示解释：把 性能剖析、roofline 与显存容量 的负载、容量与错误率画成时间线，标出越界点与止损动作，区分触发与放大因素。

#### 6. 拓展知识
相关诊断工具：torch.profiler、torch.cuda.Event/elapsed_time；对比扩容、降级与回滚的成本，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 事故是容量不足、性能退化还是正确性问题，如何用 torch.profiler 在 5 分钟内区分？
2. 止损应选择限流、降级还是回滚，依据哪个 p50/p99 延迟 阈值？
3. 如何证明修复有效而非负载自然回落，需要多长时间窗与对照？

#### 8. 易错点与反例
1. 反例：先重试后查因，放大 异步未同步导致计时偏短，得出虚假加速 并丢失现场。
2. 反例：只扩容量不找根因，遇到 只测单次忽略冷启动与首次调优 后再次复发。
3. 反例：用平均吞吐判断恢复，忽略 只测单次忽略冷启动与首次调优 的尾延迟仍在恶化。

#### 9. 实践与验证
验证方案：对 matmul、逐元素、拷贝分别用 CUDA event 与 profiler 计时，warmup=10、repeat=100、取中位数；用 bytes/time 反推有效带宽并与理论比较。 在受控负载下复现并采集 p50/p99 延迟、有效带宽 GB/s、MFU/TFLOPS 的时间序列；oracle：event 计时与 profiler 的 CUDA time 偏差 < 5%，大 size 下有效带宽落在理论峰值的可解释比例内。

#### 10. 评分标准
E079 评分：
- 不合格：直接给方案，无现场固定与容量分析；
- 合格：能定位一个瓶颈并给出止损动作；
- 良好：能区分触发/放大因素，给出容量模型与防复发措施；
- 优秀/专家：能补充成本权衡、灰度回滚、监控门禁与残余风险。
### E080. 生产事故：导出部署

- 难度：专家级
- 知识域：K10 导出部署
- 考察能力：资源预算、吞吐、尾延迟、容量阈值与单位成本
- 题型：性能/容量/成本
- 面试频率：开放题
- 前置知识：state_dict、Export、runtime、schema、迁移
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

生产事故视角关注 图导出、图签名与跨版本加载 的性能、容量与成本越界：吞吐下降、显存 OOM、通信拥塞或延迟长尾。排查要先固定现场（版本、配置、负载）再定位，避免把 torch.export/ExportedProgram 与 torch.jit.trace/script 的偶发现象当根因。

#### 2. 问题分析
1. 固定事故现场：版本、配置、负载画像、时间窗口与错误类别。
2. 用容量/性能模型定位瓶颈：导出图是函数式的：无 Python 副作用，输入输出由 graph_signature 声明；torch.export 的功能化把原地写改写为显式输出。ONNX 的算子覆盖度由 opset 版本决定，超出该 opset 的算子导出即失败。
3. 区分根因与放大因素，例如 trace 未覆盖控制流分支导致逻辑被固化到单一分支 是否为触发条件。
4. 给出止损、恢复与防复发措施，并用 导出成功率、eager 与导出输出误差 验证。

#### 3. 具体答案
面试中可以这样回答：

> 导出把可变的 Python 模块冻结成只含 aten 算子的静态图，并用水印式签名声明输入输出契约；正确性等于导出图在目标 runtime 与 eager 数值一致，兼容性等于签名与 opset 能跨版本被消费。
>
> 容量关系：导出图是函数式的：无 Python 副作用，输入输出由 graph_signature 声明；torch.export 的功能化把原地写改写为显式输出。ONNX 的算子覆盖度由 opset 版本决定，超出该 opset 的算子导出即失败。
>
> 可验证事实：torch.export 需要示例输入，返回含 graph_module、range_constraints 与 signature 的 ExportedProgram；torch.jit.trace 只记录执行过的算子，遇控制流会固化当时走的分支。

#### 4. 技术洞察
事故的根因通常是容量或契约被突破：导出图是函数式的：无 Python 副作用，输入输出由 graph_signature 声明；torch.export 的功能化把原地写改写为显式输出。ONNX 的算子覆盖度由 opset 版本决定，超出该 opset 的算子导出即失败。 被打破时表现为 dynamic shape 未显式声明使其被编译成常量；止损优先于根因，证据留存优先于重试。

#### 5. 图文说明
| 层次 | 图导出、图签名与跨版本加载 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 模型、示例输入、dynamic 声明与目标 opset/runtime | 控制流未覆盖、动态维度缺声明 |
| 执行与状态 | 追踪图捕获、功能化改写、签名与序列化 | 分支固化、opset 缺算子、加载执行任意代码 |
| 输出与 oracle | 对比导出/eager 输出、校验签名与 checker | 数值偏差、改变维度即失效、导出被拒绝 |

图示解释：把 图导出、图签名与跨版本加载 的负载、容量与错误率画成时间线，标出越界点与止损动作，区分触发与放大因素。

#### 6. 拓展知识
相关诊断工具：torch.export/exported.run_decompositions、torch.onnx.export；对比扩容、降级与回滚的成本，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 事故是容量不足、性能退化还是正确性问题，如何用 torch.export/exported.run_decompositions 在 5 分钟内区分？
2. 止损应选择限流、降级还是回滚，依据哪个 导出成功率 阈值？
3. 如何证明修复有效而非负载自然回落，需要多长时间窗与对照？

#### 8. 易错点与反例
1. 反例：先重试后查因，放大 trace 未覆盖控制流分支导致逻辑被固化到单一分支 并丢失现场。
2. 反例：只扩容量不找根因，遇到 opset 过低缺失算子导致导出失败 后再次复发。
3. 反例：用平均吞吐判断恢复，忽略 opset 过低缺失算子导致导出失败 的尾延迟仍在恶化。

#### 9. 实践与验证
验证方案：对同一模型分别 torch.export 与 jit.trace，跑固定输入比对输出；改变一个输入维度检验 dynamic_shapes 是否被尊重；用 onnx.checker 校验导出图。 在受控负载下复现并采集 导出成功率、图算子数、eager 与导出输出误差 的时间序列；oracle：导出输出与 eager 最大差 < 1e-5，声明为 dynamic 的维度改变时无需重导。

#### 10. 评分标准
E080 评分：
- 不合格：直接给方案，无现场固定与容量分析；
- 合格：能定位一个瓶颈并给出止损动作；
- 良好：能区分触发/放大因素，给出容量模型与防复发措施；
- 优秀/专家：能补充成本权衡、灰度回滚、监控门禁与残余风险。
### E081. 生产事故：自定义算子

- 难度：专家级
- 知识域：K11 自定义算子
- 考察能力：接口、状态机、隔离、灰度、回滚与治理
- 题型：系统设计/工程落地
- 面试频率：开放题
- 前置知识：schema、dispatcher、stream、autograd、ABI
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

生产事故视角关注 算子注册、调度与数值正确性 的性能、容量与成本越界：吞吐下降、显存 OOM、通信拥塞或延迟长尾。排查要先固定现场（版本、配置、负载）再定位，避免把 torch.library/TORCH_LIBRARY 注册 与 autograd.Function 与双反向 的偶发现象当根因。

#### 2. 问题分析
1. 固定事故现场：版本、配置、负载画像、时间窗口与错误类别。
2. 用容量/性能模型定位瓶颈：CUDA kernel 启动取 blocks=⌈N/threads⌉、threads≤1024 且为 32 的倍数；自定义算子若参与训练必须同时提供 forward 与 backward（或注册 autograd 公式），否则梯度断链；访问需满足 0≤idx<N 防越界。
3. 区分根因与放大因素，例如 只写 forward 未注册 backward 使梯度静默为零 是否为触发条件。
4. 给出止损、恢复与防复发措施，并用 相对误差、compute-sanitizer 命中数 验证。

#### 3. 具体答案
面试中可以这样回答：

> 自定义算子是 dispatcher 上的一个实现对：前向 kernel 负责数值、反向公式负责梯度；正确性由边界条件、设备守卫与 gradcheck 共同保证。
>
> 容量关系：CUDA kernel 启动取 blocks=⌈N/threads⌉、threads≤1024 且为 32 的倍数；自定义算子若参与训练必须同时提供 forward 与 backward（或注册 autograd 公式），否则梯度断链；访问需满足 0≤idx<N 防越界。
>
> 可验证事实：torch.utils.cpp_extension.load/load_inline 首次调用时编译并缓存扩展；缺少 TORCH_CHECK 的越界访问可能不报错而返回垃圾数据或破坏相邻内存。

#### 4. 技术洞察
事故的根因通常是容量或契约被突破：CUDA kernel 启动取 blocks=⌈N/threads⌉、threads≤1024 且为 32 的倍数；自定义算子若参与训练必须同时提供 forward 与 backward（或注册 autograd 公式），否则梯度断链；访问需满足 0≤idx<N 防越界。 被打破时表现为 kernel 越界写破坏相邻显存造成随机错误；止损优先于根因，证据留存优先于重试。

#### 5. 图文说明
| 层次 | 算子注册、调度与数值正确性 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 张量 shape/dtype/device、线程与 grid 配置、边界条件 | 越界、设备不匹配、dtype 未分派 |
| 执行与状态 | 追踪 kernel 启动、CUDAGuard、dispatcher 分派与归约 | 越界写、跑错设备、梯度断链 |
| 输出与 oracle | 对比参考实现、gradcheck 与 sanitizer 报告 | 误差超标、梯度为零、随机错误 |

图示解释：把 算子注册、调度与数值正确性 的负载、容量与错误率画成时间线，标出越界点与止损动作，区分触发与放大因素。

#### 6. 拓展知识
相关诊断工具：torch.utils.cpp_extension.load/load_inline、AT_DISPATCH_FLOATING_TYPES；对比扩容、降级与回滚的成本，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 事故是容量不足、性能退化还是正确性问题，如何用 torch.utils.cpp_extension.load/load_inline 在 5 分钟内区分？
2. 止损应选择限流、降级还是回滚，依据哪个 相对误差 阈值？
3. 如何证明修复有效而非负载自然回落，需要多长时间窗与对照？

#### 8. 易错点与反例
1. 反例：先重试后查因，放大 只写 forward 未注册 backward 使梯度静默为零 并丢失现场。
2. 反例：只扩容量不找根因，遇到 缺少 CUDAGuard 在多卡环境跑错设备 后再次复发。
3. 反例：用平均吞吐判断恢复，忽略 缺少 CUDAGuard 在多卡环境跑错设备 的尾延迟仍在恶化。

#### 9. 实践与验证
验证方案：实现逐元素算子并与 eager 参考比较，用 gradcheck 校验反向；把 N 设为非 warp 倍数检验尾块处理，跑 compute-sanitizer。 在受控负载下复现并采集 相对误差、kernel 耗时、compute-sanitizer 命中数 的时间序列；oracle：fp32 最大绝对误差 < 1e-6、gradcheck 通过，N=线程数±1 时无越界且结果正确。

#### 10. 评分标准
E081 评分：
- 不合格：直接给方案，无现场固定与容量分析；
- 合格：能定位一个瓶颈并给出止损动作；
- 良好：能区分触发/放大因素，给出容量模型与防复发措施；
- 优秀/专家：能补充成本权衡、灰度回滚、监控门禁与残余风险。
### E082. 生产事故：测试治理

- 难度：专家级
- 知识域：K12 测试治理
- 考察能力：接口、状态机、隔离、灰度、回滚与治理
- 题型：系统设计/工程落地
- 面试频率：开放题
- 前置知识：oracle、证据、日志、回滚、门禁、复盘
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

生产事故视角关注 测试契约、容差与证据化调试 的性能、容量与成本越界：吞吐下降、显存 OOM、通信拥塞或延迟长尾。排查要先固定现场（版本、配置、负载）再定位，避免把 pytest 参数化与 fixture 与 assert_close 的 rtol/atol 的偶发现象当根因。

#### 2. 问题分析
1. 固定事故现场：版本、配置、负载画像、时间窗口与错误类别。
2. 用容量/性能模型定位瓶颈：torch.testing.assert_close 在 |a−b| ≤ atol + rtol·|b| 时通过，float32 默认 rtol≈1.3e-6、atol≈1e-5；flaky 率 = 失败重跑数/总重跑数，需按阈值门禁。
3. 区分根因与放大因素，例如 容差过宽掩盖真实数值回归 是否为触发条件。
4. 给出止损、恢复与防复发措施，并用 通过率、覆盖率 验证。

#### 3. 具体答案
面试中可以这样回答：

> 测试是把需求写成可执行的契约；有效的门禁需要确定性、隔离、明确容差和可定位失败的证据，而不是只断言最终数值。
>
> 容量关系：torch.testing.assert_close 在 |a−b| ≤ atol + rtol·|b| 时通过，float32 默认 rtol≈1.3e-6、atol≈1e-5；flaky 率 = 失败重跑数/总重跑数，需按阈值门禁。
>
> 可验证事实：pytest -k 按名筛选、-x 首次失败即停、--reruns 需 pytest-rerunfailures 插件；未设种子的随机测试在 CI 中会产生不可复现失败。

#### 4. 技术洞察
事故的根因通常是容量或契约被突破：torch.testing.assert_close 在 |a−b| ≤ atol + rtol·|b| 时通过，float32 默认 rtol≈1.3e-6、atol≈1e-5；flaky 率 = 失败重跑数/总重跑数，需按阈值门禁。 被打破时表现为 测试间共享全局状态导致顺序相关失败；止损优先于根因，证据留存优先于重试。

#### 5. 图文说明
| 层次 | 测试契约、容差与证据化调试 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 被测模块、参数组合、种子、容差与基线 | 容差过宽、随机未固定、基线过期 |
| 执行与状态 | 追踪 fixture 生命周期、全局状态与断言点 | 顺序相关、静默 skip、只测终值 |
| 输出与 oracle | 核对断言、失败可定位性与 flaky 统计 | 假绿、不可复现失败、回归漏检 |

图示解释：把 测试契约、容差与证据化调试 的负载、容量与错误率画成时间线，标出越界点与止损动作，区分触发与放大因素。

#### 6. 拓展知识
相关诊断工具：pytest/parametrize/fixture、torch.testing.assert_close；对比扩容、降级与回滚的成本，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 事故是容量不足、性能退化还是正确性问题，如何用 pytest/parametrize/fixture 在 5 分钟内区分？
2. 止损应选择限流、降级还是回滚，依据哪个 通过率 阈值？
3. 如何证明修复有效而非负载自然回落，需要多长时间窗与对照？

#### 8. 易错点与反例
1. 反例：先重试后查因，放大 容差过宽掩盖真实数值回归 并丢失现场。
2. 反例：只扩容量不找根因，遇到 无 GPU 时静默 skip 使 CI 假绿 后再次复发。
3. 反例：用平均吞吐判断恢复，忽略 无 GPU 时静默 skip 使 CI 假绿 的尾延迟仍在恶化。

#### 9. 实践与验证
验证方案：为某模块写参数化测试（多 shape/dtype），固定种子并用加严容差的 assert_close；注入一个已知 bug 验证测试能失败。 在受控负载下复现并采集 通过率、flaky 率、覆盖率 的时间序列；oracle：测试在正确实现下稳定通过、在注入 bug 下必然失败，多次运行 flaky 率为 0。

#### 10. 评分标准
E082 评分：
- 不合格：直接给方案，无现场固定与容量分析；
- 合格：能定位一个瓶颈并给出止损动作；
- 良好：能区分触发/放大因素，给出容量模型与防复发措施；
- 优秀/专家：能补充成本权衡、灰度回滚、监控门禁与残余风险。
### E083. 治理门禁：张量/设备

- 难度：专家级
- 知识域：K01 张量/设备
- 考察能力：接口、状态机、隔离、灰度、回滚与治理
- 题型：系统设计/工程落地
- 面试频率：开放题
- 前置知识：shape、stride、dtype、device
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

治理门禁把 张量布局、视图语义与设备放置 的经验固化为可执行的准入规则：谁在什么条件下可以合并、发布或扩容。门禁必须可自动化、可审计，覆盖 stride 与 storage_offset 构成的地址映射、view/reshape/contiguous 的复制边界 等关键面，而不是依赖人工记忆。

#### 2. 问题分析
1. 列出必须守住的不变量与对应可自动化的检查项。
2. 把不变量写成阈值：对连续张量有 stride[i]=∏_{j>i} shape[j]；view 只在目标 shape 能由既有 stride 整除分解时成立，否则 reshape 触发复制；实数占用满足 numel·itemsize ≤ storage.nbytes − storage_offset·itemsize。
3. 设计门禁的执行点（CI、发布、运行时）与失败处置（阻断/告警/回滚）。
4. 定义豁免与升级路径，避免门禁被绕过而 对非连续张量调用 view 抛 RuntimeError: view size is not compatible with input tensor's size and stride。

#### 3. 具体答案
面试中可以这样回答：

> 张量的本质是 (storage, size, stride, storage_offset) 四元组对一段连续内存的解释；视图与副本的区别决定是否共享存储、是否可安全原地写。设备放置是显式拓扑决策，.to() 按需搬运并可能触发隐式同步。
>
> 门禁不变量：对连续张量有 stride[i]=∏_{j>i} shape[j]；view 只在目标 shape 能由既有 stride 整除分解时成立，否则 reshape 触发复制；实数占用满足 numel·itemsize ≤ storage.nbytes − storage_offset·itemsize。
>
> 可验证事实：torch.Tensor.transpose 返回共享存储的视图（只互换 stride，不影响 storage），而 .reshape 在不可视图化时返回副本，可用 data_ptr() 是否相同判定。

#### 4. 技术洞察
有效门禁 = 可自动化判据 + 明确责任 + 可回滚动作：对连续张量有 stride[i]=∏_{j>i} shape[j]；view 只在目标 shape 能由既有 stride 整除分解时成立，否则 reshape 触发复制；实数占用满足 numel·itemsize ≤ storage.nbytes − storage_offset·itemsize。 提供判据，有效带宽 GB/s 提供阈值，缺一不可。

#### 5. 图文说明
| 层次 | 张量布局、视图语义与设备放置 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 张量、目标 shape/stride、device/dtype 与 memory_format | 约束缺失会误判视图可行性或越界读 |
| 执行与状态 | 追踪 storage、stride、offset 与 allocator 块的 owner 和生命周期 | 共享存储被原地写、碎片泄漏或隐式同步 |
| 输出与 oracle | 对照 data_ptr、is_contiguous 与显存曲线 | 把副本误当视图、峰值显存与理论值严重偏离 |

图示解释：把 张量布局、视图语义与设备放置 的准入流程画成 CI→发布→运行时的门禁链，每级绑定不变量与失败处置，观察是否有绕过路径。

#### 6. 拓展知识
门禁支撑：tensor.stride()/is_contiguous()/storage_offset()/data_ptr()、torch.cuda.memory_summary()/memory_snapshot() 与基线数据；比较强门禁与快节奏发布的成本，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 该门禁在 CI 还是运行时执行，失败时阻断发布还是自动回滚，依据什么？
2. 如何防止门禁被紧急豁免长期绕过，豁免的审计与到期如何设计？
3. 门禁阈值如何随规模演进，误报与漏报如何权衡？

#### 8. 易错点与反例
1. 反例：门禁只告警不阻断，长期被人忽略，直到 对非连续张量调用 view 抛 RuntimeError: view size is not compatible with input tensor's size and stride。
2. 反例：门禁依赖人工判断不可自动执行，遇到 在未同步的跨界 .to() 后立即读数据得到脏值 时失效。
3. 反例：豁免无到期与审计，形成永久后门并放大 pin_memory 张量长期驻留耗尽锁页内存。

#### 9. 实践与验证
验证方案：对同一逻辑张量构造 contiguous、转置、expand 三种布局，断言 is_contiguous/stride/storage_offset，并用 data_ptr 判定 reshape 是否复制。 把判据接入 CI 并在注入缺陷（对非连续张量调用 view 抛 RuntimeError: view size is not compatible with input tensor's size and stride）时验证门禁能阻断；oracle：data_ptr 相同判为视图、不同判为副本；峰值显存与理论 numel·itemsize 的偏差落在分配器块对齐范围内。

#### 10. 评分标准
E083 评分：
- 不合格：只讲原则，没有可执行判据；
- 合格：能给出一个自动化检查与失败处置；
- 良好：能设计完整的门禁链、豁免与到期机制；
- 优秀/专家：能补充误报/漏报权衡、阈值演进与被绕过的防护。
### E084. 治理门禁：autograd

- 难度：专家级
- 知识域：K02 autograd
- 考察能力：接口、状态机、隔离、灰度、回滚与治理
- 题型：系统设计/工程落地
- 面试频率：开放题
- 前置知识：requires_grad、leaf、view、detach、反向
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

治理门禁把 动态计算图、梯度累积与高阶微分 的经验固化为可执行的准入规则：谁在什么条件下可以合并、发布或扩容。门禁必须可自动化、可审计，覆盖 动态图与 grad_fn 链、叶张量与 .grad 累加 等关键面，而不是依赖人工记忆。

#### 2. 问题分析
1. 列出必须守住的不变量与对应可自动化的检查项。
2. 把不变量写成阈值：反向传播沿 grad_fn 链执行链式法则 g_i=Σ_j g_j·(∂f_j/∂x_i)，梯度以累加方式写入 .grad（不清零则叠加）；对需要梯度的张量做 in-place 会使其 version counter 加一，若该版本被 backward 引用则抛 RuntimeError: a variable needed for gradient computation has been modified by an inplace operation。
3. 设计门禁的执行点（CI、发布、运行时）与失败处置（阻断/告警/回滚）。
4. 定义豁免与升级路径，避免门禁被绕过而 detach 后仍期望梯度回传导致叶张量 .grad 为 None。

#### 3. 具体答案
面试中可以这样回答：

> autograd 记录前向操作序列构成的动态图，反向时按逆序调用 grad_fn，用链式法则把上游梯度乘上局部雅可比后累加到叶张量。图的生命周期由 retain_graph 控制，原地写通过 version counter 保证数值一致。
>
> 门禁不变量：反向传播沿 grad_fn 链执行链式法则 g_i=Σ_j g_j·(∂f_j/∂x_i)，梯度以累加方式写入 .grad（不清零则叠加）；对需要梯度的张量做 in-place 会使其 version counter 加一，若该版本被 backward 引用则抛 RuntimeError: a variable needed for gradient computation has been modified by an inplace operation。
>
> 可验证事实：torch.autograd.gradcheck 在 float64 下用有限差分（默认 eps=1e-6、rtol=1.3e-6、atol=1e-5）校验解析梯度，要求输入 requires_grad=True 且函数确定性。

#### 4. 技术洞察
有效门禁 = 可自动化判据 + 明确责任 + 可回滚动作：反向传播沿 grad_fn 链执行链式法则 g_i=Σ_j g_j·(∂f_j/∂x_i)，梯度以累加方式写入 .grad（不清零则叠加）；对需要梯度的张量做 in-place 会使其 version counter 加一，若该版本被 backward 引用则抛 RuntimeError: a variable needed for gradient computation has been modified by an inplace operation。 提供判据，梯度相对误差 提供阈值，缺一不可。

#### 5. 图文说明
| 层次 | 动态计算图、梯度累积与高阶微分 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | requires_grad 状态、叶张量标志、是否原地写 | 误把非叶张量当叶、图已释放 |
| 执行与状态 | 遍历 grad_fn 链、version counter、saved tensor 所有权 | 图重复释放、原地写破坏反向、梯度断链 |
| 输出与 oracle | 比对解析梯度与数值差分、.grad 累加结果 | 梯度为零/None、误差超容差、二阶导缺失 |

图示解释：把 动态计算图、梯度累积与高阶微分 的准入流程画成 CI→发布→运行时的门禁链，每级绑定不变量与失败处置，观察是否有绕过路径。

#### 6. 拓展知识
门禁支撑：tensor.grad_fn 与 next_functions 遍历、torch.autograd.anomaly_mode/set_detect_anomaly 与基线数据；比较强门禁与快节奏发布的成本，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 该门禁在 CI 还是运行时执行，失败时阻断发布还是自动回滚，依据什么？
2. 如何防止门禁被紧急豁免长期绕过，豁免的审计与到期如何设计？
3. 门禁阈值如何随规模演进，误报与漏报如何权衡？

#### 8. 易错点与反例
1. 反例：门禁只告警不阻断，长期被人忽略，直到 detach 后仍期望梯度回传导致叶张量 .grad 为 None。
2. 反例：门禁依赖人工判断不可自动执行，遇到 第二次 backward 未设 retain_graph=True 触发 Trying to backward through the graph a second time 时失效。
3. 反例：豁免无到期与审计，形成永久后门并放大 对参与反向的张量原地写触发 version counter 报错。

#### 9. 实践与验证
验证方案：构造 y=x·sin(x)，用 torch.autograd.grad 求一阶导并与有限差分对比；再开 create_graph=True 求二阶导，验证与解析式 cos(x)−x·sin(x) 一致。 把判据接入 CI 并在注入缺陷（detach 后仍期望梯度回传导致叶张量 .grad 为 None）时验证门禁能阻断；oracle：gradcheck 返回 True（误差在 rtol=1e-3/atol=1e-5 内），二阶导相对误差 < 1e-4。

#### 10. 评分标准
E084 评分：
- 不合格：只讲原则，没有可执行判据；
- 合格：能给出一个自动化检查与失败处置；
- 良好：能设计完整的门禁链、豁免与到期机制；
- 优秀/专家：能补充误报/漏报权衡、阈值演进与被绕过的防护。
### E085. 治理门禁：Module/状态

- 难度：专家级
- 知识域：K03 Module/状态
- 考察能力：接口、状态机、隔离、灰度、回滚与治理
- 题型：系统设计/工程落地
- 面试频率：开放题
- 前置知识：参数、buffer、模式和 state_dict
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

治理门禁把 模块树、参数字典与状态持久化 的经验固化为可执行的准入规则：谁在什么条件下可以合并、发布或扩容。门禁必须可自动化、可审计，覆盖 Module 树与命名参数、parameter 与 buffer 的区别 等关键面，而不是依赖人工记忆。

#### 2. 问题分析
1. 列出必须守住的不变量与对应可自动化的检查项。
2. 把不变量写成阈值：state_dict 的键等于模块路径拼接参数/缓冲区名（如 layer1.0.conv.weight）；load_state_dict(strict=True) 要求键集合完全一致，否则报 missing/unexpected keys；buffer 不参与 optimizer 更新但会被 .to() 搬运。
3. 设计门禁的执行点（CI、发布、运行时）与失败处置（阻断/告警/回滚）。
4. 定义豁免与升级路径，避免门禁被绕过而 buffer 未 register_buffer 导致不随 .to() 和 state_dict 迁移。

#### 3. 具体答案
面试中可以这样回答：

> nn.Module 是一棵注册树，参数与缓冲区经命名空间形成扁平键空间；state_dict 是这棵树的快照契约，.to() 递归搬运所有注册张量。模式切换与注册完整性共同决定训练/推理一致性。
>
> 门禁不变量：state_dict 的键等于模块路径拼接参数/缓冲区名（如 layer1.0.conv.weight）；load_state_dict(strict=True) 要求键集合完全一致，否则报 missing/unexpected keys；buffer 不参与 optimizer 更新但会被 .to() 搬运。
>
> 可验证事实：model.eval() 只切换 dropout/BatchNorm 等模块的 training 标志，不关闭 autograd；省显存必须叠加 torch.no_grad() 或 inference_mode()。

#### 4. 技术洞察
有效门禁 = 可自动化判据 + 明确责任 + 可回滚动作：state_dict 的键等于模块路径拼接参数/缓冲区名（如 layer1.0.conv.weight）；load_state_dict(strict=True) 要求键集合完全一致，否则报 missing/unexpected keys；buffer 不参与 optimizer 更新但会被 .to() 搬运。 提供判据，参数量 提供阈值，缺一不可。

#### 5. 图文说明
| 层次 | 模块树、参数字典与状态持久化 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 模块结构、注册的参数/缓冲区、目标 device/dtype | 漏注册 buffer 或键空间不一致 |
| 执行与状态 | 遍历模块树、加载/保存快照、切换 training 标志 | 键缺失被静默、模式与图不匹配、别名错乱 |
| 输出与 oracle | 逐张量比对 state_dict、核对模式相关的统计 | 权重随机初始化、BN 统计漂移、显存未释放 |

图示解释：把 模块树、参数字典与状态持久化 的准入流程画成 CI→发布→运行时的门禁链，每级绑定不变量与失败处置，观察是否有绕过路径。

#### 6. 拓展知识
门禁支撑：named_parameters()/named_buffers()/recurse、state_dict()/load_state_dict() 与基线数据；比较强门禁与快节奏发布的成本，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 该门禁在 CI 还是运行时执行，失败时阻断发布还是自动回滚，依据什么？
2. 如何防止门禁被紧急豁免长期绕过，豁免的审计与到期如何设计？
3. 门禁阈值如何随规模演进，误报与漏报如何权衡？

#### 8. 易错点与反例
1. 反例：门禁只告警不阻断，长期被人忽略，直到 buffer 未 register_buffer 导致不随 .to() 和 state_dict 迁移。
2. 反例：门禁依赖人工判断不可自动执行，遇到 共享参数被重复注册改变键空间与加载行为 时失效。
3. 反例：豁免无到期与审计，形成永久后门并放大 strict=False 静默吞掉缺失键使部分权重随机初始化。

#### 9. 实践与验证
验证方案：构造含卷积、BN、自定义 buffer 的模块，round-trip state_dict 到新实例并 assert_close 全部张量；删掉一个 buffer 验证 strict=True 报 missing keys。 把判据接入 CI 并在注入缺陷（buffer 未 register_buffer 导致不随 .to() 和 state_dict 迁移）时验证门禁能阻断；oracle：load_state_dict 返回 missing=[]、unexpected=[]；train 模式两次 forward 后 BN running_mean 发生变化。

#### 10. 评分标准
E085 评分：
- 不合格：只讲原则，没有可执行判据；
- 合格：能给出一个自动化检查与失败处置；
- 良好：能设计完整的门禁链、豁免与到期机制；
- 优秀/专家：能补充误报/漏报权衡、阈值演进与被绕过的防护。
### E086. 治理门禁：数据管道

- 难度：专家级
- 知识域：K04 数据管道
- 考察能力：接口、状态机、隔离、灰度、回滚与治理
- 题型：系统设计/工程落地
- 面试频率：开放题
- 前置知识：Dataset、Sampler、collate、padding、版本
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

治理门禁把 DataLoader 的并行、采样与预取 的经验固化为可执行的准入规则：谁在什么条件下可以合并、发布或扩容。门禁必须可自动化、可审计，覆盖 map-style 与 iterable-style Dataset、num_workers 与进程启动方式 等关键面，而不是依赖人工记忆。

#### 2. 问题分析
1. 列出必须守住的不变量与对应可自动化的检查项。
2. 把不变量写成阈值：map-style 数据集每 epoch 批数 = ⌈N/batch_size⌉（drop_last 时为 ⌊N/batch_size⌋）；worker 是独立进程并各自持有 RNG，随机变换必须经 worker_init_fn 播种才可复现。
3. 设计门禁的执行点（CI、发布、运行时）与失败处置（阻断/告警/回滚）。
4. 定义豁免与升级路径，避免门禁被绕过而 在 Dataset.__init__ 或 __getitem__ 中初始化 CUDA 导致 fork 后 worker 崩溃。

#### 3. 具体答案
面试中可以这样回答：

> 数据管道把采样、取数、批处理和预取解耦到多 worker 进程；正确性取决于索引覆盖与随机性可复现，性能取决于预取深度与 H2D 重叠。
>
> 门禁不变量：map-style 数据集每 epoch 批数 = ⌈N/batch_size⌉（drop_last 时为 ⌊N/batch_size⌋）；worker 是独立进程并各自持有 RNG，随机变换必须经 worker_init_fn 播种才可复现。
>
> 可验证事实：pin_memory=True 配合 non_blocking=True 能把 H2D 拷贝与计算重叠；persistent_workers=True 在 epoch 间保留 worker 进程，避免反复 fork 开销。

#### 4. 技术洞察
有效门禁 = 可自动化判据 + 明确责任 + 可回滚动作：map-style 数据集每 epoch 批数 = ⌈N/batch_size⌉（drop_last 时为 ⌊N/batch_size⌋）；worker 是独立进程并各自持有 RNG，随机变换必须经 worker_init_fn 播种才可复现。 提供判据，吞吐 samples/s 提供阈值，缺一不可。

#### 5. 图文说明
| 层次 | DataLoader 的并行、采样与预取 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 数据集大小、batch_size、worker 数、采样方式 | 索引不覆盖、随机性不可复现 |
| 执行与状态 | 追踪 worker 进程、预取队列、pin 缓冲与 sampler 状态 | CUDA-in-fork 崩溃、句柄泄漏、顺序错乱 |
| 输出与 oracle | 核对样本集合、shape、吞吐与首 batch 延迟 | 重复/丢失样本、collate 形状错误、吞吐饱和异常 |

图示解释：把 DataLoader 的并行、采样与预取 的准入流程画成 CI→发布→运行时的门禁链，每级绑定不变量与失败处置，观察是否有绕过路径。

#### 6. 拓展知识
门禁支撑：DataLoader/worker_init_fn/generator、torch.utils.data.get_worker_info 与基线数据；比较强门禁与快节奏发布的成本，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 该门禁在 CI 还是运行时执行，失败时阻断发布还是自动回滚，依据什么？
2. 如何防止门禁被紧急豁免长期绕过，豁免的审计与到期如何设计？
3. 门禁阈值如何随规模演进，误报与漏报如何权衡？

#### 8. 易错点与反例
1. 反例：门禁只告警不阻断，长期被人忽略，直到 在 Dataset.__init__ 或 __getitem__ 中初始化 CUDA 导致 fork 后 worker 崩溃。
2. 反例：门禁依赖人工判断不可自动执行，遇到 worker 数过大引发内存或文件句柄耗尽 时失效。
3. 反例：豁免无到期与审计，形成永久后门并放大 collate_fn 对不定长序列堆叠得到错误 shape。

#### 9. 实践与验证
验证方案：固定 seed 与 worker_init_fn，连续两 epoch 记录样本索引序列验证覆盖完整且可复现；测量 num_workers∈{0,2,4,8} 的吞吐拐点。 把判据接入 CI 并在注入缺陷（在 Dataset.__init__ 或 __getitem__ 中初始化 CUDA 导致 fork 后 worker 崩溃）时验证门禁能阻断；oracle：两轮索引并集等于 0..N−1 且无重复，吞吐在某一 worker 数后饱和。

#### 10. 评分标准
E086 评分：
- 不合格：只讲原则，没有可执行判据；
- 合格：能给出一个自动化检查与失败处置；
- 良好：能设计完整的门禁链、豁免与到期机制；
- 优秀/专家：能补充误报/漏报权衡、阈值演进与被绕过的防护。
### E087. 治理门禁：训练/优化器

- 难度：专家级
- 知识域：K05 训练/优化器
- 考察能力：接口、状态机、隔离、灰度、回滚与治理
- 题型：系统设计/工程落地
- 面试频率：开放题
- 前置知识：forward、backward、累积、optimizer、scheduler
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

治理门禁把 优化器更新、学习率调度与梯度处理 的经验固化为可执行的准入规则：谁在什么条件下可以合并、发布或扩容。门禁必须可自动化、可审计，覆盖 优化器状态（Adam 的 m/v）、zero_grad 语义 等关键面，而不是依赖人工记忆。

#### 2. 问题分析
1. 列出必须守住的不变量与对应可自动化的检查项。
2. 把不变量写成阈值：Adam：m_t=β1·m_{t−1}+(1−β1)·g_t，v_t=β2·v_{t−1}+(1−β2)·g_t²，偏差校正 m̂=m/(1−β1^t)、v̂=v/(1−β2^t)，θ←θ−lr·m̂/(√v̂+ε)；梯度累积 k 步等价于把有效批放大 k 倍，但必须对 loss 除以 k 或按 k 归约，否则梯度被放大 k 倍。
3. 设计门禁的执行点（CI、发布、运行时）与失败处置（阻断/告警/回滚）。
4. 定义豁免与升级路径，避免门禁被绕过而 忘记 zero_grad 使梯度跨 step 累加，等效 lr 漂移。

#### 3. 具体答案
面试中可以这样回答：

> 训练步是 zero_grad→forward→loss→scaled backward→unscale→clip→step→scheduler 的固定顺序；顺序错位会静默改变有效学习率或裁剪幅度。
>
> 门禁不变量：Adam：m_t=β1·m_{t−1}+(1−β1)·g_t，v_t=β2·v_{t−1}+(1−β2)·g_t²，偏差校正 m̂=m/(1−β1^t)、v̂=v/(1−β2^t)，θ←θ−lr·m̂/(√v̂+ε)；梯度累积 k 步等价于把有效批放大 k 倍，但必须对 loss 除以 k 或按 k 归约，否则梯度被放大 k 倍。
>
> 可验证事实：zero_grad(set_to_none=True) 把 .grad 置 None（自 PyTorch 2.0 起为默认行为），比置零更省内存；optimizer.state_dict() 按参数索引保存 momentum/m/v 缓冲区。

#### 4. 技术洞察
有效门禁 = 可自动化判据 + 明确责任 + 可回滚动作：Adam：m_t=β1·m_{t−1}+(1−β1)·g_t，v_t=β2·v_{t−1}+(1−β2)·g_t²，偏差校正 m̂=m/(1−β1^t)、v̂=v/(1−β2^t)，θ←θ−lr·m̂/(√v̂+ε)；梯度累积 k 步等价于把有效批放大 k 倍，但必须对 loss 除以 k 或按 k 归约，否则梯度被放大 k 倍。 提供判据，loss 曲线 提供阈值，缺一不可。

#### 5. 图文说明
| 层次 | 优化器更新、学习率调度与梯度处理 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 初始参数、优化器配置、损失缩放与累积步 k | 顺序错位、未缩放 loss |
| 执行与状态 | 追踪 .grad、优化器状态缓冲区、lr 调度历史 | 梯度残留、动量错乱、裁剪幅度被缩放 |
| 输出与 oracle | 核对 loss 单调性、参数更新与 lr 序列 | loss 震荡、等效 lr 漂移、累积实现不等价 |

图示解释：把 优化器更新、学习率调度与梯度处理 的准入流程画成 CI→发布→运行时的门禁链，每级绑定不变量与失败处置，观察是否有绕过路径。

#### 6. 拓展知识
门禁支撑：optimizer.state_dict()/param_groups、scheduler.get_last_lr() 与基线数据；比较强门禁与快节奏发布的成本，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 该门禁在 CI 还是运行时执行，失败时阻断发布还是自动回滚，依据什么？
2. 如何防止门禁被紧急豁免长期绕过，豁免的审计与到期如何设计？
3. 门禁阈值如何随规模演进，误报与漏报如何权衡？

#### 8. 易错点与反例
1. 反例：门禁只告警不阻断，长期被人忽略，直到 忘记 zero_grad 使梯度跨 step 累加，等效 lr 漂移。
2. 反例：门禁依赖人工判断不可自动执行，遇到 scheduler.step 放错位置（每 batch 而非每 epoch）改变 lr 曲线 时失效。
3. 反例：豁免无到期与审计，形成永久后门并放大 clip_grad_norm_ 在 GradScaler.unscale_ 之前调用使裁剪阈值被放大 S 倍。

#### 9. 实践与验证
验证方案：在单个 batch 上跑 200 步验证 loss 单调下降到过拟合；打印 get_last_lr 序列核对调度曲线；对同一初始点比较累积实现与放大批实现的参数更新。 把判据接入 CI 并在注入缺陷（忘记 zero_grad 使梯度跨 step 累加，等效 lr 漂移）时验证门禁能阻断；oracle：单批过拟合 loss < 0.01；累积与大批实现的参数差在 1e-6 内。

#### 10. 评分标准
E087 评分：
- 不合格：只讲原则，没有可执行判据；
- 合格：能给出一个自动化检查与失败处置；
- 良好：能设计完整的门禁链、豁免与到期机制；
- 优秀/专家：能补充误报/漏报权衡、阈值演进与被绕过的防护。
### E088. 治理门禁：AMP/恢复

- 难度：专家级
- 知识域：K06 AMP/恢复
- 考察能力：接口、状态机、隔离、灰度、回滚与治理
- 题型：系统设计/工程落地
- 面试频率：开放题
- 前置知识：autocast、scaler、快照、随机状态
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

治理门禁把 混合精度、检查点与确定性 的经验固化为可执行的准入规则：谁在什么条件下可以合并、发布或扩容。门禁必须可自动化、可审计，覆盖 autocast 的算子白名单、GradScaler 动态缩放 等关键面，而不是依赖人工记忆。

#### 2. 问题分析
1. 列出必须守住的不变量与对应可自动化的检查项。
2. 把不变量写成阈值：GradScaler 先用 scale S 放大 loss 再反向，unscale_ 后若检测到 inf/nan 则跳过该 step 并把 S 减半；fp16 最正规数约 6.1e-5、次正规至约 6e-8，小于此量级的梯度下溢，故需缩放；bf16 指数位与 fp32 同为 8 位因此不需要缩放。
3. 设计门禁的执行点（CI、发布、运行时）与失败处置（阻断/告警/回滚）。
4. 定义豁免与升级路径，避免门禁被绕过而 fp16 无 scaler 导致梯度下溢为 0 或 NaN。

#### 3. 具体答案
面试中可以这样回答：

> AMP 通过 autocast 选低精度算子、GradScaler 维持梯度动态范围；恢复的正确性是状态闭包问题——模型、优化器、缩放与随机数状态缺一不可。
>
> 门禁不变量：GradScaler 先用 scale S 放大 loss 再反向，unscale_ 后若检测到 inf/nan 则跳过该 step 并把 S 减半；fp16 最正规数约 6.1e-5、次正规至约 6e-8，小于此量级的梯度下溢，故需缩放；bf16 指数位与 fp32 同为 8 位因此不需要缩放。
>
> 可验证事实：torch.use_deterministic_algorithms(True) 会让部分算子回退到确定性实现或直接报错；cudnn.benchmark=True 的自动调优会破坏逐步可复现性。完整恢复需保存 model、optimizer、scaler、epoch、CPU 与 CUDA RNG 及 sampler 状态。

#### 4. 技术洞察
有效门禁 = 可自动化判据 + 明确责任 + 可回滚动作：GradScaler 先用 scale S 放大 loss 再反向，unscale_ 后若检测到 inf/nan 则跳过该 step 并把 S 减半；fp16 最正规数约 6.1e-5、次正规至约 6e-8，小于此量级的梯度下溢，故需缩放；bf16 指数位与 fp32 同为 8 位因此不需要缩放。 提供判据，scale factor 曲线 提供阈值，缺一不可。

#### 5. 图文说明
| 层次 | 混合精度、检查点与确定性 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | dtype、autocast 区域、scaler 配置、checkpoint 内容 | 状态不闭合、精度选择错误 |
| 执行与状态 | 追踪 scale factor、RNG、优化器状态与 nan/inf 分支 | 下溢、NaN、恢复后状态漂移 |
| 输出与 oracle | 比对续训与基线参数、scale 曲线、依赖确定性开关 | 曲线跳变、结果不可复现、梯度全零 |

图示解释：把 混合精度、检查点与确定性 的准入流程画成 CI→发布→运行时的门禁链，每级绑定不变量与失败处置，观察是否有绕过路径。

#### 6. 拓展知识
门禁支撑：torch.cuda.amp.GradScaler 与 autocast、torch.get_rng_state/get_cuda_rng_state/numpy.random.get_state 与基线数据；比较强门禁与快节奏发布的成本，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 该门禁在 CI 还是运行时执行，失败时阻断发布还是自动回滚，依据什么？
2. 如何防止门禁被紧急豁免长期绕过，豁免的审计与到期如何设计？
3. 门禁阈值如何随规模演进，误报与漏报如何权衡？

#### 8. 易错点与反例
1. 反例：门禁只告警不阻断，长期被人忽略，直到 fp16 无 scaler 导致梯度下溢为 0 或 NaN。
2. 反例：门禁依赖人工判断不可自动执行，遇到 resume 只存模型权重丢失优化器动量使曲线跳变 时失效。
3. 反例：豁免无到期与审计，形成永久后门并放大 只存 torch RNG 未存 numpy/random 状态导致数据顺序不同。

#### 9. 实践与验证
验证方案：训练 50 步保存完整 checkpoint，重启加载后再跑 50 步与不中断基线逐参数比较；fp16 下注入大梯度观测 S 是否减半。 把判据接入 CI 并在注入缺陷（fp16 无 scaler 导致梯度下溢为 0 或 NaN）时验证门禁能阻断；oracle：续训与基线参数最大差 < 1e-5（bit 级取决于确定性开关），scale factor 与跳过计数一致。

#### 10. 评分标准
E088 评分：
- 不合格：只讲原则，没有可执行判据；
- 合格：能给出一个自动化检查与失败处置；
- 良好：能设计完整的门禁链、豁免与到期机制；
- 优秀/专家：能补充误报/漏报权衡、阈值演进与被绕过的防护。
### E089. 治理门禁：分布式

- 难度：专家级
- 知识域：K07 分布式
- 考察能力：接口、状态机、隔离、灰度、回滚与治理
- 题型：系统设计/工程落地
- 面试频率：开放题
- 前置知识：rank、DDP、FSDP、collective、容错
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

治理门禁把 进程组、集合通信与梯度同步 的经验固化为可执行的准入规则：谁在什么条件下可以合并、发布或扩容。门禁必须可自动化、可审计，覆盖 进程组与 rank/world_size、all_reduce/reduce_scatter/all_gather 等关键面，而不是依赖人工记忆。

#### 2. 问题分析
1. 列出必须守住的不变量与对应可自动化的检查项。
2. 把不变量写成阈值：DDP 默认 all_reduce(sum) 后除以 world_size 得平均梯度，故全局有效批 = 本地批 × world_size × 累积步数；ring all_reduce 单步通信量 ≈ 2·(N−1)/N·S（S 为梯度字节数），大 N 时趋近 2S。
3. 设计门禁的执行点（CI、发布、运行时）与失败处置（阻断/告警/回滚）。
4. 定义豁免与升级路径，避免门禁被绕过而 各 rank 集合通信调用不匹配导致 hang 直到 watchdog 超时。

#### 3. 具体答案
面试中可以这样回答：

> 分布式训练把单机步拆成计算→梯度归约→更新三段；正确性在于所有 rank 以相同顺序进入集合通信且梯度按 world_size 归一，性能在于把通信藏进反向的尾部。
>
> 门禁不变量：DDP 默认 all_reduce(sum) 后除以 world_size 得平均梯度，故全局有效批 = 本地批 × world_size × 累积步数；ring all_reduce 单步通信量 ≈ 2·(N−1)/N·S（S 为梯度字节数），大 N 时趋近 2S。
>
> 可验证事实：NCCL 只支持 CUDA 张量、gloo 支持 CPU；DDP 默认把梯度分成约 25 MiB 的桶以便与反向计算重叠通信。

#### 4. 技术洞察
有效门禁 = 可自动化判据 + 明确责任 + 可回滚动作：DDP 默认 all_reduce(sum) 后除以 world_size 得平均梯度，故全局有效批 = 本地批 × world_size × 累积步数；ring all_reduce 单步通信量 ≈ 2·(N−1)/N·S（S 为梯度字节数），大 N 时趋近 2S。 提供判据，all_reduce 带宽 GB/s 提供阈值，缺一不可。

#### 5. 图文说明
| 层次 | 进程组、集合通信与梯度同步 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 进程数、后端、张量是否 CUDA、参与反向的参数集合 | 后端不匹配、参数未参与、集合调用不一致 |
| 执行与状态 | 追踪 rank、bucket、通信算子与反向重叠 | hang、梯度未归约、通信未重叠 |
| 输出与 oracle | 对比单卡/DDP 梯度、通信带宽与 step 方差 | 梯度差超容差、超时、吞吐不随卡数提升 |

图示解释：把 进程组、集合通信与梯度同步 的准入流程画成 CI→发布→运行时的门禁链，每级绑定不变量与失败处置，观察是否有绕过路径。

#### 6. 拓展知识
门禁支撑：torch.distributed.init_process_group/barrier、dist.all_reduce/all_gather/reduce_scatter 与基线数据；比较强门禁与快节奏发布的成本，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 该门禁在 CI 还是运行时执行，失败时阻断发布还是自动回滚，依据什么？
2. 如何防止门禁被紧急豁免长期绕过，豁免的审计与到期如何设计？
3. 门禁阈值如何随规模演进，误报与漏报如何权衡？

#### 8. 易错点与反例
1. 反例：门禁只告警不阻断，长期被人忽略，直到 各 rank 集合通信调用不匹配导致 hang 直到 watchdog 超时。
2. 反例：门禁依赖人工判断不可自动执行，遇到 未按 world_size 归一梯度使等效 lr 随卡数变化 时失效。
3. 反例：豁免无到期与审计，形成永久后门并放大 NCCL/驱动版本不一致引发通信超时或崩溃。

#### 9. 实践与验证
验证方案：用 torchrun 起 2 进程，固定初始化使单卡与 DDP 梯度可对齐，比较 all_reduce 前后梯度；人为让一个 rank 提前退出观测 NCCL watchdog 超时。 把判据接入 CI 并在注入缺陷（各 rank 集合通信调用不匹配导致 hang 直到 watchdog 超时）时验证门禁能阻断；oracle：DDP 与单卡初始梯度（除以 world_size 后）相对误差 < 1e-6；NCCL_DEBUG 显示预期的 ring/tree 算法。

#### 10. 评分标准
E089 评分：
- 不合格：只讲原则，没有可执行判据；
- 合格：能给出一个自动化检查与失败处置；
- 良好：能设计完整的门禁链、豁免与到期机制；
- 优秀/专家：能补充误报/漏报权衡、阈值演进与被绕过的防护。
### E090. 治理门禁：编译栈

- 难度：专家级
- 知识域：K08 编译栈
- 考察能力：接口、状态机、隔离、灰度、回滚与治理
- 题型：系统设计/工程落地
- 面试频率：开放题
- 前置知识：Dynamo、AOTAutograd、Inductor、guard、缓存
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

治理门禁把 Dynamo 捕获、guard 与 Inductor 代码生成 的经验固化为可执行的准入规则：谁在什么条件下可以合并、发布或扩容。门禁必须可自动化、可审计，覆盖 TorchDynamo 的字节码捕获、guard 与 specialization 等关键面，而不是依赖人工记忆。

#### 2. 问题分析
1. 列出必须守住的不变量与对应可自动化的检查项。
2. 把不变量写成阈值：一个编译单元的 guard 绑定在张量的 shape/dtype/device 与 Python 常量上；任一 guard 失效即触发重编译，默认累计重编译上限约 8 次（torch._dynamo.config.accumulated_recompile_limit）。图被 graph break 切成多个子图，未捕获区间回退 eager。
3. 设计门禁的执行点（CI、发布、运行时）与失败处置（阻断/告警/回滚）。
4. 定义豁免与升级路径，避免门禁被绕过而 在图中调用 .item()/print/依赖数据的 Python 分支触发 graph break。

#### 3. 具体答案
面试中可以这样回答：

> 编译栈把 Python 执行轨迹捕获成 FX 图，用 guard 保证图对后续输入仍有效；失效即重编译、无法捕获即断图。稳定收益来自低断点数与低重编译率而非一次加速。
>
> 门禁不变量：一个编译单元的 guard 绑定在张量的 shape/dtype/device 与 Python 常量上；任一 guard 失效即触发重编译，默认累计重编译上限约 8 次（torch._dynamo.config.accumulated_recompile_limit）。图被 graph break 切成多个子图，未捕获区间回退 eager。
>
> 可验证事实：设置 TORCH_LOGS=graph_breaks,guards,recompiles 可打印断点与重编译原因；torch._dynamo.explain(fn)(*args) 返回断点数量与原因；dynamic=True 或 torch._dynamo.mark_dynamic 可放宽形状守卫。

#### 4. 技术洞察
有效门禁 = 可自动化判据 + 明确责任 + 可回滚动作：一个编译单元的 guard 绑定在张量的 shape/dtype/device 与 Python 常量上；任一 guard 失效即触发重编译，默认累计重编译上限约 8 次（torch._dynamo.config.accumulated_recompile_limit）。图被 graph break 切成多个子图，未捕获区间回退 eager。 提供判据，重编译次数 提供阈值，缺一不可。

#### 5. 图文说明
| 层次 | Dynamo 捕获、guard 与 Inductor 代码生成 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 函数代码、示例输入、shape/dtype/device 与 dynamic 声明 | 守卫过严、断点隐藏、动态维度未声明 |
| 执行与状态 | 追踪 Dynamo 捕获、guard 集合、子图与 Inductor 缓存 | 重编译风暴、断点回退 eager、缓存未命中 |
| 输出与 oracle | 对比 compiled/eager 数值、重编译与断点计数 | 数值不一致、加速比 < 1、重编译失控 |

图示解释：把 Dynamo 捕获、guard 与 Inductor 代码生成 的准入流程画成 CI→发布→运行时的门禁链，每级绑定不变量与失败处置，观察是否有绕过路径。

#### 6. 拓展知识
门禁支撑：TORCH_LOGS=graph_breaks,guards,recompiles、torch._dynamo.explain 与基线数据；比较强门禁与快节奏发布的成本，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 该门禁在 CI 还是运行时执行，失败时阻断发布还是自动回滚，依据什么？
2. 如何防止门禁被紧急豁免长期绕过，豁免的审计与到期如何设计？
3. 门禁阈值如何随规模演进，误报与漏报如何权衡？

#### 8. 易错点与反例
1. 反例：门禁只告警不阻断，长期被人忽略，直到 在图中调用 .item()/print/依赖数据的 Python 分支触发 graph break。
2. 反例：门禁依赖人工判断不可自动执行，遇到 动态 shape 引发重编译风暴使性能倒挂 时失效。
3. 反例：豁免无到期与审计，形成永久后门并放大 别名或原地写使编译结果与 eager 数值不一致。

#### 9. 实践与验证
验证方案：对同一函数用固定 shape 与变化 shape 各跑 20 次，用 explain 统计 graph break 与重编译；对比 compiled 与 eager 输出最大差。 把判据接入 CI 并在注入缺陷（在图中调用 .item()/print/依赖数据的 Python 分支触发 graph break）时验证门禁能阻断；oracle：固定 shape 下重编译次数为 1、输出与 eager 最大差 < 1e-4；变化 shape 下开启 dynamic 后重编译显著下降。

#### 10. 评分标准
E090 评分：
- 不合格：只讲原则，没有可执行判据；
- 合格：能给出一个自动化检查与失败处置；
- 良好：能设计完整的门禁链、豁免与到期机制；
- 优秀/专家：能补充误报/漏报权衡、阈值演进与被绕过的防护。
### E091. 治理门禁：性能

- 难度：专家级
- 知识域：K09 性能
- 考察能力：不变量、二阶影响、证据缺口与演进路线
- 题型：综合开放/专家追问
- 面试频率：开放题
- 前置知识：关键路径、带宽、算力、通信、显存、统计
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

治理门禁把 性能剖析、roofline 与显存容量 的经验固化为可执行的准入规则：谁在什么条件下可以合并、发布或扩容。门禁必须可自动化、可审计，覆盖 CUDA event 计时与同步、roofline 与算术强度 等关键面，而不是依赖人工记忆。

#### 2. 问题分析
1. 列出必须守住的不变量与对应可自动化的检查项。
2. 把不变量写成阈值：算术强度 I=FLOPs/Bytes，roofline 上界 = min(峰值算力 P, I×峰值带宽 BW)，据此判定 compute-bound 或 memory-bound；CUDA event 计时为 t=elapsed_time(start,stop)，异步操作必须 torch.cuda.synchronize() 后计时才可信。
3. 设计门禁的执行点（CI、发布、运行时）与失败处置（阻断/告警/回滚）。
4. 定义豁免与升级路径，避免门禁被绕过而 异步未同步导致计时偏短，得出虚假加速。

#### 3. 具体答案
面试中可以这样回答：

> 性能结论必须绑定可复现的测量协议：warmup、同步、重复次数、分位数；再用 roofline 判定算力受限还是带宽受限，最后才谈优化手段。
>
> 门禁不变量：算术强度 I=FLOPs/Bytes，roofline 上界 = min(峰值算力 P, I×峰值带宽 BW)，据此判定 compute-bound 或 memory-bound；CUDA event 计时为 t=elapsed_time(start,stop)，异步操作必须 torch.cuda.synchronize() 后计时才可信。
>
> 可验证事实：torch.profiler.profile(activities=[ProfilerActivity.CUDA]) 会插入 CUDA 事件并输出按算子聚合的 self/total CUDA time；首次迭代含 cuBLAS/context 初始化与自动调优，必须先 warmup 再计时。

#### 4. 技术洞察
有效门禁 = 可自动化判据 + 明确责任 + 可回滚动作：算术强度 I=FLOPs/Bytes，roofline 上界 = min(峰值算力 P, I×峰值带宽 BW)，据此判定 compute-bound 或 memory-bound；CUDA event 计时为 t=elapsed_time(start,stop)，异步操作必须 torch.cuda.synchronize() 后计时才可信。 提供判据，p50/p99 延迟 提供阈值，缺一不可。

#### 5. 图文说明
| 层次 | 性能剖析、roofline 与显存容量 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 算子/规模、dtype、warmup 与 repeat、计时方式 | 未同步、未 warmup、规模误导 |
| 执行与状态 | 追踪 kernel 时长、显存曲线、launch 与同步点 | 计时偏短、碎片抖动、把 launch 开销当算力 |
| 输出与 oracle | 对比 event/profiler、roofline 定位与分位数 | 加速比不稳定、有效带宽超理论值、尾延迟超标 |

图示解释：把 性能剖析、roofline 与显存容量 的准入流程画成 CI→发布→运行时的门禁链，每级绑定不变量与失败处置，观察是否有绕过路径。

#### 6. 拓展知识
门禁支撑：torch.profiler、torch.cuda.Event/elapsed_time 与基线数据；比较强门禁与快节奏发布的成本，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 该门禁在 CI 还是运行时执行，失败时阻断发布还是自动回滚，依据什么？
2. 如何防止门禁被紧急豁免长期绕过，豁免的审计与到期如何设计？
3. 门禁阈值如何随规模演进，误报与漏报如何权衡？

#### 8. 易错点与反例
1. 反例：门禁只告警不阻断，长期被人忽略，直到 异步未同步导致计时偏短，得出虚假加速。
2. 反例：门禁依赖人工判断不可自动执行，遇到 把 profiler 自身开销计入结论 时失效。
3. 反例：豁免无到期与审计，形成永久后门并放大 只测单次忽略冷启动与首次调优。

#### 9. 实践与验证
验证方案：对 matmul、逐元素、拷贝分别用 CUDA event 与 profiler 计时，warmup=10、repeat=100、取中位数；用 bytes/time 反推有效带宽并与理论比较。 把判据接入 CI 并在注入缺陷（异步未同步导致计时偏短，得出虚假加速）时验证门禁能阻断；oracle：event 计时与 profiler 的 CUDA time 偏差 < 5%，大 size 下有效带宽落在理论峰值的可解释比例内。

#### 10. 评分标准
E091 评分：
- 不合格：只讲原则，没有可执行判据；
- 合格：能给出一个自动化检查与失败处置；
- 良好：能设计完整的门禁链、豁免与到期机制；
- 优秀/专家：能补充误报/漏报权衡、阈值演进与被绕过的防护。
### E092. 治理门禁：导出部署

- 难度：专家级
- 知识域：K10 导出部署
- 考察能力：不变量、二阶影响、证据缺口与演进路线
- 题型：综合开放/专家追问
- 面试频率：开放题
- 前置知识：state_dict、Export、runtime、schema、迁移
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

治理门禁把 图导出、图签名与跨版本加载 的经验固化为可执行的准入规则：谁在什么条件下可以合并、发布或扩容。门禁必须可自动化、可审计，覆盖 torch.export/ExportedProgram、torch.jit.trace/script 等关键面，而不是依赖人工记忆。

#### 2. 问题分析
1. 列出必须守住的不变量与对应可自动化的检查项。
2. 把不变量写成阈值：导出图是函数式的：无 Python 副作用，输入输出由 graph_signature 声明；torch.export 的功能化把原地写改写为显式输出。ONNX 的算子覆盖度由 opset 版本决定，超出该 opset 的算子导出即失败。
3. 设计门禁的执行点（CI、发布、运行时）与失败处置（阻断/告警/回滚）。
4. 定义豁免与升级路径，避免门禁被绕过而 trace 未覆盖控制流分支导致逻辑被固化到单一分支。

#### 3. 具体答案
面试中可以这样回答：

> 导出把可变的 Python 模块冻结成只含 aten 算子的静态图，并用水印式签名声明输入输出契约；正确性等于导出图在目标 runtime 与 eager 数值一致，兼容性等于签名与 opset 能跨版本被消费。
>
> 门禁不变量：导出图是函数式的：无 Python 副作用，输入输出由 graph_signature 声明；torch.export 的功能化把原地写改写为显式输出。ONNX 的算子覆盖度由 opset 版本决定，超出该 opset 的算子导出即失败。
>
> 可验证事实：torch.export 需要示例输入，返回含 graph_module、range_constraints 与 signature 的 ExportedProgram；torch.jit.trace 只记录执行过的算子，遇控制流会固化当时走的分支。

#### 4. 技术洞察
有效门禁 = 可自动化判据 + 明确责任 + 可回滚动作：导出图是函数式的：无 Python 副作用，输入输出由 graph_signature 声明；torch.export 的功能化把原地写改写为显式输出。ONNX 的算子覆盖度由 opset 版本决定，超出该 opset 的算子导出即失败。 提供判据，导出成功率 提供阈值，缺一不可。

#### 5. 图文说明
| 层次 | 图导出、图签名与跨版本加载 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 模型、示例输入、dynamic 声明与目标 opset/runtime | 控制流未覆盖、动态维度缺声明 |
| 执行与状态 | 追踪图捕获、功能化改写、签名与序列化 | 分支固化、opset 缺算子、加载执行任意代码 |
| 输出与 oracle | 对比导出/eager 输出、校验签名与 checker | 数值偏差、改变维度即失效、导出被拒绝 |

图示解释：把 图导出、图签名与跨版本加载 的准入流程画成 CI→发布→运行时的门禁链，每级绑定不变量与失败处置，观察是否有绕过路径。

#### 6. 拓展知识
门禁支撑：torch.export/exported.run_decompositions、torch.onnx.export 与基线数据；比较强门禁与快节奏发布的成本，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 该门禁在 CI 还是运行时执行，失败时阻断发布还是自动回滚，依据什么？
2. 如何防止门禁被紧急豁免长期绕过，豁免的审计与到期如何设计？
3. 门禁阈值如何随规模演进，误报与漏报如何权衡？

#### 8. 易错点与反例
1. 反例：门禁只告警不阻断，长期被人忽略，直到 trace 未覆盖控制流分支导致逻辑被固化到单一分支。
2. 反例：门禁依赖人工判断不可自动执行，遇到 dynamic shape 未显式声明使其被编译成常量 时失效。
3. 反例：豁免无到期与审计，形成永久后门并放大 opset 过低缺失算子导致导出失败。

#### 9. 实践与验证
验证方案：对同一模型分别 torch.export 与 jit.trace，跑固定输入比对输出；改变一个输入维度检验 dynamic_shapes 是否被尊重；用 onnx.checker 校验导出图。 把判据接入 CI 并在注入缺陷（trace 未覆盖控制流分支导致逻辑被固化到单一分支）时验证门禁能阻断；oracle：导出输出与 eager 最大差 < 1e-5，声明为 dynamic 的维度改变时无需重导。

#### 10. 评分标准
E092 评分：
- 不合格：只讲原则，没有可执行判据；
- 合格：能给出一个自动化检查与失败处置；
- 良好：能设计完整的门禁链、豁免与到期机制；
- 优秀/专家：能补充误报/漏报权衡、阈值演进与被绕过的防护。
### E093. 治理门禁：自定义算子

- 难度：专家级
- 知识域：K11 自定义算子
- 考察能力：不变量、二阶影响、证据缺口与演进路线
- 题型：综合开放/专家追问
- 面试频率：开放题
- 前置知识：schema、dispatcher、stream、autograd、ABI
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

治理门禁把 算子注册、调度与数值正确性 的经验固化为可执行的准入规则：谁在什么条件下可以合并、发布或扩容。门禁必须可自动化、可审计，覆盖 torch.library/TORCH_LIBRARY 注册、autograd.Function 与双反向 等关键面，而不是依赖人工记忆。

#### 2. 问题分析
1. 列出必须守住的不变量与对应可自动化的检查项。
2. 把不变量写成阈值：CUDA kernel 启动取 blocks=⌈N/threads⌉、threads≤1024 且为 32 的倍数；自定义算子若参与训练必须同时提供 forward 与 backward（或注册 autograd 公式），否则梯度断链；访问需满足 0≤idx<N 防越界。
3. 设计门禁的执行点（CI、发布、运行时）与失败处置（阻断/告警/回滚）。
4. 定义豁免与升级路径，避免门禁被绕过而 只写 forward 未注册 backward 使梯度静默为零。

#### 3. 具体答案
面试中可以这样回答：

> 自定义算子是 dispatcher 上的一个实现对：前向 kernel 负责数值、反向公式负责梯度；正确性由边界条件、设备守卫与 gradcheck 共同保证。
>
> 门禁不变量：CUDA kernel 启动取 blocks=⌈N/threads⌉、threads≤1024 且为 32 的倍数；自定义算子若参与训练必须同时提供 forward 与 backward（或注册 autograd 公式），否则梯度断链；访问需满足 0≤idx<N 防越界。
>
> 可验证事实：torch.utils.cpp_extension.load/load_inline 首次调用时编译并缓存扩展；缺少 TORCH_CHECK 的越界访问可能不报错而返回垃圾数据或破坏相邻内存。

#### 4. 技术洞察
有效门禁 = 可自动化判据 + 明确责任 + 可回滚动作：CUDA kernel 启动取 blocks=⌈N/threads⌉、threads≤1024 且为 32 的倍数；自定义算子若参与训练必须同时提供 forward 与 backward（或注册 autograd 公式），否则梯度断链；访问需满足 0≤idx<N 防越界。 提供判据，相对误差 提供阈值，缺一不可。

#### 5. 图文说明
| 层次 | 算子注册、调度与数值正确性 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 张量 shape/dtype/device、线程与 grid 配置、边界条件 | 越界、设备不匹配、dtype 未分派 |
| 执行与状态 | 追踪 kernel 启动、CUDAGuard、dispatcher 分派与归约 | 越界写、跑错设备、梯度断链 |
| 输出与 oracle | 对比参考实现、gradcheck 与 sanitizer 报告 | 误差超标、梯度为零、随机错误 |

图示解释：把 算子注册、调度与数值正确性 的准入流程画成 CI→发布→运行时的门禁链，每级绑定不变量与失败处置，观察是否有绕过路径。

#### 6. 拓展知识
门禁支撑：torch.utils.cpp_extension.load/load_inline、AT_DISPATCH_FLOATING_TYPES 与基线数据；比较强门禁与快节奏发布的成本，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 该门禁在 CI 还是运行时执行，失败时阻断发布还是自动回滚，依据什么？
2. 如何防止门禁被紧急豁免长期绕过，豁免的审计与到期如何设计？
3. 门禁阈值如何随规模演进，误报与漏报如何权衡？

#### 8. 易错点与反例
1. 反例：门禁只告警不阻断，长期被人忽略，直到 只写 forward 未注册 backward 使梯度静默为零。
2. 反例：门禁依赖人工判断不可自动执行，遇到 kernel 越界写破坏相邻显存造成随机错误 时失效。
3. 反例：豁免无到期与审计，形成永久后门并放大 缺少 CUDAGuard 在多卡环境跑错设备。

#### 9. 实践与验证
验证方案：实现逐元素算子并与 eager 参考比较，用 gradcheck 校验反向；把 N 设为非 warp 倍数检验尾块处理，跑 compute-sanitizer。 把判据接入 CI 并在注入缺陷（只写 forward 未注册 backward 使梯度静默为零）时验证门禁能阻断；oracle：fp32 最大绝对误差 < 1e-6、gradcheck 通过，N=线程数±1 时无越界且结果正确。

#### 10. 评分标准
E093 评分：
- 不合格：只讲原则，没有可执行判据；
- 合格：能给出一个自动化检查与失败处置；
- 良好：能设计完整的门禁链、豁免与到期机制；
- 优秀/专家：能补充误报/漏报权衡、阈值演进与被绕过的防护。
### E094. 治理门禁：测试治理

- 难度：专家级
- 知识域：K12 测试治理
- 考察能力：不变量、二阶影响、证据缺口与演进路线
- 题型：综合开放/专家追问
- 面试频率：开放题
- 前置知识：oracle、证据、日志、回滚、门禁、复盘
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

治理门禁把 测试契约、容差与证据化调试 的经验固化为可执行的准入规则：谁在什么条件下可以合并、发布或扩容。门禁必须可自动化、可审计，覆盖 pytest 参数化与 fixture、assert_close 的 rtol/atol 等关键面，而不是依赖人工记忆。

#### 2. 问题分析
1. 列出必须守住的不变量与对应可自动化的检查项。
2. 把不变量写成阈值：torch.testing.assert_close 在 |a−b| ≤ atol + rtol·|b| 时通过，float32 默认 rtol≈1.3e-6、atol≈1e-5；flaky 率 = 失败重跑数/总重跑数，需按阈值门禁。
3. 设计门禁的执行点（CI、发布、运行时）与失败处置（阻断/告警/回滚）。
4. 定义豁免与升级路径，避免门禁被绕过而 容差过宽掩盖真实数值回归。

#### 3. 具体答案
面试中可以这样回答：

> 测试是把需求写成可执行的契约；有效的门禁需要确定性、隔离、明确容差和可定位失败的证据，而不是只断言最终数值。
>
> 门禁不变量：torch.testing.assert_close 在 |a−b| ≤ atol + rtol·|b| 时通过，float32 默认 rtol≈1.3e-6、atol≈1e-5；flaky 率 = 失败重跑数/总重跑数，需按阈值门禁。
>
> 可验证事实：pytest -k 按名筛选、-x 首次失败即停、--reruns 需 pytest-rerunfailures 插件；未设种子的随机测试在 CI 中会产生不可复现失败。

#### 4. 技术洞察
有效门禁 = 可自动化判据 + 明确责任 + 可回滚动作：torch.testing.assert_close 在 |a−b| ≤ atol + rtol·|b| 时通过，float32 默认 rtol≈1.3e-6、atol≈1e-5；flaky 率 = 失败重跑数/总重跑数，需按阈值门禁。 提供判据，通过率 提供阈值，缺一不可。

#### 5. 图文说明
| 层次 | 测试契约、容差与证据化调试 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 被测模块、参数组合、种子、容差与基线 | 容差过宽、随机未固定、基线过期 |
| 执行与状态 | 追踪 fixture 生命周期、全局状态与断言点 | 顺序相关、静默 skip、只测终值 |
| 输出与 oracle | 核对断言、失败可定位性与 flaky 统计 | 假绿、不可复现失败、回归漏检 |

图示解释：把 测试契约、容差与证据化调试 的准入流程画成 CI→发布→运行时的门禁链，每级绑定不变量与失败处置，观察是否有绕过路径。

#### 6. 拓展知识
门禁支撑：pytest/parametrize/fixture、torch.testing.assert_close 与基线数据；比较强门禁与快节奏发布的成本，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 该门禁在 CI 还是运行时执行，失败时阻断发布还是自动回滚，依据什么？
2. 如何防止门禁被紧急豁免长期绕过，豁免的审计与到期如何设计？
3. 门禁阈值如何随规模演进，误报与漏报如何权衡？

#### 8. 易错点与反例
1. 反例：门禁只告警不阻断，长期被人忽略，直到 容差过宽掩盖真实数值回归。
2. 反例：门禁依赖人工判断不可自动执行，遇到 测试间共享全局状态导致顺序相关失败 时失效。
3. 反例：豁免无到期与审计，形成永久后门并放大 无 GPU 时静默 skip 使 CI 假绿。

#### 9. 实践与验证
验证方案：为某模块写参数化测试（多 shape/dtype），固定种子并用加严容差的 assert_close；注入一个已知 bug 验证测试能失败。 把判据接入 CI 并在注入缺陷（容差过宽掩盖真实数值回归）时验证门禁能阻断；oracle：测试在正确实现下稳定通过、在注入 bug 下必然失败，多次运行 flaky 率为 0。

#### 10. 评分标准
E094 评分：
- 不合格：只讲原则，没有可执行判据；
- 合格：能给出一个自动化检查与失败处置；
- 良好：能设计完整的门禁链、豁免与到期机制；
- 优秀/专家：能补充误报/漏报权衡、阈值演进与被绕过的防护。
### E095. 开放决策：张量/设备

- 难度：专家级
- 知识域：K01 张量/设备
- 考察能力：不变量、二阶影响、证据缺口与演进路线
- 题型：综合开放/专家追问
- 面试频率：开放题
- 前置知识：shape、stride、dtype、device
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

开放决策要求在信息不完备下对 张量布局、视图语义与设备放置 做出可辩护的选择：目标、约束、候选与不确定性都必须显式。决策不是追求最优，而是在给定风险预算下选择可逆、可观测、可回退的方案，重点关注 stride 与 storage_offset 构成的地址映射 与 memory_format 与 channels_last。

#### 2. 问题分析
1. 明确目标函数、硬约束与风险预算，列出候选方案。
2. 用第一性关系估计各方案的代价：对连续张量有 stride[i]=∏_{j>i} shape[j]；view 只在目标 shape 能由既有 stride 整除分解时成立，否则 reshape 触发复制；实数占用满足 numel·itemsize ≤ storage.nbytes − storage_offset·itemsize。
3. 识别关键不确定性与其信息价值，设计最小验证实验。
4. 选择可逆路径并设定停止/回退门槛，防止 对非连续张量调用 view 抛 RuntimeError: view size is not compatible with input tensor's size and stride。

#### 3. 具体答案
面试中可以这样回答：

> 张量的本质是 (storage, size, stride, storage_offset) 四元组对一段连续内存的解释；视图与副本的区别决定是否共享存储、是否可安全原地写。设备放置是显式拓扑决策，.to() 按需搬运并可能触发隐式同步。
>
> 决策依据：对连续张量有 stride[i]=∏_{j>i} shape[j]；view 只在目标 shape 能由既有 stride 整除分解时成立，否则 reshape 触发复制；实数占用满足 numel·itemsize ≤ storage.nbytes − storage_offset·itemsize。
>
> 可验证事实：torch.Tensor.transpose 返回共享存储的视图（只互换 stride，不影响 storage），而 .reshape 在不可视图化时返回副本，可用 data_ptr() 是否相同判定。

#### 4. 技术洞察
开放决策的质量取决于可逆性与信息价值：优先选择能低成本试错并快速回退的路径，对连续张量有 stride[i]=∏_{j>i} shape[j]；view 只在目标 shape 能由既有 stride 整除分解时成立，否则 reshape 触发复制；实数占用满足 numel·itemsize ≤ storage.nbytes − storage_offset·itemsize。 用于估算代价避免拍脑袋。

#### 5. 图文说明
| 层次 | 张量布局、视图语义与设备放置 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 张量、目标 shape/stride、device/dtype 与 memory_format | 约束缺失会误判视图可行性或越界读 |
| 执行与状态 | 追踪 storage、stride、offset 与 allocator 块的 owner 和生命周期 | 共享存储被原地写、碎片泄漏或隐式同步 |
| 输出与 oracle | 对照 data_ptr、is_contiguous 与显存曲线 | 把副本误当视图、峰值显存与理论值严重偏离 |

图示解释：把 张量布局、视图语义与设备放置 的候选方案、关键不确定性与回退门槛画成决策树，标出每步的信息收益与不可逆风险。

#### 6. 拓展知识
决策支撑：tensor.stride()/is_contiguous()/storage_offset()/data_ptr()、PYTORCH_CUDA_ALLOC_CONF 环境变量 与 A/B 或影子流量；比较各候选的迁移成本与失败恢复，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 在信息不足时先做哪个最小实验来降低不确定性，期望得到什么数据？
2. 如果关键假设被推翻，回退到哪一步、损失多大、耗时多久？
3. 如何在多目标（性能、成本、风险）间排序，权重如何随阶段变化？

#### 8. 易错点与反例
1. 反例：在不确定性最大处做不可逆投入，被 对非连续张量调用 view 抛 RuntimeError: view size is not compatible with input tensor's size and stride 直接击穿。
2. 反例：只比较理想路径成本，忽略 在未同步的跨界 .to() 后立即读数据得到脏值 的运维与恢复代价。
3. 反例：没有停止门槛，持续投入直到 pin_memory 张量长期驻留耗尽锁页内存 才被动放弃。

#### 9. 实践与验证
验证方案：对同一逻辑张量构造 contiguous、转置、expand 三种布局，断言 is_contiguous/stride/storage_offset，并用 data_ptr 判定 reshape 是否复制。 用影子流量或 A/B 验证关键假设，采集 有效带宽 GB/s 与 峰值显存 GiB；oracle：data_ptr 相同判为视图、不同判为副本；峰值显存与理论 numel·itemsize 的偏差落在分配器块对齐范围内。

#### 10. 评分标准
E095 评分：
- 不合格：只给结论，不暴露假设与约束；
- 合格：能列出候选与一个关键不确定性；
- 良好：能给出可逆路径、回退门槛与验证计划；
- 优秀/专家：能补充信息价值量化、多目标权衡、阶段化决策与残余风险。
### E096. 开放决策：autograd

- 难度：专家级
- 知识域：K02 autograd
- 考察能力：不变量、二阶影响、证据缺口与演进路线
- 题型：综合开放/专家追问
- 面试频率：开放题
- 前置知识：requires_grad、leaf、view、detach、反向
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

开放决策要求在信息不完备下对 动态计算图、梯度累积与高阶微分 做出可辩护的选择：目标、约束、候选与不确定性都必须显式。决策不是追求最优，而是在给定风险预算下选择可逆、可观测、可回退的方案，重点关注 动态图与 grad_fn 链 与 retain_graph/create_graph 生命周期。

#### 2. 问题分析
1. 明确目标函数、硬约束与风险预算，列出候选方案。
2. 用第一性关系估计各方案的代价：反向传播沿 grad_fn 链执行链式法则 g_i=Σ_j g_j·(∂f_j/∂x_i)，梯度以累加方式写入 .grad（不清零则叠加）；对需要梯度的张量做 in-place 会使其 version counter 加一，若该版本被 backward 引用则抛 RuntimeError: a variable needed for gradient computation has been modified by an inplace operation。
3. 识别关键不确定性与其信息价值，设计最小验证实验。
4. 选择可逆路径并设定停止/回退门槛，防止 detach 后仍期望梯度回传导致叶张量 .grad 为 None。

#### 3. 具体答案
面试中可以这样回答：

> autograd 记录前向操作序列构成的动态图，反向时按逆序调用 grad_fn，用链式法则把上游梯度乘上局部雅可比后累加到叶张量。图的生命周期由 retain_graph 控制，原地写通过 version counter 保证数值一致。
>
> 决策依据：反向传播沿 grad_fn 链执行链式法则 g_i=Σ_j g_j·(∂f_j/∂x_i)，梯度以累加方式写入 .grad（不清零则叠加）；对需要梯度的张量做 in-place 会使其 version counter 加一，若该版本被 backward 引用则抛 RuntimeError: a variable needed for gradient computation has been modified by an inplace operation。
>
> 可验证事实：torch.autograd.gradcheck 在 float64 下用有限差分（默认 eps=1e-6、rtol=1.3e-6、atol=1e-5）校验解析梯度，要求输入 requires_grad=True 且函数确定性。

#### 4. 技术洞察
开放决策的质量取决于可逆性与信息价值：优先选择能低成本试错并快速回退的路径，反向传播沿 grad_fn 链执行链式法则 g_i=Σ_j g_j·(∂f_j/∂x_i)，梯度以累加方式写入 .grad（不清零则叠加）；对需要梯度的张量做 in-place 会使其 version counter 加一，若该版本被 backward 引用则抛 RuntimeError: a variable needed for gradient computation has been modified by an inplace operation。 用于估算代价避免拍脑袋。

#### 5. 图文说明
| 层次 | 动态计算图、梯度累积与高阶微分 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | requires_grad 状态、叶张量标志、是否原地写 | 误把非叶张量当叶、图已释放 |
| 执行与状态 | 遍历 grad_fn 链、version counter、saved tensor 所有权 | 图重复释放、原地写破坏反向、梯度断链 |
| 输出与 oracle | 比对解析梯度与数值差分、.grad 累加结果 | 梯度为零/None、误差超容差、二阶导缺失 |

图示解释：把 动态计算图、梯度累积与高阶微分 的候选方案、关键不确定性与回退门槛画成决策树，标出每步的信息收益与不可逆风险。

#### 6. 拓展知识
决策支撑：tensor.grad_fn 与 next_functions 遍历、torch.autograd.gradcheck/gradgradcheck 与 A/B 或影子流量；比较各候选的迁移成本与失败恢复，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 在信息不足时先做哪个最小实验来降低不确定性，期望得到什么数据？
2. 如果关键假设被推翻，回退到哪一步、损失多大、耗时多久？
3. 如何在多目标（性能、成本、风险）间排序，权重如何随阶段变化？

#### 8. 易错点与反例
1. 反例：在不确定性最大处做不可逆投入，被 detach 后仍期望梯度回传导致叶张量 .grad 为 None 直接击穿。
2. 反例：只比较理想路径成本，忽略 第二次 backward 未设 retain_graph=True 触发 Trying to backward through the graph a second time 的运维与恢复代价。
3. 反例：没有停止门槛，持续投入直到 对参与反向的张量原地写触发 version counter 报错 才被动放弃。

#### 9. 实践与验证
验证方案：构造 y=x·sin(x)，用 torch.autograd.grad 求一阶导并与有限差分对比；再开 create_graph=True 求二阶导，验证与解析式 cos(x)−x·sin(x) 一致。 用影子流量或 A/B 验证关键假设，采集 梯度相对误差 与 峰值激活内存 MiB；oracle：gradcheck 返回 True（误差在 rtol=1e-3/atol=1e-5 内），二阶导相对误差 < 1e-4。

#### 10. 评分标准
E096 评分：
- 不合格：只给结论，不暴露假设与约束；
- 合格：能列出候选与一个关键不确定性；
- 良好：能给出可逆路径、回退门槛与验证计划；
- 优秀/专家：能补充信息价值量化、多目标权衡、阶段化决策与残余风险。
### E097. 开放决策：Module/状态

- 难度：专家级
- 知识域：K03 Module/状态
- 考察能力：不变量、二阶影响、证据缺口与演进路线
- 题型：综合开放/专家追问
- 面试频率：开放题
- 前置知识：参数、buffer、模式和 state_dict
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

开放决策要求在信息不完备下对 模块树、参数字典与状态持久化 做出可辩护的选择：目标、约束、候选与不确定性都必须显式。决策不是追求最优，而是在给定风险预算下选择可逆、可观测、可回退的方案，重点关注 Module 树与命名参数 与 state_dict 键空间。

#### 2. 问题分析
1. 明确目标函数、硬约束与风险预算，列出候选方案。
2. 用第一性关系估计各方案的代价：state_dict 的键等于模块路径拼接参数/缓冲区名（如 layer1.0.conv.weight）；load_state_dict(strict=True) 要求键集合完全一致，否则报 missing/unexpected keys；buffer 不参与 optimizer 更新但会被 .to() 搬运。
3. 识别关键不确定性与其信息价值，设计最小验证实验。
4. 选择可逆路径并设定停止/回退门槛，防止 buffer 未 register_buffer 导致不随 .to() 和 state_dict 迁移。

#### 3. 具体答案
面试中可以这样回答：

> nn.Module 是一棵注册树，参数与缓冲区经命名空间形成扁平键空间；state_dict 是这棵树的快照契约，.to() 递归搬运所有注册张量。模式切换与注册完整性共同决定训练/推理一致性。
>
> 决策依据：state_dict 的键等于模块路径拼接参数/缓冲区名（如 layer1.0.conv.weight）；load_state_dict(strict=True) 要求键集合完全一致，否则报 missing/unexpected keys；buffer 不参与 optimizer 更新但会被 .to() 搬运。
>
> 可验证事实：model.eval() 只切换 dropout/BatchNorm 等模块的 training 标志，不关闭 autograd；省显存必须叠加 torch.no_grad() 或 inference_mode()。

#### 4. 技术洞察
开放决策的质量取决于可逆性与信息价值：优先选择能低成本试错并快速回退的路径，state_dict 的键等于模块路径拼接参数/缓冲区名（如 layer1.0.conv.weight）；load_state_dict(strict=True) 要求键集合完全一致，否则报 missing/unexpected keys；buffer 不参与 optimizer 更新但会被 .to() 搬运。 用于估算代价避免拍脑袋。

#### 5. 图文说明
| 层次 | 模块树、参数字典与状态持久化 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 模块结构、注册的参数/缓冲区、目标 device/dtype | 漏注册 buffer 或键空间不一致 |
| 执行与状态 | 遍历模块树、加载/保存快照、切换 training 标志 | 键缺失被静默、模式与图不匹配、别名错乱 |
| 输出与 oracle | 逐张量比对 state_dict、核对模式相关的统计 | 权重随机初始化、BN 统计漂移、显存未释放 |

图示解释：把 模块树、参数字典与状态持久化 的候选方案、关键不确定性与回退门槛画成决策树，标出每步的信息收益与不可逆风险。

#### 6. 拓展知识
决策支撑：named_parameters()/named_buffers()/recurse、register_buffer/register_parameter 与 A/B 或影子流量；比较各候选的迁移成本与失败恢复，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 在信息不足时先做哪个最小实验来降低不确定性，期望得到什么数据？
2. 如果关键假设被推翻，回退到哪一步、损失多大、耗时多久？
3. 如何在多目标（性能、成本、风险）间排序，权重如何随阶段变化？

#### 8. 易错点与反例
1. 反例：在不确定性最大处做不可逆投入，被 buffer 未 register_buffer 导致不随 .to() 和 state_dict 迁移 直接击穿。
2. 反例：只比较理想路径成本，忽略 共享参数被重复注册改变键空间与加载行为 的运维与恢复代价。
3. 反例：没有停止门槛，持续投入直到 strict=False 静默吞掉缺失键使部分权重随机初始化 才被动放弃。

#### 9. 实践与验证
验证方案：构造含卷积、BN、自定义 buffer 的模块，round-trip state_dict 到新实例并 assert_close 全部张量；删掉一个 buffer 验证 strict=True 报 missing keys。 用影子流量或 A/B 验证关键假设，采集 参数量 与 键匹配率；oracle：load_state_dict 返回 missing=[]、unexpected=[]；train 模式两次 forward 后 BN running_mean 发生变化。

#### 10. 评分标准
E097 评分：
- 不合格：只给结论，不暴露假设与约束；
- 合格：能列出候选与一个关键不确定性；
- 良好：能给出可逆路径、回退门槛与验证计划；
- 优秀/专家：能补充信息价值量化、多目标权衡、阶段化决策与残余风险。
### E098. 开放决策：数据管道

- 难度：专家级
- 知识域：K04 数据管道
- 考察能力：不变量、二阶影响、证据缺口与演进路线
- 题型：综合开放/专家追问
- 面试频率：开放题
- 前置知识：Dataset、Sampler、collate、padding、版本
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

开放决策要求在信息不完备下对 DataLoader 的并行、采样与预取 做出可辩护的选择：目标、约束、候选与不确定性都必须显式。决策不是追求最优，而是在给定风险预算下选择可逆、可观测、可回退的方案，重点关注 map-style 与 iterable-style Dataset 与 sampler 与 shuffle 语义。

#### 2. 问题分析
1. 明确目标函数、硬约束与风险预算，列出候选方案。
2. 用第一性关系估计各方案的代价：map-style 数据集每 epoch 批数 = ⌈N/batch_size⌉（drop_last 时为 ⌊N/batch_size⌋）；worker 是独立进程并各自持有 RNG，随机变换必须经 worker_init_fn 播种才可复现。
3. 识别关键不确定性与其信息价值，设计最小验证实验。
4. 选择可逆路径并设定停止/回退门槛，防止 在 Dataset.__init__ 或 __getitem__ 中初始化 CUDA 导致 fork 后 worker 崩溃。

#### 3. 具体答案
面试中可以这样回答：

> 数据管道把采样、取数、批处理和预取解耦到多 worker 进程；正确性取决于索引覆盖与随机性可复现，性能取决于预取深度与 H2D 重叠。
>
> 决策依据：map-style 数据集每 epoch 批数 = ⌈N/batch_size⌉（drop_last 时为 ⌊N/batch_size⌋）；worker 是独立进程并各自持有 RNG，随机变换必须经 worker_init_fn 播种才可复现。
>
> 可验证事实：pin_memory=True 配合 non_blocking=True 能把 H2D 拷贝与计算重叠；persistent_workers=True 在 epoch 间保留 worker 进程，避免反复 fork 开销。

#### 4. 技术洞察
开放决策的质量取决于可逆性与信息价值：优先选择能低成本试错并快速回退的路径，map-style 数据集每 epoch 批数 = ⌈N/batch_size⌉（drop_last 时为 ⌊N/batch_size⌋）；worker 是独立进程并各自持有 RNG，随机变换必须经 worker_init_fn 播种才可复现。 用于估算代价避免拍脑袋。

#### 5. 图文说明
| 层次 | DataLoader 的并行、采样与预取 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 数据集大小、batch_size、worker 数、采样方式 | 索引不覆盖、随机性不可复现 |
| 执行与状态 | 追踪 worker 进程、预取队列、pin 缓冲与 sampler 状态 | CUDA-in-fork 崩溃、句柄泄漏、顺序错乱 |
| 输出与 oracle | 核对样本集合、shape、吞吐与首 batch 延迟 | 重复/丢失样本、collate 形状错误、吞吐饱和异常 |

图示解释：把 DataLoader 的并行、采样与预取 的候选方案、关键不确定性与回退门槛画成决策树，标出每步的信息收益与不可逆风险。

#### 6. 拓展知识
决策支撑：DataLoader/worker_init_fn/generator、IterableDataset 与分片 与 A/B 或影子流量；比较各候选的迁移成本与失败恢复，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 在信息不足时先做哪个最小实验来降低不确定性，期望得到什么数据？
2. 如果关键假设被推翻，回退到哪一步、损失多大、耗时多久？
3. 如何在多目标（性能、成本、风险）间排序，权重如何随阶段变化？

#### 8. 易错点与反例
1. 反例：在不确定性最大处做不可逆投入，被 在 Dataset.__init__ 或 __getitem__ 中初始化 CUDA 导致 fork 后 worker 崩溃 直接击穿。
2. 反例：只比较理想路径成本，忽略 worker 数过大引发内存或文件句柄耗尽 的运维与恢复代价。
3. 反例：没有停止门槛，持续投入直到 collate_fn 对不定长序列堆叠得到错误 shape 才被动放弃。

#### 9. 实践与验证
验证方案：固定 seed 与 worker_init_fn，连续两 epoch 记录样本索引序列验证覆盖完整且可复现；测量 num_workers∈{0,2,4,8} 的吞吐拐点。 用影子流量或 A/B 验证关键假设，采集 吞吐 samples/s 与 首 batch 延迟；oracle：两轮索引并集等于 0..N−1 且无重复，吞吐在某一 worker 数后饱和。

#### 10. 评分标准
E098 评分：
- 不合格：只给结论，不暴露假设与约束；
- 合格：能列出候选与一个关键不确定性；
- 良好：能给出可逆路径、回退门槛与验证计划；
- 优秀/专家：能补充信息价值量化、多目标权衡、阶段化决策与残余风险。
### E099. 开放决策：训练/优化器

- 难度：专家级
- 知识域：K05 训练/优化器
- 考察能力：不变量、二阶影响、证据缺口与演进路线
- 题型：综合开放/专家追问
- 面试频率：开放题
- 前置知识：forward、backward、累积、optimizer、scheduler
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

开放决策要求在信息不完备下对 优化器更新、学习率调度与梯度处理 做出可辩护的选择：目标、约束、候选与不确定性都必须显式。决策不是追求最优，而是在给定风险预算下选择可逆、可观测、可回退的方案，重点关注 优化器状态（Adam 的 m/v） 与 梯度累积与有效批。

#### 2. 问题分析
1. 明确目标函数、硬约束与风险预算，列出候选方案。
2. 用第一性关系估计各方案的代价：Adam：m_t=β1·m_{t−1}+(1−β1)·g_t，v_t=β2·v_{t−1}+(1−β2)·g_t²，偏差校正 m̂=m/(1−β1^t)、v̂=v/(1−β2^t)，θ←θ−lr·m̂/(√v̂+ε)；梯度累积 k 步等价于把有效批放大 k 倍，但必须对 loss 除以 k 或按 k 归约，否则梯度被放大 k 倍。
3. 识别关键不确定性与其信息价值，设计最小验证实验。
4. 选择可逆路径并设定停止/回退门槛，防止 忘记 zero_grad 使梯度跨 step 累加，等效 lr 漂移。

#### 3. 具体答案
面试中可以这样回答：

> 训练步是 zero_grad→forward→loss→scaled backward→unscale→clip→step→scheduler 的固定顺序；顺序错位会静默改变有效学习率或裁剪幅度。
>
> 决策依据：Adam：m_t=β1·m_{t−1}+(1−β1)·g_t，v_t=β2·v_{t−1}+(1−β2)·g_t²，偏差校正 m̂=m/(1−β1^t)、v̂=v/(1−β2^t)，θ←θ−lr·m̂/(√v̂+ε)；梯度累积 k 步等价于把有效批放大 k 倍，但必须对 loss 除以 k 或按 k 归约，否则梯度被放大 k 倍。
>
> 可验证事实：zero_grad(set_to_none=True) 把 .grad 置 None（自 PyTorch 2.0 起为默认行为），比置零更省内存；optimizer.state_dict() 按参数索引保存 momentum/m/v 缓冲区。

#### 4. 技术洞察
开放决策的质量取决于可逆性与信息价值：优先选择能低成本试错并快速回退的路径，Adam：m_t=β1·m_{t−1}+(1−β1)·g_t，v_t=β2·v_{t−1}+(1−β2)·g_t²，偏差校正 m̂=m/(1−β1^t)、v̂=v/(1−β2^t)，θ←θ−lr·m̂/(√v̂+ε)；梯度累积 k 步等价于把有效批放大 k 倍，但必须对 loss 除以 k 或按 k 归约，否则梯度被放大 k 倍。 用于估算代价避免拍脑袋。

#### 5. 图文说明
| 层次 | 优化器更新、学习率调度与梯度处理 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | 初始参数、优化器配置、损失缩放与累积步 k | 顺序错位、未缩放 loss |
| 执行与状态 | 追踪 .grad、优化器状态缓冲区、lr 调度历史 | 梯度残留、动量错乱、裁剪幅度被缩放 |
| 输出与 oracle | 核对 loss 单调性、参数更新与 lr 序列 | loss 震荡、等效 lr 漂移、累积实现不等价 |

图示解释：把 优化器更新、学习率调度与梯度处理 的候选方案、关键不确定性与回退门槛画成决策树，标出每步的信息收益与不可逆风险。

#### 6. 拓展知识
决策支撑：optimizer.state_dict()/param_groups、clip_grad_norm_ 返回值 与 A/B 或影子流量；比较各候选的迁移成本与失败恢复，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 在信息不足时先做哪个最小实验来降低不确定性，期望得到什么数据？
2. 如果关键假设被推翻，回退到哪一步、损失多大、耗时多久？
3. 如何在多目标（性能、成本、风险）间排序，权重如何随阶段变化？

#### 8. 易错点与反例
1. 反例：在不确定性最大处做不可逆投入，被 忘记 zero_grad 使梯度跨 step 累加，等效 lr 漂移 直接击穿。
2. 反例：只比较理想路径成本，忽略 scheduler.step 放错位置（每 batch 而非每 epoch）改变 lr 曲线 的运维与恢复代价。
3. 反例：没有停止门槛，持续投入直到 clip_grad_norm_ 在 GradScaler.unscale_ 之前调用使裁剪阈值被放大 S 倍 才被动放弃。

#### 9. 实践与验证
验证方案：在单个 batch 上跑 200 步验证 loss 单调下降到过拟合；打印 get_last_lr 序列核对调度曲线；对同一初始点比较累积实现与放大批实现的参数更新。 用影子流量或 A/B 验证关键假设，采集 loss 曲线 与 学习率序列；oracle：单批过拟合 loss < 0.01；累积与大批实现的参数差在 1e-6 内。

#### 10. 评分标准
E099 评分：
- 不合格：只给结论，不暴露假设与约束；
- 合格：能列出候选与一个关键不确定性；
- 良好：能给出可逆路径、回退门槛与验证计划；
- 优秀/专家：能补充信息价值量化、多目标权衡、阶段化决策与残余风险。
### E100. 开放决策：AMP/恢复

- 难度：专家级
- 知识域：K06 AMP/恢复
- 考察能力：不变量、二阶影响、证据缺口与演进路线
- 题型：综合开放/专家追问
- 面试频率：开放题
- 前置知识：autocast、scaler、快照、随机状态
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[推断][建议][待验证]

#### 1. 背景知识

开放决策要求在信息不完备下对 混合精度、检查点与确定性 做出可辩护的选择：目标、约束、候选与不确定性都必须显式。决策不是追求最优，而是在给定风险预算下选择可逆、可观测、可回退的方案，重点关注 autocast 的算子白名单 与 bf16 与 fp16 数值范围。

#### 2. 问题分析
1. 明确目标函数、硬约束与风险预算，列出候选方案。
2. 用第一性关系估计各方案的代价：GradScaler 先用 scale S 放大 loss 再反向，unscale_ 后若检测到 inf/nan 则跳过该 step 并把 S 减半；fp16 最正规数约 6.1e-5、次正规至约 6e-8，小于此量级的梯度下溢，故需缩放；bf16 指数位与 fp32 同为 8 位因此不需要缩放。
3. 识别关键不确定性与其信息价值，设计最小验证实验。
4. 选择可逆路径并设定停止/回退门槛，防止 fp16 无 scaler 导致梯度下溢为 0 或 NaN。

#### 3. 具体答案
面试中可以这样回答：

> AMP 通过 autocast 选低精度算子、GradScaler 维持梯度动态范围；恢复的正确性是状态闭包问题——模型、优化器、缩放与随机数状态缺一不可。
>
> 决策依据：GradScaler 先用 scale S 放大 loss 再反向，unscale_ 后若检测到 inf/nan 则跳过该 step 并把 S 减半；fp16 最正规数约 6.1e-5、次正规至约 6e-8，小于此量级的梯度下溢，故需缩放；bf16 指数位与 fp32 同为 8 位因此不需要缩放。
>
> 可验证事实：torch.use_deterministic_algorithms(True) 会让部分算子回退到确定性实现或直接报错；cudnn.benchmark=True 的自动调优会破坏逐步可复现性。完整恢复需保存 model、optimizer、scaler、epoch、CPU 与 CUDA RNG 及 sampler 状态。

#### 4. 技术洞察
开放决策的质量取决于可逆性与信息价值：优先选择能低成本试错并快速回退的路径，GradScaler 先用 scale S 放大 loss 再反向，unscale_ 后若检测到 inf/nan 则跳过该 step 并把 S 减半；fp16 最正规数约 6.1e-5、次正规至约 6e-8，小于此量级的梯度下溢，故需缩放；bf16 指数位与 fp32 同为 8 位因此不需要缩放。 用于估算代价避免拍脑袋。

#### 5. 图文说明
| 层次 | 混合精度、检查点与确定性 的检查点 | 失败或证据信号 |
|---|---|---|
| 输入与约束 | dtype、autocast 区域、scaler 配置、checkpoint 内容 | 状态不闭合、精度选择错误 |
| 执行与状态 | 追踪 scale factor、RNG、优化器状态与 nan/inf 分支 | 下溢、NaN、恢复后状态漂移 |
| 输出与 oracle | 比对续训与基线参数、scale 曲线、依赖确定性开关 | 曲线跳变、结果不可复现、梯度全零 |

图示解释：把 混合精度、检查点与确定性 的候选方案、关键不确定性与回退门槛画成决策树，标出每步的信息收益与不可逆风险。

#### 6. 拓展知识
决策支撑：torch.cuda.amp.GradScaler 与 autocast、torch.use_deterministic_algorithms 与 A/B 或影子流量；比较各候选的迁移成本与失败恢复，未运行结论保留 [待验证]。

#### 7. 常见追问及回答要点
1. 在信息不足时先做哪个最小实验来降低不确定性，期望得到什么数据？
2. 如果关键假设被推翻，回退到哪一步、损失多大、耗时多久？
3. 如何在多目标（性能、成本、风险）间排序，权重如何随阶段变化？

#### 8. 易错点与反例
1. 反例：在不确定性最大处做不可逆投入，被 fp16 无 scaler 导致梯度下溢为 0 或 NaN 直接击穿。
2. 反例：只比较理想路径成本，忽略 resume 只存模型权重丢失优化器动量使曲线跳变 的运维与恢复代价。
3. 反例：没有停止门槛，持续投入直到 只存 torch RNG 未存 numpy/random 状态导致数据顺序不同 才被动放弃。

#### 9. 实践与验证
验证方案：训练 50 步保存完整 checkpoint，重启加载后再跑 50 步与不中断基线逐参数比较；fp16 下注入大梯度观测 S 是否减半。 用影子流量或 A/B 验证关键假设，采集 scale factor 曲线 与 resume 前后参数最大差；oracle：续训与基线参数最大差 < 1e-5（bit 级取决于确定性开关），scale factor 与跳过计数一致。

#### 10. 评分标准
E100 评分：
- 不合格：只给结论，不暴露假设与约束；
- 合格：能列出候选与一个关键不确定性；
- 良好：能给出可逆路径、回退门槛与验证计划；
- 优秀/专家：能补充信息价值量化、多目标权衡、阶段化决策与残余风险。
