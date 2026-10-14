# 入门级 PyTorch 面试题

本文件包含 `B001`–`B100`，共 100 题；统一十类题型各 10 题，GPU、通信和性能结果未经目标环境运行时标为 [待验证]。

### B001. Tensor 与 Python 数组有什么区别？

- 难度：入门级
- 知识域：K01 张量、dtype、device 与内存布局
- 考察能力：概念边界、适用条件与反例
- 题型：概念解释与辨析
- 面试频率：高频
- 前置知识：Python 数组、线性代数中的向量和矩阵
- 关联题目：B002、B003、I001
- 事实状态：[已确认]

#### 1. 背景知识

`torch.Tensor` 是 PyTorch 的多维数组抽象，由一块连续 storage 加上 shape、stride、storage_offset、dtype、device 等元数据组成；Python `list` 是对象容器，NumPy `ndarray` 是带 shape/stride/dtype 的数组但没有设备与 autograd 概念。Tensor 与 NumPy 可以通过 `torch.from_numpy`、`Tensor.numpy()` 零拷贝共享内存，这也决定了它继承了底层 buffer 的生命周期风险。这里讨论的边界是内存共享：区分“表达同一数学对象”与“共享同一块存储”。

#### 2. 问题分析

按四个正交维度拆解：数据存放位置（storage 所有权）、元数据（shape/stride/dtype/device）、数学语义（广播、矩阵乘等运算集）、系统语义（autograd、kernel 调度、跨设备搬运）。再列出两个不变量：一个 Tensor 的 `numel == shape` 各维乘积；连续 Tensor 满足 `numel(shape) * itemsize == storage 字节数`。边界反例是 Python int 列表元素不是同质类型、不能直接做逐元素算子。

#### 3. 具体答案

面试中可以这样回答：
> PyTorch Tensor 与 Python list 的根本差别有四点。第一，Tensor 是同质、定长、定 dtype 的多维数组，底层是一段带 stride 的 storage，而 list 是异构的对象引用数组。第二，Tensor 有 device 概念，能在 CPU 和 CUDA 等设备间搬运并由 kernel 调度，list 只是普通 Python 对象。第三，Tensor 能参与 autograd，携带 `requires_grad` 和 `grad_fn`，这是 list 没有的。第四，Tensor 和 NumPy ndarray 可以零拷贝互转：`torch.from_numpy(a)` 与 `x.numpy()` 共享同一块内存，改一个另一个会变，只有 `.clone()` 才真正复制。相比 NumPy，Tensor 多出的正是 device 与 autograd 这两层能力。

#### 4. 技术洞察

核心不变量：`Tensor = storage + storage_offset + shape + stride + dtype + device`。连续张量占用字节 `numel * itemsize`，其中 `numel = prod(shape)`。逻辑索引到存储索引的映射为 `offset = storage_offset + sum(i_k * stride_k)`。因此 `from_numpy`/`numpy()` 是零拷贝的“元数据包装”，而 `.clone()` 复制 storage，这两者可验证的差别是 `data_ptr()` 是否相同。

#### 5. 图文说明

| 维度 | torch.Tensor | Python list | NumPy ndarray |
|---|---|---|---|
| 元素类型 | 同质、固定 dtype | 任意对象 | 同质、固定 dtype |
| 设备 | CPU/CUDA 等 | 无 | 仅主机内存 |
| autograd | 支持 | 不支持 | 不支持 |
| 零拷贝互转 | 与 ndarray 共享内存 | 不适用 | 与 Tensor 共享内存 |

图示解释：把 Tensor 看成“storage 数据流 + 元数据控制流 + device/autograd 旁路”，list 只有对象引用这一层。

#### 6. 拓展知识

沿 K01 边界继续比较：`from_numpy` 只接受受支持的 dtype（如 `bool/int8..int64/float16..float64`），共享内存时 Tensor 与数组任一被释放都可能悬空；`torch.as_tensor` 会根据来源决定是否复制；`Tensor.item()` 只对单元素有效。版本上 `torch.from_numpy` 的零拷贝语义长期稳定，但首次引入 autograd 的 `numpy()` 互转约束需按目标版本复核，未运行结论保留 `[待验证]`。

#### 7. 常见追问及回答要点

1. 为什么 `torch.from_numpy` 能零拷贝，而 `torch.tensor(np_array)` 通常不能？——前者包装现有 buffer，后者会复制。
2. 共享内存时，如果原 ndarray 被 `del` 或重建，Tensor 会怎样？——取决于底层 buffer 是否仍被引用，引用计数归零后访问是未定义/悬空风险。
3. Tensor 与 ndarray 的 dtype 支持范围差异会导致什么后果？——某些 dtype 无法零拷贝互转，会报错或强制复制。

#### 8. 易错点与反例

1. 认为 `Tensor.numpy()` 一定安全：若 Tensor 在 GPU 上，`numpy()` 会直接报 `TypeError`，必须先 `.cpu()`。
2. 忘记共享内存：`x = torch.from_numpy(a); x += 1` 后误以为原数组 `a` 没变，实际已被修改。
3. 用 list 元素逐元素运算代替 Tensor：`[1,2]+[3,4]` 是拼接而非逐元素相加，语义与 Tensor 完全不同。

#### 9. 实践与验证

在目标环境执行最小脚本：`a = np.arange(4, dtype=np.float32); x = torch.from_numpy(a); print(x.data_ptr() == a.ctypes.data)` 应为 True，`x.clone().data_ptr()` 应不同；再对 `x += 1` 检查 `a` 是否改变。oracle 是“共享内存的指针相等、克隆后不等，且原地运算对源数组可见”。未运行结论标注 `[待验证]`。

#### 10. 评分标准

B001 评分：不合格只会说“Tensor 是张量、list 是列表”；合格能说出同质性、dtype/device、autograd 三点差别；良好能补充零拷贝互转及 `data_ptr` 验证；优秀能给出 `storage+stride+offset` 的完整模型并指出内存生命周期与悬空引用风险。
### B002. `view`、`reshape` 和 `contiguous` 有什么区别？

- 难度：入门级
- 知识域：K01 张量、dtype、device 与内存布局
- 考察能力：概念边界、适用条件与反例
- 题型：概念解释与辨析
- 面试频率：高频
- 前置知识：shape、stride、连续内存
- 关联题目：B001、I005、A003
- 事实状态：[已确认]

#### 1. 背景知识

转置、`transpose`、切片、`expand` 等操作只改变 stride/offset 等元数据，产生非连续视图。`view` 要求目标形状与现有 stride 兼容，否则抛 `RuntimeError`；`reshape` 在兼容时返回视图、否则自动拷贝；`contiguous` 在非连续时显式复制成连续布局。三者都保持 `numel` 与逻辑元素顺序（reshape 语义按行优先展开）。

#### 2. 问题分析

先明确不变量：只要是纯视图操作，`data_ptr()` 不变、修改互相可见；一旦发生拷贝，`data_ptr()` 改变、两者分离。分析步骤：写一个 `t()` 转置张量，分别尝试 `view`、`reshape`、`contiguous().view`，比较结果与 `data_ptr`。边界是 `expand` 出 stride=0 的维度，`view` 一定失败。

#### 3. 具体答案

面试中可以这样回答：
> 三者都用于改变形状，但拷贝语义不同。`view` 只做 stride 重解释，要求张量在内存里对该形状是连续的，非连续时直接报 `RuntimeError`。`reshape` 是“能视图就视图、不能就复制”，所以几乎总成功，但你可能悄悄付出一次拷贝。`contiguous` 不改变逻辑内容，只在需要时把 storage 复制成连续布局，返回的 Tensor 一定 `is_contiguous()==True`。典型例子：`x = torch.arange(12).view(3,4); y = x.t(); y.view(12)` 报错，因为 `y` 非连续；`y.reshape(12)` 成功（内部拷贝）；`y.contiguous().view(12)` 也成功。要判断是否发生拷贝，比较 `data_ptr()`。

#### 4. 技术洞察

不变量一：纯视图操作保持 `data_ptr` 与 `storage_offset` 的相对关系，`numel` 不变。不变量二：`view` 成功的充要条件是目标形状能由现有 stride 表示，即对目标形状存在一组“递减且可分”的 stride；转置后 stride 递减关系被破坏，故失败。示意：`(3,4)` 连续张量 stride `(4,1)`，转置后 `(1,4)`，无法再解释成 stride 递减的 `(12,)`。

#### 5. 图文说明

| 操作 | 是否可能拷贝 | 非连续时行为 | 典型失败信号 |
|---|---|---|---|
| `view` | 否 | 报错 | `RuntimeError: view size is not compatible` |
| `reshape` | 是（按需） | 自动复制 | 无异常但 `data_ptr` 改变 |
| `contiguous` | 是（按需） | 复制为连续 | 无异常但内存翻倍 |

图示解释：数据流上把“元数据重解释”和“storage 复制”画成两条分支，view 走重解释分支、非连续时断开，reshape/contiguous 可回退到复制分支。

#### 6. 拓展知识

继续对比 `transpose`/`permute`（改 stride，纯视图）与 `flatten`（可能拷贝）；`expand` 用 stride=0 实现零拷贝广播但不分配内存，`repeat` 会复制。与 `torch.compile`/推理部署交叉时，非连续张量可能触发额外 kernel 或影响 ONNX 导出，具体以目标版本为准，未运行内容标 `[待验证]`。

#### 7. 常见追问及回答要点

1. `reshape` 什么时候会拷贝、怎么证明？——`data_ptr()` 改变或对来源修改不可见即发生拷贝。
2. 为什么 `x.t().view(-1)` 会失败，而 `x.t().reshape(-1)` 不会？——前者要求连续 stride，后者允许内部复制。
3. `contiguous()` 对已连续张量会二次拷贝吗？——不会，直接返回自身（`data_ptr` 不变）。

#### 8. 易错点与反例

1. 在训练循环里反复对非连续张量 `reshape`，以为免费，实际每次拷贝拖慢并多占显存。
2. 把 `view` 用在 `expand`/转置结果上，运行时报错却误判为“shape 写错”。
3. 依赖 `y = x.t()` 后再 `y.view(...)` 的别名关系做原地写，结果因中间发生拷贝而改不到原张量。

#### 9. 实践与验证

脚本：构造 `x = torch.arange(12).view(3,4)`，记录 `x.t().is_contiguous()`（False）、尝试 `x.t().view(12)`（捕获异常）、`r = x.t().reshape(12)` 比较 `r.data_ptr() != x.data_ptr()`。oracle 为“view 抛错、reshape 成功且发生拷贝、contiguous 后 is_contiguous 为真”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B002 评分：不合格只背 API 名；合格能说清 view 需连续、reshape 按需拷贝、contiguous 强制连续；良好能用 `data_ptr` 证明拷贝并给出转置反例；优秀能解释 stride 可表示性与零拷贝边界（如 stride=0 的 expand）。
### B003. `requires_grad`、leaf Tensor 和 `backward()` 如何协作？

- 难度：入门级
- 知识域：K02 autograd、计算图与梯度
- 考察能力：概念边界、适用条件与反例
- 题型：概念解释与辨析
- 面试频率：高频
- 前置知识：函数求导、Tensor 基础
- 关联题目：I002、E001
- 事实状态：[已确认]

#### 1. 背景知识

autograd 在前向时记录可微操作形成反向图，每个非叶子结果带 `grad_fn`。`requires_grad=True` 且由用户直接创建（无 `grad_fn`）的张量是 leaf Tensor，其 `.grad` 会被 `backward()` 累积。`backward()` 从标量损失出发按链式法则回传，默认释放中间图（`retain_graph=False`），需要多次回传则设 `retain_graph=True`。非叶子的 `.grad` 默认不保留，除非 `retain_grad()`。

#### 2. 问题分析

分析链条：谁决定 `requires_grad`（叶子或任一输入的 requires_grad 传播）、梯度存到哪（叶子的 `.grad`）、图何时释放（一次 backward 后）。不变量：`backward()` 默认只对单元素标量张量或提供 `gradient` 参数才可调用；梯度累积语义意味着循环中必须 `zero_grad()`。边界反例：对非标量直接 `backward()` 会要求 `gradient`。

#### 3. 具体答案

面试中可以这样回答：
> 三者的协作是：用户创建的、`requires_grad=True` 的张量是 leaf，前向中所有可微算子会把它和结果串成一张图，非叶子结果带 `grad_fn`。调用 `loss.backward()` 时，autograd 按链式法则从 `loss` 往叶子上回传，并把梯度累加进各叶子张量的 `.grad`。也就是说 `requires_grad` 决定“参不参与”，leaf 决定“梯度落在哪”，`backward()` 决定“什么时候算、往哪放”。默认图只回传一次就释放，循环训练里必须每步 `optimizer.zero_grad()`，否则梯度会在 `.grad` 上累加。若要在非叶子中间量上看梯度，需对其调用 `retain_grad()`。

#### 4. 技术洞察

不变量：对叶子 `w`，`w.grad` 是历史累加值，`w.grad = sum over backward calls of dL/dw`。链式法则给出 `dL/dx = sum_j (dL/dy_j) * (dy_j/dx)`。一次性图释放意味着第二次 `backward()` 若无 `retain_graph=True` 会报 `Trying to backward through the graph a second time`。梯度存在性与 `requires_grad` 一致：`w.grad is None` 说明未参与或未回传。

#### 5. 图文说明

| 概念 | 判定条件 | 梯度去向 | 常见错误 |
|---|---|---|---|
| requires_grad | 显式设置或由输入传播 | 决定是否建图 | 以为数据张量默认需要梯度 |
| leaf Tensor | `requires_grad=True` 且 `grad_fn is None` | 梯度写入 `.grad` | 把中间结果当叶子 |
| backward() | 标量或给 `gradient` | 触发回传并默认释放图 | 循环中忘记 `zero_grad` |

图示解释：数据流上是“输入→算子→loss”的 DAG，证据流上是叶子的 `.grad` 累积桶与图的生命周期。

#### 6. 拓展知识

延伸比较 `detach()`（切断图但共享数据）、`torch.no_grad()`（不建图）、`retain_grad`、二阶导 `create_graph=True` 与 `torch.autograd.grad`。与分布式交叉时 `DDP` 依赖 `backward()` 触发梯度 bucket 通信，未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 为什么 `optimizer.zero_grad()` 必须存在？——`.grad` 是累加的，不清零会把上一步梯度叠加进来。
2. `x.detach()` 与 `x.data` 有何不同？——`detach` 返回共享数据但不参与图的新张量，`x.data` 绕过版本检查不安全。
3. 怎么让非叶子中间量也有梯度？——回传前调用 `t.retain_grad()`。

#### 8. 易错点与反例

1. 训练循环少了 `zero_grad()`，loss 看似下降其实是梯度被累加放大，等价改变学习率。
2. 对非标量张量直接 `backward()`，报 `grad can be implicitly created only for scalar outputs`。
3. 想复用同一张图做两次回传（如两段 loss），未设 `retain_graph=True` 导致第二次报错。

#### 9. 实践与验证

脚本：`w = torch.tensor([2.0], requires_grad=True); x = w*3; y = x.sum(); y.backward(retain_graph=True); print(w.grad)` 得到 3；再 `y.backward(); print(w.grad)` 得到 6 验证累加；`print(x.grad)` 为 None，`x.retain_grad()` 后重试可得。oracle 为“叶子 .grad 累积、非叶子默认 None、二次回传需 retain_graph”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B003 评分：不合格混用 requires_grad 与 leaf；合格能区分三概念并说明梯度落点；良好能演示累加与图释放；优秀能补充 detach/retain_grad/二阶导与分布式触发等边界。
### B004. `nn.Module`、Parameter、buffer 和 `state_dict` 的关系是什么？

- 难度：入门级
- 知识域：K03 `nn.Module`、参数、buffer 与状态
- 考察能力：概念边界、适用条件与反例
- 题型：概念解释与辨析
- 面试频率：高频
- 前置知识：Python 类、Tensor、优化器
- 关联题目：I001、A004
- 事实状态：[已确认]

#### 1. 背景知识

`nn.Module` 是网络容器，负责注册子模块、参数和 buffer。`nn.Parameter` 是 Tensor 子类，注册后出现在 `module.parameters()`、被优化器更新并进入 `state_dict`。`register_buffer` 注册的持久 buffer 进入 `state_dict` 但不参与梯度。`state_dict` 是从参数/持久 buffer 名字到 Tensor 的字典，常用于保存和加载权重；`load_state_dict` 默认 `strict=True`，键名或形状不匹配会报错。

#### 2. 问题分析

沿所有权追踪：参数属于 `_parameters`，buffer 属于 `_buffers`，子模块属于 `_modules`，三者共同决定 `state_dict` 的键。不变量：`state_dict` 的键是“层级路径 + 名字”，`parameters()` 只含 `Parameter`，`buffers()` 只含 buffer。边界：`requires_grad=False` 的 Parameter 仍是参数、仍在校验和移动设备范围内，但不会被优化器当成可训练项（是否更新取决于优化器传参）。

#### 3. 具体答案

面试中可以这样回答：
> `nn.Module` 是状态与计算的容器。你赋给模块的可学习张量如果包成 `nn.Parameter`，就进入 `_parameters`，出现在 `parameters()` 里、被优化器更新、并写入 `state_dict`；用 `register_buffer` 注册的统计量（如 BatchNorm 的 running_mean）进入 `_buffers`，写进 `state_dict` 但不求梯度、不被优化器更新；网络结构本身（子模块）进入 `_modules`。`state_dict` 就是这三类对象的“名字→张量”快照，加载时按名字对齐，`strict=False` 可允许缺失/多余键。`module.to(device)` 会递归移动参数与 buffer。要冻结某层，通常设 `param.requires_grad=False` 并在优化器里过滤。

#### 4. 技术洞察

不变量：`set(state_dict().keys())` 覆盖所有 Parameter 与持久 buffer（`persistent=True`）；非持久 buffer 不进 `state_dict`。参数更新等价于 `p <- p - lr * p.grad`（SGD），因此未进 `parameters()` 的张量不会被更新。`load_state_dict` 的匹配是按 `name` 与 `shape` 双重校验，形状不符报 `size mismatch`。

#### 5. 图文说明

| 对象 | 注册方式 | 进 parameters() | 进 state_dict | 被优化器更新 |
|---|---|---|---|---|
| Parameter | `self.p = nn.Parameter(...)` | 是 | 是 | 是（若传入优化器） |
| 持久 buffer | `register_buffer(..., persistent=True)` | 否 | 是 | 否 |
| 非持久 buffer | `register_buffer(..., persistent=False)` | 否 | 否 | 否 |

图示解释：把 Module 看成一棵注册树，控制流是层级命名，数据流是参数/buffer 的读写与搬运。

#### 6. 拓展知识

延伸比较 `named_parameters()`、`modules()`、`children()`；`state_dict` 与 `torch.save(model)` 的取舍（前者可移植、后者序列化类）；与 DDP/FSDP 交叉时 `state_dict` 的键会带 `module.` 前缀、FSDP 需 `full_state_dict`/`state_dict_type` 控制。未运行内容标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 为什么推荐保存 `state_dict` 而不是整个模型对象？——与类定义解耦、可移植、避免 pickle 绑定。
2. `requires_grad=False` 的 Parameter 还算 Parameter 吗？——算，仍在 `state_dict` 与设备移动范围内，只是不更新。
3. 加载时出现 unexpected/missing keys 怎么定位？——比对 `state_dict().keys()` 差异，注意 DDP 的 `module.` 前缀。

#### 8. 易错点与反例

1. 直接把 Tensor 赋给模块属性（未包 Parameter），它不会出现在 `parameters()`，优化器静默不更新。
2. 忘记 `state_dict` 里的 buffer：换了设备或重训后 running stats 不一致导致推理漂移。
3. 多卡训练保存带 `module.` 前缀，单卡加载严格模式报 missing/unexpected keys。

#### 9. 实践与验证

脚本：定义含 Parameter、持久 buffer、普通 Tensor 的模块，打印 `named_parameters()` 与 `state_dict().keys()`，验证普通 Tensor 不在其中；`load_state_dict` 用错误 shape 触发 `size mismatch`；`model.to('cpu')` 后确认 buffer 与参数 device 一致。oracle 为“三类对象归属与 state_dict 键集合符合预期”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B004 评分：不合格混淆参数与 buffer；合格能说清注册归属与 state_dict 组成；良好能说明 load 校验与冻结机制；优秀能补充非持久 buffer、DDP/FSDP 键前缀等工程边界。
### B005. Dataset、DataLoader、`collate_fn` 和 pinned memory 如何协作？

- 难度：入门级
- 知识域：K04 数据管道与输入规范
- 考察能力：概念边界、适用条件与反例
- 题型：概念解释与辨析
- 面试频率：高频
- 前置知识：Python 迭代器、batch 和 Tensor
- 关联题目：I001、I005、A003
- 事实状态：[已确认]

#### 1. 背景知识

`Dataset` 定义样本取用接口（`__getitem__`/`__len__`），`DataLoader` 负责采样、批处理与多进程加载。`collate_fn` 把一批样本（含可能变长的序列）整理成张量或结构；默认 `default_collate` 会对同形状张量做 `stack`。`pin_memory=True` 把 batch 放进页锁定（page-locked）主机内存，配合 `tensor.to(device, non_blocking=True)` 可让 H2D 拷贝与计算异步重叠。

#### 2. 问题分析

按数据流拆解：采样器给出索引→worker 调用 `__getitem__`→`collate_fn` 组 batch→主进程搬运到设备。不变量：同一 batch 的所有张量在各维必须可 stack（形状一致或 collate 自定义）。边界：`num_workers>0` 时 worker 进程有独立随机状态、且 `pin_memory` 推荐只在 worker 内产生张量；Windows/spawn 下 `__getitem__` 必须可 pickle。

#### 3. 具体答案

面试中可以这样回答：
> Dataset 只负责“按索引/迭代给一个样本”，DataLoader 负责采样、分批和并行加载。`collate_fn` 决定“一批样本如何拼成一个 batch”——默认实现会把等形状张量沿新维 stack，变长序列就需要自定义（如 pad 或返回 list）。`pin_memory=True` 把 batch 放到页锁定内存，这样 `.to(cuda, non_blocking=True)` 的 H2D 拷贝能真正异步，与 GPU 计算重叠，否则可能同步阻塞。经验上 `num_workers` 取可用 CPU 核数的一部分、`pin_memory=True` 且 `non_blocking=True` 才有收益；`persistent_workers` 可避免每个 epoch 重建 worker。

#### 4. 技术洞察

核心不变量：稳态吞吐受最慢环节限制，`T_step ≥ max(数据端时间, GPU 计算时间)`，当数据端时间更小时 GPU 满负荷。吞吐模型：数据管道每步时间约 `max(worker 取数时间/ num_workers, GPU 计算时间) + 剩余同步开销`；若 `worker_time/num_workers > compute_time`，GPU 会“饿死”（utilization 掉、`nvidia-smi` 出现空档）。pin_memory 的作用是把每 batch 一次同步拷贝变为可重叠的异步拷贝，代价是页锁定内存占用。可验证信号：`DataLoader` worker 数从 0 增到 N 时迭代延迟的变化、Profiler 中 `cudaMemcpyAsync` 与 kernel 的重叠度。

#### 5. 图文说明

| 阶段 | 责任方 | 关键参数 | 失败信号 |
|---|---|---|---|
| 取样本 | Dataset | `__getitem__` | 慢/抛异常导致 worker 重启 |
| 组 batch | collate_fn | 默认 stack / 自定义 | 形状不一致报错 |
| 并行加载 | DataLoader | num_workers、prefetch_factor | GPU 空转、CPU 打满 |
| 异步搬运 | pin_memory + non_blocking | pin_memory=True | H2D 同步阻塞 |

图示解释：数据流从磁盘→worker→主进程 pin 内存→GPU，控制流由 worker 数与预取深度调节，证据流看 GPU 利用率与 H2D 重叠。

#### 6. 拓展知识

延伸比较 `IterableDataset`（流式、需 `worker_init_fn` 分片）、`Sampler`、`drop_last`、`prefetch_factor`、`persistent_workers`；与 AMP/DDP 交叉时每个 rank 的 sampler 需按 rank 分片避免重复样本。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. `num_workers` 是不是越大越好？——否，受限于 CPU 核数、磁盘和进程/IPC 开销，过大反而抖动。
2. 变长序列为什么默认 collate 会失败？——默认要求等形状才能 stack，需自定义 pad。
3. 多进程下如何保证每个 worker 数据不重复？——用 `worker_init_fn`+`get_worker_info` 分片，或 `DistributedSampler` 按 rank 分。

#### 8. 易错点与反例

1. 设了 `pin_memory=True` 却没写 `non_blocking=True`，异步收益为零。
2. `num_workers>0` 时在 `__getitem__` 里用全局随机数，导致所有 worker 产出相同增强。
3. 在数据集里返回未清理的文件句柄/大对象，worker 数量一多就内存或 fd 耗尽。

#### 9. 实践与验证

脚本：构造一个耗时 `__getitem__`（`time.sleep`）的 Dataset，比较 `num_workers` 0/2/4 下每 epoch 墙钟时间与 GPU 空转比例；打印一个 batch 的 `pin_memory` 张量 `is_pinned()` 应为 True；用自定义 `collate_fn` 处理变长序列验证不再报错。oracle 为“吞吐随 worker 提升到拐点、pin 生效、变长可 batch”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B005 评分：不合格只会说“DataLoader 加载数据”；合格能说清 Dataset/collate/pin 的分工；良好能推出吞吐模型并指出 GPU 饿死条件；优秀能结合 worker 分片、persistent_workers 与分布式采样给出工程取舍。
### B006. Tensor 广播规则是什么，什么时候会产生错误结果？

- 难度：入门级
- 知识域：K01 张量、dtype、device 与内存布局
- 考察能力：概念边界、适用条件与反例
- 题型：概念解释与辨析
- 面试频率：高频
- 前置知识：shape、维度和逐元素运算
- 关联题目：B001、I007、A003
- 事实状态：[已确认]

#### 1. 背景知识

广播规则：从末尾维度对齐，两维相等、或其中一维为 1（可为缺失维度补 1），则结果维取较大者；否则报 `size mismatch`。广播不复制数据，而是通过 stride=0 的视图实现。`expand` 显式广播，二元算子自动广播。`.expand` 与隐式广播在反向传播时会对被广播维度求和（gradient sum-reduction）。

#### 2. 问题分析

先用“从右向左、逐维判定相等或 1”手推结果形状，再检查是否与预期一致。不安全场景是“两个非 1 维本不该对齐却恰好相等”，例如 `(N,1)` 与 `(N,)` 相加会广播成 `(N,N)` 而不报错。不变量：广播后结果维 = max，且参与广播的维度总元素数不变；反向时对广播维做求和。

#### 3. 具体答案

面试中可以这样回答：
> 广播从最右维开始逐维比较：相等就保留，有一个为 1（或维度缺失）就扩展为另一个，否则报错。它对“形状不匹配”很宽容，所以危险在于静默产生错误结果。典型陷阱是把 `(N,)` 的标签广播到 `(N,1)` 的预测上，得到 `(N,N)` 的误差矩阵，数值不报错但语义完全错。还有原地操作 `x += y` 时，广播展宽的目标比 `x` 大，会报 “output with shape ... doesn't match the broadcast shape”。判断方法：先手算结果形状，再用 `expand` 或显式 `unsqueeze`/`sum(dim)` 把形状写清楚，避免依赖隐式广播。

#### 4. 技术洞察

不变量：结果形状 `out_i = max(a_i, b_i)`，其中维度按末尾对齐、缺失记 1，且要求逐维 `a_i==b_i 或 a_i==1 或 b_i==1`。广播用 `stride=0` 表示被扩展维，因此不额外分配内存；反向传播对广播前的维度做 reduce-sum，即 `dL/dx = sum(dL/dy over broadcast dims)`。可验证：`torch.broadcast_shapes((3,1),(1,4)) == (3,4)`。

#### 5. 图文说明

| 情形 | A 形状 | B 形状 | 结果 | 说明 |
|---|---|---|---|---|
| 正常 | (3,4) | (4,) | (3,4) | B 右对齐扩到行 |
| 危险 | (N,1) | (N,) | (N,N) | 语义变矩阵，静默错误 |
| 报错 | (3,4) | (2,) | 错误 | 4≠2 且都非 1 |

图示解释：把广播画成 stride=0 的“影子维”，控制流是逐维判定，数据流是逻辑扩展而非物理复制。

#### 6. 拓展知识

延伸比较 `expand`（零拷贝广播）与 `repeat`（复制）、`broadcast_tensors`、`torch.broadcast_shapes`；训练中 loss 形状约定（`(N,)` vs `(N,1)`）与 `.squeeze(-1)` 的关系；与混合精度/编译交叉时广播维 reduction 的数值影响。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 广播会给被扩展维分配内存吗？——不会，用 stride=0 实现，`expand` 后 `data_ptr` 不变。
2. 反向传播对被广播维做什么？——对该维做求和（reduce-sum）得到原形状梯度。
3. 为什么 `x += y` 有时报广播错误？——原地操作要求结果形状等于 `x`，广播展宽后不自洽。

#### 8. 易错点与反例

1. 把 `(N,1)` 的逐样本损失和 `(N,)` 的权重相加，得到 `(N,N)` 后 `.mean()` 语义全错。
2. 依赖隐式广播替代显式 `unsqueeze`，代码可读性差且重构时静默改变形状。
3. 在 `(C,H,W)` 图像上加 `(H,W)` 掩码忘了加通道维，意外广播到 batch 维。

#### 9. 实践与验证

脚本：`assert torch.broadcast_shapes((3,1),(1,4))==(3,4)`；`x=torch.randn(5,1); y=torch.randn(5); print((x+y).shape)` 应观察 `(5,5)` 并触发告警性检查；`e = torch.ones(1,4).expand(3,4)` 验证 `e.stride()` 含 0。oracle 为“可预测形状、危险案例被显式识别”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B006 评分：不合格只说“自动扩展”；合格能手推右对齐规则；良好能给出 `(N,1)+(N,)` 静默错误反例与 stride=0 机制；优秀能补充反向 reduce-sum 与原地广播约束。
### B007. dtype 转换如何影响精度、梯度和内存？

- 难度：入门级
- 知识域：K01 张量、dtype、device 与内存布局
- 考察能力：概念边界、适用条件与反例
- 题型：概念解释与辨析
- 面试频率：高频
- 前置知识：浮点数、Tensor dtype、训练循环
- 关联题目：I003、I005、A003
- 事实状态：[已确认]

#### 1. 背景知识

dtype 决定每元素位宽、可表示范围与精度。常见：`float32`(4B)、`float16`(2B, 最大约 65504)、`bfloat16`(2B, 8 位指数)、`float64`(8B)、`int8/16/32/64`、`bool`(1B)。`.to(dtype)` 默认复制并转换，整数转换向零截断；张量运算有类型提升（type promotion）规则。autocast 会自动为部分算子选择低精度、其余保持 float32。

#### 2. 问题分析

沿三条影响线：精度（舍入/溢出）、梯度（低精度梯度、累加误差）、内存（itemsize 与带宽）。不变量：转换后的数值范围与可表示精度由目标 dtype 决定；`float64→float32` 有信息损失且不可逆；`sum`/`mean` 在 float16 上累加易溢出或丢精度。边界反例：大整数超 `int32` 范围溢出，`float16` 表示 `1e5` 变 `inf`。

#### 3. 具体答案

面试中可以这样回答：
> dtype 转换同时影响精度、梯度和显存。精度上，从高精度到低精度是有损的：`float64→float32` 会舍入，`float32→float16` 还可能在超过约 65504 时变 `inf`，小梯度则可能下溢为 0，所以训练通常用 `bfloat16`（指数范围同 float32）或在 AMP 下保留 float32 主权重。内存上，float16/bfloat16 每元素 2 字节、float32 是 4 字节，直接减半显存与带宽。整数转换向零截断（`int(3.9)==3`），且 `int8` 会溢出回绕。梯度方面，低精度累加会丢精度，所以归约（sum/mean）常需升到 float32。实践原则：模型计算可用 bf16/fp16，但 loss 累加、优化器状态和 master weights 保持 float32。

#### 4. 技术洞察

核心不变量：`bytes = numel * itemsize`，且可表示范围与精度由目标 dtype 决定。内存/带宽关系：张量字节数 `= numel * itemsize`；float16→float32 使显存与内存带宽需求翻倍。表示范围：float16 最大约 `65504`、最小正规数约 `6.1e-5`；bfloat16 与 float32 同为 8 位指数，动态范围一致但尾数仅 8 位。类型提升规则可验证：`(torch.ones(1,dtype=torch.float16)+torch.ones(1,dtype=torch.float32)).dtype == torch.float32`。整数溢出是可验证反例。

#### 5. 图文说明

| dtype | 字节 | 指数位 | 典型用途 | 反例 |
|---|---|---|---|---|
| float32 | 4 | 8 | 主权重、loss 累加 | 显存翻倍 |
| float16 | 2 | 5 | AMP 前向 | >65504 变 inf |
| bfloat16 | 2 | 8 | AMP 前向（大动态范围） | 尾数少，精度低 |
| int8 | 1 | - | 量化存储 | 溢出回绕 |

图示解释：把 dtype 画成“位宽/指数/尾数”三元组，数据流是转换路径，证据流是比较转换前后的 max/数值与显存。

#### 6. 拓展知识

延伸：`torch.autocast` 的算子白/黑名单、`GradScaler` 解决 fp16 下溢、量化（`torch.quantize_per_tensor`、PTQ/QAT）、`Tensor.to(memory_format=...)`。与部署交叉时校准数据 dtype 决定数值一致性。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. fp16 和 bf16 怎么选？——优先 bf16（动态范围大、不易溢出），fp16 需 GradScaler。
2. 为什么归约要用 float32 累加？——低精度累加误差随长度增长，可能溢出或丢有效位。
3. `.to(dtype)` 一定会复制吗？——dtype 变化时复制；同 dtype 同设备通常返回自身。

#### 8. 易错点与反例

1. 用 `int32` 存超大 token id/偏移，溢出成负数导致索引越界。
2. 在 float16 上直接对大张量 `sum()`，中间值溢出为 inf 而误以为数据异常。
3. 把输入整体转 float16 却忘了保留 master weights，训练长期丢失精度甚至发散。

#### 9. 实践与验证

脚本：`t=torch.tensor([70000.],dtype=torch.float32).half()` 得到 inf；`torch.tensor([70000.]).half().float()` 无法还原；`(torch.ones(1,dtype=torch.float16)+torch.ones(1,dtype=torch.float32)).dtype` 为 float32；比较同数据 float32/float16 的 `.element_size()`。oracle 为“范围/精度/字节符合预期”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B007 评分：不合格只说“dtype 是类型”；合格能列出常见 dtype 位宽与用途；良好能给出溢出/下溢反例并说明累加精度；优秀能结合 AMP、GradScaler、master weights 与量化给出工程取舍。
### B008. CPU 与 GPU 之间的数据迁移有哪些边界？

- 难度：入门级
- 知识域：K01 张量、dtype、device 与内存布局
- 考察能力：概念边界、适用条件与反例
- 题型：概念解释与辨析
- 面试频率：高频
- 前置知识：device、host memory、CUDA stream
- 关联题目：B005、I005、A003
- 事实状态：[已确认][待验证]

#### 1. 背景知识

CPU↔GPU 搬运主要 API 是 `Tensor.to(device)`/`.cuda()`/`.cpu()`，底层走 PCIe/NVLink。可分页（pageable）内存的 H2D 拷贝通常同步；页锁定（pinned）内存配合 `non_blocking=True` 才能异步并重叠计算。搬运 `requires_grad` 张量会保持 computation graph 关系；不同 device 的算子混用会报 `Expected all tensors on same device`。张量在 GPU 上时 `numpy()` 会报类型错误，需先 `.cpu()`。

#### 2. 问题分析

区分方向（H2D/D2H/D2D）、是否 pinned、是否异步、是否需回传梯度。不变量：一个算子的所有输入需在同一 device；`.cpu()` 会触发同步（若来自 GPU）。边界：在 CUDA stream 未同步前 `.cpu()` 会隐式同步；pinned 内存会占用不可换出的主机内存，过度 pin 影响系统。

#### 3. 具体答案

面试中可以这样回答：
> CPU 与 GPU 之间搬运有清晰边界。第一，算子要求所有张量同 device，否则报错，所以搬运是显式的，不会自动发生（除个别内部 .item()/.cpu() 会隐式同步）。第二，可分页内存的 H2D 是同步的；只有先用 `pin_memory=True`（或 `.pin_memory()`）把数据放到页锁定内存，再用 `.to(device, non_blocking=True)`，拷贝才能与计算异步重叠。第三，`.cpu()` 从 GPU 取数据会同步当前 stream，`float(tensor)`/`tensor.item()` 也会触发同步，训练循环里频繁调用会拖慢。第四，GPU 张量不能直接 `.numpy()`，要先 `.cpu()`。第五，把 `requires_grad` 张量搬到 GPU 会保留图关系，梯度会回到原设备的叶子。

#### 4. 技术洞察

带宽/时间模型：`t_copy ≈ bytes / effective_bandwidth + latency`，PCIe 与 NVLink 带宽差异可达数倍，因此 pin + async + 大 batch 才能摊薄开销。不变量：`non_blocking=True` 只有在源是 pinned 内存且目标为 CUDA 时才是真异步；否则等价同步。可验证信号：Profiler 中 `Memcpy HtoD` 是否与 kernel 重叠，以及同步 API 造成的 stream 空档。

#### 5. 图文说明

| 路径 | 源内存 | 是否可异步 | 常见触发 |
|---|---|---|---|
| H2D | pageable | 否（同步） | `.to('cuda')` |
| H2D | pinned | 是 | `.to('cuda', non_blocking=True)` |
| D2H | GPU | 需同步才可靠读值 | `.cpu()`、`.item()` |
| D2D | GPU→GPU | 是 | `.to('cuda:1')` |

图示解释：控制流是显式 device 切换，数据流是 PCIe/NVLink 拷贝，证据流看 stream 是否同步与拷贝是否重叠。

#### 6. 拓展知识

延伸：`torch.cuda.Stream` 与多流重叠、`record_stream` 管理生命周期、`cudaMemcpyAsync`、UVA/统一内存、`DataLoader` pin_memory 的由来。多卡时 `to('cuda:N')` 与 NCCL 通信缓冲区关系。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. `non_blocking=True` 为什么有时仍同步？——源不是 pinned 内存或做 D2H 读取值时退化为同步。
2. `.item()` 为什么会拖慢训练？——它隐式同步设备，破坏异步流水。
3. 为什么 GPU 张量不能直接 `numpy()`？——numpy 只能访问主机内存，需先搬到 CPU。

#### 8. 易错点与反例

1. 逐元素循环里调用 `.item()` 打印 loss，导致每步同步、GPU 利用率骤降。
2. 在训练中把大张量 `.cpu()` 再 `.cuda()` 来回搬，PCIe 成为瓶颈。
3. 假设 `.to('cuda')` 是异步的，未同步就复用被覆盖的 pinned buffer，读到脏数据。

#### 9. 实践与验证

脚本：构造大张量，比较 `x.to('cuda')` 与 `x.pin_memory().to('cuda', non_blocking=True)` 的墙钟与 Profiler 重叠度；验证 `torch.randn(4, device='cuda').numpy()` 抛错、`.cpu().numpy()` 成功；打印 `x.is_pinned()`。oracle 为“pin+non_blocking 重叠、GPU->numpy 需先 cpu”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B008 评分：不合格只说“搬到 GPU”；合格能区分 H2D/D2H 与同步性；良好能解释 pin+non_blocking 异步条件并指出隐式同步点；优秀能结合多流、NVLink、生命周期管理给出性能取舍。
### B009. `no_grad()` 与 `inference_mode()` 有什么区别？

- 难度：入门级
- 知识域：K02 autograd、计算图与梯度
- 考察能力：概念边界、适用条件与反例
- 题型：概念解释与辨析
- 面试频率：高频
- 前置知识：autograd、推理流程
- 关联题目：B003、I005、A002
- 事实状态：[已确认]

#### 1. 背景知识

`torch.no_grad()` 关闭梯度记录，前向不建图、省内存，但内部创建的张量仍可能带 `requires_grad=True`。`torch.inference_mode()`（1.9 引入）更严格：除不建图外，还关闭版本计数器与 view 追踪，性能略好；但在该模式下创建的张量之后不能安全用于 autograd。两者都可作为上下文管理器或装饰器。

#### 2. 问题分析

分三个层面：是否建图、张量是否可直接用于后续 autograd、是否有额外开销（版本计数）。不变量：no_grad 下张量的 `requires_grad` 仍可为 True 且可参与后续已存在的图；inference_mode 下张量被标记为“推理模式”，离开后再用于需要梯度的运算会报错。边界：对已建好的图调用 no_grad 计算不影响梯度回传。

#### 3. 具体答案

面试中可以这样回答：
> 两者目的都是关掉 autograd、省显存提速，但严格程度不同。`no_grad()` 只是“不记录新梯度”，它创建出来的张量仍然可能 `requires_grad=True`，可以递给训练流程继续建图，因此适合“我需要在这段里算东西但不想让它们进梯度”的场景，比如评估、参数初始化、手动更新。`inference_mode()` 更进一步，会关闭版本计数和 view 追踪，所以更快、更省，但它产出的张量不能再安全地参与需要梯度的运算——典型用于纯推理和部署前向。选择上：评估/推理用 `inference_mode`，需要复用张量做后续可微计算的用 `no_grad`。

#### 4. 技术洞察

不变量：`no_grad` 下 `torch.tensor(..., requires_grad=True).requires_grad` 仍为 True，但对其结果运算不带 `grad_fn`；`inference_mode` 下张量带推理标志，离开上下文后用于 autograd 会报 `Inference tensors cannot be saved for backward`。性能差异来自 inference_mode 省去版本计数器维护与保存张量记录。

#### 5. 图文说明

| 特性 | no_grad | inference_mode |
|---|---|---|
| 建图 | 否 | 否 |
| 版本计数/view 追踪 | 保留 | 关闭 |
| 产出张量可用于后续 autograd | 可以 | 不安全/报错 |
| 典型场景 | 评估、参数初始化、手动更新 | 纯推理、部署 |

图示解释：控制流是“是否记录梯度”的开关，数据流是产出的张量能否重回可微图，证据流看 `requires_grad`/`grad_fn` 与报错。

#### 6. 拓展知识

延伸：`@torch.no_grad()` 装饰器、`detach()` 的区别（detach 产出共享数据但不回图的张量）、`torch.set_grad_enabled`。与部署交叉：导出/推理路径推荐 inference_mode；与 DDP 交叉时评估阶段常配合 no_grad 避免不必要的图。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. no_grad 下创建的张量还有 requires_grad 吗？——可能仍有，但没有 grad_fn。
2. inference_mode 张量能送回训练循环吗？——不能，会因缺版本信息报错。
3. 评估阶段该选哪个？——纯推理选 inference_mode，需复用张量则 no_grad。

#### 8. 易错点与反例

1. 在 inference_mode 里生成张量，之后拿去算 loss 回传，报“Inference tensors cannot be saved for backward”。
2. 以为 no_grad 会永久改变张量属性，其实它只影响上下文内的新建图。
3. 手动权重更新（如 EMA）忘记包 no_grad，导致每次更新都建图占显存。

#### 9. 实践与验证

脚本：`with torch.no_grad(): a=torch.tensor([1.],requires_grad=True); b=a*2` 验证 `b.grad_fn is None` 且 `b.requires_grad is True`；`with torch.inference_mode(): c=torch.tensor([1.])` 离开后参与 `c*2` 求梯度触发报错。oracle 为“no_grad 仍可 requires_grad 但无图、inference_mode 张量不可回图”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B009 评分：不合格认为两者完全相同；合格能说都关梯度但严格度不同；良好能举出推理张量回图报错的反例；优秀能结合版本计数、部署与 DDP 场景给出选择依据。
### B010. 如何让 Dataset 和随机增强可复现？

- 难度：入门级
- 知识域：K04 数据管道与输入规范
- 考察能力：概念边界、适用条件与反例
- 题型：概念解释与辨析
- 面试频率：高频
- 前置知识：Dataset、DataLoader、随机数
- 关联题目：B005、I007、E005
- 事实状态：[已确认][待验证]

#### 1. 背景知识

可复现性依赖对多个随机源的控制：Python `random`、NumPy、PyTorch CPU/CUDA RNG、cuDNN 算法选择。`torch.manual_seed` 设置所有设备种子；`DataLoader` 每个 worker 需 `worker_init_fn` 设置独立且可推导的种子；`torch.use_deterministic_algorithms(True)` 强制确定性算法（部分算子性能下降或报错）；`cudnn.deterministic=True`、`cudnn.benchmark=False` 关闭非确定性算法与自动调优。

#### 2. 问题分析

拆解随机源：采样器顺序、数据增强、初始化、dropout、cuDNN。不变量：给定相同种子、相同代码路径和相同确定性算法，两次运行应产生逐位一致结果。边界：`num_workers>0` 时若不在 worker 内设置种子，增强结果不可复现；某些 GPU 原子操作（如 `index_add`）本身非确定。

#### 3. 具体答案

面试中可以这样回答：
> 可复现要同时管住四个随机源和算法选择。第一，种子：`torch.manual_seed(s)`、`np.random.seed(s)`、`random.seed(s)` 一起设，CUDA 用 `torch.cuda.manual_seed_all(s)`。第二，DataLoader 多进程：必须用 `worker_init_fn` 结合 `get_worker_info()` 给每个 worker 设可推导的种子，否则各 worker 可能共享父进程状态或重复。第三，数据增强要用带显式 seed/generator 的随机函数，保证样本与随机流的对应关系稳定。第四，算法确定性：`torch.use_deterministic_algorithms(True)` 加 `cudnn.deterministic=True`、`cudnn.benchmark=False`。最后，checkpoint 里要保存优化器状态和 RNG 状态（`torch.get_rng_state`）才能断点续训复现。

#### 4. 技术洞察

不变量：复现 = 固定所有随机源的初始状态 + 固定算法确定性。worker 种子常用公式 `seed = base_seed + worker_id`（`get_worker_info().seed` 已由 base 派生），保证每个 epoch 可重放。可验证：同一脚本跑两次打印中间 loss 或张量哈希应完全一致；关闭 deterministic 时可能每次微差。

#### 5. 图文说明

| 随机源 | 控制手段 | 多进程注意点 |
|---|---|---|
| Python/NumPy | manual_seed / np.random.seed | 每 worker 独立重设 |
| PyTorch CPU/CUDA | manual_seed / manual_seed_all | worker_init_fn |
| 采样/增强 | generator / 显式 seed | 用 worker seed |
| 算法 | deterministic + cudnn 设置 | 部分算子无确定实现 |

图示解释：把复现画成“种子注入 + 确定性算法”两条控制线，数据流的每个随机决策点都需被覆盖，证据流是比较两次运行的哈希。

#### 6. 拓展知识

延伸：`torch.Generator`、`torch.get_rng_state/set_rng_state`、`DistributedSampler` 的 shuffle 种子、AMP 的 GradScaler 状态保存。与分布式交叉时每个 rank 的种子应不同但可推导，否则各 rank 数据增强一致。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 为什么设了 manual_seed 多进程增强仍不可复现？——worker 未独立设种。
2. benchmark=True 对复现有何影响？——自动选算法使同一输入可能用不同 kernel，结果不可复现。
3. 断点续训为何仍对不上？——未保存 RNG 状态与优化器状态。

#### 8. 易错点与反例

1. 只在主进程设种子，`num_workers>0` 时所有 worker 从同一状态出发，增强重复。
2. 训练开 `cudnn.benchmark=True` 追求速度，却声称结果可复现。
3. 保存 checkpoint 只存模型权重，恢复后 optimizer momentum 和随机流丢失，loss 曲线接不上。

#### 9. 实践与验证

脚本：设固定种子跑两遍相同前向，比较 `torch.equal(out1,out2)`；开 `use_deterministic_algorithms(True)` 后验证一致；用 `worker_init_fn` 打印各 worker seed 验证不重复；保存/恢复 RNG 状态验证续训一致。oracle 为“两次逐位一致、worker 种子互异且可推导”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B010 评分：不合格只说“设 seed”；合格能列出多个随机源；良好能覆盖 worker_init_fn 与确定性算法；优秀能补充 RNG 状态保存、分布式种子派生与 benchmark 取舍。
### B011. 基本概念：张量、dtype、device 与内存布局

- 难度：入门级
- 知识域：K01 张量、dtype、device 与内存布局
- 考察能力：变量、因果链、复杂度与容量上限
- 题型：原理推导与复杂度
- 面试频率：高频
- 前置知识：shape、stride、设备和精度
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

张量的内存开销由 `numel * itemsize` 决定，布局由 stride 描述，逻辑索引 `(i_0..i_{n-1})` 映射到存储偏移 `offset + Σ i_k·stride_k`。广播、转置等是元数据操作（可零拷贝），而 dtype 变更、`contiguous`、相加会分配新 storage。设备维度额外引入搬运开销与 kernel 调度。

#### 2. 问题分析

先定义变量：`numel`、`itemsize`、`stride`、`bandwidth`。推导三件事：存储字节数、拷贝时间、以及“视图 vs 复制”是否分配内存。给出成立条件（连续、无重叠视图），再用 `expand`（stride=0）作为打破“stride 单调”的边界。

#### 3. 具体答案

面试中可以这样回答：
> 张量的成本可以量化。存储字节 `bytes = numel * itemsize`，其中 `numel = Π shape_i`；float32 的 itemsize 是 4，float16/bf16 是 2。逻辑访问靠 stride 映射，连续张量的 stride 满足 `stride_k = Π_{j>k} shape_j`，因此 `a[i,j] = storage[offset + i*stride0 + j*stride1]`。像转置、`transpose`、`expand`、切片这类只改 stride/offset 的操作不分配新内存，是 O(1) 元数据操作；`expand` 用 stride=0 表示广播，所以 `numel` 变大但 storage 不变。反之 `contiguous`、dtype 转换、逐元素相加会分配，拷贝时间约 `bytes / 有效带宽`。所以复杂度上：视图操作 O(1) 空间，复制操作 O(numel) 空间与 O(bytes/BW) 时间。

#### 4. 技术洞察

不变量一：`bytes = numel * itemsize`。不变量二：连续张量 stride 递减且 `stride_{n-1}=1`。不变量三：视图操作保持 `data_ptr` 不变。可验证：`torch.empty(2,3).stride()==(3,1)`，`torch.empty(2,3).t().stride()==(1,3)`，`torch.ones(1,4).expand(3,4).stride()` 含 0。

#### 5. 图文说明

| 操作 | 是否分配 | 空间复杂度 | 时间/带宽 |
|---|---|---|---|
| transpose/expand/slice | 否 | O(1) | 0 拷贝 |
| contiguous/reshape(需拷贝) | 是 | O(numel) | O(bytes/BW) |
| dtype 转换 | 是 | O(numel) | O(bytes/BW) |
| CPU↔GPU 搬运 | 是 | O(numel) | O(bytes/链路BW) |

图示解释：把“元数据流（stride/offset）”与“数据流（storage 分配/拷贝）”分开，视图只动前者。

#### 6. 拓展知识

延伸：`memory_format`（channels_last 对卷积的影响）、`storage_offset` 与非连续切片、`torch.Tensor.is_contiguous(memory_format=...)`、量化下的 itemsize 与打包布局。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 为什么 `t()` 不拷贝却改变访问顺序？——只交换 stride，storage 不变。
2. `expand` 后 `numel` 增大为何显存不增？——被扩展维 stride=0，复用同一元素。
3. dtype 转换的开销量级？——O(numel)，约 `bytes/BW`。

#### 8. 易错点与反例

1. 把 `view`/`t` 当成免费复制，忽略了后续非连续算子可能触发隐式 `contiguous`。
2. 认为 `expand` 会复制数据，误以为显存爆炸。
3. 忽略 `storage_offset`，用切片视图的 `data_ptr` 推断元素物理位置。

#### 9. 实践与验证

脚本：`x=torch.empty(4,8);` 打印 `x.stride()`、`x.numel()*x.element_size()` 与 `x.storage().nbytes()`；对 `x.t()` 比较 `data_ptr` 与 `is_contiguous`；对 `x.expand` 检查 stride 含 0。oracle 为“字节公式与 stride 映射一致、视图零分配”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B011 评分：不合格只背“张量是数组”；合格能写出 bytes 公式与 stride 映射；良好能区分视图/复制并给出复杂度；优秀能结合 memory_format、量化与设备搬运给出定量模型。
### B012. 基本概念：张量/设备

- 难度：入门级
- 知识域：K01 张量、dtype、device 与内存布局
- 考察能力：变量、因果链、复杂度与容量上限
- 题型：原理推导与复杂度
- 面试频率：高频
- 前置知识：shape、stride、dtype、device
- 关联题目：B001、B011、I012
- 事实状态：[已确认]

#### 1. 背景知识

“张量/设备”问题关注张量与它所在设备的关系：device 属于张量元数据，跨设备算子非法；搬运 H2D/D2H/D2D 有带宽与延迟成本；pinned 内存才可异步；`.item()`/`.cpu()` 会隐式同步。设备选择影响复杂度主项：计算量不变，但搬运与同步引入额外墙钟。

#### 2. 问题分析

定义变量：数据字节 `B`、链路有效带宽 `BW`、延迟 `L`、计算时间 `T_c`。推导总时间 `T ≈ T_c + B/BW + L` 与重叠条件；分析同步点如何串行化。边界：非 pinned 时 `non_blocking` 失效，重叠系数变为 0。

#### 3. 具体答案

面试中可以这样回答：
> 张量带 device 元数据，所有参与同一算子的张量必须在同一设备，否则报错。因此跨设备必须显式搬运，成本可建模为 `T_copy ≈ B/BW + L`，其中 `B = numel*itemsize` 是字节、`BW` 是 PCIe/NVLink 有效带宽、`L` 是启动延迟。要让它不成为瓶颈有两条：第一，用 pinned 内存 + `non_blocking=True` 把拷贝和 GPU 计算重叠，使总时间趋近 `max(T_compute, T_copy)` 而不是两者相加；第二，搬大块而不是小块，让 `B/BW` 主导并摊薄 `L`。反过来，`.item()`、`.cpu()`、逐元素 `.item()` 会强制同步，把异步流水打断。可验证信号是 Profiler 里 `Memcpy HtoD` 是否与 kernel 重叠。

#### 4. 技术洞察

不变量：`T_total ≥ max(T_compute, T_copy)` 仅在异步重叠成立时可达；同步点使 `T_total = T_compute + T_copy + ...`。设备合法性不变量：算子要求 `device(x_i)` 全相等。可验证：`torch.randn(3,device='cpu')+torch.randn(3,device='cuda')` 报错；`x.is_cuda`、`x.device` 反映元数据。

#### 5. 图文说明

| 项 | 公式/条件 | 影响 |
|---|---|---|
| 拷贝时间 | B/BW + L | 决定 H2D 开销 |
| 重叠 | pinned 且 non_blocking | 从相加变取 max |
| 同步点 | .item()/.cpu() | 打断流水、利用率下降 |
| 非法混用 | device 不一致 | RuntimeError |

图示解释：时间线图：kernel 流与拷贝流能否重叠，同步点画成竖线阻断重叠。

#### 6. 拓展知识

延伸：多流（`torch.cuda.Stream`）与 `record_stream`、UVA/统一内存按页迁移、NVLink 与 PCIe P2P、`DataLoader` pin_memory 的动机。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 为什么搬大块更划算？——`L` 固定，`B` 越大 `B/BW` 越主导，单位字节成本下降。
2. 什么条件下重叠成立？——pinned 源 + 异步 API + 无隐式同步。
3. 设备不一致报什么错？——`Expected all tensors to be on the same device`。

#### 8. 易错点与反例

1. 假设搬运算子会自动 fallback 到 CPU，实际直接报错。
2. 在小 batch 上频繁 H2D，`L` 主导导致越并发越慢。
3. 训练循环打印 `loss.item()` 每步同步，利用率掉一大截。

#### 9. 实践与验证

脚本：用 `time` 比较不同大小张量 H2D 的吞吐，验证大块单位成本更低；构造 CPU/GPU 混加验证报错；比较 `.item()` 在循环内外对墙钟的影响。oracle 为“拷贝时间随大小呈线性+截距、混用报错、同步拖慢”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B012 评分：不合格只说“张量有设备”；合格能说明同设备约束与显式搬运；良好能写出拷贝时间模型与重叠条件；优秀能结合多流、P2P、统一内存与同步点给出定量分析。
### B013. 基本概念：Module/状态

- 难度：入门级
- 知识域：K03 Module/状态
- 考察能力：变量、因果链、复杂度与容量上限
- 题型：原理推导与复杂度
- 面试频率：高频
- 前置知识：参数、buffer、模式和 state_dict
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

Module 的状态由参数、buffer、子模块三类注册对象构成。训练显存的主要组成：参数、梯度、优化器状态、激活。Adam 每个参数额外存两个动量（约 2×参数量），SGD+momentum 存一个（1×），因此 fp32 + Adam 的参数相关显存约为参数量的 4 倍。`state_dict` 遍历是 O(张量数)。

#### 2. 问题分析

定义参数量 `P`、dtype 字节 `w`。推导：权重 `P·w`、梯度 `P·w`、Adam 状态 `2P·w`（若用 fp32 master 再叠加）；额外激活与 batch、序列长度相关。给出不含激活的参数显存公式，并用“冻结部分参数”作为减少 `P` 的边界。

#### 3. 具体答案

面试中可以这样回答：
> Module 的状态可以按“谁被注册、谁被更新”来量化。设参数量为 P、单参数字节为 w。仅权重是 `P·w`；训练时梯度再加 `P·w`；若用 Adam，优化器要为每个参数存一阶、二阶动量，即再加 `2P·w`，所以 fp32+Adam 的光参数相关显存约 `4P·w`，这就是“显存约为参数量 4 倍”的来源。SGD+Momentum 只存一个动量，约为 `3P·w`。如果做 AMP 并有 fp32 master weights，还会再叠一份。buffer（如 BN running stats）也占内存且随 model 移动，但不进优化器。冻结层把其参数移出优化器可省下梯度与优化器状态那段。`state_dict()` 的构建是遍历注册树、收集张量，复杂度与张量数量成正比。

#### 4. 技术洞察

不变量：可训练参数量 `P = Σ numel(p) for p in parameters() if p.requires_grad`。显存下界（不含激活）`≈ P·w·(1 + grad? + optimizer_mult)`。Adam 更新：`m_t=β1 m_{t-1}+(1-β1)g`、`v_t=β2 v_{t-1}+(1-β2)g²`，故两个状态。可验证：`sum(p.numel() for p in model.parameters())` 与 `state_dict` 中张量字节数。

#### 5. 图文说明

| 组成 | 大小 | 是否随优化器变 |
|---|---|---|
| 参数 | P·w | 否 |
| 梯度 | P·w | 否 |
| Adam 状态 | 2P·w | 是 |
| buffer | Σ numel·w | 否 |
| 激活 | 与 batch/序列相关 | 否 |

图示解释：把显存画成“参数/梯度/优化器/激活”四层桶，视图是注册树，数据流是每步读写这些桶。

#### 6. 拓展知识

延伸：`parameters()` vs `named_parameters()`、梯度检查点（用计算换激活显存）、ZeRO/FSDP 分片优化器状态、8-bit Adam、`torch.cuda.memory_summary()` 对账。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 为什么常说“显存是参数量的几倍”？——加梯度与优化器状态，Adam 约 4×。
2. 冻结层省了什么？——梯度与优化器状态，权重仍占。
3. buffer 和参数在显存上区别？——都占内存，但 buffer 不被优化器更新。

#### 8. 易错点与反例

1. 只按权重估显存，忽略 Adam 双动量，实际 OOM。
2. 用 `state_dict` 字节数当作训练显存，漏掉优化器状态。
3. 冻结后仍把参数传给优化器，梯度照算照存，没省。

#### 9. 实践与验证

脚本：`P=sum(p.numel() for p in model.parameters())`，构造 Adam 后 `torch.cuda.memory_allocated()` 前后对比，验证约 `4P·4` 字节的增量级；打印各 buffer 的 `numel`。oracle 为“参数量与显存倍率对得上”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B013 评分：不合格只会数层数；合格能区分参数/buffer；良好能推出 4P 显存模型；优秀能结合梯度检查点、ZeRO/FSDP、8-bit 优化器给出容量取舍。
### B014. 基本概念：数据管道

- 难度：入门级
- 知识域：K04 数据管道
- 考察能力：变量、因果链、复杂度与容量上限
- 题型：原理推导与复杂度
- 面试频率：高频
- 前置知识：Dataset、Sampler、collate、padding、版本
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

数据管道由 Dataset、Sampler、DataLoader、collate、pin_memory 组成。多 worker 以进程并行取数，`prefetch_factor` 决定每个 worker 预取批数，主进程用队列缓冲。吞吐受限于最慢环节（IO/解码/增强）与 GPU 计算。`IterableDataset` 用于流式数据，需手工按 worker/rank 分片。

#### 2. 问题分析

定义单样本处理时间 `t_s`、batch 大小 `B`、worker 数 `W`、预取深度。推导稳态吞吐：若取数主导，`t_batch ≈ B·t_s/W`；若计算主导，数据流应能满队列。给出 GPU 不饿的条件，并用 W 过大引入的 IPC/内存开销作为边界。

#### 3. 具体答案

面试中可以这样回答：
> 数据管道的性能可以用排队模型看。设单样本耗时 `t_s`、batch `B`、worker 数 `W`，则取一批的稳态时间约 `B·t_s/W`（理想并行、忽略 IPC）。GPU 不饿的条件是 `B·t_s/W ≤ T_compute`，否则数据端成为瓶颈，表现为 GPU 利用率有周期性空档。`num_workers` 提升吞吐到某个拐点后就饱和甚至下降，因为进程创建、序列化、内存拷贝和 GIL 释放之外的开销开始主导；`prefetch_factor` 提供缓冲但要占内存。`pin_memory=True` 把 H2D 从同步变异步，是让数据流真正和计算重叠的最后一环。流式 `IterableDataset` 还要显式按 `get_worker_info()` 分片，否则多 worker 会重复消费同一份数据。

#### 4. 技术洞察

不变量：稳态下取样速率需 ≥ 消费速率 `1/T_compute` 才能满负荷。`prefetch_factor` 使在途批数 `≈ W·prefetch_factor`，内存占用随之增长。可验证：固定 Dataset 处理时间，扫 W=0,1,2,4,... 观察 epoch 时间到拐点；Profiler 看 GPU 空档。

#### 5. 图文说明

| 变量 | 含义 | 影响 |
|---|---|---|
| t_s | 单样本耗时 | 决定数据端下界 |
| W | worker 数 | 提升到拐点后饱和 |
| prefetch_factor | 每 worker 预取批数 | 增大缓冲与内存 |
| pin_memory | 异步 H2D | 让计算/拷贝重叠 |

图示解释：生产者-消费者队列图，worker 是生产者、GPU 是消费者，队列深度由 prefetch 控制。

#### 6. 拓展知识

延伸：`persistent_workers`、`drop_last`、`Sampler`/`DistributedSampler`、共享内存 `share_memory_()`、`IterableDataset` 分片公式。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 如何判断瓶颈在数据端？——epoch 时间随 W 变化明显、GPU 有空档。
2. prefetch 越大越好吗？——否，占用内存且到拐点后无收益。
3. 流式数据多 worker 如何不重复？——按 worker_id/rank 分片。

#### 8. 易错点与反例

1. W 设成远超 CPU 核数，IPC 抖动使吞吐反而下降。
2. 自定义 Dataset 每次重开文件/建连接，W 增大后 fd/内存耗尽。
3. IterableDataset 未分片，多 worker 各自从头遍历，样本重复。

#### 9. 实践与验证

脚本：固定 `t_s`，扫 W 记录 epoch 时间画曲线找拐点；打印各 worker 的 `get_worker_info().id` 验证分片；对比 pin/non_blocking 开关的墙钟。oracle 为“吞吐随 W 提升到拐点、分片不重复、pin 有收益”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B014 评分：不合格只会调 num_workers；合格能说明生产者-消费者关系；良好能推出吞吐拐点与 GPU 不饿条件；优秀能结合持久 worker、共享内存、分布式分片给出容量与调优方案。
### B015. 基本概念：训练/优化器

- 难度：入门级
- 知识域：K05 训练/优化器
- 考察能力：变量、因果链、复杂度与容量上限
- 题型：原理推导与复杂度
- 面试频率：高频
- 前置知识：forward、backward、累积、optimizer、scheduler
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

训练步骤=前向、反向、优化器更新。优化器内存/计算复杂度以参数量 P 为量级：SGD 为 O(P)，Momentum 存 1 个状态，Adam/AdamW 存 2 个状态并做逐元素更新。学习率调度器（Step/Cosine/OneCycle）改变 lr 序列。梯度裁剪、权重衰减在更新前/中施加。

#### 2. 问题分析

定义 P、batch 数、每步梯度计算量。推导优化器每步内存与时间都是 O(P)，Adam 的额外读写使每步带宽高于 SGD；学习率调度是 O(步数) 的纯标量逻辑。边界：`zero_grad` 缺省导致梯度累积改变有效 lr；梯度裁剪在 AMP 下需先 unscale。

#### 3. 具体答案

面试中可以这样回答：
> 优化器的复杂度以参数量 P 为标尺。SGD 每步只做 `p -= lr·g`，时间是 O(P)、内存 O(P)（仅权重和梯度）；加 momentum 多存一个 `P` 的状态；Adam/AdamW 额外维护一阶、二阶动量 `m`、`v`，内存约 `3P·w`（含权重）且每步要做 `m,v` 的读改写，因此带宽开销更高。lr 调度器（StepLR/CosineAnnealing/OneCycle）是 O(步数) 的标量计算，几乎不占资源，却显著影响收敛。关键不变量：只有清空 `.grad` 后一次 `backward` 才代表当前 batch 的梯度；梯度裁剪要先在 AMP 下 `unscale_` 再裁剪，否则裁剪阈值作用在缩放后的梯度上。权重衰减在 AdamW 里是解耦的 `p -= lr·wd·p`。

#### 4. 技术洞察

核心不变量：优化器每步只使用当前累积周期的梯度，且状态量决定额外显存。更新式：SGD `p ← p − lr·g`；Momentum `v ← μv + g; p ← p − lr·v`；Adam `m,v` 更新后 `p ← p − lr·m̂/(√v̂+ε)`。内存下界 `P·w·(1+1+optimizer_states)`。可验证：`len(optimizer.state_dict()['state'])` 显示每个参数的状态条目数（Adam 为 2 个张量）。

#### 5. 图文说明

| 优化器 | 每参数状态 | 每步复杂度 | 额外显存 |
|---|---|---|---|
| SGD | 0 | O(P) | 无 |
| SGD+Momentum | 1 | O(P) | P·w |
| Adam/AdamW | 2 | O(P) | 2P·w |

图示解释：控制流是 step 循环，数据流是权重/梯度/状态的读写，证据流看 state_dict 条目数与每步耗时。

#### 6. 拓展知识

延伸：`foreach`/`fused` 优化的批量更新、8-bit Adam、LAMB、梯度累积模拟大 batch、`clip_grad_norm_` 与 AMP scaler 顺序。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. Adam 为什么比 SGD 占显存多？——多存两个动量张量。
2. 梯度累积如何影响 lr？——等效放大 batch，通常需按累积步缩放 lr/归一化。
3. AMP 下裁剪顺序？——先 `scaler.unscale_(optimizer)` 再裁剪再加权 step。

#### 8. 易错点与反例

1. 忘记 `zero_grad`，梯度跨步累积，等价偷偷增大 lr 与 batch。
2. Adam 权重衰减直接加到梯度（非解耦），与 AdamW 行为不同。
3. AMP 下直接裁剪缩放后的梯度，阈值失效导致裁剪无效或过强。

#### 9. 实践与验证

脚本：构造一层线性层，切换 SGD/Adam 打印 `optimizer.state_dict()['state']` 各参数张量数量；跑几步观察权重变化；验证 `zero_grad` 前后 `.grad`。oracle 为“Adam 每参数 2 个状态张量、梯度可清零”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B015 评分：不合格只会说“优化器更新参数”；合格能区分 SGD/Momentum/Adam 的内存；良好能写出更新式与复杂度；优秀能结合梯度累积、解耦权重衰减、AMP 裁剪顺序与 fused 优化给出工程方案。
### B016. 基本概念：AMP/恢复

- 难度：入门级
- 知识域：K06 AMP/恢复
- 考察能力：变量、因果链、复杂度与容量上限
- 题型：原理推导与复杂度
- 面试频率：高频
- 前置知识：autocast、scaler、快照、随机状态
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

AMP 用 `autocast` 让部分算子在 fp16/bf16 下执行、其余保持 fp32，并用 `GradScaler` 对 fp16 损失做缩放以避免梯度下溢。缩放损失 `L' = S·L`，反传得到 `S·g`，step 前 `unscale`（除以 S）。若检测到 inf/nan，`scaler.step()` 跳过该步并降低 S；连续若干步无溢出则增大 S（`growth_interval`）。断点恢复需保存 scaler 状态。

#### 2. 问题分析

定义缩放因子 S、初始值（fp16 默认 2^16=65536）、增长因子 2、回退因子 0.5。推导缩放如何改变可表示梯度范围，以及 S 的自动调优过程。边界：溢出时该步必须跳过，否则权重被 inf 污染；恢复时不存 scaler 状态会导致 S 跳变。

#### 3. 具体答案

面试中可以这样回答：
> AMP 的核心机制是“低精度算 + 高精度兜底 + 损失缩放”。`autocast` 为矩阵乘、卷积等算子选 fp16，为 softmax、归约、loss 等保持 fp32，从而兼得速度和数值稳定。fp16 的最小正规数约 6.1e-5，小梯度会下溢成 0，于是 `GradScaler` 把 loss 乘上 S（fp16 初值默认 `2^16=65536`），反传得到放大 `S` 倍的梯度，更新前再 `unscale_` 除回。梯度里若出现 inf/nan，scaler 跳过这步并把 S 减半；连续 `growth_interval` 步无溢出就把 S 翻倍，逐步逼近不漏精的最大安全缩放。因此训练循环的固定顺序是：`scaler.scale(loss).backward()` →（可选裁剪前）`scaler.unscale_(opt)` → `scaler.step(opt)` → `scaler.update()`。断点续训必须保存 `scaler.state_dict()`，否则 S 从初值重来、前期可能反复跳步。

#### 4. 技术洞察

不变量：缩放不改变最优解，只改变梯度的可表示范围；`unscale` 必须在 `step` 前完成（裁剪也要在 unscale 后）。调优规则：溢出 → `S ← S·backoff`（默认 0.5）并跳过；连续无溢出 `growth_interval` 步 → `S ← S·growth`（默认 2）。可验证：`GradScaler().get_scale()` 初始为 65536.0；`state_dict()` 含 `scale`、`growth_factor`、`backoff_factor`、`growth_interval`、`_growth_tracker`。

#### 5. 图文说明

| 量 | 值/规则 | 作用 |
|---|---|---|
| 初始 scale | 65536 (fp16) | 防下溢 |
| 溢出处理 | 跳过步，S 减半 | 防污染权重 |
| 无溢出 | 连续 N 步后 S 翻倍 | 逼近平稳值 |
| 保存 | scaler.state_dict() | 续训一致 |

图示解释：把 scaler 画成带反馈的旋钮，数据流是 loss→scale→backward→unscale→step，证据流看 S 与是否跳步。

#### 6. 拓展知识

延伸：bf16 通常不需要 GradScaler（动态范围同 fp32）；`autocast` 的 cpu/dtype 参数；`torch.amp.GradScaler(device_type=...)`；FSDP/DDP 下 scaler 与通信的关系。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 为什么 bf16 常不需要 GradScaler？——指数范围与 fp32 相同，不易下溢。
2. 溢出时为什么必须跳过 step？——否则 inf/nan 梯度会写入权重。
3. 恢复为什么要存 scaler？——否则 S 回到初值，训练动态改变。

#### 8. 易错点与反例

1. 直接 `loss.backward()` 而不用 `scaler.scale`，fp16 小梯度下溢为 0，训练停滞。
2. 先 `scaler.step` 再 `scaler.unscale_`，unscale 无意义且可能报错。
3. 保存 checkpoint 漏了 scaler 状态，续训时 S 突变导致曲线不连续。

#### 9. 实践与验证

脚本：`s=torch.amp.GradScaler('cuda'); print(s.get_scale())` 验证 65536；构造会产生 inf 的 loss 观察 `s.step` 是否跳过并降 S；保存/加载 `s.state_dict()` 验证 S 一致。oracle 为“初值、溢出跳步降 S、无溢出升 S、状态可恢复”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B016 评分：不合格只知“半精度加速”；合格能说 autocast+GradScaler 分工；良好能写出缩放/调优规则与更新顺序；优秀能结合 bf16、分布式与断点恢复给出数值稳定方案。
### B017. 基本概念：分布式

- 难度：入门级
- 知识域：K07 分布式
- 考察能力：变量、因果链、复杂度与容量上限
- 题型：原理推导与复杂度
- 面试频率：高频
- 前置知识：rank、DDP、FSDP、collective、容错
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

数据并行下 DDP 在每个进程持有完整模型副本，反向传播时对梯度做 AllReduce 平均，并用 bucket 把多个参数梯度打包、与反向计算重叠通信。通信量约为每步参数梯度字节数 `P·w`。Ring AllReduce 的每 rank 传输量约 `2(N-1)/N · B`（N 为进程数、B 为消息字节）。`gradient_as_bucket_view` 可减少显存拷贝。

#### 2. 问题分析

定义 N、梯度字节 B、链路带宽 BW。推导 AllReduce 时间 `≈ 2(N-1)/N·B/BW` 与随 N 的增长趋势；分析 bucket 大小对重叠效率的影响（太小启动开销大、太大延迟重叠）。边界：通信与计算若不能重叠，总时间近似相加；跨节点链路慢会放大该问题。

#### 3. 具体答案

面试中可以这样回答：
> DDP 的代价可以用通信量刻画。设参数量 P、单参数字节 w，则每步要 AllReduce 的梯度字节约 `B = P·w`。Ring AllReduce 每个 rank 的发送/接收量约 `2(N-1)/N·B`，N 很大时趋于 `2B`，所以时间约 `2(N-1)/N·B/BW`，随 N 增长但逐渐饱和，真正的瓶颈常在跨节点带宽。DDP 靠两件事让它“免费”：一是把参数梯度按 bucket 打包，减少启动次数并让多个小张量合并传输；二是把 bucket 的 AllReduce 与反向传播重叠——反向是按层从后往前算的，算完一层就能把该层 bucket 发出去通信，这样总时间趋近 `max(T_backward, T_comm)` 而非相加。调优上 bucket_cap_mb 太大延迟重叠、太小启动开销大；`gradient_as_bucket_view=True` 能省掉梯度拷贝的显存。

#### 4. 技术洞察

不变量：AllReduce 结果=各 rank 梯度之和/均值，保证各副本参数一致；`2(N-1)/N` 是 Ring 的带宽因子。可验证：`torch.distributed` 单机多进程跑 2 vs 4 rank 计时；NCCL 日志显示 bucket 与通信量。计算/通信比是“是否 scale 划算”的判据。

#### 5. 图文说明

| 维度 | 公式/参数 | 影响 |
|---|---|---|
| 通信量 | B=P·w 每步 | 决定通信下界 |
| Ring 因子 | 2(N-1)/N | 随 N 饱和 |
| bucket_cap_mb | 打包大小 | 重叠效率与启动开销 |
| 重叠 | 反向逐层触发 | 总时间取 max |

图示解释：时间线上反向 kernel 与 AllReduce 交错，bucket 越大越少启动但重叠越晚。

#### 6. 拓展知识

延伸：`no_sync()` 做梯度累积、`find_unused_parameters`、FSDP 分片参数/梯度/优化器、ZeRO 阶段、`all_reduce` vs `reduce_scatter+all_gather`。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. DDP 为什么不随 N 线性变慢？——Ring 因子 `2(N-1)/N` 饱和到 2。
2. 如何让通信隐藏进计算？——bucket + 反向重叠。
3. 每步通信量是多少？——约参数梯度字节 P·w。

#### 8. 易错点与反例

1. 小模型多卡：通信无法被计算掩盖，扩展效率远低于线性。
2. bucket 设得过小，海量 AllReduce 启动开销拖慢训练。
3. 以为 DDP 省显存——它每卡持有完整模型与优化器状态。

#### 9. 实践与验证

脚本：用 `torchrun` 起 2/4 进程跑同一模型，记录每步时间与 NCCL 通信量，验证不随 N 线性变差；开 `no_sync()` 累积若干步再同步，比较通信次数。oracle 为“通信量≈P·w、扩展随 N 饱和、no_sync 减少通信”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B017 评分：不合格只会说“DDP 多卡跑”；合格能说 AllReduce 平均梯度；良好能写出通信量/Ring 因子与重叠机制；优秀能结合 bucket 调优、no_sync、FSDP 与带宽瓶颈给出扩展性分析。
### B018. 基本概念：编译栈

- 难度：入门级
- 知识域：K08 编译栈
- 考察能力：变量、因果链、复杂度与容量上限
- 题型：原理推导与复杂度
- 面试频率：高频
- 前置知识：Dynamo、AOTAutograd、Inductor、guard、缓存
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

`torch.compile` 以 Dynamo 捕获 Python 字节码成图、AOTAutograd 生成前反向图、Inductor 生成融合 kernel。遇到不支持的 Python 控制流或数据依赖会“图断裂”（graph break）回退到 eager。编译产物按“guard”（输入 shape/dtype/设备等）缓存，guard 不满足会触发重编译。融合减少 kernel 启动与中间张量读写，是主要收益来源。

#### 2. 问题分析

定义图规模、kernel 数、中间张量流量。推导融合收益：把 N 个逐元素算子融合成 1 个 kernel，减少 `(N-1)` 次中间张量写+读，即省 `~2(N-1)·bytes` 的内存带宽；启动开销从 N 次降到 1 次。边界：动态 shape 导致反复重编译，编译时间可能超过收益；graph break 使部分区域退回 eager。

#### 3. 具体答案

面试中可以这样回答：
> `torch.compile` 的复杂度可以用“图规模 + guard 数量 + kernel 数”来看。前端 Dynamo 把 Python 逐字节码捕获成 FX 图，遇到数据依赖控制流、`.item()`、不支持的第三方库就会 graph break，把那段留回 eager。中端 AOTAutograd 生成前向和反向图。后端 Inductor 把图编译成 Triton/C++ kernel，并把连续逐元素算子融合，于是原本 N 个算子对应的 N 次 kernel 启动和约 `2(N-1)` 次中间张量读写被压成 1 个 kernel，收益主要来自省下的 HBM 带宽和启动延迟。代价是编译有固定开销，且产物按 guard 缓存：输入 shape/dtype/device 变化会触发重编译（recompilation），动态 shape 场景可能“编译到天荒地老”。所以工程上要固定 shape 或用 `dynamic=True` 并限制动态维，观察 `TORCH_LOGS=recompiles` 判断是否频繁重编。

#### 4. 技术洞察

不变量：融合不改变数学语义，只改变内存访问与启动次数；收益上界≈被融合算子的中间张量往返带宽。可验证：`torch._dynamo.explain` 输出 graph break 数；`TORCH_LOGS=recompiles` 显示重编译原因；对比 eager/compiled 的 Profiler kernel 数。

#### 5. 图文说明

| 阶段 | 组件 | 复杂度关注点 |
|---|---|---|
| 捕获 | Dynamo | graph break 数量 |
| 微分 | AOTAutograd | 前/反向图大小 |
| 代码生成 | Inductor | 融合度、kernel 数 |
| 缓存 | guard | 重编译次数 |

图示解释：流水线图从 Python→FX 图→前反向图→kernel，旁路是 graph break 回退 eager。

#### 6. 拓展知识

延伸：`fullgraph=True`、`mode='reduce-overhead'`（CUDA Graph）、`max-autotune`、CUDA Graph 与动态 shape 冲突、与 AMP/分布式组合。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 什么时候编译反而更慢？——动态 shape 频繁重编译、graph break 多、编译开销摊不掉。
2. 融合为什么省时间？——减少 kernel 启动与中间张量 HBM 往返。
3. 如何定位 graph break？——`TORCH_LOGS=graph_breaks` 或 `explain`。

#### 8. 易错点与反例

1. 在热路径里 `print(tensor)`/`.item()`，每次触发 graph break 与重编译。
2. 输入 shape 每步都变，guard 频繁失效，编译时间超过收益。
3. 以为 compile 自动处理不适合的算子，实际部分回退 eager 却被忽略。

#### 9. 实践与验证

脚本：同一模型跑 eager 与 `torch.compile`，用 Profiler 比较 kernel 数与墙钟；`TORCH_LOGS=recompiles` 检查重编译；构造变化 shape 观察缓存失效。oracle 为“融合后 kernel 数下降、固定 shape 无重编译”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B018 评分：不合格只会说“compile 加速”；合格能说 Dynamo/AOTAutograd/Inductor 分工；良好能解释融合收益与重编译代价；优秀能结合 guard、动态 shape、CUDA Graph 给出可观测的调优策略。
### B019. 基本概念：性能

- 难度：入门级
- 知识域：K09 性能
- 考察能力：变量、因果链、复杂度与容量上限
- 题型：原理推导与复杂度
- 面试频率：高频
- 前置知识：关键路径、带宽、算力、通信、显存、统计
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

性能常以 Roofline 模型分析：计算受峰值算力 `FLOPS` 限制或受内存带宽 `BW` 限制，由算术强度 `AI = FLOPs / Bytes` 决定。临界点 `AI* = FLOPS_peak / BW`。GPU 上矩阵乘算力强、AI 高，逐元素/归约算子是内存受限。利用率、p99 延迟、显存占用是常见指标。

#### 2. 问题分析

定义算子 FLOPs、访存字节、峰值算力、带宽。推导可达性能 `Achieved ≈ min(FLOPS_peak, AI·BW)`；判断算子属计算受限还是带宽受限。边界：小算子受启动延迟主导，Roofline 静态模型失效。

#### 3. 具体答案

面试中可以这样回答：
> 判断性能瓶颈先算算术强度 `AI = FLOPs / Bytes`，再和临界点 `AI* = 峰值算力 / 峰值带宽` 比。若 `AI < AI*`，算子内存受限，可达性能约 `AI·BW`；若 `AI > AI*`，计算受限，上界是峰值算力。比如大矩阵乘 AI 高，是计算受限，优化方向是提高 tensor core 利用率和融合；逐元素 `a+b`、softmax、LayerNorm 这类 AI 低的算子是带宽受限，优化方向是减少访存、算子融合、用低精度。小算子即使 AI 合适，也会被 kernel 启动延迟主导，此时 Roofline 不准，要看 Profiler 的 launch 开销。所以性能题的标准答法是：先分类计算/带宽受限，再给出上界公式，最后说明常数项（延迟、启动、同步）来自哪里。

#### 4. 技术洞察

不变量：`T_min ≥ max(FLOPs/FLOPS_peak, Bytes/BW)`；`AI* = FLOPS_peak/BW`。带宽受限时优化优先降低 Bytes（融合、复用、低精度）。可验证：Profiler 中某算子的 achieved FLOPS 与 achieved BW 和理论上界对比；`torch.cuda.get_device_properties` 读峰值参数。

#### 5. 图文说明

| 区域 | 条件 | 优化方向 |
|---|---|---|
| 计算受限 | AI > AI* | tensor core、融合、减少冗余计算 |
| 带宽受限 | AI < AI* | 融合、低精度、缓存复用 |
| 延迟受限 | 算子过小 | 增大批、CUDA Graph、合并 kernel |

图示解释：Roofline 折线图，横轴 AI、纵轴可达 FLOPS，斜坡是带宽墙、平台是算力墙。

#### 6. 拓展知识

延伸：occupancy、tensor core、shared memory、`torch.backends.cudnn`、Profiler 的 kernel 时间占比、显存带宽对注意力的影响。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. LayerNorm 属哪类？——带宽受限。
2. 大 matmul 的优化点？——tensor core 利用率、切分、融合。
3. 为什么小算子 Roofline 失效？——启动延迟主导。

#### 8. 易错点与反例

1. 用峰值 FLOPS 报告带宽受限算子的“利用率”，数字难看却没意义。
2. 忽略 kernel 启动与同步开销，把理论下界当实际可达。
3. 只看平均耗时忽略 p99，掩盖长尾抖动。

#### 9. 实践与验证

脚本：对 matmul 与 elementwise add 分别算 AI 并从 Profiler 取 achieved FLOPS/BW，与 Roofline 上界对比；用 `get_device_properties` 读峰值。oracle 为“带宽受限算子 reached BW 接近峰值、计算受限接近算力峰值”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B019 评分：不合格只会看总时间；合格能区分计算/带宽受限；良好能写出 Roofline 上界与临界点；优秀能结合 occupancy、启动延迟、p99 与融合给出定位与优化路径。
### B020. 基本概念：导出部署

- 难度：入门级
- 知识域：K10 导出部署
- 考察能力：变量、因果链、复杂度与容量上限
- 题型：原理推导与复杂度
- 面试频率：高频
- 前置知识：state_dict、Export、runtime、schema、迁移
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

导出部署把 PyTorch 模型转成可移植格式：`torch.export`/TorchScript 产出图，ONNX 导出用 opset 描述算子集。动态轴（batch/seq）、控制流、自定义算子影响可导出性；opset 版本决定可用算子。部署端推理引擎（ONNX Runtime/TensorRT）按图优化并做 kernel 选择。

#### 2. 问题分析

定义图节点数、动态维数量、opset。推导导出复杂度：算子覆盖度与动态 shape 的组合；静态 shape 可做更多常量折叠。边界：数据依赖控制流与 Python 对象无法导出，需改为张量运算或 `torch.cond`。

#### 3. 具体答案

面试中可以这样回答：
> 导出部署的本质是把动态的 Python 执行“冻结”成静态图。两个主流是 TorchScript/`torch.export` 和 ONNX。复杂度和风险点有三个：第一，算子覆盖——图里每个算子都必须在目标 opset 里有对应，否则导出失败或需要自定义算子；opset 越高算子越多但旧推理引擎可能不支持。第二，动态 shape——`dynamic_axes` 声明 batch/seq 可变后，导出器不能做与具体形状绑定的优化，且下游引擎要处理 shape 推断；全静态 shape 优化空间最大但泛化差。第三，控制流——数据依赖的 if/while、`.item()`、Python 容器往往无法导出，需要重写成张量运算或 `torch.cond`。可验证的做法是导出后用 `onnx.checker` 校验、与 eager 输出对比 `allclose`，并覆盖不同 batch/seq 长度。

#### 4. 技术洞察

不变量：导出的图必须对声明的动态轴与输入集合保持数值等价；算子集是导出的“语法”。可验证：`torch.onnx.export` 后用 `onnx.checker.check_model`；用 ONNX Runtime 跑同一输入与 PyTorch 输出 `allclose`。复杂度上，常量折叠/图优化与静态 shape 规模和算子数相关。

#### 5. 图文说明

| 关注点 | 静态 shape | 动态 shape |
|---|---|---|
| 优化空间 | 大 | 受限 |
| 导出难度 | 低 | 需声明并校验 |
| 常见失败 | 少 | shape 推断错误 |
| 控制流 | 需可追踪 | 需可追踪 |

图示解释：数据流 PyTorch 模块→图→opset→推理引擎，控制流是动态轴声明与算子映射。

#### 6. 拓展知识

延伸：`torch.export` 的 `dynamic_shapes`、`Dim`、TorchScript 与 `torch.jit.trace` 的取舍、ONNX 自定义算子、TensorRT/OpenVINO 后端、量化导出。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. trace 与控制流的冲突？——trace 只记录执行路径，数据依赖分支会固化。
2. 动态轴的代价？——优化受限、需下游 shape 推断。
3. 如何证明导出正确？——checker + 输出 allclose + 多 shape 覆盖。

#### 8. 易错点与反例

1. 用 `jit.trace` 导出带 `if x.sum()>0` 的模型，分支被固化导致错误。
2. 导出的 opset 太高，目标推理引擎不支持对应算子。
3. 只测固定 batch 就对，线上换 batch 触发布局/形状错误。

#### 9. 实践与验证

脚本：导出一个含动态 batch 的模型到 ONNX，`onnx.checker` 校验，用 ONNX Runtime 在 batch=1/4/8 下与 PyTorch `allclose`；对含数据依赖分支的模型验证导出失败或需改写。oracle 为“多 shape 数值一致、checker 通过”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B020 评分：不合格只会说“导出成 ONNX”；合格能说 TorchScript/ONNX 两条路；良好能解释 opset、动态轴与控制流约束；优秀能结合 checker/allclose 校验、后端优化与量化给出可落地导出方案。
### B021. 基本概念：自定义算子

- 难度：入门级
- 知识域：K11 自定义算子
- 考察能力：事件顺序、状态迁移、所有权与观测点
- 题型：流程/调用链/数据流
- 面试频率：高频
- 前置知识：schema、dispatcher、stream、autograd、ABI
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

自定义算子在调用链上位于“算子定义 → dispatcher 分派 → 具体后端 kernel”。实现方式三类：`torch.autograd.Function`（Python 级，定义 forward/backward）、`torch.library`/`TORCH_LIBRARY`（注册 schema 与后端实现）、C++/CUDA 扩展（`torch.utils.cpp_extension`）。关键流程是注册 schema、绑定设备 kernel、可选注册反向。

#### 2. 问题分析

按调用链定位：Python 调用 → 算子 schema → dispatcher 按（算子名、device、layout）选内核 → 执行 → 返回。不变量：schema 决定参数/返回值签名与别名规则；autograd.Function 的 backward 必须对每个需要梯度的输入返回对应梯度。边界：注册了前向但未注册反向，反向会报无可用 kernel 或走 fallback。

#### 3. 具体答案

面试中可以这样回答：
> 自定义算子在调用链里的“工作位置”是 dispatcher。你先把算子用 schema 注册进去（`torch.library.custom_op` 或 `TORCH_LIBRARY`），dispatcher 依据算子名、device、dtype、layout 选到具体实现：CPU kernel、CUDA kernel 或某种 fallback。数据流是张量从 Python 进、经 dispatcher、进 kernel、返回。梯度方面有两条路：用 `torch.autograd.Function` 时，你在 Python 里写 `forward` 和 `backward`，引擎按链式法则调用你的 backward，你要为每个需要梯度的输入返回梯度（不需要的返回 None）；用 C++ 扩展时通常在算子注册里再注册一个 `_backward` 或基于已有可微算子组合。流程上还有一个易错点是「别名规则」：schema 里要声明输入是否被修改/返回是否与输入共享存储，否则 dispatcher 的版本检查和图捕获会出错。验证方式是 `torch.autograd.gradcheck` 做数值梯度对比。

#### 4. 技术洞察

不变量：每个算子有唯一 `schema`，dispatcher 的分派键是 `(dispatch_key)` 如 CPU/CUDA/Autograd。`autograd.Function.backward` 返回值个数必须等于 `forward` 输入个数（或省略）。可验证：`torch.autograd.gradcheck(fn, inputs)` 与解析/数值梯度比较通过；`torch.library.opcheck` 校验 schema、别名与 autograd。

#### 5. 图文说明

| 阶段 | 组件 | 责任 |
|---|---|---|
| 注册 | torch.library / cpp_extension | 定义 schema 与后端 kernel |
| 分派 | dispatcher | 按 device/layout 选 kernel |
| 执行 | CPU/CUDA kernel | 真正计算 |
| 反向 | autograd.Function.backward | 返回各输入梯度 |

图示解释：调用链图 Python→schema→dispatcher→kernel，反向链上再挂一个 backward 节点。

#### 6. 拓展知识

延伸：`torch.library` 的 `impl`/`register_autograd`、CUDA 扩展编译、`make_fx` 捕获自定义算子、与 torch.compile 的图断裂关系。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 自定义算子如何被 dispatcher 找到？——按注册的 schema 与 dispatch key。
2. backward 返回值有什么约束？——与 forward 输入一一对应。
3. 别名规则为什么重要？——影响版本计数、图捕获与正确性。

#### 8. 易错点与反例

1. 只注册前向没注册反向，训练时反向报无 kernel。
2. backward 里漏返回某个输入梯度，梯度对不上或报错。
3. schema 未声明 input mutation，dispatcher 版本检查失效导致静默错误。

#### 9. 实践与验证

脚本：用 `torch.autograd.Function` 实现一个可微算子，`gradcheck` 通过；给同一算子模拟缺失 backward 验证报错；用 `torch.library.opcheck` 校验 schema。oracle 为“gradcheck 通过、错误路径符合预期”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B021 评分：不合格只会说“写个算子”；合格能说注册与 dispatcher 分派；良好能写 autograd.Function 双向契约；优秀能结合别名规则、opcheck、compile 捕获给出工程实现。
### B022. 基本概念：测试治理

- 难度：入门级
- 知识域：K12 测试治理
- 考察能力：事件顺序、状态迁移、所有权与观测点
- 题型：流程/调用链/数据流
- 面试频率：高频
- 前置知识：oracle、证据、日志、回滚、门禁、复盘
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

测试治理是围绕模型/训练流程的质量门禁：单元测试（算子/shape/dtype）、集成测试（单步训练、checkpoint 往返）、端到端（小数据过拟合、指标阈值）、回归对拍（golden 输出、与参考实现 allclose）。CI 上需分 CPU/GPU job、管理容差、识别 flaky。

#### 2. 问题分析

按测试金字塔定位：越底层越快越多，越上层越慢越少。不变量：测试应可复现（固定种子）、有明确 oracle（数值容差或阈值）、失败可定位。边界：GPU 非确定性算子使逐位比较不可行，需改用容差；flaky 测试要隔离而非重试掩盖。

#### 3. 具体答案

面试中可以这样回答：
> 测试治理的“工作位置”贯穿开发流程：改一个算子先跑单元测试，改数据管道跑集成测试，发布前跑端到端和回归对拍。分层上，单元层验证 shape、dtype、device、边界值和 `gradcheck`；集成层验证“单步训练 loss 有限/下降、checkpoint 保存加载一致、断点续训 loss 曲线可接”；端到端用一个极小数据集过拟合，指标应高于阈值，回归则与上一版本 golden 输出在容差内 `allclose`。治理要点有三：可复现——固定随机种子与确定性算法，否则结果抖动无法判定；容差——GPU 归约非确定，逐位比较不可行，要设 `rtol/atol`；flaky 隔离——重复失败的用例要标出根因（资源竞争、非确定 kernel），不能靠重试掩盖。CI 还应在 PR 上跑快用例、在夜间跑全量 GPU 用例，并记录覆盖率。

#### 4. 技术洞察

不变量：测试 oracle 必须独立于被测实现的随机性来源；`|actual − expected| ≤ atol + rtol·|expected|` 是通用数值判据。可验证：CI 中同一测试在固定种子下重复 N 次结果一致；覆盖率工具报告行覆盖与分支覆盖。

#### 5. 图文说明

| 层级 | 范围 | oracle |
|---|---|---|
| 单元 | 算子/shape/dtype | gradcheck、精确或容差 |
| 集成 | 单步训练/checkpoint | loss 有限、往返一致 |
| 端到端 | 小数据过拟合 | 指标 > 阈值 |
| 回归 | 版本对拍 | golden allclose |

图示解释：金字塔图，底层快而多、顶层慢而少；旁路是 flaky 隔离仓。

#### 6. 拓展知识

延伸：`pytest` 参数化、`torch.testing.assert_close`、`gradcheck`、CI 分 job、基准回归（性能门禁）、golden 更新流程。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. GPU 结果如何比较？——用容差 `assert_close(rtol, atol)`。
2. 如何处理 flaky？——找根因隔离，不靠无脑重试。
3. 回归测试的 oracle 从哪来？——上一稳定版本或参考实现。

#### 8. 易错点与反例

1. 用 `==` 比较浮点张量，永远失败或永远“通过”。
2. 测试依赖未固定的随机种子，本地过 CI 挂。
3. 用重试把真 bug 掩盖成偶发。

#### 9. 实践与验证

脚本：写一个 `pytest` 用例，用 `assert_close` 比较自定义算子与参考实现；跑两遍固定种子验证一致；故意引入 flaky（依赖时间）观察并修复。oracle 为“用例确定、容差合理、失败可定位”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B022 评分：不合格只会说“写测试”；合格能说清分层与 oracle；良好能处理容差、可复现与 flaky；优秀能设计 CI 门禁、覆盖率、性能与回归对拍治理方案。
### B023. 工作位置：张量/设备

- 难度：入门级
- 知识域：K01 张量/设备
- 考察能力：事件顺序、状态迁移、所有权与观测点
- 题型：流程/调用链/数据流
- 面试频率：高频
- 前置知识：shape、stride、dtype、device
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

“张量/设备”工作位置指张量的创建、搬运、连续化、dtype 变换在流程中发生的位置：数据准备阶段（pin、to device）、前向算子内部（隐式连续化/类型提升）、loss 与指标（`.item()` 同步）、checkpoint（`.cpu()` 保存）。

#### 2. 问题分析

按流程定位每个 device/dtype 变更点：数据加载→上设备→算子→归约→读值→保存。不变量：进入 GPU 计算前所有张量应同 device、合适 dtype、尽量连续。边界：在算子内部隐式发生的拷贝/转换不易察觉，是性能与正确性隐患。

#### 3. 具体答案

面试中可以这样回答：
> 张量和设备相关的工作散落在流程的几个固定位置。第一，数据准备：`pin_memory` 把数据放到页锁定内存，`non_blocking=True` 在真正要用时才搬到 GPU，这是 H2D 的最佳位置。第二，前向算子：类型提升会把 fp16 和 fp32 输入提升到 fp32，非连续张量可能触发隐式 `contiguous`，这些拷贝发生在算子内部、最难发现，要靠 Profiler 里的 `Memcpy`/`Contiguous` 定位。第三，loss 与指标：`loss.item()`、`float(x)` 会同步设备读值，应放到评估或低频日志，不要每步调用。第四，保存：checkpoint 前把张量 `.cpu()`，既避免 GPU 显存常驻又能随权重一起序列化；注意保存 `state_dict` 时张量仍在原 device，加载后要 `to(device)`。治理原则是把“设备与 dtype 边界”显式写在数据入口，避免让算子内部隐式转换承担。

#### 4. 技术洞察

不变量：算子要求输入同 device、同 dtype（否则按提升规则转换，可能引入拷贝）。可验证信号：Profiler 中 `Memcpy HtoD`/`Memcpy DtoD` 与 `aten::to`、`aten::contiguous` 出现的位置和耗时；`.item()` 造成的同步点。

#### 5. 图文说明

| 流程位置 | 典型操作 | 风险 |
|---|---|---|
| 数据准备 | pin + non_blocking .to | 忘记 pin 则同步 |
| 前向算子 | 类型提升/隐式 contiguous | 隐式拷贝拖慢 |
| 指标 | .item() | 同步打断流水 |
| 保存 | .cpu() | 漏 cpu 导致显存常驻 |

图示解释：数据流时间线，标注每个 device/dtype 变更点及是否同步。

#### 6. 拓展知识

延伸：`memory_format`、`to(memory_format=channels_last)`、D2D 拷贝、`torch.cuda.Stream` 隔离、checkpoint 的 map_location。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 隐式拷贝怎么发现？——Profiler 看 `aten::contiguous`/`Memcpy`。
2. `.item()` 放哪里合适？——低频日志或评估段。
3. 保存时要不要 `.cpu()`？——推荐，避免显存常驻且便于加载。

#### 8. 易错点与反例

1. 每步 `loss.item()` 记录，GPU 利用率骤降。
2. 数据没 pin 却开 non_blocking，异步未生效。
3. 保存 checkpoint 保留 GPU 张量，训练脚本因此 OOM。

#### 9. 实践与验证

脚本：用 Profiler 打开 `record_shapes`，统计一个训练步里 `aten::to`/`aten::contiguous` 的次数与耗时；把 `.item()` 移出循环对比墙钟。oracle 为“隐式拷贝点被显式定位、同步点减少后变快”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B023 评分：不合格只会说“要放 GPU”；合格能指出数据入口与保存位置；良好能用 Profiler 定位隐式拷贝与同步；优秀能设计显式的 device/dtype 边界规范并给出性能证据。
### B024. 工作位置：autograd

- 难度：入门级
- 知识域：K02 autograd
- 考察能力：事件顺序、状态迁移、所有权与观测点
- 题型：流程/调用链/数据流
- 面试频率：高频
- 前置知识：requires_grad、leaf、view、detach、反向
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

autograd 的工作位置：前向时每个可微算子创建 `grad_fn` 节点并保存反向所需张量；由 `requires_grad` 决定是否建图；`loss.backward()` 触发引擎从根节点按拓扑序反向，调用各 `grad_fn` 计算并对叶子 `.grad` 累加。`torch.no_grad`/`detach` 在流程中设定“此处不建图”。

#### 2. 问题分析

按训练步定位：进入前向（建图开始）→算子（累积 grad_fn）→loss（图根）→backward（反向遍历）→叶子 .grad→step 前 zero_grad。不变量：图中每个节点对应一个算子，边是梯度路径；仅叶子的 .grad 持久。边界：in-place 修改被保存张量会触发版本检查错误。

#### 3. 具体答案

面试中可以这样回答：
> autograd 在训练步里有明确的工作位置。前向一进入可微计算，就为每个算子创建携带 `grad_fn` 的节点，并把反向需要的中间张量保存下来——这一步决定显存峰值。`requires_grad` 是建图开关；用 `no_grad`/`detach` 的地方不建图，能省显存。loss 是图的根，`backward()` 让引擎从根按拓扑序反向遍历，逐节点用链式法则算梯度，最终累加到叶子张量（你创建的、`requires_grad=True` 且无 grad_fn 的 Parameter/输入）的 `.grad` 上。非叶子中间量默认不留梯度。因为 .grad 是累加的，优化器 step 之前必须 `zero_grad()`，否则跨步叠加。还有一条硬约束：前向保存过的张量不能在原地被改（如 `x += 1`），否则反向时版本检查报错，正确写法是用 `x = x + 1` 或 `no_grad` 上下文做原地更新。

#### 4. 技术洞察

不变量：梯度只写叶子；`backward` 默认释放图；版本计数器保证“保存的张量未被 in-place 修改”。可验证：`y=x*x; y.backward(torch.ones_like(y))` 后 `x.grad==2x`；`x += 1` 后再反传触发 `a variable needed for gradient computation has been modified by an inplace operation`。

#### 5. 图文说明

| 流程位置 | autograd 行为 | 证据 |
|---|---|---|
| 前向算子 | 建节点、保存张量 | grad_fn 链 |
| loss | 图根 | `loss.grad_fn` 非 None |
| backward | 反向遍历、写叶子 .grad | `p.grad` |
| step 前 | zero_grad 清零 | .grad 归零 |

图示解释：前向链与反向链两条箭头方向相反，叶子 .grad 是汇聚桶。

#### 6. 拓展知识

延伸：`retain_graph`、`register_hook`、`torch.autograd.grad`、梯度检查点（前向不存中间量、反向重算）、DDP 在 backward 时触发通信。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 为什么非叶子默认没 .grad？——中间量梯度不持久，需 `retain_grad`。
2. in-place 修改为什么报错？——保存的张量被改，版本检查失败。
3. 梯度检查点工作在哪？——前向不保存、反向重算以换显存。

#### 8. 易错点与反例

1. 在需要梯度的计算图上做 `x += ...`，反传报版本检查错误。
2. 复用图两次却未 `retain_graph=True`。
3. 忘记 `zero_grad`，梯度跨步累加。

#### 9. 实践与验证

脚本：`x=torch.tensor([2.],requires_grad=True); y=x*x; y.backward(torch.ones_like(y)); print(x.grad)` 应为 4；制造 in-place 修改观察报错；用 `register_hook` 观察反向流经某张量。oracle 为“梯度正确、in-place 报错、hook 被触发”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B024 评分：不合格混用心智模型；合格能说清建图/反传/累加三阶段；良好能解释 in-place 版本检查与非叶子梯度；优秀能结合 gradient checkpoint、DDP 通信、hook 给出流程级设计。
### B025. 工作位置：Module/状态

- 难度：入门级
- 知识域：K03 Module/状态
- 考察能力：事件顺序、状态迁移、所有权与观测点
- 题型：流程/调用链/数据流
- 面试频率：高频
- 前置知识：参数、buffer、模式和 state_dict
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

Module 的工作位置：构造时注册子模块/参数/buffer；前向 `forward` 里使用这些注册对象；`.to/.cuda/.float` 递归作用于参数与 buffer；`state_dict` 保存/加载遍历注册树；`train()/eval()` 切换训练态影响 Dropout/BatchNorm。

#### 2. 问题分析

沿生命周期定位：`__init__` 注册→`forward` 使用→train/eval 切换→to 搬运→state_dict 持久化。不变量：注册后对象由模块统一管理（设备/训练态/dtype 同步）；未注册张量不被管理。边界：在 forward 里临时创建的大张量不进 state_dict，也不会被 to 移动。

#### 3. 具体答案

面试中可以这样回答：
> Module 的状态在生命周期里有几个固定工作位置。`__init__` 里你把子模块、`nn.Parameter`、`register_buffer` 注册进来，注册后就归 Module 统一管理：`to(device)`、`.half()`、`.float()` 会递归作用到所有参数和 buffer，`train()/eval()` 切换会传播到子模块并影响 Dropout、BatchNorm 行为。前向 `forward` 只使用这些注册对象，临时创建的大张量不属于状态，不会被 `state_dict` 保存，也不会被 `to` 搬运。保存/加载时 `state_dict` 遍历注册树得到“名字→张量”，加载按名字和形状匹配，`strict` 控制是否容忍缺失/多余键。所以工程上的原则是：凡是需要跨设备、跨保存、跨 train/eval 保持一致的东西，必须注册成 parameter/buffer；反之临时中间量不要注册，避免污染 state_dict。

#### 4. 技术洞察

不变量：`state_dict().keys()` = Parameter 名 ∪ 持久 buffer 名；`.to()/.train()` 递归覆盖同一集合。可验证：注册前后 `named_buffers()`/`named_parameters()` 变化；`train()` 后 `self.training==True` 且子模块同步。

#### 5. 图文说明

| 生命周期位置 | 动作 | 受影响状态 |
|---|---|---|
| __init__ | 注册 parameter/buffer/submodule | 注册树 |
| forward | 使用注册对象 | 计算图 |
| train/eval | 切换训练态 | Dropout/BN |
| to/half | 递归搬运/转换 | 参数+buffer |
| state_dict | 遍历持久化 | 参数+持久 buffer |

图示解释：一棵注册树，四种操作分别遍历它；临时张量漂浮在树外。

#### 6. 拓展知识

延伸：`apply` 递归函数、`requires_grad_`、EMA 的 buffer 化、`torch.nn.utils.weight_norm`、DDP/FSDP 对注册树的重写。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 为什么临时张量不注册？——避免进 state_dict、避免被 to/train 影响。
2. `to` 会动 buffer 吗？——会，参数和 buffer 都递归搬运。
3. train/eval 影响哪些层？——Dropout、BatchNorm 等行为依赖 training 标志。

#### 8. 易错点与反例

1. 把统计量存成普通属性，eval 时未同步设备导致 device 不一致。
2. eval 忘记调用，Dropout/BN 行为错误、指标偏低。
3. 在 forward 里缓存大张量到 `self.x`（Tensor 属性），既不注册又泄漏显存。

#### 9. 实践与验证

脚本：对比 `model.train()`/`model.eval()` 下 Dropout 输出的均值；`model.to('meta')` 或换 dtype 检查参数与 buffer 同步；打印 `state_dict().keys()` 验证临时张量不在其中。oracle 为“状态集合一致、train/eval 行为可区分”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B025 评分：不合格只写 forward；合格能说注册与生命周期；良好能解释 to/train/state_dict 的递归覆盖；优秀能结合 EMA buffer 化、DDP 包装与泄漏排查给出状态治理方案。
### B026. 工作位置：数据管道

- 难度：入门级
- 知识域：K04 数据管道
- 考察能力：事件顺序、状态迁移、所有权与观测点
- 题型：流程/调用链/数据流
- 面试频率：高频
- 前置知识：Dataset、Sampler、collate、padding、版本
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

数据管道工作位置：`Dataset.__init__` 准备索引/元数据，`__getitem__` 取单样本并做变换，`Sampler` 决定顺序，`DataLoader` 在 worker 进程调用前两者并 collate，主进程把 batch 放入 pin 内存、再 `.to(device)`。`worker_init_fn` 在每个 worker 开始时设置。

#### 2. 问题分析

按位置定位：主进程构造 loader→worker 进程取数（并行）→主进程组 batch→pin→上设备。不变量：同一 batch 内样本形状需可 collate；worker 间数据不重叠；随机流可复现。边界：worker 内不得依赖父进程可变全局状态。

#### 3. 具体答案

面试中可以这样回答：
> 数据管道的工作位置可以精确到进程。主进程构造 `DataLoader`，按 `Sampler` 生成索引；若 `num_workers>0`，索引被分发给 worker 子进程，每个 worker 在自己的进程里调用 `Dataset.__getitem__` 取样本并做变换（增强在这里发生）。worker 返回的样本由主进程的 collate 组 batch，默认 stack；变长数据要自定义 `collate_fn`。组好的 batch 若开 `pin_memory` 就放在页锁定内存，在主进程用非阻塞方式上 GPU。几个位置相关的约束：增强必须在 worker 内、且随机源要由 `worker_init_fn` 设置，否则复现不了；worker 之间数据不能重叠，用 `get_worker_info()` 或 `DistributedSampler` 分片；worker 里不要持有父进程的可变全局状态，因为进程隔离。调试时把 `num_workers` 设 0 可让取数在主进程串行执行，便于定位。

#### 4. 技术洞察

不变量：`Σ worker 数据分片 = 全集且互斥`；随机变换由 worker 局部种子决定。可验证：打印各 worker 的 `get_worker_info().id/seed` 与处理的索引集合，检查并集等于全集、交集为空。

#### 5. 图文说明

| 位置 | 进程 | 动作 |
|---|---|---|
| Sampler | 主 | 生成索引序列 |
| __getitem__ | worker | 取样本+增强 |
| collate | 主 | 组 batch |
| pin/to | 主 | 锁定内存+上设备 |

图示解释：主进程与多个 worker 进程的流水线，箭头标注数据流向和分片边界。

#### 6. 拓展知识

延伸：`IterableDataset` 分片、`persistent_workers`、共享内存张量、`prefetch_factor` 的预取位置、`collate_fn` 中做 pad。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 增强发生在哪个进程？——worker 进程。
2. 如何保证 worker 不重复？——按 worker_id/rank 分片。
3. 怎么快速定位管道 bug？——num_workers=0 串行复现。

#### 8. 易错点与反例

1. 在 `__getitem__` 里用全局 `random` 而 worker_init_fn 未设种，增强重复。
2. worker 里打开的文件/连接未关闭，进程退出时 fd 泄漏。
3. 把大对象当全局常量放在 Dataset，worker 复制导致内存膨胀。

#### 9. 实践与验证

脚本：打印各 worker id 与索引集合验证分片；`num_workers=0` vs >0 对比增强是否可复现；对比 collate 默认与自定义在变长输入下的行为。oracle 为“分片互斥覆盖全集、可复现、变长可 batch”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B026 评分：不合格只知 DataLoader 能多进程；合格能标出各阶段进程归属；良好能解释分片与 worker 随机流；优秀能结合流式数据、预取与共享内存给出可调试的管道设计。
### B027. 工作位置：训练/优化器

- 难度：入门级
- 知识域：K05 训练/优化器
- 考察能力：事件顺序、状态迁移、所有权与观测点
- 题型：流程/调用链/数据流
- 面试频率：高频
- 前置知识：forward、backward、累积、optimizer、scheduler
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

训练循环工作位置（每步顺序）：`optimizer.zero_grad()` → 前向 → 计算 loss → `loss.backward()` →（可选梯度裁剪）→ `optimizer.step()` → `scheduler.step()`。优化器持有参数引用与状态；scheduler 根据 epoch/step 调整 lr。梯度累积时把 `zero_grad/step` 移到累积周期边界。

#### 2. 问题分析

按步内顺序定位每个操作，强调顺序不变量：必须清零后再反传、反传后 step、step 后调 scheduler（若按 step）。边界：梯度累积把 step 频率降低；AMP 下裁剪需在 unscale 后。错误顺序（如先 step 后 zero_grad）会静默改变语义。

#### 3. 具体答案

面试中可以这样回答：
> 训练循环的工作位置有严格顺序。每一步是：先 `optimizer.zero_grad()` 清空上一步梯度，再前向算 loss，然后 `loss.backward()` 把梯度写进各叶子 `.grad`，需要时对梯度做裁剪或缩放反归一（AMP 下先 `scaler.unscale_`），接着 `optimizer.step()` 用梯度更新参数，最后 `scheduler.step()` 更新学习率。顺序不能乱：先 step 后 zero_grad 会在下一次反传时叠加上一步梯度；先 zero_grad 后没反传就 step 相当于用零梯度空转。梯度累积的场景下，把 zero_grad 和 step 放到累积周期的开头和结尾，中间连续 backward，从而用较小显存模拟大 batch，此时学习率通常要按累积步数相应调整。评估阶段则切到 `eval()` 并用 `no_grad`/`inference_mode`，避免建图。

#### 4. 技术洞察

不变量：`step` 使用且只使用当前累积周期的梯度；scheduler 与 step 计数对齐。可验证：打印每步 `param.grad` 与 `param` 差值验证更新方向；梯度累积 K 步后与单步 K 倍 batch 的等价性（在合适归一化下）。

#### 5. 图文说明

| 顺序 | 操作 | 破坏后果 |
|---|---|---|
| 1 | zero_grad | 漏则梯度累加 |
| 2 | forward+loss | - |
| 3 | backward | - |
| 4 | unscale/clip | 顺序错则裁剪无效 |
| 5 | step | - |
| 6 | scheduler.step | 顺序错则 lr 错位 |

图示解释：环形步进图，每步六节点按序执行，标出累积周期边界。

#### 6. 拓展知识

延伸：`torch.cuda.amp` 与 `GradScaler` 顺序、梯度裁剪 `clip_grad_norm_`、OneCycle 与 warmup、EMA 更新位置、DDP 的 `no_sync` 累积。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 为什么 zero_grad 要在 backward 前？——否则叠加历史梯度。
2. 梯度累积怎么调 lr？——按累积步缩放或归一化 loss。
3. scheduler 何时 step？——与 step/epoch 计数对齐，勿错位。

#### 8. 易错点与反例

1. 把 `scheduler.step()` 放到 `optimizer.step()` 前，lr 用错。
2. 梯度累积后忘记除以累积步数，等效放大 lr。
3. AMP 下先裁剪后 unscale，裁剪阈值失准。

#### 9. 实践与验证

脚本：写单步训练，断言 zero_grad 后 `.grad` 为 None/0、step 后参数变化方向与 `-lr*g` 一致；梯度累积 K 步验证与 batch*K 的梯度近似成比例。oracle 为“更新方向正确、累积等价”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B027 评分：不合格顺序随意；合格能列出正确顺序；良好能解释每个错序的后果；优秀能结合梯度累积、AMP、调度器与 DDP 给出正确且可验证的训练循环设计。
### B028. 工作位置：AMP/恢复

- 难度：入门级
- 知识域：K06 AMP/恢复
- 考察能力：事件顺序、状态迁移、所有权与观测点
- 题型：流程/调用链/数据流
- 面试频率：高频
- 前置知识：autocast、scaler、快照、随机状态
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

AMP/恢复的工作位置：前向的 `autocast` 上下文决定算子精度；loss 后用 `scaler.scale(loss).backward()`；step 前 `scaler.unscale_(optimizer)`；`scaler.step(optimizer)`+`scaler.update()` 完成自适应缩放；checkpoint 保存 `model.state_dict`、`optimizer.state_dict`、`scaler.state_dict`、epoch/步数与 RNG 状态。

#### 2. 问题分析

按训练步 + 断点生命周期定位：autocast 包住前向；scale 包住 loss；unscale→step→update 收尾；保存/加载覆盖四类状态。不变量：unscale 必须在裁剪与 step 前；恢复时四类状态一致才能复现。边界：只存模型权重无法续训。

#### 3. 具体答案

面试中可以这样回答：
> AMP 的工作位置在训练步里是固定的：`with autocast(): out = model(x)` 让白名单算子在低精度下跑、其余自动保持 fp32；`loss = criterion(out, y)` 后在 fp32 下算；外层 `scaler.scale(loss).backward()` 把梯度放大防下溢；更新前 `scaler.unscale_(optimizer)` 除回原尺度（要裁剪就在这之后 `clip_grad_norm_`）；然后 `scaler.step(optimizer)` 与 `scaler.update()` 完成更新与缩放因子的自适应。断点恢复的工作位置在 checkpoint：除了 `model.state_dict()` 和 `optimizer.state_dict()`，还要存 `scaler.state_dict()`（含 scale、growth/backoff、growth_tracker）、当前 epoch/global step，以及 RNG 状态（CPU/CUDA）才能让续训与不中断训练一致。漏存 scaler 会让 scale 回到初值、前期可能反复跳步；漏存 RNG 会让 dropout/增强流错位。

#### 4. 技术洞察

不变量：`unscale` 在 `step` 前、裁剪在 `unscale` 后；恢复集 = {model, optimizer, scaler, epoch/step, rng}。可验证：保存后加载续训 5 步，与不中断跑同样 5 步比较 loss 逐位一致（确定性算法下）。

#### 5. 图文说明

| 位置 | 调用 | 状态 |
|---|---|---|
| 前向 | autocast | 算子精度选择 |
| loss | scaler.scale | 放大梯度 |
| 更新前 | scaler.unscale_ | 还原尺度/裁剪 |
| 收尾 | step+update | 自适应 scale |
| checkpoint | 四种 state_dict + rng | 可续训 |

图示解释：训练步流水线旁挂一条“状态持久化”支线，标出必须保存的五类对象。

#### 6. 拓展知识

延伸：bf16 无需 scaler、`torch.amp.autocast(device_type)`、FSDP/DDP 下 scaler 与分片状态、断点续训的 sampler/RNG。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. unscale 与裁剪的先后？——先 unscale 后裁剪。
2. 恢复要存哪些？——model/optimizer/scaler/step/rng。
3. 漏存 scaler 会怎样？——scale 跳变、前期跳步。

#### 8. 易错点与反例

1. 保存只存 model.state_dict，续训 optimizer 动量与 scaler 丢失。
2. 在 autocast 内计算并回传 loss，数值不稳。
3. 恢复后未恢复 RNG，dropout 流错位导致曲线接不上。

#### 9. 实践与验证

脚本：训练若干步保存完整 checkpoint，重启加载后比较紧接着的 loss/权重与不中断运行一致；单独制造“只存模型”的恢复验证 loss 曲线偏移。oracle 为“完整状态续训逐位一致”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B028 评分：不合格只知 autocast；合格能说 scaler 顺序；良好能列出完整恢复状态；优秀能结合自适应 scale、RNG、分布式给出可复现的断点续训方案。
### B029. 工作位置：分布式

- 难度：入门级
- 知识域：K07 分布式
- 考察能力：事件顺序、状态迁移、所有权与观测点
- 题型：流程/调用链/数据流
- 面试频率：高频
- 前置知识：rank、DDP、FSDP、collective、容错
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

分布式工作位置：`init_process_group` 建立通信组→每个 rank 设置本地设备→`DistributedSampler` 分片数据→`DDP(model)` 包装→前向后向时 DDP 的梯度 hook 触发 AllReduce→保存时 rank 0 写盘、`barrier` 对齐。FSDP 在此基础上分片参数/梯度/优化器状态。

#### 2. 问题分析

按位置定位：初始化→数据分片→模型包装→前向→反向通信→同步/保存。不变量：所有 rank 看到一致的参数（反向 AllReduce 保证）；数据分片互斥覆盖全集。边界：rank 间集合通信必须成对，缺失或形状不一致会 hang；只在 rank 0 保存要其余 rank 等待。

#### 3. 具体答案

面试中可以这样回答：
> 分布式的工作位置分布在训练流程的固定节点。第一步 `init_process_group`（NCCL）建立通信组、给每个 rank 绑定本地 GPU；数据上用 `DistributedSampler` 把索引按 rank 分片，保证各 rank 看到不同样本、并集是全集。模型用 `DDP(model)` 包装，DDP 在构造时广播参数保证初始一致，并在反向传播时为每个参数注册梯度 hook，算完就对该参数梯度做 AllReduce，使各副本梯度一致、参数在 `step` 后继续保持一致。需要跨 rank 同步的点（如保存 checkpoint、评估汇总）用 `dist.barrier()` 或 `all_reduce` 对齐；保存通常只在 rank 0 执行，其余 rank 等待，避免多进程写同一文件。FSDP 的工作位置更进一步：它在构造时分片参数、前向按需 all-gather、反向 reduce-scatter 梯度、优化器状态也分片，从而把显存摊到各卡。诊断上，集合通信调用必须所有 rank 成对出现且张量形状一致，否则直接 hang。

#### 4. 技术洞察

不变量：DDP 后各 rank 参数逐位一致（同初值 + 相同 AllReduce 梯度）；`DistributedSampler` 确保 `Σ 分片 = 数据集`。可验证：训练一步后打印各 rank 参数哈希一致；打印 sampler 索引集合互斥。hang 的常见信号是某 rank 抛异常早退导致其他 rank 卡在 AllReduce。

#### 5. 图文说明

| 位置 | 操作 | 一致性保证 |
|---|---|---|
| 初始化 | init_process_group | 通信组/设备绑定 |
| 数据 | DistributedSampler | 分片互斥覆盖 |
| 模型 | DDP 包装 | 初始参数广播一致 |
| 反向 | 梯度 hook AllReduce | 梯度一致 |
| 保存/同步 | rank0 + barrier | 无重复写、对齐 |

图示解释：多 rank 水平流水线，反向阶段每个参数处画一条横向 AllReduce 箭头汇聚。

#### 6. 拓展知识

延伸：`no_sync` 梯度累积、`find_unused_parameters`、FSDP/ZeRO 分片位置、`all_gather` 评估指标、`torchrun` 启动与 rank 环境变量。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 各 rank 参数为何总一致？——初始广播+反向 AllReduce。
2. 数据如何不重复？——DistributedSampler 按 rank 分片。
3. 为什么会 hang？——集合通信不成对或形状不一致。

#### 8. 易错点与反例

1. 不用 DistributedSampler，各 rank 重复同一数据，等效减小 batch。
2. 只有某 rank 走到的分支里调用集合通信，其他 rank 未调用→hang。
3. 所有 rank 同时写同一 checkpoint 文件，损坏或覆盖。

#### 9. 实践与验证

脚本：`torchrun --nproc_per_node=2` 跑一步，打印各 rank 参数哈希验证一致；打印 sampler 索引集合验证互斥；构造“仅 rank0 调用 all_reduce”验证 hang（设超时）。oracle 为“参数一致、分片互斥、非法通信会超时”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B029 评分：不合格只会说“多卡”；合格能说 DDP 平均梯度；良好能标出初始化/分片/反向通信位置；优秀能结合 FSDP 分片、no_sync、集合通信对称性与 hang 诊断给出落地设计。
### B030. 工作位置：编译栈

- 难度：入门级
- 知识域：K08 编译栈
- 考察能力：事件顺序、状态迁移、所有权与观测点
- 题型：流程/调用链/数据流
- 面试频率：高频
- 前置知识：Dynamo、AOTAutograd、Inductor、guard、缓存
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

编译栈工作位置：首次调用时 Dynamo 捕获 Python 成 FX 图（捕获位置在函数入口与算子边界）；AOTAutograd 在捕获后生成前向/反向图；Inductor 在编译期把图 lowering 成 Triton/C++ kernel；产物按 guard 缓存，后续调用命中缓存直接跑编译产物，guard 失效则重编译。

#### 2. 问题分析

按时间线定位：首次（捕获+编译，慢）→缓存命中（快）→guard 失效（重编译）。不变量：编译产物与 guard 条件绑定；图断裂处回退 eager。边界：数据依赖控制流、外部调用、动态 shape 是常见断点与重编译源。

#### 3. 具体答案

面试中可以这样回答：
> 编译栈的工作位置分三段。第一段在首次调用：Dynamo 从被装饰函数的入口开始逐字节码捕获，遇到不支持的 Python 语义（数据依赖分支、`.item()`、未支持的库调用）就 graph break，把这段留回 eager；支持的部分形成 FX 图。第二段紧随其后：AOTAutograd 基于图生成前向和反向图，Inductor 再把图 lowering 成融合后的 Triton/C++ kernel，这一步是真正“编译”发生的地方，也是编译延迟所在。第三段在后续调用：产物按 guard（输入 shape、dtype、device、Tensor 元数据等）缓存，命中就跳过捕获和编译直接执行；一旦 guard 不满足（比如 shape 变了）就在新 guard 下重新走一遍捕获+编译，这就是 recompilation。工程上要让热点路径“编译一次、长期命中”：固定输入签名、减少 graph break、避免在热点里读张量值。观测手段是 `TORCH_LOGS=graph_breaks,recompiles` 和 `torch._dynamo.explain`。

#### 4. 技术洞察

不变量：缓存键 = guard 集合；`命中 ⟺ 输入满足所有 guard`。收益来自编译期融合（减少 kernel 数与中间张量 HBM 往返），成本是编译延迟按图规模和重编译次数累积。可验证：同输入重复调用观察第二次显著变快；改变 shape 观察重编译日志。

#### 5. 图文说明

| 阶段 | 时机 | 产物/成本 |
|---|---|---|
| 捕获 | 首次调用 | FX 图 / graph break |
| 微分+代码生成 | 首次调用 | 前反向图+kernel（编译延迟） |
| 缓存命中 | 后续调用 | 低开销执行 |
| 重编译 | guard 失效 | 再走捕获+编译 |

图示解释：状态机图“首次编译→命中→失效重编”，旁路是 graph break 回退 eager。

#### 6. 拓展知识

延伸：`mode='reduce-overhead'` 的 CUDA Graph、动态 shape 的符号化、`fullgraph=True`、编译缓存与 AOTInductor 落盘。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 编译延迟花在哪？——捕获、微分、代码生成。
2. 何时重编译？——输入不满足 guard（shape/dtype/device 等）。
3. graph break 影响？——该段退回 eager，削弱融合收益。

#### 8. 易错点与反例

1. 热点里 `if tensor.item()>0` 分支，每次 graph break。
2. 输入 shape 不断变化，guard 反复失效。
3. 把编译当零成本，短任务编译开销大于收益。

#### 9. 实践与验证

脚本：装饰一个函数，首次与第二次调用计时对比；用 `TORCH_LOGS=recompiles` 观察改 shape 时是否重编译；用 `explain` 数 graph break。oracle 为“第二次显著变快、shape 变化触发重编译日志”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B030 评分：不合格只会说“compile 加速”；合格能说捕获/编译/缓存三阶段；良好能解释 graph break 与 guard 重编译；优秀能结合 CUDA Graph、动态 shape、编译缓存给出可观测的编译期治理。
### B031. 工作位置：性能

- 难度：入门级
- 知识域：K09 性能
- 考察能力：前置条件、后置条件、断言与错误处理
- 题型：代码/伪代码/配置分析
- 面试频率：高频
- 前置知识：关键路径、带宽、算力、通信、显存、统计
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

性能“工作位置”指应该在哪里度量：数据加载、前向、反向、优化器、通信、同步。工具是 `torch.profiler`（kernel/算子时间、shape、显存）、`nvidia-smi`/`ncu`、以及手动 `torch.cuda.Event` 计时。`torch.profiler.schedule` 控制 warmup/active 采样窗口，避免把冷启动和编译开销算进去。

#### 2. 问题分析

先确定“瓶颈环节”再选工具：GPU 空档→看数据管道；kernel 密集→看 kcu/Profiler 算子占比；显存峰值→看 memory timeline；多卡→看通信占比。不变量：计时必须排除 warmup 且同步；否则测到的是启动/异步提交延迟。边界：只测端到端不看分段，无法定位。

#### 3. 具体答案

面试中可以这样回答：
> 性能问题要先找到工作位置。我通常按分段计时：数据加载、H2D、前向、反向、优化器 step、通信，每段用 `torch.cuda.Event` 或 Profiler 量化，先确认哪段占比最大。工具选择对应位置：GPU 利用率有周期性空档就去看 `DataLoader` 和 CPU；kernel 时间集中就去 `torch.profiler` 看 top 算子和 `record_shapes`，必要时用 `ncu` 看 occupancy/带宽；显存峰值看 memory timeline；多卡看 NCCL 通信占比。关键是排除干扰：Profiler 要设 `schedule` 只采样中间几步、先 warmup 几轮，手动计时要在前后插事件并 `synchronize`，否则测到的是异步提交时间。没有分段就只有一个端到端数字，无法定位瓶颈。

#### 4. 技术洞察

不变量：`T_e2e = T_data + T_h2d + T_fwd + T_bwd + T_opt + T_comm + 同步残差`（可重叠项取 max）；正确计时需 `warmup` + 同步。可验证：Profiler 的 kernel 时间之和与端到端比例合理；关掉数据管道观察 GPU 空档是否消失。

#### 5. 图文说明

| 环节 | 工具 | 典型信号 |
|---|---|---|
| 数据 | Profiler/时间戳 | GPU 空档、CPU 打满 |
| 前向/反向 | Profiler/ncu | 算子占比、带宽/算力 |
| 优化器 | Profiler | step 时间、状态读写 |
| 通信 | NCCL 日志 | AllReduce 占比 |

图示解释：端到端时间轴按环节着色，标出可重叠与串行区间。

#### 6. 拓展知识

延伸：`record_function` 自定义区间、`ProfilerActivity.CUDA`、memory snapshot、`torch.cuda.memory_stats`。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 为什么要 warmup？——排除初始化和编译开销。
2. 怎么区分数据瓶颈？——GPU 空档+CPU 忙。
3. Profiler 影响测量吗？——有开销，需采样窗口。

#### 8. 易错点与反例

1. 不做 warmup 直接计时，把首次编译/分配算进平均。
2. 计时不同步，测到异步提交的假时间。
3. 只看端到端不分段，误判瓶颈。

#### 9. 实践与验证

脚本：用 `torch.profiler.profile(schedule=..., activities=[CPU,CUDA])` 跑若干步并 `key_averages().table()`；用 `Event` 分段计时前后加 `synchronize`；对比 warmup 前后计时差异。oracle 为“分段占比合理、warmup 后稳定”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B031 评分：不合格只会跑总时间；合格能分段计时并选工具；良好能排除 warmup/异步干扰；优秀能结合 Profiler/ncu/memory timeline 与多卡通信给出定位证据链。
### B032. 工作位置：导出部署

- 难度：入门级
- 知识域：K10 导出部署
- 考察能力：前置条件、后置条件、断言与错误处理
- 题型：代码/伪代码/配置分析
- 面试频率：高频
- 前置知识：state_dict、Export、runtime、schema、迁移
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

导出部署的“工作位置”在训练完成与线上推理之间：`torch.export`/`torch.onnx.export` 生成图，可选 TorchScript，随后由 ONNX Runtime/TensorRT 加载。配置项包括 opset、dynamic_axes、输入名字与 shape，以及导出前的 `eval()` 与 `no_grad`。

#### 2. 问题分析

按流水线定位：`eval()` 冻结->dummy input 追踪->声明动态轴->导出图->校验。不变量：导出在 `eval()` 下进行（避免 dropout/BN 训练态）；导出图对声明输入集合数值等价。边界：模型含未支持算子或数据依赖控制流则导出失败/错误。

#### 3. 具体答案

面试中可以这样回答：
> 导出的工作位置是“训练结束后、上推理引擎前”。正确顺序是：先把模型 `.eval()` 并放 CPU（或目标设备），构造符合契约的 dummy input，声明动态轴（如 batch 维），调用 `torch.onnx.export` 或 `torch.export`。关键契约有三：一是必须在 eval 模式导出，否则 dropout/BatchNorm 会把训练态行为固化进图；二是输入签名要写清名字、dtype、shape 与哪些轴是动态的，这等价于给下游引擎一份 I/O 契约；三是导出后必须校验：`onnx.checker.check_model` 查结构合法，再用 ONNX Runtime 在多组输入（不同 batch/seq）上与 PyTorch 输出 `allclose`。opset 要按目标引擎能力选，不是越高越好。若模型有数据依赖分支或 `numpy` 操作，导出会失败或产生错误图，需要改写成张量算子。

#### 4. 技术洞察

不变量：导出图在声明的动态轴取值域内数值等价于 eager；eval 模式冻结行为。可验证：`checker` 通过 + 多 shape `allclose`。复杂度：图节点数决定序列化大小与优化时间，动态轴越多优化空间越小。

#### 5. 图文说明

| 步骤 | 调用 | 契约 |
|---|---|---|
| 冻结 | model.eval() | 无训练态算子 |
| 追踪 | dummy input | 输入 dtype/shape |
| 导出 | export(..., dynamic_axes) | 动态轴声明 |
| 校验 | checker + ORT allclose | 数值等价 |

图示解释：流水线图 训练态→eval→图→引擎，契约标注在箭头处。

#### 6. 拓展知识

延伸：`torch.export` 的 `dynamic_shapes` 与 `Dim`、AOTInductor、量化导出、TensorRT 引擎构建与精度校验。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 为什么必须 eval 后导出？——避免固化 dropout/BN 训练态。
2. 动态轴怎么选？——只放开真正变化的维（batch/seq）。
3. 如何证明导出对？——checker + 多 shape allclose。

#### 8. 易错点与反例

1. 忘记 `eval()`，导出图里带 Dropout，推理结果随机。
2. dummy input 形状写死，线上换 batch 报 shape 错误。
3. 只校验结构不做数值对比，漏掉算子语义偏差。

#### 9. 实践与验证

脚本：导出前后用同一输入比较 `allclose`；用 `onnx.checker` 校验；在 batch=1/4 两组输入上跑 ORT 对比；故意在 train 模式导出观察输出不稳定。oracle 为“结构合法、多 shape 等价”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B032 评分：不合格只会调 export；合格能说 eval+checker；良好能设计动态轴与多 shape 校验；优秀能结合 opset、量化、后端引擎与精度对拍给出可部署导出流程。
### B033. 工作位置：自定义算子

- 难度：入门级
- 知识域：K11 自定义算子
- 考察能力：前置条件、后置条件、断言与错误处理
- 题型：代码/伪代码/配置分析
- 面试频率：高频
- 前置知识：schema、dispatcher、stream、autograd、ABI
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

自定义算子的“工作位置”在性能/功能瓶颈处：把不肯优化的 Python 组合替换为融合 kernel。实现路径：`torch.autograd.Function`（带反向的 Python 算子）、`torch.library.custom_op`（schema 注册）、C++/CUDA 扩展。配置包括是否可微、设备、编译标志。

#### 2. 问题分析

按流程定位：分析热点算子->设计 schema（I/O、别名、是否可变）->实现 kernel->注册反向->gradcheck->接入模型。不变量：schema 决定 dispatcher 路由；反向数值需与前向自洽。边界：不可微部分要用 `no_grad` 或返回 None 梯度。

#### 3. 具体答案

面试中可以这样回答：
> 自定义算子的工作位置是“已被 Profiler 证明的热点”和“现有算子表达不了的语义”。流程是：先用 profiler 确认某段 Python 组合是瓶颈，然后把这段定义成一个算子。用 `torch.autograd.Function` 时写 `forward` 和 `backward`，`backward` 的返回值要按 `forward` 的输入顺序给出（不可微的返回 None）；用 `torch.library` 则是注册 schema 和实现，工具会按 device/dtype 分派。有几个契约必须显式：输入/输出是否共享存储（别名）、是否原地修改输入、支持哪些 dtype/device。实现完成后用 `torch.autograd.gradcheck` 做数值梯度对拍，再用 `torch.library.opcheck` 检查 schema、别名和 autograd 行为，最后接进模型验证端到端。注意不要为“看起来会更快的想象”写算子，一定要有 profile 证据。

#### 4. 技术洞察

不变量：自定义算子的 `backward` 与 `forward` 在数值上必须满足 `gradcheck`；schema 的别名/可变性声明必须与实现一致。可验证：`gradcheck(fn, inputs, eps=1e-6, atol=1e-4)` 通过；`opcheck(fn, args)` 全绿。

#### 5. 图文说明

| 步骤 | 工具 | 产出 |
|---|---|---|
| 定位 | profiler | 热点算子 |
| 定义 | autograd.Function/library | schema+前后向 |
| 校验 | gradcheck/opcheck | 数值与契约 |
| 集成 | 模型/compile | 端到端收益 |

图示解释：从热点到 kernel 的流水线，校验作为一个闸门。

#### 6. 拓展知识

延伸：CUDA 扩展编译、`torch.library` 的 `register_autograd`、与 `torch.compile` 的自定义算子捕获（`custom_op` 可被图捕获）。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 什么时候值得写算子？——有 profile 证据且现有算子无法表达/融合。
2. 反向怎么写？——按输入顺序返回梯度，不可微给 None。
3. 如何验证？——gradcheck + opcheck。

#### 8. 易错点与反例

1. 无 profile 证据凭感觉写算子，收益为负还增加维护成本。
2. backward 返回值个数/顺序与输入不符，梯度错配。
3. 声明无别名实则原地改了输入，破坏版本检查与图捕获。

#### 9. 实践与验证

脚本：写一个可微的 `Sigmoid` 风格 Function，`gradcheck` 通过；故意写错 backward 观察 gradcheck 失败；用 `opcheck` 校验 schema。oracle 为“gradcheck/opcheck 通过、错误实现被捕获”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B033 评分：不合格随手写算子；合格能说 autograd.Function 前后向；良好能用 gradcheck/opcheck 校验；优秀能结合 profile 证据、schema 别名与 compile 捕获给出可维护的算子方案。
### B034. 工作位置：测试治理

- 难度：入门级
- 知识域：K12 测试治理
- 考察能力：前置条件、后置条件、断言与错误处理
- 题型：代码/伪代码/配置分析
- 面试频率：高频
- 前置知识：oracle、证据、日志、回滚、门禁、复盘
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

测试治理的“工作位置”在开发流水线的多个闸门：pre-commit 快测、PR 的 CPU 单测、CI 的 GPU 集成测试、发布前的回归与性能门禁。配置包括 pytest 标记、`torch.testing.assert_close` 容差、固定种子、CI 矩阵。

#### 2. 问题分析

按闸门定位测试与 oracle：本地快测、PR 单测、夜间 GPU、发布回归。不变量：每个闸门有明确通过判据与超时；失败可复现。边界：非确定性算子需容差；慢测试放夜间。

#### 3. 具体答案

面试中可以这样回答：
> 测试治理的工作位置是流水线上的多个闸门。pre-commit 跑格式和秒级单测；PR 上跑 CPU 单元测试，覆盖 shape/dtype/device、边界值和 `gradcheck`；CI 的 GPU job 跑集成测试，比如单步训练 loss 有限、checkpoint 往返一致、续训曲线可接；发布前跑端到端小数据过拟合和与上一版本的回归对拍，再加性能门禁。每个闸门要有明确判据：数值用 `assert_close(rtol, atol)`、行为用断言、性能用“不劣于基线 X%”。配置上必须固定随机种子和确定性算法，否则同样的测试会随机红绿；慢用例标 `slow` 放夜间，避免拖垮 PR。对 flaky 用例要单独隔离并记录根因，而不是无脑重试，因为重试会把真 bug 变成偶发。

#### 4. 技术洞察

不变量：闸门的 oracle 独立于被测随机性；`assert_close` 判据 `|a-e| ≤ atol + rtol|e|`。可验证：同一测试重复 N 次全通过（无 flaky）；覆盖率报告；性能基线对比。

#### 5. 图文说明

| 闸门 | 位置 | 覆盖 |
|---|---|---|
| pre-commit | 本地 | 格式、秒级单测 |
| PR CI | 云端 CPU | 单元+gradcheck |
| 夜间 | GPU | 集成+性能 |
| 发布 | 回归 | 端到端+对拍 |

图示解释：流水线闸门图，每道闸门标注 oracle 与超时。

#### 6. 拓展知识

延伸：pytest marker、参数化、`torch.testing`、CI 缓存、golden 更新流程、性能回归（benchmark 门禁）。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 数值怎么判？——容差 assert_close。
2. flaky 怎么治？——找根因隔离。
3. 性能如何门禁？——对比基线设阈值。

#### 8. 易错点与反例

1. 所有测试放 PR，CI 时间过长导致跳过测试。
2. 用精确相等比较浮点。
3. 靠重试掩盖 flaky，放过真 bug。

#### 9. 实践与验证

脚本：搭 pytest 目录，用 marker 区分快慢；`assert_close` 比较两实现；重复运行验证无 flaky；记录性能基线。oracle 为“分层、可复现、判据明确”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B034 评分：不合格只会跑 pytest；合格能说清各闸门；良好能设计容差与复现；优秀能结合 CI 矩阵、性能门禁、flaky 治理给出质量体系。
### B035. 输入输出契约：张量/设备

- 难度：入门级
- 知识域：K01 张量/设备
- 考察能力：前置条件、后置条件、断言与错误处理
- 题型：代码/伪代码/配置分析
- 面试频率：高频
- 前置知识：shape、stride、dtype、device
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

张量/设备算子的 I/O 契约：输入需声明 shape（含可变维）、dtype、device、是否连续；输出需声明 shape、dtype、device；原地算子还需声明是否修改输入。契约可用 `torch.testing.assert_close`、shape 断言、`is_contiguous` 检查来验证。`torch._C._nn` 等底层算子在 device/dtype 不匹配时报清晰错误。

#### 2. 问题分析

按输入/输出两侧列契约：输入约束（必须同 device、可提升 dtype、广播兼容）、输出保证（shape 推导、dtype 提升结果、device 保持）。不变量：二元算子的输出 device = 输入 device，dtype = 类型提升结果，shape = 广播结果。边界：原地算子要求输出形状等于被写张量。

#### 3. 具体答案

面试中可以这样回答：
> 张量运算的 I/O 契约要写清楚四件事。输入侧：所有张量必须同一 device，否则报错；dtype 可以不同但会按提升规则统一；shape 需满足广播；对需要连续布局的算子（如某些 `view`）会要求连续。输出侧：device 与输入一致；dtype 是提升后的结果（如 fp16 + fp32 → fp32）；shape 由广播或算子语义决定。原地算子的契约更强：它要求广播后结果形状等于被写张量，且不接受需要类型转换的输入（会报 “result type ... can't be cast”）。工程上我把它写成入口断言：`assert x.device==y.device`、`assert compute_output_shape(...)==out.shape`，并用 `torch.testing.assert_close(out, ref)` 做数值对拍。这样任何契约破坏都会在最早点暴露，而不是在深层的 kernel 里报模糊错误。

#### 4. 技术洞察

不变量：`device(out)=device(in)`；`dtype(out)=promote(dtypes)`；`shape(out)=broadcast(shapes)`（二元）或算子定义。可验证：`(torch.ones(2,3)+torch.ones(3)).shape==(2,3)`；`(torch.ones(1,dtype=torch.float16)+torch.ones(1,dtype=torch.float32)).dtype==torch.float32`；CPU+GPU 报错。

#### 5. 图文说明

| 侧 | 契约 | 验证 |
|---|---|---|
| 输入 shape | 广播兼容 | broadcast_shapes |
| 输入 dtype | 可提升 | promote 结果检查 |
| 输入 device | 必须一致 | 混用报错 |
| 输出 | shape/dtype/device | assert_close |

图示解释：算子黑盒，左侧输入契约、右侧输出契约、下方原始错误信息。

#### 6. 拓展知识

延伸：类型提升表、`result_type`、`torch.promote_types`、布局契约（channels_last）、`assert_close` 的 `check_device/check_dtype`。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 输出 dtype 怎么定？——按类型提升。
2. 原地算子为何更强约束？——输出 shape/类型须与目标一致。
3. 如何自动校验契约？——入口断言+assert_close。

#### 8. 易错点与反例

1. 混用 CPU/GPU 张量，报错信息在深层不易定位。
2. 假设输出 dtype 等于第一个输入，忽略提升。
3. 原地加广播展宽，报 “output ... doesn't match broadcast shape”。

#### 9. 实践与验证

脚本：断言 `torch.broadcast_shapes`、`torch.promote_types`、混用 device 报错；用 `assert_close(check_dtype=True, check_device=True)` 对拍。oracle 为“契约断言全部成立”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B035 评分：不合格不写契约；合格能说 device/dtype/shape 三项；良好能写出提升与广播规则并用断言校验；优秀能覆盖布局、原地约束与类型提升边界，形成可复用入口校验。
### B036. 输入输出契约：autograd

- 难度：入门级
- 知识域：K02 autograd
- 考察能力：前置条件、后置条件、断言与错误处理
- 题型：代码/伪代码/配置分析
- 面试频率：高频
- 前置知识：requires_grad、leaf、view、detach、反向
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

autograd 的 I/O 契约：输入张量的 `requires_grad` 经由算子传播到输出（任一输入需要梯度则输出需要）；输出的 `grad_fn` 记录反向函数；`backward()` 的输入梯度必须与输出同形状（非标量时显式给 `grad_outputs`）；反向产生的梯度按输入顺序可获取。`torch.autograd.grad` 直接返回指定输入的梯度。

#### 2. 问题分析

按前后向两侧定义契约：前向输出 `requires_grad` 的取值规则、`grad_fn` 归属；反向输入需与输出同形、输出为叶子梯度。不变量：`requires_grad(out) = any(requires_grad(in))`（对可微算子）；`grad` 形状=对应张量形状。边界：输出不需要梯度时无 `grad_fn`。

#### 3. 具体答案

面试中可以这样回答：
> autograd 的契约分前向和反向。前向侧：只要任一输入 `requires_grad=True`，通过可微算子得到的输出就 `requires_grad=True` 且带 `grad_fn`；如果全部输入不需要梯度，输出也不建图。反向侧：`backward` 对非标量输出必须传与之同形状的 `grad_outputs`（标量可省略），回传得到的梯度形状必须等于对应张量的形状，且只写进叶子 `.grad`。如果你要直接拿中间量梯度，用 `torch.autograd.grad(loss, [x], retain_graph=..., allow_unused=...)`，它对每个请求项返回一个梯度（未使用时视 `allow_unused` 返回 None）。工程上我会在自定义算子处用契约断言：输出 `grad_fn` 是否为 None、`torch.autograd.grad` 结果 shape 是否匹配，并用 `gradcheck` 做数值验证。

#### 4. 技术洞察

不变量：`requires_grad` 沿可微算子“或”传播；`grad.shape == tensor.shape`；`torch.autograd.grad` 返回值与请求列表一一对应。可验证：`(x.requires_grad, y=x*2 → y.requires_grad)`；`torch.autograd.grad(y.sum(), x)[0].shape==x.shape`；`allow_unused=True` 时无关输入返回 None。

#### 5. 图文说明

| 侧 | 契约 | 验证 |
|---|---|---|
| 前向要求 | any(in.requires_grad) | out.requires_grad |
| 前向产物 | 可微则带 grad_fn | out.grad_fn |
| 反向输入 | 非标量需 grad_outputs | 形状匹配 |
| 反向输出 | grad 形状=张量形状 | autograd.grad |

图示解释：前向边“或”传播 requires_grad，反向边形状对齐。

#### 6. 拓展知识

延伸：`allow_unused`、`materialize_grads`、`create_graph` 二阶导、`retain_graph`、hook 的输出契约。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 输出何时带 grad_fn？——任一输入需要梯度且算子可微。
2. `autograd.grad` 与 `.backward` 区别？——前者直接返回指定梯度。
3. 无梯度输入返回什么？——`allow_unused=True` 时为 None。

#### 8. 易错点与反例

1. 对非标量直接 backward 不传 grad_outputs，报错。
2. 请求了不参与图的输入却未设 `allow_unused`，报错。
3. 以为中间量 `.grad` 会有值，实际为 None。

#### 9. 实践与验证

脚本：断言 `requires_grad` 传播；对非标量输出尝试 bootstrapping；用 `torch.autograd.grad` 取指定梯度并断言 shape；`allow_unused` 验证 None。oracle 为“传播与形状契约成立”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B036 评分：不合格混淆 forward/backward 契约；合格能说 requires_grad 传播与 grad_fn；良好能写出反向形状与 grad_outputs 约束；优秀能结合 autograd.grad、allow_unused、二阶导与 hook 给出可验证契约。
### B037. 输入输出契约：Module/状态

- 难度：入门级
- 知识域：K03 Module/状态
- 考察能力：前置条件、后置条件、断言与错误处理
- 题型：代码/伪代码/配置分析
- 面试频率：高频
- 前置知识：参数、buffer、模式和 state_dict
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

Module/状态的 I/O 契约：`state_dict()` 返回 `OrderedDict[str, Tensor]`，键是层级路径，值是参数/持久 buffer；`load_state_dict(sd, strict=True)` 要求键集合与形状严格匹配；`parameters()`/`buffers()` 暴露迭代器；`forward` 的输入输出由模型语义定义。`_load_from_state_dict` 是加载的扩展点。

#### 2. 问题分析

按持久化两侧定义契约：保存侧键集合与形状、加载侧匹配规则、strict 语义。不变量：`state_dict` 键 = Parameter ∪ 持久 buffer；加载后逐张量相等或报错。边界：`strict=False` 允许缺失/多余键但会打印提示。

#### 3. 具体答案

面试中可以这样回答：
> Module 的状态契约围绕 `state_dict` 展开。保存侧：`state_dict()` 是一个“键→张量”的映射，键是参数和持久 buffer 的层级路径（`layer1.weight`、`bn.running_mean`），临时张量不在其中。加载侧：`load_state_dict(sd, strict=True)` 按键名匹配，缺键、多键或形状不符都会报错（分别为 missing、unexpected、size mismatch）；`strict=False` 只对形状不符报错，允许缺失/多余但会提示。契约不变量是“加载后每个键对应的张量与保存时数值一致”。跨封装迁移时要注意键前缀：DDP 保存带 `module.`，单卡加载需要去掉或用 `strict=False` 加手动对齐，FSDP 则用 `state_dict_type` 控制导出格式。工程上我会在加载前后对比 `set(state_dict().keys())`，并用 `torch.equal` 抽查关键张量来确认契约成立。

#### 4. 技术洞察

不变量：`keys(state_dict) = keys(parameters) ∪ keys(persistent buffers)`；`load` 后 `torch.equal(saved[k], loaded[k])`。前缀规则：DDP wrapper 引入 `module.`。可验证：人为改 shape 触发 `size mismatch`；缺键触发 missing keys。

#### 5. 图文说明

| 侧 | 契约 | 失败信号 |
|---|---|---|
| 保存 | 键=注册路径 | 缺 buffer 导致推理漂移 |
| 加载 strict | 键+形状完全匹配 | missing/unexpected/size mismatch |
| 前缀 | DDP module. | 单卡加载报键不匹配 |

图示解释：两棵注册树对齐，箭头表示键匹配，错位处报错。

#### 6. 拓展知识

延伸：`register_load_state_dict_pre_hook`、`_load_from_state_dict`、`assign`/`get_extra_state`、EMA buffer 持久化。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 键从哪来？——参数与持久 buffer 的层级路径。
2. strict 的作用？——键与形状的严格校验。
3. DDP 键前缀如何解决？——`module.` 前缀去除或非严格加载映射。

#### 8. 易错点与反例

1. 保存时漏掉 buffer，加载后 running stats 不一致。
2. DDP 权重直接给单卡加载，missing/unexpected keys。
3. 用 `strict=False` 掩盖真实的键错位，模型部分未加载。

#### 9. 实践与验证

脚本：保存后 `load_state_dict` 并 `torch.equal` 抽查；制造缺键/错 shape 观察报错；给模型加 `module.` 前缀验证非严格加载。oracle 为“严格匹配通过、错误被捕获”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B037 评分：不合格只知 save/load；合格能说 state_dict 组成；良好能解释 strict/形状校验与前缀问题；优秀能结合 hook、extra_state、FSDP 格式给出可迁移的状态契约。
### B038. 输入输出契约：数据管道

- 难度：入门级
- 知识域：K04 数据管道
- 考察能力：前置条件、后置条件、断言与错误处理
- 题型：代码/伪代码/配置分析
- 面试频率：高频
- 前置知识：Dataset、Sampler、collate、padding、版本
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

数据管道的 I/O 契约：`Dataset.__getitem__` 返回单样本（张量/字典/元组），`DataLoader` 的 `collate_fn` 决定一批样本如何拼装，产出 batch 的 shape/dtype/device 契约。默认 `default_collate` 对同形状张量 stack、对 int/str 转张量/list。`pin_memory` 影响产出张量是否在页锁定内存。

#### 2. 问题分析

定义两侧契约：Dataset 返回类型与形状、collate 输出 batch 的维度约定（通常首维为 batch）。不变量：batch 首维 = batch size（除 `drop_last`）；collate 输出张量形状一致。边界：变长序列需自定义 collate，否则默认 stack 报错。

#### 3. 具体答案

面试中可以这样回答：
> 数据管道的 I/O 契约有两个接口。第一，`Dataset.__getitem__(i)` 的返回契约：通常返回一个张量或字典/元组，形状是单样本形状，dtype 与 device 明确（一般 CPU）。第二，`collate_fn(samples)` 的契约：它把一批单样本拼成 batch，默认实现要求同形状张量才能 `stack`，产出张量首维等于 batch size；不同长度的序列、不同大小的图必须自定义 collate，常见做法是 pad 到 batch 内最大长度并返回一个 mask。产出 batch 的 device 默认在 CPU，开了 `pin_memory` 则张量在页锁定内存。工程上我会把 batch 契约写成断言：首维等于 batch size、dtype 一致、变长数据附带 mask 且 mask shape 与长度对齐。训练侧再显式 `to(device)`。契约一旦破坏（如某样本返回 list、长度不一），应该在 collate 处就报清晰错误，而不是在模型前向深处才失败。

#### 4. 技术洞察

不变量：`batch.shape[0] == batch_size`（除 `drop_last` 尾部）；`default_collate` 要求批内形状一致。可验证：打印 batch 的 shape/dtype/`is_pinned`；构造变长样本观察默认 collate 报错、自定义后成功。

#### 5. 图文说明

| 接口 | 输入 | 输出契约 |
|---|---|---|
| __getitem__ | index | 单样本张量/结构 |
| collate_fn | list[sample] | batch(首维=batch) |
| pin_memory | batch | 页锁定内存标志 |

图示解释：单样本→批的拼装流程，契约标注在产出箭头。

#### 6. 拓展知识

延伸：`default_collate` 对 dict 的递归、`NestedTensor`、变长 mask 约定、多任务返回结构、`IterableDataset` 的 collate。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. batch 首维含义？——batch size。
2. 变长如何 collate？——pad + mask 或返回 list。
3. `pin_memory` 影响什么？——产出张量的内存类型。

#### 8. 易错点与反例

1. 变长序列用默认 collate，报 stack 维度不一致。
2. 某样本返回 torch 张量、另一个返回 numpy，collate 结果混乱。
3. 断言 batch 首维一定等于设定值，忽略 `drop_last=False` 的尾批较小。

#### 9. 实践与验证

脚本：打印 batch 的 shape/dtype/is_pinned；构造等长与变长样本分别用默认/自定义 collate；验证尾批大小。oracle 为“契约断言成立、变长可处理”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B038 评分：不合格只知 DataLoader 出 batch；合格能说两条接口；良好能写出 batch 契约与变长处理；优秀能结合 mask 约定、pin 内存与分布式分片给出可校验的数据契约。
### B039. 输入输出契约：训练/优化器

- 难度：入门级
- 知识域：K05 训练/优化器
- 考察能力：前置条件、后置条件、断言与错误处理
- 题型：代码/伪代码/配置分析
- 面试频率：高频
- 前置知识：forward、backward、累积、optimizer、scheduler
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

优化器的 I/O 契约：输入是参数（或参数组）迭代器；隐含要求每个可训练参数在 `step()` 前有 `.grad`（否则跳过或用 None）；`state_dict()` 输出参数身份映射到状态。`scheduler` 输入是 optimizer，输出更新后的 lr。参数组可设不同 lr/weight_decay。

#### 2. 问题分析

按两类接口定义契约：参数侧（哪些参数被管理、grad 是否必备、参数组结构）、状态侧（state_dict 与参数索引对应）。不变量：`step()` 只更新有非 None 梯度的参数；`state_dict` 可完整恢复。边界：参数在 step 后加入优化器会破坏 state 与参数索引的稳定映射。

#### 3. 具体答案

面试中可以这样回答：
> 优化器的契约有两侧。输入侧：构造时传入参数或参数组（每个组可有独立 lr、weight_decay），优化器持有这些参数的引用；每一步 `step()` 只会更新“`.grad` 非 None”的参数，没有梯度的参数被跳过——所以冻结层如果不加进优化器，天然不会更新。这里有个硬约束：参数组结构在训练中应稳定，动态增删参数会打乱 `state_dict` 里“参数→状态”的映射。状态侧：`optimizer.state_dict()` 把每个参数的状态（Adam 的 m、v；Momentum 的 buffer）连同参数组超参一起序列化，`load_state_dict` 恢复时按参数顺序对齐，因此模型结构和优化器必须一起保存、一起加载，否则状态会错位。调度器则以 optimizer 为输入，`step()` 后按规则改写各参数组的 lr，其 `state_dict` 记录 step 计数，恢复也要一起保存。

#### 4. 技术洞察

不变量：`step` 跳过 `grad is None` 的参数；`state_dict['state']` 的键与 `param_groups` 顺序绑定。可验证：冻结参数后确认其值不变；保存/恢复 optimizer 后继续训练，参数更新与不中断一致。参数组变更会破坏索引映射。

#### 5. 图文说明

| 侧 | 契约 | 失败信号 |
|---|---|---|
| 参数输入 | 组结构稳定 | 动态增删致状态错位 |
| grad 前提 | step 跳过 None | 误以为所有参数都更新 |
| 状态 | state_dict 可恢复 | 只存模型致续训不接 |

图示解释：optimizer 黑盒左侧参数组、右侧状态字典，中间 step 读写 .grad。

#### 6. 拓展知识

延伸：`add_param_group`、param_groups 超参、`zero_grad(set_to_none=True)`、scheduler 的 per-group lr、FSDP 分片优化器状态。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 无梯度参数会更新吗？——不会，step 跳过。
2. 为什么模型和优化器要一起存？——状态按参数顺序对齐。
3. 参数组能训练中改吗？——尽量避免，会打乱状态映射。

#### 8. 易错点与反例

1. 冻结层仍加入优化器且梯度非 None，参数照样更新。
2. 只保存模型权重，恢复后 Adam 动量丢失。
3. 训练中途 `add_param_group`，旧 state 与新参数错位。

#### 9. 实践与验证

脚本：冻结一组参数确认值不变；保存 optimizer.state_dict 后恢复续训，比较参数轨迹；检查 `zero_grad(set_to_none=True)` 后 `.grad is None`。oracle 为“跳过无梯度、状态可完整恢复”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B039 评分：不合格只会说 optimizer.step；合格能说参数/grad/状态契约；良好能解释 state 与参数顺序绑定、scheduler 输入输出；优秀能结合冻结、参数组稳定、FSDP 给出状态一致性与恢复方案。
### B040. 输入输出契约：AMP/恢复

- 难度：入门级
- 知识域：K06 AMP/恢复
- 考察能力：前置条件、后置条件、断言与错误处理
- 题型：代码/伪代码/配置分析
- 面试频率：高频
- 前置知识：autocast、scaler、快照、随机状态
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

AMP 的 I/O 契约：`autocast` 上下文内算子按白/黑名单决定精度，输入 dtype 可为 fp32，输出 dtype 因算子而异；`GradScaler.scale(loss)` 输入 fp32 loss，输出缩放后的 loss；`unscale_` 就地除以 scale；`step` 返回是否真正更新；`scaler.state_dict()` 输出缩放状态。checkpoint 需同时保存 model/optimizer/scaler 的 state_dict。

#### 2. 问题分析

按两侧定义契约：前向输入输出 dtype、loss 的 dtype、缩放与反缩放、step 是否跳过。不变量：loss 在 autocast 外算、为 fp32；`unscale` 后梯度真实尺度；溢出步被跳过。边界：autocast 内算 loss 会降低数值稳定。

#### 3. 具体答案

面试中可以这样回答：
> AMP 的契约围绕“dtype 边界”和“缩放边界”。输入侧：`with autocast(): out = model(x)`，x 可以是 fp32，autocast 内部把白名单算子降到 fp16/bf16，输出 dtype 依算子而异；`loss` 必须在 autocast 之外、以 fp32 计算，这是数值稳定的前提。缩放侧：`scaler.scale(loss)` 接受 fp32 loss、返回放大后的 loss，反传得到放大梯度；更新前 `scaler.unscale_(optimizer)` 就地除回真实尺度——所以“裁剪要在 unscale 之后”。`scaler.step(optimizer)` 返回一个布尔，表示这一步是否真的执行了更新（溢出时返回 False、跳过并把 scale 减半），`scaler.update()` 再调整 scale。恢复侧：必须保存并加载 `scaler.state_dict()`，与 model、optimizer 的 state_dict 一起，才能保证续训时 scale 与训练动态一致。

#### 4. 技术洞察

不变量：loss 在 fp32/autocast 外计算；`unscale` 在裁剪/step 前；溢出步不更新参数。可验证：`scaler.step` 返回值；`scaler.get_scale()` 随溢出下降、无溢出上升；恢复后 scale 一致。

#### 5. 图文说明

| 接口 | 输入 | 输出契约 |
|---|---|---|
| autocast | fp32 张量 | 白名单低精度输出 |
| scaler.scale | fp32 loss | 放大 loss |
| unscale_ | optimizer | 梯度还原真实尺度 |
| step/update | optimizer | 布尔+自适应 scale |

图示解释：dtype 边界与缩放边界两条竖线，loss 必须落在 autocast 边界之外。

#### 6. 拓展知识

延伸：bf16 契约（无需 scaler）、`torch.amp.autocast(device_type)`、DDP/FSDP 下 scaler 与状态分片、`_growth_tracker` 含义。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. loss 能在 autocast 内算吗？——不建议，数值不稳。
2. 裁剪与 unscale 顺序？——先 unscale。
3. step 返回什么？——是否真正更新。

#### 8. 易错点与反例

1. 在 autocast 内算并回传 loss，出现 nan。
2. 未用 scaler.scale 直接 backward，小梯度下溢。
3. 恢复漏存 scaler 状态，scale 从初值重启。

#### 9. 实践与验证

脚本：打印 `scaler.get_scale()` 初值；制造溢出观察 `step` 返回 False 且 scale 减半；保存/加载 scaler.state_dict 比较一致。oracle 为“dtype 边界、缩放契约、状态恢复均成立”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B040 评分：不合格只知 autocast；合格能说 loss 与缩放边界；良好能解释 unscale/step/update 契约；优秀能结合 bf16、分布式与断点恢复给出完整 AMP 状态契约。
### B041. 输入输出契约：分布式

- 难度：入门级
- 知识域：K07 分布式
- 考察能力：正确性、性能、成本、运维与迁移条件
- 题型：对比与取舍
- 面试频率：高频
- 前置知识：rank、DDP、FSDP、collective、容错
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

分布式的 I/O 契约：`init_process_group` 之后每个 rank 有全局 rank/世界大小；集合通信（all_reduce/all_gather/broadcast/reduce_scatter）要求所有 rank 参与且张量形状/类型匹配；`DistributedSampler` 输入数据集、输出按 rank 分片的索引；DDP `forward` 的 I/O 与单卡一致，内部保证参数一致。

#### 2. 问题分析

按通信契约定义两侧：集合通信的输入（参与集合、张量形状/dtype）、输出（结果张量/无返回）；数据分片输入输出。不变量：集合通信参与集合必须完全一致，否则 hang；分片索引并集=全集。边界：某 rank 提前 return 会破坏后续集合通信的对称性。

#### 3. 具体答案

面试中可以这样回答：
> 分布式的 I/O 契约核心是“对称性”。集合通信函数是集体操作，所有 rank 必须用相同顺序、相同通信组、匹配的形状和 dtype 调用，否则结果错误甚至永久 hang。比如 `all_reduce(t)` 要求各 rank 的 `t` 形状一致，输出是规约后的同形张量；`all_gather` 输出一个拼接张量，各分片形状需可对齐；`broadcast(src)` 以 src rank 的值为准，其余 rank 的输入被覆盖。数据侧，`DistributedSampler` 的输入是数据集长度，输出是当前 rank 负责的索引子集，契约是“各 rank 分片互斥且并集覆盖全集”。一个常见破坏来自控制流：如果只有部分 rank 进入带集合通信的分支（比如调试时 `if rank==0`），其余 rank 会卡在等待。工程上要保证所有 rank 走相同数量、相同顺序的集合通信，并用 `barrier` 只在必要时同步。

#### 4. 技术洞察

不变量：集合通信的“参与集合 + 调用顺序 + 形状/dtype”必须逐 rank 一致；分片并集=全集。可验证：`all_reduce` 后各 rank 结果相同；打印 sampler 索引集合验证互斥覆盖；故意让 rank0 多调一次集合通信观察 hang（设 timeout）。

#### 5. 图文说明

| 操作 | 输入契约 | 输出契约 |
|---|---|---|
| all_reduce | 各 rank 同形张量 | 同形规约结果 |
| all_gather | 各 rank 分片 | 拼接全量 |
| broadcast | src 有效 | 各 rank 同值 |
| DistributedSampler | 数据集长度 | 本 rank 索引子集 |

图示解释：多 rank 水平对齐图，集合通信是横跨所有 rank 的同步栅栏。

#### 6. 拓展知识

延伸：`ProcessGroup` 分组、`new_group` 子集通信、`all_reduce` 的 reduce op、FSDP 的 gather/scatter 契约、超时与 hang 调试。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 为什么会 hang？——集合通信不对称。
2. all_gather 输出形状？——各分片按维拼接。
3. 分片如何保证不重不漏？——DistributedSampler 按 rank 划分。

#### 8. 易错点与反例

1. 只有 rank0 进入的函数里调用 all_reduce，其余 rank 卡死。
2. 各 rank 传入 shape 不一致的张量，报错或结果错乱。
3. 各 rank 都写同一文件，覆盖或损坏。

#### 9. 实践与验证

脚本：`torchrun` 2 rank 跑 `all_reduce` 验证结果一致；打印 sampler 分片验证互斥；构造不对称集合通信并设短超时观察异常。oracle 为“对称即正确、不对称即失败”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B041 评分：不合格只会说“多卡通信”；合格能说集合通信对称要求；良好能写出各操作 I/O 与分片契约；优秀能结合子组通信、hang 诊断、FSDP 契约给出可验证设计方案。
### B042. 输入输出契约：编译栈

- 难度：入门级
- 知识域：K08 编译栈
- 考察能力：正确性、性能、成本、运维与迁移条件
- 题型：对比与取舍
- 面试频率：高频
- 前置知识：Dynamo、AOTAutograd、Inductor、guard、缓存
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

编译栈的 I/O 契约：被编译函数的输入是张量（shape/dtype/device 构成 guard），输出是等价的 eager 结果；编译产物按 guard 缓存；`torch.compile` 对不支持的输入会回退或重编译。动态 shape 通过符号化支持，但会增加编译复杂度。

#### 2. 问题分析

按两侧定义契约：输入 guard（shape/dtype/device/常量）、输出数值等价；编译产物的复用条件。不变量：编译函数在 guard 覆盖域内输出数值等价于 eager；guard 是缓存键。边界：输入落在 guard 外→重编译或回退。

#### 3. 具体答案

面试中可以这样回答：
> `torch.compile` 的契约可以概括为“guard 定义输入域，编译产物在该域内数值等价于 eager”。输入侧的契约是 guard：张量的 shape、dtype、device，甚至某些 Python 常量和对象属性都会被记录成守卫条件。输出侧的契约是数值等价——编译只是把这段计算换成融合 kernel，数学结果不变。产物复用条件是“新输入的 guard 与编译时一致”，命中就快；一旦 shape/dtype/device 变了，guard 失效触发重编译，这就是为什么固定输入签名很重要。对于确实需要可变 shape 的场景，用 `dynamic_shapes` 把指定维符号化，让一个编译产物覆盖一个 shape 范围，但代价是优化空间变小、编译更复杂。验证方式是对比 eager 与 compiled 在域内多组输入的 `allclose`，并用 `TORCH_LOGS=recompiles` 确认没有意外重编译。

#### 4. 技术洞察

不变量：`编译产物 == eager`（数值等价）在 guard 域内成立；缓存键 = guard 集合。可验证：`torch.allclose(compiled(x), eager(x))` 在多 shape 下成立；改变 shape 观察重编译日志。

#### 5. 图文说明

| 侧 | 契约 | 失败信号 |
|---|---|---|
| 输入 | guard(shape/dtype/device) | 未命中触发重编译 |
| 输出 | 数值等价 eager | 结果偏差 |
| 缓存 | guard 命中 | 频繁重编译 |

图示解释：guard 框住一个输入域，域内复用同一产物，域外重编译。

#### 6. 拓展知识

延伸：`dynamic_shapes`/`Dim`、`fullgraph`、`TORCH_LOGS`、编译缓存落盘、与 AMP 组合的 dtype guard。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. guard 包含什么？——shape/dtype/device/常量等。
2. 动态 shape 的代价？——优化受限、编译复杂。
3. 如何验证等价？——eager vs compiled allclose。

#### 8. 易错点与反例

1. 输入 shape 抖动，guard 频繁失效拖慢。
2. 以为 compile 改变语义，实际只是融合。
3. 不检查重编译，误以为加速其实在反复编译。

#### 9. 实践与验证

脚本：对 eager 与 compiled 多组 shape 比较 allclose；`TORCH_LOGS=recompiles` 观察；用 `dynamic_shapes` 覆盖 shape 范围验证单一产物。oracle 为“域内等价、固定签名无重编译”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B042 评分：不合格只会说 compile 加速；合格能说 guard 与数值等价；良好能解释重编译与动态 shape 取舍；优秀能结合 dynamic_shapes、日志观测与缓存给出可验证编译契约。
### B043. 输入输出契约：性能

- 难度：入门级
- 知识域：K09 性能
- 考察能力：正确性、性能、成本、运维与迁移条件
- 题型：对比与取舍
- 面试频率：高频
- 前置知识：关键路径、带宽、算力、通信、显存、统计
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

性能的 I/O 契约：输入是固定工作负载（shape/batch/dtype/迭代次数）、输出是可复现指标（吞吐、p99 延迟、显存峰值、利用率）。度量必须排除 warmup、固定频率/功耗状态、多次取分位数。契约还包括基线对比与门禁阈值。

#### 2. 问题分析

按两侧定义：输入工作负载必须固定且代表性；输出指标必须可复现、含分布而非单点。不变量：相同输入下指标波动在容差内；报告须含 warmup、迭代数、分位数。边界：不固定频率/后台负载时结果不可比。

#### 3. 具体答案

面试中可以这样回答：
> 性能的契约是“固定输入工作负载，输出可复现指标”。输入侧要锁定 shape、batch、dtype、迭代次数、精度设置（如 TF32 开关），并 warmup 若干轮排除初始化和编译开销；否则同一模型两次测出不同数字。输出侧不能只给一个平均数，要报告吞吐（samples/s 或 tokens/s）、延迟分位数（p50/p99）、显存峰值和 GPU 利用率，最好多次重复取中位数和方差。契约的可验证性体现在“换人换机器复跑结果落在容差内”，所以要把 CUDA/驱动/库版本、GPU 型号、功耗模式都记录下来。做优化对比时，唯一变量原则：每次只改一项（比如只开 compile 或只换 dtype），其余保持基线，并用同一输入跑对照。门禁上设“不劣于基线 X%”，超过就拦。没有这些约束，性能数字基本不可复现。

#### 4. 技术洞察

不变量：相同输入+相同环境→指标落在容差内；报告必须含 warmup、迭代数、分位数与环境元数据。可验证：重复测量方差小；基线复跑一致。缺任一项则结论不可比。

#### 5. 图文说明

| 侧 | 契约 | 缺失后果 |
|---|---|---|
| 输入 | 固定负载+环境 | 结果不可比 |
| 输出 | 吞吐/p99/显存/利用率 | 掩盖长尾 |
| 方法 | warmup+多次分位 | 噪声误判 |

图示解释：测量流程框，输入契约在左、指标契约在右、环境元数据是底注。

#### 6. 拓展知识

延伸：`torch.utils.benchmark`、CUDA Event 计时、锁定 GPU 频率、TF32/AMP 对结果影响、benchmark 门禁。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 为什么要分位数？——平均值掩盖长尾。
2. warmup 作用？——排除冷启动/编译。
3. 如何保证可比？——固定环境与单一变量。

#### 8. 易错点与反例

1. 只报平均值，p99 超标线上仍卡。
2. 换环境复跑数字差异大，却归因于代码优化。
3. 一次只跑一遍就下结论。

#### 9. 实践与验证

脚本：用 `torch.utils.benchmark` 或 Event 计时，warmup 后跑 N 次，报告 median/IQR；记录环境元数据；重复实验验证方差。oracle 为“复跑稳定、报告含分位与环境”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B043 评分：不合格只给一个时间数字；合格能说固定负载与 warmup；良好能报告分位与环境元数据；优秀能设计单一变量对照、门禁阈值与可复现性能契约。
### B044. 输入输出契约：导出部署

- 难度：入门级
- 知识域：K10 导出部署
- 考察能力：正确性、性能、成本、运维与迁移条件
- 题型：对比与取舍
- 面试频率：高频
- 前置知识：state_dict、Export、runtime、schema、迁移
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

导出部署的 I/O 契约：输入是 dummy input 的签名（名字、shape、dtype、动态轴）与模型 eval 态；输出是可被推理引擎加载的图，且对声明输入域数值等价。ONNX 用 `dynamic_axes` 声明动态维，TorchScript/`torch.export` 用 `dynamic_shapes`。

#### 2. 问题分析

定义输入域（动态轴取值）与输出等价性；声明 opset 与约束。不变量：图在输入域内与 eager 等价；输入名/顺序是下游引擎调用的契约。边界：域外或未支持算子导致错误。

#### 3. 具体答案

面试中可以这样回答：
> 导出的 I/O 契约就是“输入签名 + 数值等价”。输入侧要声明每个输入的名字、dtype、shape，以及哪些轴是动态的（如 batch、sequence）；这些名字和顺序会成为推理引擎调用时的契约，必须先定清楚。输出侧要保证对输入域内的任意取值，导出图的结果与 PyTorch eager 在容差内一致。opset 也是契约的一部分：它决定图里能用哪些算子，选高了老引擎不支持、选低了算子缺失。验证时，我会构造覆盖动态轴边界（如 batch=1 和最大 batch）的多组输入，用 ONNX Runtime 跑推理并与 PyTorch `assert_close`，同时 `onnx.checker.check_model` 保证结构合法。任何“数据依赖分支、未支持算子、动态轴外取值”都会破坏契约，需要在导出前消除或限制。

#### 4. 技术洞察

不变量：导出图在声明输入域内数值等价；输入签名=下游调用契约。可验证：多 shape allclose + checker。动态轴越多，优化空间与可预测性越差。

#### 5. 图文说明

| 侧 | 契约 | 破坏信号 |
|---|---|---|
| 输入 | 名字/dtype/shape/动态轴 | 调用 shape 不符 |
| opset | 算子集合 | 算子不支持 |
| 输出 | 域内数值等价 | allclose 失败 |

图示解释：输入域矩形内等价，矩形边界由动态轴声明，域外未定义。

#### 6. 拓展知识

延伸：`torch.export` 的 `Dim`/`dynamic_shapes`、ONNX external data、TensorRT 动态 profile、量化导出的 input scales。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 动态轴如何声明？——`dynamic_axes`/`Dim`。
2. 如何证明等价？——多 shape allclose。
3. opset 如何选？——按目标引擎支持。

#### 8. 易错点与反例

1. 只覆盖单 shape，线上批量失败。
2. opset 过高，推理引擎缺算子。
3. 输入名/顺序与线上调用不一致导致绑定错误。

#### 9. 实践与验证

脚本：导出后 `onnx.checker` 校验；ORT 在 batch 边界值上与 PyTorch allclose；打印图输入名与线上调用对齐。oracle 为“结构合法、域内等价、签名一致”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B044 评分：不合格只会调 export；合格能说输入签名与等价性；良好能声明动态轴并多 shape 校验；优秀能结合 opset、后端 profile、量化给出端到端导出契约。
### B045. 输入输出契约：自定义算子

- 难度：入门级
- 知识域：K11 自定义算子
- 考察能力：正确性、性能、成本、运维与迁移条件
- 题型：对比与取舍
- 面试频率：高频
- 前置知识：schema、dispatcher、stream、autograd、ABI
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

自定义算子的 I/O 契约：schema 定义输入/输出类型、是否可变（mutable）、别名（alias）；实现按 device/dtype 分派；若可微需定义反向的输入输出契约。`torch.library` 用于注册，`opcheck` 校验契约。

#### 2. 问题分析

按 schema 两侧定义：输入类型/别名/可变性、输出类型/是否共享存储；反向输入输出。不变量：schema 声明与实现行为一致；gradcheck 数值成立。边界：声明为不别名却原地改输入会破坏契约。

#### 3. 具体答案

面试中可以这样回答：
> 自定义算子的 I/O 契约由 schema 固定。输入侧要声明每个参数的张量类型、是否可原地修改（mutable）、返回张量是否与输入共享存储（alias）。这些不只是文档，dispatcher 和图捕获依赖它们做版本检查和正确性判断——如果你声明不别名却实际原地改了输入，autograd 版本计数和 `torch.compile` 捕获都会出错。输出侧要声明 dtype/device 规则和 shape 语义。若算子可微，反向也是一个契约：`autograd.Function.backward` 必须按 `forward` 输入数量与顺序返回梯度，不可微项返回 None；数值上必须通过 `gradcheck`。工程上我会用 `torch.library.opcheck` 自动检查 schema、别名与 autograd 行为是否一致，再用 `gradcheck` 做数值验证。契约一旦写错，问题往往在最深的地方才暴露，所以要在注册阶段就把 I/O 说清楚。

#### 4. 技术洞察

不变量：schema 声明的 alias/mutable 必须与实现一致；`backward` 返回值个数=forward 输入个数。可验证：`opcheck` 覆盖 schema/alias/autograd；`gradcheck` 数值通过。

#### 5. 图文说明

| 侧 | 契约 | 破坏信号 |
|---|---|---|
| 输入 schema | 类型/mutable/alias | 版本检查/图捕获错 |
| 输出 schema | dtype/device/shape | 下游断言失败 |
| 反向 | 输入对输出梯度 | gradcheck 失败 |

图示解释：算子 schema 是接口声明，实现是行为，opcheck 校验二者一致。

#### 6. 拓展知识

延伸：`torch.library.custom_op`/`register_autograd`、mutating ops、`SchemaInfo`、与 fake tensor/compile 的关系。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. alias 声明为何关键？——影响版本检查与图捕获。
2. mutable 输入如何写？——schema 声明并在实现中原地改。
3. 如何自动校验？——opcheck + gradcheck。

#### 8. 易错点与反例

1. 实现原地改输入但 schema 未声明 mutable，静默错误。
2. backward 返回数量不符，梯度错配。
3. 输出 device/dtype 与 schema 不一致，下游失败。

#### 9. 实践与验证

脚本：注册一个算子，`opcheck` 全绿；故意声明不别名但原地改，观察 opcheck/版本检查报错；`gradcheck` 校验可微路径。oracle 为“契约一致、错误被捕获”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B045 评分：不合格只写实现不管 schema；合格能说输入/输出/反向契约；良好能解释 alias/mutable 影响；优秀能用 opcheck/gradcheck 建立可验证的自定义算子契约。
### B046. 输入输出契约：测试治理

- 难度：入门级
- 知识域：K12 测试治理
- 考察能力：正确性、性能、成本、运维与迁移条件
- 题型：对比与取舍
- 面试频率：高频
- 前置知识：oracle、证据、日志、回滚、门禁、复盘
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

测试治理的 I/O 契约：测试输入是固定种子下的数据与配置，输出是明确的 pass/fail 与可复现证据（日志、指标、容差）。契约要求测试可独立运行、有超时、失败可定位。golden 文件与容差是 oracle 的一部分。

#### 2. 问题分析

按两侧定义：输入须固定（种子/数据/环境），输出须是判定与证据。不变量：相同输入重复运行结果稳定；失败信息指向最小复现。边界：非确定算子破坏输入稳定性，需容差或确定性算法。

#### 3. 具体答案

面试中可以这样回答：
> 测试治理的 I/O 契约是“固定输入，确定输出”。输入侧：测试必须固定随机种子、数据、依赖版本和确定性设置，最好能独立运行（不依赖执行顺序或外部服务），并设超时防止挂死。输出侧：要给出明确的 pass/fail 和可复现证据——失败的张量值、形状、随机种子、以及最小复现命令。数值 oracle 用 `assert_close(rtol, atol)` 而不是精确相等，因为 GPU 归约本身非确定。回归测试还会用 golden 输出（上一稳定版本产物）作为输入-输出对，配合容差比较。契约的健全性可以用“重复运行稳定性”验证：同一测试跑很多次结果一致说明输入被真正固定；如果时红时绿，说明有未固定的随机源或资源竞争，要定位根因而不是重试。这样 CI 里的红才可信。

#### 4. 技术洞察

不变量：`相同契约输入 ⇒ 相同判定`；oracle 独立于随机性。可验证：重复运行无 flaky；失败附最小复现；容差判据 `|a-e| ≤ atol+rtol|e|`。

#### 5. 图文说明

| 侧 | 契约 | 破坏信号 |
|---|---|---|
| 输入 | 固定种子/数据/环境 | flaky |
| 输出 | pass/fail+证据 | 失败不可复现 |
| oracle | 容差/golden | 精确比较误判 |

图示解释：测试是函数，输入契约保证稳定、输出契约保证可诊断。

#### 6. 拓展知识

延伸：`pytest` fixture 固化环境、`torch.testing.assert_close`、golden 版本管理、CI 重跑策略、覆盖率。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 如何避免 flaky？——固定所有随机源与顺序。
2. 精确比较为何不行？——GPU 非确定。
3. 失败如何可诊断？——附种子/张量/最小复现。

#### 8. 易错点与反例

1. 测试依赖执行顺序，单独跑就失败。
2. 用精确相等，偶发失败被忽略。
3. 不设超时，CI 挂死。

#### 9. 实践与验证

脚本：写一个固定种子的测试并重复运行验证稳定；制造依赖时间的 flaky 并修复；失败时输出最小复现信息。oracle 为“稳定、可诊断、有超时”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B046 评分：不合格只会写断言；合格能说固定输入与容差；良好能设计可复现、可诊断的测试契约；优秀能结合 golden、CI 重跑策略、覆盖率给出质量门禁契约。
### B047. 常见误区：张量/设备

- 难度：入门级
- 知识域：K01 张量/设备
- 考察能力：正确性、性能、成本、运维与迁移条件
- 题型：对比与取舍
- 面试频率：高频
- 前置知识：shape、stride、dtype、device
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

张量/设备常见误区：把视图当拷贝、忽略 device 一致性、误判 dtype 提升、以为 `non_blocking` 总是异步、认为 `.item()` 无代价、忽略 `storage_offset`。这些误区在正确性上造成静默错误，在性能上造成隐蔽瓶颈。

#### 2. 问题分析

逐条对比“直觉 vs 实际”并给证据：视图 vs 拷贝（`data_ptr`）、device 混用（报错）、dtype 提升（`promote_types`）、异步前提（pinned）、同步代价（`.item`）。边界：命名相似 API（`reshape`/`view`）易混。

#### 3. 具体答案

面试中可以这样回答：
> 张量与设备这块最常见的误区有六个。第一，把 `view`/`t`/`expand` 当成免费复制——它们其实是改 stride 的视图，共享 storage，改一个另一个会变；要真复制得用 `clone()`。第二，忽略 device 一致性，以为 CPU 和 GPU 张量能自动一起算——实际直接报错。第三，误判 dtype 提升，以为输出 dtype 等于第一个输入，实际按提升规则（fp16+fp32→fp32）。第四，以为 `non_blocking=True` 一定异步——只有源是 pinned 内存时才对，否则退化为同步。第五，认为 `.item()` 很便宜——它触发设备同步，放训练循环里会显著拖慢。第六，忽略 `storage_offset`，用非连续切片的 `data_ptr` 推断元素物理位置。这些误区都有可验证信号：`data_ptr`、报错信息、`promote_types` 结果、Profiler 同步点。

#### 4. 技术洞察

不变量：视图操作保 `data_ptr`；`device(out)=device(in)`；`dtype(out)=promote(...)`；异步前提是 pinned。可验证：逐条用最小脚本给出 `data_ptr`/dtype/报错/计时证据。

#### 5. 图文说明

| 误区 | 实际 | 证据 |
|---|---|---|
| 视图=拷贝 | 共享 storage | data_ptr 相同 |
| 跨设备自动算 | 报错 | RuntimeError |
| 输出 dtype=首个输入 | 类型提升 | promote_types |
| non_blocking 必异步 | 需 pinned | 计时差异 |

图示解释：直觉框与实际框并排，中间用证据箭头连接。

#### 6. 拓展知识

延伸：`is_contiguous`、`memory_format`、`clone` vs `detach`、TF32 与 dtype 关系、Profiler 同步点识别。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 如何判断是否拷贝？——比较 `data_ptr`。
2. non_blocking 何时有效？——pinned 源。
3. 输出 dtype 谁决定？——类型提升。

#### 8. 易错点与反例

1. `y=x.t(); y[0]=1` 改到 x，误以为独立。
2. 混用 CPU/GPU 期望自动搬运。
3. 循环里 `.item()` 记录指标，性能骤降。

#### 9. 实践与验证

脚本：逐条构造最小复现，打印 `data_ptr`/dtype/报错/计时，形成“误区-证据”对照表。oracle 为“每条误区都被证据推翻”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B047 评分：不合格会跟着误区走；合格能识别 2-3 个误区；良好能给出每条的证据与验证方法；优秀能系统化建立“直觉-实际-证据”对照并提出防错规范。
### B048. 常见误区：autograd

- 难度：入门级
- 知识域：K02 autograd
- 考察能力：正确性、性能、成本、运维与迁移条件
- 题型：对比与取舍
- 面试频率：高频
- 前置知识：requires_grad、leaf、view、detach、反向
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

autograd 常见误区：把梯度累积当覆盖、忘记 `zero_grad`、在图中 in-place 修改、误以为非叶子有 `.grad`、对非标量直接 backward、以为 `detach` 会复制、混淆 `no_grad` 与 `detach`。这些导致训练不收敛或报版本检查错误。

#### 2. 问题分析

逐条对比并给证据：累积（重复 backward 后 `.grad` 变大）、in-place（报错）、非叶子（`.grad is None`）、非标量（需 grad_outputs）、detach（共享数据不回图）。边界：`retain_graph` 与多次回传。

#### 3. 具体答案

面试中可以这样回答：
> autograd 有五个高频误区。第一，以为每次 backward 会覆盖梯度，实际是累加，所以必须 `zero_grad`。第二，认为可以随意 in-place 修改参与梯度的张量——前向保存过的张量被原地改会触发版本检查报错，正确写法是 `x = x + 1`。第三，以为所有参与运算的张量都有 `.grad`——只有叶子有，中间量默认没有，要看就 `retain_grad()`。第四，对非标量输出直接 `backward()`，会报必须提供 `grad_outputs`。第五，以为 `detach()` 复制数据——它共享同一 storage 只是不回图，改它会改原张量。这些误区都可以用最小脚本验证：重复 backward 看 `.grad` 翻倍、in-place 看报错、`print(中间量.grad)` 看 None、非标量 backward 看异常、detach 后比 `data_ptr`。

#### 4. 技术洞察

不变量：`.grad` 累加；in-place 破坏版本计数；只有叶子持久 `.grad`；detach 共享 storage。可验证：`x.grad` 在两次 backward 后为 2 倍；`x += 1` 报错；`y.grad is None`；`x.detach().data_ptr()==x.data_ptr()`。

#### 5. 图文说明

| 误区 | 实际 | 证据 |
|---|---|---|
| backward 覆盖梯度 | 累加 | .grad 翻倍 |
| 可随意 in-place | 版本检查报错 | RuntimeError |
| 都有 .grad | 仅叶子 | None |
| detach 复制 | 共享 storage | data_ptr 相同 |

图示解释：误区与实际两列对照，证据作为判定标准。

#### 6. 拓展知识

延伸：`retain_graph`、`create_graph`、`register_hook`、`torch.autograd.set_detect_anomaly(True)` 定位 NaN/版本问题。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 梯度为何要清零？——累加语义。
2. in-place 为何报错？——保存张量被改，版本检查失败。
3. detach 与 clone 区别？——detach 共享数据，clone 复制。

#### 8. 易错点与反例

1. 忘 `zero_grad`，等效放大 lr。
2. 在反向图上原地更新 `running` 张量导致报错。
3. 用 `detach` 后原地改值，意外改了原张量。

#### 9. 实践与验证

脚本：逐条最小复现，打印 `.grad` 值、捕获异常、比较 `data_ptr`；开 `set_detect_anomaly(True)` 观察定位信息。oracle 为“每条误区被证据推翻”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B048 评分：不合格跟着误区；合格能识别主要误区；良好能给出证据与正确写法；优秀能结合 anomaly detect、hook、gradient checkpoint 建立 autograd 防错体系。
### B049. 常见误区：Module/状态

- 难度：入门级
- 知识域：K03 Module/状态
- 考察能力：正确性、性能、成本、运维与迁移条件
- 题型：对比与取舍
- 面试频率：高频
- 前置知识：参数、buffer、模式和 state_dict
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

Module/状态常见误区：普通 Tensor 属性不被注册（不被优化器更新/不被 `to` 移动/不进 state_dict）、`requires_grad=False` 的 Parameter 仍被移动但可能仍被更新、忘记 eval 导致 Dropout/BN 行为错、多卡 `module.` 前缀、buffer 与参数混用、加载 `strict=False` 掩盖错位。

#### 2. 问题分析

逐条对比：注册 vs 未注册、参数 vs buffer、train vs eval、前缀、strict 语义。不变量：注册对象被统一管理；eval 冻结训练态行为。边界：Tensor 属性与 Parameter 名字不同的行为。

#### 3. 具体答案

面试中可以这样回答：
> Module 状态有六个常见误区。第一，把可学习张量写成普通 `self.w = torch.randn(...)` 属性，它不注册，于是不进 `parameters()`、优化器不更新、`to()` 不移动、不进 state_dict——必须包成 `nn.Parameter`。第二，把 buffer 和参数混用：BN 的 running stats 是 buffer，进 state_dict 但不该被优化器更新，用 `register_buffer` 而不是 Parameter。第三，忘记 `model.eval()`，Dropout 仍在丢弃、BN 用 batch 统计，推理指标偏低且不稳定。第四，多卡保存的 state_dict 带 `module.` 前缀，单卡加载报 missing/unexpected keys，要去前缀或映射。第五，用 `strict=False` 掩盖真实的键错位，导致部分层实际没加载到。第六，把临时大张量缓存成 `self.x`，既不注册又可能泄漏显存。这些都能验证：打印 `named_parameters()`、对比 train/eval 输出、检查 state_dict 键集合。

#### 4. 技术洞察

不变量：只有注册对象进 `parameters()`/`state_dict`/`to`；`eval()` 使 `training=False` 传播。可验证：未注册张量不在 `named_parameters()`；`eval()` 后 Dropout 输出确定；键集合对比发现前缀。

#### 5. 图文说明

| 误区 | 实际 | 证据 |
|---|---|---|
| 普通 Tensor 属性 | 不注册 | named_parameters 缺失 |
| buffer 当参数 | 不该更新 | 状态被优化器改 |
| 忘 eval | 训练态行为 | 输出随机 |
| strict=False 兜底 | 掩盖错位 | 键差异被隐藏 |

图示解释：注册树内/外两种对象，误区把树内对象放到树外。

#### 6. 拓展知识

延伸：`apply`、EMA buffer 化、`_load_from_state_dict`、DDP/FSDP 包装对注册树的重写。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 为什么要 Parameter？——进优化器并随模块管理。
2. buffer 与参数区别？——进 state_dict 但不更新。
3. eval 影响哪些层？——Dropout、BN。

#### 8. 易错点与反例

1. 普通 Tensor 权重永远不更新，训练像随机初始化。
2. eval 忘调用，推理结果每次不同。
3. `strict=False` 误用，模型部分层随机初始化却无告警。

#### 9. 实践与验证

脚本：对比 Parameter 与普通 Tensor 属性的 `named_parameters()`；train/eval 输出对比；构造 `module.` 前缀并加载验证报错。oracle 为“注册行为与 train/eval 语义符合预期”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B049 评分：不合格常犯未注册错；合格能区分参数/buffer；良好能解释 eval 与前缀问题；优秀能结合 FSDP、EMA、加载 hook 建立状态治理防错清单。
### B050. 常见误区：数据管道

- 难度：入门级
- 知识域：K04 数据管道
- 考察能力：正确性、性能、成本、运维与迁移条件
- 题型：对比与取舍
- 面试频率：高频
- 前置知识：Dataset、Sampler、collate、padding、版本
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

数据管道常见误区：worker 未设种子导致增强重复、`num_workers` 越大越好、`pin_memory` 未配 `non_blocking`、默认 collate 处理变长失败、自定义 Dataset 持有大对象、worker 中不可 pickle 对象、`persistent_workers` 缺省频繁重启、`DistributedSampler` 未设 shuffle/seed 导致各 rank 一致。

#### 2. 问题分析

逐条对比并给证据：worker 种子（打印 id/seed）、num_workers 拐点（计时）、pin+non_blocking（计时）、变长 collate（报错）、分片（索引集合）。不变量：worker 数据不重不漏、增强可复现。

#### 3. 具体答案

面试中可以这样回答：
> 数据管道的误区集中在并行与随机性。第一，`num_workers>0` 时没在 `worker_init_fn` 里设种子，多个 worker 从相同状态出发，增强结果重复，等于削弱了数据多样性。第二，以为 worker 越多越快——实际到某个拐点后进程创建、IPC、内存拷贝让吞吐下降，要扫参数找拐点。第三，开了 `pin_memory` 却写 `non_blocking=False`，异步收益为零。第四，用默认 collate 处理变长序列，直接报 stack 维度不一致，需要自定义 pad+mask。第五，在 Dataset 里持有大对象或打开文件不关，worker 数量一多就 fd/内存耗尽。第六，`DistributedSampler` 不设 `shuffle` 和种子，各 rank 顺序一致，或忘了 `set_epoch` 导致每个 epoch 打乱相同。验证方法很简单：打印各 worker 的 id/seed 与索引集合、扫 worker 数计时、对比 pin 开关。

#### 4. 技术洞察

不变量：worker 分片互斥覆盖全集；增强由 worker 局部种子决定；`set_epoch` 改变 sampler 的随机序。可验证：线程分片并集=全集；不同 epoch 的 shuffle 不同；num_workers 扫描找拐点。

#### 5. 图文说明

| 误区 | 实际 | 证据 |
|---|---|---|
| worker 不设种 | 增强重复 | 各 worker 输出相同 |
| worker 越多越好 | 有拐点 | 计时曲线 |
| pin 不配 non_blocking | 零收益 | 计时无差 |
| 默认 collate 变长 | 报错 | stack 维度不一致 |

图示解释：worker 并行图旁标出随机流，误区处随机流汇合。

#### 6. 拓展知识

延伸：`persistent_workers`、`prefetch_factor`、共享内存、`IterableDataset` 分片、`set_epoch` 与分布式 shuffle。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. worker 增强为何重复？——种子未按 worker 派生。
2. num_workers 有最优吗？——有拐点，需实测。
3. 分布式 shuffle 如何正确？——每个 epoch `set_epoch`。

#### 8. 易错点与反例

1. 所有 worker 产出相同增强，模型见过少样本。
2. num_workers=64 反而慢，未测得拐点。
3. 忘 `set_epoch`，各 epoch 打乱顺序相同。

#### 9. 实践与验证

脚本：打印 worker id/seed 与索引集合验证分片与随机流；扫 num_workers 计时；对比 set_epoch 前后采样顺序。oracle 为“分片互斥、增强可复现、拐点可测”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B050 评分：不合格盲调 worker 数；合格能说种子与分片；良好能解释拐点与 pin 组合；优秀能结合 persistent_workers、set_epoch、共享内存给出可实测的数据管道调优清单。
### B051. 常见误区：训练/优化器

- 难度：入门级
- 知识域：K05 训练/优化器
- 考察能力：输入矩阵、oracle、故障注入与指标
- 题型：测试/实验与验证
- 面试频率：高频
- 前置知识：forward、backward、累积、optimizer、scheduler
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

训练/优化器常见误区：忘 `zero_grad`、`scheduler.step` 位置错、梯度累积忘记归一、Adam 与 AdamW 权重衰减混淆、学习率与 batch 不同步缩放、在 `no_grad` 外手动更新权重、loss 未归约、梯度裁剪在 AMP 下顺序错。验证靠单步断言与损失曲线。

#### 2. 问题分析

逐条给最小实验：清零（`.grad` 归零）、scheduler 位置（lr 值）、累积归一（梯度量级）、AdamW 解耦（权重衰减项）。不变量：step 使用当前周期梯度；lr 序列符合配置。边界：累积周期边界处理。

#### 3. 具体答案

面试中可以这样回答：
> 训练循环的误区大多能用一个单步实验验证。第一，忘 `zero_grad`：跑两步打印 `.grad` 会看到累加，等效放大 lr。第二，`scheduler.step()` 放在 `optimizer.step()` 之前：打印 `param_group['lr']` 会发现用错的学习率；按 step 调度应放在 optimizer.step 之后。第三，梯度累积忘记归一：K 步累积后梯度是单步的 K 倍，如果不同时缩放 lr 或除以 K，等效放大 K 倍。第四，混淆 Adam 的 L2 与 AdamW 的解耦权重衰减：前者把 wd 加到梯度、后者直接衰减权重，正则行为不同。第五，batch 变了 lr 不缩放：线性缩放规则只是经验，但大 batch 不调 lr 常导致不收敛。第六，手动 EMA 更新没包 `no_grad`，每次更新都建图。验证方式：打印 lr、梯度范数和权重差，与手算对照。

#### 4. 技术洞察

不变量：`zero_grad` 后 `.grad` 归零；`scheduler` 输出 lr 序列确定；累积 K 步梯度 ≈ 单步 K 倍（无归一）。可验证：打印 `scheduler.get_last_lr()`；`torch.nn.utils.clip_grad_norm_` 前后梯度范数。

#### 5. 图文说明

| 误区 | 实际 | 证据 |
|---|---|---|
| 忘 zero_grad | 梯度累加 | .grad 翻倍 |
| scheduler 先 step | lr 错位 | get_last_lr |
| 累积不归一 | 等效放大 lr | 梯度范数 |
| Adam 当 AdamW | 正则不同 | 权重衰减项 |

图示解释：每步六节点顺序图，误区标在错位的节点。

#### 6. 拓展知识

延伸：线性缩放与 warmup、梯度裁剪、EMA 的 no_grad、`set_to_none`、OneCycle。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 梯度为何累加？——设计如此，需手动清零。
2. scheduler 放哪？——optimizer.step 之后（按 step）。
3. 累积如何调 lr？——按累积步缩放或归一 loss。

#### 8. 易错点与反例

1. 训练看似收敛，实际是梯度累加放大 lr 的假象。
2. scheduler 错位，warmup 失效。
3. AdamW 写成 Adam+weight_decay，正则强度不符。

#### 9. 实践与验证

脚本：单步训练打印 `.grad`、`get_last_lr()`、权重差；K 步累积对比单步梯度；切换 Adam/AdamW 观察权重衰减。oracle 为“各指标与手算一致”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B051 评分：不合格顺序随意；合格能列主要误区；良好能用单步实验验证；优秀能结合调度器、梯度累积、AMP 与 AdamW 给出正确且可验证的训练循环。
### B052. 常见误区：AMP/恢复

- 难度：入门级
- 知识域：K06 AMP/恢复
- 考察能力：输入矩阵、oracle、故障注入与指标
- 题型：测试/实验与验证
- 面试频率：高频
- 前置知识：autocast、scaler、快照、随机状态
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

AMP/恢复常见误区：不用 GradScaler 直接 fp16 回传（下溢）、autocast 内算 loss、先 step 后 unscale、裁剪在 unscale 前、断点只存模型权重、bf16 仍加 scaler、恢复后 RNG 未存导致 dropout 流错位。验证靠 scale 变化与续训一致性。

#### 2. 问题分析

逐条给实验：scale 初值/变化、unscale 顺序、checkpoint 完整性、RNG 一致性。不变量：溢出步跳过；恢复后训练动态一致。

#### 3. 具体答案

面试中可以这样回答：
> AMP 与恢复的误区都能用“scale 日志 + 续训对拍”验证。第一，直接对 fp16 loss 调 backward 而不用 `scaler.scale`：fp16 最小正规数约 6.1e-5，小梯度下溢为 0，训练停滞——正确做法是 `scaler.scale(loss).backward()`。第二，在 autocast 内计算并回传 loss：数值不稳、易出 nan。第三，先 `scaler.step` 再 `scaler.unscale_`：unscale 失去意义且可能报错，正确顺序是 unscale 后再裁剪、step。第四，梯度裁剪放在 unscale 之前：阈值作用在放大后的梯度上，裁剪失效。第五，断点只存模型权重：优化器动量和 scaler 的 scale/growth_tracker 丢失，续训曲线接不上，还会让 scale 从初值反复跳步。第六，用 bf16 还套 GradScaler：bf16 指数范围同 fp32，通常不需要，徒增复杂。第七，恢复未存 RNG，dropout/增强流错位。

#### 4. 技术洞察

不变量：unscale 在裁剪/step 前；恢复集含 scaler 与 RNG；溢出步不更新。可验证：`scaler.get_scale()` 随溢出下降；续训 5 步与不中断逐位一致。

#### 5. 图文说明

| 误区 | 实际 | 证据 |
|---|---|---|
| 不用 scaler | 下溢 | 梯度为 0、loss 不动 |
| autocast 内算 loss | 数值不稳 | nan |
| 先 step 后 unscale | 顺序错 | 报错/无效 |
| 只存模型 | 状态丢失 | 续训偏移 |

图示解释：AMP 步流水线，误区标在错误顺序或缺失的持久化支线。

#### 6. 拓展知识

延伸：bf16 无需 scaler、`torch.amp` 新 API、FSDP/DDP 下 scaler 状态、RNG 保存。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 为什么必须 scale？——fp16 梯度易下溢。
2. unscale 与裁剪顺序？——先 unscale。
3. 恢复要存什么？——model/optimizer/scaler/RNG/step。

#### 8. 易错点与反例

1. 不用 scaler，loss 长时间不动。
2. autocast 内回传，NaN。
3. 恢复缺 scaler，scale 跳变。

#### 9. 实践与验证

脚本：打印 scale 初值与溢出后变化；故意错序观察报错；完整/不完整 checkpoint 续训对拍 loss。oracle 为“scale 动态正确、完整恢复逐位一致”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B052 评分：不合格只开 autocast；合格能说 scaler 顺序；良好能验证 scale 与续训；优秀能结合 bf16、分布式与 RNG 给出可复现恢复方案并识别反例。
### B053. 常见误区：分布式

- 难度：入门级
- 知识域：K07 分布式
- 考察能力：输入矩阵、oracle、故障注入与指标
- 题型：测试/实验与验证
- 面试频率：高频
- 前置知识：rank、DDP、FSDP、collective、容错
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

分布式常见误区：不用 DistributedSampler 导致各 rank 重复数据、集合通信不对称导致 hang、只用 rank0 保存却无 barrier、`find_unused_parameters` 滥用、把 DDP 当省显存手段、忘记 `set_epoch`、进程内 CUDA 上下文未设置正确 device、NCCL 环境变量缺失。验证靠参数一致性与分片。

#### 2. 问题分析

逐条给验证：分片（索引集合）、对称性（hang 超时）、保存（文件完整）、参数一致（哈希）、显存（占用）。不变量：各 rank 参数一致、分片互斥、通信对称。

#### 3. 具体答案

面试中可以这样回答：
> 分布式的误区集中在“对称性”和“一致性”。第一，不用 `DistributedSampler`：各 rank 拿到相同数据，等效把 batch 缩小、梯度含重复样本；正确做法是按 rank 分片并每 epoch `set_epoch`。第二，集合通信不对称：只有部分 rank 进入某个带 all_reduce 的分支，其余 rank 永远等待，表现为 hang；必须保证所有 rank 走相同数量、相同顺序的集合通信。第三，保存 checkpoint 只在 rank0 写但没有 barrier：其他 rank 可能提前退出或读到半成品；加 `dist.barrier()` 对齐。第四，滥用 `find_unused_parameters=True`：它每步扫描未用参数，有开销，只应在确实有条件分支时开启。第五，误以为 DDP 省显存：DDP 每卡持有完整模型、梯度和优化器状态，真正省显存要靠 FSDP/ZeRO。验证方法：打印各 rank 参数哈希验证一致、打印 sampler 分片验证互斥、构造不对称通信设短超时观察异常。

#### 4. 技术洞察

不变量：DDP 各 rank 参数一致；分片互斥覆盖全集；集合通信对称。可验证：参数哈希相同；sampler 索引互斥；不对称调用触发 timeout。

#### 5. 图文说明

| 误区 | 实际 | 证据 |
|---|---|---|
| 无 DistributedSampler | 数据重复 | 索引集合相同 |
| 通信不对称 | hang | 超时 |
| 无 barrier 保存 | 文件不全 | 读取失败 |
| DDP 省显存 | 每卡全量 | 显存占用 |

图示解释：多 rank 对齐图，误区破坏对齐或对称。

#### 6. 拓展知识

延伸：`no_sync`、`find_unused_parameters`、FSDP 分片、NCCL 调优、`torchrun` 环境、elastic。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 为什么会 hang？——通信不对称。
2. DDP 收益何时打折？——通信无法被计算掩盖。
3. 如何省显存？——FSDP/ZeRO，不是 DDP。

#### 8. 易错点与反例

1. 各 rank 重复数据，指标虚高。
2. 条件分支里调集合通信，hang。
3. 误信 DDP 省显存，多卡反而 OOM。

#### 9. 实践与验证

脚本：torchrun 2/4 rank 打印参数哈希与 sampler 索引；构造不对称通信设 timeout；对比 DDP/FSDP 显存占用。oracle 为“参数一致、分片互斥、非法通信超时”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B053 评分：不合格只会 multi-GPU 启动；合格能说分片与 AllReduce；良好能识别 hang/保存/显存误区并验证；优秀能结合 FSDP、no_sync、NCCL 给出可诊断的分布式防错清单。
### B054. 常见误区：编译栈

- 难度：入门级
- 知识域：K08 编译栈
- 考察能力：输入矩阵、oracle、故障注入与指标
- 题型：测试/实验与验证
- 面试频率：高频
- 前置知识：Dynamo、AOTAutograd、Inductor、guard、缓存
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

编译栈常见误区：以为 compile 必加速、忽略编译延迟与重编译、热路径里读张量值导致 graph break、动态 shape 频繁重编、`fullgraph=True` 遇断点直接报错、CUDA Graph 与动态 shape/显存分配冲突、以为 compile 改变数值语义、忽略 warmup。验证靠 kernel 数与重编译日志。

#### 2. 问题分析

逐条给实验：kernel 数（Profiler）、重编译（TORCH_LOGS）、graph break（explain）、短任务开销（计时）。不变量：编译域内数值等价；guard 命中才复用。边界：短任务编译开销摊不掉。

#### 3. 具体答案

面试中可以这样回答：
> 编译栈的误区围绕“收益不是无条件”的。第一，以为 `torch.compile` 一定更快：短任务或频繁重编译时，编译延迟可能超过收益，要先 warmup 再测稳态。第二，忽略重编译：输入 shape/dtype/device 变化会让 guard 失效、每次重新编译，热点路径必须固定输入签名。第三，在热点里读张量值（`.item()`、`print(tensor)`、数据依赖 `if`）导致 graph break，这段退回 eager，融合收益大打折扣。第四，用 `fullgraph=True` 时任何断点都会直接报错，需要先把代码改成可捕获。第五，把 `mode='reduce-overhead'`（CUDA Graph）用在动态 shape 或频繁新分配的场景：CUDA Graph 要求静态地址，会冲突。第六，以为 compile 改变数值语义——它只是融合，域内数值等价，若有偏差通常是 dtype/归约顺序差异。验证：Profiler 比 kernel 数、`TORCH_LOGS=graph_breaks,recompiles` 看断点与重编、计时对比 warmup 前后。

#### 4. 技术洞察

不变量：编译域内 `allclose(eager, compiled)`；收益 = 融合省下的启动与带宽 − 编译/重编译开销。可验证：kernel 数下降、重编译日志为空、稳态变快。

#### 5. 图文说明

| 误区 | 实际 | 证据 |
|---|---|---|
| compile 必快 | 有编译开销 | warmup 后计时 |
| 无重编译 | shape 变化重编 | recompiles 日志 |
| 读值无害 | graph break | explain |
| CUDA Graph 通用 | 需静态地址 | 冲突报错 |

图示解释：编译收益曲线先负后正，误区和正确判断标在曲线两端。

#### 6. 拓展知识

延伸：`dynamic_shapes`、`fullgraph`、CUDA Graph 捕获、AOTInductor、编译缓存。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 何时 compile 反而慢？——短任务/频繁重编。
2. 如何减少重编译？——固定输入签名。
3. graph break 影响？——退回 eager 损失融合。

#### 8. 易错点与反例

1. 在热路径 `print(loss.item())`，每次断图。
2. 输入 shape 抖动，重编译拖慢。
3. 短脚本开 compile，只见变慢不见收益。

#### 9. 实践与验证

脚本：warmup 后对比 eager/compiled 稳态计时与 kernel 数；`TORCH_LOGS=recompiles` 检查；构造 `fullgraph=True` 的断点观察报错。oracle 为“稳态加速、无重编、断点可定位”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B054 评分：不合格盲信 compile；合格能说收益与开销；良好能识别 graph break/重编译；优秀能结合 CUDA Graph、动态 shape、观测日志给出可落地的编译调优判断。
### B055. 常见误区：性能

- 难度：入门级
- 知识域：K09 性能
- 考察能力：输入矩阵、oracle、故障注入与指标
- 题型：测试/实验与验证
- 面试频率：高频
- 前置知识：关键路径、带宽、算力、通信、显存、统计
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

性能常见误区：不看 warmup、用平均掩盖 p99、混淆计算/带宽受限、只看端到端不分段、Profiler 全量开启改变行为、忽略通信/同步、以峰值算力当实际、忽略输入分布、在不稳定环境对比。验证靠分段计时与 Roofline。

#### 2. 问题分析

逐条给实验：warmup（前后差异）、分位（尾延迟）、分段（占比）、Roofline（受限类型）、同步（利用率）。不变量：测量排除冷启动且含分布；单变量对照。

#### 3. 具体答案

面试中可以这样回答：
> 性能误区往往让结论不可复现。第一，不做 warmup：把首次编译、显存分配、CUDA 上下文初始化算进平均，重跑差异巨大。第二，只报平均：p99 长尾才是线上体验，平均好掩盖卡顿。第三，不判断受限类型：对大 matmul 用“带宽受限”思路优化，或对 elementwise 追求算力峰值，方向就错了——要先算算术强度再定方向。第四，只看端到端不分段：无法知道瓶颈在数据、前向、反向、优化器还是通信。第五，全量开 Profiler：采样开销会改变时间分布，要用 `schedule` 只采中间几步。第六，忽略同步：`.item()`、`.cpu()` 的隐式同步让 GPU 空转却被算进“计算时间”。第七，在不稳定环境对比：后台负载、功耗模式变化都会污染结果。正确做法是固定环境、单一变量、报告分位与分段占比。

#### 4. 技术洞察

不变量：测量 = 固定输入+环境+warmup+分位；优化方向由受限类型决定。可验证：分段占比和为 1；Roofline 分类与 Profiler 的 achieved BW/FLOPS 一致。

#### 5. 图文说明

| 误区 | 实际 | 证据 |
|---|---|---|
| 无 warmup | 冷启动污染 | 前后计时差 |
| 只看平均 | 掩盖长尾 | p50 vs p99 |
| 不分类受限 | 优化方向错 | Roofline |
| 全量 Profiler | 改变行为 | schedule 采样 |

图示解释：时间轴分段+分位叠加，误区标在缺失的维度。

#### 6. 拓展知识

延伸：`torch.utils.benchmark`、CUDA Event、频率锁定、Roofline、memory snapshot。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 怎么分受限类型？——算算术强度比临界点。
2. 为什么看 p99？——线上卡顿来自长尾。
3. Profiler 影响测量吗？——有开销，需采样。

#### 8. 易错点与反例

1. 一次运行下结论，方差极大。
2. 只测平均，上线卡顿。
3. Profiler 全开，测得比真实还慢。

#### 9. 实践与验证

脚本：分段+分位计时，画时间轴占比；用 Roofline 分类并对照 Profiler；对比全量/采样 Profiler 时间。oracle 为“分段合理、分类正确、测量稳定”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B055 评分：不合格只看单次数；合格能说 warmup 与分位；良好能分段并判断受限类型；优秀能设计单变量、可复现、含尾延迟与资源维度的性能验证方案。
### B056. 常见误区：导出部署

- 难度：入门级
- 知识域：K10 导出部署
- 考察能力：输入矩阵、oracle、故障注入与指标
- 题型：测试/实验与验证
- 面试频率：高频
- 前置知识：state_dict、Export、runtime、schema、迁移
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

导出部署常见误区：忘记 eval 导致 Dropout 固化、dummy input shape 写死、用 trace 固化数据依赖分支、opset 过高/过低、只校验结构不做数值对拍、动态轴未覆盖边界、导出后未固定随机、量化未校准。验证靠多 shape allclose。

#### 2. 问题分析

逐条给实验：eval（输出稳定）、trace 分支（结果错）、opset（引擎支持）、对拍（allclose）、动态轴边界（batch 极值）。不变量：域内数值等价；eval 冻结训练态。

#### 3. 具体答案

面试中可以这样回答：
> 导出的误区大多会在线上第一次不同输入时爆发。第一，导出前没 `model.eval()`：Dropout 和 BatchNorm 的训练态行为被固化，推理结果带随机性或统计偏差。第二，dummy input 形状写死：图里 batch/seq 被固定，线上换 batch 直接报 shape 错。第三，用 `torch.jit.trace` 追踪含数据依赖分支的模型：trace 只记录实际走的那条路径，其他分支被丢弃，输入触发别的分支时结果错。第四，opset 选择不当：过高目标引擎不支持算子，过低的算子集缺能力。第五，只做 `onnx.checker` 结构校验不做数值对拍：算子语义偏差（如 pad、round 模式）检查不出来，必须用 ORT 与 PyTorch `allclose`。第六，动态轴只测中间值不测边界：batch=1 或最大 batch 常暴露 reshape/广播问题。第七，量化未用校准数据：激活范围估计错误，精度崩。

#### 4. 技术洞察

不变量：eval 冻结行为；导出图在声明输入域内等价。可验证：多 shape allclose；trace 对分支模型失败；checker 通过但数值偏差能靠对拍暴露。

#### 5. 图文说明

| 误区 | 实际 | 证据 |
|---|---|---|
| 忘 eval | 训练态固化 | 输出随机/偏差 |
| trace 分支 | 只记录一条路径 | 换输入结果错 |
| 无对拍 | 语义偏差漏检 | allclose 失败 |
| 动态轴不测边界 | 边界报错 | batch 极值失败 |

图示解释：导出流程旁标出误区导致的错误输出路径。

#### 6. 拓展知识

延伸：`torch.export` 取代 trace、动态轴边界测试、量化校准（PTQ）、TensorRT 精度 profile、ONNX 算子版本。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 为什么必须 eval？——避免固化训练态。
2. trace 与控制流冲突？——分支被固化。
3. 如何确保导出正确？——多 shape allclose+边界。

#### 8. 易错点与反例

1. 忘 eval，线上每次输出不同。
2. trace 带分支模型，换输入错。
3. 只 checker 不对拍，pad 语义偏差上线才现。

#### 9. 实践与验证

脚本：导出前后多 shape allclose；trace 分支模型验证失败；checker+ORT 对拍；batch 边界测试。oracle 为“域内等价、边界通过、静态结构合法”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B056 评分：不合格只导出不校验；合格能说 eval 与对拍；良好能识别 trace 分支与动态轴边界；优秀能结合 export、量化校准、后端 profile 给出可上线导出验证清单。
### B057. 常见误区：自定义算子

- 难度：入门级
- 知识域：K11 自定义算子
- 考察能力：输入矩阵、oracle、故障注入与指标
- 题型：测试/实验与验证
- 面试频率：高频
- 前置知识：schema、dispatcher、stream、autograd、ABI
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

自定义算子常见误区：无 profile 证据就写、backward 返回值与输入不匹配、schema 未声明 alias/mutable、不写 gradcheck、不考虑 compile 捕获、CUDA 扩展编译环境不固定、忽略设备/dtype 支持范围。验证靠 gradcheck/opcheck 与 profile 收益。

#### 2. 问题分析

逐条给实验：profile 证据（前后计时）、backward 匹配（gradcheck）、alias（opcheck）、compile（图捕获）。不变量：数值梯度一致；schema 与行为一致。

#### 3. 具体答案

面试中可以这样回答：
> 自定义算子的误区有两类：不该写和写不对。第一，没有 profile 证据就写算子：现有算子可能已经够快，自己写的 kernel 反而更慢还增加维护成本，必须先用 Profiler 证明这段是瓶颈。第二，`autograd.Function.backward` 返回值个数或顺序与 forward 输入不符：梯度错配甚至静默错误，必须用 `gradcheck` 数值对拍。第三，schema 没声明 alias/mutable 却原地改输入：破坏版本检查和图捕获，用 `opcheck` 才能查出来。第四，不测试可微路径：只测前向正确，训练时梯度错。第五，不考虑 `torch.compile` 捕获：普通 Python 自定义函数可能触发 graph break，用 `torch.library.custom_op` 注册才能被图捕获。第六，CUDA 扩展编译环境和 flags 不固定：换机器编译失败或行为不同。验证：gradcheck+opcheck+profile 前后对比三件套。

#### 4. 技术洞察

不变量：`gradcheck` 数值通过；schema 与实现一致；收益需 profile 证明。可验证：`gradcheck` 通过、`opcheck` 全绿、优化前后计时。

#### 5. 图文说明

| 误区 | 实际 | 证据 |
|---|---|---|
| 无证据就写 | 可能更慢 | profile 对比 |
| backward 不匹配 | 梯度错 | gradcheck 失败 |
| 未声明 alias | 版本/图捕获错 | opcheck |
| 不支持 compile | graph break | explain |

图示解释：从“是否需要”到“是否正确”的两道闸门，误区分别卡在两道门。

#### 6. 拓展知识

延伸：`torch.library.custom_op` 可被 compile 捕获、CUDA 扩展构建、`make_fx`、kernel 微基准。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 何时才该写算子？——有 profile 证据且现有算子无法表达。
2. 如何验证可微？——gradcheck。
3. 如何保证契约？——opcheck。

#### 8. 易错点与反例

1. 凭感觉写算子，收益为负。
2. backward 数量不符，梯度错配。
3. 未声明 mutable，训练时版本检查失败。

#### 9. 实践与验证

脚本：gradcheck/opcheck 跑一个自定义算子；profile 对比原生实现；构造带自定义函数的模型观察 compile 断点。oracle 为“数值与契约通过、收益为正”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B057 评分：不合格随手写算子；合格能说 gradcheck/opcheck；良好能用 profile 证明必要性并校验契约；优秀能结合 compile 捕获、CUDA 构建与微基准给出可维护的自定义算子工程规范。
### B058. 常见误区：测试治理

- 难度：入门级
- 知识域：K12 测试治理
- 考察能力：输入矩阵、oracle、故障注入与指标
- 题型：测试/实验与验证
- 面试频率：高频
- 前置知识：oracle、证据、日志、回滚、门禁、复盘
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

测试治理常见误区：用精确相等比浮点、测试依赖顺序、不固定种子、无超时、靠重试掩盖 flaky、只测正常路径、无回归基线、忽略 GPU 非确定性、golden 不定期更新。验证靠重复运行与容差。

#### 2. 问题分析

逐条给实验：精确比较（失败）、顺序依赖（单独跑失败）、种子（flaky）、重试（掩盖）、回归（无基线）。不变量：测试独立、可复现、有容差与基线。

#### 3. 具体答案

面试中可以这样回答：
> 测试治理的误区会让 CI 的红绿失去意义。第一，用 `==` 比较浮点张量：GPU 归约非确定，必须用 `torch.testing.assert_close(rtol, atol)`。第二，测试依赖执行顺序或共享状态：单独跑就失败，说明用例没有隔离，要每个用例自建环境。第三，不固定随机种子和确定性算法：同样代码时红时绿，判据不可信。第四，不设超时：一个 hang 的集合通信会让 CI 卡死。第五，用重试掩盖 flaky：真正的问题是未固定的随机源或资源竞争，重试只是把 bug 隐藏成偶发。第六，只测正常路径：不给边界输入、异常输入、大 shape、极端 dtype，覆盖不到真正的失败路径。第七，没有回归基线和 golden：改了代码不知道有没有破坏旧行为。第八，golden 从不更新：越过容差被永久忽略。验证方法：重复运行 N 次验证稳定、单独运行验证独立、故意引入 flaky 观察并修复。

#### 4. 技术洞察

不变量：测试与随机性/顺序解耦；`assert_close` 容差；每红线可归因。可验证：N 次重复全绿；乱序运行仍通过；失败附最小复现。

#### 5. 图文说明

| 误区 | 实际 | 证据 |
|---|---|---|
| 精确比较 | 非确定误判 | assert_close |
| 依赖顺序 | 不独立 | 单独跑失败 |
| 重试掩盖 | 真 bug 潜伏 | flaky 记录 |
| 只测正常路径 | 漏失败路径 | 边界用例 |

图示解释：测试质量四维（独立/可复现/容差/基线），误区破坏对应维度。

#### 6. 拓展知识

延伸：pytest 参数化与 fixture、`torch.testing`、golden 管理、CI flaky 隔离池、覆盖率门禁。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 浮点如何比较？——assert_close。
2. flaky 怎么治？——找根因隔离。
3. 如何覆盖失败路径？——边界/异常/极值用例。

#### 8. 易错点与反例

1. 用相等比较，测试永远失败或永远通过。
2. 测试间共享全局模型，顺序敏感。
3. 无超时，CI hang。

#### 9. 实践与验证

脚本：写用例并用 assert_close；重复运行验证稳定；乱序执行验证独立；构造 flaky 并修复。oracle 为“独立、可复现、有容差与基线”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B058 评分：不合格只写断言；合格能说容忍与复现；良好能识别顺序/重试/覆盖误区并验证；优秀能设计独立、可复现、含基线与 flaky 治理的测试体系。
### B059. 最小代码练习：张量/设备

- 难度：入门级
- 知识域：K01 张量/设备
- 考察能力：输入矩阵、oracle、故障注入与指标
- 题型：测试/实验与验证
- 面试频率：高频
- 前置知识：shape、stride、dtype、device
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

最小代码练习（张量/设备）：构造张量并验证 shape/stride/dtype/device；验证视图与拷贝（`data_ptr`）；验证广播与类型提升；验证 H2D 异步（pin+non_blocking）；用 Profiler 观察隐式拷贝。目标是形成可复现的最小验证脚本。

#### 2. 问题分析

设计最小实验覆盖每个不变量：`numel==prod(shape)`、视图 `data_ptr` 不变、广播形状、提升 dtype、pin 后 `is_pinned`。边界：非连续 `view` 失败。

#### 3. 具体答案

面试中可以这样回答：
> 我会写一个 20 行左右的最小脚本，把张量/设备的核心不变量都钉住。第一步，构造 `x = torch.arange(12).view(3,4)`，断言 `x.numel()==12`、`x.stride()==(4,1)`、`x.is_contiguous()`。第二步，验证视图与拷贝：`y=x.t()` 断言 `y.data_ptr()==x.data_ptr()` 且 `not y.is_contiguous()`；`z=x.clone()` 断言 `z.data_ptr()!=x.data_ptr()`。第三步，验证广播与提升：`torch.broadcast_shapes((3,1),(1,4))==(3,4)`；`(torch.ones(1,dtype=torch.float16)+torch.ones(1,dtype=torch.float32)).dtype==torch.float32`。第四步（有 GPU 时），把张量 `pin_memory().to('cuda', non_blocking=True)`，断言 `is_pinned()` 与设备；用 Profiler 观察 `aten::contiguous`/`Memcpy` 是否出现。这个脚本既是练习也是回归测试，任何设备/布局相关改动都能被它捕获。

#### 4. 技术洞察

不变量清单：`numel=prod(shape)`；视图保 `data_ptr`；`broadcast_shapes` 结果；`promote_types` 结果；`is_pinned`。可验证：脚本断言全部通过即为练习达标。

#### 5. 图文说明

| 步骤 | 断言 | 证据 |
|---|---|---|
| 构造 | numel/stride | 元数据 |
| 视图 vs 拷贝 | data_ptr | 相等/不等 |
| 广播/提升 | shape/dtype | broadcast/promote |
| 设备 | is_pinned/device | 搬运异步 |

图示解释：脚本结构图，四个步骤对应四类不变量。

#### 6. 拓展知识

延伸：把脚本升级为 pytest 用例、加 memory_format、加多流重叠计时、加 ncu 观测。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 怎么验证视图？——`data_ptr` 相同。
2. 怎么验证提升？——`promote_types`。
3. 怎么验证异步？——Profiler 重叠。

#### 8. 易错点与反例

1. 只断言形状不查 stride，漏掉布局问题。
2. 用 `==` 比较张量，得到逐元素 bool 张量而非整体判断。
3. 在无 GPU 环境跑设备断言，未做跳过。

#### 9. 实践与验证

脚本：按四步写出可运行断言脚本并全部通过；无 GPU 时用 `torch.cuda.is_available()` 跳过。oracle 为“所有不变量断言通过”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B059 评分：不合格只会创建张量；合格能写基本断言；良好能覆盖视图/广播/提升/设备四类不变量；优秀能把脚本工程化为可复用回归测试并扩展多流与 ncu 观测。
### B060. 最小代码练习：autograd

- 难度：入门级
- 知识域：K02 autograd
- 考察能力：输入矩阵、oracle、故障注入与指标
- 题型：测试/实验与验证
- 面试频率：高频
- 前置知识：requires_grad、leaf、view、detach、反向
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

最小代码练习（autograd）：验证 `requires_grad` 传播、leaf 与 `grad_fn`、梯度累加、`retain_grad`、非标量 backward 需 `grad_outputs`、in-place 报错、`gradcheck` 数值对拍、`no_grad`/`detach` 行为。

#### 2. 问题分析

设计最小实验覆盖：传播（or 规则）、累加（两次回传）、非叶子（None→retain_grad）、非标量（grad_outputs）、in-place（报错）、gradcheck。边界：detach 共享数据。

#### 3. 具体答案

面试中可以这样回答：
> 我写一个最小脚本把 autograd 的契约逐条钉住。第一，传播：`x=torch.tensor([2.],requires_grad=True); y=x*3; assert y.requires_grad and y.grad_fn is not None`。第二，叶子与累加：`y.sum().backward()` 后 `assert x.grad==3`；再 backward 一次（retain_graph）后 `x.grad==6`，证明累加。第三，非叶子：断言中间量 `.grad is None`，`中间量.retain_grad()` 后回传可得梯度。第四，非标量：`torch.autograd.grad(y, x, torch.ones_like(y))` 返回形状匹配的梯度；直接 `y.backward()` 报错。第五，in-place：构造 `a=torch.tensor(1.,requires_grad=True); b=a*a; a.add_(1)` 后回传触发版本检查错误。第六，`gradcheck` 对自定义/复合函数做数值对拍。这个脚本把 autograd 的“传播-累加-叶子-形状-版本”五条契约都覆盖了。

#### 4. 技术洞察

不变量：`requires_grad(out)=any(in)`；`.grad` 累加；仅叶子持久；`grad.shape==tensor.shape`；in-place 破坏版本。可验证：脚本断言 + `gradcheck`。

#### 5. 图文说明

| 步骤 | 断言 | 证据 |
|---|---|---|
| 传播 | out.requires_grad | grad_fn |
| 累加 | 两次回传翻倍 | .grad |
| 中间量 | 默认 None | retain_grad |
| in-place | 报错 | RuntimeError |

图示解释：前向链与反向链并排，标注五条契约检查点。

#### 6. 拓展知识

延伸：升级为 pytest、加 `autograd.grad` 的 allow_unused、加二阶导 `create_graph`、加 hook 观测。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 如何证明累加？——两次回传 `.grad` 翻倍。
2. 中间量梯度？——默认 None，retain_grad。
3. 如何测自定义反向？——gradcheck。

#### 8. 易错点与反例

1. 断言 `中间量.grad` 有值，实际为 None。
2. 直接对非标量 backward，未传 grad_outputs 报错。
3. 在需要梯度的张量上 in-place，未预期报错。

#### 9. 实践与验证

脚本：按六步写断言并通过；`gradcheck` 校验一个复合函数。oracle 为“五条契约断言 + gradcheck 全通过”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B060 评分：不合格只会 backward；合格能验证传播与累加；良好能覆盖非叶子/非标量/in-place；优秀能把练习固化为 gradcheck 驱动的 autograd 回归测试。
### B061. 最小代码练习：Module/状态

- 难度：入门级
- 知识域：K03 Module/状态
- 考察能力：现象分层、假设树、止损、根因与复发防护
- 题型：故障排查与事故复盘
- 面试频率：高频
- 前置知识：参数、buffer、模式和 state_dict
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

最小代码练习（Module/状态）：定义含 Parameter、buffer、普通 Tensor 属性的模块；验证注册归属；验证 `state_dict` 往返一致；验证 `.to/.eval` 递归；复现“普通 Tensor 权重不更新”事故并复盘。恢复手段是注册为 Parameter/buffer。

#### 2. 问题分析

构造可复现事故：普通 Tensor 属性训练多步权重不变；对比 Parameter 会变。排查路径：`named_parameters()` 缺失→定位注册问题→改 Parameter。验证：state_dict 往返 `torch.equal`。

#### 3. 具体答案

面试中可以这样回答：
> 这个练习我会做成一个“事故复现+修复”。先写一个错误模块：`self.w = torch.randn(2,2)` 当权重，用 SGD 训练几步，打印 `w` 会发现它完全没变——因为普通 Tensor 属性不注册，不在 `parameters()`，优化器根本看不到它，这是“训练像随机初始化”的典型事故。排查就是打 `list(model.named_parameters())`，发现 `w` 不在其中。修复：改成 `self.w = nn.Parameter(torch.randn(2,2))`，再训练权重会更新。第二步验证状态契约：给模块加一个 `register_buffer('running', torch.zeros(1))`，保存 `state_dict` 再新建模型 `load_state_dict`，断言 `torch.equal`；断言 `running` 在 state_dict 里但不在 parameters 里。第三步验证 `.to`/`.eval` 递归：换成 meta/float 后检查参数与 buffer 同步，`eval()` 后 `training=False`。复盘结论是：凡是需要被优化、被搬运、被持久化的张量，必须走注册。

#### 4. 技术洞察

不变量：`named_parameters()` 只含 Parameter；`state_dict` 含 Parameter+持久 buffer；`to` 递归覆盖二者。可验证：错误模块权重不变、修复后变化；往返 `torch.equal` 为真。

#### 5. 图文说明

| 步骤 | 观察 | 结论 |
|---|---|---|
| 普通属性训练 | 权重不变 | 未注册 |
| Parameter 训练 | 权重变化 | 已注册 |
| state_dict 往返 | 相等 | 状态契约 |
| to/eval | 递归生效 | 统一管理 |

图示解释：注册树内/外两类对象，事故是对象落在树外。

#### 6. 拓展知识

延伸：把练习升级为 pytest、加 `requires_grad=False` 的对照、加 `load_state_dict(strict=False)` 观察提示。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 权重为何不更新？——未注册，优化器不可见。
2. 如何定位？——`named_parameters()`。
3. buffer 如何验证？——在 state_dict 不在 parameters。

#### 8. 易错点与反例

1. 以为优化器会自动发现所有张量属性。
2. 把 buffer 当参数登记，导致被更新。
3. 断言普通属性也在 state_dict。

#### 9. 实践与验证

脚本：复现权重不变，改 Parameter 后更新；state_dict 往返 equal；检查 to/eval 递归。oracle 为“注册行为、状态契约、递归均符合预期”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B061 评分：不合格不知注册；合格能复现并修复；良好能验证 state_dict 与递归；优秀能把事故做成回归测试并总结“必须注册”的判定规则。
### B062. 最小代码练习：数据管道

- 难度：入门级
- 知识域：K04 数据管道
- 考察能力：现象分层、假设树、止损、根因与复发防护
- 题型：故障排查与事故复盘
- 面试频率：高频
- 前置知识：Dataset、Sampler、collate、padding、版本
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

最小代码练习（数据管道）：实现一个小 Dataset，验证 collate 输出契约；复现“多 worker 增强重复”事故；用 `worker_init_fn` 修复；验证分片互斥；验证 pin 与 `non_blocking`。目标是形成可调试的数据管道最小脚本。

#### 2. 问题分析

构造事故：`num_workers=2` 时增强用全局 `random`，各 worker 输出相同。排查：打印 worker id 与样本；修复：`worker_init_fn` 设种。验证：各 worker 样本不同、索引集合互斥。

#### 3. 具体答案

面试中可以这样回答：
> 这个练习聚焦“多 worker 复现问题”。先写一个 Dataset，`__getitem__` 返回样本和一个随机增强值（用 `random.random()`）。设 `num_workers=2` 跑一个 epoch，收集每个样本的增强值，会发现两个 worker 产出的增强序列高度重复或完全一致——因为子进程默认继承父进程随机状态。排查打印 `torch.utils.data.get_worker_info().id` 和 seed，确认 worker 间种子相同。修复：写 `worker_init_fn(worker_id)` 里 `random.seed(seed+worker_id)` 和 `torch.manual_seed(get_worker_info().seed)`，重跑后各 worker 增强不同且可复现。第二步验证 collate 契约：默认 collate 对等长样本 stack，输出首维等于 batch size；对变长样本会报错，需自定义 pad。第三步（有 GPU 时）验证 pin：`pin_memory=True` 后 batch 张量 `is_pinned()` 为真，配 `non_blocking=True`。复盘：并行数据管道的随机性必须在 worker 内显式设种。

#### 4. 技术洞察

不变量：worker 分片互斥；增强由 worker 局部种子决定；pin 后 `is_pinned`。可验证：打印各 worker id/seed/样本；索引集合并集=全集；batch 形状契约。

#### 5. 图文说明

| 步骤 | 观察 | 结论 |
|---|---|---|
| 无 init_fn | 增强重复 | 种子未派生 |
| 有 init_fn | 增强不同 | 可分复现 |
| 默认 collate | 等长成功 | stack 契约 |
| 变长样本 | 报错 | 需自定义 |

图示解释：worker 并行图，随机流在修复前后分别汇合/分叉。

#### 6. 拓展知识

延伸：加 `persistent_workers`、加 IterableDataset 分片、加 `set_epoch`、加 pin 计时。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 为何增强重复？——worker 共享父进程随机态。
2. 如何修复？——worker_init_fn 按 worker 派生种子。
3. 变长如何 batch？——自定义 collate pad+mask。

#### 8. 易错点与反例

1. 只在主进程设种。
2. 变长样本用默认 collate。
3. pin_memory 开了但不 non_blocking。

#### 9. 实践与验证

脚本：复现并修复增强重复；打印分片；验证 collate 契约与 pin。oracle 为“增强分复现、分片互斥、collate 契约成立”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B062 评分：不合格不知 worker 种子；合格能复现并修复；良好能验证分片与 collate；优秀能把练习做成可复现数据管道回归并总结并行随机性规范。
### B063. 最小代码练习：训练/优化器

- 难度：入门级
- 知识域：K05 训练/优化器
- 考察能力：现象分层、假设树、止损、根因与复发防护
- 题型：故障排查与事故复盘
- 面试频率：高频
- 前置知识：forward、backward、累积、optimizer、scheduler
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

最小代码练习（训练/优化器）：单步训练验证更新方向 `p ← p − lr·g`；验证梯度累积等价性；复现“忘 zero_grad 导致 lr 变大”事故；验证 scheduler 顺序。目标是可断言的最小训练循环。

#### 2. 问题分析

构造事故：两步不放 zero_grad，`.grad` 翻倍。排查：打印 `.grad`。验证：step 后参数差等于 `-lr*g`。累积：K 步未归一梯度为单步 K 倍。

#### 3. 具体答案

面试中可以这样回答：
> 这个练习用单步断言把训练循环钉死。先构造 `p = nn.Parameter(torch.ones(1))`，`opt = SGD([p], lr=0.1)`。前向 `loss = p.sum()`，反传，检查 `p.grad==1`；`opt.step()` 后断言 `p==0.9`，验证更新方向。第二步复现事故：不调用 `zero_grad` 连续两步反传，打印 `p.grad` 会发现是 2（累加），`step` 后等效步长翻倍——这就是“忘清零导致 lr 变大”的复现。修复：每步开头 `opt.zero_grad()`。第三步验证梯度累积：把 batch 分成 K 份连续 backward 后一次 step，与整 batch 一次 step 对比梯度，会发现未归一时累积梯度约为 K 倍，需按累积步缩放 lr 或归一 loss。第四步验证 scheduler：`StepLR` 打印 `get_last_lr()`，确认 `scheduler.step()` 放在 `optimizer.step()` 之后。复盘：训练循环的顺序和清零是不可省的两条硬约束。

#### 4. 技术洞察

不变量：`p_new = p − lr·g`；`.grad` 累加；累积 K 步梯度≈K 倍。可验证：断言 `p` 的精确值、`.grad` 的倍数、`get_last_lr()` 序列。

#### 5. 图文说明

| 步骤 | 观察 | 结论 |
|---|---|---|
| 单步 | p=0.9 | 更新方向正确 |
| 不清零两步 | grad=2 | 累加事故 |
| K 步累积 | 梯度 K 倍 | 需归一 |
| scheduler | lr 序列 | 顺序正确 |

图示解释：单步更新数值与手算对照，事故标在缺失的 zero_grad 节点。

#### 6. 拓展知识

延伸：加 clip_grad_norm、加 Adam 状态检查、加 OneCycle lr 曲线。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 如何验证更新方向？——断言 `p==p0−lr*g`。
2. 累加如何证明？——两次回传 `.grad` 翻倍。
3. scheduler 顺序？——打印 lr 序列。

#### 8. 易错点与反例

1. 不清零当两步。
2. 累积不归一等效放大 lr。
3. scheduler 位置错 lr 不对。

#### 9. 实践与验证

脚本：单步断言、累加复现、累积对比、scheduler 序列。oracle 为“更新/累加/累积/调度全部与手算一致”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B063 评分：不合格只跑循环；合格能验证单步更新；良好能复现累加事故并处理累积；优秀能把练习固化为训练循环回归测试并给出清零/顺序/归一规范。
### B064. 最小代码练习：AMP/恢复

- 难度：入门级
- 知识域：K06 AMP/恢复
- 考察能力：现象分层、假设树、止损、根因与复发防护
- 题型：故障排查与事故复盘
- 面试频率：高频
- 前置知识：autocast、scaler、快照、随机状态
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

最小代码练习（AMP/恢复）：验证 GradScaler 初值与动态变化；复现“不用 scaler 下溢”；完整 checkpoint（model/optimizer/scaler/RNG）往返；验证溢出步被跳过。目标是可断言的 AMP+恢复脚本。

#### 2. 问题分析

构造事故：fp16 loss 不用 scaler 回传，小梯度为 0。排查：打印梯度。修复：`scaler.scale`。验证：scale 初值 65536、溢出降半、恢复一致。

#### 3. 具体答案

面试中可以这样回答：
> 这个练习覆盖 AMP 与断点恢复两个事故点。第一，验证 scaler 契约：`s=torch.amp.GradScaler('cuda'); assert s.get_scale()==65536.0`，再保存 `s.state_dict()`，确认含 scale/growth_factor/backoff_factor/growth_interval。第二，复现下溢：构造一个梯度很小的 fp16 运算，不用 scaler 直接 backward，观察梯度下溢为 0 或 loss 不动；改用 `s.scale(loss).backward()` 后梯度可表示。第三，复现溢出跳步：人为制造 inf 梯度，观察 `s.step(opt)` 返回 False 且 `s.get_scale()` 减半——证明溢出步被跳过。第四，验证恢复：保存一个完整 checkpoint（model、optimizer、scaler、epoch、RNG），重启加载后续训 5 步，与不中断运行同样 5 步比较 loss/权重，在确定性下应逐位一致；若只存模型，则 optimizer 动量与 scale 丢失、曲线偏移。复盘：AMP 的状态是训练状态的一部分，必须一起持久化。

#### 4. 技术洞察

不变量：scale 初值 65536；溢出→减半且跳步；无溢出→按 interval 翻倍；恢复集完整才逐位一致。可验证：`get_scale()`、`step` 返回值、续训对拍。

#### 5. 图文说明

| 步骤 | 观察 | 结论 |
|---|---|---|
| 初值 | 65536 | scaler 契约 |
| 溢出 | step False, scale/2 | 跳步 |
| 完整恢复 | 逐位一致 | 状态齐全 |
| 只存模型 | 曲线偏移 | 状态缺失 |

图示解释：AMP 步流水线 + 状态持久化支线，事故标在缺失支线。

#### 6. 拓展知识

延伸：加 bf16 对照（无需 scaler）、加分布式 scaler、加 RNG 保存验证。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. scale 初值？——65536。
2. 溢出会怎样？——跳步+减半。
3. 恢复要存什么？——model/optimizer/scaler/RNG/step。

#### 8. 易错点与反例

1. 不用 scaler，下溢。
2. 只存模型，续训偏移。
3. 恢复 RNG 未存，dropout 错位。

#### 9. 实践与验证

脚本：断言 scale 初值；复现下溢与溢出；完整/不完整恢复对拍。oracle 为“scale 动态正确、完整恢复逐位一致”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B064 评分：不合格只开 autocast；合格能验证 scaler 动态；良好能复现下溢/溢出并完整恢复；优秀能把练习固化为 AMP+checkpoint 回归测试并给出状态清单。
### B065. 最小代码练习：分布式

- 难度：入门级
- 知识域：K07 分布式
- 考察能力：现象分层、假设树、止损、根因与复发防护
- 题型：故障排查与事故复盘
- 面试频率：高频
- 前置知识：rank、DDP、FSDP、collective、容错
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

最小代码练习（分布式）：`torchrun` 起 2 rank；验证 `DistributedSampler` 分片互斥；验证 DDP 一步后各 rank 参数一致；复现“通信不对称导致 hang”；验证 rank0 保存+barrier。目标是最小可验证分布式脚本。

#### 2. 问题分析

构造事故：仅 rank0 进入带 all_reduce 的分支→其余 rank 卡住。排查：设 timeout 观察异常。修复：所有 rank 走相同集合通信。验证：参数哈希与分片。

#### 3. 具体答案

面试中可以这样回答：
> 这个练习用 2 个进程跑通并验证分布式的三条契约。第一，分片：用 `DistributedSampler` 包数据集，每个 rank 打印自己的索引集合，断言两 rank 集合互斥、并集等于全集；注意每个 epoch 要 `set_epoch` 才能正确 shuffle。第二，一致性：用 `DDP` 包一个线性层，跑一步前向反向，把各 rank 的参数 `sum()` 或哈希 `all_gather` 到 rank0 打印，断言逐位一致——这是 DDP 初始广播+梯度 AllReduce 的结果。第三，复现 hang 事故：写一个 `if dist.get_rank()==0: dist.all_reduce(t)` 的分支，其余 rank 不调用，运行会卡住直到初始化时设的 `timeout` 触发，证明“集合通信必须所有 rank 成对调用”。第四，保存：rank0 写 checkpoint，其余 rank 在 `dist.barrier()` 等待，验证文件完整。复盘：分布式正确性依赖“对称的集合通信 + 一致的分片 + 统一的参数”。

#### 4. 技术洞察

不变量：分片互斥覆盖全集；DDP 各 rank 参数一致；集合通信对称。可验证：索引集合、参数哈希、不对称调用的 timeout。

#### 5. 图文说明

| 步骤 | 观察 | 结论 |
|---|---|---|
| 分片 | 集合互斥 | sampler 契约 |
| 一步 | 参数一致 | DDP 一致 |
| 不对称 | hang/timeout | 对称要求 |
| 保存 | barrier 对齐 | 文件完整 |

图示解释：多 rank 对齐图，事故破坏集合通信的横向对齐。

#### 6. 拓展知识

延伸：加 `no_sync` 累积、加 FSDP 显存对比、加 NCCL 日志、加 evaluate 的 all_gather 指标。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 为什么 hang？——通信不对称。
2. 参数为何一致？——初始广播+AllReduce。
3. 分片如何验证？——索引集合互斥。

#### 8. 易错点与反例

1. 不用 DistributedSampler，数据重复。
2. 条件分支里调集合通信。
3. 多 rank 同写文件。

#### 9. 实践与验证

脚本：torchrun 2 rank 跑分片/一致性/不对称/保存四项。oracle 为“分片互斥、参数一致、不对称超时、文件完整”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B065 评分：不合格只会启动多进程；合格能验证分片与一致；良好能复现 hang 并修复；优秀能把练习固化为分布式回归并给出对称性/一致性防错规范。
### B066. 最小代码练习：编译栈

- 难度：入门级
- 知识域：K08 编译栈
- 考察能力：现象分层、假设树、止损、根因与复发防护
- 题型：故障排查与事故复盘
- 面试频率：高频
- 前置知识：Dynamo、AOTAutograd、Inductor、guard、缓存
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

最小代码练习（编译栈）：对比 eager 与 compile 的数值等价与 kernel 数；复现“动态 shape 重编译”；复现“graph break”；验证 warmup 后稳态加速。目标是最小可观测编译脚本。

#### 2. 问题分析

构造事故：热路径 `.item()` 分支导致每次 graph break。排查：`TORCH_LOGS=graph_breaks`。验证：kernel 数下降、recompiles 日志、warmup 后计时。

#### 3. 具体答案

面试中可以这样回答：
> 这个练习用日志和计时验证编译的三件事。第一，数值等价：定义函数 `f(x)=relu(x@w).sum()`，分别 eager 与 `torch.compile(f)` 跑同一输入，断言 `torch.allclose`。第二，融合收益：用 Profiler 数 kernel，eager 有多个 elementwise kernel，compiled 融合后 kernel 数下降。第三，复现重编译：调用 compiled 函数时传入不同 shape 的输入，打开 `TORCH_LOGS=recompiles` 观察 guard 失效触发重编译；说明固定输入签名的重要性。第四，复现 graph break：在被编译函数里插入 `if x.item()>0:` 分支，打开 `TORCH_LOGS=graph_breaks` 观察断图，该段退回 eager。第五，warmup 与稳态：先 warmup 几轮，再计时 compiled 与 eager，确认编译开销被摊掉后 compiled 更快；对短任务直接计时则可能更慢。复盘：编译收益要“先 warmup、看 kernel 数、查重编译、避免 graph break”四步确认，不能只看一次计时。

#### 4. 技术洞察

不变量：域内 allclose；guard 命中才复用；融合减少 kernel 数。可验证：allclose、recompiles 日志、graph_breaks 日志、稳态计时。

#### 5. 图文说明

| 步骤 | 观察 | 结论 |
|---|---|---|
| 等价 | allclose 通过 | 数值不变 |
| 融合 | kernel 数下降 | 收益来源 |
| 变 shape | recompiles 日志 | guard 失效 |
| .item() | graph_breaks 日志 | 断图 |

图示解释：编译流水线，四个观测点分别打在等价、融合、缓存、捕获环节。

#### 6. 拓展知识

延伸：加 `dynamic_shapes` 覆盖 shape 范围、加 `mode='reduce-overhead'` 观察 CUDA Graph、加 AOTInductor 落盘。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 如何确认等价？——allclose。
2. 何时重编译？——guard 失效。
3. graph break 信号？——graph_breaks 日志。

#### 8. 易错点与反例

1. 不 warmup 就下结论。
2. 热路径 `.item()` 断图。
3. shape 抖动重编拖慢。

#### 9. 实践与验证

脚本：等价断言、kernel 计数、重编译日志、graph break 日志、稳态计时。oracle 为“等价、融合、缓存行为可观测”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B066 评分：不合格盲信 compile；合格能验证等价；良好能用日志定位重编译/断图；优秀能给出 warmup+日志+计时的完整编译验证方法并解释收益来源。
### B067. 最小代码练习：性能

- 难度：入门级
- 知识域：K09 性能
- 考察能力：现象分层、假设树、止损、根因与复发防护
- 题型：故障排查与事故复盘
- 面试频率：高频
- 前置知识：关键路径、带宽、算力、通信、显存、统计
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

最小代码练习（性能）：用 warmup + CUDA Event/benchmark 计时；分段计时定位瓶颈；用 Roofline 判断受限类型；复现“Profiler 全开改变行为”；报告分位数。目标是可复现的性能验证脚本。

#### 2. 问题分析

构造：分段计时一个训练步，确认占比；算 matmul/elementwise 的算术强度判断受限；对比全量/采样 Profiler。边界：小算子受启动延迟主导。

#### 3. 具体答案

面试中可以这样回答：
> 这个练习把性能测量做成可复现的流程。第一，warmup：先跑若干轮丢弃，再用 `torch.cuda.Event` 计时或 `torch.utils.benchmark`，报告多次运行的中位数与四分位；不做 warmup 会看到首轮明显更慢。第二，分段：用 Event 分别测数据加载、H2D、前向、反向、optimizer.step，画占比，定位瓶颈环节；若 GPU 有空档而 CPU 忙，瓶颈在数据。第三，受限类型：对 matmul 和 elementwise 分别算算术强度 `FLOPs/Bytes` 并与 `峰值算力/带宽` 比，判断前者计算受限、后者带宽受限，再用 Profiler 的 achieved FLOPS/BW 对照。第四，测量干扰：对比全量 Profiler 与 `schedule` 采样 Profiler 的墙钟，证明全量开有开销会改变时间分布。第五，分位数：报告 p50/p99 而非仅平均，暴露长尾。复盘：性能结论必须带 warmup、分段、受限类型、分位和环境元数据。

#### 4. 技术洞察

不变量：`T_e2e` 可拆分为可重叠项取 max；受限类型由 `AI vs AI*` 决定。可验证：分段占比和为 1；achieved BW/FLOPS 与 Roofline 一致；warmup 前后差异。

#### 5. 图文说明

| 步骤 | 观察 | 结论 |
|---|---|---|
| warmup | 首轮更慢 | 排除冷启动 |
| 分段 | 各段占比 | 定位瓶颈 |
| Roofline | 受限类型 | 优化方向 |
| Profiler | 全量更慢 | 采样需要 |

图示解释：时间轴分段 + Roofline 折线，两个视图共同支撑结论。

#### 6. 拓展知识

延伸：加 CUDA Graph 测试、加频率锁定、加 memory snapshot、加 p99 归因。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 为何 warmup？——排除冷启动。
2. 如何定位瓶颈？——分段占比。
3. 为什么看 p99？——长尾体验。

#### 8. 易错点与反例

1. 不 warmup 直接平均。
2. 只报平均漏长尾。
3. 全量 Profiler 污染结果。

#### 9. 实践与验证

脚本：warmup+分位计时、分段占比、Roofline 分类、Profiler 对比。oracle 为“测量稳定、分段合理、分类正确”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B067 评分：不合格只看单次总时间；合格能 warmup 并分段；良好能判断受限类型与分位；优秀能给出含环境元数据、单变量对照与尾延迟的性能验证方法。
### B068. 最小代码练习：导出部署

- 难度：入门级
- 知识域：K10 导出部署
- 考察能力：现象分层、假设树、止损、根因与复发防护
- 题型：故障排查与事故复盘
- 面试频率：高频
- 前置知识：state_dict、Export、runtime、schema、迁移
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

最小代码练习（导出部署）：导出一个含动态 batch 的模型；`onnx.checker` 校验；ORT 多 shape 对拍；复现“忘 eval 导致输出随机”；复现“trace 固化分支”。目标是可验证导出脚本。

#### 2. 问题分析

构造事故：不 eval 导出 Dropout 模型→输出随机。排查：同一输入两次结果不同。修复：`eval()`。验证：多 shape allclose、checker 通过。

#### 3. 具体答案

面试中可以这样回答：
> 这个练习用最简模型覆盖导出的关键契约。第一，正常导出：定义含 Linear+ReLU 的模型，`model.eval()`，构造 dummy input，`torch.onnx.export(..., dynamic_axes={'input':{0:'batch'}})`，用 `onnx.checker.check_model` 校验结构。第二，数值对拍：用 ONNX Runtime 在 batch=1 和 batch=8 上推理，与 PyTorch 输出 `assert_close`，确认动态轴与数值都正确。第三，复现事故：在模型里加 Dropout 且在 train 模式导出，用同一输入跑两次 ORT，输出不同——说明训练态被固化；修复是导出前 `eval()`。第四，复现 trace 问题：写一个 `if x.sum()>0` 分支的模型用 `torch.jit.trace` 导出，换一个触发另一分支的输入会发现结果与 PyTorch 不符，证明 trace 固化执行路径，需改用 `torch.export` 或改写。复盘：导出必须 eval、声明动态轴、对拍多 shape，并警惕数据依赖控制流。

#### 4. 技术洞察

不变量：eval 冻结行为；导出图域内等价；trace 只记录执行路径。可验证：checker 通过、多 shape allclose、train 模式输出随机、trace 分支错误。

#### 5. 图文说明

| 步骤 | 观察 | 结论 |
|---|---|---|
| 正常导出 | checker+allclose | 契约成立 |
| 忘 eval | 输出随机 | 训练态固化 |
| trace 分支 | 结果不符 | 路径固化 |
| 多 shape | 等价 | 动态轴正确 |

图示解释：导出流水线，事故标在 eval 与分支固化两处。

#### 6. 拓展知识

延伸：加 `torch.export` 对照、加量化导出、加 TensorRT 精度 profile。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 为何 eval？——冻结 dropout/BN。
2. trace 为何危险？——分支固化。
3. 如何验证？——多 shape allclose。

#### 8. 易错点与反例

1. 忘 eval，输出随机。
2. trace 分支模型，换输入错。
3. 只 checker 不对拍。

#### 9. 实践与验证

脚本：正常导出+对拍；复现 eval 与 trace 事故。oracle 为“checker 通过、多 shape 等价、事故可复现”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B068 评分：不合格只导出；合格能 eval+checker；良好能多 shape 对拍并复现事故；优秀能用 export 替代 trace、覆盖动态轴边界并总结导出验证清单。
### B069. 最小代码练习：自定义算子

- 难度：入门级
- 知识域：K11 自定义算子
- 考察能力：现象分层、假设树、止损、根因与复发防护
- 题型：故障排查与事故复盘
- 面试频率：高频
- 前置知识：schema、dispatcher、stream、autograd、ABI
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

最小代码练习（自定义算子）：用 `autograd.Function` 实现一个可微算子；`gradcheck` 通过；复现“backward 返回数量错”报错；用 `opcheck` 校验 schema。目标是可验证自定义算子脚本。

#### 2. 问题分析

构造事故：backward 少返回一个梯度→gradcheck/回传报错。排查：对比 forward 输入数量。验证：gradcheck 数值通过、opcheck 契约。

#### 3. 具体答案

面试中可以这样回答：
> 这个练习从一个可微算子入手。第一，实现：用 `torch.autograd.Function` 写 `MySquare.forward(ctx, x)` 保存 `x`，`backward` 返回 `grad_output*2*x`；调用后用 `torch.autograd.gradcheck(MySquare.apply, (x,))` 做数值梯度对拍，确认通过。第二，复现事故：把 `backward` 改成只返回一个梯度但 forward 有两个输入，`gradcheck` 或反传会报返回数量不匹配——这是最常见的自定义算子错误。第三，schema 契约：如果用 `torch.library` 注册，用 `torch.library.opcheck(fn, args)` 检查别名、可变性和 autograd 行为；故意声明不别名但原地改输入，opcheck 会报错。第四，与 compile：普通 Python 函数在 `torch.compile` 下可能 graph break，而 `torch.library.custom_op` 注册的算子可被图捕获，可对比 `torch._dynamo.explain` 的断点数。复盘：自定义算子必须“数值（gradcheck）+ 契约（opcheck）+ 可编译”三方面验证。

#### 4. 技术洞察

不变量：`gradcheck` 数值通过；backward 返回数=forward 输入数；schema 与实现一致。可验证：gradcheck 通过、opcheck 全绿、explain 断点数。

#### 5. 图文说明

| 步骤 | 观察 | 结论 |
|---|---|---|
| gradcheck | 通过 | 可微正确 |
| 错 backward | 报错 | 数量契约 |
| opcheck | 全绿/报错 | schema 一致 |
| compile | 断点/不断 | 可捕获 |

图示解释：算子实现经过数值、契约、编译三道验证门。

#### 6. 拓展知识

延伸：加 CUDA 扩展实现、加二阶导 `create_graph`、加自定义算子前的 profile 证据。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 如何验证可微？——gradcheck。
2. 常见错误？——backward 数量/顺序不符。
3. 如何校验 schema？——opcheck。

#### 8. 易错点与反例

1. backward 返回值与输入不匹配。
2. 未声明 alias 却原地改。
3. 未用 custom_op 导致 compile 断图。

#### 9. 实践与验证

脚本：实现+gradcheck；复现 backward 错误；opcheck 校验；compile 断点对比。oracle 为“数值与契约通过”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B069 评分：不合格随手写算子；合格能实现并 gradcheck；良好能复现 backward 错误并 opcheck；优秀能结合 custom_op 与 compile 给出可维护的可微算子验证流程。
### B070. 最小代码练习：测试治理

- 难度：入门级
- 知识域：K12 测试治理
- 考察能力：现象分层、假设树、止损、根因与复发防护
- 题型：故障排查与事故复盘
- 面试频率：高频
- 前置知识：oracle、证据、日志、回滚、门禁、复盘
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

最小代码练习（测试治理）：写 pytest 用例覆盖 shape/dtype/边界；用 `assert_close` 容差；固定种子验证可复现；复现 flaky 并修复；用参数化覆盖多设备/多 dtype。目标是可复用的最小测试套件。

#### 2. 问题分析

构造事故：测试依赖时间/顺序→flaky。排查：重复运行观察波动。修复：固定种子与独立 fixture。验证：重复 N 次全绿、边界用例覆盖。

#### 3. 具体答案

面试中可以这样回答：
> 这个练习把测试治理落成可运行的 pytest。第一，单元用例：对一个小函数用 `torch.testing.assert_close(out, ref, rtol=1e-4, atol=1e-5)`，避免精确比较；用 `pytest.mark.parametrize` 覆盖 shape、dtype、device 组合。第二，边界：加空 batch、单元素、超长序列等边界输入用例，验证不崩溃且行为符合契约。第三，可复现：用例开始 `torch.manual_seed(0)`，重复运行 N 次结果一致；若引入 `time.time()` 依赖会变 flaky，排查后改为固定值。第四，复现 flaky：故意写一个依赖全局状态的用例，单独跑通过、和其他用例一起跑失败，说明用例未隔离，修复为每个用例自建模型。第五，超时与标记：给慢用例打 `slow` 标记并设超时，避免 CI 卡死。复盘：好的测试独立、可复现、有容差、覆盖边界，并且失败可诊断。

#### 4. 技术洞察

不变量：用例独立且可复现；数值用容差；边界被覆盖。可验证：重复 N 次全绿、乱序运行通过、边界用例存在且通过。

#### 5. 图文说明

| 步骤 | 观察 | 结论 |
|---|---|---|
| 容差比较 | 通过 | 避免精确比较 |
| 固定种子 | 稳定 | 可复现 |
| flaky | 单跑过/合跑挂 | 未隔离 |
| 参数化 | 多组合 | 覆盖边界 |

图示解释：测试套件结构，参数化+边界+fixture 三根支柱。

#### 6. 拓展知识

延伸：加 golden 对拍、加覆盖率门禁、加 CI 分 job、加性能基线。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 浮点如何比较？——assert_close。
2. flaky 怎么修？——隔离状态、固定种子。
3. 如何覆盖边界？——参数化+边界输入。

#### 8. 易错点与反例

1. 精确比较偶发失败。
2. 用例共享全局状态，顺序敏感。
3. 无慢用例标记，CI 超时。

#### 9. 实践与验证

脚本：写 pytest 含参数化/边界/固定种子；重复与乱序运行验证稳定；复现并修复 flaky。oracle 为“独立、可复现、覆盖边界”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B070 评分：不合格只写断言；合格能用容差与参数化；良好能复现修复 flaky 并覆盖边界；优秀能搭建含 golden、覆盖率、CI 分 job 的可治理测试套件。
### B071. 边界条件：张量/设备

- 难度：入门级
- 知识域：K01 张量/设备
- 考察能力：资源预算、吞吐、尾延迟、容量阈值与单位成本
- 题型：性能/容量/成本
- 面试频率：高频
- 前置知识：shape、stride、dtype、device
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

张量/设备的边界主要由容量与数值范围决定：元素数 `numel` 用 int64 索引、超过显存可用量即 OOM；`int32` 类索引/偏移溢出（约 2.1e9）；0 维（标量）张量与空张量（`numel=0`）在算子中的语义；fp16 范围 `±65504`；广播展宽使 `numel` 激增。这些边界在容量和成本上直接表现为 OOM、溢出或静默错误。

#### 2. 问题分析

按容量/成本列出边界：显存上界 `numel*itemsize`、索引上界 2^31、数值上界 fp16。边界反例：`torch.arange(2**31, dtype=torch.int32)` 溢出；空 batch 的归约；`(N,1)+(N,)` 广播成 `(N,N)` 使 `numel` 平方级膨胀。

#### 3. 具体答案

面试中可以这样回答：
> 张量/设备的边界有三个维度：容量、数值范围、形状退化。容量上，一个张量占 `numel*itemsize` 字节，任何中间结果超过可用显存就 OOM；广播尤其危险，`(N,1)+(N,)` 会产生 `(N,N)`，`numel` 从 O(N) 变成 O(N²)，例如 N=1e5 就是 1e10 元素、fp32 要 40GB。数值上，索引/偏移常用 int32，超过 2^31−1 溢出，`arange` 或大 embedding 索引会出错；fp16 超过 65504 变 inf。形状退化上，0 维标量张量和空张量（`numel==0`）在多数算子可运行但要小心 `mean()` 得到 nan、`max()` 报无元素错误。成本敏感场景要预估中间张量峰值，必要时分块或用 checkpoint 换显存。验证方式是算 `numel*itemsize`、用 `torch.iinfo`/`finfo` 查范围、构造广播反例。

#### 4. 技术洞察

不变量：`bytes=numel*itemsize`；广播后 `numel=Π max(dim)`；索引 dtype 决定上界。可验证：`torch.iinfo(torch.int32).max==2**31-1`；空张量 `.sum()==0` 而 `.mean()` 为 nan；广播形状由 `broadcast_shapes` 给出。

#### 5. 图文说明

| 边界 | 阈值 | 后果 |
|---|---|---|
| 显存容量 | numel*itemsize | OOM |
| 索引范围 | 2^31 | int32 溢出 |
| 数值范围 | 65504 (fp16) | inf |
| 广播膨胀 | 维乘积 | O(N²) 显存成本 |

图示解释：容量/数值/形状三轴边界图，越界点标出失败模式。

#### 6. 拓展知识

延伸：`torch.chunk`/分块、梯度检查点、`torch.finfo`、`int64` 索引、`expand` 避免实体化。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 何时 OOM？——中间张量峰值超显存。
2. 索引为何要 int64？——避免 2^31 溢出。
3. 广播为何成本高？——结果 numel 可能平方级。

#### 8. 易错点与反例

1. 用 int32 存超大偏移，溢出成负。
2. 空 batch 直接 `.max()`，报无元素。
3. 大 N 广播成 (N,N)，瞬间 OOM。

#### 9. 实践与验证

脚本：断言 `iinfo/finfo` 范围；构造 `(100000,1)+(100000,)` 估算显存但用 `broadcast_shapes` 不实体化；空张量 `.sum/.mean` 行为对比。oracle 为“边界阈值与后果可预测”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B071 评分：不合格不考虑边界；合格能说 OOM 与范围；良好能量化容量/数值/广播三类边界；优秀能结合分块、checkpoint、int64 索引给出容量预算与成本控制方案。
### B072. 边界条件：autograd

- 难度：入门级
- 知识域：K02 autograd
- 考察能力：资源预算、吞吐、尾延迟、容量阈值与单位成本
- 题型：性能/容量/成本
- 面试频率：高频
- 前置知识：requires_grad、leaf、view、detach、反向
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

autograd 的边界主要是计算图规模与显存：保存的中间张量决定显存峰值，长序列/大模型下反向图可能 OOM；二阶导需 `create_graph=True`，图更大；梯度累积增加步数而非图大小；`retain_graph` 会常驻图增加显存。边界反例：RNN/Transformer 长序列激活显存随长度线性甚至平方增长。

#### 2. 问题分析

按容量成本列出：图节点数、保存张量字节、二阶图。推导：激活显存 ≈ 每层保存张量之和，Transformer 注意力中间量 O(seq²)。边界：梯度检查点用计算换显存；`retain_graph` 使图不释放。

#### 3. 具体答案

面试中可以这样回答：
> autograd 的边界本质是“计算图规模决定显存峰值”。前向为反向保存的中间张量都要驻留，直到对应部分回传完成；网络越深、序列越长，保存量越大，长序列 Transformer 的注意力中间量随序列长度呈平方增长，这是反向 OOM 的主因之一。几个边界要记：第一，`retain_graph=True` 会让图在回传后继续驻留，多次回传才释放，显存成本叠加。第二，二阶导要 `create_graph=True`，反向图本身又要建图，显存和计算都显著增加。第三，梯度累积不增大图（每步释放），只是把多步梯度累加，是省显存的正道；梯度检查点用前向重算换取不保存中间量，把激活显存从 O(层数×中间量) 降到接近 O(层数)。第四，图的节点数与算子数成正比，Python 级循环里的细碎算子会放大图。验证方式：用 memory snapshot 看激活占比，开/关 checkpoint 对比峰值。

#### 4. 技术洞察

不变量：前向保存张量总量 ≈ 反向激活显存峰值；`retain_graph` 推迟释放；二阶图规模更大。可验证：memory snapshot 中 activation 区块；checkpoint 前后峰值差。

#### 5. 图文说明

| 边界 | 成本 | 缓解 |
|---|---|---|
| 长序列注意力 | O(seq²) | 分块/FlashAttention |
| retain_graph | 图常驻 | 用后关闭 |
| 二阶导 | 图套图 | 只在必要时用 |
| 深网络激活 | O(层数) | 梯度检查点 |

图示解释：显存峰值由激活主导，checkpoint 把“存储”换成“重算”。

#### 6. 拓展知识

延伸：`torch.utils.checkpoint`、FlashAttention、`set_detect_anomaly`（会保存更多信息）、激活重计算与重物化。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 反向为何耗显存？——保存的激活。
2. retain_graph 代价？——图不释放。
3. 如何省激活？——梯度检查点/分块注意力。

#### 8. 易错点与反例

1. 循环里一直 retain_graph，显存逐步耗尽。
2. 长序列不分块，注意力 OOM。
3. 用梯度累积误以为增大了图。

#### 9. 实践与验证

脚本：memory snapshot 对比开/关 checkpoint 的峰值；长序列注意力显存随 seq 增长曲线；retain_graph 前后显存对比。oracle 为“激活主导峰值、checkpoint 降峰”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B072 评分：不合格不考虑激活；合格能说反向存激活；良好能量化长序列/retain/二阶边界；优秀能结合 checkpoint、分块注意力、梯度累积给出显存预算与容量方案。
### B073. 边界条件：Module/状态

- 难度：入门级
- 知识域：K03 Module/状态
- 考察能力：资源预算、吞吐、尾延迟、容量阈值与单位成本
- 题型：性能/容量/成本
- 面试频率：高频
- 前置知识：参数、buffer、模式和 state_dict
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

Module/状态的边界主要是参数量与状态容量：全量微调时参数+梯度+优化器状态约为参数量的 4 倍（Adam fp32）；大 embedding/词表使参数集中在少数张量；`state_dict` 体积随参数量增长，checkpoint 写入/读取成为瓶颈；冻结/部分微调（LoRA）改变可训练参数占比。边界反例：单卡装不下全量微调，需 FSDP/ZeRO 或参数高效微调。

#### 2. 问题分析

按容量成本列出：参数显存、优化器状态、checkpoint 体积、通信量。推导：Adam fp32 约 `4P·w`；LoRA 只训练低秩增量，可训练参数从 P 降到约 `r·(d_in+d_out)` 量级。边界：参数集中在大 embedding 时，切分粒度受限。

#### 3. 具体答案

面试中可以这样回答：
> Module/状态的边界可以用参数量 P 来预算。全量微调显存下界（不含激活）约 `4P·w`（参数+梯度+Adam 两动量），所以 7B 参数 fp32 就要约 112GB，单卡装不下，必须靠 FSDP/ZeRO 分片或参数高效微调。checkpoint 体积同样随 P 增长，写入/读取成为瓶颈，常配合只存可训练参数或分片保存。参数分布也有边界：大词表 embedding 让参数量集中在少数大张量，FSDP 切分时这些大张量决定单卡峰值。参数高效微调（如 LoRA）把可训练参数从 P 降到低秩增量 `r·(d_in+d_out)`，优化器状态随之骤减，是小显存下微调大模型的常规手段。冻结层要从优化器剔除才能真正省下梯度与状态。验证方式：`sum(p.numel())` 估算倍数、对比全量/ LoRA 的显存与 checkpoint 大小。

#### 4. 技术洞察

不变量：训练显存（不含激活）≈ `P·w·(1+grad+optimizer)`；checkpoint 体积 ∝ P。可验证：参数量统计与显存/文件大小对账；LoRA 可训练参数量公式。

#### 5. 图文说明

| 边界 | 量 | 缓解 |
|---|---|---|
| 全量微调显存 | ~4P·w | FSDP/ZeRO |
| checkpoint 体积 | ∝P | 只存可训练/分片 |
| 大 embedding | 参数集中 | 切分粒度 |
| 小显存微调 | 需降可训练量 | LoRA/PEFT |

图示解释：参数量 P 为横轴，显存/文件/通信随 P 增长，几种缓解手段降低斜率。

#### 6. 拓展知识

延伸：LoRA/QLoRA、FSDP 分片优化器状态、8-bit Adam、梯度检查点、checkpoint 异步写盘。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 全量微调显存为何 4 倍？——梯度+两动量。
2. 如何省？——FSDP 或 LoRA。
3. 冻结够吗？——要同时移出优化器。

#### 8. 易错点与反例

1. 单卡全量微调 7B，直接 OOM。
2. 只冻结但仍在优化器里，没省状态。
3. checkpoint 全量保存，磁盘/时间爆炸。

#### 9. 实践与验证

脚本：统计 P 估算 4P 显存并与 `memory_allocated` 对账；LoRA 前后可训练参数量与 checkpoint 大小对比。oracle 为“参数量与成本对得上”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B073 评分：不合格不看参数量；合格能算 4P 显存；良好能说明 checkpoint/大 embedding 边界；优秀能结合 FSDP、LoRA、8-bit 优化器给出容量受限下的微调方案。
### B074. 边界条件：数据管道

- 难度：入门级
- 知识域：K04 数据管道
- 考察能力：资源预算、吞吐、尾延迟、容量阈值与单位成本
- 题型：性能/容量/成本
- 面试频率：高频
- 前置知识：Dataset、Sampler、collate、padding、版本
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

数据管道的边界主要是吞吐/内存权衡与数据形态：worker 数受 CPU/fd/内存限制，`prefetch_factor` 决定在途批数与内存；变长序列需 pad 到 batch 内最大长度，pad 浪费随长度方差增大；超大 batch 使 pin 内存与显存压力上升；磁盘/解码带宽是硬上限。边界反例：worker 过多 fd 耗尽、全 pad 到全局最大长度浪费严重。

#### 2. 问题分析

按成本列出：worker 内存、prefetch 缓冲、pad 浪费、磁盘带宽。推导：pad 浪费 ≈ `(batch*max_len − Σ len)/...`；prefetch 内存 ≈ `W*prefetch*batch_bytes`。边界：num_workers 拐点、pad 比例。

#### 3. 具体答案

面试中可以这样回答：
> 数据管道的边界是“吞吐换资源”。第一，worker 数不是越大越好：每个 worker 是独立进程，有内存、文件描述符和 IPC 成本，超过 CPU 核数后吞吐到拐点甚至下降，还会因为 fd 耗盡报错。第二，`prefetch_factor` 提供缓冲让取数与计算重叠，但在途批数约 `W*prefetch_factor`，内存占用随之线性增长，大 batch 高分辨率下可能吃掉大量主机内存。第三，变长序列必须 pad 才能 stack，pad 浪费 = 总 pad token / 总 token，长度方差越大浪费越多；按长度分桶（bucketing）或动态 batch 可显著降低。第四，磁盘/解码带宽是硬上限，热数据可缓存到内存或预解码。第五，pin 内存不可换出，过度 pin 会影响整机。验证方式：扫 worker 数与 prefetch 找吞吐拐点和内存占用；统计 pad 比例；测磁盘吞吐。

#### 4. 技术洞察

不变量：稳态吞吐受最慢环节限制；pad 浪费由长度方差决定；prefetch 内存 ∝ `W·prefetch_factor`。可验证：计时曲线找拐点；统计 pad token 占比；监控主机内存。

#### 5. 图文说明

| 边界 | 成本 | 缓解 |
|---|---|---|
| worker 数 | fd/内存/IPC | 找拐点 |
| prefetch | 主机内存 | 调深度 |
| 变长 pad | token 浪费 | 分桶/动态 batch |
| 磁盘带宽 | 吞吐上限 | 缓存/预解码 |

图示解释：吞吐-资源权衡曲线，拐点左侧欠并行、右侧过并行。

#### 6. 拓展知识

延伸：`bucket_by_length`、动态 padding、`IterableDataset` 流式、内存映射数据、`num_workers` 与 CPU 亲和性。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. worker 最优？——有拐点，需实测。
2. pad 浪费如何降？——分桶、动态 padding。
3. prefetch 代价？——主机内存。

#### 8. 易错点与反例

1. worker 设过大，fd 耗尽。
2. 全 pad 到全局最大长度，算力浪费。
3. prefetch 过深，主机内存 OOM。

#### 9. 实践与验证

脚本：扫 num_workers/prefetch 记录吞吐与内存；统计 pad 比例；对比分桶前后有效 token 率。oracle 为“拐点可测、pad 可降、内存可控”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B074 评分：不合格盲调参数；合格能说 worker/pad 边界；良好能量化拐点与 pad 浪费；优秀能结合分桶、动态 batch、流式数据给出吞吐-内存权衡方案。
### B075. 边界条件：训练/优化器

- 难度：入门级
- 知识域：K05 训练/优化器
- 考察能力：资源预算、吞吐、尾延迟、容量阈值与单位成本
- 题型：性能/容量/成本
- 面试频率：高频
- 前置知识：forward、backward、累积、optimizer、scheduler
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

训练/优化器的边界主要是 batch 规模与显存/收敛的权衡：大 batch 用梯度累积模拟，但受 lr 缩放与归一影响；Adam 状态随参数量线性增长；过长训练步数使 scheduler 进入尾部；极端 lr 导致发散或停滞；混合精度下的溢出。边界反例：batch 无限增大后收敛反而变差（泛化 gap），需要 lr warmup。

#### 2. 问题分析

按成本列出：batch 的显存/通信、累积状态、lr 上界、步数。推导：有效 batch = micro_batch × grad_accum × world_size；大 batch 常需线性缩放 lr + warmup。边界：累积步数过多使 BN 统计或优化动态改变。

#### 3. 具体答案

面试中可以这样回答：
> 训练/优化器的边界围绕“batch 规模与显存/收敛”。第一，大 batch 装不下时用梯度累积模拟：有效 batch = micro_batch × 累积步数 × 卡数，但要注意归一化（loss 除以累积步或梯度平均），否则等效放大 lr。第二，大 batch 常需要相应放大 lr 并加 warmup，否则早期容易发散；但 batch 无限增大后泛化可能变差，收益递减。第三，Adam 的优化器状态随参数量线性增长，是长训练的最大常驻成本，配合 FSDP/8-bit 才能扩展。第四，极端学习率会导致 loss 爆炸或停滞，需要梯度裁剪和 warmup 保护。第五，步数边界：scheduler 尾段 lr 很小、收敛变慢，OneCycle/Cosine 需与总步数匹配。验证方式：扫描有效 batch 与 lr 的关系观察是否发散；监控梯度范数与显存；对比累积归一前后的收敛。复盘：规模和 lr 是耦合的，改一个必须评估另一个。

#### 4. 技术洞察

不变量：`有效 batch = micro × accum × world`；Adam 状态 ∝ P；大 batch 与 lr 需协同缩放。可验证：梯度范数随 lr 变化；累积归一前后梯度量级；warmup 对早期稳定性的影响。

#### 5. 图文说明

| 边界 | 量 | 风险 |
|---|---|---|
| 大 batch | micro×accum×world | lr 需缩放 |
| Adam 状态 | ∝P | 常驻显存 |
| 极端 lr | - | 发散/停滞 |
| 步数尾部 | scheduler | 收敛慢 |

图示解释：规模-lr-收敛三角，边界处分别触发散、OOM、收益递减。

#### 6. 拓展知识

延伸：线性缩放与平方根缩放、LAMB、warmup、梯度裁剪、OneCycle、累积与 BN 冲突。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 累积如何调 lr？——线性缩放或按累积步归一。
2. 大 batch 为何发散？——lr 未同步放大/缺 warmup。
3. Adam 状态多大？——∝参数量。

#### 8. 易错点与反例

1. 累积后不归一，等效放大 lr。
2. 大 batch 直接沿用小 batch lr，收敛差。
3. lr 过大无裁剪，loss 爆成 nan。

#### 9. 实践与验证

脚本：扫有效 batch 与 lr 观察稳定性；监控梯度范数；对比累积归一前后收敛。oracle 为“规模与 lr 协同、梯度受控”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B075 评分：不合格只调 batch；合格能说累积与 lr 关系；良好能量化有效 batch 与 Adam 成本；优秀能结合 warmup、裁剪、FSDP 给出规模扩展下的稳定性方案。
### B076. 边界条件：AMP/恢复

- 难度：入门级
- 知识域：K06 AMP/恢复
- 考察能力：资源预算、吞吐、尾延迟、容量阈值与单位成本
- 题型：性能/容量/成本
- 面试频率：高频
- 前置知识：autocast、scaler、快照、随机状态
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

AMP/恢复的边界主要是数值范围与状态一致性：fp16 范围 `±65504`、最小正规数约 6.1e-5，超界即 inf/下溢；scale 上限过大则频繁溢出跳步、过小则下溢；bf16 动态范围大但尾数少；断点恢复若状态不全会破坏训练动态。边界反例：loss 或激活超过 fp16 上界直接 inf，scaler 反复减半。

#### 2. 问题分析

按成本列出：fp16 数值域、scale 上下界、状态全集。推导：良好 scale 使梯度既不下溢也不溢出；溢出率过高→有效步数减少。边界：bf16 免 scaler 但精度低。

#### 3. 具体答案

面试中可以这样回答：
> AMP 的边界是数值域与状态一致性的双重约束。第一，fp16 只能表示到约 65504，超过就 inf；小梯度低于约 6.1e-5 会下溢为 0。GradScaler 把 loss 乘 S 提升梯度可表示范围，但 S 太大会频繁溢出（每溢出一次就跳步并减半），太小又回到下溢，所以 S 必须落在“既不溢出也不下溢”的窗口，靠自适应调优逼近。第二，溢出率是容量边界：如果某些层激活天然超 fp16 范围，会持续溢出、有效训练步数下降，这时应改 bf16（指数范围同 fp32）或对敏感层保留 fp32。第三，bbox bf16 不需要 scaler，但尾数只有 8 位，累积误差大，归约/loss 仍要 fp32。第四，断点恢复的边界是“状态全集”：model、optimizer、scaler、epoch/step、RNG 缺一不可，否则续训动态与不中断不一致。验证方式：监控 scale 与溢出率、对比 bf16/fp16 收敛、完整/不完整恢复对拍。

#### 4. 技术洞察

不变量：fp16 范围 `[6.1e-5, 65504]`；scale 调优在三者间平衡（下溢/不溢/溢出频率）；恢复=状态全集。可验证：`get_scale()` 轨迹、溢出次数、续训对拍。

#### 5. 图文说明

| 边界 | 阈值/量 | 后果 |
|---|---|---|
| fp16 范围 | 65504 / 6.1e-5 | inf / 下溢 |
| scale 窗口 | 自适应 | 溢出跳步/无效 |
| bf16 | 尾数 8 位 | 累积误差 |
| 恢复状态 | 全集 | 动态不一致 |

图示解释：scale 数值轴上标出下溢区、窗口、溢出去，调优目标是落在窗口。

#### 6. 拓展知识

延伸：bf16 vs fp16 选择、per-layer 精度、`torch.amp` 的白黑名单定制、scaler 状态分片。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. fp16 上界？——65504。
2. scale 太大太小后果？——频繁溢出/下溢。
3. bf16 需 scaler 吗？——不需要。

#### 8. 易错点与反例

1. 激活超 fp16 上界，持续 inf。
2. scale 过小，梯度仍下溢。
3. 恢复缺状态，曲线偏移。

#### 9. 实践与验证

脚本：监控 `get_scale` 与溢出；对比 bf16/fp16 收敛；完整恢复对拍。oracle 为“scale 落在窗口、恢复一致”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B076 评分：不合格只开 autocast；合格能说 fp16 范围；良好能解释 scale 窗口与溢出率；优秀能结合 bf16、per-layer 精度、状态全集给出数值边界治理方案。
### B077. 边界条件：分布式

- 难度：入门级
- 知识域：K07 分布式
- 考察能力：资源预算、吞吐、尾延迟、容量阈值与单位成本
- 题型：性能/容量/成本
- 面试频率：高频
- 前置知识：rank、DDP、FSDP、collective、容错
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

分布式的边界主要是通信-计算比与扩展效率：通信量 ∝ 参数/梯度字节，随卡数增加 AllReduce 占比上升；跨节点带宽远低于 NVLink，跨节点扩展效率骤降；显存上 DDP 每卡全量、FSDP 分片；batch 需按卡数缩放。边界反例：小模型多卡通信无法被计算掩盖，扩展效率远低于线性。

#### 2. 问题分析

按成本列出：通信量 `2(N-1)/N·B`、带宽层次（NVLink vs InfiniBand）、显存倍数、有效 batch。推导扩展效率 = 单卡时间/(N×单卡时间) 的下界受通信限制。边界：计算/通信比小时扩展无收益。

#### 3. 具体答案

面试中可以这样回答：
> 分布式的边界是“通信-计算比决定扩展效率”。第一，每步通信量约参数梯度字节 `B=P·w`，Ring AllReduce 因子 `2(N-1)/N`，随卡数 N 增长但饱和；当通信时间超过计算时间、又无法重叠时，加卡不再提速甚至变慢。第二，带宽分层：同节点 NVLink 远快于跨节点 InfiniBand/以太，跨节点扩展效率掉得最厉害，所以要尽量把通信限制在节点内或减少跨节点 AllReduce。第三，显存边界：DDP 每卡持有完整模型、梯度、优化器状态，参数量超过单卡显存就装不下，必须用 FSDP/ZeRO 分片。第四，有效 batch 随卡数放大，通常要同步放大 lr 并 warmup。第五，小模型多卡时计算太少、通信占比高，扩展效率远低于线性——这种场景单卡或更少卡反而划算。验证方式：扫 1/2/4/8 卡测扩展效率曲线，用 NCCL 日志看通信占比，对比 DDP/FSDP 显存。

#### 4. 技术洞察

不变量：通信量 ∝ P，Ring 因子饱和；扩展效率受 `T_comm/T_comp` 限制；显存 DDP 全量、FSDP 分片。可验证：扩展效率曲线；NCCL 通信占比；显存占用对账。

#### 5. 图文说明

| 边界 | 量 | 后果 |
|---|---|---|
| 通信量 | 2(N-1)/N·P·w | 扩展饱和 |
| 带宽层次 | NVLink vs IB | 跨节点掉速 |
| DDP 显存 | 全量每卡 | 大模型装不下 |
| 小模型多卡 | 通信占比高 | 负扩展 |

图示解释：扩展效率随 N 上升后饱和，跨节点曲线更低。

#### 6. 拓展知识

延伸：FSDP/ZeRO 分片、梯度累积减少同步频率、通信与计算重叠、拓扑感知、张量/流水线并行。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 扩展为何饱和？——通信量随 N 增长。
2. 何时多卡不划算？——计算/通信比太低。
3. DDP 显存瓶颈？——每卡全量，需 FSDP。

#### 8. 易错点与反例

1. 小模型堆很多卡，越堆越慢。
2. 忽略跨节点带宽，效率远低预期。
3. 以为 DDP 能装下任意大模型。

#### 9. 实践与验证

脚本：扫卡数测扩展效率曲线；NCCL 日志看通信占比；DDP/FSDP 显存对比。oracle 为“扩展饱和、跨节点更低、分片省显存”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B077 评分：不合格只会加卡；合格能说通信量；良好能量化扩展效率与带宽层次；优秀能结合 FSDP、重叠、拓扑与并行策略给出扩展性评估方案。
### B078. 边界条件：编译栈

- 难度：入门级
- 知识域：K08 编译栈
- 考察能力：资源预算、吞吐、尾延迟、容量阈值与单位成本
- 题型：性能/容量/成本
- 面试频率：高频
- 前置知识：Dynamo、AOTAutograd、Inductor、guard、缓存
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

编译栈的边界主要是编译开销与动态性：固定 shape 优化空间最大，动态 shape 支持范围有限且重编译成本高；图规模决定编译时间与内存；graph break 使部分退回 eager；CUDA Graph 要求静态地址与内存池；编译缓存可复用但 guard 严格。边界反例：输入 shape 种类极多时，每个新 shape 都重编译，总时间超过 eager。

#### 2. 问题分析

按成本列出：编译时间 ∝ 图规模；重编译次数 ∝ 不同 guard 数量；CUDA Graph 内存池约束；缓存命中率。推导：总时间 = 首次编译 + 命中执行 + 重编译惩罚。边界：shape 空间大→缓存抖动。

#### 3. 具体答案

面试中可以这样回答：
> 编译栈的边界围绕“编译成本 vs 动态性”。第一，编译时间随图规模和算子数增长，首次调用要付出捕获、微分、代码生成的开销；如果任务短暂、调用次数少，这个成本摊不掉，编译反而更慢，所以要先 warmup 看稳态。第二，动态性的边界：固定 shape 时 Inductor 能做最激进的融合与常量折叠；动态 shape 要靠符号化，优化受限，且 guard 集合随形状种类膨胀，shape 空间一大就频繁重编译、缓存抖动。第三，图规模的边界：Python 里细碎算子、循环展开会放大图，编译时间和内存都涨；graph break 会把图切成多段，每段单独编译还可能回退 eager。第四，CUDA Graph 的边界：`mode='reduce-overhead'` 要求静态地址与复用内存池，动态 shape 或频繁新分配会冲突。验证方式：用 recompiles 日志统计重编译次数、用 explain 数断点、对比不同 shape 空间下的总耗时。复盘：编译适合“稳定签名 + 长生命周期”的热点，不适合输入抖动或一次性任务。

#### 4. 技术洞察

不变量：总时间 = 编译 + 命中执行 + 重编译惩罚；缓存命中率随 guard 多样性下降。可验证：recompiles 日志、explain 断点数、不同 shape 空间计时。

#### 5. 图文说明

| 边界 | 量 | 后果 |
|---|---|---|
| 图规模 | 节点数 | 编译时间/内存 |
| 动态 shape | 符号域 | 优化受限/重编 |
| graph break | 断点数 | 回退 eager |
| CUDA Graph | 静态地址 | 冲突报错 |

图示解释：编译收益随“调用次数×图规模”上升，短小任务落在收益为负区。

#### 6. 拓展知识

延伸：`dynamic_shapes` 范围、AOTInductor 落盘缓存、CUDA Graph 内存池、编译与 AMP 的 dtype guard。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 何时编译不划算？——短任务/输入抖动。
2. 动态 shape 代价？——优化受限、重编。
3. graph break 后果？——回退 eager。

#### 8. 易错点与反例

1. 一次性脚本开 compile，只见变慢。
2. shape 空间大，重编抖动。
3. CUDA Graph 用于动态 shape，冲突。

#### 9. 实践与验证

脚本：统计 recompiles/断点、不同 shape 空间总耗时、CUDA Graph 冲突复现。oracle 为“边界条件可量化、收益区可判定”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B078 评分：不合格盲用 compile；合格能说编译成本；良好能解释动态性与缓存边界；优秀能给出“稳定签名+长生命周期才编译”的量化判据并处理 CUDA Graph 约束。
### B079. 边界条件：性能

- 难度：入门级
- 知识域：K09 性能
- 考察能力：资源预算、吞吐、尾延迟、容量阈值与单位成本
- 题型：性能/容量/成本
- 面试频率：高频
- 前置知识：关键路径、带宽、算力、通信、显存、统计
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

性能的边界主要是 Roofline 两侧与延迟下限：计算受限算子上界为峰值算力、带宽受限上界为带宽；小算子受启动延迟主导；多卡受通信限制；显存带宽对注意力等算子关键。边界反例：算子太小，启动开销比计算还大，加并行无用；显存不足触发重算或换出。

#### 2. 问题分析

按成本列出：计算上界、带宽上界、启动延迟、通信上界。推导 `T ≥ max(FLOPs/FLOPS, Bytes/BW)`，小算子 `T ≈ 启动延迟 + 数据量/BW`。边界：算术强度决定优化方向；延迟下限不可逾越。

#### 3. 具体答案

面试中可以这样回答：
> 性能的边界由 Roofline 和延迟下限共同决定。第一，计算受限算子的上界是峰值算力，带宽受限算子的上界是峰值带宽，任何实现都越不过 `max(FLOPs/FLOPS_peak, Bytes/BW)`；判断属于哪类看算术强度 `AI` 与临界点 `AI*=FLOPS/BW` 的关系。第二，小算子的边界是启动延迟：当 `numel` 很小，kernel launch 开销（微秒级）和同步开销比实际计算还大，此时增大并行、换算法都没用，要靠增大 batch、算子融合或 CUDA Graph 减少启动次数。第三，多卡算子的边界是通信：AllReduce 时间随卡数增长，当它超过计算时间又无法重叠时整体变慢。第四，显存带宽是软边界：注意力、LayerNorm 这类算子受带宽限制，优化方向是减少访存（融合、低精度、分块），而不是追求算力峰值。验证方式：从 Profiler 取 achieved FLOPS/BW 与理论上界对比，观察小算子的 launch 占比。

#### 4. 技术洞察

不变量：`T ≥ max(计算上界, 带宽上界)`；小算子 `T ≈ launch + data/BW`；通信上界 ∝ 卡数。可验证：achieved 指标接近对应峰值即接近该墙；小算子 launch 时间占比高。

#### 5. 图文说明

| 边界 | 上界 | 应对 |
|---|---|---|
| 计算受限 | 峰值 FLOPS | tensor core/融合 |
| 带宽受限 | 峰值 BW | 减访存/低精度 |
| 小算子 | 启动延迟 | 增大 batch/CUDA Graph |
| 多卡 | 通信 | 重叠/减少同步 |

图示解释：Roofline 折线是硬墙，延迟下限是小算子区域的水平线。

#### 6. 拓展知识

延伸：occupancy、tensor core、共享内存复用、FlashAttention、CUDA Graph。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 如何判断碰到哪面墙？——achieved vs 峰值。
2. 小算子为何优化无效？——启动延迟主导。
3. 注意力优化方向？——减访存。

#### 8. 易错点与反例

1. 对启延迟主导的小算子堆并行。
2. 对带宽受限算子追算力峰值。
3. 忽略多卡通信边界。

#### 9. 实践与验证

脚本：Profiler 取 achieved FLOPS/BW 对照 Roofline；测小算子 launch 占比；对比融合/增大 batch 的收益。oracle 为“achieved 贴近对应墙、小算子受 launch 限制”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B079 评分：不合格只调参；合格能说 Roofline；良好能识别启动延迟与通信边界；优秀能结合 achieved 指标、融合、分块与 CUDA Graph 给出碰墙判定与优化路径。
### B080. 边界条件：导出部署

- 难度：入门级
- 知识域：K10 导出部署
- 考察能力：资源预算、吞吐、尾延迟、容量阈值与单位成本
- 题型：性能/容量/成本
- 面试频率：高频
- 前置知识：state_dict、Export、runtime、schema、迁移
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

导出部署的边界主要是算子覆盖、动态范围与数值一致：opset 决定可用算子，动态轴取值域有限，量化有精度损失；推理引擎的优化能力随图结构变化；导出图的数值与 eager 在容差内一致。边界反例：自定义算子无 ONNX 映射导出失败；极端输入（超长序列、极小 batch）暴露 reshape/广播错误。

#### 2. 问题分析

按成本列出：算子覆盖度、动态轴范围、量化精度损失、图规模。推导：可导出的充要条件是可追踪 + 算子有映射；量化误差随激活范围估计。边界：动态轴边界值与未支持算子。

#### 3. 具体答案

面试中可以这样回答：
> 导出的边界有三类。第一，算子覆盖：图里每个算子必须有目标 opset / 引擎的映射，自定义算子、某些动态控制流没有映射就导出失败，需要改写或提供自定义插件；opset 越高算子越多但旧引擎可能不支持，这是双向边界。第二，动态范围：声明的动态轴只覆盖一个取值域，超出域（比训练更长的序列、更大的 batch）可能在 reshape/广播处报错，所以要对域边界（最小/最大 batch、最大序列）多测。第三，数值一致：导出图与 eager 只在容差内一致，量化会引入额外误差，必须用真实校准数据和多输入对拍精度，误差超阈值就要调整量化方案。此外图规模影响序列化大小和后端优化时间，超大图可能超过引擎编译能力。验证方式：checker + 多 shape 边界 allclose + 量化精度评估 + 覆盖不支持算子的失败用例。

#### 4. 技术洞察

不变量：可导出 ⟺ 可追踪且算子有映射；数值等价只在容差内、量化误差有界。可验证：checker、边界 shape allclose、量化误差指标。

#### 5. 图文说明

| 边界 | 限制 | 后果 |
|---|---|---|
| 算子覆盖 | opset/引擎 | 导出失败 |
| 动态域 | 声明范围 | 域外报错 |
| 量化 | 精度损失 | 误差超阈 |
| 图规模 | 引擎能力 | 编译失败 |

图示解释：输入域矩形+算子集合，两者之外的输入/算子都未定义。

#### 6. 拓展知识

延伸：ONNX 自定义算子、TensorRT/OpenVINO 支持矩阵、量化校准、动态 shape profile、精度-延迟权衡。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 何时导出失败？——算子无映射/不可追踪。
2. 动态轴要测什么？——域边界。
3. 量化如何保精度？——校准+对拍。

#### 8. 易错点与反例

1. 自定义算子无 ONNX 映射，导出中断。
2. 只测中间 batch，边界批量失败。
3. 量化不校准，精度崩。

#### 9. 实践与验证

脚本：checker+边界 shape allclose；量化前后误差对比；构造不支持算子验证失败路径。oracle 为“覆盖/域/精度/规模四类边界被明确并测试”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B080 评分：不合格只导出不测边界；合格能说算子与动态轴限制；良好能覆盖域边界与量化精度；优秀能结合插件、支持矩阵、校准给出部署边界的系统评估。
### B081. 边界条件：自定义算子

- 难度：入门级
- 知识域：K11 自定义算子
- 考察能力：接口、状态机、隔离、灰度、回滚与治理
- 题型：系统设计/工程落地
- 面试频率：高频
- 前置知识：schema、dispatcher、stream、autograd、ABI
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

自定义算子的工程边界：何时自建 vs 复用现有算子、支持哪些 device/dtype、是否需要跨版本 ABI 稳定、如何与 `torch.compile`/导出协同、维护成本与测试覆盖。系统设计上要在“性能收益、开发/维护成本、可移植性”间取舍。边界反例：为一次性热点写算子，后期无人维护且新版本 ABI 断裂。

#### 2. 问题分析

按系统设计维度：复用 vs 自建、设备/dtype 覆盖、ABI/版本、可编译/可导出、维护成本。推导决策：收益 = 性能提升 × 使用频率 − 开发维护成本 − 可移植性损失。边界：只在 profile 证明且长期热点时自建。

#### 3. 具体答案

面试中可以这样回答：
> 自定义算子的工程边界要从系统设计角度权衡。第一，先判断“要不要写”：只有在 Profiler 证明某段是长期热点、且现有算子/融合无法表达时才自建；为一次性任务写算子，收益摊不掉开发维护成本，还会成为技术债。第二，覆盖边界：明确支持哪些 device（CPU/CUDA）、哪些 dtype，未覆盖的组合要么 fallback 要么明确报错；C++/CUDA 扩展的 ABI 可能随 PyTorch/CUDA 版本变化，跨版本升级需要重新编译和测试。第三，协同边界：算子如果要在 `torch.compile` 下被捕获，最好用 `torch.library.custom_op` 注册而非裸 Python 函数；要导出 ONNX 则需提供算子映射或插件，否则导出路径断裂。第四，落地边界：写 gradcheck/opcheck/单测，纳入 CI，否则后续重构无人敢动。决策公式大致是“性能提升×调用频率”要显著大于“开发+维护+可移植性损失”。验证方式是微基准 + 端到端 profile + 降级路径测试。

#### 4. 技术洞察

不变量：收益必须由 profile 证明；覆盖域与 ABI 决定可维护性。可验证：微基准对比、opcheck/gradcheck、compile 捕获率、跨版本编译测试。

#### 5. 图文说明

| 维度 | 复用现有 | 自建算子 |
|---|---|---|
| 成本 | 低 | 开发+维护 |
| 收益 | 受限 | 潜力高 |
| 可移植 | 好 | ABI/版本风险 |
| 可编译/导出 | 通常支持 | 需注册/插件 |

图示解释：决策漏斗，先用 profile 过滤、再用收益-成本比决策。

#### 6. 拓展知识

延伸：`torch.library` 的 ABI 稳定、CUDA 扩展构建矩阵、算子降级 fallback、算子版本管理。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 何时自建？——profile 证明长期热点且无法表达。
2. 覆盖风险？——未支持 device/dtype 与 ABI 变化。
3. 如何降级？——提供 fallback 与显式报错。

#### 8. 易错点与反例

1. 为一次性任务写算子，沦为债务。
2. 未覆盖目标 dtype/device，上线才崩。
3. 裸 Python 函数导致 compile 断图、导出失败。

#### 9. 实践与验证

脚本：微基准证明收益；opcheck/gradcheck；compile/export 路径测试；构造未支持 dtype 验证降级。oracle 为“收益为正、覆盖明确、可编译可导出”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B081 评分：不合格盲目造轮子；合格能说 profile 驱动；良好能权衡成本/覆盖/可移植；优秀能设计含降级、CI、ABI 与导出协同的可维护算子工程方案。
### B082. 边界条件：测试治理

- 难度：入门级
- 知识域：K12 测试治理
- 考察能力：接口、状态机、隔离、灰度、回滚与治理
- 题型：系统设计/工程落地
- 面试频率：高频
- 前置知识：oracle、证据、日志、回滚、门禁、复盘
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

测试治理的工程边界：覆盖率与成本权衡、哪些不必测、CI 时间预算、flaky 在规模下的治理、golden 版本管理、性能门禁的成本。系统设计上测试是“质量止损”而非全覆盖。边界反例：追求 100% 覆盖导致 CI 数小时，或只测正常路径 放过事故。

#### 2. 问题分析

按工程维度：覆盖 vs 成本、优先级、CI 预算、flaky 治理、门禁。推导：测试价值 = 失败概率 × 失败代价 × 检出率 − 维护成本。边界：低频/低成本路径可低覆盖，高频/高风险路径必须高覆盖。

#### 3. 具体答案

面试中可以这样回答：
> 测试治理的工程边界是“用有限预算买最大止损”。第一，覆盖率不是越高越好：按“失败概率×失败代价”排优先级，训练主链路、数据管道、分布式、导出这些高频高风险路径必须厚覆盖，而一次性脚本、纯展示代码可以少测。第二，CI 有时间预算：快用例放 PR，慢用例（GPU 集成、性能、大模型）放夜间或按需触发，用 pytest 标记分层，避免 PR 卡几小时。第三，flaky 在规模下会成为主要噪音：必须建隔离池、记录根因、给出 owner，而不是重试掩盖。第四，golden/回归基线要有版本和更新流程，否则容差被无限放宽。第五，性能门禁也要成本核算：全量 benchmark 很贵，通常只对关键算子/模型设阈值。验证方式：统计测试时间分布、flaky 率、事故是否被测试拦截。复盘：测试是投资，要按风险和成本配置，而不是无差别全覆盖。

#### 4. 技术洞察

不变量：测试价值 ∝ 失败概率×代价×检出率；CI 预算是硬约束。可验证：测试时间分布、flaky 率、逃逸缺陷统计。

#### 5. 图文说明

| 维度 | 低成本路径 | 高风险路径 |
|---|---|---|
| 覆盖 | 低 | 高 |
| CI 时机 | 夜间 | PR |
| 门禁 | 弱 | 强 |
| flaky | 隔离 | 归因+owner |

图示解释：风险-成本矩阵，资源向高失败概率×高代价象限倾斜。

#### 6. 拓展知识

延伸：覆盖率门禁、CI 分片并行、测试选择（只跑受影响用例）、golden 管理、逃逸缺陷分析。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 覆盖越高越好吗？——否，按风险配置。
2. 慢测试放哪？——夜间/按需。
3. flaky 如何治理？——隔离+归因+owner。

#### 8. 易错点与反例

1. 追求 100% 覆盖拖垮 CI。
2. 只测正常路径 放过事故。
3. 靠重试掩盖 flaky。

#### 9. 实践与验证

脚本：统计测试时间与 flaky 率；用标记分层；对比按需触发后的 PR 时长。oracle 为“测试按风险配置、CI 在预算内、flaky 有 owner”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B082 评分：不合格只追覆盖；合格能说风险优先；良好能设计分层 CI 与 flaky 治理；优秀能基于逃逸缺陷与事故数据持续校准测试投资与门禁成本。
### B083. 对比辨析：张量/设备

- 难度：入门级
- 知识域：K01 张量/设备
- 考察能力：接口、状态机、隔离、灰度、回滚与治理
- 题型：系统设计/工程落地
- 面试频率：高频
- 前置知识：shape、stride、dtype、device
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

张量/设备对比辨析：连续 vs `channels_last` 内存格式（卷积友好）、视图 vs 拷贝（零拷贝 vs 分配）、CPU vs GPU 张量（搬运与并行度）、不同 dtype（精度/带宽/范围）、`expand` vs `repeat`。取舍取决于算子类型与性能目标。

#### 2. 问题分析

按对比维度：内存格式、拷贝语义、设备、dtype、广播实现。给出选择依据：卷积用 channels_last、避免不必要拷贝用视图、低精度换带宽、expand 省内存。边界：channels_last 对逐元素也可能有利有弊。

#### 3. 具体答案

面试中可以这样回答：
> 张量/设备的对比主要是几组取舍。第一，内存格式 `contiguous` vs `channels_last`：卷积在 NHWC（channels_last）下更利于 tensor core，逐元素/全连接通常 contiguous 更好，选错会触发隐式转换。第二，视图 vs 拷贝：`transpose`/`slice`/`expand` 是零拷贝视图（`data_ptr` 不变），`contiguous`/`clone`/`repeat` 会分配；频繁对非连续张量做需要连续的操作会隐式拷贝，拖慢并占显存。第三，CPU vs GPU 张量：GPU 并行度高但有小 kernel 启动与 H2D 成本，小张量放 CPU 可能更快，大张量放 GPU。第四，dtype：fp16/bf16 省一半带宽与显存、fp32 精度高、int8 更省但范围小，训练常用 bf16/AMP。第五，`expand`（stride=0 零拷贝）vs `repeat`（复制），能用 expand 就别 repeat。验证方式：对比不同格式/精度的耗时与显存，检查 `data_ptr`/`is_contiguous`。

#### 4. 技术洞察

不变量：视图保 `data_ptr`、`repeat` 分配；channels_last 改变 stride 而非数据。可验证：`is_contiguous(memory_format=channels_last)`；耗时/显存随格式与 dtype 变化。

#### 5. 图文说明

| 对比 | 选项 A | 选项 B | 选择依据 |
|---|---|---|---|
| 格式 | contiguous | channels_last | 算子类型 |
| 拷贝 | 视图 | 拷贝 | 是否需要独立存储 |
| 设备 | CPU | GPU | 规模 vs 启动成本 |
| 精度 | fp32 | fp16/bf16 | 精度 vs 带宽 |

图示解释：二维决策表，行是场景、列是选项，标注推荐。

#### 6. 拓展知识

延伸：`to(memory_format=...)`、TF32、`bfloat16` vs `float16`、量化、`as_strided`。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 卷积选哪种格式？——channels_last。
2. 视图和拷贝怎么选？——需独立存储才拷贝。
3. 小张量放哪？——可能 CPU 更快。

#### 8. 易错点与反例

1. 对非连续张量反复操作，隐式拷贝。
2. 卷积用 contiguous，损失 tensor core 收益。
3. 无脑 repeat，显存翻倍。

#### 9. 实践与验证

脚本：对比 contiguous/channels_last 卷积耗时；对比视图/拷贝的 `data_ptr` 与耗时；对比 expand/repeat 显存。oracle 为“不同选择的性能/内存差异可测”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B083 评分：不合格不区分；合格能说基本差异；良好能按场景选择并验证；优秀能结合 tensor core、隐式拷贝、精度与带宽给出量化取舍方案。
### B084. 对比辨析：autograd

- 难度：入门级
- 知识域：K02 autograd
- 考察能力：接口、状态机、隔离、灰度、回滚与治理
- 题型：系统设计/工程落地
- 面试频率：高频
- 前置知识：requires_grad、leaf、view、detach、反向
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

autograd 对比辨析：`no_grad` vs `inference_mode`（是否保留版本/view 追踪，产出是否可回图）、`detach()` vs `.data`（安全 vs 绕过版本检查）、`requires_grad=False` vs `detach`、`torch.autograd.grad` vs `.backward`。取舍取决于“是否复用张量、是否需要中间梯度、是否长期推理”。

#### 2. 问题分析

按对比维度：建图、产出可回图性、安全性、返回内容。给出选择：纯推理用 inference_mode，需复用张量用 no_grad，切数据用 detach，取指定梯度用 autograd.grad。

#### 3. 具体答案

面试中可以这样回答：
> autograd 的对比主要三组。第一，`no_grad` vs `inference_mode`：都关建图，但 inference_mode 还关掉版本计数和 view 追踪，更快更省，代价是产出的张量离开上下文后不能再安全地参与需要梯度的运算；所以纯推理/部署选 inference_mode，需要在同一流程里复用张量做可微计算则选 no_grad。第二，`detach()` vs `.data`：两者都返回共享数据、脱离图的张量，但 `.data` 绕过版本检查和 autograd 机制，原地改它可能静默破坏梯度；`detach()` 是官方安全写法，要改值应配合 `no_grad`。第三，`requires_grad=False` vs `detach`：前者在创建时就决定不参与，后者是对已有张量的图切断；冻结参数常设 `requires_grad=False`，临时切断用 detach。第四，`torch.autograd.grad` 直接返回指定输入的梯度，`.backward` 把梯度写进叶子 `.grad`。验证方式：检查 `grad_fn`/`requires_grad`、比较 `data_ptr`、验证 inference 张量回图报错。

#### 4. 技术洞察

不变量：inference_mode 张量不可回图；`detach` 共享 `data_ptr` 且 `requires_grad=False`；`.data` 不安全。可验证：`detach().data_ptr()==x.data_ptr()`；inference 张量参与 backward 报错。

#### 5. 图文说明

| 对比 | A | B | 选择 |
|---|---|---|---|
| no_grad vs inference_mode | 保留追踪 | 关闭追踪 | 复用选前者 |
| detach vs .data | 安全 | 不安全 | 用 detach |
| requires_grad vs detach | 创建时 | 切断已有 | 冻结用前者 |

图示解释：两组开关，按“是否复用/是否安全/是否长期”选址。

#### 6. 拓展知识

延伸：`torch.set_grad_enabled`、`@torch.no_grad()` 装饰器、`autograd.grad` 的 `retain_graph`/`allow_unused`、二阶导。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 推理用哪个？——inference_mode。
2. detach 和 .data？——前者安全。
3. 如何取中间梯度？——autograd.grad。

#### 8. 易错点与反例

1. 用 `.data` 改张量，静默破坏梯度。
2. inference_mode 张量拿去算 loss 回传，报错。
3. 需要复用张量却用了 inference_mode。

#### 9. 实践与验证

脚本：对比 no_grad/inference_mode 产出张量的回图行为；比较 detach/.data 的安全性；用 autograd.grad 取中间梯度。oracle 为“各对比的选择依据有行为证据”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B084 评分：不合格混用；合格能说两组差异；良好能按复用/安全场景选择并验证；优秀能结合版本计数、二阶导、部署给出 autograd 开关的工程选择准则。
### B085. 对比辨析：Module/状态

- 难度：入门级
- 知识域：K03 Module/状态
- 考察能力：接口、状态机、隔离、灰度、回滚与治理
- 题型：系统设计/工程落地
- 面试频率：高频
- 前置知识：参数、buffer、模式和 state_dict
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

Module/状态对比辨析：Parameter vs buffer vs 普通 Tensor 属性（是否更新/进 state_dict/被 to 移动）、train vs eval（训练态行为）、保存 `state_dict` vs 保存整个模型（可移植 vs 方便）、冻结 vs LoRA。取舍取决于“是否需要更新、是否随状态持久化、是否跨设备”。

#### 2. 问题分析

按对比维度：注册归属、是否更新、state_dict、设备移动、可移植。给出选择：可训练用 Parameter，统计量用 buffer，临时用普通属性；保存优先 state_dict。

#### 3. 具体答案

面试中可以这样回答：
> Module 状态的对比可以列成一张表。第一，`nn.Parameter` vs buffer vs 普通 Tensor：Parameter 进 `parameters()`、被优化器更新、进 state_dict、随 `.to` 移动；buffer 进 state_dict、随 `.to` 移动但不更新（如 BN running stats）；普通 Tensor 属性什么都不做，是“隐形状态”，训练/保存/搬运全都不生效。第二，`train()` vs `eval()`：区别在 Dropout、BatchNorm 等训练态算子行为，推理必须 eval。第三，保存 `state_dict` vs 保存整个 `model`：前者只存张量、与类定义解耦、可移植、推荐；后者 pickle 整个对象、依赖类路径，跨环境易失败。第四，冻结 vs LoRA：冻结只是不更新部分参数（仍需移出优化器才省状态），LoRA 是用低秩增量替代更新，可训练参数量大幅下降，适合小显存微调。验证方式：`named_parameters()`/`named_buffers()`、state_dict 键集合、train/eval 输出对比。

#### 4. 技术洞察

不变量：注册对象才被统一管理；`state_dict` 含 Parameter+持久 buffer；eval 冻结行为。可验证：键集合、train/eval 输出、可训练参数量。

#### 5. 图文说明

| 对比 | A | B | 选择 |
|---|---|---|---|
| Parameter/Buffer/普通 | 更新/持久/无 | - | 按用途注册 |
| train/eval | 训练态 | 推理态 | 推理必 eval |
| state_dict/model | 可移植 | 整体 | 优先前者 |
| 冻结/LoRA | 不更新部分 | 低秩增量 | 省显存用 LoRA |

图示解释：对象注册树 + 生命周期对比表。

#### 6. 拓展知识

延伸：`register_buffer(persistent=False)`、EMA buffer、`torch.save` 的 map_location、PEFT 库。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 三者区别？——更新/持久/设备。
2. 为何存 state_dict？——可移植。
3. LoRA 优势？——可训练参数骤减。

#### 8. 易错点与反例

1. 普通属性当权重，永不更新。
2. 存整个模型，跨版本加载失败。
3. 冻结但仍加入优化器，没省状态。

#### 9. 实践与验证

脚本：对比三类对象的注册与 state_dict；train/eval 输出；state_dict vs model 保存大小与加载。oracle 为“各对比差异有证据”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B085 评分：不合格混淆三类状态；合格能说注册与持久化；良好能对比 train/eval 与保存方式；优秀能结合 LoRA、EMA、可移植性给出状态管理工程准则。
### B086. 对比辨析：数据管道

- 难度：入门级
- 知识域：K04 数据管道
- 考察能力：接口、状态机、隔离、灰度、回滚与治理
- 题型：系统设计/工程落地
- 面试频率：高频
- 前置知识：Dataset、Sampler、collate、padding、版本
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

数据管道对比辨析：Map-style `Dataset` vs `IterableDataset`（随机索引 vs 流式）、`num_workers` 多进程 vs 单进程、`pin_memory` 开 vs 关、默认 collate vs 自定义 collate、固定长度 vs 分桶/动态 padding。取舍取决于数据规模、IO 形态、吞吐与内存预算。

#### 2. 问题分析

按对比维度：索引/流式、并行度、异步搬运、批次拼装、长度处理。给出选择：可随机访问用 Map-style，流式/超大数据集用 Iterable；吞吐瓶颈用多 worker；GPU 重叠用 pin+non_blocking；变长用自定义 collate 或分桶。

#### 3. 具体答案

面试中可以这样回答：
> 数据管道的对比有几组关键取舍。第一，Map-style `Dataset` vs `IterableDataset`：前者支持 `__getitem__`，天然可 shuffle、可随机采样，适合可随机访问的数据；后者按流式顺序产出，适合超大数据集、日志流或需要顺序读取的场景，但要多 worker 分片必须显式实现（`get_worker_info`）。第二，单进程 vs 多 worker：多 worker 并行取数隐藏 IO/解码，但增加内存、fd 和 IPC 成本，到拐点后收益递减。第三，pin_memory 开 vs 关：开则 H2D 可异步重叠，但占不可换出内存。第四，默认 collate vs 自定义：默认只处理等形状 stack，变长序列必须自定义 pad+mask。第五，固定长度 vs 分桶/动态 padding：固定 pad 到全局最长浪费大，分桶按长度聚类可显著提高有效 token 率。验证方式：吞吐计时、pad 比例统计、显存/内存监控。

#### 4. 技术洞察

不变量：Map-style 可随机访问、Iterable 流式；多 worker 分片互斥；pad 浪费由长度方差决定。可验证：迭代吞吐曲线、有效 token 率、内存占用。

#### 5. 图文说明

| 对比 | A | B | 选择 |
|---|---|---|---|
| Dataset | Map-style | Iterable | 随机访问 vs 流式 |
| 进程 | 单 | 多 worker | 吞吐 vs 资源 |
| 内存 | 普通 | pinned | 异步重叠 |
| 长度 | 固定 pad | 分桶 | 有效 token 率 |

图示解释：数据管道各环节的二选一决策表。

#### 6. 拓展知识

延伸：`IterableDataset` 分片、动态 batch by token、内存映射、渐进式解码。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. Map vs Iterable？——随机访问 vs 流式。
2. 何时多 worker？——IO 瓶颈且资源足够。
3. 变长如何高效？——分桶/动态 padding。

#### 8. 易错点与反例

1. 流式数据用 Map-style 强行 shuffle。
2. 多 worker 未分片，重复数据。
3. 全 pad 到最长，token 浪费。

#### 9. 实践与验证

脚本：对比 Map/Iterable 吞吐；扫 worker 数；统计 pad 比例与分桶收益。oracle 为“各对比选择有吞吐/资源证据”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B086 评分：不合格只会用默认 DataLoader；合格能说 Map/Iterable 与 worker；良好能对比 pin/collate/分桶并量化；优秀能按数据形态与预算给出端到端管道设计。
### B087. 对比辨析：训练/优化器

- 难度：入门级
- 知识域：K05 训练/优化器
- 考察能力：接口、状态机、隔离、灰度、回滚与治理
- 题型：系统设计/工程落地
- 面试频率：高频
- 前置知识：forward、backward、累积、optimizer、scheduler
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

训练/优化器对比辨析：SGD vs Momentum vs Adam vs AdamW（状态量、收敛、正则）、全量微调 vs LoRA、梯度累积 vs 加大 batch（显存/收敛/BN 行为）、固定 lr vs 调度器。取舍取决于显存、收敛速度、泛化与工程成本。

#### 2. 问题分析

按对比维度：状态内存、收敛速度、正则行为、显存、泛化。给出选择：显存紧用 SGD/累积，快速收敛用 AdamW，大模型微调用 LoRA，大 batch 配调度器。

#### 3. 具体答案

面试中可以这样回答：
> 训练/优化器的对比主要四组。第一，优化器：SGD 无状态、内存省但收敛慢、常配合 momentum；Adam 状态多（2 个动量）、收敛快、对 lr 不敏感但显存高；AdamW 把权重衰减解耦，正则行为比“Adam+L2”更正确，是 Transformer 训练默认。第二，全量微调 vs LoRA：全量更新所有参数，显存和 checkpoint 都大；LoRA 冻结原权重、只训练低秩增量，可训练参数量骤减，适合小显存，代价是容量受限、可能欠拟合复杂任务。第三，梯度累积 vs 直接加大 batch：累积用时间换显存模拟大 batch，但 BatchNorm 仍用 micro-batch 统计，且需要正确归一；真正大 batch 的 BN 统计不同，收敛行为也会变。第四，固定 lr vs 调度器：warmup+Cosine/OneCycle 通常更稳更快，但需要与总步数匹配。验证方式：对比收敛曲线、显存、checkpoint 大小与泛化指标。

#### 4. 技术洞察

不变量：优化器状态内存 SGD<Momentum<Adam；LoRA 可训练参数≈低秩增量；累积与真 batch 在 BN 上不等价。可验证：`state_dict` 状态量、可训练参数量、收敛曲线。

#### 5. 图文说明

| 对比 | A | B | 选择 |
|---|---|---|---|
| 优化器 | SGD | AdamW | 显存 vs 收敛 |
| 微调 | 全量 | LoRA | 容量 vs 显存 |
| 大 batch | 累积 | 真大 batch | BN 行为 |
| lr | 固定 | 调度器 | 稳定性 |

图示解释：收敛-显存权衡曲线，不同方案落在不同点。

#### 6. 拓展知识

延伸：LAMB、8-bit Adam、QLoRA、梯度裁剪、warmup、累积与 DDP 的 no_sync。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. AdamW 与 Adam？——解耦权重衰减。
2. LoRA 代价？——容量受限。
3. 累积与真 batch 差？——BN 统计。

#### 8. 易错点与反例

1. 小显存全量微调大模型，OOM。
2. 累积当作真大 batch，BN 行为不一致。
3. Adam+L2 当 AdamW，正则强度不符。

#### 9. 实践与验证

脚本：对比优化器状态量与收敛；全量/LoRA 显存与 checkpoint；累积 vs 真 batch 的 BN 统计差异。oracle 为“各对比有收敛/显存证据”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B087 评分：不合格只用一个优化器；合格能说状态量与收敛；良好能对比微调/累积/调度并量化；优秀能按显存-收敛-泛化给出训练方案取舍与验证计划。
### B088. 对比辨析：AMP/恢复

- 难度：入门级
- 知识域：K06 AMP/恢复
- 考察能力：接口、状态机、隔离、灰度、回滚与治理
- 题型：系统设计/工程落地
- 面试频率：高频
- 前置知识：autocast、scaler、快照、随机状态
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

AMP 对比辨析：fp16 vs bf16（指数/尾数、是否需 scaler）、autocast vs 手动 `.half()`（自动算子选择 vs 全局降精度）、GradScaler vs 无 scaler、AMP vs 全 fp32（速度/显存 vs 数值稳定）。取舍取决于硬件支持、数值敏感度与部署目标。

#### 2. 问题分析

按对比维度：动态范围、尾数精度、是否需要 scaler、算子选择、部署兼容。给出选择：优先 bf16（大范围免 scaler），fp16 需 scaler；用 autocast 而非全局 half；部署前确认引擎支持 dtype。

#### 3. 具体答案

面试中可以这样回答：
> AMP 的对比有四组。第一，fp16 vs bf16：fp16 有 5 位指数，范围窄（±65504），小梯度易下溢，需要 GradScaler；bf16 有 8 位指数、范围同 fp32，通常免 scaler，但尾数只有 8 位、精度低。硬件同时支持时优先 bf16 更省心，除非对精度极敏感或硬件只支持 fp16。第二，`autocast` vs 手动 `.half()`：autocast 只把白名单算子降到低精度，softmax/归约/loss 保持 fp32，兼顾速度与稳定；手动全局 `.half()` 会让数值敏感算子也降精度，易 NaN，所以几乎总用 autocast。第三，GradScaler vs 无 scaler：fp16 训练需要，bf16 通常不需要，套在 bf16 上徒增复杂度。第四，AMP vs 全 fp32：AMP 省显存和带宽、提速，但引入精度风险，需要验证收敛与最终指标；对数值极稳定的推理可直接低精度，训练建议保留 fp32 master weights。验证方式：对比 fp16/bf16/fp32 的收敛、显存、速度与 NaN 率。

#### 4. 技术洞察

不变量：fp16 需 scaler（范围窄）、bf16 一般不需；autocast 只降白名单算子；master weights 保 fp32。可验证：NaN/溢出率、收敛差异、显存与速度对比。

#### 5. 图文说明

| 对比 | A | B | 选择 |
|---|---|---|---|
| 格式 | fp16 | bf16 | 范围 vs 精度 |
| 转换 | autocast | 手动 half | 稳定 vs 激进 |
| scaler | 需要 | 通常不需 | 按格式 |
| 精度 | AMP | fp32 | 速度 vs 稳定 |

图示解释：范围-精度-稳定性三角，bf16 落在大范围、低尾数点。

#### 6. 拓展知识

延伸：per-layer dtype、TF32、fp8、`torch.amp` 白黑名单定制、部署端 dtype 支持。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. fp16/bf16 怎么选？——优先 bf16。
2. autocast/手动？——用 autocast。
3. 为何 bf16 免 scaler？——范围同 fp32。

#### 8. 易错点与反例

1. 全局 `.half()` 导致 NaN。
2. bf16 还套 GradScaler，多余。
3. 部署引擎不支持所选 dtype。

#### 9. 实践与验证

脚本：对比 fp16/bf16/fp32 收敛与 NaN 率、显存、速度；验证 autocast 下 loss 保持 fp32。oracle 为“各对比有数值/性能证据”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B088 评分：不合格只会开 autocast；合格能说 fp16/bf16 差异；良好能对比转换方式与 scaler 必要性；优秀能结合 per-layer/fp8/master weights 与部署 dtype 给出精度-性能权衡方案。
### B089. 对比辨析：分布式

- 难度：入门级
- 知识域：K07 分布式
- 考察能力：接口、状态机、隔离、灰度、回滚与治理
- 题型：系统设计/工程落地
- 面试频率：高频
- 前置知识：rank、DDP、FSDP、collective、容错
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

分布式对比辨析：数据并行（DDP）vs 全分片数据并行（FSDP/ZeRO）vs 张量并行（TP）vs 流水线并行（PP）；DDP 每卡全量、通信 AllReduce；FSDP 分片参数/梯度/优化器状态、通信 all-gather/reduce-scatter；TP 切分单层矩阵、通信频繁；PP 切分层、有气泡。取舍取决于模型大小、带宽、拓扑。

#### 2. 问题分析

按对比维度：切分对象、通信模式、显存节省、通信量、实现复杂度。给出选择：模型装得下用 DDP，装不下用 FSDP/ZeRO，单层过大用 TP，深层网络用 PP。

#### 3. 具体答案

面试中可以这样回答：
> 分布式并行策略按“切什么”来对比。第一，DDP（数据并行）：每卡保存完整模型副本，反向对梯度 AllReduce，实现简单、扩展性好，但显存不省——每卡都有全量参数、梯度、优化器状态，模型超过单卡显存就装不下。第二，FSDP/ZeRO（全分片数据并行）：把参数、梯度、优化器状态按卡分片，前向按需 all-gather 参数、反向 reduce-scatter 梯度，显存随卡数下降，代价是通信量增加、实现更复杂，是训练超大模型的主流。第三，张量并行 TP：把单个矩阵乘按行/列切到多卡，单层就能扩展，但每层都要通信、对带宽极敏感，通常限制在节点内 NVLink。第四，流水线并行 PP：按层切分成阶段，跨卡流水执行，通信少但有流水线气泡，需要 micro-batch 填充。实际大模型训练常是“TP 节点内 + PP/DP 节点间”的混合。验证方式：对比显存占用、通信量、扩展效率与实现复杂度。

#### 4. 技术洞察

不变量：DDP 显存不省、FSDP 显存 ∝ 1/N；TP 通信频繁、PP 有气泡。可验证：显存随卡数曲线、NCCL 通信占比、扩展效率。

#### 5. 图文说明

| 策略 | 切分对象 | 通信 | 显存节省 |
|---|---|---|---|
| DDP | 数据 | AllReduce 梯度 | 不省 |
| FSDP/ZeRO | 参数+梯度+状态 | gather/scatter | 随卡数 |
| TP | 单层矩阵 | 频繁 | 部分 |
| PP | 层 | 阶段间 | 部分 |

图示解释：四种策略的切分示意，标注通信模式与适用规模。

#### 6. 拓展知识

延伸：ZeRO 三阶段、序列并行、专家并行、TP+PP+DP 混合、通信重叠。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. DDP 省显存吗？——不省。
2. FSDP 何时用？——单卡装不下。
3. TP 局限？——通信密集，限节点内。

#### 8. 易错点与反例

1. DDP 训练超大模型，OOM。
2. TP 跨节点，带宽成瓶颈。
3. PP 不填充 micro-batch，气泡严重。

#### 9. 实践与验证

脚本：DDP/FSDP 显存对比；NCCL 通信占比；TP/PP 扩展效率。oracle 为“各策略显存/通信/效率差异可测”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B089 评分：不合格只会 DDP；合格能说 DDP/FSDP 区别；良好能对比四种并行并量化；优秀能设计混合并行方案并说明通信-显存-复杂度取舍。
### B090. 对比辨析：编译栈

- 难度：入门级
- 知识域：K08 编译栈
- 考察能力：接口、状态机、隔离、灰度、回滚与治理
- 题型：系统设计/工程落地
- 面试频率：高频
- 前置知识：Dynamo、AOTAutograd、Inductor、guard、缓存
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

编译栈对比辨析：eager vs `torch.compile`（即时执行 vs 图融合）、TorchScript vs `torch.export`（旧脚本 vs 新导出）、`torch.compile` vs CUDA Graph（JIT 编译融合 vs 捕获重放）、静态 shape vs 动态 shape。取舍取决于部署目标、动态性与稳定性。

#### 2. 问题分析

按对比维度：执行模式、优化手段、动态性、部署环境、开销。给出选择：训练用 compile、部署用 export，固定 shape 用 CUDA Graph，动态性强则 eager 或动态 compile。

#### 3. 具体答案

面试中可以这样回答：
> 编译栈的对比有几组。第一，eager vs `torch.compile`：eager 即时逐算子执行、调试友好但有 kernel 启动和中间张量开销；compile 把热点函数编译成融合 kernel、提速明显，但有编译延迟和重编译风险，适合稳态热点。第二，TorchScript vs `torch.export`：TorchScript 是老一代图/脚本方案，对动态支持和维护性较弱；`torch.export` 是新一代导出路径，配合 `dynamic_shapes` 更规范，推荐新项目用 export。第三，`torch.compile` vs CUDA Graph：compile 通过算子融合和代码生成减少 kernel 数与访存；CUDA Graph 通过捕获整段执行流并重放来消除启动开销，两者互补，`mode='reduce-overhead'` 会用 CUDA Graph，但要求静态地址与 shape。第四，静态 vs 动态 shape：静态优化空间最大、CUDA Graph 可用；动态支持范围有限、易重编译。验证方式：对比 kernel 数、稳态耗时、重编译次数、部署兼容性。

#### 4. 技术洞察

不变量：compile 融合减 kernel、CUDA Graph 消启动、export 保图；动态性换优化空间。可验证：kernel 数、耗时、重编译日志、导出校验。

#### 5. 图文说明

| 对比 | A | B | 选择 |
|---|---|---|---|
| 执行 | eager | compile | 开发 vs 稳态 |
| 导出 | TorchScript | export | 新项目用 export |
| 加速 | compile | CUDA Graph | 互补 |
| shape | 静态 | 动态 | 优化 vs 泛化 |

图示解释：四种技术沿“部署/训练、静态/动态”维度分布。

#### 6. 拓展知识

延伸：AOTInductor、`torch.export` 与 compile 协同、CUDA Graph 内存池、TorchScript 迁移。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. compile 与 CUDA Graph？——融合 vs 消启动。
2. TorchScript 还行吗？——新项目用 export。
3. 动态 shape 代价？——优化受限、重编。

#### 8. 易错点与反例

1. 部署仍用 TorchScript，功能受限。
2. CUDA Graph 用于动态 shape，冲突。
3. 短任务开 compile，得不偿失。

#### 9. 实践与验证

脚本：对比 eager/compile kernel 数与耗时；export 校验；CUDA Graph 冲突复现。oracle 为“各技术差异可观测”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B090 评分：不合格混用概念；合格能说 eager/compile 差异；良好能对比导出与 CUDA Graph 并验证；优秀能按训练/部署、静态/动态给出编译与导出技术的组合选型方案。
### B091. 对比辨析：性能

- 难度：入门级
- 知识域：K09 性能
- 考察能力：不变量、二阶影响、证据缺口与演进路线
- 题型：综合开放/专家追问
- 面试频率：高频
- 前置知识：关键路径、带宽、算力、通信、显存、统计
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

性能的对比辨析是“优化手段 vs 代价”的系统取舍：算力优化（tensor core/融合）vs 带宽优化（低精度/减少访存）vs 启动优化（CUDA Graph/增大 batch）vs 并行（多卡/流水线）。每种手段有适用墙与副作用，需先诊断受限类型再选手段。

#### 2. 问题分析

构建决策树：算算术强度→计算/带宽受限→选手段；再看算子大小是否受启动延迟主导；多卡看通信比。对比各手段的收益、代价与前提。边界：手段错配（带宽算子追算力峰值）无效。

#### 3. 具体答案

面试中可以这样回答：
> 性能优化的对比本质是“先诊断墙，再选手段”。诊断：用 Roofline 算算术强度，判断计算受限还是带宽受限；看算子大小是否被启动延迟主导；多卡看通信占比。然后选手段：计算受限 → 提高 tensor core 利用率、算子融合、减少冗余计算、用 TF32/bf16；带宽受限 → 减访存（融合、重用、分块）、低精度、channels_last；启动受限 → 增大 batch、合并 kernel、CUDA Graph；通信受限 → 重叠、增大计算/通信比、减少同步。每种手段都有代价：融合增加编译复杂度和寄存器压力；低精度有数值风险；CUDA Graph 要求静态；多卡有通信。工程上按“单变量、可验证、可回滚”的原则逐项尝试，先做收益大成本低的（如融合、dtype），再考虑高成本的（重写 kernel、换并行）。验证：Profiler 的 achieved 指标贴近对应墙、分段占比变化、p99 改善。专家级还要考虑残余风险：是否只在特定 shape 收益、是否影响数值精度、是否可回滚。

#### 4. 技术洞察

不变量：优化方向由受限类型决定；`T ≥ max(计算, 带宽)`；手段与墙必须匹配。可验证：achieved 指标、分段占比、单变量对照。

#### 5. 图文说明

| 墙 | 手段 | 代价 |
|---|---|---|
| 计算 | tensor core/融合 | 寄存器/编译 |
| 带宽 | 低精度/减访存 | 数值风险 |
| 启动 | CUDA Graph/大 batch | 静态约束 |
| 通信 | 重叠/减少同步 | 复杂度 |

图示解释：诊断→选手段→验证→回滚的决策闭环。

#### 6. 拓展知识

延伸：多手段叠加的交互、autotune、profile-guided 优化、性能回归门禁。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 先做什么？——先诊断墙。
2. 手段错配后果？——无效甚至更慢。
3. 如何控风险？——单变量+可回滚。

#### 8. 易错点与反例

1. 不诊断就乱试优化。
2. 带宽算子追算力峰值。
3. 多手段同时上，无法归因。

#### 9. 实践与验证

脚本：Roofline 分类 + 单手段对照 + Profiler 验证 + 回滚测试。oracle 为“手段与墙匹配、收益可归因、可回滚”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B091 评分：不合格乱调参；合格能诊断受限类型；良好能按墙选手段并量化；优秀能设计单变量、可回滚、含残余风险与性能门禁的优化体系。
### B092. 对比辨析：导出部署

- 难度：入门级
- 知识域：K10 导出部署
- 考察能力：不变量、二阶影响、证据缺口与演进路线
- 题型：综合开放/专家追问
- 面试频率：高频
- 前置知识：state_dict、Export、runtime、schema、迁移
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

导出部署的对比辨析：TorchScript vs `torch.export` vs ONNX（表达力/生态/调试性）、静态 vs 动态 shape（优化 vs 泛化）、浮点 vs 量化（精度 vs 延迟/显存）、云 vs 边缘引擎（能力/支持算子）。取舍取决于目标硬件、动态性与精度要求。

#### 2. 问题分析

按对比维度：表达力、目标引擎、动态性、精度、调试性。给出选择：新项目 export/ONNX，固定 shape 上 TensorRT，动态强用 ONNX Runtime，边缘设备用量化。

#### 3. 具体答案

面试中可以这样回答：
> 导出部署的对比围绕“表达力、目标、动态性、精度”。第一，格式：TorchScript 是老的图/脚本方案，生态和维护趋弱；`torch.export` 是新一代、对动态支持更规范、与 PyTorch 演进同步；ONNX 是跨框架的交换格式，生态最广但依赖 opset 映射，某些 PyTorch 语义（动态控制流、自定义算子）表达受限。第二，静态 vs 动态 shape：静态能做最激进的常量折叠与 kernel 特化、可用 CUDA Graph，但泛化差；动态声明后优化受限、要处理边界。第三，浮点 vs 量化：fp32/fp16 精度高、显存大；int8/int4 量化解锁低延迟低显存，但需要校准、有精度损失，必须用真实数据评估误差。第四，引擎：云端 GPU 可选 TensorRT 追求极致，通用场景用 ONNX Runtime 保证兼容，边缘设备用支持量化与算子裁剪的轻量引擎。验证：多 shape + 边界对拍、量化误差评估、目标引擎实测延迟。专家级还要权衡 re-export 成本、算子支持矩阵与长期维护。

#### 4. 技术洞察

不变量：格式表达力与引擎支持共同决定可行性；静态换优化、量化换精度。可验证：checker+allclose、量化误差、目标引擎延迟。

#### 5. 图文说明

| 对比 | A | B | 选择 |
|---|---|---|---|
| 格式 | TorchScript | export/ONNX | 新项目后者 |
| shape | 静态 | 动态 | 优化 vs 泛化 |
| 精度 | 浮点 | 量化 | 精度 vs 延迟 |
| 引擎 | TensorRT | ORT | 极致 vs 兼容 |

图示解释：格式-引擎-精度三维选择表，按目标硬件约束收敛。

#### 6. 拓展知识

延伸：AOTInductor、量化感知训练、TensorRT 精度 profile、算子支持矩阵、边缘部署裁剪。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 新项目用哪个？——export/ONNX。
2. 量化代价？——校准与精度损失。
3. 引擎怎么选？——按目标硬件。

#### 8. 易错点与反例

1. 用 TorchScript 表达动态控制流受限。
2. 量化不校准，精度崩。
3. 引擎不支持某算子，上线才发现。

#### 9. 实践与验证

脚本：多格式导出对拍；动态边界测试；量化误差；目标引擎实测。oracle 为“格式/引擎/精度取舍有实测证据”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B092 评分：不合格只导出 ONNX；合格能说格式与 shape 差异；良好能对比量化与引擎并验证；优秀能按硬件/动态/精度目标给出端到端部署选型与风险清单。
### B093. 对比辨析：自定义算子

- 难度：入门级
- 知识域：K11 自定义算子
- 考察能力：不变量、二阶影响、证据缺口与演进路线
- 题型：综合开放/专家追问
- 面试频率：高频
- 前置知识：schema、dispatcher、stream、autograd、ABI
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

自定义算子的对比辨析：`autograd.Function`（Python、易写、需手写反向）vs `torch.library`（schema 注册、可被 compile 捕获）vs C++/CUDA 扩展（最高性能、最高成本）；融合 kernel vs 组合现有算子；可微 vs 不可微。取舍是性能、开发成本与可维护性。

#### 2. 问题分析

按对比维度：性能上限、开发成本、compile 协同、可移植性、维护。给出选择：能组合就不写，Python 够用用 autograd.Function，需 compile 捕获用 torch.library，极致性能才上 CUDA。

#### 3. 具体答案

面试中可以这样回答：
> 自定义算子的对比是性能与成本的阶梯。第一，`autograd.Function`：纯 Python，写起来最快，需自己实现 `backward`，适合把一段可微组合封装成算子或加自定义反向；但它对 dispatcher 和图捕获支持较弱，`torch.compile` 下常断图。第二，`torch.library`（`custom_op`）：注册 schema 和后端实现，能被 dispatcher 正确分派、能被 `torch.compile` 捕获、`opcheck` 可校验，是现代推荐的注册方式，开发成本介于 Python 和 C++ 之间。第三，C++/CUDA 扩展：能写极致性能的融合 kernel，但开发、编译、调试成本最高，且 ABI 随版本变化、可移植性差。第四，融合 vs 组合现有算子：组合实现简单、可读，但 kernel 启动和中间张量多；融合快但代码难维护。决策准则：先尝试组合和 `torch.compile` 自动融合，不够再用 `torch.library`，只有 profile 证明需要手写 kernel 才上 CUDA。验证：gradcheck/opcheck/compile 捕获率/微基准。

#### 4. 技术洞察

不变量：性能上限 CUDA > library > Function；可维护性相反；compile 捕获需 schema 注册。可验证：微基准、opcheck、explain 断点、跨版本编译。

#### 5. 图文说明

| 方式 | 性能上限 | 成本 | compile |
|---|---|---|---|
| 组合现有算子 | 低 | 最低 | 通常支持 |
| autograd.Function | 中 | 低 | 易断图 |
| torch.library | 中高 | 中 | 支持 |
| CUDA 扩展 | 最高 | 最高 | 需注册 |

图示解释：成本-性能阶梯，从左下到右上。

#### 6. 拓展知识

延伸：`custom_op` + `register_autograd`、CUDA 构建矩阵、融合收益 vs 维护成本、AOTInductor 自定义算子。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 先试什么？——组合/自动融合。
2. 为何用 library？——可编译、可校验。
3. 何时上 CUDA？——profile 证明必要。

#### 8. 易错点与反例

1. 直接写 CUDA，收益小维护重。
2. autograd.Function 导致 compile 断图。
3. 组合算子未用 compile 融合，性能差。

#### 9. 实践与验证

脚本：同一功能三种实现对比微基准与 compile 捕获率。oracle 为“性能/成本/可编译差异可测”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B093 评分：不合格只会写 Python 算子；合格能说三种方式差异；良好能按性能与成本选择并验证；优秀能给出“组合→library→CUDA”的阶梯式决策与可维护性方案。
### B094. 对比辨析：测试治理

- 难度：入门级
- 知识域：K12 测试治理
- 考察能力：不变量、二阶影响、证据缺口与演进路线
- 题型：综合开放/专家追问
- 面试频率：高频
- 前置知识：oracle、证据、日志、回滚、门禁、复盘
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

测试治理的对比辨析：单元 vs 集成 vs 端到端（速度 vs 覆盖）、精确 vs 容差（严格 vs 鲁棒）、本地 vs CI（即时 vs 隔离）、静态 golden vs 动态参考（稳定 vs 自适应）。取舍是反馈速度、覆盖深度与维护成本。

#### 2. 问题分析

按对比维度：速度、覆盖、稳定性、维护。给出选择：底层多单元、上层少端到端；数值用容差；本地快测+CI 全量；golden 要版本化。

#### 3. 具体答案

面试中可以这样回答：
> 测试治理的对比是“反馈速度 vs 覆盖深度”的谱系。第一，单元 vs 集成 vs 端到端：单元测试快、定位准、数量多，是金字塔底；集成测试验证组件协作（如单步训练、checkpoint 往返）；端到端验证整体行为但慢且脆弱，数量要少。第二，精确 vs 容差：精确比较只适合离散/结构断言，数值必须用容差（`assert_close`），因为 GPU 归约非确定。第三，本地 vs CI：本地跑秒级快测给即时反馈，CI 跑全量和 GPU 集成保证隔离与一致，用标记分层。第四，静态 golden vs 动态参考：golden 稳定但需版本管理和定期更新，动态参考（用另一实现现算）自适应但依赖参考实现本身正确。取舍原则：底层厚、顶层薄，数值容差、结构精确，快测本地、慢测 CI，golden 版本化、参考实现可信。验证：测试时间分布、逃逸缺陷率、flaky 率。

#### 4. 技术洞察

不变量：覆盖深度与反馈速度成反比；数值系统必须用容差；golden 需版本化。可验证：时间分布、逃逸缺陷、flaky 率。

#### 5. 图文说明

| 对比 | A | B | 选择 |
|---|---|---|---|
| 层级 | 单元 | 端到端 | 金字塔 |
| 比较 | 精确 | 容差 | 数值用容差 |
| 环境 | 本地 | CI | 分层 |
| oracle | golden | 动态参考 | 版本化/可信 |

图示解释：测试金字塔，底部快而多、顶部慢而少。

#### 6. 拓展知识

延伸：测试选择、CI 分片、golden 更新流程、参考实现的可信度、覆盖率门禁。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 为何金字塔？——速度与定位。
2. 浮点怎么比？——容差。
3. golden 问题？——需版本管理。

#### 8. 易错点与反例

1. 端到端过多，CI 慢且脆。
2. 用精确比较数值。
3. golden 过期未更新。

#### 9. 实践与验证

脚本：搭三层测试并标注时间；对比精确/容差；golden 版本化。oracle 为“速度-覆盖谱系配置合理”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B094 评分：不合格只写一种测试；合格能说金字塔；良好能对比容差/golden 并验证；优秀能按反馈速度与覆盖深度设计分层测试并治理 golden 与 flaky。
### B095. 故障信号：张量/设备

- 难度：入门级
- 知识域：K01 张量/设备
- 考察能力：不变量、二阶影响、证据缺口与演进路线
- 题型：综合开放/专家追问
- 面试频率：高频
- 前置知识：shape、stride、dtype、device
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

张量/设备的故障信号：device mismatch 报错、OOM、非连续导致 view 失败、dtype 溢出/下溢、隐式同步拖慢、共享内存被意外修改。诊断靠错误信息、`data_ptr`、`is_contiguous`、Profiler、memory summary。专家级要能定位到根因并给恢复路径与残余风险。

#### 2. 问题分析

按信号→根因→诊断→修复：`Expected all tensors on same device`→张量未同设备；`OutOfMemoryError`→峰值超显存；`view size is not compatible`→非连续；inf/nan→dtype 范围；利用率低→隐式同步。验证修复后信号消失。

#### 3. 具体答案

面试中可以这样回答：
> 张量/设备的故障信号有明确的根因映射。第一，`Expected all tensors to be on the same device`：某个张量没搬到 GPU（常见于新创建的索引/常量），诊断是检查 `x.device`，修复是统一设备。第二，`CUDA out of memory`：中间张量峰值超显存，诊断用 `torch.cuda.memory_summary()` 看 allocations，修复是减小 batch、梯度检查点、及时 `del`/`empty_cache`。第三，`view size is not compatible`：对非连续张量用了 view，诊断 `is_contiguous()`，修复用 `reshape` 或 `contiguous()`。第四，结果出现 inf/nan：dtype 范围溢出（fp16 超 65504）或下溢，诊断查数值范围，修复升精度或用 scaler。第五，GPU 利用率周期性掉：隐式同步或数据瓶颈，诊断 Profiler 找 `aten::item`/`Memcpy`，修复避免在循环里 `.item()`。专家级还要评估残余风险：修复是否只针对当前 shape、是否掩盖了更深的内存泄漏，并加监控（显存趋势、同步点）防复发。

#### 4. 技术洞察

不变量：报错信息-根因一一对应；修复后信号消失且不引入新风险。可验证：`x.device`、`memory_summary`、`is_contiguous`、Profiler 同步点。

#### 5. 图文说明

| 信号 | 根因 | 修复 |
|---|---|---|
| device mismatch | 未同设备 | 统一 .to |
| OOM | 峰值超显存 | 减 batch/checkpoint |
| view 失败 | 非连续 | reshape/contiguous |
| 利用率掉 | 隐式同步 | 去 .item |

图示解释：信号→根因→修复的排查路径图。

#### 6. 拓展知识

延伸：memory snapshot、`record_function`、泄漏检测、`torch.cuda.empty_cache` 的适用与误区。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. OOM 怎么定位？——memory summary。
2. view 失败原因？——非连续。
3. 利用率为何掉？——同步或数据瓶颈。

#### 8. 易错点与反例

1. 用 `empty_cache` 掩盖泄漏。
2. 把 device mismatch 归因于模型，实为新建常量。
3. 忽略隐式同步，误判为计算慢。

#### 9. 实践与验证

脚本：逐条复现信号并诊断，修复后确认消失；记录显存趋势防复发。oracle 为“信号-根因-修复闭环”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B095 评分：不合格看到报错就慌；合格能读错误定位；良好能用工具诊断根因；优秀能给出修复+残余风险+监控防复发的完整事故闭环。
### B096. 故障信号：autograd

- 难度：入门级
- 知识域：K02 autograd
- 考察能力：不变量、二阶影响、证据缺口与演进路线
- 题型：综合开放/专家追问
- 面试频率：高频
- 前置知识：requires_grad、leaf、view、detach、反向
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

autograd 的故障信号：`element 0 of tensors does not require grad`、`Trying to backward through the graph a second time`、`a variable needed for gradient computation has been modified by an inplace operation`、loss 为 nan、梯度为 None/为 0、`grad can be implicitly created only for scalar outputs`。诊断靠 `set_detect_anomaly`、检查 `requires_grad`/`grad_fn`、定位 in-place。

#### 2. 问题分析

信号→根因→诊断→修复：无梯度→输入未 requires_grad/被 detach；二次回传→图已释放；in-place→版本检查；nan→数值问题。验证修复后训练正常。

#### 3. 具体答案

面试中可以这样回答：
> autograd 的故障信号都能对应到具体根因。第一，`element 0 of tensors does not require grad and does not have a grad_fn`：对不需要梯度的张量调了 backward，诊断打印 `requires_grad`/`grad_fn`，常见于输入被 `.detach()` 或用了 `no_grad`。第二，`Trying to backward through the graph a second time`：对同一图回传两次且未 `retain_graph=True`，修复是保留图或重建前向。第三，`...modified by an inplace operation`：前向保存的张量被原地改，开 `torch.autograd.set_detect_anomaly(True)` 可定位到具体算子，修复是用 `x = x + 1` 而非 `x += 1` 或包 `no_grad`。第四，loss 变 nan：学习率过大、除零、log(0) 等，诊断逐层打印中间值，修复加 epsilon、梯度裁剪、调 lr。第五，梯度为 None 或 0：参数未参与计算、被冻结、或上游梯度消失，诊断 `p.grad` 与计算图。第六，对非标量 backward 报 `grad can be implicitly created only for scalar outputs`，需传 `grad_outputs` 或先 `.sum()`。专家级要顺带检查数值稳定性与监控梯度范数。

#### 4. 技术洞察

不变量：报错对应具体契约违反；`detect_anomaly` 定位 in-place；修复后梯度流恢复。可验证：`requires_grad`/`grad_fn`、anomaly 回溯、梯度范数监控。

#### 5. 图文说明

| 信号 | 根因 | 修复 |
|---|---|---|
| 不 require grad | 未建图 | 检查 requires_grad |
| 二次回传 | 图释放 | retain_graph |
| in-place | 版本检查 | 非原地写法 |
| nan | 数值问题 | eps/裁剪/lr |

图示解释：从报错到根因再到修复的映射表。

#### 6. 拓展知识

延伸：`set_detect_anomaly` 开销、梯度消失/爆炸诊断、`register_hook` 观测、混合精度与 nan。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 无梯度报错？——输入未建图。
2. 二次回传？——图已释放。
3. nan 怎么查？——逐层打印+anomaly。

#### 8. 易错点与反例

1. 对 detach 后张量回传。
2. 复用图两次未 retain。
3. in-place 改激活导致报错。

#### 9. 实践与验证

脚本：逐条复现信号并诊断；开 anomaly 定位 in-place；监控梯度范数。oracle 为“信号-根因-修复闭环”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B096 评分：不合格看到报错就重试；合格能读错误定位；良好能用 anomaly/监控诊断根因；优秀能修复并加梯度稳定性监控，给出残余风险与复发防护。
### B097. 故障信号：Module/状态

- 难度：入门级
- 知识域：K03 Module/状态
- 考察能力：不变量、二阶影响、证据缺口与演进路线
- 题型：综合开放/专家追问
- 面试频率：高频
- 前置知识：参数、buffer、模式和 state_dict
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

Module/状态的故障信号：参数不更新、eval/inference 结果随机、加载出现 missing/unexpected keys、size mismatch、显存泄漏、train/eval 行为错。诊断靠 `named_parameters`、state_dict 键集合、tensorboard/显存趋势、检查注册与 `training` 标志。

#### 2. 问题分析

信号→根因→诊断→修复：不更新→未注册；随机→未 eval；键不匹配→前缀/结构变更；泄漏→未注册张量累积。验证修复后信号消失。

#### 3. 具体答案

面试中可以这样回答：
> Module/状态的故障信号都能追到“注册/训练态/持久化”三类根因。第一，参数训练多步不更新：诊断 `list(model.named_parameters())`，若该参数不在其中说明是普通 Tensor 属性没注册，修复为 `nn.Parameter`；另一可能是 `.grad is None`（未参与损失）或 lr 为 0。第二，推理结果随机或指标偏低：忘记 `model.eval()`，Dropout/BN 仍在训练态，诊断打印 `model.training` 和层行为，修复加 eval。第三，加载 `missing keys`/`unexpected keys`：多卡 DDP 的 `module.` 前缀或结构变化，诊断对比 `state_dict().keys()`，修复去前缀或映射键。第四，`size mismatch`：张量形状改变，修复对齐结构或按需加载。第五，显存随训练增长不降：在 forward 里把张量缓存成 `self.x` 等未注册长期引用，诊断 memory snapshot，修复不要缓存或正确注册/释放。专家级要检查 checkpoint 完整性、训练态一致性与监控显存趋势。

#### 4. 技术洞察

不变量：只有注册对象才被管理与持久化；eval 冻结行为；键集合决定加载匹配。可验证：`named_parameters`、键集合对比、`training` 标志、memory snapshot。

#### 5. 图文说明

| 信号 | 根因 | 修复 |
|---|---|---|
| 参数不更新 | 未注册/无梯度 | Parameter/查 grad |
| 结果随机 | 未 eval | model.eval |
| 键不匹配 | 前缀/结构 | 去前缀/对齐 |
| 显存增长 | 未注册缓存 | 释放/注册 |

图示解释：注册树与训练态两处热点，信号分别落在这两处。

#### 6. 拓展知识

延伸：`load_state_dict(strict)`、`_register_load_state_dict_pre_hook`、memory snapshot、EMA buffer 化。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 参数为何不更新？——未注册或无梯度。
2. 推理为何随机？——未 eval。
3. 键不匹配怎么修？——去前缀/对齐。

#### 8. 易错点与反例

1. 普通 Tensor 权重永远不更新。
2. 忘 eval，指标低且不稳。
3. 缓存张量致显存缓慢泄漏。

#### 9. 实践与验证

脚本：逐条复现并诊断（named_parameters/键集合/training/snapshot）并修复。oracle 为“信号-根因-修复闭环”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B097 评分：不合格看到异常就乱改；合格能读状态定位；良好能用注册/键/显存工具诊断；优秀能修复并建立状态一致性与显存趋势监控防复发。
### B098. 故障信号：数据管道

- 难度：入门级
- 知识域：K04 数据管道
- 考察能力：不变量、二阶影响、证据缺口与演进路线
- 题型：综合开放/专家追问
- 面试频率：高频
- 前置知识：Dataset、Sampler、collate、padding、版本
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

数据管道的故障信号：worker 报错/重启、GPU 空档、数据重复、增强不可复现、fd/内存泄漏、collate 形状错误、pin 崩溃。诊断靠 worker 日志、`get_worker_info`、吞吐计时、索引集合、fd/内存监控。专家级要定位到具体进程与资源。

#### 2. 问题分析

信号→根因→诊断→修复：GPU 空档→数据瓶颈；数据重复→未分片；不可复现→种子；fd 泄漏→未关文件；collate 错误→变长。验证修复后信号消失。

#### 3. 具体答案

面试中可以这样回答：
> 数据管道的故障信号集中在进程与随机性。第一，GPU 利用率周期性空档：数据端跟不上计算，诊断计时取样与计算时间、看 CPU 占用，修复增加 worker/预取或优化 `__getitem__`。第二，数据重复或指标虚高：多 worker 未分片或 DistributedSampler 配置错，诊断打印各 worker 索引集合，修复按 `get_worker_info`/rank 分片并 `set_epoch`。第三，增强结果不可复现或所有 worker 相同：`worker_init_fn` 未设种，诊断打印 worker seed，修复按 worker 派生种子。第四，进程被 killed/OOM：worker 持有大对象或文件句柄未关，诊断看子进程退出码与 fd/内存，修复及时释放、用 `persistent_workers` 管理。第五，`RuntimeError: stack expects each tensor to be equal size`：默认 collate 遇到变长数据，修复自定义 pad+collate。第六，`pin_memory` 引发崩溃或内存压力：页锁定内存不可换出，诊断主机内存，修复限制 pin 范围或关闭。专家级要区分是数据、IO 还是模型导致，并用计数器监控 worker 重启与队列深度。

#### 4. 技术洞察

不变量：GPU 空档 ⟺ 数据端慢；重复 ⟺ 未分片；不可复现 ⟺ 种子未固定。可验证：计时、索引集合、worker seed、fd/内存计数。

#### 5. 图文说明

| 信号 | 根因 | 修复 |
|---|---|---|
| GPU 空档 | 数据瓶颈 | 增 worker/预取 |
| 数据重复 | 未分片 | 分片+set_epoch |
| 增强不可复现 | 未设种 | worker_init_fn |
| fd 泄漏 | 未关资源 | 及时释放 |

图示解释：生产者-消费者队列，信号对应生产者或队列异常。

#### 6. 拓展知识

延伸：`persistent_workers`、共享内存、内存映射、子进程退出码、worker 健康监控。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. GPU 为何空档？——数据端跟不上。
2. 数据重复怎么查？——索引集合。
3. 增强不可复现？——worker 种子。

#### 8. 易错点与反例

1. 只加 worker 不查瓶颈位置。
2. 未分片导致重复。
3. worker 持大对象被 OOM kill。

#### 9. 实践与验证

脚本：逐条复现信号并诊断（计时/索引/seed/fd），修复后确认。oracle 为“信号-根因-修复闭环”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B098 评分：不合格盲目调参；合格能读现象定位；良好能用分片/种子/计时/资源工具诊断；优秀能修复并加 worker 健康与队列监控，区分数据/IO/模型根因。
### B099. 故障信号：训练/优化器

- 难度：入门级
- 知识域：K05 训练/优化器
- 考察能力：不变量、二阶影响、证据缺口与演进路线
- 题型：综合开放/专家追问
- 面试频率：高频
- 前置知识：forward、backward、累积、optimizer、scheduler
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

训练/优化器的故障信号：loss 不降、loss 变 nan、loss 震荡、梯度爆炸/消失、lr 不生效、显存增长、收敛突然变差。诊断靠打印 lr/梯度范数/loss 曲线、`set_detect_anomaly`、检查 zero_grad/scheduler 顺序、梯度直方图。专家级要区分优化问题与数据/模型问题。

#### 2. 问题分析

信号→根因→诊断→修复：loss 不降→lr 太小/未清零/梯度消失；nan→lr 太大/除零；震荡→lr 太大；梯度爆炸→需裁剪。验证修复后曲线正常。

#### 3. 具体答案

面试中可以这样回答：
> 训练/优化器的故障信号要按“优化动态”排查。第一，loss 完全不动：可能 lr 太小、参数未注册（没被更新）、梯度为 0（消失）、或数据标签错误；诊断打印 `param.grad` 范数与 `get_last_lr()`，逐一排除。第二，loss 变 nan：lr 过大、除零、log(0)、fp16 溢出；诊断开 `set_detect_anomaly`、逐层打印中间值，修复加 epsilon、降 lr、加梯度裁剪、用 AMP scaler。第三，loss 震荡不收敛：lr 太大或 batch 太小，诊断 lr 曲线与梯度范数，修复降 lr、加 warmup/调度器、增大有效 batch。第四，梯度爆炸：梯度范数持续增大，修复 `clip_grad_norm_` 与 wd；梯度消失：深层网络或饱和激活，修复用残差/归一化/合适初始化。第五，lr 不生效：scheduler 顺序错或 param_group lr 被覆盖，诊断 `get_last_lr()`。第六，收敛中途突然变差：可能是数据分布变化或数值问题。专家级要区分是优化器、数据还是模型结构导致，并加 lr/梯度/loss 监控。

#### 4. 技术洞察

不变量：loss 不降→梯度/lr/数据之一异常；nan→数值溢出；震荡→lr 过大。可验证：lr 序列、梯度范数、loss 曲线、anomaly 回溯。

#### 5. 图文说明

| 信号 | 根因 | 修复 |
|---|---|---|
| loss 不降 | lr/梯度/数据 | 查 grad/lr |
| nan | 溢出/除零 | eps/降 lr/scaler |
| 震荡 | lr 太大 | 降 lr/warmup |
| 梯度爆炸 | 范数增大 | 裁剪 |

图示解释：loss 曲线形态对应根因的决策树。

#### 6. 拓展知识

延伸：warmup、梯度裁剪、梯度直方图、lr finder、损失尺度匹配（如 focal loss）。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. loss 不动？——lr/梯度/数据。
2. nan 怎么查？——anomaly+逐层。
3. 震荡原因？——lr 大。

#### 8. 易错点与反例

1. 只调 lr 不查梯度。
2. nan 直接重启，不修根因。
3. scheduler 顺序错导致 lr 不生效。

#### 9. 实践与验证

脚本：打印 lr/梯度范数/loss，开 anomaly，复现 nan/震荡并修复。oracle 为“信号-根因-修复闭环”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B099 评分：不合格只盯 loss；合格能查 lr/梯度；良好能用 anomaly 与监控诊断；优秀能区分优化/数据/结构根因并加 lr-梯度-loss 三位监控防复发。
### B100. 故障信号：AMP/恢复

- 难度：入门级
- 知识域：K06 AMP/恢复
- 考察能力：不变量、二阶影响、证据缺口与演进路线
- 题型：综合开放/专家追问
- 面试频率：高频
- 前置知识：autocast、scaler、快照、随机状态
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认]

#### 1. 背景知识

AMP/恢复的故障信号：loss 变 nan/inf、scale 快速下降到极小、大量溢出跳步、恢复后曲线不连续、显存异常、bf16/fp16 混用错误。诊断靠监控 `scaler.get_scale()`、溢出次数、checkpoint 完整性、RNG 一致性。专家级要区分数值溢出与状态缺失两类根因。

#### 2. 问题分析

信号→根因→诊断→修复：nan→fp16 溢出/无 scaler；scale 触底→持续溢出；曲线不连续→恢复缺状态；精度差→bf16 尾数或未保留 fp32。验证修复后训练稳定。

#### 3. 具体答案

面试中可以这样回答：
> AMP/恢复的故障信号分数值与状态两类。第一，loss 变 nan/inf：在 fp16 下某层激活或梯度超 65504，或没加 GradScaler 导致下溢/溢出；诊断监控 `scaler.get_scale()` 与溢出次数，修复用 bf16、对敏感层保 fp32、检查是否有除零/log(0)。第二，scale 快速下降到极小：持续溢出说明频繁跳步，有效训练步数不足；诊断看溢出频率与出现溢出的层，修复改 bf16 或调整模型数值稳定（归一化、eps）。第三，断点恢复后 loss 曲线不连续：状态缺失（只存了 model，缺 optimizer 动量、scaler scale、RNG、step），诊断核对 checkpoint 内容，修复保存完整状态；漏 RNG 会让 dropout 流错位。第四，精度明显变差但没 nan：bf16 尾数少或低精度累加，修复归约/loss 用 fp32、保留 master weights。第五，显存异常：scaler 或优化器状态未按预期分片。第六，fp16/bf16 混用或手动 `.half()` 导致类型错误。专家级要同时监控 scale、溢出率、梯度范数与 checkpoint 完整性，并评估数值风险的残余不确定性。

#### 4. 技术洞察

不变量：nan ⟺ 数值溢出/未缩放；scale 触底 ⟺ 持续溢出；曲线断 ⟺ 状态缺失。可验证：`get_scale()` 轨迹、溢出计数、续训对拍、梯度范数。

#### 5. 图文说明

| 信号 | 根因 | 修复 |
|---|---|---|
| nan/inf | fp16 溢出/无 scaler | bf16/保 fp32/scaler |
| scale 触底 | 持续溢出 | 改 bf16/稳定化 |
| 曲线不连续 | 状态缺失 | 存全状态+RNG |
| 精度差 | bf16 尾数 | fp32 累加 |

图示解释：数值分支与状态分支两条排查路径。

#### 6. 拓展知识

延伸：per-layer dtype、fp8、scale 监控告警、checkpoint schema 校验、确定性算法与 RNG。未运行结论标 `[待验证]`。

#### 7. 常见追问及回答要点

1. 为何 nan？——溢出或未缩放。
2. scale 触底？——持续溢出。
3. 曲线为何断？——状态不全。

#### 8. 易错点与反例

1. 不用 scaler，梯度下溢。
2. 只存 model，恢复偏移。
3. bf16 累积误差导致精度降。

#### 9. 实践与验证

脚本：监控 scale/溢出/nan；完整恢复对拍；bf16/fp16 精度对比。oracle 为“数值与状态两类信号均被诊断并修复”。未运行结论标 `[待验证]`。

#### 10. 评分标准

B100 评分：不合格看到 nan 就重启；合格能查 scale 与状态；良好能区分数值/状态根因并诊断；优秀能修复并加 scale/溢出/完整性监控，量化数值残余风险与恢复一致性。
