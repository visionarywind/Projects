# 高级 PyTorch 面试题

本文件包含 `A001`–`A100`，共 100 题；统一十类题型各 10 题，GPU、通信和性能结果未经目标环境运行时标为 [待验证]。

### A001. 如何为大模型训练选择 DDP、FSDP 或参数/张量并行？

- 难度：高级
- 知识域：K07 分布式训练与通信
- 考察能力：概念边界、适用条件与反例
- 题型：概念解释与辨析
- 面试频率：高频
- 前置知识：DDP、显存组成、collective、模型结构
- 关联题目：I004、A003、E002
- 事实状态：[已确认][推断]

#### 1. 背景知识

选择并行策略的本质是先回答三个问题：单卡是否放得下（参数 P、梯度 P、Adam 状态约 2P）、单步能否在时限内完成、以及网络能否承受相应通信量。DDP 每卡保留完整模型副本，靠反向 bucket allreduce 同步梯度；FSDP/ZeRO 把参数、梯度、优化器状态分片；张量并行（TP）把单层矩阵切到多卡，流水线并行（PP）把层切到不同 stage。

#### 2. 问题分析

先用 P 估算训练态显存：fp32 下参数 4P + 梯度 4P + Adam 的 exp_avg/exp_avg_sq 共 8P（合计约 16P 字节），再叠加激活。若模型能放进单卡，优先 DDP；放不下但单层可切且卡间是 NVLink 高带宽，选 TP；纯层数堆叠且能接受气泡，选 PP；只想省显存仍做数据并行，选 FSDP。

#### 3. 具体答案

面试中可以这样回答：

> 我按“显存→带宽→复杂度”排序：先算 P 与优化器状态是否超单卡（Adam fp32 约 16P 字节）；能放下就 DDP，通信仅为梯度 allreduce（ring 约 2(N-1)/N×P）。放不下而卡间是 NVLink，用 TP 切单层；只跨节点低带宽，用 FSDP 分片状态或 PP。反例是单卡本可放下却强行 TP：矩阵被切小后通信占比上升、效率反降。

#### 4. 技术洞察

DDP 每卡峰值显存≈参数+梯度+优化器状态；FSDP 每卡≈P/N+通信缓冲；ring allreduce 通信量≈2(N-1)/N×M，N 大时趋近 2M。TP 的每层通信量与激活大小成正比，因此依赖高带宽互联。

#### 5. 图文说明

| 策略 | 显存 | 主要通信 | 适用条件 |
|---|---|---|---|
| DDP | 每卡完整副本 | 反向梯度 allreduce | 单卡放得下、想提吞吐 |
| FSDP | 参数/梯度/状态分片 | 前向 all_gather、反向 reduce_scatter | 单卡放不下、跨节点低带宽 |
| TP | 单层切分 | 每层 allreduce（激活量级） | 卡间高带宽（NVLink） |
| PP | 层切分 | stage 边界点对点 | 层数深、可容忍气泡 |

图示解释：横轴是互联带宽、纵轴是单卡显存缺口，四象限分别落到 DDP/FSDP/TP/混合方案。

#### 6. 拓展知识

沿相邻边界继续：TP 与 FSDP 可组合成 2D/3D 并行；PP 的气泡率≈(stages-1)/microbatches。比较 ZeRO-1/2/3 的分片粒度、TP 的序列并行变体，以及通信压缩/重叠的迁移成本；未实测的吞吐数字保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. 给定 P=70B、8×80GB、NVLink 与 400G 以太网，你会如何分层组合 DDP/TP/PP/FSDP，并估算每卡峰值显存？
2. 为什么 FSDP 只降显存、不降单卡计算量，这对吞吐意味着什么？
3. PP 的 micro-batch 数与流水线气泡率的关系是什么，如何压低气泡？

#### 8. 易错点与反例

1. 把 reshape/副本当作零成本，忽略通信缓冲与梯度分片的额外显存。
2. 认为 FSDP 会减少每卡 FLOPs——它只减少驻留显存。
3. 忽视 bucket_cap_mb 与分片粒度对通信重叠和峰值显存的影响。

#### 9. 实践与验证

在 8 卡上固定一个 1B 稠密模型：分别跑 DDP、FSDP、TP=2，记录 max_memory_allocated、单步时间与 allreduce/all_gather 耗时占比；用 torch.profiler 归因通信与计算重叠率，验证“显存下降但吞吐未必提升”的取舍。

#### 10. 评分标准

不合格：只罗列 DDP/FSDP/TP 名词，说不出优化器状态量级或 ring 通信量。
合格：能按显存→带宽给出选择顺序和一条反例。
良好：能给出 16P 字节估计、ring 公式并映射到具体互联条件。
优秀/专家：能设计混合并行、量化气泡率、给出通信重叠与回滚门槛。
### A002. `torch.compile` 从 eager 到生成代码经历什么，graph break 如何处理？

- 难度：高级
- 知识域：K08 `torch.compile` 与编译栈
- 考察能力：概念边界、适用条件与反例
- 题型：概念解释与辨析
- 面试频率：高频
- 前置知识：Python 执行、autograd、算子和 profiler
- 关联题目：I005、A003、E003
- 事实状态：[已确认][待验证]

#### 1. 背景知识

torch.compile 的编译链是：Dynamo 捕获 Python 字节码→FX 图并建立 guard；AOTAutograd 生成前向/反向图；后端 Inductor 生成 Triton（GPU）或 C++（CPU）kernel；无法捕获或被标记的部分形成 graph break，回退到 eager 执行。

#### 2. 问题分析

先区分“已编译段”和“graph break 回退段”。graph break 常见来源是数据相关控制流、不被支持的 Python 对象、.item()/print 等副作用，以及 numpy/非张量调用。用 torch._dynamo.explain 和 TORCH_LOGS=graph_breaks 定位断点，再决定是改写代码、用 custom op 还是接受回退。

#### 3. 具体答案

面试中可以这样回答：

> 我会先看 graph_breaks 计数与原因。Dynamo 遇到无法追踪的 Python 语义就断图，前后各成一段图，断点处回退 eager。消除方式包括：把数据相关分支移出被编译函数、对动态 shape 标记 dynamic=True、用 torch.library 注册 custom op 让 Inductor 认识它。注意 graph break 不只影响性能：断点两侧的缓存与状态副作用语义要重新确认，否则可能改变正确性。

#### 4. 技术洞察

编译缓存以 guard 值集合为键；重编译次数正比于出现过的不同 guard 组合数。fullgraph=True 时任何 graph break 直接抛错，是检验可编译性的强约束。

#### 5. 图文说明

| 阶段 | 组件 | 产物/信号 |
|---|---|---|
| 追踪 | Dynamo | FX 图 + guards |
| 微分 | AOTAutograd | 前向/反向图 |
| 代码生成 | Inductor | Triton/C++ kernel |
| 断图 | graph break | 回退 eager 段 |

图示解释：横向是编译流水线，graph break 表现为流水线上的“旁路”，需分别度量编译段与回退段耗时。

#### 6. 拓展知识

继续延伸到 dynamic shape（Dim/mark_dynamic）、guard 导出与缓存失效、AOTAutograd 的 min-cut 重新计算，以及自定义 op 的 fake/meta 注册；不同 torch 版本对 graph break 的覆盖不同，未实测部分标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 哪些 Python 语义会让 Dynamo 断图，如何用 torch._dynamo.explain 逐条定位？
2. 为什么 graph break 也可能影响正确性，而不仅是性能？
3. fullgraph=True 编译失败时，你会如何定位首个断点？

#### 8. 易错点与反例

1. 认为 graph break 只影响速度、不影响结果，忽略断点两侧的状态副作用。
2. 用固定 shape 的基准掩盖动态 shape 触发的反复重编译。
3. 忽略首次编译的冷启动开销，把首调时间计入稳态性能。

#### 9. 实践与验证

在一个含 .item() 分支的模型上运行 torch.compile，记录 torch._dynamo.explain 的 graph_breaks 与 break reasons；改写为张量化判断后重测，对比 step time、重编译计数与数值一致性（assert_close）。

#### 10. 评分标准

不合格：只说“Dynamo→Inductor”，说不出 graph break 的成因与后果。
合格：能描述编译链并给出一种消除 graph break 的方法。
良好：能用 explain/日志定位断点并区分编译段与回退段。
优秀/专家：能讨论 dynamic shape、custom op 注册、缓存失效与正确性风险。
### A003. 如何建立 PyTorch 训练性能模型并定位瓶颈？

- 难度：高级
- 知识域：K09 性能、显存与算子分析
- 考察能力：概念边界、适用条件与反例
- 题型：概念解释与辨析
- 面试频率：高频
- 前置知识：profiler、GPU、分布式、训练循环
- 关联题目：I005、A001、A002、E004
- 事实状态：[已确认][推断]

#### 1. 背景知识

训练性能模型是把端到端 step time 分解为数据准备、H2D 拷贝、kernel 计算、通信、同步、编译/缓存和框架调度，再判断每段由算力、带宽、launch 开销还是串行依赖限制。

#### 2. 问题分析

先建立可复现基线：固定输入、warmup、多次取中位数与 p99，并 torch.cuda.synchronize()。用 torch.profiler 得到 kernel 时间表，判断每个热点是 compute-bound 还是 memory-bound（对比算术强度与机器拐点），再看通信与计算是否重叠。

#### 3. 具体答案

面试中可以这样回答：

> 我不会先猜瓶颈，而是先立基线。用 profiler 把 step time 拆开：若某 kernel cuda_time 占比最高且算术强度低于拐点，就是 memory-bound，优化方向是提高复用、融合或降低精度；若 GPU 有大段空泡则可能是数据加载或通信未重叠。每一步只改一个变量，并保留 p50/p99 与显存峰值。

#### 4. 技术洞察

roofline 下 kernel 时间≈max(FLOPs/FLOPS_peak, Bytes/BW_peak)；算术强度=FLOPs/Bytes。所有 kernel 时间之和≤墙钟时间，差额是同步、空泡与调度开销。

#### 5. 图文说明

| 分解层 | 观测工具 | 典型瓶颈 |
|---|---|---|
| 数据准备 | CPU profiler / loader 计时 | 解码慢、worker 不足 |
| H2D | cudaMemcpy 计时 | 未 pin、未重叠 |
| 计算 | profiler kernel 表 | 访存受限、launch 过多 |
| 通信 | NCCL 计时 | 未与反向重叠 |

图示解释：把 step time 画成堆叠条形图，标出可并行段与串行空泡，优先压缩占比最大且不受依赖限制的部分。

#### 6. 拓展知识

延伸学习 roofline 拐点计算、显存快照与碎片、通信/计算重叠率度量，以及不同 profiler 采样对长尾的偏差；未在目标硬件实测的数字标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 如何用 roofline 判断一个算子是 compute-bound 还是 memory-bound，需要哪些输入？
2. 为什么 profiler 的 self_cuda_time 之和常小于 step time，差额来自哪里？
3. 在通信与计算未重叠时，你会先改数据并行还是调整 bucket/分片策略？

#### 8. 易错点与反例

1. 不 sync 就读 CUDA event 时间，把入队时间误当执行时间。
2. 用单次测量下结论，被启动抖动与噪声误导。
3. 把 H2D 拷贝计入 GPU 计算时间，导致归因错误。

#### 9. 实践与验证

固定一个 transformer block，先 profiler 分解，再分别尝试融合算子、提高精度复用、开启通信重叠，单变量对比 step time 中位数、p99、GPU 利用率与显存峰值，验证每项改动是否真的命中主宰瓶颈。

#### 10. 评分标准

不合格：只给一句“用 profiler 看”。
合格：能分解 step time 并指出一类瓶颈。
良好：能用 roofline 与空泡分析定位并给出验证方案。
优秀/专家：能定量比较多个优化、处理噪声与长尾，并给出停止优化门槛。
### A004. `state_dict`、TorchScript、Export 和完整对象序列化如何取舍？

- 难度：高级
- 知识域：K10 序列化、导出、部署与兼容性
- 考察能力：概念边界、适用条件与反例
- 题型：概念解释与辨析
- 面试频率：中频
- 前置知识：Module、checkpoint、部署接口
- 关联题目：B004、I001、E003
- 事实状态：[已确认][存在争议]

#### 1. 背景知识

state_dict 保存参数与 buffer 的键值映射，轻量但需模型代码；TorchScript 与 Export 产出可部署的图/程序；完整对象 pickle 携带更多 Python 依赖，灵活但可移植性差、且有反序列化安全风险。

#### 2. 问题分析

先问部署运行时：是原生 PyTorch 加载还是非 Python runtime。若同版本同代码，state_dict 足够；若要脱离 Python 执行或做图优化，用 torch.export；若目标 runtime 只吃 TorchScript，则用 jit.script（控制流友好）或 trace（仅限无数据相关控制流）。注意 torch.load 自 2.6 起 weights_only 默认为 True。

#### 3. 具体答案

面试中可以这样回答：

> 我按“运行时约束→是否需要图→控制流复杂度”选。纯权重续训用 state_dict；需要跨语言/无 Python 推理用 torch.export 得到 ExportedProgram；目标栈只支持 TorchScript 才用它。一个关键点：PyTorch 2.6 把 torch.load 的 weights_only 默认改为 True，加载旧的自定义类 pickle 会失败，需要显式关闭并做安全检查。

#### 4. 技术洞察

state_dict=参数∪buffer，键集合稳定；torch.export 的图不含 Python 语义，动态维度需用 dynamic_shapes/Dim 声明。trace 会把数据相关 if/for 固化成单一路径。

#### 5. 图文说明

| 方案 | 载体 | 控制流 | 主要风险 |
|---|---|---|---|
| state_dict | 张量映射 | 无 | 依赖模型代码 |
| TorchScript | scripted/traced 图 | script 支持、trace 塌缩 | 覆盖范围随版本变 |
| torch.export | ATen 图 | 捕获并显式化 | 动态 shape 需声明 |
| pickle | Python 对象 | 完整 Python | 安全与可移植性 |

图示解释：按“可移植性↑/表达力↓”排布四种方案，选型就是在这条曲线上取满足运行时约束的点。

#### 6. 拓展知识

延伸到 export 的 range_constraints、custom op 注册、ONNX/ExecuTorch runtime 差异，以及版本 schema 演进对加载的影响；未运行验证的兼容结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. TorchScript 的 script 与 trace 在控制流处理上的本质区别是什么？
2. weights_only 默认为 True 带来了哪些兼容性与安全影响？
3. 如何用 dynamic_shapes 表达一个仅序列维动态的模型并验证导出？

#### 8. 易错点与反例

1. 用 trace 导出含 if/loop 的模型，导致数据相关分支被固化成单一路径。
2. 认为 weights_only=True 永远安全——自定义 op/类仍需注册。
3. 忽略导出后 dtype/device 与目标 runtime 不一致。

#### 9. 实践与验证

取一个含条件分支的模型：分别用 trace、script、torch.export 导出，构造两路输入使分支走向不同，对比输出是否与原模型 assert_close，并记录各方案的失败点与动态维度声明。

#### 10. 评分标准

不合格：只会说“state_dict 存权重”。
合格：能对比两种方案并给出适用条件。
良好：能处理控制流/动态 shape 并给出验证方法。
优秀/专家：能覆盖安全、版本演进、runtime 差异与迁移回滚。
### A005. 如何设计一个可维护的 C++/CUDA 自定义算子？

- 难度：高级
- 知识域：K11 C++/CUDA 扩展与自定义算子
- 考察能力：概念边界、适用条件与反例
- 题型：概念解释与辨析
- 面试频率：中频
- 前置知识：Tensor stride、CUDA kernel、autograd、编译系统
- 关联题目：B002、A002、E003
- 事实状态：[已确认][待验证]

#### 1. 背景知识

可维护的自定义算子要同时满足：ATen schema 清晰、dtype/device/stride 契约明确、前向正确、反向可微、stream 语义正确、可被 torch.compile 与导出识别，并有编译与测试基建。

#### 2. 问题分析

先定 schema（参数类型、是否返回多张量、是否 in-place），再实现 CPU/CUDA 分派，再补 autograd（torch.autograd.Function 或 torch.library.register_autograd），并登记 meta/fake 以支持编译与导出。最后用 gradcheck、assert_close 与 ABI 检查收口。

#### 3. 具体答案

面试中可以这样回答：

> 我会把算子当产品来做：先写 schema 定契约，再实现 kernel，再补反向和 meta 注册。kernel 必须跑在当前 stream 上（at::cuda::getCurrentCUDAStream），多卡要 CUDAGuard；编译要匹配 PyTorch 的 C++ ABI 与 -gencode。交付前用 gradcheck 验反向、assert_close 验前向，并跑 torch.compile/export 确认能被追踪。

#### 4. 技术洞察

加速比=t_baseline/t_custom（同 shape/dtype/stream）；occupancy=活跃 warp/最大 warp。gradcheck 通过表示前向/反向数值一致，是不变量。

#### 5. 图文说明

| 检查面 | 机制 | 失败信号 |
|---|---|---|
| 契约 | schema + dispatcher | 类型/形状不匹配 |
| 数值 | 参考实现 + gradcheck | 梯度误差超容差 |
| 流 | getCurrentCUDAStream | 与其它算子乱序 |
| 编译 | meta/fake 注册 | compile/export 失败 |

图示解释：从 schema→kernel→autograd→meta 的注册链，任一环缺失都会在特定工具（gradcheck/compile）处暴露。

#### 6. 拓展知识

延伸到 dispatch key 体系、流与事件同步、编译期架构选择、确定性（避免非确定归约）与算子版本兼容；未在目标卡实测的性能标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 为自定义算子实现反向有哪两种机制，各自适用场景是什么？
2. 为什么 kernel 必须使用当前 CUDA stream，如何正确获取与同步？
3. 缺少 meta/fake 注册会在 torch.compile 或导出时如何暴露？

#### 8. 易错点与反例

1. 只写前向不注册反向，autograd 直接报错或梯度为 None。
2. 在默认 stream 上跑 kernel 却从非默认 stream 读数据，产生乱序。
3. 忽略 C++ ABI 不匹配，运行时出现 undefined symbol。

#### 9. 实践与验证

实现一个带反向的融合算子（如 Swish）：用 CUDAExtension 编译，gradcheck 验梯度，assert_close 对比参考实现，并分别用 torch.compile 与 torch.export 跑的路径确认可追踪；记录 ncu 的 occupancy 与访存效率。

#### 10. 评分标准

不合格：只会写 kernel，不谈 schema/反向/流。
合格：能实现前向并说明契约。
良好：能补反向、注册 meta 并通过 gradcheck/编译。
优秀/专家：能处理 ABI、确定性、编译期架构与多卡设备与回滚。
### A006. FSDP 的参数分片、通信和 checkpoint 语义如何设计？

- 难度：高级
- 知识域：K07 分布式训练与通信
- 考察能力：概念边界、适用条件与反例
- 题型：概念解释与辨析
- 面试频率：高频
- 前置知识：DDP、参数/梯度/优化器状态、collective
- 关联题目：A001、I006、E002
- 事实状态：[已确认][推断]

#### 1. 背景知识

FSDP 的设计围绕三件事：参数分片（shard）、为计算临时 all_gather 完整参数、以及梯度与优化器状态分片；checkpoint 需处理分片状态的保存/恢复与聚合。

#### 2. 问题分析

先按层/模块 wrap，使分片粒度与通信量平衡；前向 all_gather 参数、用完即释放，反向算梯度后 reduce_scatter 回分片，优化器在分片上更新。checkpoint 用 sharded state_dict 或 full_state_dict 二选一，注意 buffer 与优化器状态也要一致恢复。

#### 3. 具体答案

面试中可以这样回答：

> FSDP 用“分片驻留、按需聚合”换显存：参数在用时 all_gather 成完整张量，算完释放；反向对梯度 reduce_scatter 回各 rank 分片，优化器只在分片上更新。checkpoint 我用 sharded_state_dict 按 rank 保存以避免聚合时的内存峰值，恢复时要保证分片数、wrap 结构、buffer 与优化器状态都匹配。它换来的是显存下降，不是计算量下降。

#### 4. 技术洞察

参数分片使每卡驻留≈P/N；ring allreduce/all_gather 通信量≈2(N-1)/N×M；wrap 越细通信次数越多、越粗峰值显存越高。

#### 5. 图文说明

| 阶段 | 通信 | 状态 |
|---|---|---|
| 前向 | all_gather 参数 | 临时完整参数 |
| 反向 | reduce_scatter 梯度 | 分片梯度 |
| 更新 | 无（本地分片） | 分片优化器状态 |
| 存档 | sharded_state_dict | 按 rank 分片 |

图示解释：把 FSDP 的“聚合-计算-释放”画成时序，标注每步显存峰值与通信量，wrap 粒度决定了两者的权衡。

#### 6. 拓展知识

延伸到 HSDP（节点内分片+节点间复制）、与 TP/PP 组合的混合并行、分片 checkpoint 的聚合工具与版本兼容；未实测的吞吐标 `[待验证]`。

#### 7. 常见追问及回答要点

1. FSDP 的 all_gather 与 reduce_scatter 分别在前向/反向何处发生，为什么这样设计？
2. 分片 checkpoint 相比 full checkpoint 在什么场景更优或更差？
3. wrap 粒度如何同时影响峰值显存与通信次数？

#### 8. 易错点与反例

1. 认为 FSDP 降低单卡 FLOPs——它只降低驻留显存。
2. 未 wrap 子模块导致整模参数无法分片、显存不降。
3. 恢复时忽略分片数/wrap 结构匹配，导致 state_dict 加载错位。

#### 9. 实践与验证

在一个能放进单卡但不放多卡的大模型上启用 FSDP，记录 max_memory_allocated 与 all_gather/reduce_scatter 耗时占比；分别用 sharded 与 full checkpoint 保存恢复，验证恢复后首个 step 的 loss 与参数一致。

#### 10. 评分标准

不合格：只知道 FSDP 分片，说不出通信发生在何时。
合格：能描述前向 all_gather、反向 reduce_scatter。
良好：能说明 wrap 粒度与 checkpoint 语义的取舍。
优秀/专家：能设计混合并行与分片 checkpoint 恢复，并给出重叠与回滚方案。
### A007. 动态 shape、guards 和缓存如何影响 `torch.compile` 的可用性？

- 难度：高级
- 知识域：K08 `torch.compile` 与编译栈
- 考察能力：概念边界、适用条件与反例
- 题型：概念解释与辨析
- 面试频率：中频
- 前置知识：Dynamo、guards、shape、缓存
- 关联题目：A002、A003、E003
- 事实状态：[已确认][待验证]

#### 1. 背景知识

torch.compile 依赖 guard 判断“这段缓存是否还能复用”。动态 shape 下，若每次输入尺寸不同且未标记动态，Dynamo 会为每个 shape 生成新 guard 并重编译，导致编译抖动与内存增长。

#### 2. 问题分析

先区分 shape 是“稳定可变”还是“偶发变化”：前者用 dynamic=True 或 mark_dynamic 声明为符号维度，让一次编译覆盖一个范围；后者可 pad 到固定桶。再用 torch._dynamo.explain/TORCH_LOGS=recompiles 观察重编译次数与原因，用 guard 导出核对。

#### 3. 具体答案

面试中可以这样回答：

> 我会先量化重编译：打开 TORCH_LOGS=recompiles 看是哪些 guard 失效。对序列长度这类稳定可变的维度，用 torch.compile(dynamic=True) 或 mark_dynamic 把它变成符号维度，一次编译覆盖一片；对偶发尺寸用 shape 分桶/padding 收敛。还要注意 reduce-overhead 的 CUDA Graph 需要地址稳定，动态会破坏捕获。

#### 4. 技术洞察

编译缓存以 guard 值集合为键；重编译次数∝出现过的不同 guard 组合数。dynamic=True 用一个符号 shape 覆盖范围，代价是生成的 kernel 更泛化。

#### 5. 图文说明

| 策略 | 覆盖范围 | 代价 |
|---|---|---|
| 静态 shape | 单尺寸 | 尺寸变即重编译 |
| dynamic/mark_dynamic | 符号范围 | kernel 泛化、可能略慢 |
| shape 分桶 | 有限桶 | 需上层控制输入 |
| padding | 固定尺寸 | 冗余计算 |

图示解释：横轴输入尺寸种类、纵轴编译次数，四种策略在“编译开销 vs 运行效率”上占据不同位置。

#### 6. 拓展知识

延伸学习 Dim 约束与 range_constraints、guard 的导出与序列化、动态 shape 下 Inductor 的 kernel 选择，以及 recompile 缓存的内存增长；未实测标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 哪些 guard 失效会导致重编译，如何用日志区分 shape、dtype 与值 guard？
2. dynamic=True 与 mark_dynamic 的粒度差异是什么，如何按维度选择？
3. 动态 shape 与 reduce-overhead 的 CUDA Graph 为何冲突？

#### 8. 易错点与反例

1. 用单一固定 shape 跑基准，掩盖了线上尺寸变化触发的重编译。
2. 对形状频繁变化的输入逐尺寸编译，造成编译风暴与内存增长。
3. 开 dynamic 后忽略 kernel 泛化带来的单次性能回退。

#### 9. 实践与验证

构造序列长度在 [64,512] 间变化的输入：分别跑静态、dynamic=True、mark_dynamic 三种配置，记录重编译次数、step time 中位数与编译内存，验证哪一种在“编译次数×单次耗时”乘积上最优。

#### 10. 评分标准

不合格：只知道动态会重编译。
合格：能说出 guard 与重编译的关系。
良好：能用日志定位并选择 dynamic/分桶策略。
优秀/专家：能权衡泛化 kernel、CUDA Graph 与内存增长，给出停止门槛。
### A008. 分布式 AMP 中梯度缩放、裁剪和同步的正确顺序是什么？

- 难度：高级
- 知识域：K06 AMP、Checkpoint 与可复现性
- 考察能力：概念边界、适用条件与反例
- 题型：概念解释与辨析
- 面试频率：高频
- 前置知识：AMP、DDP/FSDP、gradient clipping
- 关联题目：I003、I004、I009、E002
- 事实状态：[已确认][待验证]

#### 1. 背景知识

分布式 AMP 的正确顺序是：autocast 前向→scaler.scale(loss).backward()→（若裁剪）scaler.unscale_(optimizer) 后 clip_grad_norm_→scaler.step(optimizer)→scaler.update()。DDP/FSDP 的梯度同步发生在反向过程中，与 scaler 的 scale/unscale 必须协调。

#### 2. 问题分析

关键顺序约束有三条：反向前 loss 必须被 scale；裁剪必须在 unscale 之后进行；DDP 的 allreduce 对已 scale 的梯度求和是线性的，因此同步本身不受影响，但若在 unscale 前裁剪会得到错误幅度。检查点里要保存 scaler 状态。

#### 3. 具体答案

面试中可以这样回答：

> 顺序是 scale→backward（DDP 在反向里 allreduce 已 scale 的梯度，求和是线性的所以没问题）→unscale_→clip→step→update。核心坑是裁剪：grad clip 必须在 unscale 之后，否则裁的是被放大的梯度。另外 checkpoint 要存 scaler.state_dict，否则恢复后 scale 不连续；bf16 通常不需要 scaler。

#### 4. 技术洞察

scale_{new}=scale×growth_factor（连续无溢出）或 scale/backoff_factor（检测到 inf/nan）；DDP allreduce 线性，因此对 scale 后的梯度求和等价于先求和再 scale。

#### 5. 图文说明

| 步骤 | 操作 | 错误做法 |
|---|---|---|
| 前向 | with autocast | 关掉 autocast 混精度 |
| 反向 | scaler.scale(loss).backward() | 直接 loss.backward() |
| 裁剪 | unscale_ 后 clip | 先 clip 后 unscale |
| 更新 | scaler.step + update | 漏 update 使 scale 不变 |

图示解释：时序图上标出 DDP allreduce 插入点，说明线性缩放为何允许在同步前后做 scale/unscale。

#### 6. 拓展知识

延伸到 autocast 的 op 白/黑名单、随机状态与 scaler 快照的断点续训、以及 FSDP 下 unscale 与 reduce_scatter 的交互；未实测标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 为什么 DDP 对已 scale 的梯度做 allreduce 不会破坏 AMP 语义？
2. 梯度裁剪必须在 unscale 之后，若顺序错误会得到什么结果？
3. 断点续训时除了 scaler，还需要保存哪些与 AMP 相关的状态？

#### 8. 易错点与反例

1. 先 clip 后 unscale，裁剪阈值相对被放大的梯度失效。
2. 对 bf16 仍启 GradScaler，带来无谓开销且无收益。
3. 恢复训练时漏存 scaler 状态，scale 从初值重来导致早期不稳定。

#### 9. 实践与验证

在 2 卡 DDP 上跑 fp16 AMP：故意把 clip 放在 unscale 之前、之后各跑一次，比较梯度范数与收敛；再注入含 nan 的 batch，确认 scaler.step 跳过更新且 scaler.get_scale() 下降。

#### 10. 评分标准

不合格：说不清 scale/unscale/clip/step 的顺序。
合格：能写对基本顺序。
良好：能解释 DDP 线性同步与裁剪顺序的原因。
优秀/专家：能覆盖 scaler 快照续训、bf16 取舍与 FSDP 交互。
### A009. 如何把 DataLoader 数据管道扩展到多机训练而不成为瓶颈？

- 难度：高级
- 知识域：K04 数据管道与输入规范
- 考察能力：概念边界、适用条件与反例
- 题型：概念解释与辨析
- 面试频率：高频
- 前置知识：DataLoader、DistributedSampler、存储和网络
- 关联题目：B005、B010、I007、I010
- 事实状态：[已确认][推断]

#### 1. 背景知识

多机数据管道成为瓶颈，通常是“单机够用、跨机放大”的三类问题：存储/对象存储带宽被打满、CPU 解码跟不上、以及各 rank 采样不均衡或重复。DataLoader 的 num_workers、prefetch 与 DistributedSampler 是主要旋钮。

#### 2. 问题分析

先量出每机的取数吞吐与 GPU 消耗速率：若 GPU 有空泡且 H2D/解码占比高，是取数不足；再检查 DistributedSampler 是否各 rank 互斥且覆盖全集，以及 worker 随机源是否互不影响。跨机时优先考虑共享存储读放大、把解码前置或缓存。

#### 3. 具体答案

面试中可以这样回答：

> 我先确认瓶颈在存储、CPU 还是网络：profiler 看 GPU 空泡，同时测单机取数吞吐。若存储被打满，就做数据本地缓存或提升压缩/格式效率；若 CPU 解码跟不上，增加 worker 或把解码下沉到预处理。采样侧用 DistributedSampler 保证各 rank 互斥，每个 worker 用 worker_init_fn 单独播种，避免重复。

#### 4. 技术洞察

吞吐≤min(存储带宽, CPU 解码速率, H2D 带宽)；有效样本/步=batch×world_size。跨机时存储读带宽被 world_size 放大，是常见拐点。

#### 5. 图文说明

| 瓶颈 | 观测 | 手段 |
|---|---|---|
| 存储带宽 | 读吞吐打满 | 本地缓存/高压缩格式 |
| CPU 解码 | worker 饱和、GPU 空泡 | 增 worker、预解码 |
| 采样不均 | rank 样本重复/不齐 | DistributedSampler + 独立播种 |
| H2D | 拷贝占比高 | pin_memory + non_blocking |

图示解释：画“每机取数能力 vs GPU 消耗速率×world_size”两条线，交点是扩展拐点，越过即出现空泡。

#### 6. 拓展知识

延伸到 IterableDataset 与流式读取、chunk 级预取、对象存储的并发连接限制，以及数据本地性调度；未实测标 `[待验证]`。

#### 7. 常见追问及回答要点

1. DistributedSampler 如何处理样本数不被 world_size 整除，是否会引入重复样本？
2. 为什么多 worker 共享随机源会导致样本重复，worker_init_fn 起什么作用？
3. 跨机扩展时，存储读带宽如何随 world_size 放大并成为拐点？

#### 8. 易错点与反例

1. 把 num_workers 一味调大，忽略 IPC 与内存放大反而变慢。
2. 在 Dataset 里用全局随机而不重播种，导致各 worker/epoch 样本重复。
3. 手动 shuffle 又用 DistributedSampler，造成跨 rank 样本重叠。

#### 9. 实践与验证

在 2 机 16 卡上固定模型，先测单机达到的 samples/s，再测跨机与 GPU 消耗速率对比；用 worker_init_fn 修随机、用 DistributedSampler 保证互斥，验证并集覆盖全集且 GPU 空泡下降。

#### 10. 评分标准

不合格：只会说“加 num_workers”。
合格：能识别存储/CPU/采样三类瓶颈之一。
良好：能定位拐点并给出采样与预取方案。
优秀/专家：能设计跨机缓存/本地性调度并给出扩展上界与回归验证。
### A010. 如何为动态模型设计 Export/部署边界和回退方案？

- 难度：高级
- 知识域：K10 序列化、导出、部署与兼容性
- 考察能力：概念边界、适用条件与反例
- 题型：概念解释与辨析
- 面试频率：中频
- 前置知识：Module、动态控制流、custom op、部署 runtime
- 关联题目：A004、A005、E003
- 事实状态：[已确认][待验证]

#### 1. 背景知识

动态模型（含数据相关控制流、可变 shape、可变结构）的导出难点是 Python 语义无法直接进图。方案是：用 torch.export 捕获可导出部分、把动态分支显式化或下沉为 custom op、对不可导出部分保留 Python 回退边界，并准备好回退路径。

#### 2. 问题分析

先标注模型里哪些是数据相关控制流、哪些是 shape 变化、哪些依赖外部 Python 状态。对控制流用 export 的显式化能力或改写为张量算子；对 shape 用 dynamic_shapes/Dim 声明范围；对无法导出的算子注册 custom op 并补 meta/fake。最后定义导出失败的降级策略。

#### 3. 具体答案

面试中可以这样回答：

> 我会先分类动态性来源：shape 用 dynamic_shapes 声明范围；数据相关分支要么改写成张量算子、要么在导出边界外保留；不支持的算子注册成 custom op 让导数链路完整。导出后用一组覆盖各分支的输入做 assert_close，并保留一条“导出失败即回退 TorchScript/eager”的开关，避免发布被单一导出路径卡死。

#### 4. 技术洞察

导出图=静态化 Python 控制流+声明化 shape；torch.export 产出的 ExportedProgram 含 range_constraints，用于表达动态维度的上下界。

#### 5. 图文说明

| 动态来源 | 处理 | 验证 |
|---|---|---|
| 可变 shape | dynamic_shapes/Dim | range_constraints |
| 数据相关分支 | 张量化或下沉 custom op | 覆盖各分支输入 |
| 外部 Python 状态 | 提为显式输入 | 输入契约测试 |
| 不支持算子 | custom op + meta | export 成功且可追踪 |

图示解释：把“可导出核心 + Python 回退边界 + 降级开关”画成三层，标注每层的导出成败信号。

#### 6. 拓展知识

延伸到 ExportedProgram 的序列化与 runtime（ExecuTorch/ONNX）、custom op 的 register_autograd、动态控制流的 data-dependent op 表达；未实测标 `[待验证]`。

#### 7. 常见追问及回答要点

1. torch.export 对数据相关控制流的支持边界是什么，何时必须下沉为 custom op？
2. dynamic_shapes 与 Dim 如何表达范围，range_constraints 的作用是什么？
3. 导出失败时的回退路径如何设计才不影响线上可用性？

#### 8. 易错点与反例

1. 直接 trace 含 if/loop 的模型，分支被固化成单一路径。
2. 不注册 custom op，导出在遇到自定义算子时直接失败。
3. 只跑一条 happy 分支验证导出，未覆盖其它动态路径。

#### 9. 实践与验证

取一个含数据相关分支与可变序列长的模型：用 torch.export + dynamic_shapes 导出，构造覆盖各分支的输入做 assert_close，并人为制造一个导出失败场景验证回退开关能生效。

#### 10. 评分标准

不合格：只说“用 export 导出”。
合格：能识别动态性来源并给出一种处理。
良好：能处理 shape 声明、custom op 与回退边界。
优秀/专家：能设计导出/回退双路径、覆盖全部动态分支并给出发布门槛。
### A011. 架构设计：张量/设备

- 难度：高级
- 知识域：K01 张量/设备
- 考察能力：变量、因果链、复杂度与容量上限
- 题型：原理推导与复杂度
- 面试频率：中频
- 前置知识：shape、stride、dtype、device
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

先用契约、状态所有权和失败模式划清系统边界，再谈实现细节。本题位于 K01 张量/设备。连续张量满足 stride[i]=∏_{j>i}shape[j]，shape=(2,3,4) 的 stride 恰为 (12,4,1)。

#### 2. 问题分析

从“架构设计：张量/设备”出发，先固定输入契约与资源约束，把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 torch.Tensor.view/reshape 的契约，列出两条不变量：「view 与源共享 storage（data_ptr 与 storage_offset 一致）」「张量的 device/dtype 决定 kernel 派发与是否发生隐式转换」，再给一个会击穿假设的边界输入，并说明它最先暴露 对非连续张量 view 报 'view size is not compatible with input tensor's size and stride' 还是 跨设备运算报 'Expected all tensors to be on the same device'。

#### 3. 具体答案

面试中可以这样回答：

> 我会先给总体架构与组件契约，再给两条不变量和一个会击穿设计的边界输入。在 张量/设备 上，关键事实是：view 只改元数据且要求 stride 兼容；reshape 在不兼容时会静默拷贝，可用 t.data_ptr() 判断是否共享存储。 判据可写为 显存(bytes)=numel×itemsize(dtype)；连续 stride[i]=∏_{j>i}shape[j]。

#### 4. 技术洞察

架构的价值在于可验证的不变量（谁拥有状态、何时同步、失败如何回滚），而非单个性能数字。在 张量/设备 上，量化关系是 显存(bytes)=numel×itemsize(dtype)；连续 stride[i]=∏_{j>i}shape[j]；核心不变量是「view 与源共享 storage（data_ptr 与 storage_offset 一致）」，它一旦被打破，最先表现为 reshape 静默拷贝抬高峰值显存。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | torch.Tensor.view/reshape 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | view 与源共享 storage（data_ptr 与 storage_offset 一致） | 违反「张量的 device/dtype 决定 kernel 派发与是否发生隐式转换」时的报错 |
| 量化关系 | 显存(bytes)=numel×itemsize(dtype)；连续 stride[i]=∏_{j>i}shape[j] | 实测与预测偏差超容差 |
| 失败模式 | 对非连续张量 view 报 'view size is not compatible with input tensor's size and stride' | 跨设备运算报 'Expected all tensors to be on the same device' |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 张量/设备 在“架构设计”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“架构设计：张量/设备”沿相邻边界延伸：view/reshape/contiguous、pin_memory 与 stream、dtype 转换与显存。比较版本与替代实现的差异（如 t.to(device, non_blocking=True)），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [架构设计] shape=(2,3,4) 分别 view 成 (6,4)、reshape 成 (6,4) 后，stride 与是否共享存储各是什么？
2. [架构设计] 为什么 permute 之后必须先 contiguous 才能 view？
3. [架构设计] non_blocking=True 在源未 pinned 时的实际行为如何用 profiler 证明？

#### 8. 易错点与反例

1. [架构设计] 把 reshape 当零拷贝：stride 不兼容时它会 contiguous 并分配新显存
2. [架构设计] 只比较 shape 相等就判断可 view 互转，忽略 stride/storage_offset
3. [架构设计] 以为 pin_memory 总有效，实际它只对 non_blocking 有加速意义

#### 9. 实践与验证

画出组件-数据流图，标注每个接口的前置/后置条件，用最小可运行原型验证边界。具体到 张量/设备：构造 (2,3,4) 后断言 t.stride()==(12,4,1) 且 t.is_contiguous()；用 t.data_ptr() 验证 view 与 reshape 的共享/拷贝差异。把 对非连续张量 view 报 'view size is not compatible with input tensor's size and stride' 作为负例，记录 t.stride()/t.is_contiguous() 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“张量/设备”的名词，说不出「view 与源共享 storage（data_ptr 与 storage_offset 一致）」或 对非连续张量 view 报 'view size is not compatible with input tensor's size and stride'。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 显存(bytes)=numel×itemsize(dtype)；连续 stride[i]=∏_{j>i}shape[j] 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 跨设备运算报 'Expected all tensors to be on the same device' 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A012. 架构设计：autograd

- 难度：高级
- 知识域：K02 autograd
- 考察能力：变量、因果链、复杂度与容量上限
- 题型：原理推导与复杂度
- 面试频率：中频
- 前置知识：requires_grad、leaf、view、detach、反向
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

先用契约、状态所有权和失败模式划清系统边界，再谈实现细节。本题位于 K02 autograd。只有 requires_grad=True 的叶子张量在反向时把梯度累加到 .grad，非叶子的 .grad 默认是 None。

#### 2. 问题分析

从“架构设计：autograd”出发，先固定输入契约与资源约束，把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 Tensor.requires_grad_ 的契约，列出两条不变量：「叶子张量 .grad 的形状与张量一致且只由叶子持有」「requires_grad 沿 grad_fn 链单向传播」，再给一个会击穿假设的边界输入，并说明它最先暴露 忘记 zero_grad 使梯度累加 还是 对 requires_grad 的叶子做原地操作报 'a leaf Variable that requires grad is being used in an in-place operation'。

#### 3. 具体答案

面试中可以这样回答：

> 我会先给总体架构与组件契约，再给两条不变量和一个会击穿设计的边界输入。在 autograd 上，关键事实是：backward 默认累加而非覆盖，所以每个 step 前需 zero_grad，否则梯度跨 step 求和。 判据可写为 累加语义 g←g+∂L/∂θ；链式法则 dL/dx=Σ(dL/dy·dy/dx)。

#### 4. 技术洞察

架构的价值在于可验证的不变量（谁拥有状态、何时同步、失败如何回滚），而非单个性能数字。在 autograd 上，量化关系是 累加语义 g←g+∂L/∂θ；链式法则 dL/dx=Σ(dL/dy·dy/dx)；核心不变量是「叶子张量 .grad 的形状与张量一致且只由叶子持有」，它一旦被打破，最先表现为 两次 backward 未设 retain_graph 触发 RuntimeError。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | Tensor.requires_grad_ 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | 叶子张量 .grad 的形状与张量一致且只由叶子持有 | 违反「requires_grad 沿 grad_fn 链单向传播」时的报错 |
| 量化关系 | 累加语义 g←g+∂L/∂θ；链式法则 dL/dx=Σ(dL/dy·dy/dx) | 实测与预测偏差超容差 |
| 失败模式 | 忘记 zero_grad 使梯度累加 | 对 requires_grad 的叶子做原地操作报 'a leaf Variable that requires grad is being used in an in-place operation' |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 autograd 在“架构设计”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“架构设计：autograd”沿相邻边界延伸：detach、retain_grad、in-place 与版本计数、gradcheck。比较版本与替代实现的差异（如 torch.autograd.grad），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [架构设计] 为什么非叶子张量的 .grad 默认是 None，retain_grad() 改变了什么？
2. [架构设计] detach() 与 with torch.no_grad() 在切断梯度上的差别是什么？
3. [架构设计] in-place 操作在什么条件下能安全使用而不破坏 autograd？

#### 8. 易错点与反例

1. [架构设计] 认为非叶子 .grad 会自动填充，实则默认 None，需要 retain_grad()
2. [架构设计] 在 no_grad 下构建输入再 backward，得到 requires_grad 缺失错误
3. [架构设计] 用 param = param - lr*g 原地更新破坏计算图

#### 9. 实践与验证

画出组件-数据流图，标注每个接口的前置/后置条件，用最小可运行原型验证边界。具体到 autograd：断言 x.is_leaf and x.grad is None 后再 backward；用 torch.autograd.grad(..., retain_graph=True) 做两次反向对比。把 忘记 zero_grad 使梯度累加 作为负例，记录 Tensor.detach() 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“autograd”的名词，说不出「叶子张量 .grad 的形状与张量一致且只由叶子持有」或 忘记 zero_grad 使梯度累加。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 累加语义 g←g+∂L/∂θ；链式法则 dL/dx=Σ(dL/dy·dy/dx) 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 对 requires_grad 的叶子做原地操作报 'a leaf Variable that requires grad is being used in an in-place operation' 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A013. 架构设计：Module/状态

- 难度：高级
- 知识域：K03 Module/状态
- 考察能力：变量、因果链、复杂度与容量上限
- 题型：原理推导与复杂度
- 面试频率：中频
- 前置知识：参数、buffer、模式和 state_dict
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

先用契约、状态所有权和失败模式划清系统边界，再谈实现细节。本题位于 K03 Module/状态。buffer 参与 state_dict 但不是参数、不参与梯度；Parameter 两者都占。

#### 2. 问题分析

从“架构设计：Module/状态”出发，先固定输入契约与资源约束，把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 register_buffer 的契约，列出两条不变量：「同一 Module 的 state_dict key 集合确定且与设备无关」「buffer 的 dtype/device 跟随 module 或按注册时指定」，再给一个会击穿假设的边界输入，并说明它最先暴露 自定义 buffer 未注册，.to(device) 后仍留在 CPU 还是 load_state_dict 因 missing/unexpected keys 失败。

#### 3. 具体答案

面试中可以这样回答：

> 我会先给总体架构与组件契约，再给两条不变量和一个会击穿设计的边界输入。在 Module/状态 上，关键事实是：load_state_dict 默认 strict=True，缺失或多出 key 都会报错。 判据可写为 state_dict={k:param}∪{k:buffer}；训练态显存≈Σ numel×itemsize×(1+grad+opt_state)。

#### 4. 技术洞察

架构的价值在于可验证的不变量（谁拥有状态、何时同步、失败如何回滚），而非单个性能数字。在 Module/状态 上，量化关系是 state_dict={k:param}∪{k:buffer}；训练态显存≈Σ numel×itemsize×(1+grad+opt_state)；核心不变量是「同一 Module 的 state_dict key 集合确定且与设备无关」，它一旦被打破，最先表现为 eval 下忘记切换使 Dropout 仍在抽样。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | register_buffer 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | 同一 Module 的 state_dict key 集合确定且与设备无关 | 违反「buffer 的 dtype/device 跟随 module 或按注册时指定」时的报错 |
| 量化关系 | state_dict={k:param}∪{k:buffer}；训练态显存≈Σ numel×itemsize×(1+grad+opt_state) | 实测与预测偏差超容差 |
| 失败模式 | 自定义 buffer 未注册，.to(device) 后仍留在 CPU | load_state_dict 因 missing/unexpected keys 失败 |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 Module/状态 在“架构设计”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“架构设计：Module/状态”沿相邻边界延伸：persistent buffer、hook 与状态迁移、DDP 下的 buffer 同步。比较版本与替代实现的差异（如 state_dict/load_state_dict），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [架构设计] buffer 与 Parameter 在 state_dict、梯度和设备迁移上的差异是什么？
2. [架构设计] BatchNorm 在 eval 模式仍更新 running stats 需要什么额外设置？
3. [架构设计] register_buffer(persistent=False) 会从 state_dict 中移除什么？

#### 8. 易错点与反例

1. [架构设计] 把计数/统计量存成普通属性，导致 state_dict 丢失该状态
2. [架构设计] load_state_dict 后忘记 module.eval()，使 BN/Dropout 行为不符
3. [架构设计] 直接给同名属性赋值绕过注册，使 named_parameters 缺失

#### 9. 实践与验证

画出组件-数据流图，标注每个接口的前置/后置条件，用最小可运行原型验证边界。具体到 Module/状态：断言 set(m.state_dict().keys()) 与 checkpoint 完全一致；对比 train/eval 两次前向输出验证 Dropout 关闭。把 自定义 buffer 未注册，.to(device) 后仍留在 CPU 作为负例，记录 register_parameter 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“Module/状态”的名词，说不出「同一 Module 的 state_dict key 集合确定且与设备无关」或 自定义 buffer 未注册，.to(device) 后仍留在 CPU。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 state_dict={k:param}∪{k:buffer}；训练态显存≈Σ numel×itemsize×(1+grad+opt_state) 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 load_state_dict 因 missing/unexpected keys 失败 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A014. 架构设计：数据管道

- 难度：高级
- 知识域：K04 数据管道
- 考察能力：变量、因果链、复杂度与容量上限
- 题型：原理推导与复杂度
- 面试频率：中频
- 前置知识：Dataset、Sampler、collate、padding、版本
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

先用契约、状态所有权和失败模式划清系统边界，再谈实现细节。本题位于 K04 数据管道。num_workers=0 在主进程取数，>0 时 fork 子进程，每个 worker 有独立 Python 状态与 seed。

#### 2. 问题分析

从“架构设计：数据管道”出发，先固定输入契约与资源约束，把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 DataLoader(num_workers, pin_memory, persistent_workers) 的契约，列出两条不变量：「shuffle=False 且种子固定时迭代顺序可复现」「各 rank 采样集合互斥且并集覆盖全集（或按 drop_last 截断）」，再给一个会击穿假设的边界输入，并说明它最先暴露 多 worker 共享同一随机源导致样本重复 还是 worker 数过多耗尽文件描述符或内存。

#### 3. 具体答案

面试中可以这样回答：

> 我会先给总体架构与组件契约，再给两条不变量和一个会击穿设计的边界输入。在 数据管道 上，关键事实是：DistributedSampler 按 rank 切分样本，样本数不能被 world_size 整除时默认补齐以避免各 rank 步数不同。 判据可写为 吞吐≤min(存储带宽, CPU 解码速率, H2D 带宽)；有效样本/步=batch×world_size。

#### 4. 技术洞察

架构的价值在于可验证的不变量（谁拥有状态、何时同步、失败如何回滚），而非单个性能数字。在 数据管道 上，量化关系是 吞吐≤min(存储带宽, CPU 解码速率, H2D 带宽)；有效样本/步=batch×world_size；核心不变量是「shuffle=False 且种子固定时迭代顺序可复现」，它一旦被打破，最先表现为 DistributedSampler 与自定义 shuffle 双打乱造成跨 rank 重复。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | DataLoader(num_workers, pin_memory, persistent_workers) 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | shuffle=False 且种子固定时迭代顺序可复现 | 违反「各 rank 采样集合互斥且并集覆盖全集（或按 drop_last 截断）」时的报错 |
| 量化关系 | 吞吐≤min(存储带宽, CPU 解码速率, H2D 带宽)；有效样本/步=batch×world_size | 实测与预测偏差超容差 |
| 失败模式 | 多 worker 共享同一随机源导致样本重复 | worker 数过多耗尽文件描述符或内存 |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 数据管道 在“架构设计”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“架构设计：数据管道”沿相邻边界延伸：IterableDataset、chunk 预取、对象存储吞吐与 IPC 开销。比较版本与替代实现的差异（如 collate_fn），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [架构设计] DistributedSampler 如何处理样本数不被 world_size 整除，是否引入重复样本？
2. [架构设计] num_workers 增大时首步延迟、内存与 IPC 开销如何变化？
3. [架构设计] pin_memory 与 non_blocking 为何必须同时开启才有效？

#### 8. 易错点与反例

1. [架构设计] 把 num_workers 调大就以为更快，忽略 IPC 与内存放大
2. [架构设计] Dataset 里用全局随机而不经 worker_init_fn 重播种
3. [架构设计] 自己 shuffle 又用 DistributedSampler，导致跨 rank 样本重复

#### 9. 实践与验证

画出组件-数据流图，标注每个接口的前置/后置条件，用最小可运行原型验证边界。具体到 数据管道：固定 seed 两次迭代断言顺序一致；统计各 rank 采样索引并集是否覆盖全集。把 多 worker 共享同一随机源导致样本重复 作为负例，记录 DistributedSampler 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“数据管道”的名词，说不出「shuffle=False 且种子固定时迭代顺序可复现」或 多 worker 共享同一随机源导致样本重复。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 吞吐≤min(存储带宽, CPU 解码速率, H2D 带宽)；有效样本/步=batch×world_size 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 worker 数过多耗尽文件描述符或内存 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A015. 架构设计：训练/优化器

- 难度：高级
- 知识域：K05 训练/优化器
- 考察能力：变量、因果链、复杂度与容量上限
- 题型：原理推导与复杂度
- 面试频率：中频
- 前置知识：forward、backward、累积、optimizer、scheduler
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

先用契约、状态所有权和失败模式划清系统边界，再谈实现细节。本题位于 K05 训练/优化器。set_to_none=True 把 .grad 置 None 而非填零，省显存并让后续加法重新分配。

#### 2. 问题分析

从“架构设计：训练/优化器”出发，先固定输入契约与资源约束，把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 optimizer.zero_grad(set_to_none=True) 的契约，列出两条不变量：「反向之前梯度应为空或已清零」「scheduler 与 optimizer 的 step 次数对齐」，再给一个会击穿假设的边界输入，并说明它最先暴露 scheduler.step() 放错位置使 lr 序列整体偏移 还是 累积时直接累加 loss 未归一化。

#### 3. 具体答案

面试中可以这样回答：

> 我会先给总体架构与组件契约，再给两条不变量和一个会击穿设计的边界输入。在 训练/优化器 上，关键事实是：PyTorch 1.1.0 起 lr_scheduler.step() 应在 optimizer.step() 之后调用；顺序错误只给 warning 并产生错误 lr。 判据可写为 有效 batch=micro_batch×grad_accum×world_size；Adam 状态≈2P(状态)+P(梯度)。

#### 4. 技术洞察

架构的价值在于可验证的不变量（谁拥有状态、何时同步、失败如何回滚），而非单个性能数字。在 训练/优化器 上，量化关系是 有效 batch=micro_batch×grad_accum×world_size；Adam 状态≈2P(状态)+P(梯度)；核心不变量是「反向之前梯度应为空或已清零」，它一旦被打破，最先表现为 zero_grad 漏调导致梯度累加。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | optimizer.zero_grad(set_to_none=True) 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | 反向之前梯度应为空或已清零 | 违反「scheduler 与 optimizer 的 step 次数对齐」时的报错 |
| 量化关系 | 有效 batch=micro_batch×grad_accum×world_size；Adam 状态≈2P(状态)+P(梯度) | 实测与预测偏差超容差 |
| 失败模式 | scheduler.step() 放错位置使 lr 序列整体偏移 | 累积时直接累加 loss 未归一化 |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 训练/优化器 在“架构设计”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“架构设计：训练/优化器”沿相邻边界延伸：scheduler 恢复、优化器 state_dict、foreach/fused 实现差异。比较版本与替代实现的差异（如 lr_scheduler.step()），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [架构设计] lr_scheduler.step() 在 PyTorch 1.1.0 前后位置的语义差异是什么？
2. [架构设计] 梯度累积如何在数学上等价于大 batch，需要哪些归一化？
3. [架构设计] set_to_none=True 对显存与优化器行为有何具体影响？

#### 8. 易错点与反例

1. [架构设计] 用 zero_grad() 不传 set_to_none，长期分配开销更高
2. [架构设计] 把 scheduler.step() 在 epoch 与 step 间混用，导致 lr 跳变
3. [架构设计] 梯度累积时未除 steps，使等效学习率偏大

#### 9. 实践与验证

画出组件-数据流图，标注每个接口的前置/后置条件，用最小可运行原型验证边界。具体到 训练/优化器：打印前若干步 param_groups[0]['lr'] 验证调度曲线；比较累积 N 步与单步大 batch 的梯度 L2 范数。把 scheduler.step() 放错位置使 lr 序列整体偏移 作为负例，记录 optimizer.step() 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“训练/优化器”的名词，说不出「反向之前梯度应为空或已清零」或 scheduler.step() 放错位置使 lr 序列整体偏移。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 有效 batch=micro_batch×grad_accum×world_size；Adam 状态≈2P(状态)+P(梯度) 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 累积时直接累加 loss 未归一化 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A016. 架构设计：AMP/恢复

- 难度：高级
- 知识域：K06 AMP/恢复
- 考察能力：变量、因果链、复杂度与容量上限
- 题型：原理推导与复杂度
- 面试频率：中频
- 前置知识：autocast、scaler、快照、随机状态
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

先用契约、状态所有权和失败模式划清系统边界，再谈实现细节。本题位于 K06 AMP/恢复。GradScaler 默认 init_scale=65536、growth_factor=2.0、backoff_factor=0.5、growth_interval=2000。

#### 2. 问题分析

从“架构设计：AMP/恢复”出发，先固定输入契约与资源约束，把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 torch.autocast(device_type) 的契约，列出两条不变量：「反向前 loss 被 scale 放大，step 前梯度必须被 unscale」「inf/nan 检查与 skip 分支成对出现」，再给一个会击穿假设的边界输入，并说明它最先暴露 clip 放在 unscale 之前使梯度幅度错误 还是 对 bf16 仍用 GradScaler 造成无谓开销。

#### 3. 具体答案

面试中可以这样回答：

> 我会先给总体架构与组件契约，再给两条不变量和一个会击穿设计的边界输入。在 AMP/恢复 上，关键事实是：出现 inf/nan 时 scaler.step 跳过本次更新并降低 scale，避免污染权重。 判据可写为 scale_{new}=scale×growth_factor（连续无溢出）或 scale/backoff_factor（检测到 inf/nan）。

#### 4. 技术洞察

架构的价值在于可验证的不变量（谁拥有状态、何时同步、失败如何回滚），而非单个性能数字。在 AMP/恢复 上，量化关系是 scale_{new}=scale×growth_factor（连续无溢出）或 scale/backoff_factor（检测到 inf/nan）；核心不变量是「反向前 loss 被 scale 放大，step 前梯度必须被 unscale」，它一旦被打破，最先表现为 checkpoint 未保存 scaler.state_dict 使恢复后 scale 不连续。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | torch.autocast(device_type) 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | 反向前 loss 被 scale 放大，step 前梯度必须被 unscale | 违反「inf/nan 检查与 skip 分支成对出现」时的报错 |
| 量化关系 | scale_{new}=scale×growth_factor（连续无溢出）或 scale/backoff_factor（检测到 inf/nan） | 实测与预测偏差超容差 |
| 失败模式 | clip 放在 unscale 之前使梯度幅度错误 | 对 bf16 仍用 GradScaler 造成无谓开销 |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 AMP/恢复 在“架构设计”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“架构设计：AMP/恢复”沿相邻边界延伸：autocast 的 op 白/黑名单、随机状态快照、断点续训一致性。比较版本与替代实现的差异（如 scaler.scale/step/update），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [架构设计] GradScaler 连续多少次无溢出才提高 scale，默认参数是多少？
2. [架构设计] 为什么 fp16 autocast 必须 unscale 之后才能裁剪梯度？
3. [架构设计] bf16 为什么通常不需要 GradScaler？

#### 8. 易错点与反例

1. [架构设计] 以为 autocast 会把参数也转 fp16（参数保持 fp32）
2. [架构设计] 用未 unscale 的梯度直接裁剪
3. [架构设计] 恢复训练时忘记保存/加载 scaler 状态

#### 9. 实践与验证

画出组件-数据流图，标注每个接口的前置/后置条件，用最小可运行原型验证边界。具体到 AMP/恢复：记录 scaler.get_scale() 在若干 step 的轨迹；注入含 nan 的 loss 验证该 step 被跳过。把 clip 放在 unscale 之前使梯度幅度错误 作为负例，记录 GradScaler 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“AMP/恢复”的名词，说不出「反向前 loss 被 scale 放大，step 前梯度必须被 unscale」或 clip 放在 unscale 之前使梯度幅度错误。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 scale_{new}=scale×growth_factor（连续无溢出）或 scale/backoff_factor（检测到 inf/nan） 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 对 bf16 仍用 GradScaler 造成无谓开销 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A017. 架构设计：分布式

- 难度：高级
- 知识域：K07 分布式
- 考察能力：变量、因果链、复杂度与容量上限
- 题型：原理推导与复杂度
- 面试频率：中频
- 前置知识：rank、DDP、FSDP、collective、容错
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

先用契约、状态所有权和失败模式划清系统边界，再谈实现细节。本题位于 K07 分布式。DDP 在反向中把梯度按 bucket（默认 bucket_cap_mb=25）allreduce 并与反向计算重叠。

#### 2. 问题分析

从“架构设计：分布式”出发，先固定输入契约与资源约束，把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 init_process_group 的契约，列出两条不变量：「所有 rank 的 rank/world_size 一致」「collective 调用序列在各 rank 严格一致」，再给一个会击穿假设的边界输入，并说明它最先暴露 某些 rank 提前 return 使 collective 不匹配而 hang 还是 梯度 size 不一致触发 'Mismatch in collective'。

#### 3. 具体答案

面试中可以这样回答：

> 我会先给总体架构与组件契约，再给两条不变量和一个会击穿设计的边界输入。在 分布式 上，关键事实是：FSDP 把参数、梯度、优化器状态分片到各 rank：前向 all_gather 参数、反向 reduce_scatter 梯度。 判据可写为 ring allreduce≈2(N-1)/N×M；DDP 每卡显存≈params+grads+opt_states。

#### 4. 技术洞察

架构的价值在于可验证的不变量（谁拥有状态、何时同步、失败如何回滚），而非单个性能数字。在 分布式 上，量化关系是 ring allreduce≈2(N-1)/N×M；DDP 每卡显存≈params+grads+opt_states；核心不变量是「所有 rank 的 rank/world_size 一致」，它一旦被打破，最先表现为 FSDP 未 wrap 子模块使参数无法分片。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | init_process_group 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | 所有 rank 的 rank/world_size 一致 | 违反「collective 调用序列在各 rank 严格一致」时的报错 |
| 量化关系 | ring allreduce≈2(N-1)/N×M；DDP 每卡显存≈params+grads+opt_states | 实测与预测偏差超容差 |
| 失败模式 | 某些 rank 提前 return 使 collective 不匹配而 hang | 梯度 size 不一致触发 'Mismatch in collective' |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 分布式 在“架构设计”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“架构设计：分布式”沿相邻边界延伸：process group 切分、混合并行、NCCL 拓扑与超时。比较版本与替代实现的差异（如 FullyShardedDataParallel），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [架构设计] DDP 的梯度 bucketing 如何影响通信与计算重叠？
2. [架构设计] FSDP 的 all_gather 与 reduce_scatter 分别在前向/反向何处发生？
3. [架构设计] collective 调用顺序不一致时表现为 hang 还是报错，原因是什么？

#### 8. 易错点与反例

1. [架构设计] 对非参数 buffer 手动 allreduce，造成重复同步
2. [架构设计] 认为 FSDP 降低单卡计算量（实际只降显存）
3. [架构设计] 忽略 bucket_cap_mb 对重叠与峰值显存的影响

#### 9. 实践与验证

画出组件-数据流图，标注每个接口的前置/后置条件，用最小可运行原型验证边界。具体到 分布式：barrier 后断言 get_rank()/get_world_size()；对比 DDP 与 FSDP 的 max_memory_allocated。把 某些 rank 提前 return 使 collective 不匹配而 hang 作为负例，记录 DistributedDataParallel 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“分布式”的名词，说不出「所有 rank 的 rank/world_size 一致」或 某些 rank 提前 return 使 collective 不匹配而 hang。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 ring allreduce≈2(N-1)/N×M；DDP 每卡显存≈params+grads+opt_states 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 梯度 size 不一致触发 'Mismatch in collective' 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A018. 架构设计：编译栈

- 难度：高级
- 知识域：K08 编译栈
- 考察能力：变量、因果链、复杂度与容量上限
- 题型：原理推导与复杂度
- 面试频率：中频
- 前置知识：Dynamo、AOTAutograd、Inductor、guard、缓存
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

先用契约、状态所有权和失败模式划清系统边界，再谈实现细节。本题位于 K08 编译栈。Dynamo 捕获 Python 字节码生成 FX 图，对类型/形状/值建立 guard；guard 失效即重编译。

#### 2. 问题分析

从“架构设计：编译栈”出发，先固定输入契约与资源约束，把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 torch.compile 的契约，列出两条不变量：「fullgraph=True 时任何 graph break 都抛错」「同一 guard 命中后不再触发编译」，再给一个会击穿假设的边界输入，并说明它最先暴露 动态 shape 未标记导致反复重编译 还是 数据相关控制流触发 graph break。

#### 3. 具体答案

面试中可以这样回答：

> 我会先给总体架构与组件契约，再给两条不变量和一个会击穿设计的边界输入。在 编译栈 上，关键事实是：graph break 把图切段回退 eager，破坏端到端融合。 判据可写为 编译缓存以 guard 值集合为键；重编译次数∝出现过的不同 guard 组合数。

#### 4. 技术洞察

架构的价值在于可验证的不变量（谁拥有状态、何时同步、失败如何回滚），而非单个性能数字。在 编译栈 上，量化关系是 编译缓存以 guard 值集合为键；重编译次数∝出现过的不同 guard 组合数；核心不变量是「fullgraph=True 时任何 graph break 都抛错」，它一旦被打破，最先表现为 CUDA Graph 因输入地址变化捕获失败。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | torch.compile 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | fullgraph=True 时任何 graph break 都抛错 | 违反「同一 guard 命中后不再触发编译」时的报错 |
| 量化关系 | 编译缓存以 guard 值集合为键；重编译次数∝出现过的不同 guard 组合数 | 实测与预测偏差超容差 |
| 失败模式 | 动态 shape 未标记导致反复重编译 | 数据相关控制流触发 graph break |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 编译栈 在“架构设计”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“架构设计：编译栈”沿相邻边界延伸：dynamic shape 的 Dim/mark_dynamic、guard 导出、AOT 与自定义 op。比较版本与替代实现的差异（如 TORCH_LOGS=graph_breaks,recompiles），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [架构设计] Dynamo 的 guard 在什么条件下失效并触发重编译？
2. [架构设计] graph break 对 autograd 图切分与状态副作用带来什么风险？
3. [架构设计] reduce-overhead 的 CUDA Graph 为何要求输入地址固定？

#### 8. 易错点与反例

1. [架构设计] 以为 graph break 只影响性能，忽略断点两侧的状态副作用
2. [架构设计] 用固定 shape 基准掩盖动态 shape 的重编译
3. [架构设计] 忽略首次编译冷启动开销

#### 9. 实践与验证

画出组件-数据流图，标注每个接口的前置/后置条件，用最小可运行原型验证边界。具体到 编译栈：用 torch._dynamo.explain 统计 graph_breaks 数量；监控 counters['inductor']['recompiles']。把 动态 shape 未标记导致反复重编译 作为负例，记录 torch._dynamo.explain 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“编译栈”的名词，说不出「fullgraph=True 时任何 graph break 都抛错」或 动态 shape 未标记导致反复重编译。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 编译缓存以 guard 值集合为键；重编译次数∝出现过的不同 guard 组合数 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 数据相关控制流触发 graph break 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A019. 架构设计：性能

- 难度：高级
- 知识域：K09 性能
- 考察能力：变量、因果链、复杂度与容量上限
- 题型：原理推导与复杂度
- 面试频率：中频
- 前置知识：关键路径、带宽、算力、通信、显存、统计
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

先用契约、状态所有权和失败模式划清系统边界，再谈实现细节。本题位于 K09 性能。roofline 下 kernel 时间≈max(FLOPs/峰值算力, 访存字节/峰值带宽)，取决于算术强度。

#### 2. 问题分析

从“架构设计：性能”出发，先固定输入契约与资源约束，把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 torch.profiler 的契约，列出两条不变量：「所有 kernel 时间之和≤墙钟时间，差额为同步/空泡」「峰值显存≥活跃张量之和+碎片+缓存块」，再给一个会击穿假设的边界输入，并说明它最先暴露 未 sync 就读 Event 时间 还是 只测稳态忽略首次冷启动。

#### 3. 具体答案

面试中可以这样回答：

> 我会先给总体架构与组件契约，再给两条不变量和一个会击穿设计的边界输入。在 性能 上，关键事实是：GPU 计时必须 warmup 并 torch.cuda.synchronize()，否则测到的是入队时间。 判据可写为 t≈max(FLOPs/FLOPS_peak, Bytes/BW_peak)；算术强度=FLOPs/Bytes。

#### 4. 技术洞察

架构的价值在于可验证的不变量（谁拥有状态、何时同步、失败如何回滚），而非单个性能数字。在 性能 上，量化关系是 t≈max(FLOPs/FLOPS_peak, Bytes/BW_peak)；算术强度=FLOPs/Bytes；核心不变量是「所有 kernel 时间之和≤墙钟时间，差额为同步/空泡」，它一旦被打破，最先表现为 把 H2D 拷贝算进 GPU 计算时间。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | torch.profiler 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | 所有 kernel 时间之和≤墙钟时间，差额为同步/空泡 | 违反「峰值显存≥活跃张量之和+碎片+缓存块」时的报错 |
| 量化关系 | t≈max(FLOPs/FLOPS_peak, Bytes/BW_peak)；算术强度=FLOPs/Bytes | 实测与预测偏差超容差 |
| 失败模式 | 未 sync 就读 Event 时间 | 只测稳态忽略首次冷启动 |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 性能 在“架构设计”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“架构设计：性能”沿相邻边界延伸：roofline 拐点、显存碎片与快照、通信/计算重叠度量。比较版本与替代实现的差异（如 torch.cuda.max_memory_allocated），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [架构设计] 如何用 roofline 判断算子 compute-bound 还是 memory-bound，需要哪些输入？
2. [架构设计] 为什么 profiler 的 self_cuda_time 之和常小于 step time？
3. [架构设计] 在通信与计算未重叠时，先改数据并行还是分片/bucket 策略？

#### 8. 易错点与反例

1. [架构设计] 用 CPU 计时测 GPU kernel，得到错误结论
2. [架构设计] 忽略通信与 sync 造成的时间空泡
3. [架构设计] 以单次测量下结论，被启动噪声误导

#### 9. 实践与验证

画出组件-数据流图，标注每个接口的前置/后置条件，用最小可运行原型验证边界。具体到 性能：profiler 导出 kernel 表按 cuda_time 排序定位热点；固定输入重复 N 次取中位数与 p99。把 未 sync 就读 Event 时间 作为负例，记录 torch.cuda.Event 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“性能”的名词，说不出「所有 kernel 时间之和≤墙钟时间，差额为同步/空泡」或 未 sync 就读 Event 时间。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 t≈max(FLOPs/FLOPS_peak, Bytes/BW_peak)；算术强度=FLOPs/Bytes 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 只测稳态忽略首次冷启动 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A020. 架构设计：导出部署

- 难度：高级
- 知识域：K10 导出部署
- 考察能力：变量、因果链、复杂度与容量上限
- 题型：原理推导与复杂度
- 面试频率：中频
- 前置知识：state_dict、Export、runtime、schema、迁移
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

先用契约、状态所有权和失败模式划清系统边界，再谈实现细节。本题位于 K10 序列化/导出/部署。PyTorch 2.6 起 torch.load 的 weights_only 默认为 True，加载含自定义类的 pickle 需显式关闭。

#### 2. 问题分析

从“架构设计：序列化/导出/部署”出发，先固定输入契约与资源约束，把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 torch.save/torch.load(weights_only=) 的契约，列出两条不变量：「导出后的图不含 Python 语义，仅含 ATen/custom op 调用」「state_dict 的 key 语义与训练侧一致」，再给一个会击穿假设的边界输入，并说明它最先暴露 trace 把数据相关分支固化为单一路径 还是 自定义 op 未注册导致导出失败。

#### 3. 具体答案

面试中可以这样回答：

> 我会先给总体架构与组件契约，再给两条不变量和一个会击穿设计的边界输入。在 序列化/导出/部署 上，关键事实是：torch.jit.trace 记录一次执行路径，忽略数据相关控制流（if/for 会塌缩）。 判据可写为 导出图=静态化 Python 控制流+固定/声明化 shape（Dim 约束）。

#### 4. 技术洞察

架构的价值在于可验证的不变量（谁拥有状态、何时同步、失败如何回滚），而非单个性能数字。在 序列化/导出/部署 上，量化关系是 导出图=静态化 Python 控制流+固定/声明化 shape（Dim 约束）；核心不变量是「导出后的图不含 Python 语义，仅含 ATen/custom op 调用」，它一旦被打破，最先表现为 weights_only=True 加载旧 pickle 报错。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | torch.save/torch.load(weights_only=) 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | 导出后的图不含 Python 语义，仅含 ATen/custom op 调用 | 违反「state_dict 的 key 语义与训练侧一致」时的报错 |
| 量化关系 | 导出图=静态化 Python 控制流+固定/声明化 shape（Dim 约束） | 实测与预测偏差超容差 |
| 失败模式 | trace 把数据相关分支固化为单一路径 | 自定义 op 未注册导致导出失败 |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 序列化/导出/部署 在“架构设计”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“架构设计：序列化/导出/部署”沿相邻边界延伸：custom op 的 register_autograd、ONNX/ExecuTorch runtime、版本 schema 演进。比较版本与替代实现的差异（如 torch.export.export），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [架构设计] torch.jit.trace 与 torch.export 在控制流处理上有什么本质区别？
2. [架构设计] weights_only 自 2.6 起默认为 True 带来哪些兼容性影响？
3. [架构设计] 如何用 dynamic_shapes 表达一个仅序列维动态的模型并验证？

#### 8. 易错点与反例

1. [架构设计] 用 trace 导出含 if/loop 的模型，使数据相关分支被固化
2. [架构设计] 认为 weights_only=True 永远安全（自定义 op/类仍需注册）
3. [架构设计] 忽略导出后 dtype/device 与运行时不一致

#### 9. 实践与验证

画出组件-数据流图，标注每个接口的前置/后置条件，用最小可运行原型验证边界。具体到 序列化/导出/部署：对比导出前后在相同输入上的输出 assert_close；检查 ExportedProgram.graph_module.graph 的 op 集合。把 trace 把数据相关分支固化为单一路径 作为负例，记录 torch.jit.trace/script 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“序列化/导出/部署”的名词，说不出「导出后的图不含 Python 语义，仅含 ATen/custom op 调用」或 trace 把数据相关分支固化为单一路径。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 导出图=静态化 Python 控制流+固定/声明化 shape（Dim 约束） 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 自定义 op 未注册导致导出失败 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A021. 架构设计：自定义算子

- 难度：高级
- 知识域：K11 自定义算子
- 考察能力：事件顺序、状态迁移、所有权与观测点
- 题型：流程/调用链/数据流
- 面试频率：中频
- 前置知识：schema、dispatcher、stream、autograd、ABI
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

先用契约、状态所有权和失败模式划清系统边界，再谈实现细节。本题位于 K11 自定义算子。自定义算子要让 autograd 可用，必须为 Tensor 参数注册反向（Function 或 register_autograd）。

#### 2. 问题分析

从“架构设计：自定义算子”出发，先固定输入契约与资源约束，把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 torch.library/TORCH_LIBRARY 的契约，列出两条不变量：「torch.autograd.gradcheck 通过表示前向/反向数值一致」「no_grad 与 grad 模式除梯度外结果一致」，再给一个会击穿假设的边界输入，并说明它最先暴露 漏写设备选择/CUDAGuard 使多卡下用错设备 还是 C++ ABI 不匹配报 undefined symbol。

#### 3. 具体答案

面试中可以这样回答：

> 我会先给总体架构与组件契约，再给两条不变量和一个会击穿设计的边界输入。在 自定义算子 上，关键事实是：kernel 必须在当前 CUDA stream 上执行并保证输入/输出的 stream 语义，否则与其它算子乱序。 判据可写为 加速比=t_baseline/t_custom（同 shape/dtype/stream）；occupancy=活跃 warp/最大 warp。

#### 4. 技术洞察

架构的价值在于可验证的不变量（谁拥有状态、何时同步、失败如何回滚），而非单个性能数字。在 自定义算子 上，量化关系是 加速比=t_baseline/t_custom（同 shape/dtype/stream）；occupancy=活跃 warp/最大 warp；核心不变量是「torch.autograd.gradcheck 通过表示前向/反向数值一致」，它一旦被打破，最先表现为 内核用原子加导致非确定性。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | torch.library/TORCH_LIBRARY 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | torch.autograd.gradcheck 通过表示前向/反向数值一致 | 违反「no_grad 与 grad 模式除梯度外结果一致」时的报错 |
| 量化关系 | 加速比=t_baseline/t_custom（同 shape/dtype/stream）；occupancy=活跃 warp/最大 warp | 实测与预测偏差超容差 |
| 失败模式 | 漏写设备选择/CUDAGuard 使多卡下用错设备 | C++ ABI 不匹配报 undefined symbol |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 自定义算子 在“架构设计”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“架构设计：自定义算子”沿相邻边界延伸：op schema 与 fake/meta 注册、流与事件同步、编译期架构选择。比较版本与替代实现的差异（如 torch.library.register_autograd），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [架构设计] 为自定义算子实现反向有哪两种机制，各自适用场景是什么？
2. [架构设计] 为什么 kernel 必须使用当前 stream，如何获取与同步？
3. [架构设计] CUDA 扩展的 C++ ABI 不匹配会以什么形式暴露？

#### 8. 易错点与反例

1. [架构设计] 只写前向不注册反向，autograd 报错或梯度为 None
2. [架构设计] 在默认 stream 上跑 kernel 却从非默认 stream 读数据
3. [架构设计] 忽略 C++ ABI 造成运行时链接失败

#### 9. 实践与验证

画出组件-数据流图，标注每个接口的前置/后置条件，用最小可运行原型验证边界。具体到 自定义算子：gradcheck 校验自定义 Function 的梯度；torch.testing.assert_close 对比参考实现。把 漏写设备选择/CUDAGuard 使多卡下用错设备 作为负例，记录 torch.autograd.Function 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“自定义算子”的名词，说不出「torch.autograd.gradcheck 通过表示前向/反向数值一致」或 漏写设备选择/CUDAGuard 使多卡下用错设备。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 加速比=t_baseline/t_custom（同 shape/dtype/stream）；occupancy=活跃 warp/最大 warp 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 C++ ABI 不匹配报 undefined symbol 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A022. 架构设计：测试治理

- 难度：高级
- 知识域：K12 测试治理
- 考察能力：事件顺序、状态迁移、所有权与观测点
- 题型：流程/调用链/数据流
- 面试频率：中频
- 前置知识：oracle、证据、日志、回滚、门禁、复盘
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

先用契约、状态所有权和失败模式划清系统边界，再谈实现细节。本题位于 K12 测试治理。torch.testing.assert_close 对 fp32 默认 rtol=1.3e-6、atol=1e-5，对 fp16/bf16 阈值更宽。

#### 2. 问题分析

从“架构设计：测试治理”出发，先固定输入契约与资源约束，把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 torch.testing.assert_close 的契约，列出两条不变量：「相同 seed+相同算法下结果可复现」「通过阈值必须与 dtype 精度匹配」，再给一个会击穿假设的边界输入，并说明它最先暴露 对 fp16 用 fp32 阈值导致误报 还是 未固定 seed 造成随机失败。

#### 3. 具体答案

面试中可以这样回答：

> 我会先给总体架构与组件契约，再给两条不变量和一个会击穿设计的边界输入。在 测试治理 上，关键事实是：gradcheck 用有限差分对比解析梯度，默认 eps=1e-6、atol=1e-5。 判据可写为 |a-b|≤atol+rtol×|b|；flake 率≈失败次数/重跑次数。

#### 4. 技术洞察

架构的价值在于可验证的不变量（谁拥有状态、何时同步、失败如何回滚），而非单个性能数字。在 测试治理 上，量化关系是 |a-b|≤atol+rtol×|b|；flake 率≈失败次数/重跑次数；核心不变量是「相同 seed+相同算法下结果可复现」，它一旦被打破，最先表现为 把 flaky 当稳定失败反复重试掩盖问题。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | torch.testing.assert_close 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | 相同 seed+相同算法下结果可复现 | 违反「通过阈值必须与 dtype 精度匹配」时的报错 |
| 量化关系 | |a-b|≤atol+rtol×|b|；flake 率≈失败次数/重跑次数 | 实测与预测偏差超容差 |
| 失败模式 | 对 fp16 用 fp32 阈值导致误报 | 未固定 seed 造成随机失败 |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 测试治理 在“架构设计”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“架构设计：测试治理”沿相邻边界延伸：golden 值与版本记录、CI matrix 覆盖、门禁与回归集管理。比较版本与替代实现的差异（如 pytest），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [架构设计] assert_close 的 rtol/atol 如何按 dtype 选择才合理？
2. [架构设计] gradcheck 在什么情况下会误报失败？
3. [架构设计] 如何量化并治理 flaky 测试？

#### 8. 易错点与反例

1. [架构设计] 用 == 比较浮点张量
2. [架构设计] 以为关闭 cuDNN benchmark 就能完全确定
3. [架构设计] 把 golden 值硬编码却不记录环境版本

#### 9. 实践与验证

画出组件-数据流图，标注每个接口的前置/后置条件，用最小可运行原型验证边界。具体到 测试治理：固定 seed 重跑 N 次统计通过率；用 assert_close 显式指定 rtol/atol 而非手写比较。把 对 fp16 用 fp32 阈值导致误报 作为负例，记录 torch.autograd.gradcheck 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“测试治理”的名词，说不出「相同 seed+相同算法下结果可复现」或 对 fp16 用 fp32 阈值导致误报。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 |a-b|≤atol+rtol×|b|；flake 率≈失败次数/重跑次数 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 未固定 seed 造成随机失败 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A023. 容量规划：张量/设备

- 难度：高级
- 知识域：K01 张量/设备
- 考察能力：事件顺序、状态迁移、所有权与观测点
- 题型：流程/调用链/数据流
- 面试频率：中频
- 前置知识：shape、stride、dtype、device
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

容量规划把资源需求写成可复算公式，再留尾部与故障余量。本题位于 K01 张量/设备。view 只改元数据且要求 stride 兼容；reshape 在不兼容时会静默拷贝，可用 t.data_ptr() 判断是否共享存储。

#### 2. 问题分析

从“容量规划：张量/设备”出发，先枚举资源维度（显存/带宽/存储/连接/队列），把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 torch.Tensor.view/reshape 的契约，列出两条不变量：「view 与源共享 storage（data_ptr 与 storage_offset 一致）」「张量的 device/dtype 决定 kernel 派发与是否发生隐式转换」，再给一个会击穿假设的边界输入，并说明它最先暴露 对非连续张量 view 报 'view size is not compatible with input tensor's size and stride' 还是 跨设备运算报 'Expected all tensors to be on the same device'。

#### 3. 具体答案

面试中可以这样回答：

> 我会给出资源公式、拐点位置、余量比例和最先被打满的那一项资源。在 张量/设备 上，关键事实是：显存占用=numel×element_size：fp32 每元素 4B，fp16/bf16 每元素 2B。 判据可写为 显存(bytes)=numel×itemsize(dtype)；连续 stride[i]=∏_{j>i}shape[j]。

#### 4. 技术洞察

容量上界由最紧约束决定：max(显存, 带宽, 算力, 存储)，而不是各项相加。在 张量/设备 上，量化关系是 显存(bytes)=numel×itemsize(dtype)；连续 stride[i]=∏_{j>i}shape[j]；核心不变量是「view 与源共享 storage（data_ptr 与 storage_offset 一致）」，它一旦被打破，最先表现为 reshape 静默拷贝抬高峰值显存。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | torch.Tensor.view/reshape 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | view 与源共享 storage（data_ptr 与 storage_offset 一致） | 违反「张量的 device/dtype 决定 kernel 派发与是否发生隐式转换」时的报错 |
| 量化关系 | 显存(bytes)=numel×itemsize(dtype)；连续 stride[i]=∏_{j>i}shape[j] | 实测与预测偏差超容差 |
| 失败模式 | 对非连续张量 view 报 'view size is not compatible with input tensor's size and stride' | 跨设备运算报 'Expected all tensors to be on the same device' |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 张量/设备 在“容量规划”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“容量规划：张量/设备”沿相邻边界延伸：view/reshape/contiguous、pin_memory 与 stream、dtype 转换与显存。比较版本与替代实现的差异（如 t.to(device, non_blocking=True)），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [容量规划] shape=(2,3,4) 分别 view 成 (6,4)、reshape 成 (6,4) 后，stride 与是否共享存储各是什么？
2. [容量规划] 为什么 permute 之后必须先 contiguous 才能 view？
3. [容量规划] non_blocking=True 在源未 pinned 时的实际行为如何用 profiler 证明？

#### 8. 易错点与反例

1. [容量规划] 把 reshape 当零拷贝：stride 不兼容时它会 contiguous 并分配新显存
2. [容量规划] 只比较 shape 相等就判断可 view 互转，忽略 stride/storage_offset
3. [容量规划] 以为 pin_memory 总有效，实际它只对 non_blocking 有加速意义

#### 9. 实践与验证

用单卡/单进程测单位成本，再线性外推并注入 20%~50% 余量验证拐点。具体到 张量/设备：构造 (2,3,4) 后断言 t.stride()==(12,4,1) 且 t.is_contiguous()；用 t.data_ptr() 验证 view 与 reshape 的共享/拷贝差异。把 对非连续张量 view 报 'view size is not compatible with input tensor's size and stride' 作为负例，记录 t.stride()/t.is_contiguous() 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“张量/设备”的名词，说不出「view 与源共享 storage（data_ptr 与 storage_offset 一致）」或 对非连续张量 view 报 'view size is not compatible with input tensor's size and stride'。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 显存(bytes)=numel×itemsize(dtype)；连续 stride[i]=∏_{j>i}shape[j] 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 跨设备运算报 'Expected all tensors to be on the same device' 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A024. 容量规划：autograd

- 难度：高级
- 知识域：K02 autograd
- 考察能力：事件顺序、状态迁移、所有权与观测点
- 题型：流程/调用链/数据流
- 面试频率：中频
- 前置知识：requires_grad、leaf、view、detach、反向
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

容量规划把资源需求写成可复算公式，再留尾部与故障余量。本题位于 K02 autograd。backward 默认累加而非覆盖，所以每个 step 前需 zero_grad，否则梯度跨 step 求和。

#### 2. 问题分析

从“容量规划：autograd”出发，先枚举资源维度（显存/带宽/存储/连接/队列），把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 Tensor.requires_grad_ 的契约，列出两条不变量：「叶子张量 .grad 的形状与张量一致且只由叶子持有」「requires_grad 沿 grad_fn 链单向传播」，再给一个会击穿假设的边界输入，并说明它最先暴露 忘记 zero_grad 使梯度累加 还是 对 requires_grad 的叶子做原地操作报 'a leaf Variable that requires grad is being used in an in-place operation'。

#### 3. 具体答案

面试中可以这样回答：

> 我会给出资源公式、拐点位置、余量比例和最先被打满的那一项资源。在 autograd 上，关键事实是：反向默认释放计算图，第二次反向需 retain_graph=True，否则报 'Trying to backward through the graph a second time'。 判据可写为 累加语义 g←g+∂L/∂θ；链式法则 dL/dx=Σ(dL/dy·dy/dx)。

#### 4. 技术洞察

容量上界由最紧约束决定：max(显存, 带宽, 算力, 存储)，而不是各项相加。在 autograd 上，量化关系是 累加语义 g←g+∂L/∂θ；链式法则 dL/dx=Σ(dL/dy·dy/dx)；核心不变量是「叶子张量 .grad 的形状与张量一致且只由叶子持有」，它一旦被打破，最先表现为 两次 backward 未设 retain_graph 触发 RuntimeError。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | Tensor.requires_grad_ 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | 叶子张量 .grad 的形状与张量一致且只由叶子持有 | 违反「requires_grad 沿 grad_fn 链单向传播」时的报错 |
| 量化关系 | 累加语义 g←g+∂L/∂θ；链式法则 dL/dx=Σ(dL/dy·dy/dx) | 实测与预测偏差超容差 |
| 失败模式 | 忘记 zero_grad 使梯度累加 | 对 requires_grad 的叶子做原地操作报 'a leaf Variable that requires grad is being used in an in-place operation' |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 autograd 在“容量规划”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“容量规划：autograd”沿相邻边界延伸：detach、retain_grad、in-place 与版本计数、gradcheck。比较版本与替代实现的差异（如 torch.autograd.grad），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [容量规划] 为什么非叶子张量的 .grad 默认是 None，retain_grad() 改变了什么？
2. [容量规划] detach() 与 with torch.no_grad() 在切断梯度上的差别是什么？
3. [容量规划] in-place 操作在什么条件下能安全使用而不破坏 autograd？

#### 8. 易错点与反例

1. [容量规划] 认为非叶子 .grad 会自动填充，实则默认 None，需要 retain_grad()
2. [容量规划] 在 no_grad 下构建输入再 backward，得到 requires_grad 缺失错误
3. [容量规划] 用 param = param - lr*g 原地更新破坏计算图

#### 9. 实践与验证

用单卡/单进程测单位成本，再线性外推并注入 20%~50% 余量验证拐点。具体到 autograd：断言 x.is_leaf and x.grad is None 后再 backward；用 torch.autograd.grad(..., retain_graph=True) 做两次反向对比。把 忘记 zero_grad 使梯度累加 作为负例，记录 Tensor.detach() 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“autograd”的名词，说不出「叶子张量 .grad 的形状与张量一致且只由叶子持有」或 忘记 zero_grad 使梯度累加。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 累加语义 g←g+∂L/∂θ；链式法则 dL/dx=Σ(dL/dy·dy/dx) 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 对 requires_grad 的叶子做原地操作报 'a leaf Variable that requires grad is being used in an in-place operation' 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A025. 容量规划：Module/状态

- 难度：高级
- 知识域：K03 Module/状态
- 考察能力：事件顺序、状态迁移、所有权与观测点
- 题型：流程/调用链/数据流
- 面试频率：中频
- 前置知识：参数、buffer、模式和 state_dict
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

容量规划把资源需求写成可复算公式，再留尾部与故障余量。本题位于 K03 Module/状态。load_state_dict 默认 strict=True，缺失或多出 key 都会报错。

#### 2. 问题分析

从“容量规划：Module/状态”出发，先枚举资源维度（显存/带宽/存储/连接/队列），把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 register_buffer 的契约，列出两条不变量：「同一 Module 的 state_dict key 集合确定且与设备无关」「buffer 的 dtype/device 跟随 module 或按注册时指定」，再给一个会击穿假设的边界输入，并说明它最先暴露 自定义 buffer 未注册，.to(device) 后仍留在 CPU 还是 load_state_dict 因 missing/unexpected keys 失败。

#### 3. 具体答案

面试中可以这样回答：

> 我会给出资源公式、拐点位置、余量比例和最先被打满的那一项资源。在 Module/状态 上，关键事实是：.eval() 把 self.training 置 False，切换 Dropout/BatchNorm 行为，但不改变 requires_grad。 判据可写为 state_dict={k:param}∪{k:buffer}；训练态显存≈Σ numel×itemsize×(1+grad+opt_state)。

#### 4. 技术洞察

容量上界由最紧约束决定：max(显存, 带宽, 算力, 存储)，而不是各项相加。在 Module/状态 上，量化关系是 state_dict={k:param}∪{k:buffer}；训练态显存≈Σ numel×itemsize×(1+grad+opt_state)；核心不变量是「同一 Module 的 state_dict key 集合确定且与设备无关」，它一旦被打破，最先表现为 eval 下忘记切换使 Dropout 仍在抽样。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | register_buffer 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | 同一 Module 的 state_dict key 集合确定且与设备无关 | 违反「buffer 的 dtype/device 跟随 module 或按注册时指定」时的报错 |
| 量化关系 | state_dict={k:param}∪{k:buffer}；训练态显存≈Σ numel×itemsize×(1+grad+opt_state) | 实测与预测偏差超容差 |
| 失败模式 | 自定义 buffer 未注册，.to(device) 后仍留在 CPU | load_state_dict 因 missing/unexpected keys 失败 |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 Module/状态 在“容量规划”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“容量规划：Module/状态”沿相邻边界延伸：persistent buffer、hook 与状态迁移、DDP 下的 buffer 同步。比较版本与替代实现的差异（如 state_dict/load_state_dict），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [容量规划] buffer 与 Parameter 在 state_dict、梯度和设备迁移上的差异是什么？
2. [容量规划] BatchNorm 在 eval 模式仍更新 running stats 需要什么额外设置？
3. [容量规划] register_buffer(persistent=False) 会从 state_dict 中移除什么？

#### 8. 易错点与反例

1. [容量规划] 把计数/统计量存成普通属性，导致 state_dict 丢失该状态
2. [容量规划] load_state_dict 后忘记 module.eval()，使 BN/Dropout 行为不符
3. [容量规划] 直接给同名属性赋值绕过注册，使 named_parameters 缺失

#### 9. 实践与验证

用单卡/单进程测单位成本，再线性外推并注入 20%~50% 余量验证拐点。具体到 Module/状态：断言 set(m.state_dict().keys()) 与 checkpoint 完全一致；对比 train/eval 两次前向输出验证 Dropout 关闭。把 自定义 buffer 未注册，.to(device) 后仍留在 CPU 作为负例，记录 register_parameter 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“Module/状态”的名词，说不出「同一 Module 的 state_dict key 集合确定且与设备无关」或 自定义 buffer 未注册，.to(device) 后仍留在 CPU。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 state_dict={k:param}∪{k:buffer}；训练态显存≈Σ numel×itemsize×(1+grad+opt_state) 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 load_state_dict 因 missing/unexpected keys 失败 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A026. 容量规划：数据管道

- 难度：高级
- 知识域：K04 数据管道
- 考察能力：事件顺序、状态迁移、所有权与观测点
- 题型：流程/调用链/数据流
- 面试频率：中频
- 前置知识：Dataset、Sampler、collate、padding、版本
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

容量规划把资源需求写成可复算公式，再留尾部与故障余量。本题位于 K04 数据管道。DistributedSampler 按 rank 切分样本，样本数不能被 world_size 整除时默认补齐以避免各 rank 步数不同。

#### 2. 问题分析

从“容量规划：数据管道”出发，先枚举资源维度（显存/带宽/存储/连接/队列），把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 DataLoader(num_workers, pin_memory, persistent_workers) 的契约，列出两条不变量：「shuffle=False 且种子固定时迭代顺序可复现」「各 rank 采样集合互斥且并集覆盖全集（或按 drop_last 截断）」，再给一个会击穿假设的边界输入，并说明它最先暴露 多 worker 共享同一随机源导致样本重复 还是 worker 数过多耗尽文件描述符或内存。

#### 3. 具体答案

面试中可以这样回答：

> 我会给出资源公式、拐点位置、余量比例和最先被打满的那一项资源。在 数据管道 上，关键事实是：persistent_workers=True 复用 worker，避免每个 epoch 重新 fork。 判据可写为 吞吐≤min(存储带宽, CPU 解码速率, H2D 带宽)；有效样本/步=batch×world_size。

#### 4. 技术洞察

容量上界由最紧约束决定：max(显存, 带宽, 算力, 存储)，而不是各项相加。在 数据管道 上，量化关系是 吞吐≤min(存储带宽, CPU 解码速率, H2D 带宽)；有效样本/步=batch×world_size；核心不变量是「shuffle=False 且种子固定时迭代顺序可复现」，它一旦被打破，最先表现为 DistributedSampler 与自定义 shuffle 双打乱造成跨 rank 重复。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | DataLoader(num_workers, pin_memory, persistent_workers) 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | shuffle=False 且种子固定时迭代顺序可复现 | 违反「各 rank 采样集合互斥且并集覆盖全集（或按 drop_last 截断）」时的报错 |
| 量化关系 | 吞吐≤min(存储带宽, CPU 解码速率, H2D 带宽)；有效样本/步=batch×world_size | 实测与预测偏差超容差 |
| 失败模式 | 多 worker 共享同一随机源导致样本重复 | worker 数过多耗尽文件描述符或内存 |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 数据管道 在“容量规划”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“容量规划：数据管道”沿相邻边界延伸：IterableDataset、chunk 预取、对象存储吞吐与 IPC 开销。比较版本与替代实现的差异（如 collate_fn），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [容量规划] DistributedSampler 如何处理样本数不被 world_size 整除，是否引入重复样本？
2. [容量规划] num_workers 增大时首步延迟、内存与 IPC 开销如何变化？
3. [容量规划] pin_memory 与 non_blocking 为何必须同时开启才有效？

#### 8. 易错点与反例

1. [容量规划] 把 num_workers 调大就以为更快，忽略 IPC 与内存放大
2. [容量规划] Dataset 里用全局随机而不经 worker_init_fn 重播种
3. [容量规划] 自己 shuffle 又用 DistributedSampler，导致跨 rank 样本重复

#### 9. 实践与验证

用单卡/单进程测单位成本，再线性外推并注入 20%~50% 余量验证拐点。具体到 数据管道：固定 seed 两次迭代断言顺序一致；统计各 rank 采样索引并集是否覆盖全集。把 多 worker 共享同一随机源导致样本重复 作为负例，记录 DistributedSampler 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“数据管道”的名词，说不出「shuffle=False 且种子固定时迭代顺序可复现」或 多 worker 共享同一随机源导致样本重复。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 吞吐≤min(存储带宽, CPU 解码速率, H2D 带宽)；有效样本/步=batch×world_size 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 worker 数过多耗尽文件描述符或内存 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A027. 容量规划：训练/优化器

- 难度：高级
- 知识域：K05 训练/优化器
- 考察能力：事件顺序、状态迁移、所有权与观测点
- 题型：流程/调用链/数据流
- 面试频率：中频
- 前置知识：forward、backward、累积、optimizer、scheduler
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

容量规划把资源需求写成可复算公式，再留尾部与故障余量。本题位于 K05 训练/优化器。PyTorch 1.1.0 起 lr_scheduler.step() 应在 optimizer.step() 之后调用；顺序错误只给 warning 并产生错误 lr。

#### 2. 问题分析

从“容量规划：训练/优化器”出发，先枚举资源维度（显存/带宽/存储/连接/队列），把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 optimizer.zero_grad(set_to_none=True) 的契约，列出两条不变量：「反向之前梯度应为空或已清零」「scheduler 与 optimizer 的 step 次数对齐」，再给一个会击穿假设的边界输入，并说明它最先暴露 scheduler.step() 放错位置使 lr 序列整体偏移 还是 累积时直接累加 loss 未归一化。

#### 3. 具体答案

面试中可以这样回答：

> 我会给出资源公式、拐点位置、余量比例和最先被打满的那一项资源。在 训练/优化器 上，关键事实是：梯度累积须按累积步数折算 loss（loss/steps），否则等效学习率被放大。 判据可写为 有效 batch=micro_batch×grad_accum×world_size；Adam 状态≈2P(状态)+P(梯度)。

#### 4. 技术洞察

容量上界由最紧约束决定：max(显存, 带宽, 算力, 存储)，而不是各项相加。在 训练/优化器 上，量化关系是 有效 batch=micro_batch×grad_accum×world_size；Adam 状态≈2P(状态)+P(梯度)；核心不变量是「反向之前梯度应为空或已清零」，它一旦被打破，最先表现为 zero_grad 漏调导致梯度累加。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | optimizer.zero_grad(set_to_none=True) 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | 反向之前梯度应为空或已清零 | 违反「scheduler 与 optimizer 的 step 次数对齐」时的报错 |
| 量化关系 | 有效 batch=micro_batch×grad_accum×world_size；Adam 状态≈2P(状态)+P(梯度) | 实测与预测偏差超容差 |
| 失败模式 | scheduler.step() 放错位置使 lr 序列整体偏移 | 累积时直接累加 loss 未归一化 |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 训练/优化器 在“容量规划”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“容量规划：训练/优化器”沿相邻边界延伸：scheduler 恢复、优化器 state_dict、foreach/fused 实现差异。比较版本与替代实现的差异（如 lr_scheduler.step()），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [容量规划] lr_scheduler.step() 在 PyTorch 1.1.0 前后位置的语义差异是什么？
2. [容量规划] 梯度累积如何在数学上等价于大 batch，需要哪些归一化？
3. [容量规划] set_to_none=True 对显存与优化器行为有何具体影响？

#### 8. 易错点与反例

1. [容量规划] 用 zero_grad() 不传 set_to_none，长期分配开销更高
2. [容量规划] 把 scheduler.step() 在 epoch 与 step 间混用，导致 lr 跳变
3. [容量规划] 梯度累积时未除 steps，使等效学习率偏大

#### 9. 实践与验证

用单卡/单进程测单位成本，再线性外推并注入 20%~50% 余量验证拐点。具体到 训练/优化器：打印前若干步 param_groups[0]['lr'] 验证调度曲线；比较累积 N 步与单步大 batch 的梯度 L2 范数。把 scheduler.step() 放错位置使 lr 序列整体偏移 作为负例，记录 optimizer.step() 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“训练/优化器”的名词，说不出「反向之前梯度应为空或已清零」或 scheduler.step() 放错位置使 lr 序列整体偏移。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 有效 batch=micro_batch×grad_accum×world_size；Adam 状态≈2P(状态)+P(梯度) 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 累积时直接累加 loss 未归一化 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A028. 容量规划：AMP/恢复

- 难度：高级
- 知识域：K06 AMP/恢复
- 考察能力：事件顺序、状态迁移、所有权与观测点
- 题型：流程/调用链/数据流
- 面试频率：中频
- 前置知识：autocast、scaler、快照、随机状态
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

容量规划把资源需求写成可复算公式，再留尾部与故障余量。本题位于 K06 AMP/恢复。出现 inf/nan 时 scaler.step 跳过本次更新并降低 scale，避免污染权重。

#### 2. 问题分析

从“容量规划：AMP/恢复”出发，先枚举资源维度（显存/带宽/存储/连接/队列），把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 torch.autocast(device_type) 的契约，列出两条不变量：「反向前 loss 被 scale 放大，step 前梯度必须被 unscale」「inf/nan 检查与 skip 分支成对出现」，再给一个会击穿假设的边界输入，并说明它最先暴露 clip 放在 unscale 之前使梯度幅度错误 还是 对 bf16 仍用 GradScaler 造成无谓开销。

#### 3. 具体答案

面试中可以这样回答：

> 我会给出资源公式、拐点位置、余量比例和最先被打满的那一项资源。在 AMP/恢复 上，关键事实是：梯度裁剪前必须 scaler.unscale_(optimizer)，否则裁剪的是被放大后的梯度。 判据可写为 scale_{new}=scale×growth_factor（连续无溢出）或 scale/backoff_factor（检测到 inf/nan）。

#### 4. 技术洞察

容量上界由最紧约束决定：max(显存, 带宽, 算力, 存储)，而不是各项相加。在 AMP/恢复 上，量化关系是 scale_{new}=scale×growth_factor（连续无溢出）或 scale/backoff_factor（检测到 inf/nan）；核心不变量是「反向前 loss 被 scale 放大，step 前梯度必须被 unscale」，它一旦被打破，最先表现为 checkpoint 未保存 scaler.state_dict 使恢复后 scale 不连续。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | torch.autocast(device_type) 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | 反向前 loss 被 scale 放大，step 前梯度必须被 unscale | 违反「inf/nan 检查与 skip 分支成对出现」时的报错 |
| 量化关系 | scale_{new}=scale×growth_factor（连续无溢出）或 scale/backoff_factor（检测到 inf/nan） | 实测与预测偏差超容差 |
| 失败模式 | clip 放在 unscale 之前使梯度幅度错误 | 对 bf16 仍用 GradScaler 造成无谓开销 |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 AMP/恢复 在“容量规划”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“容量规划：AMP/恢复”沿相邻边界延伸：autocast 的 op 白/黑名单、随机状态快照、断点续训一致性。比较版本与替代实现的差异（如 scaler.scale/step/update），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [容量规划] GradScaler 连续多少次无溢出才提高 scale，默认参数是多少？
2. [容量规划] 为什么 fp16 autocast 必须 unscale 之后才能裁剪梯度？
3. [容量规划] bf16 为什么通常不需要 GradScaler？

#### 8. 易错点与反例

1. [容量规划] 以为 autocast 会把参数也转 fp16（参数保持 fp32）
2. [容量规划] 用未 unscale 的梯度直接裁剪
3. [容量规划] 恢复训练时忘记保存/加载 scaler 状态

#### 9. 实践与验证

用单卡/单进程测单位成本，再线性外推并注入 20%~50% 余量验证拐点。具体到 AMP/恢复：记录 scaler.get_scale() 在若干 step 的轨迹；注入含 nan 的 loss 验证该 step 被跳过。把 clip 放在 unscale 之前使梯度幅度错误 作为负例，记录 GradScaler 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“AMP/恢复”的名词，说不出「反向前 loss 被 scale 放大，step 前梯度必须被 unscale」或 clip 放在 unscale 之前使梯度幅度错误。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 scale_{new}=scale×growth_factor（连续无溢出）或 scale/backoff_factor（检测到 inf/nan） 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 对 bf16 仍用 GradScaler 造成无谓开销 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A029. 容量规划：分布式

- 难度：高级
- 知识域：K07 分布式
- 考察能力：事件顺序、状态迁移、所有权与观测点
- 题型：流程/调用链/数据流
- 面试频率：中频
- 前置知识：rank、DDP、FSDP、collective、容错
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

容量规划把资源需求写成可复算公式，再留尾部与故障余量。本题位于 K07 分布式。FSDP 把参数、梯度、优化器状态分片到各 rank：前向 all_gather 参数、反向 reduce_scatter 梯度。

#### 2. 问题分析

从“容量规划：分布式”出发，先枚举资源维度（显存/带宽/存储/连接/队列），把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 init_process_group 的契约，列出两条不变量：「所有 rank 的 rank/world_size 一致」「collective 调用序列在各 rank 严格一致」，再给一个会击穿假设的边界输入，并说明它最先暴露 某些 rank 提前 return 使 collective 不匹配而 hang 还是 梯度 size 不一致触发 'Mismatch in collective'。

#### 3. 具体答案

面试中可以这样回答：

> 我会给出资源公式、拐点位置、余量比例和最先被打满的那一项资源。在 分布式 上，关键事实是：collective 要求所有 rank 以相同顺序、相同 dtype/shape 调用，否则 hang 或 NCCL 报错。 判据可写为 ring allreduce≈2(N-1)/N×M；DDP 每卡显存≈params+grads+opt_states。

#### 4. 技术洞察

容量上界由最紧约束决定：max(显存, 带宽, 算力, 存储)，而不是各项相加。在 分布式 上，量化关系是 ring allreduce≈2(N-1)/N×M；DDP 每卡显存≈params+grads+opt_states；核心不变量是「所有 rank 的 rank/world_size 一致」，它一旦被打破，最先表现为 FSDP 未 wrap 子模块使参数无法分片。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | init_process_group 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | 所有 rank 的 rank/world_size 一致 | 违反「collective 调用序列在各 rank 严格一致」时的报错 |
| 量化关系 | ring allreduce≈2(N-1)/N×M；DDP 每卡显存≈params+grads+opt_states | 实测与预测偏差超容差 |
| 失败模式 | 某些 rank 提前 return 使 collective 不匹配而 hang | 梯度 size 不一致触发 'Mismatch in collective' |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 分布式 在“容量规划”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“容量规划：分布式”沿相邻边界延伸：process group 切分、混合并行、NCCL 拓扑与超时。比较版本与替代实现的差异（如 FullyShardedDataParallel），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [容量规划] DDP 的梯度 bucketing 如何影响通信与计算重叠？
2. [容量规划] FSDP 的 all_gather 与 reduce_scatter 分别在前向/反向何处发生？
3. [容量规划] collective 调用顺序不一致时表现为 hang 还是报错，原因是什么？

#### 8. 易错点与反例

1. [容量规划] 对非参数 buffer 手动 allreduce，造成重复同步
2. [容量规划] 认为 FSDP 降低单卡计算量（实际只降显存）
3. [容量规划] 忽略 bucket_cap_mb 对重叠与峰值显存的影响

#### 9. 实践与验证

用单卡/单进程测单位成本，再线性外推并注入 20%~50% 余量验证拐点。具体到 分布式：barrier 后断言 get_rank()/get_world_size()；对比 DDP 与 FSDP 的 max_memory_allocated。把 某些 rank 提前 return 使 collective 不匹配而 hang 作为负例，记录 DistributedDataParallel 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“分布式”的名词，说不出「所有 rank 的 rank/world_size 一致」或 某些 rank 提前 return 使 collective 不匹配而 hang。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 ring allreduce≈2(N-1)/N×M；DDP 每卡显存≈params+grads+opt_states 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 梯度 size 不一致触发 'Mismatch in collective' 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A030. 容量规划：编译栈

- 难度：高级
- 知识域：K08 编译栈
- 考察能力：事件顺序、状态迁移、所有权与观测点
- 题型：流程/调用链/数据流
- 面试频率：中频
- 前置知识：Dynamo、AOTAutograd、Inductor、guard、缓存
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

容量规划把资源需求写成可复算公式，再留尾部与故障余量。本题位于 K08 编译栈。graph break 把图切段回退 eager，破坏端到端融合。

#### 2. 问题分析

从“容量规划：编译栈”出发，先枚举资源维度（显存/带宽/存储/连接/队列），把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 torch.compile 的契约，列出两条不变量：「fullgraph=True 时任何 graph break 都抛错」「同一 guard 命中后不再触发编译」，再给一个会击穿假设的边界输入，并说明它最先暴露 动态 shape 未标记导致反复重编译 还是 数据相关控制流触发 graph break。

#### 3. 具体答案

面试中可以这样回答：

> 我会给出资源公式、拐点位置、余量比例和最先被打满的那一项资源。在 编译栈 上，关键事实是：AOTAutograd 生成前向与反向图，Inductor 生成 Triton kernel（GPU）或 C++ kernel（CPU）。 判据可写为 编译缓存以 guard 值集合为键；重编译次数∝出现过的不同 guard 组合数。

#### 4. 技术洞察

容量上界由最紧约束决定：max(显存, 带宽, 算力, 存储)，而不是各项相加。在 编译栈 上，量化关系是 编译缓存以 guard 值集合为键；重编译次数∝出现过的不同 guard 组合数；核心不变量是「fullgraph=True 时任何 graph break 都抛错」，它一旦被打破，最先表现为 CUDA Graph 因输入地址变化捕获失败。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | torch.compile 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | fullgraph=True 时任何 graph break 都抛错 | 违反「同一 guard 命中后不再触发编译」时的报错 |
| 量化关系 | 编译缓存以 guard 值集合为键；重编译次数∝出现过的不同 guard 组合数 | 实测与预测偏差超容差 |
| 失败模式 | 动态 shape 未标记导致反复重编译 | 数据相关控制流触发 graph break |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 编译栈 在“容量规划”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“容量规划：编译栈”沿相邻边界延伸：dynamic shape 的 Dim/mark_dynamic、guard 导出、AOT 与自定义 op。比较版本与替代实现的差异（如 TORCH_LOGS=graph_breaks,recompiles），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [容量规划] Dynamo 的 guard 在什么条件下失效并触发重编译？
2. [容量规划] graph break 对 autograd 图切分与状态副作用带来什么风险？
3. [容量规划] reduce-overhead 的 CUDA Graph 为何要求输入地址固定？

#### 8. 易错点与反例

1. [容量规划] 以为 graph break 只影响性能，忽略断点两侧的状态副作用
2. [容量规划] 用固定 shape 基准掩盖动态 shape 的重编译
3. [容量规划] 忽略首次编译冷启动开销

#### 9. 实践与验证

用单卡/单进程测单位成本，再线性外推并注入 20%~50% 余量验证拐点。具体到 编译栈：用 torch._dynamo.explain 统计 graph_breaks 数量；监控 counters['inductor']['recompiles']。把 动态 shape 未标记导致反复重编译 作为负例，记录 torch._dynamo.explain 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“编译栈”的名词，说不出「fullgraph=True 时任何 graph break 都抛错」或 动态 shape 未标记导致反复重编译。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 编译缓存以 guard 值集合为键；重编译次数∝出现过的不同 guard 组合数 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 数据相关控制流触发 graph break 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A031. 容量规划：性能

- 难度：高级
- 知识域：K09 性能
- 考察能力：前置条件、后置条件、断言与错误处理
- 题型：代码/伪代码/配置分析
- 面试频率：中频
- 前置知识：关键路径、带宽、算力、通信、显存、统计
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

容量规划把资源需求写成可复算公式，再留尾部与故障余量。本题位于 K09 性能。GPU 计时必须 warmup 并 torch.cuda.synchronize()，否则测到的是入队时间。

#### 2. 问题分析

从“容量规划：性能”出发，先枚举资源维度（显存/带宽/存储/连接/队列），把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 torch.profiler 的契约，列出两条不变量：「所有 kernel 时间之和≤墙钟时间，差额为同步/空泡」「峰值显存≥活跃张量之和+碎片+缓存块」，再给一个会击穿假设的边界输入，并说明它最先暴露 未 sync 就读 Event 时间 还是 只测稳态忽略首次冷启动。

#### 3. 具体答案

面试中可以这样回答：

> 我会给出资源公式、拐点位置、余量比例和最先被打满的那一项资源。在 性能 上，关键事实是：PyTorch 用缓存分配器，小张量反复分配不一定触发 cudaMalloc，但会碎片化。 判据可写为 t≈max(FLOPs/FLOPS_peak, Bytes/BW_peak)；算术强度=FLOPs/Bytes。

#### 4. 技术洞察

容量上界由最紧约束决定：max(显存, 带宽, 算力, 存储)，而不是各项相加。在 性能 上，量化关系是 t≈max(FLOPs/FLOPS_peak, Bytes/BW_peak)；算术强度=FLOPs/Bytes；核心不变量是「所有 kernel 时间之和≤墙钟时间，差额为同步/空泡」，它一旦被打破，最先表现为 把 H2D 拷贝算进 GPU 计算时间。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | torch.profiler 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | 所有 kernel 时间之和≤墙钟时间，差额为同步/空泡 | 违反「峰值显存≥活跃张量之和+碎片+缓存块」时的报错 |
| 量化关系 | t≈max(FLOPs/FLOPS_peak, Bytes/BW_peak)；算术强度=FLOPs/Bytes | 实测与预测偏差超容差 |
| 失败模式 | 未 sync 就读 Event 时间 | 只测稳态忽略首次冷启动 |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 性能 在“容量规划”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“容量规划：性能”沿相邻边界延伸：roofline 拐点、显存碎片与快照、通信/计算重叠度量。比较版本与替代实现的差异（如 torch.cuda.max_memory_allocated），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [容量规划] 如何用 roofline 判断算子 compute-bound 还是 memory-bound，需要哪些输入？
2. [容量规划] 为什么 profiler 的 self_cuda_time 之和常小于 step time？
3. [容量规划] 在通信与计算未重叠时，先改数据并行还是分片/bucket 策略？

#### 8. 易错点与反例

1. [容量规划] 用 CPU 计时测 GPU kernel，得到错误结论
2. [容量规划] 忽略通信与 sync 造成的时间空泡
3. [容量规划] 以单次测量下结论，被启动噪声误导

#### 9. 实践与验证

用单卡/单进程测单位成本，再线性外推并注入 20%~50% 余量验证拐点。具体到 性能：profiler 导出 kernel 表按 cuda_time 排序定位热点；固定输入重复 N 次取中位数与 p99。把 未 sync 就读 Event 时间 作为负例，记录 torch.cuda.Event 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“性能”的名词，说不出「所有 kernel 时间之和≤墙钟时间，差额为同步/空泡」或 未 sync 就读 Event 时间。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 t≈max(FLOPs/FLOPS_peak, Bytes/BW_peak)；算术强度=FLOPs/Bytes 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 只测稳态忽略首次冷启动 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A032. 容量规划：导出部署

- 难度：高级
- 知识域：K10 导出部署
- 考察能力：前置条件、后置条件、断言与错误处理
- 题型：代码/伪代码/配置分析
- 面试频率：中频
- 前置知识：state_dict、Export、runtime、schema、迁移
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

容量规划把资源需求写成可复算公式，再留尾部与故障余量。本题位于 K10 序列化/导出/部署。torch.jit.trace 记录一次执行路径，忽略数据相关控制流（if/for 会塌缩）。

#### 2. 问题分析

从“容量规划：序列化/导出/部署”出发，先枚举资源维度（显存/带宽/存储/连接/队列），把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 torch.save/torch.load(weights_only=) 的契约，列出两条不变量：「导出后的图不含 Python 语义，仅含 ATen/custom op 调用」「state_dict 的 key 语义与训练侧一致」，再给一个会击穿假设的边界输入，并说明它最先暴露 trace 把数据相关分支固化为单一路径 还是 自定义 op 未注册导致导出失败。

#### 3. 具体答案

面试中可以这样回答：

> 我会给出资源公式、拐点位置、余量比例和最先被打满的那一项资源。在 序列化/导出/部署 上，关键事实是：torch.export 基于 Dynamo 捕获 ATen 级图，输出 ExportedProgram（graph_module、state_dict、range_constraints）。 判据可写为 导出图=静态化 Python 控制流+固定/声明化 shape（Dim 约束）。

#### 4. 技术洞察

容量上界由最紧约束决定：max(显存, 带宽, 算力, 存储)，而不是各项相加。在 序列化/导出/部署 上，量化关系是 导出图=静态化 Python 控制流+固定/声明化 shape（Dim 约束）；核心不变量是「导出后的图不含 Python 语义，仅含 ATen/custom op 调用」，它一旦被打破，最先表现为 weights_only=True 加载旧 pickle 报错。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | torch.save/torch.load(weights_only=) 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | 导出后的图不含 Python 语义，仅含 ATen/custom op 调用 | 违反「state_dict 的 key 语义与训练侧一致」时的报错 |
| 量化关系 | 导出图=静态化 Python 控制流+固定/声明化 shape（Dim 约束） | 实测与预测偏差超容差 |
| 失败模式 | trace 把数据相关分支固化为单一路径 | 自定义 op 未注册导致导出失败 |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 序列化/导出/部署 在“容量规划”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“容量规划：序列化/导出/部署”沿相邻边界延伸：custom op 的 register_autograd、ONNX/ExecuTorch runtime、版本 schema 演进。比较版本与替代实现的差异（如 torch.export.export），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [容量规划] torch.jit.trace 与 torch.export 在控制流处理上有什么本质区别？
2. [容量规划] weights_only 自 2.6 起默认为 True 带来哪些兼容性影响？
3. [容量规划] 如何用 dynamic_shapes 表达一个仅序列维动态的模型并验证？

#### 8. 易错点与反例

1. [容量规划] 用 trace 导出含 if/loop 的模型，使数据相关分支被固化
2. [容量规划] 认为 weights_only=True 永远安全（自定义 op/类仍需注册）
3. [容量规划] 忽略导出后 dtype/device 与运行时不一致

#### 9. 实践与验证

用单卡/单进程测单位成本，再线性外推并注入 20%~50% 余量验证拐点。具体到 序列化/导出/部署：对比导出前后在相同输入上的输出 assert_close；检查 ExportedProgram.graph_module.graph 的 op 集合。把 trace 把数据相关分支固化为单一路径 作为负例，记录 torch.jit.trace/script 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“序列化/导出/部署”的名词，说不出「导出后的图不含 Python 语义，仅含 ATen/custom op 调用」或 trace 把数据相关分支固化为单一路径。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 导出图=静态化 Python 控制流+固定/声明化 shape（Dim 约束） 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 自定义 op 未注册导致导出失败 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A033. 容量规划：自定义算子

- 难度：高级
- 知识域：K11 自定义算子
- 考察能力：前置条件、后置条件、断言与错误处理
- 题型：代码/伪代码/配置分析
- 面试频率：中频
- 前置知识：schema、dispatcher、stream、autograd、ABI
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

容量规划把资源需求写成可复算公式，再留尾部与故障余量。本题位于 K11 自定义算子。kernel 必须在当前 CUDA stream 上执行并保证输入/输出的 stream 语义，否则与其它算子乱序。

#### 2. 问题分析

从“容量规划：自定义算子”出发，先枚举资源维度（显存/带宽/存储/连接/队列），把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 torch.library/TORCH_LIBRARY 的契约，列出两条不变量：「torch.autograd.gradcheck 通过表示前向/反向数值一致」「no_grad 与 grad 模式除梯度外结果一致」，再给一个会击穿假设的边界输入，并说明它最先暴露 漏写设备选择/CUDAGuard 使多卡下用错设备 还是 C++ ABI 不匹配报 undefined symbol。

#### 3. 具体答案

面试中可以这样回答：

> 我会给出资源公式、拐点位置、余量比例和最先被打满的那一项资源。在 自定义算子 上，关键事实是：CUDA 扩展编译需匹配 PyTorch 的 C++ ABI（_GLIBCXX_USE_CXX11_ABI）与 CUDA 架构 -gencode。 判据可写为 加速比=t_baseline/t_custom（同 shape/dtype/stream）；occupancy=活跃 warp/最大 warp。

#### 4. 技术洞察

容量上界由最紧约束决定：max(显存, 带宽, 算力, 存储)，而不是各项相加。在 自定义算子 上，量化关系是 加速比=t_baseline/t_custom（同 shape/dtype/stream）；occupancy=活跃 warp/最大 warp；核心不变量是「torch.autograd.gradcheck 通过表示前向/反向数值一致」，它一旦被打破，最先表现为 内核用原子加导致非确定性。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | torch.library/TORCH_LIBRARY 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | torch.autograd.gradcheck 通过表示前向/反向数值一致 | 违反「no_grad 与 grad 模式除梯度外结果一致」时的报错 |
| 量化关系 | 加速比=t_baseline/t_custom（同 shape/dtype/stream）；occupancy=活跃 warp/最大 warp | 实测与预测偏差超容差 |
| 失败模式 | 漏写设备选择/CUDAGuard 使多卡下用错设备 | C++ ABI 不匹配报 undefined symbol |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 自定义算子 在“容量规划”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“容量规划：自定义算子”沿相邻边界延伸：op schema 与 fake/meta 注册、流与事件同步、编译期架构选择。比较版本与替代实现的差异（如 torch.library.register_autograd），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [容量规划] 为自定义算子实现反向有哪两种机制，各自适用场景是什么？
2. [容量规划] 为什么 kernel 必须使用当前 stream，如何获取与同步？
3. [容量规划] CUDA 扩展的 C++ ABI 不匹配会以什么形式暴露？

#### 8. 易错点与反例

1. [容量规划] 只写前向不注册反向，autograd 报错或梯度为 None
2. [容量规划] 在默认 stream 上跑 kernel 却从非默认 stream 读数据
3. [容量规划] 忽略 C++ ABI 造成运行时链接失败

#### 9. 实践与验证

用单卡/单进程测单位成本，再线性外推并注入 20%~50% 余量验证拐点。具体到 自定义算子：gradcheck 校验自定义 Function 的梯度；torch.testing.assert_close 对比参考实现。把 漏写设备选择/CUDAGuard 使多卡下用错设备 作为负例，记录 torch.autograd.Function 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“自定义算子”的名词，说不出「torch.autograd.gradcheck 通过表示前向/反向数值一致」或 漏写设备选择/CUDAGuard 使多卡下用错设备。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 加速比=t_baseline/t_custom（同 shape/dtype/stream）；occupancy=活跃 warp/最大 warp 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 C++ ABI 不匹配报 undefined symbol 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A034. 容量规划：测试治理

- 难度：高级
- 知识域：K12 测试治理
- 考察能力：前置条件、后置条件、断言与错误处理
- 题型：代码/伪代码/配置分析
- 面试频率：中频
- 前置知识：oracle、证据、日志、回滚、门禁、复盘
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

容量规划把资源需求写成可复算公式，再留尾部与故障余量。本题位于 K12 测试治理。gradcheck 用有限差分对比解析梯度，默认 eps=1e-6、atol=1e-5。

#### 2. 问题分析

从“容量规划：测试治理”出发，先枚举资源维度（显存/带宽/存储/连接/队列），把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 torch.testing.assert_close 的契约，列出两条不变量：「相同 seed+相同算法下结果可复现」「通过阈值必须与 dtype 精度匹配」，再给一个会击穿假设的边界输入，并说明它最先暴露 对 fp16 用 fp32 阈值导致误报 还是 未固定 seed 造成随机失败。

#### 3. 具体答案

面试中可以这样回答：

> 我会给出资源公式、拐点位置、余量比例和最先被打满的那一项资源。在 测试治理 上，关键事实是：非确定性来源包括原子加、cuDNN benchmark、多线程归约顺序。 判据可写为 |a-b|≤atol+rtol×|b|；flake 率≈失败次数/重跑次数。

#### 4. 技术洞察

容量上界由最紧约束决定：max(显存, 带宽, 算力, 存储)，而不是各项相加。在 测试治理 上，量化关系是 |a-b|≤atol+rtol×|b|；flake 率≈失败次数/重跑次数；核心不变量是「相同 seed+相同算法下结果可复现」，它一旦被打破，最先表现为 把 flaky 当稳定失败反复重试掩盖问题。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | torch.testing.assert_close 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | 相同 seed+相同算法下结果可复现 | 违反「通过阈值必须与 dtype 精度匹配」时的报错 |
| 量化关系 | |a-b|≤atol+rtol×|b|；flake 率≈失败次数/重跑次数 | 实测与预测偏差超容差 |
| 失败模式 | 对 fp16 用 fp32 阈值导致误报 | 未固定 seed 造成随机失败 |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 测试治理 在“容量规划”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“容量规划：测试治理”沿相邻边界延伸：golden 值与版本记录、CI matrix 覆盖、门禁与回归集管理。比较版本与替代实现的差异（如 pytest），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [容量规划] assert_close 的 rtol/atol 如何按 dtype 选择才合理？
2. [容量规划] gradcheck 在什么情况下会误报失败？
3. [容量规划] 如何量化并治理 flaky 测试？

#### 8. 易错点与反例

1. [容量规划] 用 == 比较浮点张量
2. [容量规划] 以为关闭 cuDNN benchmark 就能完全确定
3. [容量规划] 把 golden 值硬编码却不记录环境版本

#### 9. 实践与验证

用单卡/单进程测单位成本，再线性外推并注入 20%~50% 余量验证拐点。具体到 测试治理：固定 seed 重跑 N 次统计通过率；用 assert_close 显式指定 rtol/atol 而非手写比较。把 对 fp16 用 fp32 阈值导致误报 作为负例，记录 torch.autograd.gradcheck 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“测试治理”的名词，说不出「相同 seed+相同算法下结果可复现」或 对 fp16 用 fp32 阈值导致误报。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 |a-b|≤atol+rtol×|b|；flake 率≈失败次数/重跑次数 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 未固定 seed 造成随机失败 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A035. 并发与资源：张量/设备

- 难度：高级
- 知识域：K01 张量/设备
- 考察能力：前置条件、后置条件、断言与错误处理
- 题型：代码/伪代码/配置分析
- 面试频率：中频
- 前置知识：shape、stride、dtype、device
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

并发问题的核心是共享状态的可见性与资源竞争的边界。本题位于 K01 张量/设备。显存占用=numel×element_size：fp32 每元素 4B，fp16/bf16 每元素 2B。

#### 2. 问题分析

从“并发与资源：张量/设备”出发，先识别共享可变状态、同步点与资源池，把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 torch.Tensor.view/reshape 的契约，列出两条不变量：「view 与源共享 storage（data_ptr 与 storage_offset 一致）」「张量的 device/dtype 决定 kernel 派发与是否发生隐式转换」，再给一个会击穿假设的边界输入，并说明它最先暴露 对非连续张量 view 报 'view size is not compatible with input tensor's size and stride' 还是 跨设备运算报 'Expected all tensors to be on the same device'。

#### 3. 具体答案

面试中可以这样回答：

> 我会描述并发模型、同步机制、竞争窗口和一个会触发的交错顺序。在 张量/设备 上，关键事实是：non_blocking=True 只有在源 pinned 且目标是加速器时才真正异步，否则退化为同步拷贝。 判据可写为 显存(bytes)=numel×itemsize(dtype)；连续 stride[i]=∏_{j>i}shape[j]。

#### 4. 技术洞察

并发正确性依赖 happens-before 关系；缺少同步的读写即使在单机也可能读到撕裂值。在 张量/设备 上，量化关系是 显存(bytes)=numel×itemsize(dtype)；连续 stride[i]=∏_{j>i}shape[j]；核心不变量是「view 与源共享 storage（data_ptr 与 storage_offset 一致）」，它一旦被打破，最先表现为 reshape 静默拷贝抬高峰值显存。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | torch.Tensor.view/reshape 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | view 与源共享 storage（data_ptr 与 storage_offset 一致） | 违反「张量的 device/dtype 决定 kernel 派发与是否发生隐式转换」时的报错 |
| 量化关系 | 显存(bytes)=numel×itemsize(dtype)；连续 stride[i]=∏_{j>i}shape[j] | 实测与预测偏差超容差 |
| 失败模式 | 对非连续张量 view 报 'view size is not compatible with input tensor's size and stride' | 跨设备运算报 'Expected all tensors to be on the same device' |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 张量/设备 在“并发与资源”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“并发与资源：张量/设备”沿相邻边界延伸：view/reshape/contiguous、pin_memory 与 stream、dtype 转换与显存。比较版本与替代实现的差异（如 t.to(device, non_blocking=True)），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [并发与资源] shape=(2,3,4) 分别 view 成 (6,4)、reshape 成 (6,4) 后，stride 与是否共享存储各是什么？
2. [并发与资源] 为什么 permute 之后必须先 contiguous 才能 view？
3. [并发与资源] non_blocking=True 在源未 pinned 时的实际行为如何用 profiler 证明？

#### 8. 易错点与反例

1. [并发与资源] 把 reshape 当零拷贝：stride 不兼容时它会 contiguous 并分配新显存
2. [并发与资源] 只比较 shape 相等就判断可 view 互转，忽略 stride/storage_offset
3. [并发与资源] 以为 pin_memory 总有效，实际它只对 non_blocking 有加速意义

#### 9. 实践与验证

用受控交错（barrier/sleep 或确定性调度）复现竞争，再用压测验证稳态。具体到 张量/设备：构造 (2,3,4) 后断言 t.stride()==(12,4,1) 且 t.is_contiguous()；用 t.data_ptr() 验证 view 与 reshape 的共享/拷贝差异。把 对非连续张量 view 报 'view size is not compatible with input tensor's size and stride' 作为负例，记录 t.stride()/t.is_contiguous() 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“张量/设备”的名词，说不出「view 与源共享 storage（data_ptr 与 storage_offset 一致）」或 对非连续张量 view 报 'view size is not compatible with input tensor's size and stride'。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 显存(bytes)=numel×itemsize(dtype)；连续 stride[i]=∏_{j>i}shape[j] 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 跨设备运算报 'Expected all tensors to be on the same device' 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A036. 并发与资源：autograd

- 难度：高级
- 知识域：K02 autograd
- 考察能力：前置条件、后置条件、断言与错误处理
- 题型：代码/伪代码/配置分析
- 面试频率：中频
- 前置知识：requires_grad、leaf、view、detach、反向
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

并发问题的核心是共享状态的可见性与资源竞争的边界。本题位于 K02 autograd。反向默认释放计算图，第二次反向需 retain_graph=True，否则报 'Trying to backward through the graph a second time'。

#### 2. 问题分析

从“并发与资源：autograd”出发，先识别共享可变状态、同步点与资源池，把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 Tensor.requires_grad_ 的契约，列出两条不变量：「叶子张量 .grad 的形状与张量一致且只由叶子持有」「requires_grad 沿 grad_fn 链单向传播」，再给一个会击穿假设的边界输入，并说明它最先暴露 忘记 zero_grad 使梯度累加 还是 对 requires_grad 的叶子做原地操作报 'a leaf Variable that requires grad is being used in an in-place operation'。

#### 3. 具体答案

面试中可以这样回答：

> 我会描述并发模型、同步机制、竞争窗口和一个会触发的交错顺序。在 autograd 上，关键事实是：param.data.add_() 绕过 autograd，是原地更新权重而不破坏图的标准做法。 判据可写为 累加语义 g←g+∂L/∂θ；链式法则 dL/dx=Σ(dL/dy·dy/dx)。

#### 4. 技术洞察

并发正确性依赖 happens-before 关系；缺少同步的读写即使在单机也可能读到撕裂值。在 autograd 上，量化关系是 累加语义 g←g+∂L/∂θ；链式法则 dL/dx=Σ(dL/dy·dy/dx)；核心不变量是「叶子张量 .grad 的形状与张量一致且只由叶子持有」，它一旦被打破，最先表现为 两次 backward 未设 retain_graph 触发 RuntimeError。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | Tensor.requires_grad_ 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | 叶子张量 .grad 的形状与张量一致且只由叶子持有 | 违反「requires_grad 沿 grad_fn 链单向传播」时的报错 |
| 量化关系 | 累加语义 g←g+∂L/∂θ；链式法则 dL/dx=Σ(dL/dy·dy/dx) | 实测与预测偏差超容差 |
| 失败模式 | 忘记 zero_grad 使梯度累加 | 对 requires_grad 的叶子做原地操作报 'a leaf Variable that requires grad is being used in an in-place operation' |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 autograd 在“并发与资源”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“并发与资源：autograd”沿相邻边界延伸：detach、retain_grad、in-place 与版本计数、gradcheck。比较版本与替代实现的差异（如 torch.autograd.grad），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [并发与资源] 为什么非叶子张量的 .grad 默认是 None，retain_grad() 改变了什么？
2. [并发与资源] detach() 与 with torch.no_grad() 在切断梯度上的差别是什么？
3. [并发与资源] in-place 操作在什么条件下能安全使用而不破坏 autograd？

#### 8. 易错点与反例

1. [并发与资源] 认为非叶子 .grad 会自动填充，实则默认 None，需要 retain_grad()
2. [并发与资源] 在 no_grad 下构建输入再 backward，得到 requires_grad 缺失错误
3. [并发与资源] 用 param = param - lr*g 原地更新破坏计算图

#### 9. 实践与验证

用受控交错（barrier/sleep 或确定性调度）复现竞争，再用压测验证稳态。具体到 autograd：断言 x.is_leaf and x.grad is None 后再 backward；用 torch.autograd.grad(..., retain_graph=True) 做两次反向对比。把 忘记 zero_grad 使梯度累加 作为负例，记录 Tensor.detach() 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“autograd”的名词，说不出「叶子张量 .grad 的形状与张量一致且只由叶子持有」或 忘记 zero_grad 使梯度累加。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 累加语义 g←g+∂L/∂θ；链式法则 dL/dx=Σ(dL/dy·dy/dx) 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 对 requires_grad 的叶子做原地操作报 'a leaf Variable that requires grad is being used in an in-place operation' 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A037. 并发与资源：Module/状态

- 难度：高级
- 知识域：K03 Module/状态
- 考察能力：前置条件、后置条件、断言与错误处理
- 题型：代码/伪代码/配置分析
- 面试频率：中频
- 前置知识：参数、buffer、模式和 state_dict
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

并发问题的核心是共享状态的可见性与资源竞争的边界。本题位于 K03 Module/状态。.eval() 把 self.training 置 False，切换 Dropout/BatchNorm 行为，但不改变 requires_grad。

#### 2. 问题分析

从“并发与资源：Module/状态”出发，先识别共享可变状态、同步点与资源池，把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 register_buffer 的契约，列出两条不变量：「同一 Module 的 state_dict key 集合确定且与设备无关」「buffer 的 dtype/device 跟随 module 或按注册时指定」，再给一个会击穿假设的边界输入，并说明它最先暴露 自定义 buffer 未注册，.to(device) 后仍留在 CPU 还是 load_state_dict 因 missing/unexpected keys 失败。

#### 3. 具体答案

面试中可以这样回答：

> 我会描述并发模型、同步机制、竞争窗口和一个会触发的交错顺序。在 Module/状态 上，关键事实是：BatchNorm 的 running_mean/running_var 是 buffer，eval 下不再更新、只用统计量。 判据可写为 state_dict={k:param}∪{k:buffer}；训练态显存≈Σ numel×itemsize×(1+grad+opt_state)。

#### 4. 技术洞察

并发正确性依赖 happens-before 关系；缺少同步的读写即使在单机也可能读到撕裂值。在 Module/状态 上，量化关系是 state_dict={k:param}∪{k:buffer}；训练态显存≈Σ numel×itemsize×(1+grad+opt_state)；核心不变量是「同一 Module 的 state_dict key 集合确定且与设备无关」，它一旦被打破，最先表现为 eval 下忘记切换使 Dropout 仍在抽样。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | register_buffer 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | 同一 Module 的 state_dict key 集合确定且与设备无关 | 违反「buffer 的 dtype/device 跟随 module 或按注册时指定」时的报错 |
| 量化关系 | state_dict={k:param}∪{k:buffer}；训练态显存≈Σ numel×itemsize×(1+grad+opt_state) | 实测与预测偏差超容差 |
| 失败模式 | 自定义 buffer 未注册，.to(device) 后仍留在 CPU | load_state_dict 因 missing/unexpected keys 失败 |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 Module/状态 在“并发与资源”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“并发与资源：Module/状态”沿相邻边界延伸：persistent buffer、hook 与状态迁移、DDP 下的 buffer 同步。比较版本与替代实现的差异（如 state_dict/load_state_dict），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [并发与资源] buffer 与 Parameter 在 state_dict、梯度和设备迁移上的差异是什么？
2. [并发与资源] BatchNorm 在 eval 模式仍更新 running stats 需要什么额外设置？
3. [并发与资源] register_buffer(persistent=False) 会从 state_dict 中移除什么？

#### 8. 易错点与反例

1. [并发与资源] 把计数/统计量存成普通属性，导致 state_dict 丢失该状态
2. [并发与资源] load_state_dict 后忘记 module.eval()，使 BN/Dropout 行为不符
3. [并发与资源] 直接给同名属性赋值绕过注册，使 named_parameters 缺失

#### 9. 实践与验证

用受控交错（barrier/sleep 或确定性调度）复现竞争，再用压测验证稳态。具体到 Module/状态：断言 set(m.state_dict().keys()) 与 checkpoint 完全一致；对比 train/eval 两次前向输出验证 Dropout 关闭。把 自定义 buffer 未注册，.to(device) 后仍留在 CPU 作为负例，记录 register_parameter 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“Module/状态”的名词，说不出「同一 Module 的 state_dict key 集合确定且与设备无关」或 自定义 buffer 未注册，.to(device) 后仍留在 CPU。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 state_dict={k:param}∪{k:buffer}；训练态显存≈Σ numel×itemsize×(1+grad+opt_state) 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 load_state_dict 因 missing/unexpected keys 失败 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A038. 并发与资源：数据管道

- 难度：高级
- 知识域：K04 数据管道
- 考察能力：前置条件、后置条件、断言与错误处理
- 题型：代码/伪代码/配置分析
- 面试频率：中频
- 前置知识：Dataset、Sampler、collate、padding、版本
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

并发问题的核心是共享状态的可见性与资源竞争的边界。本题位于 K04 数据管道。persistent_workers=True 复用 worker，避免每个 epoch 重新 fork。

#### 2. 问题分析

从“并发与资源：数据管道”出发，先识别共享可变状态、同步点与资源池，把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 DataLoader(num_workers, pin_memory, persistent_workers) 的契约，列出两条不变量：「shuffle=False 且种子固定时迭代顺序可复现」「各 rank 采样集合互斥且并集覆盖全集（或按 drop_last 截断）」，再给一个会击穿假设的边界输入，并说明它最先暴露 多 worker 共享同一随机源导致样本重复 还是 worker 数过多耗尽文件描述符或内存。

#### 3. 具体答案

面试中可以这样回答：

> 我会描述并发模型、同步机制、竞争窗口和一个会触发的交错顺序。在 数据管道 上，关键事实是：pin_memory=True 把 batch 放进页锁定内存，配合 non_blocking 才能与计算重叠。 判据可写为 吞吐≤min(存储带宽, CPU 解码速率, H2D 带宽)；有效样本/步=batch×world_size。

#### 4. 技术洞察

并发正确性依赖 happens-before 关系；缺少同步的读写即使在单机也可能读到撕裂值。在 数据管道 上，量化关系是 吞吐≤min(存储带宽, CPU 解码速率, H2D 带宽)；有效样本/步=batch×world_size；核心不变量是「shuffle=False 且种子固定时迭代顺序可复现」，它一旦被打破，最先表现为 DistributedSampler 与自定义 shuffle 双打乱造成跨 rank 重复。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | DataLoader(num_workers, pin_memory, persistent_workers) 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | shuffle=False 且种子固定时迭代顺序可复现 | 违反「各 rank 采样集合互斥且并集覆盖全集（或按 drop_last 截断）」时的报错 |
| 量化关系 | 吞吐≤min(存储带宽, CPU 解码速率, H2D 带宽)；有效样本/步=batch×world_size | 实测与预测偏差超容差 |
| 失败模式 | 多 worker 共享同一随机源导致样本重复 | worker 数过多耗尽文件描述符或内存 |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 数据管道 在“并发与资源”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“并发与资源：数据管道”沿相邻边界延伸：IterableDataset、chunk 预取、对象存储吞吐与 IPC 开销。比较版本与替代实现的差异（如 collate_fn），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [并发与资源] DistributedSampler 如何处理样本数不被 world_size 整除，是否引入重复样本？
2. [并发与资源] num_workers 增大时首步延迟、内存与 IPC 开销如何变化？
3. [并发与资源] pin_memory 与 non_blocking 为何必须同时开启才有效？

#### 8. 易错点与反例

1. [并发与资源] 把 num_workers 调大就以为更快，忽略 IPC 与内存放大
2. [并发与资源] Dataset 里用全局随机而不经 worker_init_fn 重播种
3. [并发与资源] 自己 shuffle 又用 DistributedSampler，导致跨 rank 样本重复

#### 9. 实践与验证

用受控交错（barrier/sleep 或确定性调度）复现竞争，再用压测验证稳态。具体到 数据管道：固定 seed 两次迭代断言顺序一致；统计各 rank 采样索引并集是否覆盖全集。把 多 worker 共享同一随机源导致样本重复 作为负例，记录 DistributedSampler 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“数据管道”的名词，说不出「shuffle=False 且种子固定时迭代顺序可复现」或 多 worker 共享同一随机源导致样本重复。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 吞吐≤min(存储带宽, CPU 解码速率, H2D 带宽)；有效样本/步=batch×world_size 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 worker 数过多耗尽文件描述符或内存 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A039. 并发与资源：训练/优化器

- 难度：高级
- 知识域：K05 训练/优化器
- 考察能力：前置条件、后置条件、断言与错误处理
- 题型：代码/伪代码/配置分析
- 面试频率：中频
- 前置知识：forward、backward、累积、optimizer、scheduler
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

并发问题的核心是共享状态的可见性与资源竞争的边界。本题位于 K05 训练/优化器。梯度累积须按累积步数折算 loss（loss/steps），否则等效学习率被放大。

#### 2. 问题分析

从“并发与资源：训练/优化器”出发，先识别共享可变状态、同步点与资源池，把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 optimizer.zero_grad(set_to_none=True) 的契约，列出两条不变量：「反向之前梯度应为空或已清零」「scheduler 与 optimizer 的 step 次数对齐」，再给一个会击穿假设的边界输入，并说明它最先暴露 scheduler.step() 放错位置使 lr 序列整体偏移 还是 累积时直接累加 loss 未归一化。

#### 3. 具体答案

面试中可以这样回答：

> 我会描述并发模型、同步机制、竞争窗口和一个会触发的交错顺序。在 训练/优化器 上，关键事实是：Adam 每参数维护 exp_avg 与 exp_avg_sq，优化器状态约 2×参数量。 判据可写为 有效 batch=micro_batch×grad_accum×world_size；Adam 状态≈2P(状态)+P(梯度)。

#### 4. 技术洞察

并发正确性依赖 happens-before 关系；缺少同步的读写即使在单机也可能读到撕裂值。在 训练/优化器 上，量化关系是 有效 batch=micro_batch×grad_accum×world_size；Adam 状态≈2P(状态)+P(梯度)；核心不变量是「反向之前梯度应为空或已清零」，它一旦被打破，最先表现为 zero_grad 漏调导致梯度累加。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | optimizer.zero_grad(set_to_none=True) 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | 反向之前梯度应为空或已清零 | 违反「scheduler 与 optimizer 的 step 次数对齐」时的报错 |
| 量化关系 | 有效 batch=micro_batch×grad_accum×world_size；Adam 状态≈2P(状态)+P(梯度) | 实测与预测偏差超容差 |
| 失败模式 | scheduler.step() 放错位置使 lr 序列整体偏移 | 累积时直接累加 loss 未归一化 |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 训练/优化器 在“并发与资源”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“并发与资源：训练/优化器”沿相邻边界延伸：scheduler 恢复、优化器 state_dict、foreach/fused 实现差异。比较版本与替代实现的差异（如 lr_scheduler.step()），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [并发与资源] lr_scheduler.step() 在 PyTorch 1.1.0 前后位置的语义差异是什么？
2. [并发与资源] 梯度累积如何在数学上等价于大 batch，需要哪些归一化？
3. [并发与资源] set_to_none=True 对显存与优化器行为有何具体影响？

#### 8. 易错点与反例

1. [并发与资源] 用 zero_grad() 不传 set_to_none，长期分配开销更高
2. [并发与资源] 把 scheduler.step() 在 epoch 与 step 间混用，导致 lr 跳变
3. [并发与资源] 梯度累积时未除 steps，使等效学习率偏大

#### 9. 实践与验证

用受控交错（barrier/sleep 或确定性调度）复现竞争，再用压测验证稳态。具体到 训练/优化器：打印前若干步 param_groups[0]['lr'] 验证调度曲线；比较累积 N 步与单步大 batch 的梯度 L2 范数。把 scheduler.step() 放错位置使 lr 序列整体偏移 作为负例，记录 optimizer.step() 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“训练/优化器”的名词，说不出「反向之前梯度应为空或已清零」或 scheduler.step() 放错位置使 lr 序列整体偏移。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 有效 batch=micro_batch×grad_accum×world_size；Adam 状态≈2P(状态)+P(梯度) 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 累积时直接累加 loss 未归一化 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A040. 并发与资源：AMP/恢复

- 难度：高级
- 知识域：K06 AMP/恢复
- 考察能力：前置条件、后置条件、断言与错误处理
- 题型：代码/伪代码/配置分析
- 面试频率：中频
- 前置知识：autocast、scaler、快照、随机状态
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

并发问题的核心是共享状态的可见性与资源竞争的边界。本题位于 K06 AMP/恢复。梯度裁剪前必须 scaler.unscale_(optimizer)，否则裁剪的是被放大后的梯度。

#### 2. 问题分析

从“并发与资源：AMP/恢复”出发，先识别共享可变状态、同步点与资源池，把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 torch.autocast(device_type) 的契约，列出两条不变量：「反向前 loss 被 scale 放大，step 前梯度必须被 unscale」「inf/nan 检查与 skip 分支成对出现」，再给一个会击穿假设的边界输入，并说明它最先暴露 clip 放在 unscale 之前使梯度幅度错误 还是 对 bf16 仍用 GradScaler 造成无谓开销。

#### 3. 具体答案

面试中可以这样回答：

> 我会描述并发模型、同步机制、竞争窗口和一个会触发的交错顺序。在 AMP/恢复 上，关键事实是：bf16 动态范围与 fp32 相同，通常无需 GradScaler；fp16 才需要 loss scaling。 判据可写为 scale_{new}=scale×growth_factor（连续无溢出）或 scale/backoff_factor（检测到 inf/nan）。

#### 4. 技术洞察

并发正确性依赖 happens-before 关系；缺少同步的读写即使在单机也可能读到撕裂值。在 AMP/恢复 上，量化关系是 scale_{new}=scale×growth_factor（连续无溢出）或 scale/backoff_factor（检测到 inf/nan）；核心不变量是「反向前 loss 被 scale 放大，step 前梯度必须被 unscale」，它一旦被打破，最先表现为 checkpoint 未保存 scaler.state_dict 使恢复后 scale 不连续。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | torch.autocast(device_type) 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | 反向前 loss 被 scale 放大，step 前梯度必须被 unscale | 违反「inf/nan 检查与 skip 分支成对出现」时的报错 |
| 量化关系 | scale_{new}=scale×growth_factor（连续无溢出）或 scale/backoff_factor（检测到 inf/nan） | 实测与预测偏差超容差 |
| 失败模式 | clip 放在 unscale 之前使梯度幅度错误 | 对 bf16 仍用 GradScaler 造成无谓开销 |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 AMP/恢复 在“并发与资源”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“并发与资源：AMP/恢复”沿相邻边界延伸：autocast 的 op 白/黑名单、随机状态快照、断点续训一致性。比较版本与替代实现的差异（如 scaler.scale/step/update），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [并发与资源] GradScaler 连续多少次无溢出才提高 scale，默认参数是多少？
2. [并发与资源] 为什么 fp16 autocast 必须 unscale 之后才能裁剪梯度？
3. [并发与资源] bf16 为什么通常不需要 GradScaler？

#### 8. 易错点与反例

1. [并发与资源] 以为 autocast 会把参数也转 fp16（参数保持 fp32）
2. [并发与资源] 用未 unscale 的梯度直接裁剪
3. [并发与资源] 恢复训练时忘记保存/加载 scaler 状态

#### 9. 实践与验证

用受控交错（barrier/sleep 或确定性调度）复现竞争，再用压测验证稳态。具体到 AMP/恢复：记录 scaler.get_scale() 在若干 step 的轨迹；注入含 nan 的 loss 验证该 step 被跳过。把 clip 放在 unscale 之前使梯度幅度错误 作为负例，记录 GradScaler 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“AMP/恢复”的名词，说不出「反向前 loss 被 scale 放大，step 前梯度必须被 unscale」或 clip 放在 unscale 之前使梯度幅度错误。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 scale_{new}=scale×growth_factor（连续无溢出）或 scale/backoff_factor（检测到 inf/nan） 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 对 bf16 仍用 GradScaler 造成无谓开销 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A041. 并发与资源：分布式

- 难度：高级
- 知识域：K07 分布式
- 考察能力：正确性、性能、成本、运维与迁移条件
- 题型：对比与取舍
- 面试频率：中频
- 前置知识：rank、DDP、FSDP、collective、容错
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

并发问题的核心是共享状态的可见性与资源竞争的边界。本题位于 K07 分布式。collective 要求所有 rank 以相同顺序、相同 dtype/shape 调用，否则 hang 或 NCCL 报错。

#### 2. 问题分析

从“并发与资源：分布式”出发，先识别共享可变状态、同步点与资源池，把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 init_process_group 的契约，列出两条不变量：「所有 rank 的 rank/world_size 一致」「collective 调用序列在各 rank 严格一致」，再给一个会击穿假设的边界输入，并说明它最先暴露 某些 rank 提前 return 使 collective 不匹配而 hang 还是 梯度 size 不一致触发 'Mismatch in collective'。

#### 3. 具体答案

面试中可以这样回答：

> 我会描述并发模型、同步机制、竞争窗口和一个会触发的交错顺序。在 分布式 上，关键事实是：ring allreduce 通信量约 2(N-1)/N×M，N 很大时趋近 2M。 判据可写为 ring allreduce≈2(N-1)/N×M；DDP 每卡显存≈params+grads+opt_states。

#### 4. 技术洞察

并发正确性依赖 happens-before 关系；缺少同步的读写即使在单机也可能读到撕裂值。在 分布式 上，量化关系是 ring allreduce≈2(N-1)/N×M；DDP 每卡显存≈params+grads+opt_states；核心不变量是「所有 rank 的 rank/world_size 一致」，它一旦被打破，最先表现为 FSDP 未 wrap 子模块使参数无法分片。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | init_process_group 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | 所有 rank 的 rank/world_size 一致 | 违反「collective 调用序列在各 rank 严格一致」时的报错 |
| 量化关系 | ring allreduce≈2(N-1)/N×M；DDP 每卡显存≈params+grads+opt_states | 实测与预测偏差超容差 |
| 失败模式 | 某些 rank 提前 return 使 collective 不匹配而 hang | 梯度 size 不一致触发 'Mismatch in collective' |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 分布式 在“并发与资源”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“并发与资源：分布式”沿相邻边界延伸：process group 切分、混合并行、NCCL 拓扑与超时。比较版本与替代实现的差异（如 FullyShardedDataParallel），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [并发与资源] DDP 的梯度 bucketing 如何影响通信与计算重叠？
2. [并发与资源] FSDP 的 all_gather 与 reduce_scatter 分别在前向/反向何处发生？
3. [并发与资源] collective 调用顺序不一致时表现为 hang 还是报错，原因是什么？

#### 8. 易错点与反例

1. [并发与资源] 对非参数 buffer 手动 allreduce，造成重复同步
2. [并发与资源] 认为 FSDP 降低单卡计算量（实际只降显存）
3. [并发与资源] 忽略 bucket_cap_mb 对重叠与峰值显存的影响

#### 9. 实践与验证

用受控交错（barrier/sleep 或确定性调度）复现竞争，再用压测验证稳态。具体到 分布式：barrier 后断言 get_rank()/get_world_size()；对比 DDP 与 FSDP 的 max_memory_allocated。把 某些 rank 提前 return 使 collective 不匹配而 hang 作为负例，记录 DistributedDataParallel 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“分布式”的名词，说不出「所有 rank 的 rank/world_size 一致」或 某些 rank 提前 return 使 collective 不匹配而 hang。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 ring allreduce≈2(N-1)/N×M；DDP 每卡显存≈params+grads+opt_states 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 梯度 size 不一致触发 'Mismatch in collective' 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A042. 并发与资源：编译栈

- 难度：高级
- 知识域：K08 编译栈
- 考察能力：正确性、性能、成本、运维与迁移条件
- 题型：对比与取舍
- 面试频率：中频
- 前置知识：Dynamo、AOTAutograd、Inductor、guard、缓存
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

并发问题的核心是共享状态的可见性与资源竞争的边界。本题位于 K08 编译栈。AOTAutograd 生成前向与反向图，Inductor 生成 Triton kernel（GPU）或 C++ kernel（CPU）。

#### 2. 问题分析

从“并发与资源：编译栈”出发，先识别共享可变状态、同步点与资源池，把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 torch.compile 的契约，列出两条不变量：「fullgraph=True 时任何 graph break 都抛错」「同一 guard 命中后不再触发编译」，再给一个会击穿假设的边界输入，并说明它最先暴露 动态 shape 未标记导致反复重编译 还是 数据相关控制流触发 graph break。

#### 3. 具体答案

面试中可以这样回答：

> 我会描述并发模型、同步机制、竞争窗口和一个会触发的交错顺序。在 编译栈 上，关键事实是：reduce-overhead 使用 CUDA Graphs 降低 launch 开销，但要求输入地址稳定。 判据可写为 编译缓存以 guard 值集合为键；重编译次数∝出现过的不同 guard 组合数。

#### 4. 技术洞察

并发正确性依赖 happens-before 关系；缺少同步的读写即使在单机也可能读到撕裂值。在 编译栈 上，量化关系是 编译缓存以 guard 值集合为键；重编译次数∝出现过的不同 guard 组合数；核心不变量是「fullgraph=True 时任何 graph break 都抛错」，它一旦被打破，最先表现为 CUDA Graph 因输入地址变化捕获失败。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | torch.compile 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | fullgraph=True 时任何 graph break 都抛错 | 违反「同一 guard 命中后不再触发编译」时的报错 |
| 量化关系 | 编译缓存以 guard 值集合为键；重编译次数∝出现过的不同 guard 组合数 | 实测与预测偏差超容差 |
| 失败模式 | 动态 shape 未标记导致反复重编译 | 数据相关控制流触发 graph break |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 编译栈 在“并发与资源”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“并发与资源：编译栈”沿相邻边界延伸：dynamic shape 的 Dim/mark_dynamic、guard 导出、AOT 与自定义 op。比较版本与替代实现的差异（如 TORCH_LOGS=graph_breaks,recompiles），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [并发与资源] Dynamo 的 guard 在什么条件下失效并触发重编译？
2. [并发与资源] graph break 对 autograd 图切分与状态副作用带来什么风险？
3. [并发与资源] reduce-overhead 的 CUDA Graph 为何要求输入地址固定？

#### 8. 易错点与反例

1. [并发与资源] 以为 graph break 只影响性能，忽略断点两侧的状态副作用
2. [并发与资源] 用固定 shape 基准掩盖动态 shape 的重编译
3. [并发与资源] 忽略首次编译冷启动开销

#### 9. 实践与验证

用受控交错（barrier/sleep 或确定性调度）复现竞争，再用压测验证稳态。具体到 编译栈：用 torch._dynamo.explain 统计 graph_breaks 数量；监控 counters['inductor']['recompiles']。把 动态 shape 未标记导致反复重编译 作为负例，记录 torch._dynamo.explain 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“编译栈”的名词，说不出「fullgraph=True 时任何 graph break 都抛错」或 动态 shape 未标记导致反复重编译。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 编译缓存以 guard 值集合为键；重编译次数∝出现过的不同 guard 组合数 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 数据相关控制流触发 graph break 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A043. 并发与资源：性能

- 难度：高级
- 知识域：K09 性能
- 考察能力：正确性、性能、成本、运维与迁移条件
- 题型：对比与取舍
- 面试频率：中频
- 前置知识：关键路径、带宽、算力、通信、显存、统计
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

并发问题的核心是共享状态的可见性与资源竞争的边界。本题位于 K09 性能。PyTorch 用缓存分配器，小张量反复分配不一定触发 cudaMalloc，但会碎片化。

#### 2. 问题分析

从“并发与资源：性能”出发，先识别共享可变状态、同步点与资源池，把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 torch.profiler 的契约，列出两条不变量：「所有 kernel 时间之和≤墙钟时间，差额为同步/空泡」「峰值显存≥活跃张量之和+碎片+缓存块」，再给一个会击穿假设的边界输入，并说明它最先暴露 未 sync 就读 Event 时间 还是 只测稳态忽略首次冷启动。

#### 3. 具体答案

面试中可以这样回答：

> 我会描述并发模型、同步机制、竞争窗口和一个会触发的交错顺序。在 性能 上，关键事实是：注意力前向 FLOPs 约为 4·b·h·s²·d（含 QK^T 与 softmax 加权）。 判据可写为 t≈max(FLOPs/FLOPS_peak, Bytes/BW_peak)；算术强度=FLOPs/Bytes。

#### 4. 技术洞察

并发正确性依赖 happens-before 关系；缺少同步的读写即使在单机也可能读到撕裂值。在 性能 上，量化关系是 t≈max(FLOPs/FLOPS_peak, Bytes/BW_peak)；算术强度=FLOPs/Bytes；核心不变量是「所有 kernel 时间之和≤墙钟时间，差额为同步/空泡」，它一旦被打破，最先表现为 把 H2D 拷贝算进 GPU 计算时间。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | torch.profiler 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | 所有 kernel 时间之和≤墙钟时间，差额为同步/空泡 | 违反「峰值显存≥活跃张量之和+碎片+缓存块」时的报错 |
| 量化关系 | t≈max(FLOPs/FLOPS_peak, Bytes/BW_peak)；算术强度=FLOPs/Bytes | 实测与预测偏差超容差 |
| 失败模式 | 未 sync 就读 Event 时间 | 只测稳态忽略首次冷启动 |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 性能 在“并发与资源”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“并发与资源：性能”沿相邻边界延伸：roofline 拐点、显存碎片与快照、通信/计算重叠度量。比较版本与替代实现的差异（如 torch.cuda.max_memory_allocated），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [并发与资源] 如何用 roofline 判断算子 compute-bound 还是 memory-bound，需要哪些输入？
2. [并发与资源] 为什么 profiler 的 self_cuda_time 之和常小于 step time？
3. [并发与资源] 在通信与计算未重叠时，先改数据并行还是分片/bucket 策略？

#### 8. 易错点与反例

1. [并发与资源] 用 CPU 计时测 GPU kernel，得到错误结论
2. [并发与资源] 忽略通信与 sync 造成的时间空泡
3. [并发与资源] 以单次测量下结论，被启动噪声误导

#### 9. 实践与验证

用受控交错（barrier/sleep 或确定性调度）复现竞争，再用压测验证稳态。具体到 性能：profiler 导出 kernel 表按 cuda_time 排序定位热点；固定输入重复 N 次取中位数与 p99。把 未 sync 就读 Event 时间 作为负例，记录 torch.cuda.Event 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“性能”的名词，说不出「所有 kernel 时间之和≤墙钟时间，差额为同步/空泡」或 未 sync 就读 Event 时间。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 t≈max(FLOPs/FLOPS_peak, Bytes/BW_peak)；算术强度=FLOPs/Bytes 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 只测稳态忽略首次冷启动 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A044. 并发与资源：导出部署

- 难度：高级
- 知识域：K10 导出部署
- 考察能力：正确性、性能、成本、运维与迁移条件
- 题型：对比与取舍
- 面试频率：中频
- 前置知识：state_dict、Export、runtime、schema、迁移
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

并发问题的核心是共享状态的可见性与资源竞争的边界。本题位于 K10 序列化/导出/部署。torch.export 基于 Dynamo 捕获 ATen 级图，输出 ExportedProgram（graph_module、state_dict、range_constraints）。

#### 2. 问题分析

从“并发与资源：序列化/导出/部署”出发，先识别共享可变状态、同步点与资源池，把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 torch.save/torch.load(weights_only=) 的契约，列出两条不变量：「导出后的图不含 Python 语义，仅含 ATen/custom op 调用」「state_dict 的 key 语义与训练侧一致」，再给一个会击穿假设的边界输入，并说明它最先暴露 trace 把数据相关分支固化为单一路径 还是 自定义 op 未注册导致导出失败。

#### 3. 具体答案

面试中可以这样回答：

> 我会描述并发模型、同步机制、竞争窗口和一个会触发的交错顺序。在 序列化/导出/部署 上，关键事实是：动态维度需用 dynamic_shapes/Dim 显式声明，否则导出为静态 shape。 判据可写为 导出图=静态化 Python 控制流+固定/声明化 shape（Dim 约束）。

#### 4. 技术洞察

并发正确性依赖 happens-before 关系；缺少同步的读写即使在单机也可能读到撕裂值。在 序列化/导出/部署 上，量化关系是 导出图=静态化 Python 控制流+固定/声明化 shape（Dim 约束）；核心不变量是「导出后的图不含 Python 语义，仅含 ATen/custom op 调用」，它一旦被打破，最先表现为 weights_only=True 加载旧 pickle 报错。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | torch.save/torch.load(weights_only=) 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | 导出后的图不含 Python 语义，仅含 ATen/custom op 调用 | 违反「state_dict 的 key 语义与训练侧一致」时的报错 |
| 量化关系 | 导出图=静态化 Python 控制流+固定/声明化 shape（Dim 约束） | 实测与预测偏差超容差 |
| 失败模式 | trace 把数据相关分支固化为单一路径 | 自定义 op 未注册导致导出失败 |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 序列化/导出/部署 在“并发与资源”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“并发与资源：序列化/导出/部署”沿相邻边界延伸：custom op 的 register_autograd、ONNX/ExecuTorch runtime、版本 schema 演进。比较版本与替代实现的差异（如 torch.export.export），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [并发与资源] torch.jit.trace 与 torch.export 在控制流处理上有什么本质区别？
2. [并发与资源] weights_only 自 2.6 起默认为 True 带来哪些兼容性影响？
3. [并发与资源] 如何用 dynamic_shapes 表达一个仅序列维动态的模型并验证？

#### 8. 易错点与反例

1. [并发与资源] 用 trace 导出含 if/loop 的模型，使数据相关分支被固化
2. [并发与资源] 认为 weights_only=True 永远安全（自定义 op/类仍需注册）
3. [并发与资源] 忽略导出后 dtype/device 与运行时不一致

#### 9. 实践与验证

用受控交错（barrier/sleep 或确定性调度）复现竞争，再用压测验证稳态。具体到 序列化/导出/部署：对比导出前后在相同输入上的输出 assert_close；检查 ExportedProgram.graph_module.graph 的 op 集合。把 trace 把数据相关分支固化为单一路径 作为负例，记录 torch.jit.trace/script 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“序列化/导出/部署”的名词，说不出「导出后的图不含 Python 语义，仅含 ATen/custom op 调用」或 trace 把数据相关分支固化为单一路径。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 导出图=静态化 Python 控制流+固定/声明化 shape（Dim 约束） 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 自定义 op 未注册导致导出失败 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A045. 并发与资源：自定义算子

- 难度：高级
- 知识域：K11 自定义算子
- 考察能力：正确性、性能、成本、运维与迁移条件
- 题型：对比与取舍
- 面试频率：中频
- 前置知识：schema、dispatcher、stream、autograd、ABI
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

并发问题的核心是共享状态的可见性与资源竞争的边界。本题位于 K11 自定义算子。CUDA 扩展编译需匹配 PyTorch 的 C++ ABI（_GLIBCXX_USE_CXX11_ABI）与 CUDA 架构 -gencode。

#### 2. 问题分析

从“并发与资源：自定义算子”出发，先识别共享可变状态、同步点与资源池，把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 torch.library/TORCH_LIBRARY 的契约，列出两条不变量：「torch.autograd.gradcheck 通过表示前向/反向数值一致」「no_grad 与 grad 模式除梯度外结果一致」，再给一个会击穿假设的边界输入，并说明它最先暴露 漏写设备选择/CUDAGuard 使多卡下用错设备 还是 C++ ABI 不匹配报 undefined symbol。

#### 3. 具体答案

面试中可以这样回答：

> 我会描述并发模型、同步机制、竞争窗口和一个会触发的交错顺序。在 自定义算子 上，关键事实是：dispatcher 按 DispatchKey（CPU/CUDA/Autograd）选择实现，schema 决定算子签名。 判据可写为 加速比=t_baseline/t_custom（同 shape/dtype/stream）；occupancy=活跃 warp/最大 warp。

#### 4. 技术洞察

并发正确性依赖 happens-before 关系；缺少同步的读写即使在单机也可能读到撕裂值。在 自定义算子 上，量化关系是 加速比=t_baseline/t_custom（同 shape/dtype/stream）；occupancy=活跃 warp/最大 warp；核心不变量是「torch.autograd.gradcheck 通过表示前向/反向数值一致」，它一旦被打破，最先表现为 内核用原子加导致非确定性。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | torch.library/TORCH_LIBRARY 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | torch.autograd.gradcheck 通过表示前向/反向数值一致 | 违反「no_grad 与 grad 模式除梯度外结果一致」时的报错 |
| 量化关系 | 加速比=t_baseline/t_custom（同 shape/dtype/stream）；occupancy=活跃 warp/最大 warp | 实测与预测偏差超容差 |
| 失败模式 | 漏写设备选择/CUDAGuard 使多卡下用错设备 | C++ ABI 不匹配报 undefined symbol |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 自定义算子 在“并发与资源”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“并发与资源：自定义算子”沿相邻边界延伸：op schema 与 fake/meta 注册、流与事件同步、编译期架构选择。比较版本与替代实现的差异（如 torch.library.register_autograd），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [并发与资源] 为自定义算子实现反向有哪两种机制，各自适用场景是什么？
2. [并发与资源] 为什么 kernel 必须使用当前 stream，如何获取与同步？
3. [并发与资源] CUDA 扩展的 C++ ABI 不匹配会以什么形式暴露？

#### 8. 易错点与反例

1. [并发与资源] 只写前向不注册反向，autograd 报错或梯度为 None
2. [并发与资源] 在默认 stream 上跑 kernel 却从非默认 stream 读数据
3. [并发与资源] 忽略 C++ ABI 造成运行时链接失败

#### 9. 实践与验证

用受控交错（barrier/sleep 或确定性调度）复现竞争，再用压测验证稳态。具体到 自定义算子：gradcheck 校验自定义 Function 的梯度；torch.testing.assert_close 对比参考实现。把 漏写设备选择/CUDAGuard 使多卡下用错设备 作为负例，记录 torch.autograd.Function 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“自定义算子”的名词，说不出「torch.autograd.gradcheck 通过表示前向/反向数值一致」或 漏写设备选择/CUDAGuard 使多卡下用错设备。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 加速比=t_baseline/t_custom（同 shape/dtype/stream）；occupancy=活跃 warp/最大 warp 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 C++ ABI 不匹配报 undefined symbol 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A046. 并发与资源：测试治理

- 难度：高级
- 知识域：K12 测试治理
- 考察能力：正确性、性能、成本、运维与迁移条件
- 题型：对比与取舍
- 面试频率：中频
- 前置知识：oracle、证据、日志、回滚、门禁、复盘
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

并发问题的核心是共享状态的可见性与资源竞争的边界。本题位于 K12 测试治理。非确定性来源包括原子加、cuDNN benchmark、多线程归约顺序。

#### 2. 问题分析

从“并发与资源：测试治理”出发，先识别共享可变状态、同步点与资源池，把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 torch.testing.assert_close 的契约，列出两条不变量：「相同 seed+相同算法下结果可复现」「通过阈值必须与 dtype 精度匹配」，再给一个会击穿假设的边界输入，并说明它最先暴露 对 fp16 用 fp32 阈值导致误报 还是 未固定 seed 造成随机失败。

#### 3. 具体答案

面试中可以这样回答：

> 我会描述并发模型、同步机制、竞争窗口和一个会触发的交错顺序。在 测试治理 上，关键事实是：flaky 测试可用重跑通过率量化而非直接删除。 判据可写为 |a-b|≤atol+rtol×|b|；flake 率≈失败次数/重跑次数。

#### 4. 技术洞察

并发正确性依赖 happens-before 关系；缺少同步的读写即使在单机也可能读到撕裂值。在 测试治理 上，量化关系是 |a-b|≤atol+rtol×|b|；flake 率≈失败次数/重跑次数；核心不变量是「相同 seed+相同算法下结果可复现」，它一旦被打破，最先表现为 把 flaky 当稳定失败反复重试掩盖问题。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | torch.testing.assert_close 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | 相同 seed+相同算法下结果可复现 | 违反「通过阈值必须与 dtype 精度匹配」时的报错 |
| 量化关系 | |a-b|≤atol+rtol×|b|；flake 率≈失败次数/重跑次数 | 实测与预测偏差超容差 |
| 失败模式 | 对 fp16 用 fp32 阈值导致误报 | 未固定 seed 造成随机失败 |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 测试治理 在“并发与资源”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“并发与资源：测试治理”沿相邻边界延伸：golden 值与版本记录、CI matrix 覆盖、门禁与回归集管理。比较版本与替代实现的差异（如 pytest），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [并发与资源] assert_close 的 rtol/atol 如何按 dtype 选择才合理？
2. [并发与资源] gradcheck 在什么情况下会误报失败？
3. [并发与资源] 如何量化并治理 flaky 测试？

#### 8. 易错点与反例

1. [并发与资源] 用 == 比较浮点张量
2. [并发与资源] 以为关闭 cuDNN benchmark 就能完全确定
3. [并发与资源] 把 golden 值硬编码却不记录环境版本

#### 9. 实践与验证

用受控交错（barrier/sleep 或确定性调度）复现竞争，再用压测验证稳态。具体到 测试治理：固定 seed 重跑 N 次统计通过率；用 assert_close 显式指定 rtol/atol 而非手写比较。把 对 fp16 用 fp32 阈值导致误报 作为负例，记录 torch.autograd.gradcheck 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“测试治理”的名词，说不出「相同 seed+相同算法下结果可复现」或 对 fp16 用 fp32 阈值导致误报。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 |a-b|≤atol+rtol×|b|；flake 率≈失败次数/重跑次数 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 未固定 seed 造成随机失败 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A047. 性能优化：张量/设备

- 难度：高级
- 知识域：K01 张量/设备
- 考察能力：正确性、性能、成本、运维与迁移条件
- 题型：对比与取舍
- 面试频率：中频
- 前置知识：shape、stride、dtype、device
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

性能优化必须基于测量而非直觉：先立基线，再定位瓶颈，再验证归因。本题位于 K01 张量/设备。non_blocking=True 只有在源 pinned 且目标是加速器时才真正异步，否则退化为同步拷贝。

#### 2. 问题分析

从“性能优化：张量/设备”出发，先固定基线并分解时间/显存，把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 torch.Tensor.view/reshape 的契约，列出两条不变量：「view 与源共享 storage（data_ptr 与 storage_offset 一致）」「张量的 device/dtype 决定 kernel 派发与是否发生隐式转换」，再给一个会击穿假设的边界输入，并说明它最先暴露 对非连续张量 view 报 'view size is not compatible with input tensor's size and stride' 还是 跨设备运算报 'Expected all tensors to be on the same device'。

#### 3. 具体答案

面试中可以这样回答：

> 我会给出基线数据、主导瓶颈、优化手段与预期收益，并标注测量误差。在 张量/设备 上，关键事实是：连续张量满足 stride[i]=∏_{j>i}shape[j]，shape=(2,3,4) 的 stride 恰为 (12,4,1)。 判据可写为 显存(bytes)=numel×itemsize(dtype)；连续 stride[i]=∏_{j>i}shape[j]。

#### 4. 技术洞察

整体收益受 Amdahl 定律限制：加速≤1/((1-p)+p/s)，先优化占比最大的部分。在 张量/设备 上，量化关系是 显存(bytes)=numel×itemsize(dtype)；连续 stride[i]=∏_{j>i}shape[j]；核心不变量是「view 与源共享 storage（data_ptr 与 storage_offset 一致）」，它一旦被打破，最先表现为 reshape 静默拷贝抬高峰值显存。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | torch.Tensor.view/reshape 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | view 与源共享 storage（data_ptr 与 storage_offset 一致） | 违反「张量的 device/dtype 决定 kernel 派发与是否发生隐式转换」时的报错 |
| 量化关系 | 显存(bytes)=numel×itemsize(dtype)；连续 stride[i]=∏_{j>i}shape[j] | 实测与预测偏差超容差 |
| 失败模式 | 对非连续张量 view 报 'view size is not compatible with input tensor's size and stride' | 跨设备运算报 'Expected all tensors to be on the same device' |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 张量/设备 在“性能优化”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“性能优化：张量/设备”沿相邻边界延伸：view/reshape/contiguous、pin_memory 与 stream、dtype 转换与显存。比较版本与替代实现的差异（如 t.to(device, non_blocking=True)），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [性能优化] shape=(2,3,4) 分别 view 成 (6,4)、reshape 成 (6,4) 后，stride 与是否共享存储各是什么？
2. [性能优化] 为什么 permute 之后必须先 contiguous 才能 view？
3. [性能优化] non_blocking=True 在源未 pinned 时的实际行为如何用 profiler 证明？

#### 8. 易错点与反例

1. [性能优化] 把 reshape 当零拷贝：stride 不兼容时它会 contiguous 并分配新显存
2. [性能优化] 只比较 shape 相等就判断可 view 互转，忽略 stride/storage_offset
3. [性能优化] 以为 pin_memory 总有效，实际它只对 non_blocking 有加速意义

#### 9. 实践与验证

warmup 后重复测量取中位数与 p99，单变量改变并用 profiler 归因。具体到 张量/设备：构造 (2,3,4) 后断言 t.stride()==(12,4,1) 且 t.is_contiguous()；用 t.data_ptr() 验证 view 与 reshape 的共享/拷贝差异。把 对非连续张量 view 报 'view size is not compatible with input tensor's size and stride' 作为负例，记录 t.stride()/t.is_contiguous() 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“张量/设备”的名词，说不出「view 与源共享 storage（data_ptr 与 storage_offset 一致）」或 对非连续张量 view 报 'view size is not compatible with input tensor's size and stride'。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 显存(bytes)=numel×itemsize(dtype)；连续 stride[i]=∏_{j>i}shape[j] 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 跨设备运算报 'Expected all tensors to be on the same device' 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A048. 性能优化：autograd

- 难度：高级
- 知识域：K02 autograd
- 考察能力：正确性、性能、成本、运维与迁移条件
- 题型：对比与取舍
- 面试频率：中频
- 前置知识：requires_grad、leaf、view、detach、反向
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

性能优化必须基于测量而非直觉：先立基线，再定位瓶颈，再验证归因。本题位于 K02 autograd。param.data.add_() 绕过 autograd，是原地更新权重而不破坏图的标准做法。

#### 2. 问题分析

从“性能优化：autograd”出发，先固定基线并分解时间/显存，把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 Tensor.requires_grad_ 的契约，列出两条不变量：「叶子张量 .grad 的形状与张量一致且只由叶子持有」「requires_grad 沿 grad_fn 链单向传播」，再给一个会击穿假设的边界输入，并说明它最先暴露 忘记 zero_grad 使梯度累加 还是 对 requires_grad 的叶子做原地操作报 'a leaf Variable that requires grad is being used in an in-place operation'。

#### 3. 具体答案

面试中可以这样回答：

> 我会给出基线数据、主导瓶颈、优化手段与预期收益，并标注测量误差。在 autograd 上，关键事实是：只有 requires_grad=True 的叶子张量在反向时把梯度累加到 .grad，非叶子的 .grad 默认是 None。 判据可写为 累加语义 g←g+∂L/∂θ；链式法则 dL/dx=Σ(dL/dy·dy/dx)。

#### 4. 技术洞察

整体收益受 Amdahl 定律限制：加速≤1/((1-p)+p/s)，先优化占比最大的部分。在 autograd 上，量化关系是 累加语义 g←g+∂L/∂θ；链式法则 dL/dx=Σ(dL/dy·dy/dx)；核心不变量是「叶子张量 .grad 的形状与张量一致且只由叶子持有」，它一旦被打破，最先表现为 两次 backward 未设 retain_graph 触发 RuntimeError。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | Tensor.requires_grad_ 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | 叶子张量 .grad 的形状与张量一致且只由叶子持有 | 违反「requires_grad 沿 grad_fn 链单向传播」时的报错 |
| 量化关系 | 累加语义 g←g+∂L/∂θ；链式法则 dL/dx=Σ(dL/dy·dy/dx) | 实测与预测偏差超容差 |
| 失败模式 | 忘记 zero_grad 使梯度累加 | 对 requires_grad 的叶子做原地操作报 'a leaf Variable that requires grad is being used in an in-place operation' |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 autograd 在“性能优化”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“性能优化：autograd”沿相邻边界延伸：detach、retain_grad、in-place 与版本计数、gradcheck。比较版本与替代实现的差异（如 torch.autograd.grad），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [性能优化] 为什么非叶子张量的 .grad 默认是 None，retain_grad() 改变了什么？
2. [性能优化] detach() 与 with torch.no_grad() 在切断梯度上的差别是什么？
3. [性能优化] in-place 操作在什么条件下能安全使用而不破坏 autograd？

#### 8. 易错点与反例

1. [性能优化] 认为非叶子 .grad 会自动填充，实则默认 None，需要 retain_grad()
2. [性能优化] 在 no_grad 下构建输入再 backward，得到 requires_grad 缺失错误
3. [性能优化] 用 param = param - lr*g 原地更新破坏计算图

#### 9. 实践与验证

warmup 后重复测量取中位数与 p99，单变量改变并用 profiler 归因。具体到 autograd：断言 x.is_leaf and x.grad is None 后再 backward；用 torch.autograd.grad(..., retain_graph=True) 做两次反向对比。把 忘记 zero_grad 使梯度累加 作为负例，记录 Tensor.detach() 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“autograd”的名词，说不出「叶子张量 .grad 的形状与张量一致且只由叶子持有」或 忘记 zero_grad 使梯度累加。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 累加语义 g←g+∂L/∂θ；链式法则 dL/dx=Σ(dL/dy·dy/dx) 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 对 requires_grad 的叶子做原地操作报 'a leaf Variable that requires grad is being used in an in-place operation' 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A049. 性能优化：Module/状态

- 难度：高级
- 知识域：K03 Module/状态
- 考察能力：正确性、性能、成本、运维与迁移条件
- 题型：对比与取舍
- 面试频率：中频
- 前置知识：参数、buffer、模式和 state_dict
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

性能优化必须基于测量而非直觉：先立基线，再定位瓶颈，再验证归因。本题位于 K03 Module/状态。BatchNorm 的 running_mean/running_var 是 buffer，eval 下不再更新、只用统计量。

#### 2. 问题分析

从“性能优化：Module/状态”出发，先固定基线并分解时间/显存，把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 register_buffer 的契约，列出两条不变量：「同一 Module 的 state_dict key 集合确定且与设备无关」「buffer 的 dtype/device 跟随 module 或按注册时指定」，再给一个会击穿假设的边界输入，并说明它最先暴露 自定义 buffer 未注册，.to(device) 后仍留在 CPU 还是 load_state_dict 因 missing/unexpected keys 失败。

#### 3. 具体答案

面试中可以这样回答：

> 我会给出基线数据、主导瓶颈、优化手段与预期收益，并标注测量误差。在 Module/状态 上，关键事实是：buffer 参与 state_dict 但不是参数、不参与梯度；Parameter 两者都占。 判据可写为 state_dict={k:param}∪{k:buffer}；训练态显存≈Σ numel×itemsize×(1+grad+opt_state)。

#### 4. 技术洞察

整体收益受 Amdahl 定律限制：加速≤1/((1-p)+p/s)，先优化占比最大的部分。在 Module/状态 上，量化关系是 state_dict={k:param}∪{k:buffer}；训练态显存≈Σ numel×itemsize×(1+grad+opt_state)；核心不变量是「同一 Module 的 state_dict key 集合确定且与设备无关」，它一旦被打破，最先表现为 eval 下忘记切换使 Dropout 仍在抽样。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | register_buffer 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | 同一 Module 的 state_dict key 集合确定且与设备无关 | 违反「buffer 的 dtype/device 跟随 module 或按注册时指定」时的报错 |
| 量化关系 | state_dict={k:param}∪{k:buffer}；训练态显存≈Σ numel×itemsize×(1+grad+opt_state) | 实测与预测偏差超容差 |
| 失败模式 | 自定义 buffer 未注册，.to(device) 后仍留在 CPU | load_state_dict 因 missing/unexpected keys 失败 |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 Module/状态 在“性能优化”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“性能优化：Module/状态”沿相邻边界延伸：persistent buffer、hook 与状态迁移、DDP 下的 buffer 同步。比较版本与替代实现的差异（如 state_dict/load_state_dict），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [性能优化] buffer 与 Parameter 在 state_dict、梯度和设备迁移上的差异是什么？
2. [性能优化] BatchNorm 在 eval 模式仍更新 running stats 需要什么额外设置？
3. [性能优化] register_buffer(persistent=False) 会从 state_dict 中移除什么？

#### 8. 易错点与反例

1. [性能优化] 把计数/统计量存成普通属性，导致 state_dict 丢失该状态
2. [性能优化] load_state_dict 后忘记 module.eval()，使 BN/Dropout 行为不符
3. [性能优化] 直接给同名属性赋值绕过注册，使 named_parameters 缺失

#### 9. 实践与验证

warmup 后重复测量取中位数与 p99，单变量改变并用 profiler 归因。具体到 Module/状态：断言 set(m.state_dict().keys()) 与 checkpoint 完全一致；对比 train/eval 两次前向输出验证 Dropout 关闭。把 自定义 buffer 未注册，.to(device) 后仍留在 CPU 作为负例，记录 register_parameter 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“Module/状态”的名词，说不出「同一 Module 的 state_dict key 集合确定且与设备无关」或 自定义 buffer 未注册，.to(device) 后仍留在 CPU。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 state_dict={k:param}∪{k:buffer}；训练态显存≈Σ numel×itemsize×(1+grad+opt_state) 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 load_state_dict 因 missing/unexpected keys 失败 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A050. 性能优化：数据管道

- 难度：高级
- 知识域：K04 数据管道
- 考察能力：正确性、性能、成本、运维与迁移条件
- 题型：对比与取舍
- 面试频率：中频
- 前置知识：Dataset、Sampler、collate、padding、版本
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

性能优化必须基于测量而非直觉：先立基线，再定位瓶颈，再验证归因。本题位于 K04 数据管道。pin_memory=True 把 batch 放进页锁定内存，配合 non_blocking 才能与计算重叠。

#### 2. 问题分析

从“性能优化：数据管道”出发，先固定基线并分解时间/显存，把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 DataLoader(num_workers, pin_memory, persistent_workers) 的契约，列出两条不变量：「shuffle=False 且种子固定时迭代顺序可复现」「各 rank 采样集合互斥且并集覆盖全集（或按 drop_last 截断）」，再给一个会击穿假设的边界输入，并说明它最先暴露 多 worker 共享同一随机源导致样本重复 还是 worker 数过多耗尽文件描述符或内存。

#### 3. 具体答案

面试中可以这样回答：

> 我会给出基线数据、主导瓶颈、优化手段与预期收益，并标注测量误差。在 数据管道 上，关键事实是：num_workers=0 在主进程取数，>0 时 fork 子进程，每个 worker 有独立 Python 状态与 seed。 判据可写为 吞吐≤min(存储带宽, CPU 解码速率, H2D 带宽)；有效样本/步=batch×world_size。

#### 4. 技术洞察

整体收益受 Amdahl 定律限制：加速≤1/((1-p)+p/s)，先优化占比最大的部分。在 数据管道 上，量化关系是 吞吐≤min(存储带宽, CPU 解码速率, H2D 带宽)；有效样本/步=batch×world_size；核心不变量是「shuffle=False 且种子固定时迭代顺序可复现」，它一旦被打破，最先表现为 DistributedSampler 与自定义 shuffle 双打乱造成跨 rank 重复。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | DataLoader(num_workers, pin_memory, persistent_workers) 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | shuffle=False 且种子固定时迭代顺序可复现 | 违反「各 rank 采样集合互斥且并集覆盖全集（或按 drop_last 截断）」时的报错 |
| 量化关系 | 吞吐≤min(存储带宽, CPU 解码速率, H2D 带宽)；有效样本/步=batch×world_size | 实测与预测偏差超容差 |
| 失败模式 | 多 worker 共享同一随机源导致样本重复 | worker 数过多耗尽文件描述符或内存 |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 数据管道 在“性能优化”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“性能优化：数据管道”沿相邻边界延伸：IterableDataset、chunk 预取、对象存储吞吐与 IPC 开销。比较版本与替代实现的差异（如 collate_fn），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [性能优化] DistributedSampler 如何处理样本数不被 world_size 整除，是否引入重复样本？
2. [性能优化] num_workers 增大时首步延迟、内存与 IPC 开销如何变化？
3. [性能优化] pin_memory 与 non_blocking 为何必须同时开启才有效？

#### 8. 易错点与反例

1. [性能优化] 把 num_workers 调大就以为更快，忽略 IPC 与内存放大
2. [性能优化] Dataset 里用全局随机而不经 worker_init_fn 重播种
3. [性能优化] 自己 shuffle 又用 DistributedSampler，导致跨 rank 样本重复

#### 9. 实践与验证

warmup 后重复测量取中位数与 p99，单变量改变并用 profiler 归因。具体到 数据管道：固定 seed 两次迭代断言顺序一致；统计各 rank 采样索引并集是否覆盖全集。把 多 worker 共享同一随机源导致样本重复 作为负例，记录 DistributedSampler 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“数据管道”的名词，说不出「shuffle=False 且种子固定时迭代顺序可复现」或 多 worker 共享同一随机源导致样本重复。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 吞吐≤min(存储带宽, CPU 解码速率, H2D 带宽)；有效样本/步=batch×world_size 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 worker 数过多耗尽文件描述符或内存 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A051. 性能优化：训练/优化器

- 难度：高级
- 知识域：K05 训练/优化器
- 考察能力：输入矩阵、oracle、故障注入与指标
- 题型：测试/实验与验证
- 面试频率：中频
- 前置知识：forward、backward、累积、optimizer、scheduler
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

性能优化必须基于测量而非直觉：先立基线，再定位瓶颈，再验证归因。本题位于 K05 训练/优化器。Adam 每参数维护 exp_avg 与 exp_avg_sq，优化器状态约 2×参数量。

#### 2. 问题分析

从“性能优化：训练/优化器”出发，先固定基线并分解时间/显存，把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 optimizer.zero_grad(set_to_none=True) 的契约，列出两条不变量：「反向之前梯度应为空或已清零」「scheduler 与 optimizer 的 step 次数对齐」，再给一个会击穿假设的边界输入，并说明它最先暴露 scheduler.step() 放错位置使 lr 序列整体偏移 还是 累积时直接累加 loss 未归一化。

#### 3. 具体答案

面试中可以这样回答：

> 我会给出基线数据、主导瓶颈、优化手段与预期收益，并标注测量误差。在 训练/优化器 上，关键事实是：set_to_none=True 把 .grad 置 None 而非填零，省显存并让后续加法重新分配。 判据可写为 有效 batch=micro_batch×grad_accum×world_size；Adam 状态≈2P(状态)+P(梯度)。

#### 4. 技术洞察

整体收益受 Amdahl 定律限制：加速≤1/((1-p)+p/s)，先优化占比最大的部分。在 训练/优化器 上，量化关系是 有效 batch=micro_batch×grad_accum×world_size；Adam 状态≈2P(状态)+P(梯度)；核心不变量是「反向之前梯度应为空或已清零」，它一旦被打破，最先表现为 zero_grad 漏调导致梯度累加。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | optimizer.zero_grad(set_to_none=True) 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | 反向之前梯度应为空或已清零 | 违反「scheduler 与 optimizer 的 step 次数对齐」时的报错 |
| 量化关系 | 有效 batch=micro_batch×grad_accum×world_size；Adam 状态≈2P(状态)+P(梯度) | 实测与预测偏差超容差 |
| 失败模式 | scheduler.step() 放错位置使 lr 序列整体偏移 | 累积时直接累加 loss 未归一化 |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 训练/优化器 在“性能优化”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“性能优化：训练/优化器”沿相邻边界延伸：scheduler 恢复、优化器 state_dict、foreach/fused 实现差异。比较版本与替代实现的差异（如 lr_scheduler.step()），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [性能优化] lr_scheduler.step() 在 PyTorch 1.1.0 前后位置的语义差异是什么？
2. [性能优化] 梯度累积如何在数学上等价于大 batch，需要哪些归一化？
3. [性能优化] set_to_none=True 对显存与优化器行为有何具体影响？

#### 8. 易错点与反例

1. [性能优化] 用 zero_grad() 不传 set_to_none，长期分配开销更高
2. [性能优化] 把 scheduler.step() 在 epoch 与 step 间混用，导致 lr 跳变
3. [性能优化] 梯度累积时未除 steps，使等效学习率偏大

#### 9. 实践与验证

warmup 后重复测量取中位数与 p99，单变量改变并用 profiler 归因。具体到 训练/优化器：打印前若干步 param_groups[0]['lr'] 验证调度曲线；比较累积 N 步与单步大 batch 的梯度 L2 范数。把 scheduler.step() 放错位置使 lr 序列整体偏移 作为负例，记录 optimizer.step() 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“训练/优化器”的名词，说不出「反向之前梯度应为空或已清零」或 scheduler.step() 放错位置使 lr 序列整体偏移。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 有效 batch=micro_batch×grad_accum×world_size；Adam 状态≈2P(状态)+P(梯度) 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 累积时直接累加 loss 未归一化 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A052. 性能优化：AMP/恢复

- 难度：高级
- 知识域：K06 AMP/恢复
- 考察能力：输入矩阵、oracle、故障注入与指标
- 题型：测试/实验与验证
- 面试频率：中频
- 前置知识：autocast、scaler、快照、随机状态
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

性能优化必须基于测量而非直觉：先立基线，再定位瓶颈，再验证归因。本题位于 K06 AMP/恢复。bf16 动态范围与 fp32 相同，通常无需 GradScaler；fp16 才需要 loss scaling。

#### 2. 问题分析

从“性能优化：AMP/恢复”出发，先固定基线并分解时间/显存，把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 torch.autocast(device_type) 的契约，列出两条不变量：「反向前 loss 被 scale 放大，step 前梯度必须被 unscale」「inf/nan 检查与 skip 分支成对出现」，再给一个会击穿假设的边界输入，并说明它最先暴露 clip 放在 unscale 之前使梯度幅度错误 还是 对 bf16 仍用 GradScaler 造成无谓开销。

#### 3. 具体答案

面试中可以这样回答：

> 我会给出基线数据、主导瓶颈、优化手段与预期收益，并标注测量误差。在 AMP/恢复 上，关键事实是：GradScaler 默认 init_scale=65536、growth_factor=2.0、backoff_factor=0.5、growth_interval=2000。 判据可写为 scale_{new}=scale×growth_factor（连续无溢出）或 scale/backoff_factor（检测到 inf/nan）。

#### 4. 技术洞察

整体收益受 Amdahl 定律限制：加速≤1/((1-p)+p/s)，先优化占比最大的部分。在 AMP/恢复 上，量化关系是 scale_{new}=scale×growth_factor（连续无溢出）或 scale/backoff_factor（检测到 inf/nan）；核心不变量是「反向前 loss 被 scale 放大，step 前梯度必须被 unscale」，它一旦被打破，最先表现为 checkpoint 未保存 scaler.state_dict 使恢复后 scale 不连续。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | torch.autocast(device_type) 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | 反向前 loss 被 scale 放大，step 前梯度必须被 unscale | 违反「inf/nan 检查与 skip 分支成对出现」时的报错 |
| 量化关系 | scale_{new}=scale×growth_factor（连续无溢出）或 scale/backoff_factor（检测到 inf/nan） | 实测与预测偏差超容差 |
| 失败模式 | clip 放在 unscale 之前使梯度幅度错误 | 对 bf16 仍用 GradScaler 造成无谓开销 |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 AMP/恢复 在“性能优化”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“性能优化：AMP/恢复”沿相邻边界延伸：autocast 的 op 白/黑名单、随机状态快照、断点续训一致性。比较版本与替代实现的差异（如 scaler.scale/step/update），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [性能优化] GradScaler 连续多少次无溢出才提高 scale，默认参数是多少？
2. [性能优化] 为什么 fp16 autocast 必须 unscale 之后才能裁剪梯度？
3. [性能优化] bf16 为什么通常不需要 GradScaler？

#### 8. 易错点与反例

1. [性能优化] 以为 autocast 会把参数也转 fp16（参数保持 fp32）
2. [性能优化] 用未 unscale 的梯度直接裁剪
3. [性能优化] 恢复训练时忘记保存/加载 scaler 状态

#### 9. 实践与验证

warmup 后重复测量取中位数与 p99，单变量改变并用 profiler 归因。具体到 AMP/恢复：记录 scaler.get_scale() 在若干 step 的轨迹；注入含 nan 的 loss 验证该 step 被跳过。把 clip 放在 unscale 之前使梯度幅度错误 作为负例，记录 GradScaler 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“AMP/恢复”的名词，说不出「反向前 loss 被 scale 放大，step 前梯度必须被 unscale」或 clip 放在 unscale 之前使梯度幅度错误。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 scale_{new}=scale×growth_factor（连续无溢出）或 scale/backoff_factor（检测到 inf/nan） 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 对 bf16 仍用 GradScaler 造成无谓开销 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A053. 性能优化：分布式

- 难度：高级
- 知识域：K07 分布式
- 考察能力：输入矩阵、oracle、故障注入与指标
- 题型：测试/实验与验证
- 面试频率：中频
- 前置知识：rank、DDP、FSDP、collective、容错
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

性能优化必须基于测量而非直觉：先立基线，再定位瓶颈，再验证归因。本题位于 K07 分布式。ring allreduce 通信量约 2(N-1)/N×M，N 很大时趋近 2M。

#### 2. 问题分析

从“性能优化：分布式”出发，先固定基线并分解时间/显存，把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 init_process_group 的契约，列出两条不变量：「所有 rank 的 rank/world_size 一致」「collective 调用序列在各 rank 严格一致」，再给一个会击穿假设的边界输入，并说明它最先暴露 某些 rank 提前 return 使 collective 不匹配而 hang 还是 梯度 size 不一致触发 'Mismatch in collective'。

#### 3. 具体答案

面试中可以这样回答：

> 我会给出基线数据、主导瓶颈、优化手段与预期收益，并标注测量误差。在 分布式 上，关键事实是：DDP 在反向中把梯度按 bucket（默认 bucket_cap_mb=25）allreduce 并与反向计算重叠。 判据可写为 ring allreduce≈2(N-1)/N×M；DDP 每卡显存≈params+grads+opt_states。

#### 4. 技术洞察

整体收益受 Amdahl 定律限制：加速≤1/((1-p)+p/s)，先优化占比最大的部分。在 分布式 上，量化关系是 ring allreduce≈2(N-1)/N×M；DDP 每卡显存≈params+grads+opt_states；核心不变量是「所有 rank 的 rank/world_size 一致」，它一旦被打破，最先表现为 FSDP 未 wrap 子模块使参数无法分片。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | init_process_group 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | 所有 rank 的 rank/world_size 一致 | 违反「collective 调用序列在各 rank 严格一致」时的报错 |
| 量化关系 | ring allreduce≈2(N-1)/N×M；DDP 每卡显存≈params+grads+opt_states | 实测与预测偏差超容差 |
| 失败模式 | 某些 rank 提前 return 使 collective 不匹配而 hang | 梯度 size 不一致触发 'Mismatch in collective' |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 分布式 在“性能优化”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“性能优化：分布式”沿相邻边界延伸：process group 切分、混合并行、NCCL 拓扑与超时。比较版本与替代实现的差异（如 FullyShardedDataParallel），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [性能优化] DDP 的梯度 bucketing 如何影响通信与计算重叠？
2. [性能优化] FSDP 的 all_gather 与 reduce_scatter 分别在前向/反向何处发生？
3. [性能优化] collective 调用顺序不一致时表现为 hang 还是报错，原因是什么？

#### 8. 易错点与反例

1. [性能优化] 对非参数 buffer 手动 allreduce，造成重复同步
2. [性能优化] 认为 FSDP 降低单卡计算量（实际只降显存）
3. [性能优化] 忽略 bucket_cap_mb 对重叠与峰值显存的影响

#### 9. 实践与验证

warmup 后重复测量取中位数与 p99，单变量改变并用 profiler 归因。具体到 分布式：barrier 后断言 get_rank()/get_world_size()；对比 DDP 与 FSDP 的 max_memory_allocated。把 某些 rank 提前 return 使 collective 不匹配而 hang 作为负例，记录 DistributedDataParallel 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“分布式”的名词，说不出「所有 rank 的 rank/world_size 一致」或 某些 rank 提前 return 使 collective 不匹配而 hang。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 ring allreduce≈2(N-1)/N×M；DDP 每卡显存≈params+grads+opt_states 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 梯度 size 不一致触发 'Mismatch in collective' 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A054. 性能优化：编译栈

- 难度：高级
- 知识域：K08 编译栈
- 考察能力：输入矩阵、oracle、故障注入与指标
- 题型：测试/实验与验证
- 面试频率：中频
- 前置知识：Dynamo、AOTAutograd、Inductor、guard、缓存
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

性能优化必须基于测量而非直觉：先立基线，再定位瓶颈，再验证归因。本题位于 K08 编译栈。reduce-overhead 使用 CUDA Graphs 降低 launch 开销，但要求输入地址稳定。

#### 2. 问题分析

从“性能优化：编译栈”出发，先固定基线并分解时间/显存，把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 torch.compile 的契约，列出两条不变量：「fullgraph=True 时任何 graph break 都抛错」「同一 guard 命中后不再触发编译」，再给一个会击穿假设的边界输入，并说明它最先暴露 动态 shape 未标记导致反复重编译 还是 数据相关控制流触发 graph break。

#### 3. 具体答案

面试中可以这样回答：

> 我会给出基线数据、主导瓶颈、优化手段与预期收益，并标注测量误差。在 编译栈 上，关键事实是：Dynamo 捕获 Python 字节码生成 FX 图，对类型/形状/值建立 guard；guard 失效即重编译。 判据可写为 编译缓存以 guard 值集合为键；重编译次数∝出现过的不同 guard 组合数。

#### 4. 技术洞察

整体收益受 Amdahl 定律限制：加速≤1/((1-p)+p/s)，先优化占比最大的部分。在 编译栈 上，量化关系是 编译缓存以 guard 值集合为键；重编译次数∝出现过的不同 guard 组合数；核心不变量是「fullgraph=True 时任何 graph break 都抛错」，它一旦被打破，最先表现为 CUDA Graph 因输入地址变化捕获失败。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | torch.compile 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | fullgraph=True 时任何 graph break 都抛错 | 违反「同一 guard 命中后不再触发编译」时的报错 |
| 量化关系 | 编译缓存以 guard 值集合为键；重编译次数∝出现过的不同 guard 组合数 | 实测与预测偏差超容差 |
| 失败模式 | 动态 shape 未标记导致反复重编译 | 数据相关控制流触发 graph break |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 编译栈 在“性能优化”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“性能优化：编译栈”沿相邻边界延伸：dynamic shape 的 Dim/mark_dynamic、guard 导出、AOT 与自定义 op。比较版本与替代实现的差异（如 TORCH_LOGS=graph_breaks,recompiles），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [性能优化] Dynamo 的 guard 在什么条件下失效并触发重编译？
2. [性能优化] graph break 对 autograd 图切分与状态副作用带来什么风险？
3. [性能优化] reduce-overhead 的 CUDA Graph 为何要求输入地址固定？

#### 8. 易错点与反例

1. [性能优化] 以为 graph break 只影响性能，忽略断点两侧的状态副作用
2. [性能优化] 用固定 shape 基准掩盖动态 shape 的重编译
3. [性能优化] 忽略首次编译冷启动开销

#### 9. 实践与验证

warmup 后重复测量取中位数与 p99，单变量改变并用 profiler 归因。具体到 编译栈：用 torch._dynamo.explain 统计 graph_breaks 数量；监控 counters['inductor']['recompiles']。把 动态 shape 未标记导致反复重编译 作为负例，记录 torch._dynamo.explain 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“编译栈”的名词，说不出「fullgraph=True 时任何 graph break 都抛错」或 动态 shape 未标记导致反复重编译。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 编译缓存以 guard 值集合为键；重编译次数∝出现过的不同 guard 组合数 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 数据相关控制流触发 graph break 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A055. 性能优化：性能

- 难度：高级
- 知识域：K09 性能
- 考察能力：输入矩阵、oracle、故障注入与指标
- 题型：测试/实验与验证
- 面试频率：中频
- 前置知识：关键路径、带宽、算力、通信、显存、统计
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

性能优化必须基于测量而非直觉：先立基线，再定位瓶颈，再验证归因。本题位于 K09 性能。注意力前向 FLOPs 约为 4·b·h·s²·d（含 QK^T 与 softmax 加权）。

#### 2. 问题分析

从“性能优化：性能”出发，先固定基线并分解时间/显存，把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 torch.profiler 的契约，列出两条不变量：「所有 kernel 时间之和≤墙钟时间，差额为同步/空泡」「峰值显存≥活跃张量之和+碎片+缓存块」，再给一个会击穿假设的边界输入，并说明它最先暴露 未 sync 就读 Event 时间 还是 只测稳态忽略首次冷启动。

#### 3. 具体答案

面试中可以这样回答：

> 我会给出基线数据、主导瓶颈、优化手段与预期收益，并标注测量误差。在 性能 上，关键事实是：roofline 下 kernel 时间≈max(FLOPs/峰值算力, 访存字节/峰值带宽)，取决于算术强度。 判据可写为 t≈max(FLOPs/FLOPS_peak, Bytes/BW_peak)；算术强度=FLOPs/Bytes。

#### 4. 技术洞察

整体收益受 Amdahl 定律限制：加速≤1/((1-p)+p/s)，先优化占比最大的部分。在 性能 上，量化关系是 t≈max(FLOPs/FLOPS_peak, Bytes/BW_peak)；算术强度=FLOPs/Bytes；核心不变量是「所有 kernel 时间之和≤墙钟时间，差额为同步/空泡」，它一旦被打破，最先表现为 把 H2D 拷贝算进 GPU 计算时间。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | torch.profiler 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | 所有 kernel 时间之和≤墙钟时间，差额为同步/空泡 | 违反「峰值显存≥活跃张量之和+碎片+缓存块」时的报错 |
| 量化关系 | t≈max(FLOPs/FLOPS_peak, Bytes/BW_peak)；算术强度=FLOPs/Bytes | 实测与预测偏差超容差 |
| 失败模式 | 未 sync 就读 Event 时间 | 只测稳态忽略首次冷启动 |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 性能 在“性能优化”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“性能优化：性能”沿相邻边界延伸：roofline 拐点、显存碎片与快照、通信/计算重叠度量。比较版本与替代实现的差异（如 torch.cuda.max_memory_allocated），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [性能优化] 如何用 roofline 判断算子 compute-bound 还是 memory-bound，需要哪些输入？
2. [性能优化] 为什么 profiler 的 self_cuda_time 之和常小于 step time？
3. [性能优化] 在通信与计算未重叠时，先改数据并行还是分片/bucket 策略？

#### 8. 易错点与反例

1. [性能优化] 用 CPU 计时测 GPU kernel，得到错误结论
2. [性能优化] 忽略通信与 sync 造成的时间空泡
3. [性能优化] 以单次测量下结论，被启动噪声误导

#### 9. 实践与验证

warmup 后重复测量取中位数与 p99，单变量改变并用 profiler 归因。具体到 性能：profiler 导出 kernel 表按 cuda_time 排序定位热点；固定输入重复 N 次取中位数与 p99。把 未 sync 就读 Event 时间 作为负例，记录 torch.cuda.Event 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“性能”的名词，说不出「所有 kernel 时间之和≤墙钟时间，差额为同步/空泡」或 未 sync 就读 Event 时间。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 t≈max(FLOPs/FLOPS_peak, Bytes/BW_peak)；算术强度=FLOPs/Bytes 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 只测稳态忽略首次冷启动 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A056. 性能优化：导出部署

- 难度：高级
- 知识域：K10 导出部署
- 考察能力：输入矩阵、oracle、故障注入与指标
- 题型：测试/实验与验证
- 面试频率：中频
- 前置知识：state_dict、Export、runtime、schema、迁移
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

性能优化必须基于测量而非直觉：先立基线，再定位瓶颈，再验证归因。本题位于 K10 序列化/导出/部署。动态维度需用 dynamic_shapes/Dim 显式声明，否则导出为静态 shape。

#### 2. 问题分析

从“性能优化：序列化/导出/部署”出发，先固定基线并分解时间/显存，把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 torch.save/torch.load(weights_only=) 的契约，列出两条不变量：「导出后的图不含 Python 语义，仅含 ATen/custom op 调用」「state_dict 的 key 语义与训练侧一致」，再给一个会击穿假设的边界输入，并说明它最先暴露 trace 把数据相关分支固化为单一路径 还是 自定义 op 未注册导致导出失败。

#### 3. 具体答案

面试中可以这样回答：

> 我会给出基线数据、主导瓶颈、优化手段与预期收益，并标注测量误差。在 序列化/导出/部署 上，关键事实是：PyTorch 2.6 起 torch.load 的 weights_only 默认为 True，加载含自定义类的 pickle 需显式关闭。 判据可写为 导出图=静态化 Python 控制流+固定/声明化 shape（Dim 约束）。

#### 4. 技术洞察

整体收益受 Amdahl 定律限制：加速≤1/((1-p)+p/s)，先优化占比最大的部分。在 序列化/导出/部署 上，量化关系是 导出图=静态化 Python 控制流+固定/声明化 shape（Dim 约束）；核心不变量是「导出后的图不含 Python 语义，仅含 ATen/custom op 调用」，它一旦被打破，最先表现为 weights_only=True 加载旧 pickle 报错。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | torch.save/torch.load(weights_only=) 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | 导出后的图不含 Python 语义，仅含 ATen/custom op 调用 | 违反「state_dict 的 key 语义与训练侧一致」时的报错 |
| 量化关系 | 导出图=静态化 Python 控制流+固定/声明化 shape（Dim 约束） | 实测与预测偏差超容差 |
| 失败模式 | trace 把数据相关分支固化为单一路径 | 自定义 op 未注册导致导出失败 |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 序列化/导出/部署 在“性能优化”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“性能优化：序列化/导出/部署”沿相邻边界延伸：custom op 的 register_autograd、ONNX/ExecuTorch runtime、版本 schema 演进。比较版本与替代实现的差异（如 torch.export.export），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [性能优化] torch.jit.trace 与 torch.export 在控制流处理上有什么本质区别？
2. [性能优化] weights_only 自 2.6 起默认为 True 带来哪些兼容性影响？
3. [性能优化] 如何用 dynamic_shapes 表达一个仅序列维动态的模型并验证？

#### 8. 易错点与反例

1. [性能优化] 用 trace 导出含 if/loop 的模型，使数据相关分支被固化
2. [性能优化] 认为 weights_only=True 永远安全（自定义 op/类仍需注册）
3. [性能优化] 忽略导出后 dtype/device 与运行时不一致

#### 9. 实践与验证

warmup 后重复测量取中位数与 p99，单变量改变并用 profiler 归因。具体到 序列化/导出/部署：对比导出前后在相同输入上的输出 assert_close；检查 ExportedProgram.graph_module.graph 的 op 集合。把 trace 把数据相关分支固化为单一路径 作为负例，记录 torch.jit.trace/script 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“序列化/导出/部署”的名词，说不出「导出后的图不含 Python 语义，仅含 ATen/custom op 调用」或 trace 把数据相关分支固化为单一路径。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 导出图=静态化 Python 控制流+固定/声明化 shape（Dim 约束） 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 自定义 op 未注册导致导出失败 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A057. 性能优化：自定义算子

- 难度：高级
- 知识域：K11 自定义算子
- 考察能力：输入矩阵、oracle、故障注入与指标
- 题型：测试/实验与验证
- 面试频率：中频
- 前置知识：schema、dispatcher、stream、autograd、ABI
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

性能优化必须基于测量而非直觉：先立基线，再定位瓶颈，再验证归因。本题位于 K11 自定义算子。dispatcher 按 DispatchKey（CPU/CUDA/Autograd）选择实现，schema 决定算子签名。

#### 2. 问题分析

从“性能优化：自定义算子”出发，先固定基线并分解时间/显存，把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 torch.library/TORCH_LIBRARY 的契约，列出两条不变量：「torch.autograd.gradcheck 通过表示前向/反向数值一致」「no_grad 与 grad 模式除梯度外结果一致」，再给一个会击穿假设的边界输入，并说明它最先暴露 漏写设备选择/CUDAGuard 使多卡下用错设备 还是 C++ ABI 不匹配报 undefined symbol。

#### 3. 具体答案

面试中可以这样回答：

> 我会给出基线数据、主导瓶颈、优化手段与预期收益，并标注测量误差。在 自定义算子 上，关键事实是：自定义算子要让 autograd 可用，必须为 Tensor 参数注册反向（Function 或 register_autograd）。 判据可写为 加速比=t_baseline/t_custom（同 shape/dtype/stream）；occupancy=活跃 warp/最大 warp。

#### 4. 技术洞察

整体收益受 Amdahl 定律限制：加速≤1/((1-p)+p/s)，先优化占比最大的部分。在 自定义算子 上，量化关系是 加速比=t_baseline/t_custom（同 shape/dtype/stream）；occupancy=活跃 warp/最大 warp；核心不变量是「torch.autograd.gradcheck 通过表示前向/反向数值一致」，它一旦被打破，最先表现为 内核用原子加导致非确定性。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | torch.library/TORCH_LIBRARY 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | torch.autograd.gradcheck 通过表示前向/反向数值一致 | 违反「no_grad 与 grad 模式除梯度外结果一致」时的报错 |
| 量化关系 | 加速比=t_baseline/t_custom（同 shape/dtype/stream）；occupancy=活跃 warp/最大 warp | 实测与预测偏差超容差 |
| 失败模式 | 漏写设备选择/CUDAGuard 使多卡下用错设备 | C++ ABI 不匹配报 undefined symbol |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 自定义算子 在“性能优化”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“性能优化：自定义算子”沿相邻边界延伸：op schema 与 fake/meta 注册、流与事件同步、编译期架构选择。比较版本与替代实现的差异（如 torch.library.register_autograd），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [性能优化] 为自定义算子实现反向有哪两种机制，各自适用场景是什么？
2. [性能优化] 为什么 kernel 必须使用当前 stream，如何获取与同步？
3. [性能优化] CUDA 扩展的 C++ ABI 不匹配会以什么形式暴露？

#### 8. 易错点与反例

1. [性能优化] 只写前向不注册反向，autograd 报错或梯度为 None
2. [性能优化] 在默认 stream 上跑 kernel 却从非默认 stream 读数据
3. [性能优化] 忽略 C++ ABI 造成运行时链接失败

#### 9. 实践与验证

warmup 后重复测量取中位数与 p99，单变量改变并用 profiler 归因。具体到 自定义算子：gradcheck 校验自定义 Function 的梯度；torch.testing.assert_close 对比参考实现。把 漏写设备选择/CUDAGuard 使多卡下用错设备 作为负例，记录 torch.autograd.Function 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“自定义算子”的名词，说不出「torch.autograd.gradcheck 通过表示前向/反向数值一致」或 漏写设备选择/CUDAGuard 使多卡下用错设备。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 加速比=t_baseline/t_custom（同 shape/dtype/stream）；occupancy=活跃 warp/最大 warp 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 C++ ABI 不匹配报 undefined symbol 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A058. 性能优化：测试治理

- 难度：高级
- 知识域：K12 测试治理
- 考察能力：输入矩阵、oracle、故障注入与指标
- 题型：测试/实验与验证
- 面试频率：中频
- 前置知识：oracle、证据、日志、回滚、门禁、复盘
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

性能优化必须基于测量而非直觉：先立基线，再定位瓶颈，再验证归因。本题位于 K12 测试治理。flaky 测试可用重跑通过率量化而非直接删除。

#### 2. 问题分析

从“性能优化：测试治理”出发，先固定基线并分解时间/显存，把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 torch.testing.assert_close 的契约，列出两条不变量：「相同 seed+相同算法下结果可复现」「通过阈值必须与 dtype 精度匹配」，再给一个会击穿假设的边界输入，并说明它最先暴露 对 fp16 用 fp32 阈值导致误报 还是 未固定 seed 造成随机失败。

#### 3. 具体答案

面试中可以这样回答：

> 我会给出基线数据、主导瓶颈、优化手段与预期收益，并标注测量误差。在 测试治理 上，关键事实是：torch.testing.assert_close 对 fp32 默认 rtol=1.3e-6、atol=1e-5，对 fp16/bf16 阈值更宽。 判据可写为 |a-b|≤atol+rtol×|b|；flake 率≈失败次数/重跑次数。

#### 4. 技术洞察

整体收益受 Amdahl 定律限制：加速≤1/((1-p)+p/s)，先优化占比最大的部分。在 测试治理 上，量化关系是 |a-b|≤atol+rtol×|b|；flake 率≈失败次数/重跑次数；核心不变量是「相同 seed+相同算法下结果可复现」，它一旦被打破，最先表现为 把 flaky 当稳定失败反复重试掩盖问题。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | torch.testing.assert_close 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | 相同 seed+相同算法下结果可复现 | 违反「通过阈值必须与 dtype 精度匹配」时的报错 |
| 量化关系 | |a-b|≤atol+rtol×|b|；flake 率≈失败次数/重跑次数 | 实测与预测偏差超容差 |
| 失败模式 | 对 fp16 用 fp32 阈值导致误报 | 未固定 seed 造成随机失败 |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 测试治理 在“性能优化”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“性能优化：测试治理”沿相邻边界延伸：golden 值与版本记录、CI matrix 覆盖、门禁与回归集管理。比较版本与替代实现的差异（如 pytest），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [性能优化] assert_close 的 rtol/atol 如何按 dtype 选择才合理？
2. [性能优化] gradcheck 在什么情况下会误报失败？
3. [性能优化] 如何量化并治理 flaky 测试？

#### 8. 易错点与反例

1. [性能优化] 用 == 比较浮点张量
2. [性能优化] 以为关闭 cuDNN benchmark 就能完全确定
3. [性能优化] 把 golden 值硬编码却不记录环境版本

#### 9. 实践与验证

warmup 后重复测量取中位数与 p99，单变量改变并用 profiler 归因。具体到 测试治理：固定 seed 重跑 N 次统计通过率；用 assert_close 显式指定 rtol/atol 而非手写比较。把 对 fp16 用 fp32 阈值导致误报 作为负例，记录 torch.autograd.gradcheck 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“测试治理”的名词，说不出「相同 seed+相同算法下结果可复现」或 对 fp16 用 fp32 阈值导致误报。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 |a-b|≤atol+rtol×|b|；flake 率≈失败次数/重跑次数 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 未固定 seed 造成随机失败 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A059. 兼容迁移：张量/设备

- 难度：高级
- 知识域：K01 张量/设备
- 考察能力：输入矩阵、oracle、故障注入与指标
- 题型：测试/实验与验证
- 面试频率：中频
- 前置知识：shape、stride、dtype、device
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

迁移风险来自隐式契约：默认值、ABI、语义和行为在版本间的变化。本题位于 K01 张量/设备。连续张量满足 stride[i]=∏_{j>i}shape[j]，shape=(2,3,4) 的 stride 恰为 (12,4,1)。

#### 2. 问题分析

从“兼容迁移：张量/设备”出发，先枚举版本差异点（默认值/API/序列化/ABI），把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 torch.Tensor.view/reshape 的契约，列出两条不变量：「view 与源共享 storage（data_ptr 与 storage_offset 一致）」「张量的 device/dtype 决定 kernel 派发与是否发生隐式转换」，再给一个会击穿假设的边界输入，并说明它最先暴露 对非连续张量 view 报 'view size is not compatible with input tensor's size and stride' 还是 跨设备运算报 'Expected all tensors to be on the same device'。

#### 3. 具体答案

面试中可以这样回答：

> 我会给出迁移前后的行为差异、兼容检查项与可回滚的迁移顺序。在 张量/设备 上，关键事实是：view 只改元数据且要求 stride 兼容；reshape 在不兼容时会静默拷贝，可用 t.data_ptr() 判断是否共享存储。 判据可写为 显存(bytes)=numel×itemsize(dtype)；连续 stride[i]=∏_{j>i}shape[j]。

#### 4. 技术洞察

破坏兼容的常是默认值变化而非 API 删除，例如 torch.load 的 weights_only 默认值。在 张量/设备 上，量化关系是 显存(bytes)=numel×itemsize(dtype)；连续 stride[i]=∏_{j>i}shape[j]；核心不变量是「view 与源共享 storage（data_ptr 与 storage_offset 一致）」，它一旦被打破，最先表现为 reshape 静默拷贝抬高峰值显存。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | torch.Tensor.view/reshape 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | view 与源共享 storage（data_ptr 与 storage_offset 一致） | 违反「张量的 device/dtype 决定 kernel 派发与是否发生隐式转换」时的报错 |
| 量化关系 | 显存(bytes)=numel×itemsize(dtype)；连续 stride[i]=∏_{j>i}shape[j] | 实测与预测偏差超容差 |
| 失败模式 | 对非连续张量 view 报 'view size is not compatible with input tensor's size and stride' | 跨设备运算报 'Expected all tensors to be on the same device' |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 张量/设备 在“兼容迁移”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“兼容迁移：张量/设备”沿相邻边界延伸：view/reshape/contiguous、pin_memory 与 stream、dtype 转换与显存。比较版本与替代实现的差异（如 t.to(device, non_blocking=True)），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [兼容迁移] shape=(2,3,4) 分别 view 成 (6,4)、reshape 成 (6,4) 后，stride 与是否共享存储各是什么？
2. [兼容迁移] 为什么 permute 之后必须先 contiguous 才能 view？
3. [兼容迁移] non_blocking=True 在源未 pinned 时的实际行为如何用 profiler 证明？

#### 8. 易错点与反例

1. [兼容迁移] 把 reshape 当零拷贝：stride 不兼容时它会 contiguous 并分配新显存
2. [兼容迁移] 只比较 shape 相等就判断可 view 互转，忽略 stride/storage_offset
3. [兼容迁移] 以为 pin_memory 总有效，实际它只对 non_blocking 有加速意义

#### 9. 实践与验证

在旧/新版本各跑同一测试矩阵，对比输出、报错与性能，沉淀兼容回归集。具体到 张量/设备：构造 (2,3,4) 后断言 t.stride()==(12,4,1) 且 t.is_contiguous()；用 t.data_ptr() 验证 view 与 reshape 的共享/拷贝差异。把 对非连续张量 view 报 'view size is not compatible with input tensor's size and stride' 作为负例，记录 t.stride()/t.is_contiguous() 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“张量/设备”的名词，说不出「view 与源共享 storage（data_ptr 与 storage_offset 一致）」或 对非连续张量 view 报 'view size is not compatible with input tensor's size and stride'。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 显存(bytes)=numel×itemsize(dtype)；连续 stride[i]=∏_{j>i}shape[j] 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 跨设备运算报 'Expected all tensors to be on the same device' 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A060. 兼容迁移：autograd

- 难度：高级
- 知识域：K02 autograd
- 考察能力：输入矩阵、oracle、故障注入与指标
- 题型：测试/实验与验证
- 面试频率：中频
- 前置知识：requires_grad、leaf、view、detach、反向
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

迁移风险来自隐式契约：默认值、ABI、语义和行为在版本间的变化。本题位于 K02 autograd。只有 requires_grad=True 的叶子张量在反向时把梯度累加到 .grad，非叶子的 .grad 默认是 None。

#### 2. 问题分析

从“兼容迁移：autograd”出发，先枚举版本差异点（默认值/API/序列化/ABI），把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 Tensor.requires_grad_ 的契约，列出两条不变量：「叶子张量 .grad 的形状与张量一致且只由叶子持有」「requires_grad 沿 grad_fn 链单向传播」，再给一个会击穿假设的边界输入，并说明它最先暴露 忘记 zero_grad 使梯度累加 还是 对 requires_grad 的叶子做原地操作报 'a leaf Variable that requires grad is being used in an in-place operation'。

#### 3. 具体答案

面试中可以这样回答：

> 我会给出迁移前后的行为差异、兼容检查项与可回滚的迁移顺序。在 autograd 上，关键事实是：backward 默认累加而非覆盖，所以每个 step 前需 zero_grad，否则梯度跨 step 求和。 判据可写为 累加语义 g←g+∂L/∂θ；链式法则 dL/dx=Σ(dL/dy·dy/dx)。

#### 4. 技术洞察

破坏兼容的常是默认值变化而非 API 删除，例如 torch.load 的 weights_only 默认值。在 autograd 上，量化关系是 累加语义 g←g+∂L/∂θ；链式法则 dL/dx=Σ(dL/dy·dy/dx)；核心不变量是「叶子张量 .grad 的形状与张量一致且只由叶子持有」，它一旦被打破，最先表现为 两次 backward 未设 retain_graph 触发 RuntimeError。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | Tensor.requires_grad_ 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | 叶子张量 .grad 的形状与张量一致且只由叶子持有 | 违反「requires_grad 沿 grad_fn 链单向传播」时的报错 |
| 量化关系 | 累加语义 g←g+∂L/∂θ；链式法则 dL/dx=Σ(dL/dy·dy/dx) | 实测与预测偏差超容差 |
| 失败模式 | 忘记 zero_grad 使梯度累加 | 对 requires_grad 的叶子做原地操作报 'a leaf Variable that requires grad is being used in an in-place operation' |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 autograd 在“兼容迁移”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“兼容迁移：autograd”沿相邻边界延伸：detach、retain_grad、in-place 与版本计数、gradcheck。比较版本与替代实现的差异（如 torch.autograd.grad），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [兼容迁移] 为什么非叶子张量的 .grad 默认是 None，retain_grad() 改变了什么？
2. [兼容迁移] detach() 与 with torch.no_grad() 在切断梯度上的差别是什么？
3. [兼容迁移] in-place 操作在什么条件下能安全使用而不破坏 autograd？

#### 8. 易错点与反例

1. [兼容迁移] 认为非叶子 .grad 会自动填充，实则默认 None，需要 retain_grad()
2. [兼容迁移] 在 no_grad 下构建输入再 backward，得到 requires_grad 缺失错误
3. [兼容迁移] 用 param = param - lr*g 原地更新破坏计算图

#### 9. 实践与验证

在旧/新版本各跑同一测试矩阵，对比输出、报错与性能，沉淀兼容回归集。具体到 autograd：断言 x.is_leaf and x.grad is None 后再 backward；用 torch.autograd.grad(..., retain_graph=True) 做两次反向对比。把 忘记 zero_grad 使梯度累加 作为负例，记录 Tensor.detach() 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“autograd”的名词，说不出「叶子张量 .grad 的形状与张量一致且只由叶子持有」或 忘记 zero_grad 使梯度累加。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 累加语义 g←g+∂L/∂θ；链式法则 dL/dx=Σ(dL/dy·dy/dx) 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 对 requires_grad 的叶子做原地操作报 'a leaf Variable that requires grad is being used in an in-place operation' 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A061. 兼容迁移：Module/状态

- 难度：高级
- 知识域：K03 Module/状态
- 考察能力：现象分层、假设树、止损、根因与复发防护
- 题型：故障排查与事故复盘
- 面试频率：中频
- 前置知识：参数、buffer、模式和 state_dict
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

迁移风险来自隐式契约：默认值、ABI、语义和行为在版本间的变化。本题位于 K03 Module/状态。buffer 参与 state_dict 但不是参数、不参与梯度；Parameter 两者都占。

#### 2. 问题分析

从“兼容迁移：Module/状态”出发，先枚举版本差异点（默认值/API/序列化/ABI），把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 register_buffer 的契约，列出两条不变量：「同一 Module 的 state_dict key 集合确定且与设备无关」「buffer 的 dtype/device 跟随 module 或按注册时指定」，再给一个会击穿假设的边界输入，并说明它最先暴露 自定义 buffer 未注册，.to(device) 后仍留在 CPU 还是 load_state_dict 因 missing/unexpected keys 失败。

#### 3. 具体答案

面试中可以这样回答：

> 我会给出迁移前后的行为差异、兼容检查项与可回滚的迁移顺序。在 Module/状态 上，关键事实是：load_state_dict 默认 strict=True，缺失或多出 key 都会报错。 判据可写为 state_dict={k:param}∪{k:buffer}；训练态显存≈Σ numel×itemsize×(1+grad+opt_state)。

#### 4. 技术洞察

破坏兼容的常是默认值变化而非 API 删除，例如 torch.load 的 weights_only 默认值。在 Module/状态 上，量化关系是 state_dict={k:param}∪{k:buffer}；训练态显存≈Σ numel×itemsize×(1+grad+opt_state)；核心不变量是「同一 Module 的 state_dict key 集合确定且与设备无关」，它一旦被打破，最先表现为 eval 下忘记切换使 Dropout 仍在抽样。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | register_buffer 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | 同一 Module 的 state_dict key 集合确定且与设备无关 | 违反「buffer 的 dtype/device 跟随 module 或按注册时指定」时的报错 |
| 量化关系 | state_dict={k:param}∪{k:buffer}；训练态显存≈Σ numel×itemsize×(1+grad+opt_state) | 实测与预测偏差超容差 |
| 失败模式 | 自定义 buffer 未注册，.to(device) 后仍留在 CPU | load_state_dict 因 missing/unexpected keys 失败 |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 Module/状态 在“兼容迁移”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“兼容迁移：Module/状态”沿相邻边界延伸：persistent buffer、hook 与状态迁移、DDP 下的 buffer 同步。比较版本与替代实现的差异（如 state_dict/load_state_dict），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [兼容迁移] buffer 与 Parameter 在 state_dict、梯度和设备迁移上的差异是什么？
2. [兼容迁移] BatchNorm 在 eval 模式仍更新 running stats 需要什么额外设置？
3. [兼容迁移] register_buffer(persistent=False) 会从 state_dict 中移除什么？

#### 8. 易错点与反例

1. [兼容迁移] 把计数/统计量存成普通属性，导致 state_dict 丢失该状态
2. [兼容迁移] load_state_dict 后忘记 module.eval()，使 BN/Dropout 行为不符
3. [兼容迁移] 直接给同名属性赋值绕过注册，使 named_parameters 缺失

#### 9. 实践与验证

在旧/新版本各跑同一测试矩阵，对比输出、报错与性能，沉淀兼容回归集。具体到 Module/状态：断言 set(m.state_dict().keys()) 与 checkpoint 完全一致；对比 train/eval 两次前向输出验证 Dropout 关闭。把 自定义 buffer 未注册，.to(device) 后仍留在 CPU 作为负例，记录 register_parameter 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“Module/状态”的名词，说不出「同一 Module 的 state_dict key 集合确定且与设备无关」或 自定义 buffer 未注册，.to(device) 后仍留在 CPU。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 state_dict={k:param}∪{k:buffer}；训练态显存≈Σ numel×itemsize×(1+grad+opt_state) 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 load_state_dict 因 missing/unexpected keys 失败 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A062. 兼容迁移：数据管道

- 难度：高级
- 知识域：K04 数据管道
- 考察能力：现象分层、假设树、止损、根因与复发防护
- 题型：故障排查与事故复盘
- 面试频率：中频
- 前置知识：Dataset、Sampler、collate、padding、版本
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

迁移风险来自隐式契约：默认值、ABI、语义和行为在版本间的变化。本题位于 K04 数据管道。num_workers=0 在主进程取数，>0 时 fork 子进程，每个 worker 有独立 Python 状态与 seed。

#### 2. 问题分析

从“兼容迁移：数据管道”出发，先枚举版本差异点（默认值/API/序列化/ABI），把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 DataLoader(num_workers, pin_memory, persistent_workers) 的契约，列出两条不变量：「shuffle=False 且种子固定时迭代顺序可复现」「各 rank 采样集合互斥且并集覆盖全集（或按 drop_last 截断）」，再给一个会击穿假设的边界输入，并说明它最先暴露 多 worker 共享同一随机源导致样本重复 还是 worker 数过多耗尽文件描述符或内存。

#### 3. 具体答案

面试中可以这样回答：

> 我会给出迁移前后的行为差异、兼容检查项与可回滚的迁移顺序。在 数据管道 上，关键事实是：DistributedSampler 按 rank 切分样本，样本数不能被 world_size 整除时默认补齐以避免各 rank 步数不同。 判据可写为 吞吐≤min(存储带宽, CPU 解码速率, H2D 带宽)；有效样本/步=batch×world_size。

#### 4. 技术洞察

破坏兼容的常是默认值变化而非 API 删除，例如 torch.load 的 weights_only 默认值。在 数据管道 上，量化关系是 吞吐≤min(存储带宽, CPU 解码速率, H2D 带宽)；有效样本/步=batch×world_size；核心不变量是「shuffle=False 且种子固定时迭代顺序可复现」，它一旦被打破，最先表现为 DistributedSampler 与自定义 shuffle 双打乱造成跨 rank 重复。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | DataLoader(num_workers, pin_memory, persistent_workers) 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | shuffle=False 且种子固定时迭代顺序可复现 | 违反「各 rank 采样集合互斥且并集覆盖全集（或按 drop_last 截断）」时的报错 |
| 量化关系 | 吞吐≤min(存储带宽, CPU 解码速率, H2D 带宽)；有效样本/步=batch×world_size | 实测与预测偏差超容差 |
| 失败模式 | 多 worker 共享同一随机源导致样本重复 | worker 数过多耗尽文件描述符或内存 |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 数据管道 在“兼容迁移”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“兼容迁移：数据管道”沿相邻边界延伸：IterableDataset、chunk 预取、对象存储吞吐与 IPC 开销。比较版本与替代实现的差异（如 collate_fn），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [兼容迁移] DistributedSampler 如何处理样本数不被 world_size 整除，是否引入重复样本？
2. [兼容迁移] num_workers 增大时首步延迟、内存与 IPC 开销如何变化？
3. [兼容迁移] pin_memory 与 non_blocking 为何必须同时开启才有效？

#### 8. 易错点与反例

1. [兼容迁移] 把 num_workers 调大就以为更快，忽略 IPC 与内存放大
2. [兼容迁移] Dataset 里用全局随机而不经 worker_init_fn 重播种
3. [兼容迁移] 自己 shuffle 又用 DistributedSampler，导致跨 rank 样本重复

#### 9. 实践与验证

在旧/新版本各跑同一测试矩阵，对比输出、报错与性能，沉淀兼容回归集。具体到 数据管道：固定 seed 两次迭代断言顺序一致；统计各 rank 采样索引并集是否覆盖全集。把 多 worker 共享同一随机源导致样本重复 作为负例，记录 DistributedSampler 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“数据管道”的名词，说不出「shuffle=False 且种子固定时迭代顺序可复现」或 多 worker 共享同一随机源导致样本重复。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 吞吐≤min(存储带宽, CPU 解码速率, H2D 带宽)；有效样本/步=batch×world_size 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 worker 数过多耗尽文件描述符或内存 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A063. 兼容迁移：训练/优化器

- 难度：高级
- 知识域：K05 训练/优化器
- 考察能力：现象分层、假设树、止损、根因与复发防护
- 题型：故障排查与事故复盘
- 面试频率：中频
- 前置知识：forward、backward、累积、optimizer、scheduler
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

迁移风险来自隐式契约：默认值、ABI、语义和行为在版本间的变化。本题位于 K05 训练/优化器。set_to_none=True 把 .grad 置 None 而非填零，省显存并让后续加法重新分配。

#### 2. 问题分析

从“兼容迁移：训练/优化器”出发，先枚举版本差异点（默认值/API/序列化/ABI），把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 optimizer.zero_grad(set_to_none=True) 的契约，列出两条不变量：「反向之前梯度应为空或已清零」「scheduler 与 optimizer 的 step 次数对齐」，再给一个会击穿假设的边界输入，并说明它最先暴露 scheduler.step() 放错位置使 lr 序列整体偏移 还是 累积时直接累加 loss 未归一化。

#### 3. 具体答案

面试中可以这样回答：

> 我会给出迁移前后的行为差异、兼容检查项与可回滚的迁移顺序。在 训练/优化器 上，关键事实是：PyTorch 1.1.0 起 lr_scheduler.step() 应在 optimizer.step() 之后调用；顺序错误只给 warning 并产生错误 lr。 判据可写为 有效 batch=micro_batch×grad_accum×world_size；Adam 状态≈2P(状态)+P(梯度)。

#### 4. 技术洞察

破坏兼容的常是默认值变化而非 API 删除，例如 torch.load 的 weights_only 默认值。在 训练/优化器 上，量化关系是 有效 batch=micro_batch×grad_accum×world_size；Adam 状态≈2P(状态)+P(梯度)；核心不变量是「反向之前梯度应为空或已清零」，它一旦被打破，最先表现为 zero_grad 漏调导致梯度累加。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | optimizer.zero_grad(set_to_none=True) 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | 反向之前梯度应为空或已清零 | 违反「scheduler 与 optimizer 的 step 次数对齐」时的报错 |
| 量化关系 | 有效 batch=micro_batch×grad_accum×world_size；Adam 状态≈2P(状态)+P(梯度) | 实测与预测偏差超容差 |
| 失败模式 | scheduler.step() 放错位置使 lr 序列整体偏移 | 累积时直接累加 loss 未归一化 |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 训练/优化器 在“兼容迁移”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“兼容迁移：训练/优化器”沿相邻边界延伸：scheduler 恢复、优化器 state_dict、foreach/fused 实现差异。比较版本与替代实现的差异（如 lr_scheduler.step()），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [兼容迁移] lr_scheduler.step() 在 PyTorch 1.1.0 前后位置的语义差异是什么？
2. [兼容迁移] 梯度累积如何在数学上等价于大 batch，需要哪些归一化？
3. [兼容迁移] set_to_none=True 对显存与优化器行为有何具体影响？

#### 8. 易错点与反例

1. [兼容迁移] 用 zero_grad() 不传 set_to_none，长期分配开销更高
2. [兼容迁移] 把 scheduler.step() 在 epoch 与 step 间混用，导致 lr 跳变
3. [兼容迁移] 梯度累积时未除 steps，使等效学习率偏大

#### 9. 实践与验证

在旧/新版本各跑同一测试矩阵，对比输出、报错与性能，沉淀兼容回归集。具体到 训练/优化器：打印前若干步 param_groups[0]['lr'] 验证调度曲线；比较累积 N 步与单步大 batch 的梯度 L2 范数。把 scheduler.step() 放错位置使 lr 序列整体偏移 作为负例，记录 optimizer.step() 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“训练/优化器”的名词，说不出「反向之前梯度应为空或已清零」或 scheduler.step() 放错位置使 lr 序列整体偏移。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 有效 batch=micro_batch×grad_accum×world_size；Adam 状态≈2P(状态)+P(梯度) 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 累积时直接累加 loss 未归一化 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A064. 兼容迁移：AMP/恢复

- 难度：高级
- 知识域：K06 AMP/恢复
- 考察能力：现象分层、假设树、止损、根因与复发防护
- 题型：故障排查与事故复盘
- 面试频率：中频
- 前置知识：autocast、scaler、快照、随机状态
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

迁移风险来自隐式契约：默认值、ABI、语义和行为在版本间的变化。本题位于 K06 AMP/恢复。GradScaler 默认 init_scale=65536、growth_factor=2.0、backoff_factor=0.5、growth_interval=2000。

#### 2. 问题分析

从“兼容迁移：AMP/恢复”出发，先枚举版本差异点（默认值/API/序列化/ABI），把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 torch.autocast(device_type) 的契约，列出两条不变量：「反向前 loss 被 scale 放大，step 前梯度必须被 unscale」「inf/nan 检查与 skip 分支成对出现」，再给一个会击穿假设的边界输入，并说明它最先暴露 clip 放在 unscale 之前使梯度幅度错误 还是 对 bf16 仍用 GradScaler 造成无谓开销。

#### 3. 具体答案

面试中可以这样回答：

> 我会给出迁移前后的行为差异、兼容检查项与可回滚的迁移顺序。在 AMP/恢复 上，关键事实是：出现 inf/nan 时 scaler.step 跳过本次更新并降低 scale，避免污染权重。 判据可写为 scale_{new}=scale×growth_factor（连续无溢出）或 scale/backoff_factor（检测到 inf/nan）。

#### 4. 技术洞察

破坏兼容的常是默认值变化而非 API 删除，例如 torch.load 的 weights_only 默认值。在 AMP/恢复 上，量化关系是 scale_{new}=scale×growth_factor（连续无溢出）或 scale/backoff_factor（检测到 inf/nan）；核心不变量是「反向前 loss 被 scale 放大，step 前梯度必须被 unscale」，它一旦被打破，最先表现为 checkpoint 未保存 scaler.state_dict 使恢复后 scale 不连续。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | torch.autocast(device_type) 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | 反向前 loss 被 scale 放大，step 前梯度必须被 unscale | 违反「inf/nan 检查与 skip 分支成对出现」时的报错 |
| 量化关系 | scale_{new}=scale×growth_factor（连续无溢出）或 scale/backoff_factor（检测到 inf/nan） | 实测与预测偏差超容差 |
| 失败模式 | clip 放在 unscale 之前使梯度幅度错误 | 对 bf16 仍用 GradScaler 造成无谓开销 |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 AMP/恢复 在“兼容迁移”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“兼容迁移：AMP/恢复”沿相邻边界延伸：autocast 的 op 白/黑名单、随机状态快照、断点续训一致性。比较版本与替代实现的差异（如 scaler.scale/step/update），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [兼容迁移] GradScaler 连续多少次无溢出才提高 scale，默认参数是多少？
2. [兼容迁移] 为什么 fp16 autocast 必须 unscale 之后才能裁剪梯度？
3. [兼容迁移] bf16 为什么通常不需要 GradScaler？

#### 8. 易错点与反例

1. [兼容迁移] 以为 autocast 会把参数也转 fp16（参数保持 fp32）
2. [兼容迁移] 用未 unscale 的梯度直接裁剪
3. [兼容迁移] 恢复训练时忘记保存/加载 scaler 状态

#### 9. 实践与验证

在旧/新版本各跑同一测试矩阵，对比输出、报错与性能，沉淀兼容回归集。具体到 AMP/恢复：记录 scaler.get_scale() 在若干 step 的轨迹；注入含 nan 的 loss 验证该 step 被跳过。把 clip 放在 unscale 之前使梯度幅度错误 作为负例，记录 GradScaler 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“AMP/恢复”的名词，说不出「反向前 loss 被 scale 放大，step 前梯度必须被 unscale」或 clip 放在 unscale 之前使梯度幅度错误。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 scale_{new}=scale×growth_factor（连续无溢出）或 scale/backoff_factor（检测到 inf/nan） 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 对 bf16 仍用 GradScaler 造成无谓开销 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A065. 兼容迁移：分布式

- 难度：高级
- 知识域：K07 分布式
- 考察能力：现象分层、假设树、止损、根因与复发防护
- 题型：故障排查与事故复盘
- 面试频率：中频
- 前置知识：rank、DDP、FSDP、collective、容错
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

迁移风险来自隐式契约：默认值、ABI、语义和行为在版本间的变化。本题位于 K07 分布式。DDP 在反向中把梯度按 bucket（默认 bucket_cap_mb=25）allreduce 并与反向计算重叠。

#### 2. 问题分析

从“兼容迁移：分布式”出发，先枚举版本差异点（默认值/API/序列化/ABI），把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 init_process_group 的契约，列出两条不变量：「所有 rank 的 rank/world_size 一致」「collective 调用序列在各 rank 严格一致」，再给一个会击穿假设的边界输入，并说明它最先暴露 某些 rank 提前 return 使 collective 不匹配而 hang 还是 梯度 size 不一致触发 'Mismatch in collective'。

#### 3. 具体答案

面试中可以这样回答：

> 我会给出迁移前后的行为差异、兼容检查项与可回滚的迁移顺序。在 分布式 上，关键事实是：FSDP 把参数、梯度、优化器状态分片到各 rank：前向 all_gather 参数、反向 reduce_scatter 梯度。 判据可写为 ring allreduce≈2(N-1)/N×M；DDP 每卡显存≈params+grads+opt_states。

#### 4. 技术洞察

破坏兼容的常是默认值变化而非 API 删除，例如 torch.load 的 weights_only 默认值。在 分布式 上，量化关系是 ring allreduce≈2(N-1)/N×M；DDP 每卡显存≈params+grads+opt_states；核心不变量是「所有 rank 的 rank/world_size 一致」，它一旦被打破，最先表现为 FSDP 未 wrap 子模块使参数无法分片。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | init_process_group 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | 所有 rank 的 rank/world_size 一致 | 违反「collective 调用序列在各 rank 严格一致」时的报错 |
| 量化关系 | ring allreduce≈2(N-1)/N×M；DDP 每卡显存≈params+grads+opt_states | 实测与预测偏差超容差 |
| 失败模式 | 某些 rank 提前 return 使 collective 不匹配而 hang | 梯度 size 不一致触发 'Mismatch in collective' |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 分布式 在“兼容迁移”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“兼容迁移：分布式”沿相邻边界延伸：process group 切分、混合并行、NCCL 拓扑与超时。比较版本与替代实现的差异（如 FullyShardedDataParallel），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [兼容迁移] DDP 的梯度 bucketing 如何影响通信与计算重叠？
2. [兼容迁移] FSDP 的 all_gather 与 reduce_scatter 分别在前向/反向何处发生？
3. [兼容迁移] collective 调用顺序不一致时表现为 hang 还是报错，原因是什么？

#### 8. 易错点与反例

1. [兼容迁移] 对非参数 buffer 手动 allreduce，造成重复同步
2. [兼容迁移] 认为 FSDP 降低单卡计算量（实际只降显存）
3. [兼容迁移] 忽略 bucket_cap_mb 对重叠与峰值显存的影响

#### 9. 实践与验证

在旧/新版本各跑同一测试矩阵，对比输出、报错与性能，沉淀兼容回归集。具体到 分布式：barrier 后断言 get_rank()/get_world_size()；对比 DDP 与 FSDP 的 max_memory_allocated。把 某些 rank 提前 return 使 collective 不匹配而 hang 作为负例，记录 DistributedDataParallel 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“分布式”的名词，说不出「所有 rank 的 rank/world_size 一致」或 某些 rank 提前 return 使 collective 不匹配而 hang。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 ring allreduce≈2(N-1)/N×M；DDP 每卡显存≈params+grads+opt_states 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 梯度 size 不一致触发 'Mismatch in collective' 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A066. 兼容迁移：编译栈

- 难度：高级
- 知识域：K08 编译栈
- 考察能力：现象分层、假设树、止损、根因与复发防护
- 题型：故障排查与事故复盘
- 面试频率：中频
- 前置知识：Dynamo、AOTAutograd、Inductor、guard、缓存
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

迁移风险来自隐式契约：默认值、ABI、语义和行为在版本间的变化。本题位于 K08 编译栈。Dynamo 捕获 Python 字节码生成 FX 图，对类型/形状/值建立 guard；guard 失效即重编译。

#### 2. 问题分析

从“兼容迁移：编译栈”出发，先枚举版本差异点（默认值/API/序列化/ABI），把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 torch.compile 的契约，列出两条不变量：「fullgraph=True 时任何 graph break 都抛错」「同一 guard 命中后不再触发编译」，再给一个会击穿假设的边界输入，并说明它最先暴露 动态 shape 未标记导致反复重编译 还是 数据相关控制流触发 graph break。

#### 3. 具体答案

面试中可以这样回答：

> 我会给出迁移前后的行为差异、兼容检查项与可回滚的迁移顺序。在 编译栈 上，关键事实是：graph break 把图切段回退 eager，破坏端到端融合。 判据可写为 编译缓存以 guard 值集合为键；重编译次数∝出现过的不同 guard 组合数。

#### 4. 技术洞察

破坏兼容的常是默认值变化而非 API 删除，例如 torch.load 的 weights_only 默认值。在 编译栈 上，量化关系是 编译缓存以 guard 值集合为键；重编译次数∝出现过的不同 guard 组合数；核心不变量是「fullgraph=True 时任何 graph break 都抛错」，它一旦被打破，最先表现为 CUDA Graph 因输入地址变化捕获失败。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | torch.compile 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | fullgraph=True 时任何 graph break 都抛错 | 违反「同一 guard 命中后不再触发编译」时的报错 |
| 量化关系 | 编译缓存以 guard 值集合为键；重编译次数∝出现过的不同 guard 组合数 | 实测与预测偏差超容差 |
| 失败模式 | 动态 shape 未标记导致反复重编译 | 数据相关控制流触发 graph break |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 编译栈 在“兼容迁移”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“兼容迁移：编译栈”沿相邻边界延伸：dynamic shape 的 Dim/mark_dynamic、guard 导出、AOT 与自定义 op。比较版本与替代实现的差异（如 TORCH_LOGS=graph_breaks,recompiles），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [兼容迁移] Dynamo 的 guard 在什么条件下失效并触发重编译？
2. [兼容迁移] graph break 对 autograd 图切分与状态副作用带来什么风险？
3. [兼容迁移] reduce-overhead 的 CUDA Graph 为何要求输入地址固定？

#### 8. 易错点与反例

1. [兼容迁移] 以为 graph break 只影响性能，忽略断点两侧的状态副作用
2. [兼容迁移] 用固定 shape 基准掩盖动态 shape 的重编译
3. [兼容迁移] 忽略首次编译冷启动开销

#### 9. 实践与验证

在旧/新版本各跑同一测试矩阵，对比输出、报错与性能，沉淀兼容回归集。具体到 编译栈：用 torch._dynamo.explain 统计 graph_breaks 数量；监控 counters['inductor']['recompiles']。把 动态 shape 未标记导致反复重编译 作为负例，记录 torch._dynamo.explain 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“编译栈”的名词，说不出「fullgraph=True 时任何 graph break 都抛错」或 动态 shape 未标记导致反复重编译。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 编译缓存以 guard 值集合为键；重编译次数∝出现过的不同 guard 组合数 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 数据相关控制流触发 graph break 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A067. 兼容迁移：性能

- 难度：高级
- 知识域：K09 性能
- 考察能力：现象分层、假设树、止损、根因与复发防护
- 题型：故障排查与事故复盘
- 面试频率：中频
- 前置知识：关键路径、带宽、算力、通信、显存、统计
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

迁移风险来自隐式契约：默认值、ABI、语义和行为在版本间的变化。本题位于 K09 性能。roofline 下 kernel 时间≈max(FLOPs/峰值算力, 访存字节/峰值带宽)，取决于算术强度。

#### 2. 问题分析

从“兼容迁移：性能”出发，先枚举版本差异点（默认值/API/序列化/ABI），把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 torch.profiler 的契约，列出两条不变量：「所有 kernel 时间之和≤墙钟时间，差额为同步/空泡」「峰值显存≥活跃张量之和+碎片+缓存块」，再给一个会击穿假设的边界输入，并说明它最先暴露 未 sync 就读 Event 时间 还是 只测稳态忽略首次冷启动。

#### 3. 具体答案

面试中可以这样回答：

> 我会给出迁移前后的行为差异、兼容检查项与可回滚的迁移顺序。在 性能 上，关键事实是：GPU 计时必须 warmup 并 torch.cuda.synchronize()，否则测到的是入队时间。 判据可写为 t≈max(FLOPs/FLOPS_peak, Bytes/BW_peak)；算术强度=FLOPs/Bytes。

#### 4. 技术洞察

破坏兼容的常是默认值变化而非 API 删除，例如 torch.load 的 weights_only 默认值。在 性能 上，量化关系是 t≈max(FLOPs/FLOPS_peak, Bytes/BW_peak)；算术强度=FLOPs/Bytes；核心不变量是「所有 kernel 时间之和≤墙钟时间，差额为同步/空泡」，它一旦被打破，最先表现为 把 H2D 拷贝算进 GPU 计算时间。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | torch.profiler 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | 所有 kernel 时间之和≤墙钟时间，差额为同步/空泡 | 违反「峰值显存≥活跃张量之和+碎片+缓存块」时的报错 |
| 量化关系 | t≈max(FLOPs/FLOPS_peak, Bytes/BW_peak)；算术强度=FLOPs/Bytes | 实测与预测偏差超容差 |
| 失败模式 | 未 sync 就读 Event 时间 | 只测稳态忽略首次冷启动 |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 性能 在“兼容迁移”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“兼容迁移：性能”沿相邻边界延伸：roofline 拐点、显存碎片与快照、通信/计算重叠度量。比较版本与替代实现的差异（如 torch.cuda.max_memory_allocated），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [兼容迁移] 如何用 roofline 判断算子 compute-bound 还是 memory-bound，需要哪些输入？
2. [兼容迁移] 为什么 profiler 的 self_cuda_time 之和常小于 step time？
3. [兼容迁移] 在通信与计算未重叠时，先改数据并行还是分片/bucket 策略？

#### 8. 易错点与反例

1. [兼容迁移] 用 CPU 计时测 GPU kernel，得到错误结论
2. [兼容迁移] 忽略通信与 sync 造成的时间空泡
3. [兼容迁移] 以单次测量下结论，被启动噪声误导

#### 9. 实践与验证

在旧/新版本各跑同一测试矩阵，对比输出、报错与性能，沉淀兼容回归集。具体到 性能：profiler 导出 kernel 表按 cuda_time 排序定位热点；固定输入重复 N 次取中位数与 p99。把 未 sync 就读 Event 时间 作为负例，记录 torch.cuda.Event 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“性能”的名词，说不出「所有 kernel 时间之和≤墙钟时间，差额为同步/空泡」或 未 sync 就读 Event 时间。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 t≈max(FLOPs/FLOPS_peak, Bytes/BW_peak)；算术强度=FLOPs/Bytes 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 只测稳态忽略首次冷启动 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A068. 兼容迁移：导出部署

- 难度：高级
- 知识域：K10 导出部署
- 考察能力：现象分层、假设树、止损、根因与复发防护
- 题型：故障排查与事故复盘
- 面试频率：中频
- 前置知识：state_dict、Export、runtime、schema、迁移
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

迁移风险来自隐式契约：默认值、ABI、语义和行为在版本间的变化。本题位于 K10 序列化/导出/部署。PyTorch 2.6 起 torch.load 的 weights_only 默认为 True，加载含自定义类的 pickle 需显式关闭。

#### 2. 问题分析

从“兼容迁移：序列化/导出/部署”出发，先枚举版本差异点（默认值/API/序列化/ABI），把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 torch.save/torch.load(weights_only=) 的契约，列出两条不变量：「导出后的图不含 Python 语义，仅含 ATen/custom op 调用」「state_dict 的 key 语义与训练侧一致」，再给一个会击穿假设的边界输入，并说明它最先暴露 trace 把数据相关分支固化为单一路径 还是 自定义 op 未注册导致导出失败。

#### 3. 具体答案

面试中可以这样回答：

> 我会给出迁移前后的行为差异、兼容检查项与可回滚的迁移顺序。在 序列化/导出/部署 上，关键事实是：torch.jit.trace 记录一次执行路径，忽略数据相关控制流（if/for 会塌缩）。 判据可写为 导出图=静态化 Python 控制流+固定/声明化 shape（Dim 约束）。

#### 4. 技术洞察

破坏兼容的常是默认值变化而非 API 删除，例如 torch.load 的 weights_only 默认值。在 序列化/导出/部署 上，量化关系是 导出图=静态化 Python 控制流+固定/声明化 shape（Dim 约束）；核心不变量是「导出后的图不含 Python 语义，仅含 ATen/custom op 调用」，它一旦被打破，最先表现为 weights_only=True 加载旧 pickle 报错。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | torch.save/torch.load(weights_only=) 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | 导出后的图不含 Python 语义，仅含 ATen/custom op 调用 | 违反「state_dict 的 key 语义与训练侧一致」时的报错 |
| 量化关系 | 导出图=静态化 Python 控制流+固定/声明化 shape（Dim 约束） | 实测与预测偏差超容差 |
| 失败模式 | trace 把数据相关分支固化为单一路径 | 自定义 op 未注册导致导出失败 |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 序列化/导出/部署 在“兼容迁移”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“兼容迁移：序列化/导出/部署”沿相邻边界延伸：custom op 的 register_autograd、ONNX/ExecuTorch runtime、版本 schema 演进。比较版本与替代实现的差异（如 torch.export.export），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [兼容迁移] torch.jit.trace 与 torch.export 在控制流处理上有什么本质区别？
2. [兼容迁移] weights_only 自 2.6 起默认为 True 带来哪些兼容性影响？
3. [兼容迁移] 如何用 dynamic_shapes 表达一个仅序列维动态的模型并验证？

#### 8. 易错点与反例

1. [兼容迁移] 用 trace 导出含 if/loop 的模型，使数据相关分支被固化
2. [兼容迁移] 认为 weights_only=True 永远安全（自定义 op/类仍需注册）
3. [兼容迁移] 忽略导出后 dtype/device 与运行时不一致

#### 9. 实践与验证

在旧/新版本各跑同一测试矩阵，对比输出、报错与性能，沉淀兼容回归集。具体到 序列化/导出/部署：对比导出前后在相同输入上的输出 assert_close；检查 ExportedProgram.graph_module.graph 的 op 集合。把 trace 把数据相关分支固化为单一路径 作为负例，记录 torch.jit.trace/script 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“序列化/导出/部署”的名词，说不出「导出后的图不含 Python 语义，仅含 ATen/custom op 调用」或 trace 把数据相关分支固化为单一路径。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 导出图=静态化 Python 控制流+固定/声明化 shape（Dim 约束） 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 自定义 op 未注册导致导出失败 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A069. 兼容迁移：自定义算子

- 难度：高级
- 知识域：K11 自定义算子
- 考察能力：现象分层、假设树、止损、根因与复发防护
- 题型：故障排查与事故复盘
- 面试频率：中频
- 前置知识：schema、dispatcher、stream、autograd、ABI
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

迁移风险来自隐式契约：默认值、ABI、语义和行为在版本间的变化。本题位于 K11 自定义算子。自定义算子要让 autograd 可用，必须为 Tensor 参数注册反向（Function 或 register_autograd）。

#### 2. 问题分析

从“兼容迁移：自定义算子”出发，先枚举版本差异点（默认值/API/序列化/ABI），把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 torch.library/TORCH_LIBRARY 的契约，列出两条不变量：「torch.autograd.gradcheck 通过表示前向/反向数值一致」「no_grad 与 grad 模式除梯度外结果一致」，再给一个会击穿假设的边界输入，并说明它最先暴露 漏写设备选择/CUDAGuard 使多卡下用错设备 还是 C++ ABI 不匹配报 undefined symbol。

#### 3. 具体答案

面试中可以这样回答：

> 我会给出迁移前后的行为差异、兼容检查项与可回滚的迁移顺序。在 自定义算子 上，关键事实是：kernel 必须在当前 CUDA stream 上执行并保证输入/输出的 stream 语义，否则与其它算子乱序。 判据可写为 加速比=t_baseline/t_custom（同 shape/dtype/stream）；occupancy=活跃 warp/最大 warp。

#### 4. 技术洞察

破坏兼容的常是默认值变化而非 API 删除，例如 torch.load 的 weights_only 默认值。在 自定义算子 上，量化关系是 加速比=t_baseline/t_custom（同 shape/dtype/stream）；occupancy=活跃 warp/最大 warp；核心不变量是「torch.autograd.gradcheck 通过表示前向/反向数值一致」，它一旦被打破，最先表现为 内核用原子加导致非确定性。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | torch.library/TORCH_LIBRARY 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | torch.autograd.gradcheck 通过表示前向/反向数值一致 | 违反「no_grad 与 grad 模式除梯度外结果一致」时的报错 |
| 量化关系 | 加速比=t_baseline/t_custom（同 shape/dtype/stream）；occupancy=活跃 warp/最大 warp | 实测与预测偏差超容差 |
| 失败模式 | 漏写设备选择/CUDAGuard 使多卡下用错设备 | C++ ABI 不匹配报 undefined symbol |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 自定义算子 在“兼容迁移”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“兼容迁移：自定义算子”沿相邻边界延伸：op schema 与 fake/meta 注册、流与事件同步、编译期架构选择。比较版本与替代实现的差异（如 torch.library.register_autograd），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [兼容迁移] 为自定义算子实现反向有哪两种机制，各自适用场景是什么？
2. [兼容迁移] 为什么 kernel 必须使用当前 stream，如何获取与同步？
3. [兼容迁移] CUDA 扩展的 C++ ABI 不匹配会以什么形式暴露？

#### 8. 易错点与反例

1. [兼容迁移] 只写前向不注册反向，autograd 报错或梯度为 None
2. [兼容迁移] 在默认 stream 上跑 kernel 却从非默认 stream 读数据
3. [兼容迁移] 忽略 C++ ABI 造成运行时链接失败

#### 9. 实践与验证

在旧/新版本各跑同一测试矩阵，对比输出、报错与性能，沉淀兼容回归集。具体到 自定义算子：gradcheck 校验自定义 Function 的梯度；torch.testing.assert_close 对比参考实现。把 漏写设备选择/CUDAGuard 使多卡下用错设备 作为负例，记录 torch.autograd.Function 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“自定义算子”的名词，说不出「torch.autograd.gradcheck 通过表示前向/反向数值一致」或 漏写设备选择/CUDAGuard 使多卡下用错设备。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 加速比=t_baseline/t_custom（同 shape/dtype/stream）；occupancy=活跃 warp/最大 warp 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 C++ ABI 不匹配报 undefined symbol 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A070. 兼容迁移：测试治理

- 难度：高级
- 知识域：K12 测试治理
- 考察能力：现象分层、假设树、止损、根因与复发防护
- 题型：故障排查与事故复盘
- 面试频率：中频
- 前置知识：oracle、证据、日志、回滚、门禁、复盘
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

迁移风险来自隐式契约：默认值、ABI、语义和行为在版本间的变化。本题位于 K12 测试治理。torch.testing.assert_close 对 fp32 默认 rtol=1.3e-6、atol=1e-5，对 fp16/bf16 阈值更宽。

#### 2. 问题分析

从“兼容迁移：测试治理”出发，先枚举版本差异点（默认值/API/序列化/ABI），把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 torch.testing.assert_close 的契约，列出两条不变量：「相同 seed+相同算法下结果可复现」「通过阈值必须与 dtype 精度匹配」，再给一个会击穿假设的边界输入，并说明它最先暴露 对 fp16 用 fp32 阈值导致误报 还是 未固定 seed 造成随机失败。

#### 3. 具体答案

面试中可以这样回答：

> 我会给出迁移前后的行为差异、兼容检查项与可回滚的迁移顺序。在 测试治理 上，关键事实是：gradcheck 用有限差分对比解析梯度，默认 eps=1e-6、atol=1e-5。 判据可写为 |a-b|≤atol+rtol×|b|；flake 率≈失败次数/重跑次数。

#### 4. 技术洞察

破坏兼容的常是默认值变化而非 API 删除，例如 torch.load 的 weights_only 默认值。在 测试治理 上，量化关系是 |a-b|≤atol+rtol×|b|；flake 率≈失败次数/重跑次数；核心不变量是「相同 seed+相同算法下结果可复现」，它一旦被打破，最先表现为 把 flaky 当稳定失败反复重试掩盖问题。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | torch.testing.assert_close 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | 相同 seed+相同算法下结果可复现 | 违反「通过阈值必须与 dtype 精度匹配」时的报错 |
| 量化关系 | |a-b|≤atol+rtol×|b|；flake 率≈失败次数/重跑次数 | 实测与预测偏差超容差 |
| 失败模式 | 对 fp16 用 fp32 阈值导致误报 | 未固定 seed 造成随机失败 |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 测试治理 在“兼容迁移”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“兼容迁移：测试治理”沿相邻边界延伸：golden 值与版本记录、CI matrix 覆盖、门禁与回归集管理。比较版本与替代实现的差异（如 pytest），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [兼容迁移] assert_close 的 rtol/atol 如何按 dtype 选择才合理？
2. [兼容迁移] gradcheck 在什么情况下会误报失败？
3. [兼容迁移] 如何量化并治理 flaky 测试？

#### 8. 易错点与反例

1. [兼容迁移] 用 == 比较浮点张量
2. [兼容迁移] 以为关闭 cuDNN benchmark 就能完全确定
3. [兼容迁移] 把 golden 值硬编码却不记录环境版本

#### 9. 实践与验证

在旧/新版本各跑同一测试矩阵，对比输出、报错与性能，沉淀兼容回归集。具体到 测试治理：固定 seed 重跑 N 次统计通过率；用 assert_close 显式指定 rtol/atol 而非手写比较。把 对 fp16 用 fp32 阈值导致误报 作为负例，记录 torch.autograd.gradcheck 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“测试治理”的名词，说不出「相同 seed+相同算法下结果可复现」或 对 fp16 用 fp32 阈值导致误报。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 |a-b|≤atol+rtol×|b|；flake 率≈失败次数/重跑次数 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 未固定 seed 造成随机失败 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A071. 故障注入：张量/设备

- 难度：高级
- 知识域：K01 张量/设备
- 考察能力：资源预算、吞吐、尾延迟、容量阈值与单位成本
- 题型：性能/容量/成本
- 面试频率：中频
- 前置知识：shape、stride、dtype、device
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

故障排查按现象分层、建假设树，先止损再定位根因，并防复发。本题位于 K01 张量/设备。view 只改元数据且要求 stride 兼容；reshape 在不兼容时会静默拷贝，可用 t.data_ptr() 判断是否共享存储。

#### 2. 问题分析

从“故障注入：张量/设备”出发，先稳定复现与止损，把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 torch.Tensor.view/reshape 的契约，列出两条不变量：「view 与源共享 storage（data_ptr 与 storage_offset 一致）」「张量的 device/dtype 决定 kernel 派发与是否发生隐式转换」，再给一个会击穿假设的边界输入，并说明它最先暴露 对非连续张量 view 报 'view size is not compatible with input tensor's size and stride' 还是 跨设备运算报 'Expected all tensors to be on the same device'。

#### 3. 具体答案

面试中可以这样回答：

> 我会描述现象、排除项、最小复现、根因机制和复发防护措施。在 张量/设备 上，关键事实是：显存占用=numel×element_size：fp32 每元素 4B，fp16/bf16 每元素 2B。 判据可写为 显存(bytes)=numel×itemsize(dtype)；连续 stride[i]=∏_{j>i}shape[j]。

#### 4. 技术洞察

根因必须能被最小复现验证；无法复现的解释只是假设而不是结论。在 张量/设备 上，量化关系是 显存(bytes)=numel×itemsize(dtype)；连续 stride[i]=∏_{j>i}shape[j]；核心不变量是「view 与源共享 storage（data_ptr 与 storage_offset 一致）」，它一旦被打破，最先表现为 reshape 静默拷贝抬高峰值显存。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | torch.Tensor.view/reshape 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | view 与源共享 storage（data_ptr 与 storage_offset 一致） | 违反「张量的 device/dtype 决定 kernel 派发与是否发生隐式转换」时的报错 |
| 量化关系 | 显存(bytes)=numel×itemsize(dtype)；连续 stride[i]=∏_{j>i}shape[j] | 实测与预测偏差超容差 |
| 失败模式 | 对非连续张量 view 报 'view size is not compatible with input tensor's size and stride' | 跨设备运算报 'Expected all tensors to be on the same device' |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 张量/设备 在“故障注入”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“故障注入：张量/设备”沿相邻边界延伸：view/reshape/contiguous、pin_memory 与 stream、dtype 转换与显存。比较版本与替代实现的差异（如 t.to(device, non_blocking=True)），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [故障注入] shape=(2,3,4) 分别 view 成 (6,4)、reshape 成 (6,4) 后，stride 与是否共享存储各是什么？
2. [故障注入] 为什么 permute 之后必须先 contiguous 才能 view？
3. [故障注入] non_blocking=True 在源未 pinned 时的实际行为如何用 profiler 证明？

#### 8. 易错点与反例

1. [故障注入] 把 reshape 当零拷贝：stride 不兼容时它会 contiguous 并分配新显存
2. [故障注入] 只比较 shape 相等就判断可 view 互转，忽略 stride/storage_offset
3. [故障注入] 以为 pin_memory 总有效，实际它只对 non_blocking 有加速意义

#### 9. 实践与验证

注入单点故障（延迟/丢包/超时/OOM），观察系统表现并验证降级与恢复。具体到 张量/设备：构造 (2,3,4) 后断言 t.stride()==(12,4,1) 且 t.is_contiguous()；用 t.data_ptr() 验证 view 与 reshape 的共享/拷贝差异。把 对非连续张量 view 报 'view size is not compatible with input tensor's size and stride' 作为负例，记录 t.stride()/t.is_contiguous() 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“张量/设备”的名词，说不出「view 与源共享 storage（data_ptr 与 storage_offset 一致）」或 对非连续张量 view 报 'view size is not compatible with input tensor's size and stride'。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 显存(bytes)=numel×itemsize(dtype)；连续 stride[i]=∏_{j>i}shape[j] 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 跨设备运算报 'Expected all tensors to be on the same device' 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A072. 故障注入：autograd

- 难度：高级
- 知识域：K02 autograd
- 考察能力：资源预算、吞吐、尾延迟、容量阈值与单位成本
- 题型：性能/容量/成本
- 面试频率：中频
- 前置知识：requires_grad、leaf、view、detach、反向
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

故障排查按现象分层、建假设树，先止损再定位根因，并防复发。本题位于 K02 autograd。backward 默认累加而非覆盖，所以每个 step 前需 zero_grad，否则梯度跨 step 求和。

#### 2. 问题分析

从“故障注入：autograd”出发，先稳定复现与止损，把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 Tensor.requires_grad_ 的契约，列出两条不变量：「叶子张量 .grad 的形状与张量一致且只由叶子持有」「requires_grad 沿 grad_fn 链单向传播」，再给一个会击穿假设的边界输入，并说明它最先暴露 忘记 zero_grad 使梯度累加 还是 对 requires_grad 的叶子做原地操作报 'a leaf Variable that requires grad is being used in an in-place operation'。

#### 3. 具体答案

面试中可以这样回答：

> 我会描述现象、排除项、最小复现、根因机制和复发防护措施。在 autograd 上，关键事实是：反向默认释放计算图，第二次反向需 retain_graph=True，否则报 'Trying to backward through the graph a second time'。 判据可写为 累加语义 g←g+∂L/∂θ；链式法则 dL/dx=Σ(dL/dy·dy/dx)。

#### 4. 技术洞察

根因必须能被最小复现验证；无法复现的解释只是假设而不是结论。在 autograd 上，量化关系是 累加语义 g←g+∂L/∂θ；链式法则 dL/dx=Σ(dL/dy·dy/dx)；核心不变量是「叶子张量 .grad 的形状与张量一致且只由叶子持有」，它一旦被打破，最先表现为 两次 backward 未设 retain_graph 触发 RuntimeError。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | Tensor.requires_grad_ 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | 叶子张量 .grad 的形状与张量一致且只由叶子持有 | 违反「requires_grad 沿 grad_fn 链单向传播」时的报错 |
| 量化关系 | 累加语义 g←g+∂L/∂θ；链式法则 dL/dx=Σ(dL/dy·dy/dx) | 实测与预测偏差超容差 |
| 失败模式 | 忘记 zero_grad 使梯度累加 | 对 requires_grad 的叶子做原地操作报 'a leaf Variable that requires grad is being used in an in-place operation' |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 autograd 在“故障注入”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“故障注入：autograd”沿相邻边界延伸：detach、retain_grad、in-place 与版本计数、gradcheck。比较版本与替代实现的差异（如 torch.autograd.grad），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [故障注入] 为什么非叶子张量的 .grad 默认是 None，retain_grad() 改变了什么？
2. [故障注入] detach() 与 with torch.no_grad() 在切断梯度上的差别是什么？
3. [故障注入] in-place 操作在什么条件下能安全使用而不破坏 autograd？

#### 8. 易错点与反例

1. [故障注入] 认为非叶子 .grad 会自动填充，实则默认 None，需要 retain_grad()
2. [故障注入] 在 no_grad 下构建输入再 backward，得到 requires_grad 缺失错误
3. [故障注入] 用 param = param - lr*g 原地更新破坏计算图

#### 9. 实践与验证

注入单点故障（延迟/丢包/超时/OOM），观察系统表现并验证降级与恢复。具体到 autograd：断言 x.is_leaf and x.grad is None 后再 backward；用 torch.autograd.grad(..., retain_graph=True) 做两次反向对比。把 忘记 zero_grad 使梯度累加 作为负例，记录 Tensor.detach() 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“autograd”的名词，说不出「叶子张量 .grad 的形状与张量一致且只由叶子持有」或 忘记 zero_grad 使梯度累加。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 累加语义 g←g+∂L/∂θ；链式法则 dL/dx=Σ(dL/dy·dy/dx) 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 对 requires_grad 的叶子做原地操作报 'a leaf Variable that requires grad is being used in an in-place operation' 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A073. 故障注入：Module/状态

- 难度：高级
- 知识域：K03 Module/状态
- 考察能力：资源预算、吞吐、尾延迟、容量阈值与单位成本
- 题型：性能/容量/成本
- 面试频率：中频
- 前置知识：参数、buffer、模式和 state_dict
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

故障排查按现象分层、建假设树，先止损再定位根因，并防复发。本题位于 K03 Module/状态。load_state_dict 默认 strict=True，缺失或多出 key 都会报错。

#### 2. 问题分析

从“故障注入：Module/状态”出发，先稳定复现与止损，把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 register_buffer 的契约，列出两条不变量：「同一 Module 的 state_dict key 集合确定且与设备无关」「buffer 的 dtype/device 跟随 module 或按注册时指定」，再给一个会击穿假设的边界输入，并说明它最先暴露 自定义 buffer 未注册，.to(device) 后仍留在 CPU 还是 load_state_dict 因 missing/unexpected keys 失败。

#### 3. 具体答案

面试中可以这样回答：

> 我会描述现象、排除项、最小复现、根因机制和复发防护措施。在 Module/状态 上，关键事实是：.eval() 把 self.training 置 False，切换 Dropout/BatchNorm 行为，但不改变 requires_grad。 判据可写为 state_dict={k:param}∪{k:buffer}；训练态显存≈Σ numel×itemsize×(1+grad+opt_state)。

#### 4. 技术洞察

根因必须能被最小复现验证；无法复现的解释只是假设而不是结论。在 Module/状态 上，量化关系是 state_dict={k:param}∪{k:buffer}；训练态显存≈Σ numel×itemsize×(1+grad+opt_state)；核心不变量是「同一 Module 的 state_dict key 集合确定且与设备无关」，它一旦被打破，最先表现为 eval 下忘记切换使 Dropout 仍在抽样。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | register_buffer 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | 同一 Module 的 state_dict key 集合确定且与设备无关 | 违反「buffer 的 dtype/device 跟随 module 或按注册时指定」时的报错 |
| 量化关系 | state_dict={k:param}∪{k:buffer}；训练态显存≈Σ numel×itemsize×(1+grad+opt_state) | 实测与预测偏差超容差 |
| 失败模式 | 自定义 buffer 未注册，.to(device) 后仍留在 CPU | load_state_dict 因 missing/unexpected keys 失败 |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 Module/状态 在“故障注入”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“故障注入：Module/状态”沿相邻边界延伸：persistent buffer、hook 与状态迁移、DDP 下的 buffer 同步。比较版本与替代实现的差异（如 state_dict/load_state_dict），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [故障注入] buffer 与 Parameter 在 state_dict、梯度和设备迁移上的差异是什么？
2. [故障注入] BatchNorm 在 eval 模式仍更新 running stats 需要什么额外设置？
3. [故障注入] register_buffer(persistent=False) 会从 state_dict 中移除什么？

#### 8. 易错点与反例

1. [故障注入] 把计数/统计量存成普通属性，导致 state_dict 丢失该状态
2. [故障注入] load_state_dict 后忘记 module.eval()，使 BN/Dropout 行为不符
3. [故障注入] 直接给同名属性赋值绕过注册，使 named_parameters 缺失

#### 9. 实践与验证

注入单点故障（延迟/丢包/超时/OOM），观察系统表现并验证降级与恢复。具体到 Module/状态：断言 set(m.state_dict().keys()) 与 checkpoint 完全一致；对比 train/eval 两次前向输出验证 Dropout 关闭。把 自定义 buffer 未注册，.to(device) 后仍留在 CPU 作为负例，记录 register_parameter 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“Module/状态”的名词，说不出「同一 Module 的 state_dict key 集合确定且与设备无关」或 自定义 buffer 未注册，.to(device) 后仍留在 CPU。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 state_dict={k:param}∪{k:buffer}；训练态显存≈Σ numel×itemsize×(1+grad+opt_state) 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 load_state_dict 因 missing/unexpected keys 失败 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A074. 故障注入：数据管道

- 难度：高级
- 知识域：K04 数据管道
- 考察能力：资源预算、吞吐、尾延迟、容量阈值与单位成本
- 题型：性能/容量/成本
- 面试频率：中频
- 前置知识：Dataset、Sampler、collate、padding、版本
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

故障排查按现象分层、建假设树，先止损再定位根因，并防复发。本题位于 K04 数据管道。DistributedSampler 按 rank 切分样本，样本数不能被 world_size 整除时默认补齐以避免各 rank 步数不同。

#### 2. 问题分析

从“故障注入：数据管道”出发，先稳定复现与止损，把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 DataLoader(num_workers, pin_memory, persistent_workers) 的契约，列出两条不变量：「shuffle=False 且种子固定时迭代顺序可复现」「各 rank 采样集合互斥且并集覆盖全集（或按 drop_last 截断）」，再给一个会击穿假设的边界输入，并说明它最先暴露 多 worker 共享同一随机源导致样本重复 还是 worker 数过多耗尽文件描述符或内存。

#### 3. 具体答案

面试中可以这样回答：

> 我会描述现象、排除项、最小复现、根因机制和复发防护措施。在 数据管道 上，关键事实是：persistent_workers=True 复用 worker，避免每个 epoch 重新 fork。 判据可写为 吞吐≤min(存储带宽, CPU 解码速率, H2D 带宽)；有效样本/步=batch×world_size。

#### 4. 技术洞察

根因必须能被最小复现验证；无法复现的解释只是假设而不是结论。在 数据管道 上，量化关系是 吞吐≤min(存储带宽, CPU 解码速率, H2D 带宽)；有效样本/步=batch×world_size；核心不变量是「shuffle=False 且种子固定时迭代顺序可复现」，它一旦被打破，最先表现为 DistributedSampler 与自定义 shuffle 双打乱造成跨 rank 重复。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | DataLoader(num_workers, pin_memory, persistent_workers) 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | shuffle=False 且种子固定时迭代顺序可复现 | 违反「各 rank 采样集合互斥且并集覆盖全集（或按 drop_last 截断）」时的报错 |
| 量化关系 | 吞吐≤min(存储带宽, CPU 解码速率, H2D 带宽)；有效样本/步=batch×world_size | 实测与预测偏差超容差 |
| 失败模式 | 多 worker 共享同一随机源导致样本重复 | worker 数过多耗尽文件描述符或内存 |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 数据管道 在“故障注入”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“故障注入：数据管道”沿相邻边界延伸：IterableDataset、chunk 预取、对象存储吞吐与 IPC 开销。比较版本与替代实现的差异（如 collate_fn），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [故障注入] DistributedSampler 如何处理样本数不被 world_size 整除，是否引入重复样本？
2. [故障注入] num_workers 增大时首步延迟、内存与 IPC 开销如何变化？
3. [故障注入] pin_memory 与 non_blocking 为何必须同时开启才有效？

#### 8. 易错点与反例

1. [故障注入] 把 num_workers 调大就以为更快，忽略 IPC 与内存放大
2. [故障注入] Dataset 里用全局随机而不经 worker_init_fn 重播种
3. [故障注入] 自己 shuffle 又用 DistributedSampler，导致跨 rank 样本重复

#### 9. 实践与验证

注入单点故障（延迟/丢包/超时/OOM），观察系统表现并验证降级与恢复。具体到 数据管道：固定 seed 两次迭代断言顺序一致；统计各 rank 采样索引并集是否覆盖全集。把 多 worker 共享同一随机源导致样本重复 作为负例，记录 DistributedSampler 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“数据管道”的名词，说不出「shuffle=False 且种子固定时迭代顺序可复现」或 多 worker 共享同一随机源导致样本重复。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 吞吐≤min(存储带宽, CPU 解码速率, H2D 带宽)；有效样本/步=batch×world_size 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 worker 数过多耗尽文件描述符或内存 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A075. 故障注入：训练/优化器

- 难度：高级
- 知识域：K05 训练/优化器
- 考察能力：资源预算、吞吐、尾延迟、容量阈值与单位成本
- 题型：性能/容量/成本
- 面试频率：中频
- 前置知识：forward、backward、累积、optimizer、scheduler
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

故障排查按现象分层、建假设树，先止损再定位根因，并防复发。本题位于 K05 训练/优化器。PyTorch 1.1.0 起 lr_scheduler.step() 应在 optimizer.step() 之后调用；顺序错误只给 warning 并产生错误 lr。

#### 2. 问题分析

从“故障注入：训练/优化器”出发，先稳定复现与止损，把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 optimizer.zero_grad(set_to_none=True) 的契约，列出两条不变量：「反向之前梯度应为空或已清零」「scheduler 与 optimizer 的 step 次数对齐」，再给一个会击穿假设的边界输入，并说明它最先暴露 scheduler.step() 放错位置使 lr 序列整体偏移 还是 累积时直接累加 loss 未归一化。

#### 3. 具体答案

面试中可以这样回答：

> 我会描述现象、排除项、最小复现、根因机制和复发防护措施。在 训练/优化器 上，关键事实是：梯度累积须按累积步数折算 loss（loss/steps），否则等效学习率被放大。 判据可写为 有效 batch=micro_batch×grad_accum×world_size；Adam 状态≈2P(状态)+P(梯度)。

#### 4. 技术洞察

根因必须能被最小复现验证；无法复现的解释只是假设而不是结论。在 训练/优化器 上，量化关系是 有效 batch=micro_batch×grad_accum×world_size；Adam 状态≈2P(状态)+P(梯度)；核心不变量是「反向之前梯度应为空或已清零」，它一旦被打破，最先表现为 zero_grad 漏调导致梯度累加。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | optimizer.zero_grad(set_to_none=True) 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | 反向之前梯度应为空或已清零 | 违反「scheduler 与 optimizer 的 step 次数对齐」时的报错 |
| 量化关系 | 有效 batch=micro_batch×grad_accum×world_size；Adam 状态≈2P(状态)+P(梯度) | 实测与预测偏差超容差 |
| 失败模式 | scheduler.step() 放错位置使 lr 序列整体偏移 | 累积时直接累加 loss 未归一化 |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 训练/优化器 在“故障注入”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“故障注入：训练/优化器”沿相邻边界延伸：scheduler 恢复、优化器 state_dict、foreach/fused 实现差异。比较版本与替代实现的差异（如 lr_scheduler.step()），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [故障注入] lr_scheduler.step() 在 PyTorch 1.1.0 前后位置的语义差异是什么？
2. [故障注入] 梯度累积如何在数学上等价于大 batch，需要哪些归一化？
3. [故障注入] set_to_none=True 对显存与优化器行为有何具体影响？

#### 8. 易错点与反例

1. [故障注入] 用 zero_grad() 不传 set_to_none，长期分配开销更高
2. [故障注入] 把 scheduler.step() 在 epoch 与 step 间混用，导致 lr 跳变
3. [故障注入] 梯度累积时未除 steps，使等效学习率偏大

#### 9. 实践与验证

注入单点故障（延迟/丢包/超时/OOM），观察系统表现并验证降级与恢复。具体到 训练/优化器：打印前若干步 param_groups[0]['lr'] 验证调度曲线；比较累积 N 步与单步大 batch 的梯度 L2 范数。把 scheduler.step() 放错位置使 lr 序列整体偏移 作为负例，记录 optimizer.step() 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“训练/优化器”的名词，说不出「反向之前梯度应为空或已清零」或 scheduler.step() 放错位置使 lr 序列整体偏移。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 有效 batch=micro_batch×grad_accum×world_size；Adam 状态≈2P(状态)+P(梯度) 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 累积时直接累加 loss 未归一化 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A076. 故障注入：AMP/恢复

- 难度：高级
- 知识域：K06 AMP/恢复
- 考察能力：资源预算、吞吐、尾延迟、容量阈值与单位成本
- 题型：性能/容量/成本
- 面试频率：中频
- 前置知识：autocast、scaler、快照、随机状态
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

故障排查按现象分层、建假设树，先止损再定位根因，并防复发。本题位于 K06 AMP/恢复。出现 inf/nan 时 scaler.step 跳过本次更新并降低 scale，避免污染权重。

#### 2. 问题分析

从“故障注入：AMP/恢复”出发，先稳定复现与止损，把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 torch.autocast(device_type) 的契约，列出两条不变量：「反向前 loss 被 scale 放大，step 前梯度必须被 unscale」「inf/nan 检查与 skip 分支成对出现」，再给一个会击穿假设的边界输入，并说明它最先暴露 clip 放在 unscale 之前使梯度幅度错误 还是 对 bf16 仍用 GradScaler 造成无谓开销。

#### 3. 具体答案

面试中可以这样回答：

> 我会描述现象、排除项、最小复现、根因机制和复发防护措施。在 AMP/恢复 上，关键事实是：梯度裁剪前必须 scaler.unscale_(optimizer)，否则裁剪的是被放大后的梯度。 判据可写为 scale_{new}=scale×growth_factor（连续无溢出）或 scale/backoff_factor（检测到 inf/nan）。

#### 4. 技术洞察

根因必须能被最小复现验证；无法复现的解释只是假设而不是结论。在 AMP/恢复 上，量化关系是 scale_{new}=scale×growth_factor（连续无溢出）或 scale/backoff_factor（检测到 inf/nan）；核心不变量是「反向前 loss 被 scale 放大，step 前梯度必须被 unscale」，它一旦被打破，最先表现为 checkpoint 未保存 scaler.state_dict 使恢复后 scale 不连续。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | torch.autocast(device_type) 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | 反向前 loss 被 scale 放大，step 前梯度必须被 unscale | 违反「inf/nan 检查与 skip 分支成对出现」时的报错 |
| 量化关系 | scale_{new}=scale×growth_factor（连续无溢出）或 scale/backoff_factor（检测到 inf/nan） | 实测与预测偏差超容差 |
| 失败模式 | clip 放在 unscale 之前使梯度幅度错误 | 对 bf16 仍用 GradScaler 造成无谓开销 |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 AMP/恢复 在“故障注入”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“故障注入：AMP/恢复”沿相邻边界延伸：autocast 的 op 白/黑名单、随机状态快照、断点续训一致性。比较版本与替代实现的差异（如 scaler.scale/step/update），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [故障注入] GradScaler 连续多少次无溢出才提高 scale，默认参数是多少？
2. [故障注入] 为什么 fp16 autocast 必须 unscale 之后才能裁剪梯度？
3. [故障注入] bf16 为什么通常不需要 GradScaler？

#### 8. 易错点与反例

1. [故障注入] 以为 autocast 会把参数也转 fp16（参数保持 fp32）
2. [故障注入] 用未 unscale 的梯度直接裁剪
3. [故障注入] 恢复训练时忘记保存/加载 scaler 状态

#### 9. 实践与验证

注入单点故障（延迟/丢包/超时/OOM），观察系统表现并验证降级与恢复。具体到 AMP/恢复：记录 scaler.get_scale() 在若干 step 的轨迹；注入含 nan 的 loss 验证该 step 被跳过。把 clip 放在 unscale 之前使梯度幅度错误 作为负例，记录 GradScaler 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“AMP/恢复”的名词，说不出「反向前 loss 被 scale 放大，step 前梯度必须被 unscale」或 clip 放在 unscale 之前使梯度幅度错误。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 scale_{new}=scale×growth_factor（连续无溢出）或 scale/backoff_factor（检测到 inf/nan） 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 对 bf16 仍用 GradScaler 造成无谓开销 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A077. 故障注入：分布式

- 难度：高级
- 知识域：K07 分布式
- 考察能力：资源预算、吞吐、尾延迟、容量阈值与单位成本
- 题型：性能/容量/成本
- 面试频率：中频
- 前置知识：rank、DDP、FSDP、collective、容错
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

故障排查按现象分层、建假设树，先止损再定位根因，并防复发。本题位于 K07 分布式。FSDP 把参数、梯度、优化器状态分片到各 rank：前向 all_gather 参数、反向 reduce_scatter 梯度。

#### 2. 问题分析

从“故障注入：分布式”出发，先稳定复现与止损，把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 init_process_group 的契约，列出两条不变量：「所有 rank 的 rank/world_size 一致」「collective 调用序列在各 rank 严格一致」，再给一个会击穿假设的边界输入，并说明它最先暴露 某些 rank 提前 return 使 collective 不匹配而 hang 还是 梯度 size 不一致触发 'Mismatch in collective'。

#### 3. 具体答案

面试中可以这样回答：

> 我会描述现象、排除项、最小复现、根因机制和复发防护措施。在 分布式 上，关键事实是：collective 要求所有 rank 以相同顺序、相同 dtype/shape 调用，否则 hang 或 NCCL 报错。 判据可写为 ring allreduce≈2(N-1)/N×M；DDP 每卡显存≈params+grads+opt_states。

#### 4. 技术洞察

根因必须能被最小复现验证；无法复现的解释只是假设而不是结论。在 分布式 上，量化关系是 ring allreduce≈2(N-1)/N×M；DDP 每卡显存≈params+grads+opt_states；核心不变量是「所有 rank 的 rank/world_size 一致」，它一旦被打破，最先表现为 FSDP 未 wrap 子模块使参数无法分片。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | init_process_group 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | 所有 rank 的 rank/world_size 一致 | 违反「collective 调用序列在各 rank 严格一致」时的报错 |
| 量化关系 | ring allreduce≈2(N-1)/N×M；DDP 每卡显存≈params+grads+opt_states | 实测与预测偏差超容差 |
| 失败模式 | 某些 rank 提前 return 使 collective 不匹配而 hang | 梯度 size 不一致触发 'Mismatch in collective' |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 分布式 在“故障注入”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“故障注入：分布式”沿相邻边界延伸：process group 切分、混合并行、NCCL 拓扑与超时。比较版本与替代实现的差异（如 FullyShardedDataParallel），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [故障注入] DDP 的梯度 bucketing 如何影响通信与计算重叠？
2. [故障注入] FSDP 的 all_gather 与 reduce_scatter 分别在前向/反向何处发生？
3. [故障注入] collective 调用顺序不一致时表现为 hang 还是报错，原因是什么？

#### 8. 易错点与反例

1. [故障注入] 对非参数 buffer 手动 allreduce，造成重复同步
2. [故障注入] 认为 FSDP 降低单卡计算量（实际只降显存）
3. [故障注入] 忽略 bucket_cap_mb 对重叠与峰值显存的影响

#### 9. 实践与验证

注入单点故障（延迟/丢包/超时/OOM），观察系统表现并验证降级与恢复。具体到 分布式：barrier 后断言 get_rank()/get_world_size()；对比 DDP 与 FSDP 的 max_memory_allocated。把 某些 rank 提前 return 使 collective 不匹配而 hang 作为负例，记录 DistributedDataParallel 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“分布式”的名词，说不出「所有 rank 的 rank/world_size 一致」或 某些 rank 提前 return 使 collective 不匹配而 hang。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 ring allreduce≈2(N-1)/N×M；DDP 每卡显存≈params+grads+opt_states 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 梯度 size 不一致触发 'Mismatch in collective' 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A078. 故障注入：编译栈

- 难度：高级
- 知识域：K08 编译栈
- 考察能力：资源预算、吞吐、尾延迟、容量阈值与单位成本
- 题型：性能/容量/成本
- 面试频率：中频
- 前置知识：Dynamo、AOTAutograd、Inductor、guard、缓存
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

故障排查按现象分层、建假设树，先止损再定位根因，并防复发。本题位于 K08 编译栈。graph break 把图切段回退 eager，破坏端到端融合。

#### 2. 问题分析

从“故障注入：编译栈”出发，先稳定复现与止损，把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 torch.compile 的契约，列出两条不变量：「fullgraph=True 时任何 graph break 都抛错」「同一 guard 命中后不再触发编译」，再给一个会击穿假设的边界输入，并说明它最先暴露 动态 shape 未标记导致反复重编译 还是 数据相关控制流触发 graph break。

#### 3. 具体答案

面试中可以这样回答：

> 我会描述现象、排除项、最小复现、根因机制和复发防护措施。在 编译栈 上，关键事实是：AOTAutograd 生成前向与反向图，Inductor 生成 Triton kernel（GPU）或 C++ kernel（CPU）。 判据可写为 编译缓存以 guard 值集合为键；重编译次数∝出现过的不同 guard 组合数。

#### 4. 技术洞察

根因必须能被最小复现验证；无法复现的解释只是假设而不是结论。在 编译栈 上，量化关系是 编译缓存以 guard 值集合为键；重编译次数∝出现过的不同 guard 组合数；核心不变量是「fullgraph=True 时任何 graph break 都抛错」，它一旦被打破，最先表现为 CUDA Graph 因输入地址变化捕获失败。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | torch.compile 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | fullgraph=True 时任何 graph break 都抛错 | 违反「同一 guard 命中后不再触发编译」时的报错 |
| 量化关系 | 编译缓存以 guard 值集合为键；重编译次数∝出现过的不同 guard 组合数 | 实测与预测偏差超容差 |
| 失败模式 | 动态 shape 未标记导致反复重编译 | 数据相关控制流触发 graph break |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 编译栈 在“故障注入”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“故障注入：编译栈”沿相邻边界延伸：dynamic shape 的 Dim/mark_dynamic、guard 导出、AOT 与自定义 op。比较版本与替代实现的差异（如 TORCH_LOGS=graph_breaks,recompiles），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [故障注入] Dynamo 的 guard 在什么条件下失效并触发重编译？
2. [故障注入] graph break 对 autograd 图切分与状态副作用带来什么风险？
3. [故障注入] reduce-overhead 的 CUDA Graph 为何要求输入地址固定？

#### 8. 易错点与反例

1. [故障注入] 以为 graph break 只影响性能，忽略断点两侧的状态副作用
2. [故障注入] 用固定 shape 基准掩盖动态 shape 的重编译
3. [故障注入] 忽略首次编译冷启动开销

#### 9. 实践与验证

注入单点故障（延迟/丢包/超时/OOM），观察系统表现并验证降级与恢复。具体到 编译栈：用 torch._dynamo.explain 统计 graph_breaks 数量；监控 counters['inductor']['recompiles']。把 动态 shape 未标记导致反复重编译 作为负例，记录 torch._dynamo.explain 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“编译栈”的名词，说不出「fullgraph=True 时任何 graph break 都抛错」或 动态 shape 未标记导致反复重编译。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 编译缓存以 guard 值集合为键；重编译次数∝出现过的不同 guard 组合数 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 数据相关控制流触发 graph break 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A079. 故障注入：性能

- 难度：高级
- 知识域：K09 性能
- 考察能力：资源预算、吞吐、尾延迟、容量阈值与单位成本
- 题型：性能/容量/成本
- 面试频率：中频
- 前置知识：关键路径、带宽、算力、通信、显存、统计
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

故障排查按现象分层、建假设树，先止损再定位根因，并防复发。本题位于 K09 性能。GPU 计时必须 warmup 并 torch.cuda.synchronize()，否则测到的是入队时间。

#### 2. 问题分析

从“故障注入：性能”出发，先稳定复现与止损，把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 torch.profiler 的契约，列出两条不变量：「所有 kernel 时间之和≤墙钟时间，差额为同步/空泡」「峰值显存≥活跃张量之和+碎片+缓存块」，再给一个会击穿假设的边界输入，并说明它最先暴露 未 sync 就读 Event 时间 还是 只测稳态忽略首次冷启动。

#### 3. 具体答案

面试中可以这样回答：

> 我会描述现象、排除项、最小复现、根因机制和复发防护措施。在 性能 上，关键事实是：PyTorch 用缓存分配器，小张量反复分配不一定触发 cudaMalloc，但会碎片化。 判据可写为 t≈max(FLOPs/FLOPS_peak, Bytes/BW_peak)；算术强度=FLOPs/Bytes。

#### 4. 技术洞察

根因必须能被最小复现验证；无法复现的解释只是假设而不是结论。在 性能 上，量化关系是 t≈max(FLOPs/FLOPS_peak, Bytes/BW_peak)；算术强度=FLOPs/Bytes；核心不变量是「所有 kernel 时间之和≤墙钟时间，差额为同步/空泡」，它一旦被打破，最先表现为 把 H2D 拷贝算进 GPU 计算时间。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | torch.profiler 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | 所有 kernel 时间之和≤墙钟时间，差额为同步/空泡 | 违反「峰值显存≥活跃张量之和+碎片+缓存块」时的报错 |
| 量化关系 | t≈max(FLOPs/FLOPS_peak, Bytes/BW_peak)；算术强度=FLOPs/Bytes | 实测与预测偏差超容差 |
| 失败模式 | 未 sync 就读 Event 时间 | 只测稳态忽略首次冷启动 |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 性能 在“故障注入”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“故障注入：性能”沿相邻边界延伸：roofline 拐点、显存碎片与快照、通信/计算重叠度量。比较版本与替代实现的差异（如 torch.cuda.max_memory_allocated），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [故障注入] 如何用 roofline 判断算子 compute-bound 还是 memory-bound，需要哪些输入？
2. [故障注入] 为什么 profiler 的 self_cuda_time 之和常小于 step time？
3. [故障注入] 在通信与计算未重叠时，先改数据并行还是分片/bucket 策略？

#### 8. 易错点与反例

1. [故障注入] 用 CPU 计时测 GPU kernel，得到错误结论
2. [故障注入] 忽略通信与 sync 造成的时间空泡
3. [故障注入] 以单次测量下结论，被启动噪声误导

#### 9. 实践与验证

注入单点故障（延迟/丢包/超时/OOM），观察系统表现并验证降级与恢复。具体到 性能：profiler 导出 kernel 表按 cuda_time 排序定位热点；固定输入重复 N 次取中位数与 p99。把 未 sync 就读 Event 时间 作为负例，记录 torch.cuda.Event 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“性能”的名词，说不出「所有 kernel 时间之和≤墙钟时间，差额为同步/空泡」或 未 sync 就读 Event 时间。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 t≈max(FLOPs/FLOPS_peak, Bytes/BW_peak)；算术强度=FLOPs/Bytes 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 只测稳态忽略首次冷启动 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A080. 故障注入：导出部署

- 难度：高级
- 知识域：K10 导出部署
- 考察能力：资源预算、吞吐、尾延迟、容量阈值与单位成本
- 题型：性能/容量/成本
- 面试频率：中频
- 前置知识：state_dict、Export、runtime、schema、迁移
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

故障排查按现象分层、建假设树，先止损再定位根因，并防复发。本题位于 K10 序列化/导出/部署。torch.jit.trace 记录一次执行路径，忽略数据相关控制流（if/for 会塌缩）。

#### 2. 问题分析

从“故障注入：序列化/导出/部署”出发，先稳定复现与止损，把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 torch.save/torch.load(weights_only=) 的契约，列出两条不变量：「导出后的图不含 Python 语义，仅含 ATen/custom op 调用」「state_dict 的 key 语义与训练侧一致」，再给一个会击穿假设的边界输入，并说明它最先暴露 trace 把数据相关分支固化为单一路径 还是 自定义 op 未注册导致导出失败。

#### 3. 具体答案

面试中可以这样回答：

> 我会描述现象、排除项、最小复现、根因机制和复发防护措施。在 序列化/导出/部署 上，关键事实是：torch.export 基于 Dynamo 捕获 ATen 级图，输出 ExportedProgram（graph_module、state_dict、range_constraints）。 判据可写为 导出图=静态化 Python 控制流+固定/声明化 shape（Dim 约束）。

#### 4. 技术洞察

根因必须能被最小复现验证；无法复现的解释只是假设而不是结论。在 序列化/导出/部署 上，量化关系是 导出图=静态化 Python 控制流+固定/声明化 shape（Dim 约束）；核心不变量是「导出后的图不含 Python 语义，仅含 ATen/custom op 调用」，它一旦被打破，最先表现为 weights_only=True 加载旧 pickle 报错。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | torch.save/torch.load(weights_only=) 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | 导出后的图不含 Python 语义，仅含 ATen/custom op 调用 | 违反「state_dict 的 key 语义与训练侧一致」时的报错 |
| 量化关系 | 导出图=静态化 Python 控制流+固定/声明化 shape（Dim 约束） | 实测与预测偏差超容差 |
| 失败模式 | trace 把数据相关分支固化为单一路径 | 自定义 op 未注册导致导出失败 |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 序列化/导出/部署 在“故障注入”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“故障注入：序列化/导出/部署”沿相邻边界延伸：custom op 的 register_autograd、ONNX/ExecuTorch runtime、版本 schema 演进。比较版本与替代实现的差异（如 torch.export.export），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [故障注入] torch.jit.trace 与 torch.export 在控制流处理上有什么本质区别？
2. [故障注入] weights_only 自 2.6 起默认为 True 带来哪些兼容性影响？
3. [故障注入] 如何用 dynamic_shapes 表达一个仅序列维动态的模型并验证？

#### 8. 易错点与反例

1. [故障注入] 用 trace 导出含 if/loop 的模型，使数据相关分支被固化
2. [故障注入] 认为 weights_only=True 永远安全（自定义 op/类仍需注册）
3. [故障注入] 忽略导出后 dtype/device 与运行时不一致

#### 9. 实践与验证

注入单点故障（延迟/丢包/超时/OOM），观察系统表现并验证降级与恢复。具体到 序列化/导出/部署：对比导出前后在相同输入上的输出 assert_close；检查 ExportedProgram.graph_module.graph 的 op 集合。把 trace 把数据相关分支固化为单一路径 作为负例，记录 torch.jit.trace/script 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“序列化/导出/部署”的名词，说不出「导出后的图不含 Python 语义，仅含 ATen/custom op 调用」或 trace 把数据相关分支固化为单一路径。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 导出图=静态化 Python 控制流+固定/声明化 shape（Dim 约束） 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 自定义 op 未注册导致导出失败 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A081. 故障注入：自定义算子

- 难度：高级
- 知识域：K11 自定义算子
- 考察能力：接口、状态机、隔离、灰度、回滚与治理
- 题型：系统设计/工程落地
- 面试频率：中频
- 前置知识：schema、dispatcher、stream、autograd、ABI
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

故障排查按现象分层、建假设树，先止损再定位根因，并防复发。本题位于 K11 自定义算子。kernel 必须在当前 CUDA stream 上执行并保证输入/输出的 stream 语义，否则与其它算子乱序。

#### 2. 问题分析

从“故障注入：自定义算子”出发，先稳定复现与止损，把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 torch.library/TORCH_LIBRARY 的契约，列出两条不变量：「torch.autograd.gradcheck 通过表示前向/反向数值一致」「no_grad 与 grad 模式除梯度外结果一致」，再给一个会击穿假设的边界输入，并说明它最先暴露 漏写设备选择/CUDAGuard 使多卡下用错设备 还是 C++ ABI 不匹配报 undefined symbol。

#### 3. 具体答案

面试中可以这样回答：

> 我会描述现象、排除项、最小复现、根因机制和复发防护措施。在 自定义算子 上，关键事实是：CUDA 扩展编译需匹配 PyTorch 的 C++ ABI（_GLIBCXX_USE_CXX11_ABI）与 CUDA 架构 -gencode。 判据可写为 加速比=t_baseline/t_custom（同 shape/dtype/stream）；occupancy=活跃 warp/最大 warp。

#### 4. 技术洞察

根因必须能被最小复现验证；无法复现的解释只是假设而不是结论。在 自定义算子 上，量化关系是 加速比=t_baseline/t_custom（同 shape/dtype/stream）；occupancy=活跃 warp/最大 warp；核心不变量是「torch.autograd.gradcheck 通过表示前向/反向数值一致」，它一旦被打破，最先表现为 内核用原子加导致非确定性。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | torch.library/TORCH_LIBRARY 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | torch.autograd.gradcheck 通过表示前向/反向数值一致 | 违反「no_grad 与 grad 模式除梯度外结果一致」时的报错 |
| 量化关系 | 加速比=t_baseline/t_custom（同 shape/dtype/stream）；occupancy=活跃 warp/最大 warp | 实测与预测偏差超容差 |
| 失败模式 | 漏写设备选择/CUDAGuard 使多卡下用错设备 | C++ ABI 不匹配报 undefined symbol |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 自定义算子 在“故障注入”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“故障注入：自定义算子”沿相邻边界延伸：op schema 与 fake/meta 注册、流与事件同步、编译期架构选择。比较版本与替代实现的差异（如 torch.library.register_autograd），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [故障注入] 为自定义算子实现反向有哪两种机制，各自适用场景是什么？
2. [故障注入] 为什么 kernel 必须使用当前 stream，如何获取与同步？
3. [故障注入] CUDA 扩展的 C++ ABI 不匹配会以什么形式暴露？

#### 8. 易错点与反例

1. [故障注入] 只写前向不注册反向，autograd 报错或梯度为 None
2. [故障注入] 在默认 stream 上跑 kernel 却从非默认 stream 读数据
3. [故障注入] 忽略 C++ ABI 造成运行时链接失败

#### 9. 实践与验证

注入单点故障（延迟/丢包/超时/OOM），观察系统表现并验证降级与恢复。具体到 自定义算子：gradcheck 校验自定义 Function 的梯度；torch.testing.assert_close 对比参考实现。把 漏写设备选择/CUDAGuard 使多卡下用错设备 作为负例，记录 torch.autograd.Function 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“自定义算子”的名词，说不出「torch.autograd.gradcheck 通过表示前向/反向数值一致」或 漏写设备选择/CUDAGuard 使多卡下用错设备。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 加速比=t_baseline/t_custom（同 shape/dtype/stream）；occupancy=活跃 warp/最大 warp 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 C++ ABI 不匹配报 undefined symbol 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A082. 故障注入：测试治理

- 难度：高级
- 知识域：K12 测试治理
- 考察能力：接口、状态机、隔离、灰度、回滚与治理
- 题型：系统设计/工程落地
- 面试频率：中频
- 前置知识：oracle、证据、日志、回滚、门禁、复盘
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

故障排查按现象分层、建假设树，先止损再定位根因，并防复发。本题位于 K12 测试治理。gradcheck 用有限差分对比解析梯度，默认 eps=1e-6、atol=1e-5。

#### 2. 问题分析

从“故障注入：测试治理”出发，先稳定复现与止损，把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 torch.testing.assert_close 的契约，列出两条不变量：「相同 seed+相同算法下结果可复现」「通过阈值必须与 dtype 精度匹配」，再给一个会击穿假设的边界输入，并说明它最先暴露 对 fp16 用 fp32 阈值导致误报 还是 未固定 seed 造成随机失败。

#### 3. 具体答案

面试中可以这样回答：

> 我会描述现象、排除项、最小复现、根因机制和复发防护措施。在 测试治理 上，关键事实是：非确定性来源包括原子加、cuDNN benchmark、多线程归约顺序。 判据可写为 |a-b|≤atol+rtol×|b|；flake 率≈失败次数/重跑次数。

#### 4. 技术洞察

根因必须能被最小复现验证；无法复现的解释只是假设而不是结论。在 测试治理 上，量化关系是 |a-b|≤atol+rtol×|b|；flake 率≈失败次数/重跑次数；核心不变量是「相同 seed+相同算法下结果可复现」，它一旦被打破，最先表现为 把 flaky 当稳定失败反复重试掩盖问题。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | torch.testing.assert_close 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | 相同 seed+相同算法下结果可复现 | 违反「通过阈值必须与 dtype 精度匹配」时的报错 |
| 量化关系 | |a-b|≤atol+rtol×|b|；flake 率≈失败次数/重跑次数 | 实测与预测偏差超容差 |
| 失败模式 | 对 fp16 用 fp32 阈值导致误报 | 未固定 seed 造成随机失败 |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 测试治理 在“故障注入”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“故障注入：测试治理”沿相邻边界延伸：golden 值与版本记录、CI matrix 覆盖、门禁与回归集管理。比较版本与替代实现的差异（如 pytest），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [故障注入] assert_close 的 rtol/atol 如何按 dtype 选择才合理？
2. [故障注入] gradcheck 在什么情况下会误报失败？
3. [故障注入] 如何量化并治理 flaky 测试？

#### 8. 易错点与反例

1. [故障注入] 用 == 比较浮点张量
2. [故障注入] 以为关闭 cuDNN benchmark 就能完全确定
3. [故障注入] 把 golden 值硬编码却不记录环境版本

#### 9. 实践与验证

注入单点故障（延迟/丢包/超时/OOM），观察系统表现并验证降级与恢复。具体到 测试治理：固定 seed 重跑 N 次统计通过率；用 assert_close 显式指定 rtol/atol 而非手写比较。把 对 fp16 用 fp32 阈值导致误报 作为负例，记录 torch.autograd.gradcheck 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“测试治理”的名词，说不出「相同 seed+相同算法下结果可复现」或 对 fp16 用 fp32 阈值导致误报。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 |a-b|≤atol+rtol×|b|；flake 率≈失败次数/重跑次数 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 未固定 seed 造成随机失败 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A083. 实现评审：张量/设备

- 难度：高级
- 知识域：K01 张量/设备
- 考察能力：接口、状态机、隔离、灰度、回滚与治理
- 题型：系统设计/工程落地
- 面试频率：中频
- 前置知识：shape、stride、dtype、device
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

实现评审关注不变量、接口契约、可观测性与演进成本。本题位于 K01 张量/设备。显存占用=numel×element_size：fp32 每元素 4B，fp16/bf16 每元素 2B。

#### 2. 问题分析

从“实现评审：张量/设备”出发，先按接口、状态机、错误处理、测试覆盖和运维逐项核对，把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 torch.Tensor.view/reshape 的契约，列出两条不变量：「view 与源共享 storage（data_ptr 与 storage_offset 一致）」「张量的 device/dtype 决定 kernel 派发与是否发生隐式转换」，再给一个会击穿假设的边界输入，并说明它最先暴露 对非连续张量 view 报 'view size is not compatible with input tensor's size and stride' 还是 跨设备运算报 'Expected all tensors to be on the same device'。

#### 3. 具体答案

面试中可以这样回答：

> 我会给出接口契约、关键不变量、缺陷类型与可执行的改进建议。在 张量/设备 上，关键事实是：non_blocking=True 只有在源 pinned 且目标是加速器时才真正异步，否则退化为同步拷贝。 判据可写为 显存(bytes)=numel×itemsize(dtype)；连续 stride[i]=∏_{j>i}shape[j]。

#### 4. 技术洞察

评审的核心是识别隐性不变量：文档没写、代码却依赖的假设。在 张量/设备 上，量化关系是 显存(bytes)=numel×itemsize(dtype)；连续 stride[i]=∏_{j>i}shape[j]；核心不变量是「view 与源共享 storage（data_ptr 与 storage_offset 一致）」，它一旦被打破，最先表现为 reshape 静默拷贝抬高峰值显存。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | torch.Tensor.view/reshape 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | view 与源共享 storage（data_ptr 与 storage_offset 一致） | 违反「张量的 device/dtype 决定 kernel 派发与是否发生隐式转换」时的报错 |
| 量化关系 | 显存(bytes)=numel×itemsize(dtype)；连续 stride[i]=∏_{j>i}shape[j] | 实测与预测偏差超容差 |
| 失败模式 | 对非连续张量 view 报 'view size is not compatible with input tensor's size and stride' | 跨设备运算报 'Expected all tensors to be on the same device' |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 张量/设备 在“实现评审”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“实现评审：张量/设备”沿相邻边界延伸：view/reshape/contiguous、pin_memory 与 stream、dtype 转换与显存。比较版本与替代实现的差异（如 t.to(device, non_blocking=True)），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [实现评审] shape=(2,3,4) 分别 view 成 (6,4)、reshape 成 (6,4) 后，stride 与是否共享存储各是什么？
2. [实现评审] 为什么 permute 之后必须先 contiguous 才能 view？
3. [实现评审] non_blocking=True 在源未 pinned 时的实际行为如何用 profiler 证明？

#### 8. 易错点与反例

1. [实现评审] 把 reshape 当零拷贝：stride 不兼容时它会 contiguous 并分配新显存
2. [实现评审] 只比较 shape 相等就判断可 view 互转，忽略 stride/storage_offset
3. [实现评审] 以为 pin_memory 总有效，实际它只对 non_blocking 有加速意义

#### 9. 实践与验证

用最小反例/边界输入攻击实现，检查断言、日志与错误分类是否到位。具体到 张量/设备：构造 (2,3,4) 后断言 t.stride()==(12,4,1) 且 t.is_contiguous()；用 t.data_ptr() 验证 view 与 reshape 的共享/拷贝差异。把 对非连续张量 view 报 'view size is not compatible with input tensor's size and stride' 作为负例，记录 t.stride()/t.is_contiguous() 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“张量/设备”的名词，说不出「view 与源共享 storage（data_ptr 与 storage_offset 一致）」或 对非连续张量 view 报 'view size is not compatible with input tensor's size and stride'。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 显存(bytes)=numel×itemsize(dtype)；连续 stride[i]=∏_{j>i}shape[j] 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 跨设备运算报 'Expected all tensors to be on the same device' 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A084. 实现评审：autograd

- 难度：高级
- 知识域：K02 autograd
- 考察能力：接口、状态机、隔离、灰度、回滚与治理
- 题型：系统设计/工程落地
- 面试频率：中频
- 前置知识：requires_grad、leaf、view、detach、反向
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

实现评审关注不变量、接口契约、可观测性与演进成本。本题位于 K02 autograd。反向默认释放计算图，第二次反向需 retain_graph=True，否则报 'Trying to backward through the graph a second time'。

#### 2. 问题分析

从“实现评审：autograd”出发，先按接口、状态机、错误处理、测试覆盖和运维逐项核对，把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 Tensor.requires_grad_ 的契约，列出两条不变量：「叶子张量 .grad 的形状与张量一致且只由叶子持有」「requires_grad 沿 grad_fn 链单向传播」，再给一个会击穿假设的边界输入，并说明它最先暴露 忘记 zero_grad 使梯度累加 还是 对 requires_grad 的叶子做原地操作报 'a leaf Variable that requires grad is being used in an in-place operation'。

#### 3. 具体答案

面试中可以这样回答：

> 我会给出接口契约、关键不变量、缺陷类型与可执行的改进建议。在 autograd 上，关键事实是：param.data.add_() 绕过 autograd，是原地更新权重而不破坏图的标准做法。 判据可写为 累加语义 g←g+∂L/∂θ；链式法则 dL/dx=Σ(dL/dy·dy/dx)。

#### 4. 技术洞察

评审的核心是识别隐性不变量：文档没写、代码却依赖的假设。在 autograd 上，量化关系是 累加语义 g←g+∂L/∂θ；链式法则 dL/dx=Σ(dL/dy·dy/dx)；核心不变量是「叶子张量 .grad 的形状与张量一致且只由叶子持有」，它一旦被打破，最先表现为 两次 backward 未设 retain_graph 触发 RuntimeError。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | Tensor.requires_grad_ 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | 叶子张量 .grad 的形状与张量一致且只由叶子持有 | 违反「requires_grad 沿 grad_fn 链单向传播」时的报错 |
| 量化关系 | 累加语义 g←g+∂L/∂θ；链式法则 dL/dx=Σ(dL/dy·dy/dx) | 实测与预测偏差超容差 |
| 失败模式 | 忘记 zero_grad 使梯度累加 | 对 requires_grad 的叶子做原地操作报 'a leaf Variable that requires grad is being used in an in-place operation' |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 autograd 在“实现评审”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“实现评审：autograd”沿相邻边界延伸：detach、retain_grad、in-place 与版本计数、gradcheck。比较版本与替代实现的差异（如 torch.autograd.grad），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [实现评审] 为什么非叶子张量的 .grad 默认是 None，retain_grad() 改变了什么？
2. [实现评审] detach() 与 with torch.no_grad() 在切断梯度上的差别是什么？
3. [实现评审] in-place 操作在什么条件下能安全使用而不破坏 autograd？

#### 8. 易错点与反例

1. [实现评审] 认为非叶子 .grad 会自动填充，实则默认 None，需要 retain_grad()
2. [实现评审] 在 no_grad 下构建输入再 backward，得到 requires_grad 缺失错误
3. [实现评审] 用 param = param - lr*g 原地更新破坏计算图

#### 9. 实践与验证

用最小反例/边界输入攻击实现，检查断言、日志与错误分类是否到位。具体到 autograd：断言 x.is_leaf and x.grad is None 后再 backward；用 torch.autograd.grad(..., retain_graph=True) 做两次反向对比。把 忘记 zero_grad 使梯度累加 作为负例，记录 Tensor.detach() 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“autograd”的名词，说不出「叶子张量 .grad 的形状与张量一致且只由叶子持有」或 忘记 zero_grad 使梯度累加。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 累加语义 g←g+∂L/∂θ；链式法则 dL/dx=Σ(dL/dy·dy/dx) 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 对 requires_grad 的叶子做原地操作报 'a leaf Variable that requires grad is being used in an in-place operation' 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A085. 实现评审：Module/状态

- 难度：高级
- 知识域：K03 Module/状态
- 考察能力：接口、状态机、隔离、灰度、回滚与治理
- 题型：系统设计/工程落地
- 面试频率：中频
- 前置知识：参数、buffer、模式和 state_dict
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

实现评审关注不变量、接口契约、可观测性与演进成本。本题位于 K03 Module/状态。.eval() 把 self.training 置 False，切换 Dropout/BatchNorm 行为，但不改变 requires_grad。

#### 2. 问题分析

从“实现评审：Module/状态”出发，先按接口、状态机、错误处理、测试覆盖和运维逐项核对，把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 register_buffer 的契约，列出两条不变量：「同一 Module 的 state_dict key 集合确定且与设备无关」「buffer 的 dtype/device 跟随 module 或按注册时指定」，再给一个会击穿假设的边界输入，并说明它最先暴露 自定义 buffer 未注册，.to(device) 后仍留在 CPU 还是 load_state_dict 因 missing/unexpected keys 失败。

#### 3. 具体答案

面试中可以这样回答：

> 我会给出接口契约、关键不变量、缺陷类型与可执行的改进建议。在 Module/状态 上，关键事实是：BatchNorm 的 running_mean/running_var 是 buffer，eval 下不再更新、只用统计量。 判据可写为 state_dict={k:param}∪{k:buffer}；训练态显存≈Σ numel×itemsize×(1+grad+opt_state)。

#### 4. 技术洞察

评审的核心是识别隐性不变量：文档没写、代码却依赖的假设。在 Module/状态 上，量化关系是 state_dict={k:param}∪{k:buffer}；训练态显存≈Σ numel×itemsize×(1+grad+opt_state)；核心不变量是「同一 Module 的 state_dict key 集合确定且与设备无关」，它一旦被打破，最先表现为 eval 下忘记切换使 Dropout 仍在抽样。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | register_buffer 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | 同一 Module 的 state_dict key 集合确定且与设备无关 | 违反「buffer 的 dtype/device 跟随 module 或按注册时指定」时的报错 |
| 量化关系 | state_dict={k:param}∪{k:buffer}；训练态显存≈Σ numel×itemsize×(1+grad+opt_state) | 实测与预测偏差超容差 |
| 失败模式 | 自定义 buffer 未注册，.to(device) 后仍留在 CPU | load_state_dict 因 missing/unexpected keys 失败 |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 Module/状态 在“实现评审”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“实现评审：Module/状态”沿相邻边界延伸：persistent buffer、hook 与状态迁移、DDP 下的 buffer 同步。比较版本与替代实现的差异（如 state_dict/load_state_dict），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [实现评审] buffer 与 Parameter 在 state_dict、梯度和设备迁移上的差异是什么？
2. [实现评审] BatchNorm 在 eval 模式仍更新 running stats 需要什么额外设置？
3. [实现评审] register_buffer(persistent=False) 会从 state_dict 中移除什么？

#### 8. 易错点与反例

1. [实现评审] 把计数/统计量存成普通属性，导致 state_dict 丢失该状态
2. [实现评审] load_state_dict 后忘记 module.eval()，使 BN/Dropout 行为不符
3. [实现评审] 直接给同名属性赋值绕过注册，使 named_parameters 缺失

#### 9. 实践与验证

用最小反例/边界输入攻击实现，检查断言、日志与错误分类是否到位。具体到 Module/状态：断言 set(m.state_dict().keys()) 与 checkpoint 完全一致；对比 train/eval 两次前向输出验证 Dropout 关闭。把 自定义 buffer 未注册，.to(device) 后仍留在 CPU 作为负例，记录 register_parameter 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“Module/状态”的名词，说不出「同一 Module 的 state_dict key 集合确定且与设备无关」或 自定义 buffer 未注册，.to(device) 后仍留在 CPU。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 state_dict={k:param}∪{k:buffer}；训练态显存≈Σ numel×itemsize×(1+grad+opt_state) 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 load_state_dict 因 missing/unexpected keys 失败 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A086. 实现评审：数据管道

- 难度：高级
- 知识域：K04 数据管道
- 考察能力：接口、状态机、隔离、灰度、回滚与治理
- 题型：系统设计/工程落地
- 面试频率：中频
- 前置知识：Dataset、Sampler、collate、padding、版本
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

实现评审关注不变量、接口契约、可观测性与演进成本。本题位于 K04 数据管道。persistent_workers=True 复用 worker，避免每个 epoch 重新 fork。

#### 2. 问题分析

从“实现评审：数据管道”出发，先按接口、状态机、错误处理、测试覆盖和运维逐项核对，把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 DataLoader(num_workers, pin_memory, persistent_workers) 的契约，列出两条不变量：「shuffle=False 且种子固定时迭代顺序可复现」「各 rank 采样集合互斥且并集覆盖全集（或按 drop_last 截断）」，再给一个会击穿假设的边界输入，并说明它最先暴露 多 worker 共享同一随机源导致样本重复 还是 worker 数过多耗尽文件描述符或内存。

#### 3. 具体答案

面试中可以这样回答：

> 我会给出接口契约、关键不变量、缺陷类型与可执行的改进建议。在 数据管道 上，关键事实是：pin_memory=True 把 batch 放进页锁定内存，配合 non_blocking 才能与计算重叠。 判据可写为 吞吐≤min(存储带宽, CPU 解码速率, H2D 带宽)；有效样本/步=batch×world_size。

#### 4. 技术洞察

评审的核心是识别隐性不变量：文档没写、代码却依赖的假设。在 数据管道 上，量化关系是 吞吐≤min(存储带宽, CPU 解码速率, H2D 带宽)；有效样本/步=batch×world_size；核心不变量是「shuffle=False 且种子固定时迭代顺序可复现」，它一旦被打破，最先表现为 DistributedSampler 与自定义 shuffle 双打乱造成跨 rank 重复。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | DataLoader(num_workers, pin_memory, persistent_workers) 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | shuffle=False 且种子固定时迭代顺序可复现 | 违反「各 rank 采样集合互斥且并集覆盖全集（或按 drop_last 截断）」时的报错 |
| 量化关系 | 吞吐≤min(存储带宽, CPU 解码速率, H2D 带宽)；有效样本/步=batch×world_size | 实测与预测偏差超容差 |
| 失败模式 | 多 worker 共享同一随机源导致样本重复 | worker 数过多耗尽文件描述符或内存 |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 数据管道 在“实现评审”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“实现评审：数据管道”沿相邻边界延伸：IterableDataset、chunk 预取、对象存储吞吐与 IPC 开销。比较版本与替代实现的差异（如 collate_fn），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [实现评审] DistributedSampler 如何处理样本数不被 world_size 整除，是否引入重复样本？
2. [实现评审] num_workers 增大时首步延迟、内存与 IPC 开销如何变化？
3. [实现评审] pin_memory 与 non_blocking 为何必须同时开启才有效？

#### 8. 易错点与反例

1. [实现评审] 把 num_workers 调大就以为更快，忽略 IPC 与内存放大
2. [实现评审] Dataset 里用全局随机而不经 worker_init_fn 重播种
3. [实现评审] 自己 shuffle 又用 DistributedSampler，导致跨 rank 样本重复

#### 9. 实践与验证

用最小反例/边界输入攻击实现，检查断言、日志与错误分类是否到位。具体到 数据管道：固定 seed 两次迭代断言顺序一致；统计各 rank 采样索引并集是否覆盖全集。把 多 worker 共享同一随机源导致样本重复 作为负例，记录 DistributedSampler 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“数据管道”的名词，说不出「shuffle=False 且种子固定时迭代顺序可复现」或 多 worker 共享同一随机源导致样本重复。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 吞吐≤min(存储带宽, CPU 解码速率, H2D 带宽)；有效样本/步=batch×world_size 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 worker 数过多耗尽文件描述符或内存 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A087. 实现评审：训练/优化器

- 难度：高级
- 知识域：K05 训练/优化器
- 考察能力：接口、状态机、隔离、灰度、回滚与治理
- 题型：系统设计/工程落地
- 面试频率：中频
- 前置知识：forward、backward、累积、optimizer、scheduler
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

实现评审关注不变量、接口契约、可观测性与演进成本。本题位于 K05 训练/优化器。梯度累积须按累积步数折算 loss（loss/steps），否则等效学习率被放大。

#### 2. 问题分析

从“实现评审：训练/优化器”出发，先按接口、状态机、错误处理、测试覆盖和运维逐项核对，把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 optimizer.zero_grad(set_to_none=True) 的契约，列出两条不变量：「反向之前梯度应为空或已清零」「scheduler 与 optimizer 的 step 次数对齐」，再给一个会击穿假设的边界输入，并说明它最先暴露 scheduler.step() 放错位置使 lr 序列整体偏移 还是 累积时直接累加 loss 未归一化。

#### 3. 具体答案

面试中可以这样回答：

> 我会给出接口契约、关键不变量、缺陷类型与可执行的改进建议。在 训练/优化器 上，关键事实是：Adam 每参数维护 exp_avg 与 exp_avg_sq，优化器状态约 2×参数量。 判据可写为 有效 batch=micro_batch×grad_accum×world_size；Adam 状态≈2P(状态)+P(梯度)。

#### 4. 技术洞察

评审的核心是识别隐性不变量：文档没写、代码却依赖的假设。在 训练/优化器 上，量化关系是 有效 batch=micro_batch×grad_accum×world_size；Adam 状态≈2P(状态)+P(梯度)；核心不变量是「反向之前梯度应为空或已清零」，它一旦被打破，最先表现为 zero_grad 漏调导致梯度累加。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | optimizer.zero_grad(set_to_none=True) 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | 反向之前梯度应为空或已清零 | 违反「scheduler 与 optimizer 的 step 次数对齐」时的报错 |
| 量化关系 | 有效 batch=micro_batch×grad_accum×world_size；Adam 状态≈2P(状态)+P(梯度) | 实测与预测偏差超容差 |
| 失败模式 | scheduler.step() 放错位置使 lr 序列整体偏移 | 累积时直接累加 loss 未归一化 |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 训练/优化器 在“实现评审”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“实现评审：训练/优化器”沿相邻边界延伸：scheduler 恢复、优化器 state_dict、foreach/fused 实现差异。比较版本与替代实现的差异（如 lr_scheduler.step()），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [实现评审] lr_scheduler.step() 在 PyTorch 1.1.0 前后位置的语义差异是什么？
2. [实现评审] 梯度累积如何在数学上等价于大 batch，需要哪些归一化？
3. [实现评审] set_to_none=True 对显存与优化器行为有何具体影响？

#### 8. 易错点与反例

1. [实现评审] 用 zero_grad() 不传 set_to_none，长期分配开销更高
2. [实现评审] 把 scheduler.step() 在 epoch 与 step 间混用，导致 lr 跳变
3. [实现评审] 梯度累积时未除 steps，使等效学习率偏大

#### 9. 实践与验证

用最小反例/边界输入攻击实现，检查断言、日志与错误分类是否到位。具体到 训练/优化器：打印前若干步 param_groups[0]['lr'] 验证调度曲线；比较累积 N 步与单步大 batch 的梯度 L2 范数。把 scheduler.step() 放错位置使 lr 序列整体偏移 作为负例，记录 optimizer.step() 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“训练/优化器”的名词，说不出「反向之前梯度应为空或已清零」或 scheduler.step() 放错位置使 lr 序列整体偏移。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 有效 batch=micro_batch×grad_accum×world_size；Adam 状态≈2P(状态)+P(梯度) 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 累积时直接累加 loss 未归一化 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A088. 实现评审：AMP/恢复

- 难度：高级
- 知识域：K06 AMP/恢复
- 考察能力：接口、状态机、隔离、灰度、回滚与治理
- 题型：系统设计/工程落地
- 面试频率：中频
- 前置知识：autocast、scaler、快照、随机状态
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

实现评审关注不变量、接口契约、可观测性与演进成本。本题位于 K06 AMP/恢复。梯度裁剪前必须 scaler.unscale_(optimizer)，否则裁剪的是被放大后的梯度。

#### 2. 问题分析

从“实现评审：AMP/恢复”出发，先按接口、状态机、错误处理、测试覆盖和运维逐项核对，把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 torch.autocast(device_type) 的契约，列出两条不变量：「反向前 loss 被 scale 放大，step 前梯度必须被 unscale」「inf/nan 检查与 skip 分支成对出现」，再给一个会击穿假设的边界输入，并说明它最先暴露 clip 放在 unscale 之前使梯度幅度错误 还是 对 bf16 仍用 GradScaler 造成无谓开销。

#### 3. 具体答案

面试中可以这样回答：

> 我会给出接口契约、关键不变量、缺陷类型与可执行的改进建议。在 AMP/恢复 上，关键事实是：bf16 动态范围与 fp32 相同，通常无需 GradScaler；fp16 才需要 loss scaling。 判据可写为 scale_{new}=scale×growth_factor（连续无溢出）或 scale/backoff_factor（检测到 inf/nan）。

#### 4. 技术洞察

评审的核心是识别隐性不变量：文档没写、代码却依赖的假设。在 AMP/恢复 上，量化关系是 scale_{new}=scale×growth_factor（连续无溢出）或 scale/backoff_factor（检测到 inf/nan）；核心不变量是「反向前 loss 被 scale 放大，step 前梯度必须被 unscale」，它一旦被打破，最先表现为 checkpoint 未保存 scaler.state_dict 使恢复后 scale 不连续。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | torch.autocast(device_type) 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | 反向前 loss 被 scale 放大，step 前梯度必须被 unscale | 违反「inf/nan 检查与 skip 分支成对出现」时的报错 |
| 量化关系 | scale_{new}=scale×growth_factor（连续无溢出）或 scale/backoff_factor（检测到 inf/nan） | 实测与预测偏差超容差 |
| 失败模式 | clip 放在 unscale 之前使梯度幅度错误 | 对 bf16 仍用 GradScaler 造成无谓开销 |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 AMP/恢复 在“实现评审”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“实现评审：AMP/恢复”沿相邻边界延伸：autocast 的 op 白/黑名单、随机状态快照、断点续训一致性。比较版本与替代实现的差异（如 scaler.scale/step/update），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [实现评审] GradScaler 连续多少次无溢出才提高 scale，默认参数是多少？
2. [实现评审] 为什么 fp16 autocast 必须 unscale 之后才能裁剪梯度？
3. [实现评审] bf16 为什么通常不需要 GradScaler？

#### 8. 易错点与反例

1. [实现评审] 以为 autocast 会把参数也转 fp16（参数保持 fp32）
2. [实现评审] 用未 unscale 的梯度直接裁剪
3. [实现评审] 恢复训练时忘记保存/加载 scaler 状态

#### 9. 实践与验证

用最小反例/边界输入攻击实现，检查断言、日志与错误分类是否到位。具体到 AMP/恢复：记录 scaler.get_scale() 在若干 step 的轨迹；注入含 nan 的 loss 验证该 step 被跳过。把 clip 放在 unscale 之前使梯度幅度错误 作为负例，记录 GradScaler 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“AMP/恢复”的名词，说不出「反向前 loss 被 scale 放大，step 前梯度必须被 unscale」或 clip 放在 unscale 之前使梯度幅度错误。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 scale_{new}=scale×growth_factor（连续无溢出）或 scale/backoff_factor（检测到 inf/nan） 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 对 bf16 仍用 GradScaler 造成无谓开销 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A089. 实现评审：分布式

- 难度：高级
- 知识域：K07 分布式
- 考察能力：接口、状态机、隔离、灰度、回滚与治理
- 题型：系统设计/工程落地
- 面试频率：中频
- 前置知识：rank、DDP、FSDP、collective、容错
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

实现评审关注不变量、接口契约、可观测性与演进成本。本题位于 K07 分布式。collective 要求所有 rank 以相同顺序、相同 dtype/shape 调用，否则 hang 或 NCCL 报错。

#### 2. 问题分析

从“实现评审：分布式”出发，先按接口、状态机、错误处理、测试覆盖和运维逐项核对，把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 init_process_group 的契约，列出两条不变量：「所有 rank 的 rank/world_size 一致」「collective 调用序列在各 rank 严格一致」，再给一个会击穿假设的边界输入，并说明它最先暴露 某些 rank 提前 return 使 collective 不匹配而 hang 还是 梯度 size 不一致触发 'Mismatch in collective'。

#### 3. 具体答案

面试中可以这样回答：

> 我会给出接口契约、关键不变量、缺陷类型与可执行的改进建议。在 分布式 上，关键事实是：ring allreduce 通信量约 2(N-1)/N×M，N 很大时趋近 2M。 判据可写为 ring allreduce≈2(N-1)/N×M；DDP 每卡显存≈params+grads+opt_states。

#### 4. 技术洞察

评审的核心是识别隐性不变量：文档没写、代码却依赖的假设。在 分布式 上，量化关系是 ring allreduce≈2(N-1)/N×M；DDP 每卡显存≈params+grads+opt_states；核心不变量是「所有 rank 的 rank/world_size 一致」，它一旦被打破，最先表现为 FSDP 未 wrap 子模块使参数无法分片。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | init_process_group 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | 所有 rank 的 rank/world_size 一致 | 违反「collective 调用序列在各 rank 严格一致」时的报错 |
| 量化关系 | ring allreduce≈2(N-1)/N×M；DDP 每卡显存≈params+grads+opt_states | 实测与预测偏差超容差 |
| 失败模式 | 某些 rank 提前 return 使 collective 不匹配而 hang | 梯度 size 不一致触发 'Mismatch in collective' |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 分布式 在“实现评审”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“实现评审：分布式”沿相邻边界延伸：process group 切分、混合并行、NCCL 拓扑与超时。比较版本与替代实现的差异（如 FullyShardedDataParallel），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [实现评审] DDP 的梯度 bucketing 如何影响通信与计算重叠？
2. [实现评审] FSDP 的 all_gather 与 reduce_scatter 分别在前向/反向何处发生？
3. [实现评审] collective 调用顺序不一致时表现为 hang 还是报错，原因是什么？

#### 8. 易错点与反例

1. [实现评审] 对非参数 buffer 手动 allreduce，造成重复同步
2. [实现评审] 认为 FSDP 降低单卡计算量（实际只降显存）
3. [实现评审] 忽略 bucket_cap_mb 对重叠与峰值显存的影响

#### 9. 实践与验证

用最小反例/边界输入攻击实现，检查断言、日志与错误分类是否到位。具体到 分布式：barrier 后断言 get_rank()/get_world_size()；对比 DDP 与 FSDP 的 max_memory_allocated。把 某些 rank 提前 return 使 collective 不匹配而 hang 作为负例，记录 DistributedDataParallel 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“分布式”的名词，说不出「所有 rank 的 rank/world_size 一致」或 某些 rank 提前 return 使 collective 不匹配而 hang。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 ring allreduce≈2(N-1)/N×M；DDP 每卡显存≈params+grads+opt_states 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 梯度 size 不一致触发 'Mismatch in collective' 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A090. 实现评审：编译栈

- 难度：高级
- 知识域：K08 编译栈
- 考察能力：接口、状态机、隔离、灰度、回滚与治理
- 题型：系统设计/工程落地
- 面试频率：中频
- 前置知识：Dynamo、AOTAutograd、Inductor、guard、缓存
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

实现评审关注不变量、接口契约、可观测性与演进成本。本题位于 K08 编译栈。AOTAutograd 生成前向与反向图，Inductor 生成 Triton kernel（GPU）或 C++ kernel（CPU）。

#### 2. 问题分析

从“实现评审：编译栈”出发，先按接口、状态机、错误处理、测试覆盖和运维逐项核对，把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 torch.compile 的契约，列出两条不变量：「fullgraph=True 时任何 graph break 都抛错」「同一 guard 命中后不再触发编译」，再给一个会击穿假设的边界输入，并说明它最先暴露 动态 shape 未标记导致反复重编译 还是 数据相关控制流触发 graph break。

#### 3. 具体答案

面试中可以这样回答：

> 我会给出接口契约、关键不变量、缺陷类型与可执行的改进建议。在 编译栈 上，关键事实是：reduce-overhead 使用 CUDA Graphs 降低 launch 开销，但要求输入地址稳定。 判据可写为 编译缓存以 guard 值集合为键；重编译次数∝出现过的不同 guard 组合数。

#### 4. 技术洞察

评审的核心是识别隐性不变量：文档没写、代码却依赖的假设。在 编译栈 上，量化关系是 编译缓存以 guard 值集合为键；重编译次数∝出现过的不同 guard 组合数；核心不变量是「fullgraph=True 时任何 graph break 都抛错」，它一旦被打破，最先表现为 CUDA Graph 因输入地址变化捕获失败。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | torch.compile 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | fullgraph=True 时任何 graph break 都抛错 | 违反「同一 guard 命中后不再触发编译」时的报错 |
| 量化关系 | 编译缓存以 guard 值集合为键；重编译次数∝出现过的不同 guard 组合数 | 实测与预测偏差超容差 |
| 失败模式 | 动态 shape 未标记导致反复重编译 | 数据相关控制流触发 graph break |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 编译栈 在“实现评审”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“实现评审：编译栈”沿相邻边界延伸：dynamic shape 的 Dim/mark_dynamic、guard 导出、AOT 与自定义 op。比较版本与替代实现的差异（如 TORCH_LOGS=graph_breaks,recompiles），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [实现评审] Dynamo 的 guard 在什么条件下失效并触发重编译？
2. [实现评审] graph break 对 autograd 图切分与状态副作用带来什么风险？
3. [实现评审] reduce-overhead 的 CUDA Graph 为何要求输入地址固定？

#### 8. 易错点与反例

1. [实现评审] 以为 graph break 只影响性能，忽略断点两侧的状态副作用
2. [实现评审] 用固定 shape 基准掩盖动态 shape 的重编译
3. [实现评审] 忽略首次编译冷启动开销

#### 9. 实践与验证

用最小反例/边界输入攻击实现，检查断言、日志与错误分类是否到位。具体到 编译栈：用 torch._dynamo.explain 统计 graph_breaks 数量；监控 counters['inductor']['recompiles']。把 动态 shape 未标记导致反复重编译 作为负例，记录 torch._dynamo.explain 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“编译栈”的名词，说不出「fullgraph=True 时任何 graph break 都抛错」或 动态 shape 未标记导致反复重编译。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 编译缓存以 guard 值集合为键；重编译次数∝出现过的不同 guard 组合数 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 数据相关控制流触发 graph break 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A091. 实现评审：性能

- 难度：高级
- 知识域：K09 性能
- 考察能力：不变量、二阶影响、证据缺口与演进路线
- 题型：综合开放/专家追问
- 面试频率：中频
- 前置知识：关键路径、带宽、算力、通信、显存、统计
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

实现评审关注不变量、接口契约、可观测性与演进成本。本题位于 K09 性能。PyTorch 用缓存分配器，小张量反复分配不一定触发 cudaMalloc，但会碎片化。

#### 2. 问题分析

从“实现评审：性能”出发，先按接口、状态机、错误处理、测试覆盖和运维逐项核对，把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 torch.profiler 的契约，列出两条不变量：「所有 kernel 时间之和≤墙钟时间，差额为同步/空泡」「峰值显存≥活跃张量之和+碎片+缓存块」，再给一个会击穿假设的边界输入，并说明它最先暴露 未 sync 就读 Event 时间 还是 只测稳态忽略首次冷启动。

#### 3. 具体答案

面试中可以这样回答：

> 我会给出接口契约、关键不变量、缺陷类型与可执行的改进建议。在 性能 上，关键事实是：注意力前向 FLOPs 约为 4·b·h·s²·d（含 QK^T 与 softmax 加权）。 判据可写为 t≈max(FLOPs/FLOPS_peak, Bytes/BW_peak)；算术强度=FLOPs/Bytes。

#### 4. 技术洞察

评审的核心是识别隐性不变量：文档没写、代码却依赖的假设。在 性能 上，量化关系是 t≈max(FLOPs/FLOPS_peak, Bytes/BW_peak)；算术强度=FLOPs/Bytes；核心不变量是「所有 kernel 时间之和≤墙钟时间，差额为同步/空泡」，它一旦被打破，最先表现为 把 H2D 拷贝算进 GPU 计算时间。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | torch.profiler 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | 所有 kernel 时间之和≤墙钟时间，差额为同步/空泡 | 违反「峰值显存≥活跃张量之和+碎片+缓存块」时的报错 |
| 量化关系 | t≈max(FLOPs/FLOPS_peak, Bytes/BW_peak)；算术强度=FLOPs/Bytes | 实测与预测偏差超容差 |
| 失败模式 | 未 sync 就读 Event 时间 | 只测稳态忽略首次冷启动 |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 性能 在“实现评审”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“实现评审：性能”沿相邻边界延伸：roofline 拐点、显存碎片与快照、通信/计算重叠度量。比较版本与替代实现的差异（如 torch.cuda.max_memory_allocated），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [实现评审] 如何用 roofline 判断算子 compute-bound 还是 memory-bound，需要哪些输入？
2. [实现评审] 为什么 profiler 的 self_cuda_time 之和常小于 step time？
3. [实现评审] 在通信与计算未重叠时，先改数据并行还是分片/bucket 策略？

#### 8. 易错点与反例

1. [实现评审] 用 CPU 计时测 GPU kernel，得到错误结论
2. [实现评审] 忽略通信与 sync 造成的时间空泡
3. [实现评审] 以单次测量下结论，被启动噪声误导

#### 9. 实践与验证

用最小反例/边界输入攻击实现，检查断言、日志与错误分类是否到位。具体到 性能：profiler 导出 kernel 表按 cuda_time 排序定位热点；固定输入重复 N 次取中位数与 p99。把 未 sync 就读 Event 时间 作为负例，记录 torch.cuda.Event 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“性能”的名词，说不出「所有 kernel 时间之和≤墙钟时间，差额为同步/空泡」或 未 sync 就读 Event 时间。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 t≈max(FLOPs/FLOPS_peak, Bytes/BW_peak)；算术强度=FLOPs/Bytes 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 只测稳态忽略首次冷启动 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A092. 实现评审：导出部署

- 难度：高级
- 知识域：K10 导出部署
- 考察能力：不变量、二阶影响、证据缺口与演进路线
- 题型：综合开放/专家追问
- 面试频率：中频
- 前置知识：state_dict、Export、runtime、schema、迁移
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

实现评审关注不变量、接口契约、可观测性与演进成本。本题位于 K10 序列化/导出/部署。torch.export 基于 Dynamo 捕获 ATen 级图，输出 ExportedProgram（graph_module、state_dict、range_constraints）。

#### 2. 问题分析

从“实现评审：序列化/导出/部署”出发，先按接口、状态机、错误处理、测试覆盖和运维逐项核对，把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 torch.save/torch.load(weights_only=) 的契约，列出两条不变量：「导出后的图不含 Python 语义，仅含 ATen/custom op 调用」「state_dict 的 key 语义与训练侧一致」，再给一个会击穿假设的边界输入，并说明它最先暴露 trace 把数据相关分支固化为单一路径 还是 自定义 op 未注册导致导出失败。

#### 3. 具体答案

面试中可以这样回答：

> 我会给出接口契约、关键不变量、缺陷类型与可执行的改进建议。在 序列化/导出/部署 上，关键事实是：动态维度需用 dynamic_shapes/Dim 显式声明，否则导出为静态 shape。 判据可写为 导出图=静态化 Python 控制流+固定/声明化 shape（Dim 约束）。

#### 4. 技术洞察

评审的核心是识别隐性不变量：文档没写、代码却依赖的假设。在 序列化/导出/部署 上，量化关系是 导出图=静态化 Python 控制流+固定/声明化 shape（Dim 约束）；核心不变量是「导出后的图不含 Python 语义，仅含 ATen/custom op 调用」，它一旦被打破，最先表现为 weights_only=True 加载旧 pickle 报错。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | torch.save/torch.load(weights_only=) 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | 导出后的图不含 Python 语义，仅含 ATen/custom op 调用 | 违反「state_dict 的 key 语义与训练侧一致」时的报错 |
| 量化关系 | 导出图=静态化 Python 控制流+固定/声明化 shape（Dim 约束） | 实测与预测偏差超容差 |
| 失败模式 | trace 把数据相关分支固化为单一路径 | 自定义 op 未注册导致导出失败 |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 序列化/导出/部署 在“实现评审”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“实现评审：序列化/导出/部署”沿相邻边界延伸：custom op 的 register_autograd、ONNX/ExecuTorch runtime、版本 schema 演进。比较版本与替代实现的差异（如 torch.export.export），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [实现评审] torch.jit.trace 与 torch.export 在控制流处理上有什么本质区别？
2. [实现评审] weights_only 自 2.6 起默认为 True 带来哪些兼容性影响？
3. [实现评审] 如何用 dynamic_shapes 表达一个仅序列维动态的模型并验证？

#### 8. 易错点与反例

1. [实现评审] 用 trace 导出含 if/loop 的模型，使数据相关分支被固化
2. [实现评审] 认为 weights_only=True 永远安全（自定义 op/类仍需注册）
3. [实现评审] 忽略导出后 dtype/device 与运行时不一致

#### 9. 实践与验证

用最小反例/边界输入攻击实现，检查断言、日志与错误分类是否到位。具体到 序列化/导出/部署：对比导出前后在相同输入上的输出 assert_close；检查 ExportedProgram.graph_module.graph 的 op 集合。把 trace 把数据相关分支固化为单一路径 作为负例，记录 torch.jit.trace/script 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“序列化/导出/部署”的名词，说不出「导出后的图不含 Python 语义，仅含 ATen/custom op 调用」或 trace 把数据相关分支固化为单一路径。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 导出图=静态化 Python 控制流+固定/声明化 shape（Dim 约束） 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 自定义 op 未注册导致导出失败 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A093. 实现评审：自定义算子

- 难度：高级
- 知识域：K11 自定义算子
- 考察能力：不变量、二阶影响、证据缺口与演进路线
- 题型：综合开放/专家追问
- 面试频率：中频
- 前置知识：schema、dispatcher、stream、autograd、ABI
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

实现评审关注不变量、接口契约、可观测性与演进成本。本题位于 K11 自定义算子。CUDA 扩展编译需匹配 PyTorch 的 C++ ABI（_GLIBCXX_USE_CXX11_ABI）与 CUDA 架构 -gencode。

#### 2. 问题分析

从“实现评审：自定义算子”出发，先按接口、状态机、错误处理、测试覆盖和运维逐项核对，把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 torch.library/TORCH_LIBRARY 的契约，列出两条不变量：「torch.autograd.gradcheck 通过表示前向/反向数值一致」「no_grad 与 grad 模式除梯度外结果一致」，再给一个会击穿假设的边界输入，并说明它最先暴露 漏写设备选择/CUDAGuard 使多卡下用错设备 还是 C++ ABI 不匹配报 undefined symbol。

#### 3. 具体答案

面试中可以这样回答：

> 我会给出接口契约、关键不变量、缺陷类型与可执行的改进建议。在 自定义算子 上，关键事实是：dispatcher 按 DispatchKey（CPU/CUDA/Autograd）选择实现，schema 决定算子签名。 判据可写为 加速比=t_baseline/t_custom（同 shape/dtype/stream）；occupancy=活跃 warp/最大 warp。

#### 4. 技术洞察

评审的核心是识别隐性不变量：文档没写、代码却依赖的假设。在 自定义算子 上，量化关系是 加速比=t_baseline/t_custom（同 shape/dtype/stream）；occupancy=活跃 warp/最大 warp；核心不变量是「torch.autograd.gradcheck 通过表示前向/反向数值一致」，它一旦被打破，最先表现为 内核用原子加导致非确定性。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | torch.library/TORCH_LIBRARY 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | torch.autograd.gradcheck 通过表示前向/反向数值一致 | 违反「no_grad 与 grad 模式除梯度外结果一致」时的报错 |
| 量化关系 | 加速比=t_baseline/t_custom（同 shape/dtype/stream）；occupancy=活跃 warp/最大 warp | 实测与预测偏差超容差 |
| 失败模式 | 漏写设备选择/CUDAGuard 使多卡下用错设备 | C++ ABI 不匹配报 undefined symbol |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 自定义算子 在“实现评审”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“实现评审：自定义算子”沿相邻边界延伸：op schema 与 fake/meta 注册、流与事件同步、编译期架构选择。比较版本与替代实现的差异（如 torch.library.register_autograd），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [实现评审] 为自定义算子实现反向有哪两种机制，各自适用场景是什么？
2. [实现评审] 为什么 kernel 必须使用当前 stream，如何获取与同步？
3. [实现评审] CUDA 扩展的 C++ ABI 不匹配会以什么形式暴露？

#### 8. 易错点与反例

1. [实现评审] 只写前向不注册反向，autograd 报错或梯度为 None
2. [实现评审] 在默认 stream 上跑 kernel 却从非默认 stream 读数据
3. [实现评审] 忽略 C++ ABI 造成运行时链接失败

#### 9. 实践与验证

用最小反例/边界输入攻击实现，检查断言、日志与错误分类是否到位。具体到 自定义算子：gradcheck 校验自定义 Function 的梯度；torch.testing.assert_close 对比参考实现。把 漏写设备选择/CUDAGuard 使多卡下用错设备 作为负例，记录 torch.autograd.Function 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“自定义算子”的名词，说不出「torch.autograd.gradcheck 通过表示前向/反向数值一致」或 漏写设备选择/CUDAGuard 使多卡下用错设备。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 加速比=t_baseline/t_custom（同 shape/dtype/stream）；occupancy=活跃 warp/最大 warp 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 C++ ABI 不匹配报 undefined symbol 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A094. 实现评审：测试治理

- 难度：高级
- 知识域：K12 测试治理
- 考察能力：不变量、二阶影响、证据缺口与演进路线
- 题型：综合开放/专家追问
- 面试频率：中频
- 前置知识：oracle、证据、日志、回滚、门禁、复盘
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

实现评审关注不变量、接口契约、可观测性与演进成本。本题位于 K12 测试治理。非确定性来源包括原子加、cuDNN benchmark、多线程归约顺序。

#### 2. 问题分析

从“实现评审：测试治理”出发，先按接口、状态机、错误处理、测试覆盖和运维逐项核对，把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 torch.testing.assert_close 的契约，列出两条不变量：「相同 seed+相同算法下结果可复现」「通过阈值必须与 dtype 精度匹配」，再给一个会击穿假设的边界输入，并说明它最先暴露 对 fp16 用 fp32 阈值导致误报 还是 未固定 seed 造成随机失败。

#### 3. 具体答案

面试中可以这样回答：

> 我会给出接口契约、关键不变量、缺陷类型与可执行的改进建议。在 测试治理 上，关键事实是：flaky 测试可用重跑通过率量化而非直接删除。 判据可写为 |a-b|≤atol+rtol×|b|；flake 率≈失败次数/重跑次数。

#### 4. 技术洞察

评审的核心是识别隐性不变量：文档没写、代码却依赖的假设。在 测试治理 上，量化关系是 |a-b|≤atol+rtol×|b|；flake 率≈失败次数/重跑次数；核心不变量是「相同 seed+相同算法下结果可复现」，它一旦被打破，最先表现为 把 flaky 当稳定失败反复重试掩盖问题。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | torch.testing.assert_close 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | 相同 seed+相同算法下结果可复现 | 违反「通过阈值必须与 dtype 精度匹配」时的报错 |
| 量化关系 | |a-b|≤atol+rtol×|b|；flake 率≈失败次数/重跑次数 | 实测与预测偏差超容差 |
| 失败模式 | 对 fp16 用 fp32 阈值导致误报 | 未固定 seed 造成随机失败 |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 测试治理 在“实现评审”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“实现评审：测试治理”沿相邻边界延伸：golden 值与版本记录、CI matrix 覆盖、门禁与回归集管理。比较版本与替代实现的差异（如 pytest），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [实现评审] assert_close 的 rtol/atol 如何按 dtype 选择才合理？
2. [实现评审] gradcheck 在什么情况下会误报失败？
3. [实现评审] 如何量化并治理 flaky 测试？

#### 8. 易错点与反例

1. [实现评审] 用 == 比较浮点张量
2. [实现评审] 以为关闭 cuDNN benchmark 就能完全确定
3. [实现评审] 把 golden 值硬编码却不记录环境版本

#### 9. 实践与验证

用最小反例/边界输入攻击实现，检查断言、日志与错误分类是否到位。具体到 测试治理：固定 seed 重跑 N 次统计通过率；用 assert_close 显式指定 rtol/atol 而非手写比较。把 对 fp16 用 fp32 阈值导致误报 作为负例，记录 torch.autograd.gradcheck 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“测试治理”的名词，说不出「相同 seed+相同算法下结果可复现」或 对 fp16 用 fp32 阈值导致误报。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 |a-b|≤atol+rtol×|b|；flake 率≈失败次数/重跑次数 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 未固定 seed 造成随机失败 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A095. 方案比较：张量/设备

- 难度：高级
- 知识域：K01 张量/设备
- 考察能力：不变量、二阶影响、证据缺口与演进路线
- 题型：综合开放/专家追问
- 面试频率：中频
- 前置知识：shape、stride、dtype、device
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

方案比较要在同一组硬约束下权衡正确性、性能、成本、运维与迁移代价。本题位于 K01 张量/设备。non_blocking=True 只有在源 pinned 且目标是加速器时才真正异步，否则退化为同步拷贝。

#### 2. 问题分析

从“方案比较：张量/设备”出发，先固定比较维度和工作负载，把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 torch.Tensor.view/reshape 的契约，列出两条不变量：「view 与源共享 storage（data_ptr 与 storage_offset 一致）」「张量的 device/dtype 决定 kernel 派发与是否发生隐式转换」，再给一个会击穿假设的边界输入，并说明它最先暴露 对非连续张量 view 报 'view size is not compatible with input tensor's size and stride' 还是 跨设备运算报 'Expected all tensors to be on the same device'。

#### 3. 具体答案

面试中可以这样回答：

> 我会给出维度对照、适用条件、反例，以及推荐方案和理由。在 张量/设备 上，关键事实是：连续张量满足 stride[i]=∏_{j>i}shape[j]，shape=(2,3,4) 的 stride 恰为 (12,4,1)。 判据可写为 显存(bytes)=numel×itemsize(dtype)；连续 stride[i]=∏_{j>i}shape[j]。

#### 4. 技术洞察

没有绝对最优，只有给定约束下的帕累托前沿。在 张量/设备 上，量化关系是 显存(bytes)=numel×itemsize(dtype)；连续 stride[i]=∏_{j>i}shape[j]；核心不变量是「view 与源共享 storage（data_ptr 与 storage_offset 一致）」，它一旦被打破，最先表现为 reshape 静默拷贝抬高峰值显存。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | torch.Tensor.view/reshape 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | view 与源共享 storage（data_ptr 与 storage_offset 一致） | 违反「张量的 device/dtype 决定 kernel 派发与是否发生隐式转换」时的报错 |
| 量化关系 | 显存(bytes)=numel×itemsize(dtype)；连续 stride[i]=∏_{j>i}shape[j] | 实测与预测偏差超容差 |
| 失败模式 | 对非连续张量 view 报 'view size is not compatible with input tensor's size and stride' | 跨设备运算报 'Expected all tensors to be on the same device' |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 张量/设备 在“方案比较”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“方案比较：张量/设备”沿相邻边界延伸：view/reshape/contiguous、pin_memory 与 stream、dtype 转换与显存。比较版本与替代实现的差异（如 t.to(device, non_blocking=True)），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [方案比较] shape=(2,3,4) 分别 view 成 (6,4)、reshape 成 (6,4) 后，stride 与是否共享存储各是什么？
2. [方案比较] 为什么 permute 之后必须先 contiguous 才能 view？
3. [方案比较] non_blocking=True 在源未 pinned 时的实际行为如何用 profiler 证明？

#### 8. 易错点与反例

1. [方案比较] 把 reshape 当零拷贝：stride 不兼容时它会 contiguous 并分配新显存
2. [方案比较] 只比较 shape 相等就判断可 view 互转，忽略 stride/storage_offset
3. [方案比较] 以为 pin_memory 总有效，实际它只对 non_blocking 有加速意义

#### 9. 实践与验证

在统一基准上用同一输入对比两方案的正确性、延迟、显存与迁移工作量。具体到 张量/设备：构造 (2,3,4) 后断言 t.stride()==(12,4,1) 且 t.is_contiguous()；用 t.data_ptr() 验证 view 与 reshape 的共享/拷贝差异。把 对非连续张量 view 报 'view size is not compatible with input tensor's size and stride' 作为负例，记录 t.stride()/t.is_contiguous() 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“张量/设备”的名词，说不出「view 与源共享 storage（data_ptr 与 storage_offset 一致）」或 对非连续张量 view 报 'view size is not compatible with input tensor's size and stride'。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 显存(bytes)=numel×itemsize(dtype)；连续 stride[i]=∏_{j>i}shape[j] 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 跨设备运算报 'Expected all tensors to be on the same device' 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A096. 方案比较：autograd

- 难度：高级
- 知识域：K02 autograd
- 考察能力：不变量、二阶影响、证据缺口与演进路线
- 题型：综合开放/专家追问
- 面试频率：中频
- 前置知识：requires_grad、leaf、view、detach、反向
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

方案比较要在同一组硬约束下权衡正确性、性能、成本、运维与迁移代价。本题位于 K02 autograd。param.data.add_() 绕过 autograd，是原地更新权重而不破坏图的标准做法。

#### 2. 问题分析

从“方案比较：autograd”出发，先固定比较维度和工作负载，把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 Tensor.requires_grad_ 的契约，列出两条不变量：「叶子张量 .grad 的形状与张量一致且只由叶子持有」「requires_grad 沿 grad_fn 链单向传播」，再给一个会击穿假设的边界输入，并说明它最先暴露 忘记 zero_grad 使梯度累加 还是 对 requires_grad 的叶子做原地操作报 'a leaf Variable that requires grad is being used in an in-place operation'。

#### 3. 具体答案

面试中可以这样回答：

> 我会给出维度对照、适用条件、反例，以及推荐方案和理由。在 autograd 上，关键事实是：只有 requires_grad=True 的叶子张量在反向时把梯度累加到 .grad，非叶子的 .grad 默认是 None。 判据可写为 累加语义 g←g+∂L/∂θ；链式法则 dL/dx=Σ(dL/dy·dy/dx)。

#### 4. 技术洞察

没有绝对最优，只有给定约束下的帕累托前沿。在 autograd 上，量化关系是 累加语义 g←g+∂L/∂θ；链式法则 dL/dx=Σ(dL/dy·dy/dx)；核心不变量是「叶子张量 .grad 的形状与张量一致且只由叶子持有」，它一旦被打破，最先表现为 两次 backward 未设 retain_graph 触发 RuntimeError。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | Tensor.requires_grad_ 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | 叶子张量 .grad 的形状与张量一致且只由叶子持有 | 违反「requires_grad 沿 grad_fn 链单向传播」时的报错 |
| 量化关系 | 累加语义 g←g+∂L/∂θ；链式法则 dL/dx=Σ(dL/dy·dy/dx) | 实测与预测偏差超容差 |
| 失败模式 | 忘记 zero_grad 使梯度累加 | 对 requires_grad 的叶子做原地操作报 'a leaf Variable that requires grad is being used in an in-place operation' |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 autograd 在“方案比较”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“方案比较：autograd”沿相邻边界延伸：detach、retain_grad、in-place 与版本计数、gradcheck。比较版本与替代实现的差异（如 torch.autograd.grad），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [方案比较] 为什么非叶子张量的 .grad 默认是 None，retain_grad() 改变了什么？
2. [方案比较] detach() 与 with torch.no_grad() 在切断梯度上的差别是什么？
3. [方案比较] in-place 操作在什么条件下能安全使用而不破坏 autograd？

#### 8. 易错点与反例

1. [方案比较] 认为非叶子 .grad 会自动填充，实则默认 None，需要 retain_grad()
2. [方案比较] 在 no_grad 下构建输入再 backward，得到 requires_grad 缺失错误
3. [方案比较] 用 param = param - lr*g 原地更新破坏计算图

#### 9. 实践与验证

在统一基准上用同一输入对比两方案的正确性、延迟、显存与迁移工作量。具体到 autograd：断言 x.is_leaf and x.grad is None 后再 backward；用 torch.autograd.grad(..., retain_graph=True) 做两次反向对比。把 忘记 zero_grad 使梯度累加 作为负例，记录 Tensor.detach() 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“autograd”的名词，说不出「叶子张量 .grad 的形状与张量一致且只由叶子持有」或 忘记 zero_grad 使梯度累加。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 累加语义 g←g+∂L/∂θ；链式法则 dL/dx=Σ(dL/dy·dy/dx) 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 对 requires_grad 的叶子做原地操作报 'a leaf Variable that requires grad is being used in an in-place operation' 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A097. 方案比较：Module/状态

- 难度：高级
- 知识域：K03 Module/状态
- 考察能力：不变量、二阶影响、证据缺口与演进路线
- 题型：综合开放/专家追问
- 面试频率：中频
- 前置知识：参数、buffer、模式和 state_dict
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

方案比较要在同一组硬约束下权衡正确性、性能、成本、运维与迁移代价。本题位于 K03 Module/状态。BatchNorm 的 running_mean/running_var 是 buffer，eval 下不再更新、只用统计量。

#### 2. 问题分析

从“方案比较：Module/状态”出发，先固定比较维度和工作负载，把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 register_buffer 的契约，列出两条不变量：「同一 Module 的 state_dict key 集合确定且与设备无关」「buffer 的 dtype/device 跟随 module 或按注册时指定」，再给一个会击穿假设的边界输入，并说明它最先暴露 自定义 buffer 未注册，.to(device) 后仍留在 CPU 还是 load_state_dict 因 missing/unexpected keys 失败。

#### 3. 具体答案

面试中可以这样回答：

> 我会给出维度对照、适用条件、反例，以及推荐方案和理由。在 Module/状态 上，关键事实是：buffer 参与 state_dict 但不是参数、不参与梯度；Parameter 两者都占。 判据可写为 state_dict={k:param}∪{k:buffer}；训练态显存≈Σ numel×itemsize×(1+grad+opt_state)。

#### 4. 技术洞察

没有绝对最优，只有给定约束下的帕累托前沿。在 Module/状态 上，量化关系是 state_dict={k:param}∪{k:buffer}；训练态显存≈Σ numel×itemsize×(1+grad+opt_state)；核心不变量是「同一 Module 的 state_dict key 集合确定且与设备无关」，它一旦被打破，最先表现为 eval 下忘记切换使 Dropout 仍在抽样。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | register_buffer 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | 同一 Module 的 state_dict key 集合确定且与设备无关 | 违反「buffer 的 dtype/device 跟随 module 或按注册时指定」时的报错 |
| 量化关系 | state_dict={k:param}∪{k:buffer}；训练态显存≈Σ numel×itemsize×(1+grad+opt_state) | 实测与预测偏差超容差 |
| 失败模式 | 自定义 buffer 未注册，.to(device) 后仍留在 CPU | load_state_dict 因 missing/unexpected keys 失败 |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 Module/状态 在“方案比较”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“方案比较：Module/状态”沿相邻边界延伸：persistent buffer、hook 与状态迁移、DDP 下的 buffer 同步。比较版本与替代实现的差异（如 state_dict/load_state_dict），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [方案比较] buffer 与 Parameter 在 state_dict、梯度和设备迁移上的差异是什么？
2. [方案比较] BatchNorm 在 eval 模式仍更新 running stats 需要什么额外设置？
3. [方案比较] register_buffer(persistent=False) 会从 state_dict 中移除什么？

#### 8. 易错点与反例

1. [方案比较] 把计数/统计量存成普通属性，导致 state_dict 丢失该状态
2. [方案比较] load_state_dict 后忘记 module.eval()，使 BN/Dropout 行为不符
3. [方案比较] 直接给同名属性赋值绕过注册，使 named_parameters 缺失

#### 9. 实践与验证

在统一基准上用同一输入对比两方案的正确性、延迟、显存与迁移工作量。具体到 Module/状态：断言 set(m.state_dict().keys()) 与 checkpoint 完全一致；对比 train/eval 两次前向输出验证 Dropout 关闭。把 自定义 buffer 未注册，.to(device) 后仍留在 CPU 作为负例，记录 register_parameter 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“Module/状态”的名词，说不出「同一 Module 的 state_dict key 集合确定且与设备无关」或 自定义 buffer 未注册，.to(device) 后仍留在 CPU。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 state_dict={k:param}∪{k:buffer}；训练态显存≈Σ numel×itemsize×(1+grad+opt_state) 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 load_state_dict 因 missing/unexpected keys 失败 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A098. 方案比较：数据管道

- 难度：高级
- 知识域：K04 数据管道
- 考察能力：不变量、二阶影响、证据缺口与演进路线
- 题型：综合开放/专家追问
- 面试频率：中频
- 前置知识：Dataset、Sampler、collate、padding、版本
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

方案比较要在同一组硬约束下权衡正确性、性能、成本、运维与迁移代价。本题位于 K04 数据管道。pin_memory=True 把 batch 放进页锁定内存，配合 non_blocking 才能与计算重叠。

#### 2. 问题分析

从“方案比较：数据管道”出发，先固定比较维度和工作负载，把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 DataLoader(num_workers, pin_memory, persistent_workers) 的契约，列出两条不变量：「shuffle=False 且种子固定时迭代顺序可复现」「各 rank 采样集合互斥且并集覆盖全集（或按 drop_last 截断）」，再给一个会击穿假设的边界输入，并说明它最先暴露 多 worker 共享同一随机源导致样本重复 还是 worker 数过多耗尽文件描述符或内存。

#### 3. 具体答案

面试中可以这样回答：

> 我会给出维度对照、适用条件、反例，以及推荐方案和理由。在 数据管道 上，关键事实是：num_workers=0 在主进程取数，>0 时 fork 子进程，每个 worker 有独立 Python 状态与 seed。 判据可写为 吞吐≤min(存储带宽, CPU 解码速率, H2D 带宽)；有效样本/步=batch×world_size。

#### 4. 技术洞察

没有绝对最优，只有给定约束下的帕累托前沿。在 数据管道 上，量化关系是 吞吐≤min(存储带宽, CPU 解码速率, H2D 带宽)；有效样本/步=batch×world_size；核心不变量是「shuffle=False 且种子固定时迭代顺序可复现」，它一旦被打破，最先表现为 DistributedSampler 与自定义 shuffle 双打乱造成跨 rank 重复。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | DataLoader(num_workers, pin_memory, persistent_workers) 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | shuffle=False 且种子固定时迭代顺序可复现 | 违反「各 rank 采样集合互斥且并集覆盖全集（或按 drop_last 截断）」时的报错 |
| 量化关系 | 吞吐≤min(存储带宽, CPU 解码速率, H2D 带宽)；有效样本/步=batch×world_size | 实测与预测偏差超容差 |
| 失败模式 | 多 worker 共享同一随机源导致样本重复 | worker 数过多耗尽文件描述符或内存 |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 数据管道 在“方案比较”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“方案比较：数据管道”沿相邻边界延伸：IterableDataset、chunk 预取、对象存储吞吐与 IPC 开销。比较版本与替代实现的差异（如 collate_fn），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [方案比较] DistributedSampler 如何处理样本数不被 world_size 整除，是否引入重复样本？
2. [方案比较] num_workers 增大时首步延迟、内存与 IPC 开销如何变化？
3. [方案比较] pin_memory 与 non_blocking 为何必须同时开启才有效？

#### 8. 易错点与反例

1. [方案比较] 把 num_workers 调大就以为更快，忽略 IPC 与内存放大
2. [方案比较] Dataset 里用全局随机而不经 worker_init_fn 重播种
3. [方案比较] 自己 shuffle 又用 DistributedSampler，导致跨 rank 样本重复

#### 9. 实践与验证

在统一基准上用同一输入对比两方案的正确性、延迟、显存与迁移工作量。具体到 数据管道：固定 seed 两次迭代断言顺序一致；统计各 rank 采样索引并集是否覆盖全集。把 多 worker 共享同一随机源导致样本重复 作为负例，记录 DistributedSampler 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“数据管道”的名词，说不出「shuffle=False 且种子固定时迭代顺序可复现」或 多 worker 共享同一随机源导致样本重复。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 吞吐≤min(存储带宽, CPU 解码速率, H2D 带宽)；有效样本/步=batch×world_size 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 worker 数过多耗尽文件描述符或内存 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A099. 方案比较：训练/优化器

- 难度：高级
- 知识域：K05 训练/优化器
- 考察能力：不变量、二阶影响、证据缺口与演进路线
- 题型：综合开放/专家追问
- 面试频率：中频
- 前置知识：forward、backward、累积、optimizer、scheduler
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

方案比较要在同一组硬约束下权衡正确性、性能、成本、运维与迁移代价。本题位于 K05 训练/优化器。Adam 每参数维护 exp_avg 与 exp_avg_sq，优化器状态约 2×参数量。

#### 2. 问题分析

从“方案比较：训练/优化器”出发，先固定比较维度和工作负载，把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 optimizer.zero_grad(set_to_none=True) 的契约，列出两条不变量：「反向之前梯度应为空或已清零」「scheduler 与 optimizer 的 step 次数对齐」，再给一个会击穿假设的边界输入，并说明它最先暴露 scheduler.step() 放错位置使 lr 序列整体偏移 还是 累积时直接累加 loss 未归一化。

#### 3. 具体答案

面试中可以这样回答：

> 我会给出维度对照、适用条件、反例，以及推荐方案和理由。在 训练/优化器 上，关键事实是：set_to_none=True 把 .grad 置 None 而非填零，省显存并让后续加法重新分配。 判据可写为 有效 batch=micro_batch×grad_accum×world_size；Adam 状态≈2P(状态)+P(梯度)。

#### 4. 技术洞察

没有绝对最优，只有给定约束下的帕累托前沿。在 训练/优化器 上，量化关系是 有效 batch=micro_batch×grad_accum×world_size；Adam 状态≈2P(状态)+P(梯度)；核心不变量是「反向之前梯度应为空或已清零」，它一旦被打破，最先表现为 zero_grad 漏调导致梯度累加。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | optimizer.zero_grad(set_to_none=True) 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | 反向之前梯度应为空或已清零 | 违反「scheduler 与 optimizer 的 step 次数对齐」时的报错 |
| 量化关系 | 有效 batch=micro_batch×grad_accum×world_size；Adam 状态≈2P(状态)+P(梯度) | 实测与预测偏差超容差 |
| 失败模式 | scheduler.step() 放错位置使 lr 序列整体偏移 | 累积时直接累加 loss 未归一化 |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 训练/优化器 在“方案比较”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“方案比较：训练/优化器”沿相邻边界延伸：scheduler 恢复、优化器 state_dict、foreach/fused 实现差异。比较版本与替代实现的差异（如 lr_scheduler.step()），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [方案比较] lr_scheduler.step() 在 PyTorch 1.1.0 前后位置的语义差异是什么？
2. [方案比较] 梯度累积如何在数学上等价于大 batch，需要哪些归一化？
3. [方案比较] set_to_none=True 对显存与优化器行为有何具体影响？

#### 8. 易错点与反例

1. [方案比较] 用 zero_grad() 不传 set_to_none，长期分配开销更高
2. [方案比较] 把 scheduler.step() 在 epoch 与 step 间混用，导致 lr 跳变
3. [方案比较] 梯度累积时未除 steps，使等效学习率偏大

#### 9. 实践与验证

在统一基准上用同一输入对比两方案的正确性、延迟、显存与迁移工作量。具体到 训练/优化器：打印前若干步 param_groups[0]['lr'] 验证调度曲线；比较累积 N 步与单步大 batch 的梯度 L2 范数。把 scheduler.step() 放错位置使 lr 序列整体偏移 作为负例，记录 optimizer.step() 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“训练/优化器”的名词，说不出「反向之前梯度应为空或已清零」或 scheduler.step() 放错位置使 lr 序列整体偏移。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 有效 batch=micro_batch×grad_accum×world_size；Adam 状态≈2P(状态)+P(梯度) 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 累积时直接累加 loss 未归一化 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
### A100. 方案比较：AMP/恢复

- 难度：高级
- 知识域：K06 AMP/恢复
- 考察能力：不变量、二阶影响、证据缺口与演进路线
- 题型：综合开放/专家追问
- 面试频率：中频
- 前置知识：autocast、scaler、快照、随机状态
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][推断][待验证]

#### 1. 背景知识

方案比较要在同一组硬约束下权衡正确性、性能、成本、运维与迁移代价。本题位于 K06 AMP/恢复。bf16 动态范围与 fp32 相同，通常无需 GradScaler；fp16 才需要 loss scaling。

#### 2. 问题分析

从“方案比较：AMP/恢复”出发，先固定比较维度和工作负载，把问题拆成输入契约、状态所有权、执行路径、失败模式四面。先固定 torch.autocast(device_type) 的契约，列出两条不变量：「反向前 loss 被 scale 放大，step 前梯度必须被 unscale」「inf/nan 检查与 skip 分支成对出现」，再给一个会击穿假设的边界输入，并说明它最先暴露 clip 放在 unscale 之前使梯度幅度错误 还是 对 bf16 仍用 GradScaler 造成无谓开销。

#### 3. 具体答案

面试中可以这样回答：

> 我会给出维度对照、适用条件、反例，以及推荐方案和理由。在 AMP/恢复 上，关键事实是：GradScaler 默认 init_scale=65536、growth_factor=2.0、backoff_factor=0.5、growth_interval=2000。 判据可写为 scale_{new}=scale×growth_factor（连续无溢出）或 scale/backoff_factor（检测到 inf/nan）。

#### 4. 技术洞察

没有绝对最优，只有给定约束下的帕累托前沿。在 AMP/恢复 上，量化关系是 scale_{new}=scale×growth_factor（连续无溢出）或 scale/backoff_factor（检测到 inf/nan）；核心不变量是「反向前 loss 被 scale 放大，step 前梯度必须被 unscale」，它一旦被打破，最先表现为 checkpoint 未保存 scaler.state_dict 使恢复后 scale 不连续。

#### 5. 图文说明

| 观测面 | 本题检查点 | 证据信号 |
|---|---|---|
| 接口/契约 | torch.autocast(device_type) 的输入契约 | 约束缺失或 dtype/device 不符 |
| 状态/不变量 | 反向前 loss 被 scale 放大，step 前梯度必须被 unscale | 违反「inf/nan 检查与 skip 分支成对出现」时的报错 |
| 量化关系 | scale_{new}=scale×growth_factor（连续无溢出）或 scale/backoff_factor（检测到 inf/nan） | 实测与预测偏差超容差 |
| 失败模式 | clip 放在 unscale 之前使梯度幅度错误 | 对 bf16 仍用 GradScaler 造成无谓开销 |

图示解释：沿控制流/数据流/资源流/证据流四条线，标注 AMP/恢复 在“方案比较”场景下的耦合点与可观测点。

#### 6. 拓展知识

把“方案比较：AMP/恢复”沿相邻边界延伸：autocast 的 op 白/黑名单、随机状态快照、断点续训一致性。比较版本与替代实现的差异（如 scaler.scale/step/update），记录迁移与恢复成本；所有未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. [方案比较] GradScaler 连续多少次无溢出才提高 scale，默认参数是多少？
2. [方案比较] 为什么 fp16 autocast 必须 unscale 之后才能裁剪梯度？
3. [方案比较] bf16 为什么通常不需要 GradScaler？

#### 8. 易错点与反例

1. [方案比较] 以为 autocast 会把参数也转 fp16（参数保持 fp32）
2. [方案比较] 用未 unscale 的梯度直接裁剪
3. [方案比较] 恢复训练时忘记保存/加载 scaler 状态

#### 9. 实践与验证

在统一基准上用同一输入对比两方案的正确性、延迟、显存与迁移工作量。具体到 AMP/恢复：记录 scaler.get_scale() 在若干 step 的轨迹；注入含 nan 的 loss 验证该 step 被跳过。把 clip 放在 unscale 之前使梯度幅度错误 作为负例，记录 GradScaler 的终态与资源计数后再判定。

#### 10. 评分标准

不合格：只能复述“AMP/恢复”的名词，说不出「反向前 loss 被 scale 放大，step 前梯度必须被 unscale」或 clip 放在 unscale 之前使梯度幅度错误。
合格：能给出基本机制、适用条件和一个具体反例。
良好：能用 scale_{new}=scale×growth_factor（连续无溢出）或 scale/backoff_factor（检测到 inf/nan） 做量化取舍，并给出可执行的验证 oracle。
优秀/专家：能覆盖 对 bf16 仍用 GradScaler 造成无谓开销 的恢复、灰度回滚、版本演进与残余风险，并给出停止门槛。
