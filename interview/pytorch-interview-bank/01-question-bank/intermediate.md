# 中等级 PyTorch 面试题

本文件包含 `I001`–`I100`，共 100 题；统一十类题型各 10 题，GPU、通信和性能结果未经目标环境运行时标为 [待验证]。

### I001. 一个标准 PyTorch 训练循环的状态和顺序如何设计？

- 难度：中等级
- 知识域：K05 优化器、训练循环与数值稳定性
- 考察能力：概念边界、适用条件与反例
- 题型：概念解释与辨析
- 面试频率：高频
- 前置知识：Module、DataLoader、autograd、optimizer
- 关联题目：B003、B005、I003、A001
- 事实状态：[已确认]

#### 1. 背景知识

- PyTorch 的动态计算图在 forward 期间构建，每个参与运算且 `requires_grad=True` 的张量都会记录 `grad_fn`；`loss.backward()` 沿图反向遍历，把梯度累加到叶子张量的 `.grad`。
- 梯度是累加而非覆写，`optimizer.zero_grad()` 若不调用，本轮梯度会与上一轮叠加。新版本推荐 `set_to_none=True`，把 `.grad` 置为 `None`，省一次 memset，并让“未使用参数”保持可识别的 `None`。
- `optimizer.step()` 只读取 `.grad` 更新 `param.data`，它不感知计算图；`scheduler.step()` 只改 `param_groups` 里的 `lr`，其调用频率必须与 epoch/step 语义对齐。
- BN、Dropout 的行为由 `module.train()`/`module.eval()` 控制，模式切换是训练循环状态的一部分。

#### 2. 问题分析

把循环拆成四个状态块：数据、前向、反向、更新。两个不变量：(1) `step()` 消费的必须是本轮干净且完整的梯度，`zero_grad → forward → backward → step` 中不得在清零后再次累积；(2) 一次 `step()` 后参与更新的参数 `.grad` 应被清空。边界反例：梯度累积场景有意在 K 个 micro-batch 后才 `step()`，此时“每步零梯度”不成立，需先累积够再清零。

#### 3. 具体答案

面试中可以这样回答：

> 我按“清梯度→前向→算损失→反向→更新→调学习率”组织。先 `optimizer.zero_grad(set_to_none=True)`；`model.train()` 保证 BN、Dropout 处于训练态；`loss = criterion(model(x), y)`；`loss.backward()` 把梯度写进叶子参数；再 `optimizer.step()` 更新权重，按 epoch 或 step 语义调 `scheduler.step()`。核心是 backward 会累加，所以每个优化步前必须清零；若做梯度累积，就在累积 K 个 micro-batch 后只 step 一次；AMP 下把 forward/loss 放进 `autocast`，用 `scaler.step(optimizer)` 取代裸 step。

#### 4. 技术洞察

```
for x, y in loader:
    optimizer.zero_grad(set_to_none=True)
    with autocast(enabled=amp):
        loss = criterion(model(x), y) / accum
    loss.backward()
    if (i + 1) % accum == 0:
        scaler.step(optimizer); scaler.update()
        optimizer.zero_grad(set_to_none=True)
        scheduler.step()
```

第 k 次 backward 后 `.grad = Σ_{j≤k} g_j`，只在清零点之后才是一次干净更新；step 级调度器必须与 `optimizer.step` 次数一致。

#### 5. 图文说明

| 阶段 | 状态变化 | 错误信号 |
|---|---|---|
| zero_grad | `.grad → None/0` | 漏调用：梯度跨步膨胀 |
| forward | 建图，挂 `grad_fn` | 漏 `model.eval()`：BN 统计漂移 |
| backward | `.grad += ∂L/∂θ` | 二次 backward 未 `retain_graph` |
| step | `param.data -= lr·f(grad)` | `no_grad` 外改 `.data` 破坏图 |
| scheduler.step | `lr` 变化 | 每 batch 调，学习率早衰 |

#### 6. 拓展知识

AMP 把 step 换成 `scaler.step`；DDP 在 backward 中插入梯度 AllReduce，`no_sync()` 只应包裹累积用的中间 micro-batch；激活检查点在 forward 不存激活、backward 重算，不改变顺序但改变显存与算力配比。

#### 7. 常见追问及回答要点

1. 为什么 `set_to_none=True`？——省一次 memset，并让未使用参数保持 `None`。
2. scheduler 在 step 前还是后？——≥1.1 在 step 后调；提前调会跳过第一个 epoch 的学习率，属 off-by-one。
3. 梯度累积如何缩放 loss？——按累积步数除，保持等效 batch 梯度尺度一致。

#### 8. 易错点与反例

1. 忘 `zero_grad`，梯度跨 batch 累积，loss 震荡，只有长训练才暴露。
2. 对同一 loss 二次 backward 未 `retain_graph`，报 “backward through the graph a second time”。
3. 每个 batch 调 `scheduler.step()`，Cosine/Step 被加速，lr 提前归零。

#### 9. 实践与验证

在 toy 线性回归上打印每步 `.grad` 的 L2 范数，正常应回到同一量级而非单调增长；记录 `param_groups[0]['lr']` 曲线核对衰减点；梯度累积用累积 K 步与单步大 batch 的梯度做 `allclose`（float32 容差 [待验证]）。当前未执行 GPU 真实训练。

#### 10. 评分标准

I001 评分：不合格只背三词，说不出顺序约束；合格能完整复述顺序并说明 backward 累加；良好能解释 `set_to_none`、scheduler off-by-one 与累积交互；优秀/专家能给出等效 batch 梯度尺度推导、AMP/DDP 顺序变形并用范数日志验证。

### I002. 训练中出现梯度为零或 `None`，如何系统定位？

- 难度：中等级
- 知识域：K02 autograd、计算图与梯度
- 考察能力：概念边界、适用条件与反例
- 题型：概念解释与辨析
- 面试频率：高频
- 前置知识：autograd、Module、优化器
- 关联题目：B003、I001、E001
- 事实状态：[已确认]

#### 1. 背景知识

- autograd 是反向模式自动微分，forward 期间构建反向图；`backward()` 从标量 loss 出发对整个图做拓扑序反向遍历。
- `.grad` 只对叶子张量（`is_leaf=True`）且 `requires_grad=True` 时填充；非叶子中间结果的梯度默认释放，除非注册 hook 或 `retain_grad()`。
- `detach()`、`torch.no_grad()`、`.data` 都会切断图；对图外张量做 backward 会得到 `None` 或报 “element 0 of tensors does not require grad”。
- 梯度为 `None` 与梯度为零是两种不同现象：前者是“没有反向路径”，后者是“反向路径的通路数值为零”。

#### 2. 问题分析

按“入口—图—叶子”三步定位：(1) loss 是否 `requires_grad`（输入全部 `requires_grad=False` 或包在 `no_grad` 内会得到 `None`）；(2) 反向图是否被 `detach`/`item()`/numpy 转换切断；(3) 叶子参数的 `.grad` 是否被置零的目标函数或 `zero_grad` 覆盖。两个不变量：(a) 只有 `requires_grad=True` 的叶子才会收到 `.grad`；(b) 反向图必须从 loss 连通到该叶子。边界反例：`softmax` 在极大 logits 下饱和，梯度数值趋于零但并不为 `None`，此时是数值问题而非图断裂。

#### 3. 具体答案

面试中可以这样回答：

> 先看 loss 本身：`loss.requires_grad` 为 False 说明整条链没建图，常见于输入张量忘了 `requires_grad=True` 或损失被 `no_grad`/`detach` 包裹。再看中间是否被 `detach()`、`.numpy()`、`int()`/`item()` 切断。若 loss 有 `grad_fn` 而某参数 `.grad is None`，检查该参数是否真的参与了 forward，或是否在 `eval` 分支里被跳过。最后确认没有别处把 `.grad` 清零。定位工具：`torch.autograd.set_detect_anomaly(True)`、对参数注册 `register_hook`、以及 `gradcheck`。

#### 4. 技术洞察

链式法则：`∂L/∂x_i = Σ_j (∂L/∂y_j)·(∂y_j/∂x_i)`。若某条边被 detach，则该分支贡献恒为零。数值上梯度为零的常见形态是 `∂σ(z)/∂z = σ(1−σ)` 在 `|z|` 很大时趋零，以及 ReLU 在负输入区间导数为零。

```
loss.requires_grad            # False → 图没建
param.grad is None            # 断链或未参与
torch.autograd.grad(loss, p, allow_unused=True)  # 显式查是否连通
```

#### 5. 图文说明

| 检查点 | 证据 | 结论 |
|---|---|---|
| loss.requires_grad | False | 图未构建（no_grad/detach/全冻结） |
| grad_fn 链 | 某处为 None | 该处 detach 断链 |
| 参数参与 forward | grad is None | 分支未执行或未使用 |
| 数值幅值 | grad 极小 | 饱和/梯度消失，非断链 |

#### 6. 拓展知识

`torch.autograd.grad` 与 `backward` 的区别在于是否写 `.grad`、是否允许未使用参数；`anomaly mode` 会打印反向出错的前向栈，代价是显著变慢；自定义 `Function` 若反向返回错形状或用了 `save_for_backward` 之外的状态，会产生难查的 `None`。

#### 7. 常见追问及回答要点

1. `None` 和零怎么区分？——`None` 是图不连通，零是通路导数为零，用 `grad_fn` 与 `allow_unused` 区分。
2. `retain_graph=True` 的作用？——保留图结构供二次 backward，否则释放后报错。
3. 冻结部分参数的正确做法？——设 `param.requires_grad=False` 并确认 optimizer 不含它，或 `set_to_none` 后 `None` 即可。

#### 8. 易错点与反例

1. 在 `with torch.no_grad():` 里算 loss 再 backward，直接报 “does not require grad”。
2. 用 `loss.item()` 参与后续计算，数值链条脱离 tensor，梯度丢失。
3. 把 `requires_grad=False` 的输入喂进网络却期望输入梯度，得到 `None`。

#### 9. 实践与验证

构造最小例子：线性模型 `x→Linear→MSELoss`，先正常 backward 记录 `.grad`；再对 loss 加一行 `.detach()` 复现 `None`；再用大 logits 的 softmax 复现“极小但非 None”。用 `gradcheck` 对含自定义算子的模块做数值梯度校验。当前未执行 GPU 或分布式实验。

#### 10. 评分标准

I002 评分：不合格把 `None` 与零混为一谈；合格能说出叶子与 requires_grad 的关系；良好能用 `grad_fn`/`allow_unused` 系统定位断链；优秀/专家能区分断链与饱和两类根因并给出 hook、anomaly、gradcheck 的组合验证。

### I006. 如何设计可恢复且可审计的 PyTorch checkpoint？

- 难度：中等级
- 知识域：K06 AMP、Checkpoint 与可复现性
- 考察能力：概念边界、适用条件与反例
- 题型：概念解释与辨析
- 面试频率：高频
- 前置知识：Module、optimizer、scheduler、随机数
- 关联题目：I001、A004、E006
- 事实状态：[已确认]

#### 1. 背景知识

- 一个可恢复 checkpoint 至少要保存四类状态：模型 `state_dict`、优化器 `state_dict`（含 Adam 的 `exp_avg`、`exp_avg_sq`、`step`）、学习率调度器与 AMP `GradScaler` 状态、以及 RNG 状态（Python、NumPy、CPU、CUDA 各自的 RNG）。
- `torch.save` 用 pickle 序列化 tensor 与普通对象；加载时 `map_location` 决定张量落到哪个设备，否则恢复会绑死在保存时的设备。
- 恢复的语义是“从第 N 步继续”，所以除参数外还必须恢复“训练进度”（epoch、global_step）与数据管道的随机性，否则会重复或跳过样本。
- 审计要求记录：恢复时的步数、跳过次数、scaler 当前 scale、以及状态字典的哈希/版本，便于复现与排障。

#### 2. 问题分析

把 checkpoint 当作一个状态机快照：模型参数、优化器动量、调度器计数、AMP scale、RNG、数据采样位置。两个不变量：(1) 恢复后第一次 `step()` 必须与崩溃前下一 step 完全等价；(2) 优化器 `param_groups` 的参数顺序必须与保存时一致，否则动量会被错配到别的参数。边界反例：中断发生在 `backward` 之后、`step` 之前，此时梯度已写入 `.grad`，严格等价要求把梯度也存下来，否则应回滚到上一个完整 step 边界。

#### 3. 具体答案

面试中可以这样回答：

> 我会保存一个字典：`model.state_dict()`、`optimizer.state_dict()`、`scheduler.state_dict()`、`scaler.state_dict()`（若用 AMP）、以及 `torch.get_rng_state()`、`torch.cuda.get_rng_state_all()`、`np.random.get_state()`、Python `random.getstate()`；再加 `epoch`、`global_step`、数据采样器的 `set_epoch` 值。加载时先建好模型与优化器再 `load_state_dict`，用 `map_location` 对齐设备，然后恢复 RNG 与计数。审计上写一个 manifest：步数、文件哈希、PyTorch 版本、world_size，便于排查。

#### 4. 技术洞察

````python
ckpt = {
  "model": model.state_dict(),
  "optim": optimizer.state_dict(),
  "sched": scheduler.state_dict(),
  "scaler": scaler.state_dict(),
  "rng_cpu": torch.get_rng_state(),
  "rng_cuda": torch.cuda.get_rng_state_all(),
  "epoch": epoch, "global_step": gstep, "version": torch.__version__,
}
torch.save(ckpt, path)
# resume
sd = torch.load(path, map_location="cpu", weights_only=False)
model.load_state_dict(sd["model"])       # strict
optimizer.load_state_dict(sd["optim"])    # 参数顺序必须一致
scheduler.load_state_dict(sd["sched"]); scaler.load_state_dict(sd["scaler"])
torch.set_rng_state(sd["rng_cpu"]); torch.cuda.set_rng_state_all(sd["rng_cuda"])
````

优化器状态按 `param_groups` 中参数下标索引，因此恢复前重建模型的参数注册顺序必须与保存时完全一致。

#### 5. 图文说明

| 状态 | 缺失后果 | 审计字段 |
|---|---|---|
| model | 权重回退 | 参数量/版本 |
| optim | 动量丢失，loss 跳变 | step 计数 |
| scaler | scale 重置，溢出反复 | scale 值 |
| RNG | 数据顺序错乱 | seed 记录 |
| 进度计数 | 重复/跳过数据 | epoch、gstep |

#### 6. 拓展知识

Sharded checkpoint（FSDP/DCP）需要按 rank 分片保存与聚合，`torch.distributed.checkpoint` 提供分布式 save/load；`weights_only=True` 提升了加载安全性但要求只含 tensor 与基本类型；若要 bitwise 复现还需固定 cudnn 算法与 `CUBLAS_WORKSPACE_CONFIG`。

#### 7. 常见追问及回答要点

1. 中断在 backward 与 step 之间怎么办？——存梯度或回滚到上一个 step 边界，二者选一并写进语义。
2. 如何保证参数顺序一致？——重建模型代码不变，或给参数带名字做校验。
3. 为什么需要 `map_location`？——保存时不固定设备，加载时按目标设备映射，避免绑死。

#### 8. 易错点与反例

1. 只存模型、不存优化器，恢复后 Adam 动量清零，loss 出现台阶。
2. 存了 RNG 却没在 resume 时 `set_rng_state`，数据打乱顺序与预期不符。
3. `scheduler` 未存，恢复后学习率从头开始，训练轨迹偏离。

#### 9. 实践与验证

验证方法：跑到第 N 步存一次，继续跑若干步记录 loss 序列；再从第 N 步恢复，比较后续 loss 是否在数值容差内一致（严格相等需 fixed determinism [待验证]）。用 `optimizer.load_state_dict` 后打印 `state` 键与 `step` 值核对。当前未执行多卡分片恢复实验。

#### 10. 评分标准

I006 评分：不合格只存模型；合格能存模型+优化器；良好能覆盖 RNG、scaler、进度计数并解释 map_location；优秀/专家能设计分片恢复、定义中断点语义，并用 loss 序列与 state 校验验证等价性。

### I007. 变长序列的 `collate_fn` 和 mask 如何设计？

- 难度：中等级
- 知识域：K04 数据管道与输入规范
- 考察能力：概念边界、适用条件与反例
- 题型：概念解释与辨析
- 面试频率：高频
- 前置知识：DataLoader、padding、attention mask
- 关联题目：B005、B006、A009
- 事实状态：[已确认]

#### 1. 背景知识

- `DataLoader` 默认用 `default_collate` 把一个 batch 的样本从 `list` 堆成带 batch 维的张量；序列长度不一时它无法直接堆叠，需要自定义 `collate_fn`。
- 变长序列的标准做法是 pad 到本 batch 的最大长度，再用 mask 标记有效位置；`torch.nn.utils.rnn.pad_sequence`、`pad_sequence` + `pack_padded_sequence` 是常见组合。
- Attention 语义里，mask 通常有两类：padding mask（把 pad 位置置极小值）与 causal mask（上三角置极小值），二者可叠加。
- 用 `-inf` 做 `softmax` 的 mask 比乘 0 更稳健，因为乘 0 后仍可能因数值误差产生非零概率；`-inf` 经 softmax 严格为 0。

#### 2. 问题分析

拆成三个职责：padding（对齐长度）、mask（标记有效位）、collate（把样本堆成 batch 并返回 mask）。两个不变量：(1) 每个样本的有效长度在 pad 后可由 mask 无损还原；(2) mask 的 shape 必须与下游 attention 的 score 矩阵对齐（通常是 `[B, L]` 或 `[B, 1, 1, L]` 广播）。边界反例：batch 里出现超长序列导致 pad 到该长度会浪费大量显存，此时应按长度分桶（bucketing）或按 token 数排序后分批，而不是一味 pad。

#### 3. 具体答案

面试中可以这样回答：

> `collate_fn` 收到的是一个 `list` of 样本，我从中取出 input_ids 与 length，用 `pad_sequence(..., batch_first=True)` pad 到本 batch 最大长度，同时产出一个 `attention_mask`：有效位为 1、pad 位为 0。对标签做同样 pad，并用 `-100` 填充让 `CrossEntropyLoss(ignore_index=-100)` 忽略。若是自回归任务再加 causal mask。返回 `(input_ids, attention_mask, labels)`。为避免长序列拖累短序列，我用长度分桶的 Sampler 让同 batch 长度接近。

#### 4. 技术洞察

````python
def collate(batch):
    ids = [b["ids"] for b in batch]
    padded = pad_sequence(ids, batch_first=True, padding_value=0)
    mask = (padded != pad_id).long()
    labels = pad_sequence([b["labels"] for b in batch],
                          batch_first=True, padding_value=-100)
    return padded, mask, labels
# attention 内：scores = scores.masked_fill(mask == 0, float("-inf"))
````

mask 与 score 的广播规则：`[B,L]` 的 mask 需 reshape 成 `[B,1,1,L]` 才能与 `[B,H,L,L]` 对齐。

#### 5. 图文说明

| 样本 | ids | length | mask |
|---|---|---|---|
| s1 | [a b c 0 0] | 3 | 1 1 1 0 0 |
| s2 | [d e 0 0 0] | 2 | 1 1 0 0 0 |

padding 只对齐长度，mask 才携带“哪些位置有效”的信息。

#### 6. 拓展知识

`pack_padded_sequence` 要求按长度降序排列以正确对齐 RNN；Transformer 一般不需要 pack，直接 mask 即可。`-inf` 与 `torch.finfo(dtype).min` 的取舍：fp16 下用极小有限值可避免 `-inf - (-inf) = nan`，但需保证其绝对值足够大使 softmax 饱和。

#### 7. 常见追问及回答要点

1. 为什么用 `-inf` 而不是乘 0？——乘 0 会保留 pad 位置的 exp(0)=1 概率质量，`-inf` 严格置 0。
2. pad 到全局最长还是 batch 内最长？——batch 内最长，避免超长样本拖累整批显存。
3. `ignore_index` 的作用？——让 loss 跳过 pad 与不应计分的标签位置。

#### 8. 易错点与反例

1. 忘记对 label 做 pad/ignore，pad 位参与 loss，梯度被无关 token 污染。
2. mask 方向搞反（有效位标 0），模型只看到 pad 位，性能崩塌。
3. `-inf` 与 fp16 混用导致整行全 `-inf` 时 softmax 产生 nan。

#### 9. 实践与验证

单测：构造长度 3 与 2 的样本，断言 batch shape、mask 求和无损还原 length；把 pad 位人为改成不同 token，断言 loss 不变，证明 ignore 生效。数值上断言 masked 位置概率严格为 0。当前未执行大模型 attention 实验。

#### 10. 评分标准

I007 评分：不合格不知道 collate 存在；合格能写出手工 pad + mask；良好能覆盖 ignore_index、桶排序与 mask 广播；优秀/专家能分析 `-inf`/fp16 数值陷阱并给出分桶与显存取舍。

### I008. Optimizer 参数组和学习率 scheduler 如何避免状态错位？

- 难度：中等级
- 知识域：K05 优化器、训练循环与数值稳定性
- 考察能力：概念边界、适用条件与反例
- 题型：概念解释与辨析
- 面试频率：高频
- 前置知识：optimizer、参数注册、训练 step
- 关联题目：B004、I001、I006
- 事实状态：[已确认]

#### 1. 背景知识

- `optimizer` 由 `param_groups` 组成，每组是一个含 `params` 列表与超参（`lr`、`weight_decay`、`betas` 等）的字典；不同组可设不同超参，常用于对 BN、bias 关闭 weight decay。
- `scheduler` 在构造时持有 optimizer 的引用，`scheduler.step()` 直接改写 `param_groups[i]['lr']`；因此 scheduler 与 optimizer 必须绑定同一个 optimizer 实例。
- `optimizer.state_dict()` 里的状态按参数下标组织，与 `param_groups` 的注册顺序强相关；模块结构或分组变化会导致状态错位。
- 常见调度器语义差异：`StepLR` 按 epoch 衰减，`CosineAnnealingLR` 按周期，`OneCycleLR` 需在每 step 调用，`ReduceLROnPlateau` 依赖指标。

#### 2. 问题分析

错位有三种来源：分组顺序变化、参数被重复/遗漏、调度器调用频率与语义不符。两个不变量：(1) 恢复或重建 optimizer 后，同一参数在 `param_groups` 与 `optimizer.state` 中的下标必须与保存时一致；(2) scheduler 的 `last_epoch` 与实际训练进度一致。边界反例：训练中途冻结某层并从 optimizer 移除其参数，再保存恢复时若重建逻辑不一致，剩余参数的动量会整体前移，导致错位。

#### 3. 具体答案

面试中可以这样回答：

> 我会把 optimizer 构建集中成一个函数，保证分组规则唯一且确定，比如按参数名是否含 `bias`/`norm` 分两组。scheduler 与 optimizer 用同一实例，`scheduler.step()` 在 `optimizer.step()` 之后按正确频率调用。保存和恢复时一起存 `optimizer.state_dict` 与 `scheduler.state_dict`，恢复后断言 `len(param_groups)` 与每组参数数量一致。若中途改过分组，就迁移旧状态或干脆从新边界重训，避免动量错配。

#### 4. 技术洞察

````python
decay, no_decay = [], []
for n, p in model.named_parameters():
    (no_decay if p.ndim <= 1 else decay).append(p)
opt = torch.optim.AdamW([
    {"params": decay, "weight_decay": 0.01},
    {"params": no_decay, "weight_decay": 0.0},
], lr=3e-4)
# 恢复一致性断言
assert len(sd["param_groups"]) == len(opt.param_groups)
for g_new, g_old in zip(opt.param_groups, sd["param_groups"]):
    assert len(g_new["params"]) == len(g_old["params"])
````

#### 5. 图文说明

| 参数 | 组 | lr | wd |
|---|---|---|---|
| weight (ndim≥2) | 0 | 3e-4 | 0.01 |
| bias/norm | 1 | 3e-4 | 0.0 |

错位判据：状态字典里第 i 个参数的 `exp_avg` 形状与当前第 i 个参数不符。

#### 6. 拓展知识

`AdamW` 把 weight decay 与自适应更新解耦，做在参数上而非梯度上，与 L2 正则不同；`ReduceLROnPlateau` 是唯一不在 `step()` 无参调用的主力调度器；混合精度下 `scaler.step` 内部会调用 `optimizer.step`，scheduler 仍在其后调用。

#### 7. 常见追问及回答要点

1. bias/norm 为什么要单独分组关 weight decay？——对一维参数做 L2 会不当收缩，影响收敛。
2. 如何检测错位？——比较 state 中张量形状与当前参数形状，或用命名参数重建。
3. 冻结层的参数要不要留在 optimizer？——不留在 optimizer 更干净，但要注意恢复一致性。

#### 8. 易错点与反例

1. 用 set 或 dict 遍历构造参数列表，顺序在恢复时不同，动量错配。
2. scheduler 绑定了旧 optimizer 实例，更新的是已废弃对象，lr 看似不变。
3. `OneCycleLR` 误按 epoch 调用，周期被压缩，学习率剧烈抖动。

#### 9. 实践与验证

构造带 BN 的网络，打印 `param_groups` 的 `lr/weight_decay` 与每组参数形状；保存再恢复后逐组断言形状与数量一致；模拟一次“改变分组”的操作，观察 loss 是否出现异常台阶以确认错位危害。当前未执行分布式参数分片场景。

#### 10. 评分标准

I008 评分：不合格不清楚 param_groups；合格能正确分组并调用 scheduler；良好能解释 state 与下标绑定、恢复一致性断言；优秀/专家能处理动态分组、给出迁移或重训策略并检测错位。

### I009. DDP 中 unused parameter、梯度累积和通信顺序如何交互？

- 难度：中等级
- 知识域：K07 分布式训练与通信
- 考察能力：概念边界、适用条件与反例
- 题型：概念解释与辨析
- 面试频率：中频
- 前置知识：DDP、autograd、梯度累积
- 关联题目：I004、I002、A001
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- DDP 的 backward 会挂一个 autograd hook，在梯度就绪时把梯度按 bucket 做 AllReduce 再写回 `.grad`；这个 hook 对每个参与参数触发。
- `find_unused_parameters=True` 允许某些参数在某次 forward 中未被使用，DDP 会遍历图找出未用参数并标记为 ready，代价是额外图遍历开销。
- `no_sync()` 上下文会跳过该次 backward 的 AllReduce，用于梯度累积的中间 micro-batch，只在最后一次同步。
- 梯度累积与 DDP 叠加时，若中间 micro-batch 也同步，会造成重复通信且等效批量的梯度尺度与单卡不一致。

#### 2. 问题分析

三个要素交叉：unused 参数、梯度累积、通信时机。两个不变量：(1) 每次 `optimizer.step()` 使用的梯度必须是“本 rank 累积后已被 AllReduce 平均”的全局梯度；(2) 累积的 micro-batch 只允许最后一次触发通信。边界反例：模型分支中某参数只在部分 micro-batch 出现，用 `no_sync` 累积时该参数的 bucket 在前几次不 ready，若 `find_unused_parameters=False` 会挂起等待或报错。

#### 3. 具体答案

面试中可以这样回答：

> DDP 在 backward 时用 autograd hook 把梯度按 bucket AllReduce。做梯度累积时，我在前 K−1 个 micro-batch 用 `with model.no_sync():` 包住 backward，只有最后一个 micro-batch 正常 backward 触发通信。若模型有条件分支导致某些参数某步不用，我打开 `find_unused_parameters=True`，但要清楚它会增加遍历开销，能静态消除分支就消除。通信与累积的配合要让 `optimizer.step()` 只在同步后发生，并保持 loss 按累积步数缩放。

#### 4. 技术洞察

````python
for i, (x, y) in enumerate(loader):
    ctx = model.no_sync() if (i + 1) % K else contextlib.nullcontext()
    with ctx:
        loss = criterion(model(x), y) / K
        loss.backward()
    if (i + 1) % K == 0:
        optimizer.step(); optimizer.zero_grad(set_to_none=True)
# 有未用参数时：
model = DDP(model, device_ids=[rank], find_unused_parameters=True)
````

通信量近似与参数量成正比，bucket 让多个小梯度合并成一次 AllReduce 以摊薄延迟。

#### 5. 图文说明

| micro-batch | no_sync | 通信 | 梯度 |
|---|---|---|---|
| 1..K−1 | 是 | 无 | 本地累积 |
| K | 否 | AllReduce | 全局平均后 step |

#### 6. 拓展知识

`gradient_as_bucket_view=True` 让 `.grad` 直接指向通信 bucket，省一次拷贝；`static_graph=True` 允许 DDP 做更激进的重排与复用，但要求图结构稳定；FSDP 进一步分片参数与梯度，通信模式不同。

#### 7. 常见追问及回答要点

1. 为什么中间 micro-batch 要 `no_sync`？——避免对同一批数据重复通信并保持尺度正确。
2. `find_unused_parameters` 何时必须开？——存在条件分支时，否则会在缺梯度处等待。
3. 梯度累积会改变有效 batch 吗？——会，需按 K 缩放 loss 以匹配单卡等效学习率。

#### 8. 易错点与反例

1. 累积时忘记 `no_sync`，K 次通信同一梯度，吞吐下降且梯度被重复平均。
2. 有未用参数但未开启 find_unused，进程在 hook 处挂起或被 NCCL 超时中止 [待验证]。
3. loss 未除 K，等效学习率随累积步数放大，训练发散。

#### 9. 实践与验证

双卡小模型验证：分别用“累积 K 步 + no_sync”与“单步大 batch”跑一步，比较参数更新是否 `allclose`；用 profiler 观察 AllReduce 次数应等于 step 次数而非 micro-batch 次数。多进程与 NCCL 结果 [待验证]。

#### 10. 评分标准

I009 评分：不合格不知道 hook/no_sync；合格能说出 DDP 在 backward 同步；良好能正确组合 no_sync、累积与 loss 缩放；优秀/专家能分析 unused 参数挂起、bucket 通信与静态图的取舍并给出验证。

### I010. 如何用 profiler 区分数据瓶颈、GPU 瓶颈和同步瓶颈？

- 难度：中等级
- 知识域：K09 性能、显存与算子分析
- 考察能力：概念边界、适用条件与反例
- 题型：概念解释与辨析
- 面试频率：高频
- 前置知识：训练循环、DataLoader、CUDA stream
- 关联题目：B005、B008、I005、A003
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- 训练瓶颈分三类：数据瓶颈（`DataLoader` 取数慢，GPU 空转等待）、计算瓶颈（GPU kernel 排队饱和）、同步瓶颈（频繁 `.item()`、`.cpu()`、`torch.cuda.synchronize()` 造成 pipeline 停顿）。
- `torch.profiler.profile` 能按 CUDA kernel 与 CPU op 分别计时，配合 `record_function` 给自定义段打标签；`schedule` 控制 warmup 与采集窗口。
- `DataLoader` 的 `num_workers>0` 用子进程预取，`pin_memory=True` + `.to(device, non_blocking=True)` 才能与计算重叠 H2D 拷贝。
- 判断信号：GPU 利用率低而 CPU 忙→数据瓶颈；GPU 利用率高且 kernel 密集→计算瓶颈；出现成片 `cudaStreamSynchronize`/`cudaMemcpy` 空洞→同步瓶颈。

#### 2. 问题分析

按时间轴把每个 step 切成三段：取数、H2D、计算，看哪段是空洞。两个不变量：(1) 理想情况下 GPU 在两次 step 之间不应有空闲（数据已预取）；(2) 主机侧同步调用次数应远小于 step 次数。边界反例：小模型 + 大数据时，即便 `num_workers` 足够，若每步都 `.item()` 打印 loss，同步开销也会把 GPU 拖成间歇运行，此时瓶颈是同步而非计算。

#### 3. 具体答案

面试中可以这样回答：

> 我用 `torch.profiler` 采一个窗口，先看 GPU kernel 时间占比。如果 GPU 有明显空洞、同时 dataloader worker 在忙，就是数据瓶颈，措施是增大 `num_workers`、开 `pin_memory` 与非阻塞拷贝、用长度分桶减少 pad。如果 GPU kernel 连续但某类 kernel（如 GEMM）占满，是计算瓶颈，考虑换精度、算子融合或编译。如果看到大量同步 API 且步间抖动大，就是同步瓶颈，把 `.item()`、`.cpu()`、`synchronize` 从热路径移走，累积后再打印。

#### 4. 技术洞察

````python
with torch.profiler.profile(
    activities=[CPU, CUDA],
    schedule=torch.profiler.schedule(wait=1, warmup=1, active=3),
    on_trace_ready=torch.profiler.tensorboard_trace_handler("./log"),
) as prof:
    for i, (x, y) in enumerate(loader):
        train_step(x, y)
        prof.step()
````

空洞判据：`step` 时间分布中出现与 GPU kernel 不对齐的间隙；若间隙长度≈取数时间，定位为数据；若间隙与同步 API 同现，定位为同步。

#### 5. 图文说明

| 现象 | 主导瓶颈 | 首选措施 |
|---|---|---|
| GPU 空转、worker 忙 | 数据 | 增 worker、pin、分桶 |
| kernel 连续饱和 | 计算 | 降精度、融合、编译 |
| 成片同步空洞 | 同步 | 去 `.item()`、延后打印 |

#### 6. 拓展知识

`torch.profiler` 的 `with_flops`、`profile_memory` 可同时给出算力与显存视图；Nsight Systems 能看到更底层的 stream 重叠与 memcpy；显存瓶颈通常表现为 allocator 峰值与碎片，需与吞吐瓶颈分开诊断。

#### 7. 常见追问及回答要点

1. 如何量化“数据瓶颈”？——比较 GPU 活跃时间占比与 dataloader 等待时间，占比低且等待高即数据受限。
2. 为什么 pin_memory 能提速？——页锁定内存使 H2D 可用 DMA 异步拷贝，与计算重叠。
3. profiler 本身的开销怎么办？——用 wait/warmup 跳过前几步，只采短窗口。

#### 8. 易错点与反例

1. 只看 `nvidia-smi` 利用率就下结论，忽略利用率可以被人为拉高但实际在等数据。
2. 每步 `print(loss.item())`，同步使 GPU 间歇，被误判为计算慢。
3. 开很多 worker 但 `persistent_workers=False`，每个 epoch 重建进程，启动开销被计入取数。

#### 9. 实践与验证

对照实验：固定模型，只改 `num_workers`（0/2/4/8）记录 step 时间，若随 worker 增加而下降则确认数据受限；再在热路径加一次 `.item()` 观察 step 时间上升，确认同步代价。GPU 与多进程数字 [待验证]。

#### 10. 评分标准

I010 评分：不合格只会看 GPU 利用率；合格能区分三类瓶颈；良好能用 profiler 证据定位并给出对应措施；优秀/专家能设计对照实验、解释 pin/非阻塞/持久 worker 的原理并量化收益。

### I003. AMP、autocast 和 GradScaler 如何配合，何时会破坏数值正确性？

- 难度：中等级
- 知识域：K06 AMP、Checkpoint 与可复现性
- 考察能力：概念边界、适用条件与反例
- 题型：概念解释与辨析
- 面试频率：高频
- 前置知识：dtype、autograd、训练循环
- 关联题目：I001、I005、A003
- 事实状态：[已确认]

#### 1. 背景知识

- `torch.autocast` 按算子类型选择执行 dtype：矩阵乘、卷积等用 FP16/BF16，而归约、softmax、LayerNorm、损失等对精度敏感的算子仍用 FP32。
- `GradScaler` 只服务 FP16：FP16 动态范围窄，梯度小值易下溢为 0，于是把 loss 乘一个 scale 后再 backward，使梯度落在可表示范围，`step` 前再 `unscale_` 还原。
- 若 `unscale` 后检测到 inf/NaN，scaler 跳过本次 `optimizer.step()` 并把 scale 除以 2；否则 scale 乘增长因子（默认 2 后回退策略由 `growth_interval` 控制）。
- BF16 与 FP32 同为 8 位指数，动态范围足够大，通常不需要 GradScaler，直接 autocast 即可。

#### 2. 问题分析

正确性依赖三个环节：算子分类是否正确、梯度缩放是否可逆、溢出是否被检测并跳过。两个不变量：(1) `scaler.step(optimizer)` 内部先 unscale 再 `optimizer.step`，且未 unscale 前不得做梯度裁剪之外的任何 `.grad` 读取；(2) 任一参数梯度含 inf/NaN 时本次更新整体被跳过。边界反例：手动对已 unscale 的梯度再做一次 `scaler.unscale_` 会二次缩放，梯度整体偏小；或把 loss 的归约算子在 autocast 内被降精度，产生系统性偏差。

#### 3. 具体答案

面试中可以这样回答：

> 我把 forward 与 loss 放进 `with autocast(dtype=torch.float16):`，loss 保持 FP32 标量，然后 `scaler.scale(loss).backward()`。更新时用 `scaler.step(optimizer)`，它内部先 unscale 再 step，如果发现 inf/NaN 就跳过这次并调小 scale；随后 `scaler.update()` 调整 scale。裁剪要在 unscale 之后做，或直接用 scaler 自带的 unscale。若用 BF16，动态范围够大，一般不用 scaler，直接 autocast 即可。数值正确性上，softmax、LayerNorm、loss、归约必须留在 FP32。

#### 4. 技术洞察

````python
scaler = torch.cuda.amp.GradScaler(enabled=use_fp16)
for x, y in loader:
    optimizer.zero_grad(set_to_none=True)
    with torch.autocast("cuda", dtype=torch.float16):
        loss = criterion(model(x), y)          # loss 为 fp32
    scaler.scale(loss).backward()
    scaler.unscale_(optimizer)                  # 还原成真实梯度
    torch.nn.utils.clip_grad_norm_(model.parameters(), 1.0)
    scaler.step(optimizer)                      # 溢出则内部 skip
    scaler.update()
````

scale 更新：`overflow → scale /= 2`；连续 `growth_interval` 次无溢出 → `scale *= growth_factor`。

#### 5. 图文说明

| 环节 | FP16 | BF16 |
|---|---|---|
| autocast | 需要 | 需要 |
| GradScaler | 必须 | 通常不需要 |
| inf/NaN 检测 | 靠 scaler | 手动 clip/检查 |
| 敏感算子 | 强制 FP32 | 强制 FP32 |

#### 6. 拓展知识

FP16 与 BF16 的取舍：FP16 精度高但范围窄，BF16 范围大但尾数少。混合精度下 LayerNorm/RMSNorm 的 variance 若用 FP16 计算会放大误差，通常强制 FP32。`torch.amp` 新 API 把 `device_type` 作为显式参数，替代了早期 `torch.cuda.amp`。

#### 7. 常见追问及回答要点

1. 为什么 loss 要在 autocast 外保持 FP32？——softmax/交叉熵对精度敏感，降精度会让梯度有偏。
2. 梯度裁剪前为什么要 unscale？——裁剪阈值是针对真实梯度尺度的，未还原会把阈值等效缩小 scale 倍。
3. BF16 为什么一般不用 scaler？——指数位与 FP32 相同，动态范围足够，梯度不易下溢。

#### 8. 易错点与反例

1. 在 autocast 内做 `LayerNorm` 却强制 FP16，小方差被截断，训练发散。
2. 忘记 `scaler.update()`，scale 一直不增长，长期处于保守值，收敛变慢。
3. 用 scaler 后又手动 `.grad` 乘 1/scale，造成二次 unscale，梯度偏小。

#### 9. 实践与验证

对照 FP32 与 AMP 的 loss 曲线，前若干步应接近；打印 `scaler.get_scale()` 观察溢出后是否下降、稳定期是否回升；用 `torch.autograd.gradcheck` 在 FP32 autocast 关闭下校验自定义算子数值梯度。GPU 与具体收敛差异 [待验证]。

#### 10. 评分标准

I003 评分：不合格以为 AMP 就是全局转 FP16；合格知道 autocast 按算子选 dtype；良好能正确组合 scaler、unscale、clip、update；优秀/专家能说明 FP16/BF16 取舍、敏感算子白名单与二次缩放陷阱并给出验证。

### I004. DDP 如何保证多进程数据并行的梯度一致？

- 难度：中等级
- 知识域：K07 分布式训练与通信
- 考察能力：概念边界、适用条件与反例
- 题型：概念解释与辨析
- 面试频率：高频
- 前置知识：训练循环、collective、进程和 rank
- 关联题目：B005、A001、E002
- 事实状态：[已确认]

#### 1. 背景知识

- DDP 每个进程持有完整模型副本，处理不同的数据分片，backward 期间通过 AllReduce 把各 rank 梯度求和再除以 world_size，得到与“单进程大 batch”等价的平均梯度。
- 数据一致性的前提是各 rank 用不同但无重叠的数据：通常靠 `DistributedSampler` 给每个 rank 划分互补子集。
- DDP 的梯度通信由 autograd hook 触发，按参数 bucket 组织，bucket 越大通信次数越少但首次就绪越晚，存在通信与重叠的取舍。
- 广播初始化：DDP 构造时从 rank 0 广播参数与 buffer，保证起点一致；`broadcast_buffers` 控制每步是否同步 BN 的 running stats。

#### 2. 问题分析

“梯度一致”有两个层面：数据不重叠、梯度语义等价于单卡大 batch。两个不变量：(1) 各 rank 的参数更新前梯度必须是全局平均而非本地； (2) `DistributedSampler` 必须按 epoch 调 `set_epoch` 才重新打乱并保证跨 rank 互补。边界反例：如果两个 rank 都从 `sampler` 外自行 shuffle 且种子相同，会采样到重叠样本；或者 world_size 增加时未按比例放大 lr，虽然梯度一致但收敛轨迹与单卡不再等价。

#### 3. 具体答案

面试中可以这样回答：

> DDP 保证每个进程从同一份参数初始化，用 `DistributedSampler` 把数据按 rank 切成互不重叠的分片。backward 时 autograd hook 触发梯度按 bucket 的 AllReduce，把各 rank 梯度求和再平均，所以每个 rank 拿到的都是全局平均梯度，更新后参数仍然一致。关键点：每个 epoch 调 `sampler.set_epoch(epoch)` 让各 rank 重新对齐打乱；如果要做等效单卡大 batch，需要按 world_size 缩放学习率；BN 的 running stats 若要全局一致需开 `broadcast_buffers` 或用 SyncBatchNorm。

#### 4. 技术洞察

````python
dist.init_process_group("nccl")
rank, ws = dist.get_rank(), dist.get_world_size()
sampler = DistributedSampler(ds, num_replicas=ws, rank=rank, shuffle=True)
loader = DataLoader(ds, sampler=sampler, batch_size=B)
model = DDP(model.to(rank), device_ids=[rank])
for epoch in range(E):
    sampler.set_epoch(epoch)          # 必须
    for x, y in loader:
        ...
````

等价关系：`g_ddp = (1/W) Σ_r g_r`，对应 batch size `B·W` 的梯度；学习率常按 `lr ∝ B·W` 的经验线性缩放调整。

#### 5. 图文说明

| rank | 样本子集 | 梯度 | 更新后参数 |
|---|---|---|---|
| 0 | 0,2,4… | g0 | 同 |
| 1 | 1,3,5… | g1 | 同 |
| AllReduce | — | (g0+g1)/2 | 一致 |

#### 6. 拓展知识

`find_unused_parameters`、`gradient_as_bucket_view`、`static_graph` 是 DDP 的三个关键开关；`no_sync` 用于累积；FSDP 用参数分片替代副本，显存更省但通信更复杂。梯度整除问题：当数据集不能被 world_size 整除时 `DistributedSampler` 默认补样本（`drop_last` 可选）。

#### 7. 常见追问及回答要点

1. 为什么梯度取平均而非求和？——与单卡大 batch 的均值损失对应，保持梯度尺度。
2. `set_epoch` 为什么必要？——让每个 epoch 的 shuffle 种子在不同 rank 间一致且可复现。
3. BN buffer 需要同步吗？——小 batch 每卡统计方差大，跨卡训练常需 SyncBN。

#### 8. 易错点与反例

1. 忘记 `set_epoch`，每个 epoch 打乱顺序不变，且跨 epoch 样本分布有偏。
2. 各 rank 自行 `random.shuffle` 且种子相同，采样重叠，等效 batch 缩水。
3. 未按 world_size 缩放 lr，梯度虽一致但有效学习率不匹配，收敛变差。

#### 9. 实践与验证

用 2 卡与 1 卡各跑固定一步，比较参数更新：把 2 卡梯度平均后应与 1 卡大 batch 梯度 `allclose`；用 `dist.all_gather` 收集各 rank 的样本索引，断言并集无重叠、各 rank 数量均衡。多进程与 NCCL 结果 [待验证]。

#### 10. 评分标准

I004 评分：不合格不知道 DDP 靠 AllReduce；合格能说出梯度平均与分片数据；良好能处理 set_epoch、lr 缩放与 BN 同步；优秀/专家能给出梯度等价推导、bucket 通信取舍并设计 all_gather 验证。

### I005. 如何分析一个训练任务的显存和吞吐瓶颈？

- 难度：中等级
- 知识域：K09 性能、显存与算子分析
- 考察能力：概念边界、适用条件与反例
- 题型：概念解释与辨析
- 面试频率：高频
- 前置知识：训练循环、Tensor layout、GPU 基础
- 关联题目：B002、B005、A003
- 事实状态：[已确认]

#### 1. 背景知识

- 训练显存由参数、梯度、优化器状态、激活、临时工作区五部分构成；Adam 的优化器状态是参数的两倍（`exp_avg`、`exp_avg_sq`）。
- 激活显存近似与 `batch × seq × hidden × 层数` 成正比，是 batch size 上限的主要约束；激活检查点用重算换显存。
- 吞吐受限于两个上限：显存带宽（memory-bound，如 elementwise、norm）与算力（compute-bound，如 GEMM）；算术强度决定落在哪一侧，即 roofline 模型。
- 关键测量工具：`torch.cuda.max_memory_allocated` 测峰值显存，`torch.profiler` 测 kernel 时间与算力占比。

#### 2. 问题分析

把资源拆成“显存预算”和“吞吐预算”。两个不变量：(1) `peak_allocated ≤ 显存上限`，其中峰值由激活与优化器状态主导；(2) 吞吐上限 `min(算力上限, 带宽上限)` 中至少一个被逼近时说明瓶颈明确。边界反例：显存还有余但吞吐上不去，说明是带宽受限而非容量受限；反之显存打满但 GPU 利用率低，说明碎片或分配器峰值而非计算问题。

#### 3. 具体答案

面试中可以这样回答：

> 显存上我先算静态部分：参数量 ×（1 参数 + 1 梯度 + 2 Adam 状态）×2 字节（FP16 存状态常为 FP32 则 ×4），再加激活。用 `max_memory_allocated` 实测峰值，按层或按阶段定位是激活还是优化器状态占大头。吞吐上我用 profiler 看 kernel 时间占比：如果 elementwise/norm 类占多且算力利用率低，就是带宽受限，措施是算子融合、提算术强度；如果 GEMM 饱和，就是算力受限，考虑降精度、切分或编译。常见手段：梯度检查点、分片优化器（ZeRO/FSDP）、增大 micro-batch 但保持累积、以及去碎片。

#### 4. 技术洞察

````
静态显存 ≈ P·(1 + 1 + 2) · bytes_state        # 参数+梯度+Adam
激活显存 ≈ B·L·H·layers · bytes_act · k        # k 为中间张量系数
roofline: t = max(FLOPs / peak_FLOPS, Bytes / peak_BW)
````

若 `FLOPs/Bytes < peak_FLOPS/peak_BW`，受带宽限制；反之受算力限制。

#### 5. 图文说明

| 分量 | 量级 | 主要措施 |
|---|---|---|
| 参数+梯度 | 2P | 分片、量化 |
| Adam 状态 | 2P（FP32） | ZeRO/8-bit |
| 激活 | ∝ B·L·H | 检查点、micro-batch |
| 临时工作区 | 视算子 | 融合、复用 |

#### 6. 拓展知识

算术强度与 roofline 是把“算得快还是搬得快”量化的工具；`torch.cuda.memory_summary` 能看碎片与 reserved/allocated 差；激活检查点是时间换空间，但重算会降低有效算力，需权衡；FlashAttention 通过分块把 attention 从 O(L²) 显存降到 O(L)。

#### 7. 常见追问及回答要点

1. 怎么判断是带宽还是算力受限？——算算术强度并与硬件 roofline 拐点比较。
2. 显存不够先动哪个？——先减 micro-batch 激活，再上检查点，最后分片优化器状态。
3. 为什么 reserved 远大于 allocated？——分配器保留缓存，碎片或大块分配所致，不一定真占用。

#### 8. 易错点与反例

1. 只按参数量估显存，忽略 Adam 状态与激活，导致 OOM 预判错误。
2. 显存有余就盲目加 batch，激活平方增长触发 OOM。
3. 把 reserved 当真实占用，误以为显存泄漏。

#### 9. 实践与验证

扫描 batch size，记录 `max_memory_allocated` 与 step 时间，绘出“显存-吞吐”曲线找拐点；用 profiler 算带宽利用率与算力利用率，判断主导资源。GPU 具体数值 [待验证]。

#### 10. 评分标准

I005 评分：不合格只报参数量；合格能拆五类显存；良好能用 roofline 区分带宽/算力瓶颈并给出措施；优秀/专家能定量估算激活与优化器状态、解释检查点与分片的时空取舍并用曲线验证。

### I011. 端到端流程：张量/设备

- 难度：中等级
- 知识域：K01 张量/设备
- 考察能力：变量、因果链、复杂度与容量上限
- 题型：原理推导与复杂度
- 面试频率：高频
- 前置知识：shape、stride、dtype、device
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- 一个张量由五元组描述：`storage`、`storage_offset`、`shape`、`stride`、`dtype`/`device`；`view`/`transpose`/`expand` 只改元数据，共享同一块 storage。
- 连续张量满足 `stride[i] = stride[i+1] * shape[i+1]`，`stride[-1] = 1`；`is_contiguous()` 就是在校验这条规则。
- 元素地址公式为 `offset + Σ idx[i] * stride[i]`；`expand` 用 stride 0 复制视图，`as_strided` 可构造任意合法视图。
- `.to(device)`/`.cuda()` 对跨设备是真实拷贝，对同设备同 dtype 是空操作；`pin_memory` + 非阻塞 `.to` 才能与计算重叠 H2D。

#### 2. 问题分析

端到端追踪“创建→布局→搬运→计算→回写”。两个不变量：(1) 任何视图与其基张量共享 storage，写入视图会改变基张量；(2) 只有连续（或满足特定 stride 约束）的张量才能 `view`，否则报错或触发拷贝。边界反例：`x.t().view(-1)` 对非连续张量会报 “view size is not compatible”，必须 `reshape`（内部可能拷贝）或先 `.contiguous()`。

#### 3. 具体答案

面试中可以这样回答：

> 我会从张量的五元组讲起：storage 加 shape/stride/offset 决定逻辑视图，元素地址是 `offset + Σ idx[i]*stride[i]`。流程上分为创建、布局变换、跨设备搬运、计算、回写。`view`/`transpose`/`expand` 都是零拷贝改元数据，`reshape` 在需要时拷贝。跨设备 `.to` 是真实拷贝，用 `non_blocking` 配合 pinned 内存才能和计算重叠。若下游要求连续，我会在边界处显式 `contiguous()`，而不是到处隐式触发拷贝。

#### 4. 技术洞察

```
addr = storage_offset + Σ_i idx[i] * stride[i]
contiguous  ⟺ stride[i] == stride[i+1] * shape[i+1]
```

````python
x = torch.arange(6).view(2, 3)      # stride (3,1)
y = x.t()                            # stride (1,3)，非连续
y.is_contiguous()                    # False
y.reshape(-1)                        # 触发拷贝
y.copy_(y.T) if False else None      # 视图别名演示
````

#### 5. 图文说明

| 阶段 | 是否拷贝 | 关键量 |
|---|---|---|
| view/transpose/expand | 否 | stride、offset |
| reshape（非连续） | 可能 | 连续性判据 |
| .to(device) | 是（跨设备） | H2D/D2D 带宽 |
| contiguous | 是 | 目标 stride |

#### 6. 拓展知识

`channels_last` 改变卷积输入的 stride 以贴合后端；`expand` 的 stride 0 视图在后续写操作处会触发 `unsupported operation`；`as_strided` 会绕过安全检查，越界会静默脏数据，属于高阶陷阱。

#### 7. 常见追问及回答要点

1. view 与 reshape 区别？——view 要求连续，reshape 不满足时内部拷贝。
2. 为什么 transpose 后操作变慢？——非连续访存，cache 命中下降。
3. storage_offset 有什么用？——切片/视图定位基址，解释为何切片也可能非连续。

#### 8. 易错点与反例

1. 对 transpose 结果直接 `view`，报 stride 不兼容。
2. 以为 `expand` 会拷贝，写入其中一行污染所有广播行。
3. 用 `as_strided` 越界读，无报错但数值已脏。

#### 9. 实践与验证

断言实验：对 `x.t()` 断言 `is_contiguous()==False`，`x.t().reshape(-1)` 后与原数据 `allclose`；用 `x.data_ptr()` 验证 view 与基张量地址差等于 `offset*itemsize`。GPU 带宽数值 [待验证]。

#### 10. 评分标准

I011 评分：不合格说不清 stride；合格能讲五元组与拷贝边界；良好能解释连续判据与 view/reshape 差异；优秀/专家能推导地址公式、说明 as_strided 风险并设计断言验证。

### I012. 端到端流程：autograd

- 难度：中等级
- 知识域：K02 autograd
- 考察能力：变量、因果链、复杂度与容量上限
- 题型：原理推导与复杂度
- 面试频率：高频
- 前置知识：requires_grad、leaf、view、detach、反向
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- autograd 反向模式微分的端到端链路：forward 时每个算子把 `grad_fn` 挂到输出上，构建反向图；`backward()` 从标量出发按拓扑逆序调用各 `grad_fn`。
- 只有 `requires_grad=True` 的叶子张量会累积 `.grad`；非叶子中间量默认不保留，`retain_grad()` 或 hook 才保留。
- 梯度沿视图像素散射：`view`/`transpose`/`expand` 的反向是形状还原与求和累加，`expand` 的反向要对广播维求和。
- 图的生命周期由 `backward` 决定：默认消费后释放，`retain_graph=True` 保留以支持多次反向。

#### 2. 问题分析

追踪“forward 建图→loss→backward 遍历→梯度写回→图释放”。两个不变量：(1) 反向图连通性等价于从 loss 到叶子的可达性；(2) 视图像素及其基张量的梯度按地址映射累加，广播维梯度对多次使用求和。边界反例：对同一 forward 结果两次 `backward` 而未 `retain_graph`，第二次报图已被释放。

#### 3. 具体答案

面试中可以这样回答：

> forward 阶段每个对 requires_grad 张量做运算的算子都会生成一个 `grad_fn` 节点，串成反向图。到了 `loss.backward()`，引擎从标量出发做逆拓扑序遍历，调用各节点的 backward，把结果累积到叶子的 `.grad`。视图类算子反向是把上游梯度还原到输入形状，广播的反向要在广播维上求和。默认一次反向之后图就被释放。要多次反向就 `retain_graph=True`，要检查中间量就 `retain_grad` 或注册 hook。

#### 4. 技术洞察

链式法则：`∂L/∂x = Σ (∂L/∂y)·(∂y/∂x)`。视图反向：

```
view(a→b):  dA = dB.reshape(a.shape)
expand:     dA = dB.sum(dim=broadcast_dims, keepdim=True)
transpose:  dA = dB.t()
```

````python
x = torch.randn(2, 3, requires_grad=True)
y = x.expand(4, 2, 3)        # 广播
loss = y.sum()
loss.backward()              # x.grad 形状 (2,3)，广播维累加
````

#### 5. 图文说明

| 阶段 | 产物 | 生命周期 |
|---|---|---|
| forward | grad_fn 图 | 随反向释放 |
| loss | 标量 + 图 | 保留至 backward |
| backward | 叶子 .grad | 累加 |
| 二次反向 | — | 需 retain_graph |

#### 6. 拓展知识

`torch.autograd.Function` 手写反向时要正确 `save_for_backward`，否则中间量被释放；`anomaly mode` 打印前向栈定位 nan；`torch.func`/`vmap` 提供函数式反向，避免全局 `.grad` 状态。

#### 7. 常见追问及回答要点

1. 为什么 expand 的反向要求和？——同一输入元素被多个输出位置使用，梯度按使用次数累加。
2. 图何时释放？——默认 backward 后释放非叶子节点。
3. 如何只求部分梯度？——`torch.autograd.grad(loss, inputs, retain_graph=...)`。

#### 8. 易错点与反例

1. 两次 backward 未 retain_graph，报图已释放。
2. in-place 修改参与梯度的张量，触发版本号不一致 RuntimeError。
3. 对 `.numpy()` 结果参与后续，链条断开，梯度为 None。

#### 9. 实践与验证

最小实验：对 `x.expand` 求导，断言 `x.grad` 等于广播倍数；用 `gradcheck` 对 view/transpose 组合校验数值梯度；打印 `loss.grad_fn` 链验证图结构。无 GPU 依赖。

#### 10. 评分标准

I012 评分：不合格讲不清反向图；合格能说 grad_fn 与叶子梯度；良好能解释视图/广播反向的散射与求和；优秀/专家能给出 retain_graph、hook、gradcheck 的组合定位与验证。

### I013. 端到端流程：Module/状态

- 难度：中等级
- 知识域：K03 Module/状态
- 考察能力：变量、因果链、复杂度与容量上限
- 题型：原理推导与复杂度
- 面试频率：高频
- 前置知识：参数、buffer、模式和 state_dict
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- `nn.Module` 用 `_parameters`、`_buffers`、`_modules` 三张有序字典组织状态；`state_dict()` 把这些展平成带点名的键，buffer 默认也在其中。
- `register_buffer` 注册的张量随 `state_dict` 保存但默认不参与梯度；`requires_grad=True` 的 buffer 会被 autograd 跟踪。
- `train()`/`eval()` 递归切换所有子模块的 `training` 标志，影响 BN running stats 与 Dropout。
- `load_state_dict(strict=True)` 要求键集合完全一致，多键或少键都会报错；`strict=False` 返回缺失/多余键清单。

#### 2. 问题分析

端到端追踪“构造→参数注册→forward→state_dict 序列化→加载”。两个不变量：(1) `state_dict` 的键集合在保存与加载两端必须一致；(2) buffer 与 parameter 的归属决定它们是否进入优化器与梯度路径。边界反例：把应作 buffer 的统计量存成普通 `self.x`，它既不在 `state_dict` 也不随 `.to(device)` 移动，加载后静默丢失。

#### 3. 具体答案

面试中可以这样回答：

> 我先把可学习张量用 `nn.Parameter` 注册到 `_parameters`，把需要随模型搬运和保存、但不训练的统计量用 `register_buffer` 注册。forward 里通过 `self.` 访问子模块，保证它们出现在 `_modules`。保存时 `state_dict` 会同时导出参数和 buffer，加载时用 `load_state_dict` 并关注 `strict` 与 missing/unexpected 键。模式和设备也属于状态：`train()/eval()` 切行为，`.to(device)` 要能覆盖 buffer。

#### 4. 技术洞察

````python
class Net(nn.Module):
    def __init__(self):
        super().__init__()
        self.w = nn.Parameter(torch.empty(3, 3))
        self.register_buffer("running", torch.zeros(3))
    def forward(self, x): return x @ self.w + self.running
sd = net.state_dict()          # keys: w, running
net.load_state_dict(sd, strict=True)
````

#### 5. 图文说明

| 容器 | 内容 | state_dict | 优化器 |
|---|---|---|---|
| _parameters | Parameter | 是 | 是 |
| _buffers | 统计量 | 是（默认） | 否 |
| _modules | 子模块 | 递归展平 | 递归 |

#### 6. 拓展知识

`register_buffer` 的 `persistent=False` 让 buffer 不进 `state_dict`（如缓存的 causal mask）；参数共享/tied weights 要求两处引用同一 `Parameter` 对象；DDP/FSDP 依赖 `named_parameters` 与 `state_dict` 的稳定性做通信与分片。

#### 7. 常见追问及回答要点

1. buffer 与 parameter 区别？——是否进优化器、是否默认参与梯度。
2. `strict=False` 何时用？——微调时键名有差异、需显式处理缺失。
3. 如何避免参数不被注册？——避免用 `list/dict` 包 parameter，改用 `ModuleList/ModuleDict`。

#### 8. 易错点与反例

1. 用普通 Python list 存层，参数不在 `_modules`，优化器漏掉。
2. 训练统计量用 `self.x` 而非 buffer，`.to(device)` 后仍在 CPU。
3. `load_state_dict` 键不匹配只 warning 就继续，权重静默错配。

#### 9. 实践与验证

打印 `named_parameters`/`named_buffers`/`state_dict().keys()` 核对归属；保存后改初始化再加载，断言输出一致；把某 buffer 设为 `persistent=False` 断言它不在 `state_dict`。无 GPU 依赖。

#### 10. 评分标准

I013 评分：不合格混淆 param 与 buffer；合格能讲三容器与 state_dict；良好能处理 strict、persistent、设备迁移；优秀/专家能设计参数共享与键一致性验证并定位静默丢失。

### I014. 端到端流程：数据管道

- 难度：中等级
- 知识域：K04 数据管道
- 考察能力：变量、因果链、复杂度与容量上限
- 题型：原理推导与复杂度
- 面试频率：高频
- 前置知识：Dataset、Sampler、collate、padding、版本
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- 数据管道端到端链路：`Dataset.__getitem__` 取单样本 → `Sampler` 给索引 → `collate_fn` 把一批样本堆成 batch → 主进程从队列取 batch 送入训练。
- `num_workers>0` 时每个 worker 是独立进程，各自持有 Dataset 副本；`worker_init_fn` 用于给每个 worker 设置不同随机种子。
- `pin_memory=True` 把 batch 放进页锁定内存，配合 `non_blocking` 让 H2D 拷贝与计算重叠；`prefetch_factor` 控制每个 worker 预取批数。
- `IterableDataset` 不用 `Sampler`，各 worker 需按 rank/worker 手动切分数据流，否则样本重复。

#### 2. 问题分析

追踪“索引→取数→组批→入队→消费”。两个不变量：(1) 一个 epoch 内所有被消费的样本索引集合等于数据集全集（不重不漏）；(2) 每个 worker 的随机变换可复现且互不相关。边界反例：`IterableDataset` 配 `num_workers>1` 时若不切分，两个 worker 会各自从头读全量，样本被读两遍。

#### 3. 具体答案

面试中可以这样回答：

> 一条 batch 的产生路径是：Sampler 决定索引顺序，`__getitem__` 按索引取单样本，worker 进程做预处理，然后 `collate_fn` 把一批样本拼成张量，主进程从预取队列取出。要保证不重不漏，我就核对一个 epoch 的索引集合。多 worker 时我给每个 worker 设不同种子以复现增强，但索引切分仍由 Sampler 负责。`IterableDataset` 没有 Sampler，必须在迭代器里按 worker id 和 rank 手动分片。最后用 `pin_memory` 和非阻塞拷贝把取数与计算重叠。

#### 4. 技术洞察

````python
def worker_init_fn(wid):
    seed = base_seed + wid
    random.seed(seed); np.random.seed(seed); torch.manual_seed(seed)
loader = DataLoader(ds, batch_size=B, shuffle=True,
                    num_workers=4, pin_memory=True, prefetch_factor=2,
                    persistent_workers=True, worker_init_fn=worker_init_fn)
````

#### 5. 图文说明

| 阶段 | 执行者 | 并行性 |
|---|---|---|
| 索引 | Sampler | 主进程 |
| __getitem__ | worker | 多进程 |
| collate | worker | 多进程 |
| 入队/消费 | 主进程 | 流水线 |

#### 6. 拓展知识

`DistributedSampler` 把索引再按 rank 划分；`persistent_workers` 避免每 epoch 重建进程；`torchdata` 的 `StatefulDataLoader` 提供可恢复的 mid-epoch 状态，是 checkpoint 只存模型之外补充数据位置的方案。

#### 7. 常见追问及回答要点

1. 为什么 worker 里 torch 随机种子要重设？——子进程继承主进程状态，不重设会跨 worker 相关。
2. shuffle=True 与 DistributedSampler 能同用吗？——不能，二者都控制顺序，会冲突。
3. collate 在哪个进程执行？——worker 进程内，主进程只收结果。

#### 8. 易错点与反例

1. `IterableDataset` 多 worker 未分片，样本重复。
2. `worker_init_fn` 缺省，各 worker 增强随机序列相同。
3. `prefetch_factor` 过大，pinned 内存爆掉，OOM。

#### 9. 实践与验证

单测：设 `num_workers=0` 与 `>0` 各跑一个 epoch，收集全部索引断言集合相等；固定 seed 跑两次断言 batch 序列一致；`IterableDataset` 用 worker 数变化的实验断言样本不重复。多进程数字 [待验证]。

#### 10. 评分标准

I014 评分：不合格不清楚 collate 与 worker；合格能描述取数链路；良好能保证不重不漏与 worker 种子复现；优秀/专家能处理 IterableDataset 分片、prefetch/持久 worker 取舍并设计索引集合验证。

### I015. 端到端流程：训练/优化器

- 难度：中等级
- 知识域：K05 训练/优化器
- 考察能力：变量、因果链、复杂度与容量上限
- 题型：原理推导与复杂度
- 面试频率：高频
- 前置知识：forward、backward、累积、optimizer、scheduler
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- 训练/优化器端到端链路：取 batch → `zero_grad` → forward → loss → `backward` → `step` → 可选 clip → `scheduler.step`。
- `backward` 把梯度累加到 `.grad`；`step` 用超参与优化器状态更新 `param.data`；SGD 用动量 buffer，Adam 用 `exp_avg`/`exp_avg_sq`/`step`。
- 梯度裁剪在 `step` 之前、unscale 之后进行，`clip_grad_norm_` 按所有参数梯度的全局范数缩放。
- scheduler 有 step 级与 epoch 级两类语义，调用频率必须与设计一致，否则有效学习率曲线改变。

#### 2. 问题分析

追踪“数据→前向→损失→反向→裁剪→更新→调度”。两个不变量：(1) `step` 之前 `.grad` 必须是完整且经裁剪的梯度；(2) 优化器内部状态（动量/二阶矩）在每个 step 后与参数同步推进。边界反例：梯度累积时若在累积途中做裁剪，会把中间 micro-batch 的局部范数当全局范数，裁剪阈值失真。

#### 3. 具体答案

面试中可以这样回答：

> 每个训练步我先清梯度，forward 算 loss，backward 得到梯度，必要时按全局范数裁剪，再 `optimizer.step()` 更新参数，最后按语义调 `scheduler.step()`。裁剪要放在 unscale 之后、step 之前，且梯度累积时只在真正 step 的那一次裁剪。我会监控梯度范数和学习率两条曲线，前者突变往往意味着 loss spike 或数据异常，后者决定收敛速度。优化器状态和参数要一起 checkpoint，保证恢复等价。

#### 4. 技术洞察

```
g = ∂L/∂θ
g_clip = g · min(1, C / (‖g‖_2 + ε))       # clip_grad_norm_
θ ← θ − lr · Optimizer(g_clip, state)
```

````python
optimizer.zero_grad(set_to_none=True)
loss.backward()
torch.nn.utils.clip_grad_norm_(model.parameters(), max_norm=C)
optimizer.step(); scheduler.step()
````

#### 5. 图文说明

| 步 | 状态 | 观测 |
|---|---|---|
| backward | .grad 累积 | grad norm |
| clip | 全局缩放 | 触发次数 |
| step | 参数+动量 | 参数范数 |
| scheduler | lr | lr 曲线 |

#### 6. 拓展知识

`AdamW` 解耦 weight decay；loss scaling 与 clip 的先后必须是 unscale→clip；梯度噪声、loss spike 常用 skip-step 机制缓解，与 scaler 跳过溢出的机制不同但可叠加。

#### 7. 常见追问及回答要点

1. clip 放在哪？——unscale 后、step 前。
2. 为什么梯度累积时不逐步裁剪？——局部范数与全局范数不等价，阈值失真。
3. 如何判断优化器异常？——看动量/二阶矩是否存在 nan 或异常量级。

#### 8. 易错点与反例

1. 先 clip 后 unscale，阈值被 scale 放大，实际裁剪失效。
2. scheduler 每 batch 调一次而设计是每 epoch，lr 提前衰减。
3. checkpoint 漏存优化器状态，恢复后动量归零，loss 台阶。

#### 9. 实践与验证

记录每步 grad norm、lr、loss；对比有无 clip 的曲线；对累积 K 步与单步大 batch 的更新做 `allclose`；恢复训练后比较后续 loss 与不中断基线。GPU 数字 [待验证]。

#### 10. 评分标准

I015 评分：不合格只知 forward/backward；合格能列出完整步序；良好能处理 clip 时机与 scheduler 频率；优秀/专家能说明累积下裁剪的失真、状态一致性与曲线验证。

### I016. 端到端流程：AMP/恢复

- 难度：中等级
- 知识域：K06 AMP/恢复
- 考察能力：变量、因果链、复杂度与容量上限
- 题型：原理推导与复杂度
- 面试频率：高频
- 前置知识：autocast、scaler、快照、随机状态
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- AMP/恢复端到端链路：`autocast` 控算子精度 → `scaler.scale(loss)` → `backward` → `scaler.unscale_` → 可选 clip → `scaler.step` → `scaler.update`。
- GradScaler 维护 `scale` 因子与 `growth_tracker`，溢出时 scale 减半并记一次跳过，连续无溢出则按 `growth_interval` 增长。
- 可恢复性要求把 scaler 状态与 RNG 状态一并写进 checkpoint，否则恢复后 scale 与随机性重置。
- 恢复点语义：若中断在 unscale 与 step 之间，梯度已被还原，严格等价需要回滚到上一个完整 step 边界。

#### 2. 问题分析

追踪“精度选择→缩放→反向→还原→更新→快照”。两个不变量：(1) 未经 unscale 的真实梯度不得进入裁剪之外的任何读取； (2) 快照必须覆盖 scaler 与 RNG，使 scale 与随机序列连续。边界反例：中断后从 checkpoint 恢复但不恢复 scaler，scale 回到初值 65536，接下来若恰好溢出，行为与中断前不一致。

#### 3. 具体答案

面试中可以这样回答：

> 一步 AMP 训练是：在 `autocast` 内跑 forward 和 loss，`scaler.scale(loss).backward()`，`scaler.unscale_(optimizer)` 还原梯度，做 clip，`scaler.step(optimizer)` 内部检测 inf/NaN 并决定是否更新，最后 `scaler.update()` 调整 scale。要做可恢复 checkpoint，我把 model、optimizer、scheduler、scaler、RNG 和进度计数一起存。恢复时逐项加载，保证 scale 与随机序列连续。对中断点我会定义清楚是回滚到上一个 step 边界还是把梯度也存下来。

#### 4. 技术洞察

````python
ckpt = {"model":..., "optim":..., "sched":...,
        "scaler": scaler.state_dict(),
        "rng_cpu": torch.get_rng_state(),
        "rng_cuda": torch.cuda.get_rng_state_all(),
        "step": gstep}
# restore: 依次 load_state_dict / set_rng_state，断言 scale 一致
````

溢出判据：`scaler.step` 内检查 `found_inf`，为真则跳过并 `scale /= 2`。

#### 5. 图文说明

| 阶段 | 精度/状态 | 快照字段 |
|---|---|---|
| autocast | fp16/bf16 | dtype 配置 |
| scale | scale 因子 | scaler scale |
| unscale | fp32 梯度 | — |
| step/update | 参数+scale | scaler + step |

#### 6. 拓展知识

`torch.amp.GradScaler(device_type=...)` 统一接口；8-bit 优化器状态可进一步省显存但影响可恢复性；`weights_only=True` 加载更安全但需确认 scaler 状态可反序列化。

#### 7. 常见追问及回答要点

1. 为什么 scaler 状态也要存？——scale 是训练状态，缺它恢复后溢出行为不一致。
2. RNG 不恢复会怎样？——dropout/数据增强序列重置，复现失败。
3. 中断在 step 中间怎么办？——回滚到 step 边界或存梯度，二者选一并写清语义。

#### 8. 易错点与反例

1. 只存 model+optim，漏 scaler，恢复后 scale 重置。
2. 存了 RNG 但恢复顺序错误（先加载模型再设 RNG 覆盖），随机性错位。
3. 恢复后重复执行了崩溃前已完成的 step，数据被消费两次。

#### 9. 实践与验证

跑 N 步存 checkpoint，再续跑记录 loss 与 scale 序列；从 N 恢复比较两序列是否在容差内一致（严格相等需固定 determinism [待验证]）；打印恢复前后 `scaler.get_scale()` 断言相等。

#### 10. 评分标准

I016 评分：不合格不知道 scaler 状态要存；合格能列出 AMP 步序；良好能覆盖 scaler+RNG 恢复与中断语义；优秀/专家能定义 step 边界、处理溢出跳过并设计序列一致性验证。

### I017. 端到端流程：分布式

- 难度：中等级
- 知识域：K07 分布式
- 考察能力：变量、因果链、复杂度与容量上限
- 题型：原理推导与复杂度
- 面试频率：高频
- 前置知识：rank、DDP、FSDP、collective、容错
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- 分布式端到端链路：`init_process_group` → 建通信组 → 广播初始参数 → 分片数据 → forward → backward 中 AllReduce → step → 周期 checkpoint 与容错。
- 后端选择：NCCL 用于 GPU 集合通信，gloo 用于 CPU；`world_size`/`rank`/`local_rank` 定义进程拓扑。
- DDP 副本 + AllReduce 平均梯度；FSDP 分片参数与梯度，前向按需 all-gather、反向 reduce-scatter，显存随 world_size 下降。
- 容错现状：原生 `torchrun` 可重启进程，但 DDP 不自动容错，需外部 orchestrator + checkpoint 恢复。

#### 2. 问题分析

追踪“初始化→广播→分片→计算→通信→更新→快照”。两个不变量：(1) 所有 rank 在每步参与同一次集合通信，顺序与数量一致；(2) 参数与优化器状态在任意 rank 上都能从 checkpoint 恢复并与其他 rank 对齐。边界反例：某 rank 因数据分支跳过了一次 forward，导致它少发一次 AllReduce，其余 rank 在集合通信处永久阻塞，直到超时。

#### 3. 具体答案

面试中可以这样回答：

> 端到端我先 `init_process_group` 用 NCCL，拿到 rank/world_size，DDP 构造时从 rank 0 广播参数保证起点一致。用 DistributedSampler 分片数据。forward 各 rank 独立，backward 时 DDP 用 autograd hook 做 AllReduce 平均梯度，更新后参数仍一致。容错上我会周期存分布式 checkpoint，进程崩溃由外部拉起后从最近 checkpoint 恢复，并保证集合通信调用顺序在所有 rank 上严格一致。FSDP 场景则是分片参数、前向 all-gather、反向 reduce-scatter。

#### 4. 技术洞察

````python
dist.init_process_group("nccl")
r, W = dist.get_rank(), dist.get_world_size()
torch.cuda.set_device(r)
m = DDP(model.to(r), device_ids=[r])
sampler.set_epoch(epoch)
if dist.get_rank() == 0: dist.checkpoint.save(...)   # 或 DCP 聚合
````

通信恒等式：DDP 每步 AllReduce 次数与参数 bucket 数一致，且所有 rank 相同。

#### 5. 图文说明

| 阶段 | 通信 | 一致性要求 |
|---|---|---|
| init | barrier | 全 rank 到齐 |
| 广播 | broadcast | 起点参数相同 |
| backward | AllReduce | 调用顺序一致 |
| checkpoint | gather/scatter | 分片对齐 |

#### 6. 拓展知识

`torch.distributed.checkpoint`（DCP）支持分片保存与 re-shard；弹性训练（`torchrun --max_restarts`）配合 rendezvous 实现成员变更；FSDP 的 `FULL_SHARD`/`SHARD_GRAD_OP`/`HYBRID_SHARD` 在通信与显存间取舍。

#### 7. 常见追问及回答要点

1. 为什么集合通信顺序必须一致？——NCCL 依赖所有 rank 发出匹配的 collective，否则死锁。
2. FSDP 与 DDP 显存差异？——FSDP 分片参数/梯度/优化器状态，显存随卡数下降。
3. 崩溃后如何保证一致性？——外部重启 + 最近 checkpoint，恢复后重新同步。

#### 8. 易错点与反例

1. 条件分支导致某些 rank 跳过集合通信，其余 rank 挂起直至超时。
2. checkpoint 只存 rank 0 的副本，FSDP 分片参数丢失。
3. 恢复后 world_size 改变未 re-shard，分片错位。

#### 9. 实践与验证

2 卡跑固定步数，`all_gather` 参数并断言各 rank 一致；杀死一个进程观察其余 rank 是否超时（验证容错缺口）；用 DCP 存再换 world_size 加载，断言能 re-shard [待验证]。

#### 10. 评分标准

I017 评分：不合格不清楚集合通信；合格能描述 DDP 链路；良好能处理 set_epoch、集合通信顺序与 checkpoint；优秀/专家能设计 FSDP 分片与弹性恢复方案并验证一致性。

### I018. 端到端流程：编译栈

- 难度：中等级
- 知识域：K08 编译栈
- 考察能力：变量、因果链、复杂度与容量上限
- 题型：原理推导与复杂度
- 面试频率：高频
- 前置知识：Dynamo、AOTAutograd、Inductor、guard、缓存
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- `torch.compile` 链路：TorchDynamo 抓取字节码生成 FX 图 → 插入 guard 校验假设 → AOTAutograd 生成前向/反向 → Inductor 生成 Triton/C++ kernel → 缓存编译产物。
- Dynamo 遇到不支持的算子会 graph break，回退到 eager 执行，并把图切成多段；guard 失败（形状、类型、设备变化）会触发重新编译。
- `dynamic=True` 或有形状变化时进入动态 shape 分支，用符号 shape 泛化，避免反复重编译。
- `mode` 参数控制优化强度：`default`、`reduce-overhead`（用 CUDA graphs）、`max-autotune`（自动调参）。

#### 2. 问题分析

追踪“字节码捕获→建图→守卫→后端 codegen→缓存命中→执行”。两个不变量：(1) 一次编译产物的 guard 必须覆盖运行时的所有输入属性，否则必须重编译或回退 eager；(2) graph break 前后段的语义与 eager 完全等价。边界反例：仅改变一个整型超参（如输入 batch size）就触发 guard 失败，导致每步重编译，性能比 eager 还差。

#### 3. 具体答案

面试中可以这样回答：

> `torch.compile` 的端到端是：Dynamo 先跑一遍字节码，把可追踪的算子抓成 FX 图，并记录 guard，比如 dtype、形状、设备。AOTAutograd 把图拆成前向和反向，再交给 Inductor 生成 Triton kernel，结果缓存起来。运行时先过 guard，命中就直接跑编译产物，失败就重编译或回退 eager。我关注 graph break 和重编译：graph break 会把图切碎，损失融合机会；频繁重编译说明输入属性不稳定，需要用动态 shape 或固定形状。backend 和 mode 决定优化强度与 CUDA graphs 的使用。

#### 4. 技术洞察

```
compile(x) → guard(predicates) ?
              命中 → cached graph
              失败 → recompile / eager fallback
graph breaks: 每段独立编译，跨段无融合
```

````python
import torch._dynamo as dyn
f = torch.compile(model, mode="reduce-overhead", dynamic=False)
dyn.explain(f, (args,))     # 查看 graph break 与 guard
````

#### 5. 图文说明

| 阶段 | 产出 | 失败模式 |
|---|---|---|
| Dynamo | FX 图 + guard | graph break |
| AOTAutograd | fwd/bwd 图 | 不可追踪算子 |
| Inductor | kernel | 不支持 op |
| 缓存 | 编译产物 | guard miss 重编译 |

#### 6. 拓展知识

CUDA graphs 在 `reduce-overhead` 下要求静态形状与静态内存地址，对动态控制流不友好；`torch._dynamo.config` 可调 cache 大小与重编译阈值；Inductor 的 `max-autotune` 首次编译耗时大但运行期快。

#### 7. 常见追问及回答要点

1. graph break 为什么有害？——图被切碎，跨段无法融合，且每段有调度开销。
2. 为什么反复重编译？——guard 覆盖的属性（形状/类型）在运行时变化。
3. 动态 shape 的代价？——符号化泛化减少重编译，但生成 kernel 可能不如特化版本快。

#### 8. 易错点与反例

1. 训练中 batch size 每步变化，触发大量重编译，整体慢于 eager。
2. `.item()`/numpy 调用造成 graph break，编译收益被切碎。
3. 依赖全局 Python 状态，guard 漏检，产生错误结果。

#### 9. 实践与验证

用 `torch._dynamo.explain` 统计 graph break 与 guard 数量；固定输入重复调用，打印重编译计数应为 0；对比 eager 与 compile 的 step 时间与内存。GPU 数字 [待验证]。

#### 10. 评分标准

I018 评分：不合格只知 compile 一键加速；合格能说出 Dynamo/Inductor 两级；良好能解释 guard、graph break 与重编译；优秀/专家能给出动态 shape 与 CUDA graphs 的取舍并用 explain 定位退化。

### I019. 端到端流程：性能

- 难度：中等级
- 知识域：K09 性能
- 考察能力：变量、因果链、复杂度与容量上限
- 题型：原理推导与复杂度
- 面试频率：高频
- 前置知识：关键路径、带宽、算力、通信、显存、统计
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- 性能端到端关注每个训练 step 的时间如何分配给取数、H2D 拷贝、前向、反向、通信、更新。
- 关键路径是串行链中最长的一段；非关键路径可与关键路径重叠（如预取与计算重叠）。
- 三个硬件上限：显存带宽、算力、互联带宽；任一被逼近即成为该阶段瓶颈。
- 统计上要看稳态分布的均值与尾延迟（P50/P95/P99），而非单次运行，避免把首次编译或冷启动当作稳态。

#### 2. 问题分析

追踪“取数→拷贝→forward→backward→通信→step”的时间条带。两个不变量：(1) 各阶段时间之和加重叠修正等于 step 时间；(2) 关键路径长度决定吞吐上限。边界反例：数据在后台被 worker 预取并与计算重叠，此时即使 `__getitem__` 很慢，只要预取队列不空，它就不在关键路径上，不能简单按 CPU 时间判定为瓶颈。

#### 3. 具体答案

面试中可以这样回答：

> 我用 profiler 把 step 拆成取数、H2D、forward、backward、通信、更新六段，看哪段落在关键路径。判据是：如果 GPU 出现空洞且与取数时间吻合，是数据；如果 backward 里通信占比高，是互联；如果某类 kernel 占满 GPU，是算力。我会用稳态若干步的均值加尾延迟，而不是单步。优化顺序是先消除重叠缺失（pin + 非阻塞 + 预取），再降精度或融合算子，最后才考虑算法级改动。

#### 4. 技术洞察

```
T_step = max(t_data_prefetch, t_h2d) + t_fwd + t_bwd + t_opt   # 含重叠
throughput = batch_size / T_step
关键路径：argmax 的实际串行链
```

#### 5. 图文说明

| 段 | 资源 | 重叠对象 |
|---|---|---|
| 取数 | CPU/IO | 上一 step 计算 |
| H2D | 拷贝引擎 | 计算 |
| fwd/bwd | GPU | 预取 |
| 通信 | 互联 | 反向计算 |

#### 6. 拓展知识

roofline 把算子分为带宽/算力受限；Nsight 能看 stream 级重叠；尾延迟常由 checkpoint 写盘、allreduce 抖动或数据长尾造成，均值会掩盖它。

#### 7. 常见追问及回答要点

1. 为什么用尾延迟？——均值掩盖偶发停顿，长尾影响 step 抖动与扩展性。
2. 如何判断重叠是否发生？——观察 GPU 时间轴是否有被预取/拷贝填充的空洞。
3. 优化优先级？——先补重叠，再降精度/融合，最后算法。

#### 8. 易错点与反例

1. 用单步时间下结论，把冷启动编译算进稳态。
2. CPU 取数慢但与计算重叠良好，仍被误判为瓶颈。
3. 忽略尾延迟，稳态均值达标但周期性卡顿。

#### 9. 实践与验证

固定模型扫描 batch 与 worker 数，记录 step 的 P50/P95；对照理论 `batch/T_step`；用 trace 检查重叠。GPU 与互联数字 [待验证]。

#### 10. 评分标准

I019 评分：不合格只看总时间；合格能拆阶段；良好能用重叠与尾延迟定位关键路径；优秀/专家能建立吞吐模型并给出分阶段优化优先级与验证。

### I020. 端到端流程：导出部署

- 难度：中等级
- 知识域：K10 导出部署
- 考察能力：变量、因果链、复杂度与容量上限
- 题型：原理推导与复杂度
- 面试频率：高频
- 前置知识：state_dict、Export、runtime、schema、迁移
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- 导出部署链路：训练得到 `state_dict` → `model.eval()` → 导出计算图（`torch.export`/ONNX/TorchScript）→ 运行时加载 → 推理。
- `torch.export` 基于 Dynamo 捕获完整可序列化图，产物含参数与图结构；ONNX 是跨框架的算子图标准，带 opset 版本与动态轴声明。
- `state_dict` 只含张量，不绑定代码；加载需要与导出时完全一致的结构定义。
- 导出要求消除训练专属行为：`.eval()`、`no_grad`、去除 dropout 随机性与训练分支。

#### 2. 问题分析

追踪“训练态→导出图→序列化→运行时→推理”。两个不变量：(1) 导出图在给定输入下的输出与原 eager 模型在容差内一致；(2) 图结构与参数绑定稳定，能跨进程/跨版本加载。边界反例：导出时输入是静态 shape，运行时来了不同长度序列，若未声明动态轴，图会报形状不匹配或结果错误。

#### 3. 具体答案

面试中可以这样回答：

> 导出前我把模型切到 `eval()` 并包在 `no_grad` 里，确认没有训练专属分支和随机算子。然后用 `torch.export` 捕获图，或导 ONNX 并声明动态轴，比如 batch 和序列长度。产物里既有图也有参数，加载到运行时直接推理。我会对比导出前后在同一输入上的输出，确认数值一致，再测不同 shape 的泛化。迁移时关注 opset/算子支持，不支持的算子要替换或回退。

#### 4. 技术洞察

````python
model.eval()
ep = torch.export.export(model, (example_input,))
torch.export.save(ep, "m.pt2")
torch.onnx.export(model, example_input, "m.onnx",
                  dynamic_axes={"input": {0: "batch", 1: "seq"}},
                  opset_version=17)
````

#### 5. 图文说明

| 阶段 | 产物 | 风险 |
|---|---|---|
| eval + no_grad | 推理态模型 | 漏切 train 分支 |
| 捕获图 | ExportedProgram/ONNX | 不支持算子 |
| 动态轴 | 符号维度 | 静态 shape 泛化差 |
| runtime | 推理引擎 | 精度/版本差异 |

#### 6. 拓展知识

`torch.export` 强调“可序列化 + 无 Python 副作用”，比 TorchScript trace 更严格；TensorRT/ONNX Runtime 会做算子融合与精度校准；导出图的数值差异多来自 fp16 量化与融合顺序。

#### 7. 常见追问及回答要点

1. 为什么导出前必须 eval？——否则 dropout/BN 训练行为被固化。
2. 动态轴怎么声明？——给输入维度命名，运行时可变。
3. 导出与 state_dict 关系？——导出图内已含参数，state_dict 是另一种只含张量的迁移方式。

#### 8. 易错点与反例

1. 导出时忘记 eval，dropout 随机性进入图，推理不稳定。
2. 未声明动态轴，换 batch size 即报错。
3. 导出图输出与 eager 不一致却未做数值对比，上线才暴露。

#### 9. 实践与验证

同一输入对比 eager 与导出图输出 `assert_close`；用两个不同 shape 的输入测动态轴；在目标 runtime 上跑一遍比对。运行时与精度差异 [待验证]。

#### 10. 评分标准

I020 评分：不合格只知 save/load；合格能导出图并 eval；良好能处理动态轴与数值一致性；优秀/专家能分析算子支持、量化与跨运行时迁移并设计对比验证。

### I021. 端到端流程：自定义算子

- 难度：中等级
- 知识域：K11 自定义算子
- 考察能力：事件顺序、状态迁移、所有权与观测点
- 题型：流程/调用链/数据流
- 面试频率：高频
- 前置知识：schema、dispatcher、stream、autograd、ABI
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- 自定义算子端到端链路：Python/C++ 调用 → dispatcher 按 schema 与 backend 分派 → 落到注册的 kernel（CPU/CUDA/自定义）→ 执行 → 反向由 `autograd.Function` 或 `torch.library` 注册的 backward 提供。
- `torch.library` 用 schema 声明算子签名，包括参数别名与 mutable 标注，例如 `(Tensor(a!) out, Tensor a)`，这决定功能化与内存规划。
- CUDA kernel 必须在当前 stream 上执行，用 `at::cuda::getCurrentCUDAStream()`，否则与其它算子乱序。
- `torch.autograd.Function` 的 `forward`/`backward` 必须成对，`save_for_backward` 保存反向所需张量。

#### 2. 问题分析

追踪“调用→分派→kernel→stream→反向注册”。两个不变量：(1) schema 的别名/mutable 标注必须与 kernel 实际读写一致，否则功能化与自动求导误判； (2) kernel 在当前 stream 上执行，且不与其它算子产生隐式同步。边界反例：out 参数声明为 pure 但 kernel 原地写入，AOTAutograd 功能化时会漏拷贝，导致输入被意外修改。

#### 3. 具体答案

面试中可以这样回答：

> 我会先用 `torch.library` 或 `TORCH_LIBRARY` 定义 schema，明确哪些是输入、哪些是 out 参数、哪些会被原地修改，别名标注要如实写。dispatcher 会按设备和 dispatch key 把调用路由到 CPU/CUDA/autograd kernel。CUDA 实现里我用当前 stream，避免引入同步，并在需要时用 stream 同步或事件。反向我会用 `autograd.Function` 或注册 backward，确保梯度公式正确、`save_for_backward` 保存必要中间量。最后用 gradcheck 与 eager 参考实现对比。

#### 4. 技术洞察

```
call: python → dispatcher(schema, key) → kernel(device)
schema: aten::myop(Tensor a, Tensor(b!) out) -> ()
stream: at::cuda::getCurrentCUDAStream()
grad:  autograd.Function.forward / backward
```

````python
@torch.library.custom_op("mylib::scale", mutates_args=())
def scale(x: Tensor, k: float) -> Tensor:
    return _scale_cuda(x, k)
@scale.register_fake
def _(x, k): return torch.empty_like(x)
````

#### 5. 图文说明

| 阶段 | 组件 | 关键约束 |
|---|---|---|
| 调用 | dispatcher | schema 匹配 |
| 分派 | dispatch key | 设备/autograd |
| kernel | CUDA | 当前 stream |
| 反向 | autograd | 公式与保存量 |

#### 6. 拓展知识

`register_fake`/`Meta` 用于形状推导与编译；功能化（functionalization）把原地操作改写成纯函数以支持编译与自动微分；ABI 兼容性要求扩展与 PyTorch 主版本匹配，否则加载失败。

#### 7. 常见追问及回答要点

1. schema 别名标注作用？——决定功能化、内存复用与自动微分是否正确。
2. 为什么必须用当前 stream？——否则与其它 kernel 乱序，出现数据竞争。
3. 反向如何校验？——gradcheck 与参考实现的数值梯度对比。

#### 8. 易错点与反例

1. out 参数标成纯函数但原地写入，输入被偷偷改写。
2. kernel 用默认 stream 而非当前 stream，与相邻算子乱序。
3. 忘注册 fake/meta，编译时形状推导失败，编译回退。

#### 9. 实践与验证

用 `gradcheck` 校验反向；对比自定义算子与 eager 参考实现输出 `assert_close`；在不同 stream 下运行并断言无数据竞争（用同步后比对）。GPU 结果 [待验证]。

#### 10. 评分标准

I021 评分：不合格不知道 dispatcher；合格能定义 schema 与 kernel；良好能正确标注别名、使用当前 stream、注册反向；优秀/专家能分析功能化与 ABI 约束并用 gradcheck 与参考实现验证。

### I022. 端到端流程：测试治理

- 难度：中等级
- 知识域：K12 测试治理
- 考察能力：事件顺序、状态迁移、所有权与观测点
- 题型：流程/调用链/数据流
- 面试频率：高频
- 前置知识：oracle、证据、日志、回滚、门禁、复盘
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- 测试治理端到端链路：定义 oracle → 构造输入 → 执行 → 比对 → 生成证据 → 门禁判定 → 失败回滚 → 复盘登记。
- oracle 有三种：解析真值、对照实现（reference）、性质/不变量断言；缺少 oracle 的测试只是“跑得通”而非“正确”。
- 门禁（gate）把测试结果与准入绑定，失败即阻断合并或发布；回滚需要可复现的失败用例与最小复现。
- 证据包括随机种子、版本、环境指纹与日志，缺一不可复现。

#### 2. 问题分析

追踪“用例→执行→判定→门禁→回滚→复盘”。两个不变量：(1) 每个测试必须绑定明确 oracle 与失败判据；(2) 失败必须可复现，即种子与环境被记录。边界反例：一个只断言“不抛异常”的测试，在数值完全错误时仍通过，属于假阳性，无法作为门禁。

#### 3. 具体答案

面试中可以这样回答：

> 我的测试治理链路是：先为每个用例定义 oracle，可以是解析解、参考实现或性质断言，绝不停留在“不报错”。执行时固定随机种子并记录环境指纹，产出可复现证据。把关键用例接入门禁，失败即阻断合并。线上或回归失败时先止损回滚，再用最小复现定位，最后把根因与防护登记进复盘的登记表，形成回归用例。测试用例本身也要版本化，避免漂移。

#### 4. 技术洞察

````python
def test_softmax_props():
    x = torch.randn(64, 100, generator=g)     # 固定 seed
    y = softmax(x)
    assert torch.allclose(y.sum(-1), torch.ones(64), atol=1e-6)
    assert (y >= 0).all()
    # reference
    ref = x.exp() / x.exp().sum(-1, keepdim=True)
    assert torch.allclose(y, ref, atol=1e-6)
````

#### 5. 图文说明

| 阶段 | 产物 | 失败处置 |
|---|---|---|
| 定义 oracle | 判据 | 无 oracle 则无效 |
| 执行 | 日志+种子 | 不可复现则阻断 |
| 门禁 | pass/fail | 失败阻断合并 |
| 复盘 | 回归用例 | 防复发 |

#### 6. 拓展知识

`torch.testing.assert_close` 提供 dtype 感知的容差；property-based testing 用随机输入覆盖边界；flaky test 治理靠隔离随机源与稳定环境；golden file 对比适合生成任务但需版本管理。

#### 7. 常见追问及回答要点

1. 没有 oracle 怎么办？——用参考实现或性质断言替代，至少要有数值判据。
2. 如何防 flaky？——固定种子、隔离全局状态、去掉时间/顺序依赖。
3. 门禁如何演进？——从冒烟到关键路径到全量，分级阻断。

#### 8. 易错点与反例

1. 只断言不抛异常，数值错了也通过。
2. 依赖全局随机种子未隔离，测试相互影响而 flaky。
3. 失败用例未登记为回归，同一 bug 反复出现。

#### 9. 实践与验证

为关键算子建立解析解与参考实现双 oracle；注入一个已知 bug 断言测试能捕获；连续跑 100 次断言结果稳定（无 flaky）。无 GPU 依赖的用例可本地执行 [待验证]。

#### 10. 评分标准

I022 评分：不合格只知道“跑测试”；合格能定义 oracle 与判据；良好能设计可复现证据与门禁分级；优秀/专家能注入缺陷验证测试有效性、治理 flaky 并建立回归登记闭环。

### I023. 配置约束：张量/设备

- 难度：中等级
- 知识域：K01 张量/设备
- 考察能力：事件顺序、状态迁移、所有权与观测点
- 题型：流程/调用链/数据流
- 面试频率：高频
- 前置知识：shape、stride、dtype、device
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- 张量运算的前置条件包括：参与逐元素运算的张量 shape 可广播、dtype 一致（或可安全提升）、device 相同；违反会抛 `RuntimeError`。
- `torch.add` 等允许 dtype 隐式提升（type promotion），但 `torch.mm`/`matmul` 要求 dtype 匹配的矩阵，混用会报错。
- 广播规则：从尾部对齐，维度为 1 或相等才可广播，否则报 “shapes cannot be broadcast”。
- 断言范式：用 `assert x.device == y.device and x.dtype == y.dtype` 做前置校验，或用 `torch.testing.assert_close` 做后置校验。

#### 2. 问题分析

为张量配置定义前置/后置条件。两个不变量：(1) 逐元素算子要求 shape 广播兼容、dtype 与 device 一致；(2) 视图操作要求满足连续或特定 stride 约束。边界反例：`x` 在 CPU、`y` 在 CUDA 上直接相加会报 device 不匹配；但 `x + y_cpu_scalar` 中 0 维 CPU 张量却能参与，这是 type promotion 的边界，不能一概认为跨设备必错。

#### 3. 具体答案

面试中可以这样回答：

> 我会在每个入口写清楚前置条件：shape 是否可广播、dtype 是否一致、device 是否相同、视图是否连续。然后用断言在开发期捕获违规，比如断言 device 相等、dtype 相等、shape 尾部对齐。后置条件断言输出 dtype/device/shape 符合预期。对 type promotion 和 0 维张量的边界要特别小心，不能把“跨设备必报错”当铁律。错误处理上，把设备搬运集中在边界层做，避免深层算子到处 `.to`。

#### 4. 技术洞察

```
broadcast: 从尾对齐，dim==1 或相等
pre:  x.dtype == y.dtype and x.device == y.device
post: out.shape == broadcast(x.shape, y.shape)
```

````python
def add_checked(a, b):
    assert a.device == b.device, f"device mismatch {a.device} vs {b.device}"
    assert a.dtype == b.dtype, f"dtype mismatch {a.dtype} vs {b.dtype}"
    return a + b
````

#### 5. 图文说明

| 条件 | 类型 | 违反后果 |
|---|---|---|
| shape 可广播 | 前置 | RuntimeError |
| dtype 一致 | 前置 | 隐式提升或报错 |
| device 相同 | 前置 | device mismatch |
| 输出 shape | 后置 | 下游维度错 |

#### 6. 拓展知识

type promotion 遵循一张提升表，`float32 + float64 → float64`，但 `int64 + float32` 需小心整数截断；`torch.compile` 对 dtype 变化会触发重编译；`channels_last` 属于布局配置，不改变逻辑 shape。

#### 7. 常见追问及回答要点

1. 广播为什么从尾部对齐？——保持高维语义，低维张量按尾维扩展。
2. dtype 不一致一定报错吗？——不，type promotion 会提升，但混精度算子可能报错。
3. 断言放哪？——公共入口做前置，返回处做后置。

#### 8. 易错点与反例

1. CPU 张量与 CUDA 张量相加，device mismatch。
2. 忘记广播是尾部对齐，误以为形状相同的中间维也兼容。
3. 断言写死 dtype==float32，AMP 下 fp16 输入误报。

#### 9. 实践与验证

写单测覆盖：合法广播通过、非法广播抛错、device/dtype 不一致抛错、0 维张量边界通过；用 `assert_close` 校验输出形状与数值。无 GPU 依赖。

#### 10. 评分标准

I023 评分：不合格不清楚广播规则；合格能写出前置断言；良好能覆盖 dtype/device/视图条件与后置校验；优秀/专家能处理 promotion 与 0 维边界、把校验放到正确层次。

### I024. 配置约束：autograd

- 难度：中等级
- 知识域：K02 autograd
- 考察能力：事件顺序、状态迁移、所有权与观测点
- 题型：流程/调用链/数据流
- 面试频率：高频
- 前置知识：requires_grad、leaf、view、detach、反向
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- autograd 的前置条件：loss 必须 `requires_grad`；张量若被 in-place 修改，其版本号（version counter）变化会使反向报错。
- `torch.no_grad()`/`inference_mode()` 禁用建图；`detach()` 返回不追踪梯度的视图；`retain_graph` 控制图在反向后的保留。
- 自定义 `Function` 的 backward 必须返回与输入数量一致的梯度，且不得修改 `save_for_backward` 保存的张量。
- `torch.autograd.set_detect_anomaly(True)` 把数值错误升级为带前向栈的异常，属于开发期强校验。

#### 2. 问题分析

给自动微分配置前置/后置条件。两个不变量：(1) 反向时图必须未被释放且未被 in-place 破坏；(2) 每个需要梯度的输入在 backward 后必须具备可累加的 `.grad` 语义。边界反例：对叶子张量做 `x += 1`（in-place）后使用其旧值参与反向，会因版本号不一致报错；但 `x.data += 1` 绕过版本控制则可执行，属危险边界。

#### 3. 具体答案

面试中可以这样回答：

> 我会先断言 loss 需要梯度，并在开发期开 anomaly 模式把数值问题定位到前向栈。前置条件上，避免对需要梯度的张量做 in-place 修改，必要时用 `retain_graph` 才能多次反向。`no_grad`/`inference_mode` 用在推理分支，`detach` 用在真的不需要梯度的中间量。自定义 Function 我会校验 backward 返回值数量与形状，并用 gradcheck 做后置校验。恢复或复用图时明确图的生命周期。

#### 4. 技术洞察

```
pre:  loss.requires_grad
post: for p in params: p.grad is not None (或显式 allow_unused)
in-place guard: tensor._version 在反向时必须未变
```

````python
assert loss.requires_grad
with torch.no_grad():
    ref = model(x)          # 推理分支
loss.backward(retain_graph=need_twice)
````

#### 5. 图文说明

| 条件 | 类型 | 违反后果 |
|---|---|---|
| loss 需梯度 | 前置 | backward 报错 |
| 图未释放 | 前置 | 二次反向报错 |
| 无 in-place 破坏 | 前置 | version counter 报错 |
| grad 形状正确 | 后置 | 静默错误 |

#### 6. 拓展知识

`inference_mode` 比 `no_grad` 更激进，禁止对张量做需要 autograd 元数据的操作；功能化（functionalization）把 in-place 改写为 out-of-place 以支持编译；vmap/forward-mode 是另一套前置约束。

#### 7. 常见追问及回答要点

1. version counter 是什么？——记录张量被 in-place 修改次数，反向校验一致性。
2. `no_grad` 与 `detach` 区别？——前者管作用域，后者管单个张量。
3. backward 返回值约束？——数量与形状须与 forward 输入对应。

#### 8. 易错点与反例

1. 对需要梯度的输入做 in-place，报 “a variable needed for gradient has been modified”。
2. 一次反向后又用同一 loss，未 retain_graph，图已释放。
3. 自定义 backward 返回 1 个梯度而 forward 有 2 个输入。

#### 9. 实践与验证

复现三类失败：in-place 修改、二次反向、返回数量不符；再用 anomaly 模式确认报错栈指向正确；`gradcheck` 校验自定义 Function。无 GPU 依赖。

#### 10. 评分标准

I024 评分：不合格不知 in-place 陷阱；合格能写前置断言；良好能覆盖图生命周期与自定义 Function 约束；优秀/专家能用 anomaly/gradcheck 定位并解释版本号与功能化机制。

### I025. 配置约束：Module/状态

- 难度：中等级
- 知识域：K03 Module/状态
- 考察能力：事件顺序、状态迁移、所有权与观测点
- 题型：流程/调用链/数据流
- 面试频率：高频
- 前置知识：参数、buffer、模式和 state_dict
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- `state_dict` 是键到张量的有序映射，键由模块层级加点名组成；加载时键必须与当前模型匹配。
- `load_state_dict(strict=True)` 要求键集合完全一致；`strict=False` 返回 `missing_keys`/`unexpected_keys`。
- `train()/eval()` 影响行为，属于运行时配置而非 `state_dict` 内容；`persistent=False` 的 buffer 不进 `state_dict`。
- 参数共享（tied weights）时多个键指向同一张量，加载顺序与别名会影响结果。

#### 2. 问题分析

为 Module 状态配置前置/后置条件。两个不变量：(1) 保存与加载两端的键集合（含 buffer）一致；(2) 参数/缓冲区的设备与 dtype 在加载后符合预期。边界反例：`strict=False` 加载一个缺少部分权重的 checkpoint，缺失参数保持随机初始化，表面成功但模型语义错误，属于静默失败。

#### 3. 具体答案

面试中可以这样回答：

> 加载前我会断言目标模型的 `state_dict().keys()` 与 checkpoint 的键集合一致，或者显式处理 missing/unexpected。加载后用 `assert_close` 抽查关键权重。配置上区分参数和 buffer，确认 buffer 也随设备迁移。对参数共享要确保别名一致，避免加载后别名断开。模式切换 `train/eval` 属于运行时配置，要在加载后按训练或推理设置。

#### 4. 技术洞察

```
pre:  set(ckpt.keys()) == set(model.state_dict().keys())
post: model.to(dev); model.eval()/train()
```

````python
res = model.load_state_dict(ckpt, strict=False)
assert not res.missing_keys, res.missing_keys
assert not res.unexpected_keys, res.unexpected_keys
model = model.to(device)
````

#### 5. 图文说明

| 检查 | 类型 | 违反后果 |
|---|---|---|
| 键集合一致 | 前置 | 权重错配/随机 |
| buffer 在列 | 前置 | 统计量丢失 |
| 设备/dtype | 后置 | 运行时设备错 |
| 模式 | 后置 | BN/Dropout 行为错 |

#### 6. 拓展知识

分布式 checkpoint 需按 rank 分片；`strict=False` 常用于微调时替换分类头，但应显式列出忽略的键；参数共享在网络量化与剪枝后可能断裂。

#### 7. 常见追问及回答要点

1. buffer 为什么要在 state_dict？——它是模型状态（如 BN running stats）。
2. 何时用 strict=False？——结构有差异且已显式处理缺失键。
3. 设备迁移谁负责？——加载后显式 `.to(device)`，或按 checkpoint 设备映射。

#### 8. 易错点与反例

1. `strict=False` 后不检查 missing_keys，关键权重仍随机。
2. 改了层名未做映射，权重整体错配。
3. 只搬参数不搬 buffer，BN 统计停在 CPU。

#### 9. 实践与验证

断言键集合相等；改一处权重保存再加载，断言对应参数变化；把 buffer 设为 `persistent=False` 断言不在 state_dict；检查 `.to()` 后 buffer 设备。无 GPU 依赖。

#### 10. 评分标准

I025 评分：不合格只会 load_state_dict；合格能处理键匹配；良好能覆盖 buffer、设备与 strict 语义；优秀/专家能处理参数共享与分片 checkpoint 并设计键一致性验证。

### I026. 配置约束：数据管道

- 难度：中等级
- 知识域：K04 数据管道
- 考察能力：事件顺序、状态迁移、所有权与观测点
- 题型：流程/调用链/数据流
- 面试频率：高频
- 前置知识：Dataset、Sampler、collate、padding、版本
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- `collate_fn` 的契约：输入是样本 `list`，输出是可直接喂给模型的 batch；默认 `default_collate` 要求样本结构一致、张量可堆叠。
- `num_workers>0` 要求 Dataset 可 pickle（除非用 spawn 传参方式），且各 worker 的执行是独立进程。
- `pin_memory=True` 仅在主机有对应 CUDA 设备时有意义；`persistent_workers=True` 要求 `num_workers>0`。
- `IterableDataset` 与 `Sampler` 互斥；`DistributedSampler` 与 `shuffle=True` 互斥。

#### 2. 问题分析

给数据管道配置前置/后置条件。两个不变量：(1) `collate_fn` 输出结构在训练循环里保持稳定；(2) 多 worker 下每个 epoch 的样本索引集合不重不漏，且 worker 随机性可复现。边界反例：设 `persistent_workers=True` 但 `num_workers=0`，`DataLoader` 直接报参数非法，属配置前置失败。

#### 3. 具体答案

面试中可以这样回答：

> 我会在构造 `DataLoader` 时校验配置组合：`persistent_workers` 需要 `num_workers>0`，`DistributedSampler` 不能和 `shuffle` 同用，`IterableDataset` 不能用 Sampler。`collate_fn` 的输出我会用断言确认 batch 维存在、dtype/device 符合预期。多 worker 时用 `worker_init_fn` 隔离随机种子，并在一个 epoch 后核对索引集合不重不漏。pin_memory 只在有 CUDA 时开，避免无意义开销。

#### 4. 技术洞察

```
pre:  num_workers>=0; persistent_workers ⇒ num_workers>0
pre:  IterableDataset ⇒ 无 Sampler; DistributedSampler ⇒ shuffle=False
post: batch[0].dim() == sample.dim()+1, 索引集合 == range(len(ds))
```

````python
assert not (is_iterable and has_sampler)
assert not (use_batch_sampler and shuffle)
loader = DataLoader(ds, collate_fn=collate, num_workers=w,
                    pin_memory=torch.cuda.is_available())
````

#### 5. 图文说明

| 配置 | 前置约束 | 违反后果 |
|---|---|---|
| persistent_workers | w>0 | 构造报错 |
| DistributedSampler | shuffle=False | 语义冲突 |
| pin_memory | 有 CUDA | 无用开销 |
| collate 输出 | batch 维 | 模型维度错 |

#### 6. 拓展知识

`default_collate` 对 `None`、变长 list 处理有限；`torchdata` 的 `StatefulDataLoader` 支持 mid-epoch 恢复；`prefetch_factor` 与 `persistent_workers` 共同影响内存与延迟。

#### 7. 常见追问及回答要点

1. 多 worker 下如何复现增强？——`worker_init_fn` 给每 worker 设确定种子。
2. 为什么 IterableDataset 不能有 Sampler？——采样由迭代器自身决定。
3. pin_memory 的代价？——固定内存不可分页，过量会挤占系统内存。

#### 8. 易错点与反例

1. `num_workers=0` 却设 `persistent_workers=True`，构造即报错。
2. DistributedSampler 配 shuffle=True，采样重复。
3. collate 输出缺 batch 维，模型 shape 报错。

#### 9. 实践与验证

单测枚举非法配置断言抛错；收集一个 epoch 索引断言集合等于全集；`num_workers>1` 固定种子跑两次断言 batch 序列一致。多进程数字 [待验证]。

#### 10. 评分标准

I026 评分：不合格不清楚配置互斥；合格能正确设参数；良好能覆盖 worker 种子与索引不重不漏；优秀/专家能处理 IterableDataset 分片与 pin/prefetch 取舍并设计断言。

### I027. 配置约束：训练/优化器

- 难度：中等级
- 知识域：K05 训练/优化器
- 考察能力：事件顺序、状态迁移、所有权与观测点
- 题型：流程/调用链/数据流
- 面试频率：高频
- 前置知识：forward、backward、累积、optimizer、scheduler
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- `optimizer.step()` 的前置条件：至少一个 `param_groups` 且每组 `params` 非空；使用 `scaler` 时须先 unscale。
- Adam 的前置：`lr`、`betas`、`eps`、`weight_decay` 在合法域（`0<=betas<1`、`eps>0`）。
- scheduler 必须绑定正在使用的 optimizer 实例，且 `last_epoch` 与训练进度一致。
- `clip_grad_norm_` 需要所有待裁剪参数的 `.grad` 非 None，否则该参数被跳过。

#### 2. 问题分析

为训练/优化器配置前置/后置条件。两个不变量：(1) `step` 前梯度完整、可被优化器消费；(2) `param_groups` 的超参在合法域且与预期分组一致。边界反例：空 `param_groups`（例如所有参数被冻结后）调用 `optimizer.step()` 会抛 “optimizer got an empty parameter list”，属前置失败。

#### 3. 具体答案

面试中可以这样回答：

> 我会在构造 optimizer 时校验参数列表非空、超参在合法域，并断言 optimizer 与 scheduler 绑定的是同一实例。`step` 前我会确认梯度已计算且已 unscale。分组上检查每个参数的 lr/weight_decay 符合设计，冻结参数不留在优化器里。后置上检查参数是否真的变化、是否有 nan。恢复时断言 `param_groups` 结构与 state 一致。

#### 4. 技术洞察

```
pre:  lr>0; 0<=betas<1; eps>0
pre:  optimizer.param_groups[0]['params'] 非空
post: θ_new != θ_old (lr>0 且 grad 非零)
```

````python
assert optimizer.param_groups and optimizer.param_groups[0]["params"]
assert 0 <= beta1 < 1 and 0 <= beta2 < 1 and eps > 0
before = [p.detach().clone() for p in model.parameters()]
optimizer.step()
assert any(not torch.equal(a, b) for a, b in zip(before, model.parameters()))
````

#### 5. 图文说明

| 条件 | 类型 | 违反后果 |
|---|---|---|
| param_groups 非空 | 前置 | step 报错 |
| 超参合法 | 前置 | 发散/nan |
| 梯度已 unscale | 前置 | 更新错尺度 |
| 参数变化 | 后置 | 无效训练 |

#### 6. 拓展知识

`ReduceLROnPlateau` 需要指标而非无参 `step`；`AdamW` 的 decay 作用于参数；梯度裁剪对 None 梯度参数静默跳过，需注意是否包含全部参数。

#### 7. 常见追问及回答要点

1. 冻结全部参数后还能 step 吗？——不能，空参数列表报错。
2. 超参非法会怎样？——`betas>=1` 动量发散，`eps=0` 除零。
3. 如何验证 step 生效？——比较更新前后参数。

#### 8. 易错点与反例

1. 优化器只建在部分参数上，其余层永不更新。
2. `betas` 写错量级，二阶矩更新异常，loss nan。
3. scheduler 绑定旧 optimizer，lr 实际未变。

#### 9. 实践与验证

断言参数列表非空与超参域；单步更新后断言参数变化；人为设 `lr=0` 断言参数不变（边界）。无 GPU 依赖。

#### 10. 评分标准

I027 评分：不合格不知空参数报错；合格能正确配置；良好能覆盖超参域与分组、验证 step 生效；优秀/专家能处理冻结参数、scheduler 绑定与恢复一致性并设计断言。

### I028. 配置约束：AMP/恢复

- 难度：中等级
- 知识域：K06 AMP/恢复
- 考察能力：事件顺序、状态迁移、所有权与观测点
- 题型：流程/调用链/数据流
- 面试频率：高频
- 前置知识：autocast、scaler、快照、随机状态
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- `autocast(device_type=...)` 需明确设备类型；在 CPU 上应用 CUDA autocast 或反之会导致行为不符或被忽略。
- `GradScaler(device_type=...)` 必须与 autocast 的设备一致；FP16 才需要 scaler，BF16 通常不需要。
- 恢复前置：`scaler.load_state_dict` 必须在任何 `scale/step/update` 之前调用。
- `scaler.unscale_` 对同一 optimizer 未 step 前重复调用会触发告警或行为未定义，需保证每 step 一次。

#### 2. 问题分析

为 AMP/恢复配置前置/后置条件。两个不变量：(1) autocast 与 scaler 的 device_type 一致且与张量设备匹配；(2) 恢复时 scaler 状态与 RNG 在任何训练操作前加载。边界反例：在已 `autocast` 的 fp16 上手动把 loss 转成 fp16 再 scale，scale 后仍可能上溢，`scaler` 频繁减半，训练缓慢。

#### 3. 具体答案

面试中可以这样回答：

> 我会先确认 `autocast` 和 `GradScaler` 都用同一个 `device_type`，并且和张量设备一致。FP16 才用 scaler，BF16 直接 autocast。恢复时我在建好模型、优化器之后，立即 `load_state_dict` 恢复 scaler，并恢复 RNG，之后才开始训练。unscale 每步只做一次，clip 放在 unscale 之后。我会断言 scale 因子为正且恢复前后一致。

#### 4. 技术洞察

```
pre:  autocast.device_type == scaler.device_type == x.device.type
pre:  scaler.load_state_dict 在首次 scale() 之前
post: scaler.get_scale() > 0; 每 step 至多一次 unscale
```

````python
scaler = torch.amp.GradScaler(device_type="cuda", enabled=use_fp16)
with torch.amp.autocast(device_type="cuda", dtype=torch.float16):
    loss = criterion(model(x), y)
scaler.scale(loss).backward()
scaler.unscale_(optimizer)
scaler.step(optimizer); scaler.update()
````

#### 5. 图文说明

| 条件 | 类型 | 违反后果 |
|---|---|---|
| device_type 一致 | 前置 | 缩放无效/忽略 |
| FP16 才用 scaler | 前置 | 多余开销 |
| 恢复顺序正确 | 前置 | scale/RNG 错位 |
| unscale 每步一次 | 前置 | 梯度尺度错 |

#### 6. 拓展知识

BF16 的动态范围与 FP32 相同指数宽度，梯度不易下溢，实践中常省略 scaler；`torch.amp` 新接口统一 `device_type`，旧的 `torch.cuda.amp` 仅支持 CUDA。

#### 7. 常见追问及回答要点

1. 为什么 FP16 要 scaler？——动态范围窄，小梯度下溢为 0。
2. BF16 为什么不用？——指数位足够，下溢风险低。
3. 恢复顺序为什么重要？——scale 是训练状态，晚加载会覆盖。

#### 8. 易错点与反例

1. autocast 用 cuda 而 scaler 用 cpu，缩放未生效。
2. 恢复时先训练一步再加载 scaler，状态被覆盖。
3. 每次 forward 重复 `scaler.unscale_`，梯度被反复缩放。

#### 9. 实践与验证

断言 scaler 与 autocast 的 device_type 相同；保存恢复后断言 `get_scale()` 相等；用日志确认每 step 只 unscale 一次。GPU 数字 [待验证]。

#### 10. 评分标准

I028 评分：不合格不分 FP16/BF16；合格能正确配对 autocast 与 scaler；良好能处理恢复顺序与 unscale 次数；优秀/专家能解释动态范围差异并设计 scale 一致性验证。

### I029. 配置约束：分布式

- 难度：中等级
- 知识域：K07 分布式
- 考察能力：事件顺序、状态迁移、所有权与观测点
- 题型：流程/调用链/数据流
- 面试频率：高频
- 前置知识：rank、DDP、FSDP、collective、容错
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- 分布式前置：必须先 `init_process_group` 并在任何集合通信前设置本进程设备（`torch.cuda.set_device(local_rank)`）。
- `world_size` 必须与实际启动的进程数一致；不一致会导致集合通信挂起或超时。
- 使用 `DistributedSampler` 必须每 epoch 调 `set_epoch(epoch)`，否则 shuffle 序列在 epoch 间不变。
- DDP 包装要求模型已 `.to(device)`，且 `device_ids` 与当前设备匹配。

#### 2. 问题分析

为分布式配置前置/后置条件。两个不变量：(1) 所有 rank 使用相同的 `world_size` 与集合通信序列；(2) 每个 rank 的数据子集互补且各自设备正确。边界反例：只在一个 rank 上调用 `all_reduce`，其余 rank 未参与，前者会一直阻塞到超时，而非立即报错，属“挂起型”前置失败。

#### 3. 具体答案

面试中可以这样回答：

> 我在启动时断言 `world_size` 与进程数一致、`rank` 在范围内，并 `set_device(local_rank)`。DDP 包装前确认模型已在对应设备上。数据用 `DistributedSampler` 并每 epoch `set_epoch`。所有集合通信必须所有 rank 一起调用，我会把通信封装到统一入口避免分支。后置上，用 `all_gather` 抽查参数一致。容错配置上，超时时间与重启策略要显式设置。

#### 4. 技术洞察

```
pre:  world_size == num_procs; 0 <= rank < world_size
pre:  set_device(local_rank) 在任何集合通信前
pre:  所有 rank 调用次数相同的 collective
post: all_gather(params) 各 rank 一致
```

````python
dist.init_process_group("nccl", world_size=W, rank=r, timeout=timedelta(minutes=30))
torch.cuda.set_device(r)
assert dist.get_world_size() == W and dist.get_rank() == r
````

#### 5. 图文说明

| 条件 | 类型 | 违反后果 |
|---|---|---|
| world_size 一致 | 前置 | 挂起/超时 |
| 设设备 | 前置 | 错误设备上通信 |
| set_epoch | 前置 | shuffle 重复 |
| 通信序列一致 | 前置 | 死锁 |

#### 6. 拓展知识

`torchrun` 通过环境变量注入 `RANK/WORLD_SIZE/LOCAL_RANK`；NCCL 有独立超时；弹性训练允许成员变更但需 rendezvous 与 checkpoint 支持。

#### 7. 常见追问及回答要点

1. 通信不一致为什么挂起？——NCCL 等待匹配的 collective，直到超时。
2. set_epoch 的作用？——让各 rank 的 shuffle 在同 epoch 内一致可复现。
3. 如何设超时？——`init_process_group(timeout=...)` 与 NCCL 环境变量。

#### 8. 易错点与反例

1. 分支里只有部分 rank 调用 `all_reduce`，其余挂起。
2. 忘记 `set_epoch`，每个 epoch 数据顺序相同。
3. world_size 与实际进程数不符，通信错配。

#### 9. 实践与验证

2 进程跑一步，`all_gather` 参数断言一致；临时让一个 rank 少调一次 collective，观察是否超时（验证前置必要性）；断言 `get_world_size()` 等于启动参数 [待验证]。

#### 10. 评分标准

I029 评分：不合格不清楚集合通信一致性；合格能正确初始化与设设备；良好能处理 set_epoch 与通信序列；优秀/专家能设计超时/容错配置并验证参数一致性。

### I030. 配置约束：编译栈

- 难度：中等级
- 知识域：K08 编译栈
- 考察能力：事件顺序、状态迁移、所有权与观测点
- 题型：流程/调用链/数据流
- 面试频率：高频
- 前置知识：Dynamo、AOTAutograd、Inductor、guard、缓存
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- `torch.compile` 的配置前置：输入 dtype、device、shape 的一致性决定 guard 命中；动态 shape 需显式开启 `dynamic=True` 或用 `mark_dynamic`。
- CUDA graphs（`reduce-overhead`）要求静态形状、静态内存地址，且不允许 CPU 侧同步或动态控制流。
- 确定性算法配置：`torch.use_deterministic_algorithms(True)` 与 cudnn benchmark 互斥影响复现。
- 编译缓存在进程内有效；跨进程需保证 guard 与后端配置一致才能复用。

#### 2. 问题分析

为编译栈配置前置/后置条件。两个不变量：(1) 编译产物与运行时输入属性必须在 guard 覆盖范围内；(2) 开启 CUDA graphs 时输入地址与形状稳定。边界反例：用 `reduce-overhead` 但训练循环里有 `.item()` 触发同步，CUDA graph 捕获失败并回退，性能反而不稳定。

#### 3. 具体答案

面试中可以这样回答：

> 我会先明确输入属性的稳定性：如果 shape 会变，就开动态 shape 或 `mark_dynamic`，否则固定 shape。要用 CUDA graphs 就保证静态形状、静态地址，并去掉循环内的同步调用和数据依赖的控制流。确定性算法配置要和对复现的要求一致。后置上我会用 `torch._dynamo.explain` 检查是否存在 graph break，并统计重编译次数应为 0。若发现退化，先稳定输入属性再谈调优。

#### 4. 技术洞察

```
pre:  shape/dtype/device 稳定 ⇒ guard 命中
pre:  reduce-overhead ⇒ 静态形状 + 无同步
post: recompile_count == 0; graph_breaks 记录
```

````python
torch._dynamo.mark_dynamic(x, 0)
f = torch.compile(model, mode="reduce-overhead")
torch._dynamo.reset(); explain = torch._dynamo.explain(f, (x,))
````

#### 5. 图文说明

| 配置 | 前置 | 违反后果 |
|---|---|---|
| 动态 shape | dynamic/mark_dynamic | 反复重编译 |
| CUDA graphs | 静态形状/地址 | 捕获失败回退 |
| 确定性 | deterministic 开启 | 结果不可复现 |
| 缓存 | guard 一致 | 缓存未命中 |

#### 6. 拓展知识

`torch._dynamo.config.cache_size_limit` 控制每函数缓存上限；`cudagraph_trees` 支持多图；Inductor 的 autotune 结果可按 cache 目录持久化。

#### 7. 常见追问及回答要点

1. 什么触发重编译？——guard 覆盖的属性变化。
2. CUDA graphs 为何怕同步？——捕获期不允许 CPU/GPU 同步与动态分配。
3. 动态 shape 代价？——泛化但可能不如特化 kernel。

#### 8. 易错点与反例

1. 循环内 `.item()` 使 CUDA graph 捕获失败。
2. 不标记动态维，输入长度一变就重编译。
3. 开确定性后仍期望用非确定性算法提速，配置冲突。

#### 9. 实践与验证

固定输入跑 10 步断言重编译为 0；改变一个维断言是否重编译；用 explain 统计 graph break。GPU 数字 [待验证]。

#### 10. 评分标准

I030 评分：不合格只知 compile 开关；合格能设动态 shape；良好能处理 CUDA graphs 前置与确定性配置；优秀/专家能用 explain 定位退化并给出稳定输入属性的方案。

### I031. 配置约束：性能

- 难度：中等级
- 知识域：K09 性能
- 考察能力：前置条件、后置条件、断言与错误处理
- 题型：代码/伪代码/配置分析
- 面试频率：高频
- 前置知识：关键路径、带宽、算力、通信、显存、统计
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- profiler 配置前置：`activities` 需包含目标设备（CPU/CUDA），`schedule` 的 wait/warmup/active 控制采集窗口，避免把冷启动计入。
- 测时间应使用 `torch.cuda.Event` 或 profiler 的设备时间；CPU 计时需 `torch.cuda.synchronize()` 才准确，但同步本身会干扰异步执行。
- benchmark 前置：预热若干次丢弃首轮，固定输入形状，关闭会引入抖动的后台任务。
- 内存统计用 `torch.cuda.reset_peak_memory_stats` 与 `max_memory_allocated` 配对。

#### 2. 问题分析

为性能测量配置前置/后置条件。两个不变量：(1) 计时区间内无遗漏，异步 kernel 被同步后计入；(2) 每次测量在相同输入与相同状态下进行，可比。边界反例：不预热直接测，第一次包含 cuBLAS 句柄初始化与 kernel 编译，时间远大于稳态，测量结果失真。

#### 3. 具体答案

面试中可以这样回答：

> 我会在计时前预热若干步并丢弃，用 profiler 的 schedule 跳过 warmup。设备计时用 CUDA event 或 profiler 的 device time，避免把异步 kernel 漏掉。每次测同一输入形状与 batch，先 `reset_peak_memory_stats` 再测峰值显存。后置上我会记录 P50/P95 而非单值，并断言测量区间内没有额外的同步或日志 I/O 干扰。

#### 4. 技术洞察

````python
for _ in range(warmup): step()
torch.cuda.synchronize(); torch.cuda.reset_peak_memory_stats()
start = torch.cuda.Event(True); end = torch.cuda.Event(True)
start.record()
for _ in range(iters): step()
end.record(); torch.cuda.synchronize()
ms = start.elapsed_time(end) / iters
peak = torch.cuda.max_memory_allocated()
````

#### 5. 图文说明

| 配置 | 前置/后置 | 违反后果 |
|---|---|---|
| 预热 | 前置 | 含初始化，偏慢 |
| 设备 event | 前置 | 漏计异步 |
| 固定输入 | 前置 | 不可比 |
| 峰值内存复位 | 前置 | 峰值虚高 |

#### 6. 拓展知识

CUDA event 分辨率约 0.5µs，足够测 kernel 级；多 stream 场景需在目标 stream 记录；`profile_memory=True` 给出分配来源，配合 `memory_summary` 看碎片。

#### 7. 常见追问及回答要点

1. 为什么要预热？——跳过句柄初始化与编译，测稳态。
2. CPU 计时为何不准？——异步 kernel 未完成，需同步。
3. 如何测显存峰值？——复位后取 `max_memory_allocated`。

#### 8. 易错点与反例

1. 不预热直接测，首轮时间虚高。
2. 用纯 CPU 计时不同步，测到的是提交时间。
3. 每步 `.item()` 打印，测量被同步污染。

#### 9. 实践与验证

对比“预热 vs 不预热”的耗时差异以证明预热必要；多次测量断言 P95/P50 比值稳定；复位前后峰值内存差异。GPU 数字 [待验证]。

#### 10. 评分标准

I031 评分：不合格不知预热；合格能用 event 计时；良好能处理同步、峰值内存与统计；优秀/专家能设计可比基准并识别测量污染来源。

### I032. 配置约束：导出部署

- 难度：中等级
- 知识域：K10 导出部署
- 考察能力：前置条件、后置条件、断言与错误处理
- 题型：代码/伪代码/配置分析
- 面试频率：高频
- 前置知识：state_dict、Export、runtime、schema、迁移
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- 导出配置前置：`model.eval()`、`torch.no_grad()`、固定或声明动态的输入；ONNX 需指定 `opset_version` 与 `dynamic_axes`。
- `torch.export` 要求图可序列化、无 Python 副作用；不支持的算子会报错需替换或改为自定义算子。
- 导出后置：导出图与 eager 输出在容差内一致，且能跨进程加载。
- `state_dict` 与导出图是两种迁移载体，前者依赖代码结构，后者含图。

#### 2. 问题分析

为导出部署配置前置/后置条件。两个不变量：(1) 导出前模型处于推理态且无随机算子；(2) 导出图在声明输入域内数值等价于原模型。边界反例：模型里含 `torch.nonzero` 之类数据依赖形状的算子，ONNX/export 无法静态化，导出失败或错误，需要在导出前改写。

#### 3. 具体答案

面试中可以这样回答：

> 导出前我切 `eval()`、包 `no_grad`，确认没有 dropout 训练分支和随机算子。ONNX 我固定 `opset_version`，对 batch/seq 这些可变维用 `dynamic_axes` 声明。`torch.export` 我会检查是否有不支持的算子或 Python 副作用。导出后用同一输入对比输出，且用第二个不同 shape 的输入验证动态轴。后置上确认产物能在干净进程加载并推理。

#### 4. 技术洞察

````python
model.eval()
torch.onnx.export(model, x, "m.onnx", opset_version=17,
                  dynamic_axes={"x": {0: "B", 1: "L"}, "y": {0: "B"}})
ep = torch.export.export(model, (x,))
torch.export.save(ep, "m.pt2")
assert torch.allclose(model(x), ort_session.run(None, {..})[0], atol=1e-4)
````

#### 5. 图文说明

| 配置 | 类型 | 违反后果 |
|---|---|---|
| eval/no_grad | 前置 | 随机性入图 |
| opset/动态轴 | 前置 | 算子缺失/形状报错 |
| 算子可导出 | 前置 | 导出失败 |
| 数值等价 | 后置 | 上线偏差 |

#### 6. 拓展知识

TensorRT 会做融合与量化校准，数值与 PyTorch 有差异；`torch.export` 的 `dynamic_shapes` 比 ONNX 的动态轴更强类型化；控制流需转为可导出形式（`torch.cond`）。

#### 7. 常见追问及回答要点

1. 为什么导数前要 eval？——消除 BN/Dropout 训练行为。
2. 动态轴如何声明？——按维度名标注可变的维。
3. 导出失败常见原因？——不支持算子、数据依赖形状、Python 副作用。

#### 8. 易错点与反例

1. 未 eval，dropout 进入图。
2. 用数据依赖的 `nonzero`，导出形状不可静态化。
3. 只测一个 shape，动态轴实际无效上线才暴露。

#### 9. 实践与验证

两 shape 对比导出与 eager 输出 `assert_close`；干净进程加载推理；扫描 unsupported ops。运行时差异 [待验证]。

#### 10. 评分标准

I032 评分：不合格只知 onnx.export；合格能 eval 并设 opset；良好能处理动态轴与算子支持；优秀/专家能改写不可导出算子、处理控制流并做数值等价验证。

### I033. 配置约束：自定义算子

- 难度：中等级
- 知识域：K11 自定义算子
- 考察能力：前置条件、后置条件、断言与错误处理
- 题型：代码/伪代码/配置分析
- 面试频率：高频
- 前置知识：schema、dispatcher、stream、autograd、ABI
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- 自定义算子 schema 必须声明参数别名与可变性，如 `Tensor(a!)` 表示原地修改入参；`torch.library` 与 C++ `TORCH_LIBRARY` 均以 schema 驱动分派。
- CUDA kernel 前置：必须在当前 stream 执行，用 `at::cuda::getCurrentCUDAStream()`；跨 stream 需事件同步。
- 反向注册前置：`autograd.Function` 或 `register_autograd` 必须与 forward 语义匹配，`save_for_backward` 只保存反向所需张量。
- 编译兼容：需提供 fake/meta 实现以支持形状推导与 `torch.compile`。

#### 2. 问题分析

为自定义算子配置前置/后置条件。两个不变量：(1) schema 的别名/可变标注与 kernel 实际行为一致；(2) kernel 在当前 stream 上执行，不引入隐式全局同步。边界反例：声明 out 参数为可变却在新 tensor 上写，调用方持有的旧输出未被更新，语义与声明不符。

#### 3. 具体答案

面试中可以这样回答：

> 我先定义 schema，把哪些参数会被原地修改标成 `(a!)`，哪些是和输入别名一致的输出标成别名。CUDA 实现里用当前 stream，避免和相邻算子乱序；需要跨 stream 就用事件同步而不是 `synchronize`。反向我会用 `autograd.Function` 或注册 backward，并用 gradcheck 校验。为了让 `torch.compile` 能用，我会补 `register_fake` 做形状推导。后置上对比自定义实现与参考实现的输出。

#### 4. 技术洞察

````python
@torch.library.custom_op("mylib::fused", mutates_args=())
def fused(x: Tensor, y: Tensor) -> Tensor:
    return _fused_cuda(x, y)
@fused.register_fake
def _(x, y): return torch.empty_like(x)
def _setup(ctx, x, y, out): ctx.save_for_backward(x, y); return out
````

#### 5. 图文说明

| 条件 | 类型 | 违反后果 |
|---|---|---|
| 别名/可变标注 | 前置 | 功能化误判 |
| 当前 stream | 前置 | 数据竞争 |
| fake/meta | 前置 | 编译失败 |
| 反向公式 | 后置 | 梯度错误 |

#### 6. 拓展知识

功能化把原地改写为纯函数；`torch.compile` 依赖 meta/fake 做符号形状；ABI 兼容要求扩展与主版本一致，否则运行期加载失败。

#### 7. 常见追问及回答要点

1. `(a!)` 标注的意义？——告诉功能化该参数被原地修改。
2. 为什么用当前 stream？——保持与其它算子的执行顺序。
3. fake 实现缺失会怎样？——编译时形状推导失败回退 eager。

#### 8. 易错点与反例

1. 声明 pure 却原地写，功能化漏拷贝。
2. kernel 用默认 stream，与相邻算子乱序。
3. 反向返回形状错误，梯度广播出错。

#### 9. 实践与验证

`gradcheck` 校验反向；多 stream 下运行并同步后比对无竞争；`torch.compile` 下运行断言无回退。GPU 结果 [待验证]。

#### 10. 评分标准

I033 评分：不合格不知 schema；合格能注册算子与 kernel；良好能正确处理别名、stream 与 fake；优秀/专家能分析功能化、ABI 与编译兼容并用 gradcheck 验证。

### I034. 配置约束：测试治理

- 难度：中等级
- 知识域：K12 测试治理
- 考察能力：前置条件、后置条件、断言与错误处理
- 题型：代码/伪代码/配置分析
- 面试频率：高频
- 前置知识：oracle、证据、日志、回滚、门禁、复盘
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- 可复现测试的前置：固定 `torch.manual_seed`、`numpy`/`random` 种子、`torch.use_deterministic_algorithms(True)`、`CUBLAS_WORKSPACE_CONFIG`。
- 数值断言用 `torch.testing.assert_close`，可分别设 `rtol`/`atol`；dtype 提升与设备差异需在容差中体现。
- 测试隔离：每个用例重置全局状态，避免用例间耦合导致 flaky。
- 门禁配置：分级（冒烟/关键路径/全量），失败阻断级别不同。

#### 2. 问题分析

为测试治理配置前置/后置条件。两个不变量：(1) 同一用例在相同种子与环境指纹下可稳定复现；(2) 断言必须绑定 oracle（解析解/参考实现/性质），而非仅“不抛异常”。边界反例：一个断言 `y.sum(-1)==1` 的 softmax 测试，在数值溢出产生 nan 时 `nan==1` 为 False 能捕获，但一个只看 shape 的测试无法发现数值错误。

#### 3. 具体答案

面试中可以这样回答：

> 每个测试用例开始固定种子并设置确定性算法，记录环境指纹。断言用 `assert_close` 明确 rtol/atol，并绑定 oracle：能解析的用解析解，不能的用参考实现，或至少用性质断言如概率和为 1、输出落在合法区间。用例之间重置全局状态避免耦合。门禁按重要性分级，冒烟快、关键路径全，失败按级别阻断。失败必须能复现，所以种子与环境一起记录。

#### 4. 技术洞察

````python
def test_gelu():
    torch.manual_seed(0)
    torch.use_deterministic_algorithms(True)
    x = torch.randn(1024)
    ref = 0.5 * x * (1 + torch.erf(x / math.sqrt(2)))
    assert torch.testing.assert_close(f(x), ref, rtol=1e-5, atol=1e-6) is None
````

#### 5. 图文说明

| 配置 | 类型 | 违反后果 |
|---|---|---|
| 固定种子 | 前置 | flaky |
| 确定性算法 | 前置 | 结果漂移 |
| oracle 断言 | 前置 | 假阳性 |
| 状态隔离 | 前置 | 用例耦合 |

#### 6. 拓展知识

某些 CUDA 算子没有确定性实现，需容忍非确定或用替代算子；`assert_close` 对 fp16/bf16 需放宽容差；golden file 适合生成任务但需随版本更新。

#### 7. 常见追问及回答要点

1. 如何消除 flaky？——固定种子、隔离状态、避免时间/顺序依赖。
2. 容差怎么定？——按 dtype 与算子累积误差估计，fp16 放宽容差。
3. 门禁分级意义？——快速反馈与关键保护兼顾。

#### 8. 易错点与反例

1. 用例共享全局随机状态，顺序变了结果就变。
2. 断言过松（仅 shape），数值错误漏检。
3. 无确定性配置，同一用例偶发失败。

#### 9. 实践与验证

对同一用例连跑 100 次断言无 flaky；注入数值 bug 断言测试捕获；记录种子与环境断言可复现。无 GPU 依赖用例本地执行 [待验证]。

#### 10. 评分标准

I034 评分：不合格只会“跑通”；合格能固定种子与 oracle；良好能分级门禁与隔离状态；优秀/专家能注入缺陷验证测试有效性并治理确定性/容差问题。

### I035. 测试设计：张量/设备

- 难度：中等级
- 知识域：K01 张量/设备
- 考察能力：前置条件、后置条件、断言与错误处理
- 题型：代码/伪代码/配置分析
- 面试频率：高频
- 前置知识：shape、stride、dtype、device
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- 张量测试的 oracle 通常是 NumPy/纯 Python 参考实现，或解析解；用 `assert_close` 做数值对比。
- 输入矩阵要覆盖：0 维、标量与张量混合、空张量、非连续（转置后）、广播边界（维度 1 与不等）、极值（`inf`/`nan`）、dtype 组合。
- 设备矩阵覆盖 CPU 与 CUDA；跨设备一致性用同一 seed 生成数据后比较。
- 性质断言适合覆盖大范围输入：交换律、形状满足广播、连续化后值不变。

#### 2. 问题分析

为张量/设备设计测试矩阵。两个不变量：(1) 输出在所有合法输入组合下形状等于广播预测；(2) 非连续输入经 `.contiguous()` 后数值不变。边界反例：只测了连续、同 dtype、CPU 的方形张量，漏掉非连续与广播，导致 `view` 在真实转置输入上报错未在 CI 暴露。

#### 3. 具体答案

面试中可以这样回答：

> 我按行列组织输入矩阵：行是 shape（标量、1D、2D、广播、空），列是 dtype（fp32/fp16/int）与 layout（连续、转置、expand），再加设备维度 CPU/CUDA。oracle 用 NumPy 做参考，`assert_close` 对比。对形状类算子我用性质断言，比如输出形状等于广播预测、连续化后值不变、transpose 两次回到原布局。对 `view` 这类算子专门测非连续输入应报错，测 `reshape` 应成功。极值输入测 nan/inf 的传播。

#### 4. 技术洞察

```
grid = shapes × dtypes × layouts × devices
oracle: numpy ref 或解析解
property: out.shape == broadcast(a.shape, b.shape)
          a.t().t() == a
```

````python
@pytest.mark.parametrize("shape", [( ), (5,), (3,1), (3,4), (2,3,4)])
@pytest.mark.parametrize("dtype", [torch.float32, torch.float16, torch.int64])
def test_add(shape, dtype):
    a = torch.randn(shape) if dtype.is_floating_point else torch.randint(0,9,shape)
    ref = (a.numpy().astype...)  # numpy oracle
    assert torch.allclose(op(a), ref_tensor, atol=tol(dtype))
````

#### 5. 图文说明

| 维度 | 取值 | 覆盖目标 |
|---|---|---|
| shape | 0,1,2,广播,空 | 形状/广播 |
| layout | 连续, transpose | 连续性 |
| dtype | fp32,fp16,int | 提升/截断 |
| device | CPU,CUDA | 一致性 |

#### 6. 拓展知识

`torch.testing.assert_close` 的 dtype 感知容差优于手写 atol；性质测试可用 `hypothesis` 生成形状；跨设备一致性要求先 `torch.manual_seed` 再生成。

#### 7. 常见追问及回答要点

1. oracle 从哪来？——NumPy/解析解/参考实现三选一。
2. 如何覆盖非连续？——先 transpose 再测，也测 view 应报错。
3. 容差怎么定？——按 dtype，fp16 放宽。

#### 8. 易错点与反例

1. 只测连续方形张量，转置输入漏测。
2. 只测 fp32，fp16 溢出路径未覆盖。
3. 用固定 atol 对 fp16 断言，误报失败。

#### 9. 实践与验证

用参数化矩阵跑全部组合；对每个失败项最小化输入；断言形状与数值双正确。无 GPU 依赖部分本地跑，CUDA 部分 [待验证]。

#### 10. 评分标准

I035 评分：不合格只测一种输入；合格能构造输入矩阵；良好能用参考 oracle 与性质断言；优秀/专家能覆盖非连续/极值/多 dtype 并用最小化失败定位。

### I036. 测试设计：autograd

- 难度：中等级
- 知识域：K02 autograd
- 考察能力：前置条件、后置条件、断言与错误处理
- 题型：代码/伪代码/配置分析
- 面试频率：高频
- 前置知识：requires_grad、leaf、view、detach、反向
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- autograd 测试的核心 oracle 是数值梯度：`gradcheck` 用有限差分对解析梯度做对比，是自动微分的金标准测试。
- 输入矩阵覆盖：叶子/非叶子、requires_grad 组合、视图（view/transpose/expand）、广播、in-place、多次使用同一张量。
- 故障注入：故意返回错误的 backward 或修改保存张量，测试应能捕获。
- 复现性 oracle：相同 seed 下梯度应可复现；`retain_graph` 与二次反向需单独测。

#### 2. 问题分析

为自动微分设计测试。两个不变量：(1) 解析梯度与数值梯度在容差内一致；(2) 视图/广播的反向满足形状与求和语义。边界反例：只对 `requires_grad=True` 的单输入测 gradcheck，漏掉了广播输入，导致 `expand` 反向漏求和未被发现。

#### 3. 具体答案

面试中可以这样回答：

> 我用 `gradcheck` 做主要 oracle，对函数和输入做参数化：单输入、多输入、视图输入、广播输入都要测。对视图我额外断言反向梯度形状等于输入形状，广播维梯度等于使用次数乘积。故障注入上我写一个故意错误的反向函数，断言 gradcheck 失败，证明测试有效。二次反向和 retain_graph 单独用例。全流程固定种子保证可复现。

#### 4. 技术洞察

```
gradcheck(f, x):  max|∂f/∂x_analytic − (f(x+ε)−f(x−ε))/(2ε)| < tol
view/expand backward: dInput = reduce_to_input_shape(dOut)
```

````python
x = torch.randn(2, 3, dtype=torch.double, requires_grad=True)
assert gradcheck(lambda t: t.expand(4, 2, 3).sum(), (x,))
# 故障注入
def bad_backward(g): return g * 2      # 应被 gradcheck 捕获
````

#### 5. 图文说明

| 维度 | 取值 | 关注点 |
|---|---|---|
| 输入角色 | 叶子/非叶子 | grad 是否写入 |
| 变换 | view/transpose/expand | 反向散射 |
| 反向次数 | 1/2 | retain_graph |
| 注入 | 错误 backward | 测试有效性 |

#### 6. 拓展知识

`gradcheck` 需 double 精度；对不可微点（ReLU 在 0、abs 在 0）需避开；forward-mode (`jacfwd`) 与 vmap 是补充测试 手段。

#### 7. 常见追问及回答要点

1. gradcheck 为什么要 double？——有限差分精度不足，fp32 会误判。
2. 广播反向如何断言？——梯度形状与输入一致，广播维求和。
3. 如何证明测试有效？——注入错误反向，断言测试变红。

#### 8. 易错点与反例

1. 只测单输入，广播/视图反向漏测。
2. 用 fp32 跑 gradcheck，容差误报。
3. 测试没有负样本，错误反向也能通过。

#### 9. 实践与验证

对 view/expand/transpose 组合跑 gradcheck；二次反向用例；注入错误 backward 验证测试能捕获。无 GPU 依赖。

#### 10. 评分标准

I036 评分：不合格只比对前向；合格能用 gradcheck；良好能覆盖视图/广播与二次反向；优秀/专家能通过故障注入证明测试有效性并处理不可微点。

### I037. 测试设计：Module/状态

- 难度：中等级
- 知识域：K03 Module/状态
- 考察能力：前置条件、后置条件、断言与错误处理
- 题型：代码/伪代码/配置分析
- 面试频率：高频
- 前置知识：参数、buffer、模式和 state_dict
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- Module 测试 oracle：同一 seed 下两次构造/加载的模型输出应一致；`state_dict` 往返后输出不变。
- 输入矩阵覆盖：train/eval 模式、有无 buffer、参数共享、嵌套模块、hook 存在与否。
- buffer 测试：`persistent=False` 的 buffer 不进 `state_dict`；训练统计量应随模式切换更新。
- 故障注入：故意改一个参数名或漏存 buffer，测试应捕获加载差异。

#### 2. 问题分析

为 Module/状态设计测试。两个不变量：(1) `state_dict` 保存-加载往返后，模型在相同输入下输出不变；(2) train/eval 切换导致 BN/Dropout 行为按预期改变。边界反例：只测参数没测 buffer，BN 的 running stats 在加载后未恢复，推理输出与保存时不同却通过测试。

#### 3. 具体答案

面试中可以这样回答：

> 我测四件事：一是状态往返——保存 `state_dict`、重新构造、加载、用同一输入断言输出一致；二是模式切换——eval 下 dropout 关闭、BN 用 running stats，train 下统计量更新；三是 buffer——断言需要持久化的 buffer 在 `state_dict` 里，`persistent=False` 的不在；四是注入——故意改键名或漏 buffer，断言加载报错或输出不同。参数共享我会断言两处引用同一对象。

#### 4. 技术洞察

````python
m1 = Net(); m2 = Net()
m1.eval(); m2.eval()
sd = m1.state_dict()
m2.load_state_dict(sd); m2.eval()
x = torch.randn(4, 8)
assert torch.allclose(m1(x), m2(x))
# buffer 检查
assert "running" in sd
assert "cache" not in sd        # persistent=False
# 共享
assert net.a.weight is net.b.weight
````

#### 5. 图文说明

| 用例 | oracle | 注入 |
|---|---|---|
| 往返 | 输出一致 | 改键名 |
| 模式 | BN/Dropout 行为 | 漏 eval |
| buffer | 键存在性 | 漏 buffer |
| 共享 | 同一对象 | 复制权重 |

#### 6. 拓展知识

BN 在 eval 用 running stats，在 train 用 batch 统计并更新 buffer；Dropout 在 eval 恒等；hook 可测中间激活但需注意清理避免泄漏。

#### 7. 常见追问及回答要点

1. 为什么往返比对要在 eval 下？——消除 dropout 随机性，聚焦状态正确性。
2. buffer 何时不持久化？——如缓存的 mask，可由输入重建。
3. 参数共享怎么测？——断言 `is` 同一对象，且更新一处另一处变化。

#### 8. 易错点与反例

1. 换 eval 前后直接比输出，训练态统计不同导致误判。
2. 只测参数忽略 buffer，BN 加载后未恢复。
3. 参数共享被 copy 打断，测试未覆盖。

#### 9. 实践与验证

往返输出 `assert_close`；train/eval 行为断言；buffer 键存在性；参数共享对象身份断言。无 GPU 依赖。

#### 10. 评分标准

I037 评分：不合格只比参数量；合格能做 state_dict 往返；良好能覆盖模式、buffer 与共享；优秀/专家能注入键/buffer 错误验证测试并处理 hook 与清理。

### I038. 测试设计：数据管道

- 难度：中等级
- 知识域：K04 数据管道
- 考察能力：前置条件、后置条件、断言与错误处理
- 题型：代码/伪代码/配置分析
- 面试频率：高频
- 前置知识：Dataset、Sampler、collate、padding、版本
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- 数据管道测试 oracle：一个 epoch 消费的索引集合应等于数据集全集；相同 seed 下 batch 序列可复现。
- 输入矩阵覆盖：`num_workers` 0/1/>1、`collate_fn` 自定义、变长序列、`IterableDataset`、`pin_memory`。
- 故障注入：让 collate 丢一个字段或 worker 种子不设，测试应暴露。
- 指标：索引覆盖率、样本重复率、每 worker 负载均衡。

#### 2. 问题分析

为数据管道设计测试。两个不变量：(1) 一个 epoch 的样本不重不漏；(2) 多 worker 下随机变换可复现且各 worker 互不相同。边界反例：`num_workers=0` 时一切正常，切到 `num_workers=4` 后 `IterableDataset` 每个 worker 读全量，样本重复，只有覆盖多 worker 的测试才能发现。

#### 3. 具体答案

面试中可以这样回答：

> 我测三组：索引覆盖——用 `sampler` 收集一个 epoch 的全部索引，断言与全集相等且无重复；collate——构造变长样本，断言 batch 形状、mask、dtype 正确，pad 位不参与 loss；多 worker——分别在 0/1/4 worker 下跑，断言索引集合一致，且固定种子下 batch 序列可复现，各 worker 的增强随机序列不同。对 `IterableDataset` 专门测多 worker 是否重复。故障注入让 collate 漏字段，断言测试变红。

#### 4. 技术洞察

````python
def test_index_coverage():
    ds = RangeDataset(1000)
    seen = []
    for batch in DataLoader(ds, batch_size=16, shuffle=True, num_workers=4):
        seen.extend(batch.tolist())
    assert sorted(seen) == list(range(1000))
# 多 worker 复现
def test_determinism():
    a = list(collect(num_workers=4, seed=0))
    b = list(collect(num_workers=4, seed=0))
    assert a == b
````

#### 5. 图文说明

| 用例 | 指标 | 注入 |
|---|---|---|
| 覆盖 | 集合等于全集 | 漏样本 |
| collate | shape/mask | 丢字段 |
| 复现 | 两次序列相同 | 不设 worker 种子 |
| 分片 | 无重复 | IterableDataset |

#### 6. 拓展知识

`worker_init_fn` 需同时设 `random`/`numpy`/`torch` 种子；`persistent_workers` 影响进程重启与状态；分布式下还要叠加 rank 维度做覆盖测试。

#### 7. 常见追问及回答要点

1. 如何证明不重不漏？——收集索引集合与全集比对。
2. 多 worker 如何复现？——固定主种子并由 `worker_init_fn` 派生 worker 种子。
3. IterableDataset 怎么测？——多 worker 下断言样本不重复。

#### 8. 易错点与反例

1. 只测 `num_workers=0`，多 worker 分片 bug 漏检。
2. collate 的 pad 位参与 loss，测试未断言 ignore。
3. worker 种子不设，两次运行 batch 顺序不同。

#### 9. 实践与验证

索引覆盖单测；多 worker 复现单测；IterableDataset 重复检测；collate 字段注入。多进程数字 [待验证]。

#### 10. 评分标准

I038 评分：不合格只跑一遍 loader；合格能测覆盖；良好能覆盖 collate、多 worker 复现与分片；优秀/专家能注入字段错误、处理 IterableDataset 并量化负载均衡。

### I039. 测试设计：训练/优化器

- 难度：中等级
- 知识域：K05 训练/优化器
- 考察能力：前置条件、后置条件、断言与错误处理
- 题型：代码/伪代码/配置分析
- 面试频率：高频
- 前置知识：forward、backward、累积、optimizer、scheduler
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- 优化器测试 oracle：手写一步更新公式（如 SGD `θ←θ−lr·g`）与 `optimizer.step` 结果对比。
- scheduler 测试 oracle：按定义手算每个 epoch/step 的 lr 序列，与 `param_groups` 实际值比对。
- 输入矩阵：优化器类型（SGD/Adam/AdamW）、是否有 weight decay、是否有 momentum、是否梯度累积。
- 故障注入：不 unscale、漏 zero_grad、scheduler 频率错误，测试应暴露。

#### 2. 问题分析

为训练/优化器设计测试。两个不变量：(1) 单步更新结果等于手写公式；(2) scheduler 的 lr 序列等于定义序列。边界反例：Adam 的偏差修正在第一步最显著，若测试从第 2 步开始比对，可能漏掉 `1−β^t` 修正错误。

#### 3. 具体答案

面试中可以这样回答：

> 我用“手写参考”做 oracle：SGD 一步 `θ−lr·g`，Adam 一步套完整公式含偏差修正。scheduler 我手算前几个 epoch 的 lr 与 `param_groups` 实际值逐点比对。梯度累积我测等效性：累积 K 步的更新应与单步大 batch 的更新接近（浮点容差内）。故障注入包括漏 `zero_grad` 和 scheduler 频率翻倍，断言测试能捕获。AdamW 的 weight decay 单独测，确认它作用在参数而非梯度。

#### 4. 技术洞察

```
SGD:  θ1 = θ0 − lr·g
Adam: m1=(1−β1)g; v1=(1−β2)g²
      m̂=m1/(1−β1^t); v̂=v1/(1−β2^t)
      θ1 = θ0 − lr·m̂/(√v̂+ε)
StepLR: lr_e = lr0 · γ^{floor(e/step_size)}
```

````python
opt = torch.optim.SGD([p], lr=0.1)
p.grad = torch.ones_like(p)
opt.step()
assert torch.allclose(p, p0 - 0.1)
````

#### 5. 图文说明

| 用例 | oracle | 注入 |
|---|---|---|
| SGD/Adam 单步 | 手写公式 | 错误动量 |
| scheduler | 定义序列 | 频率翻倍 |
| 累积 | 等效大 batch | 漏 zero_grad |
| AdamW | decay 作用于参数 | 作用于梯度 |

#### 6. 拓展知识

Adam 的 `amsgrad` 维护 `max_exp_avg_sq`；`ReduceLROnPlateau` 依赖指标需单独测；混合精度下 optimizer 状态以 fp32 保存，测试需考虑。

#### 7. 常见追问及回答要点

1. 为什么手写 oracle？——独立于框架实现，能捕获公式错误。
2. 第一步为什么关键？——偏差修正因子最大。
3. AdamW 与 Adam 差别？——decay 作用于参数而非梯度。

#### 8. 易错点与反例

1. 从第 2 步开始比对，漏掉偏差修正错误。
2. scheduler 频率错，测试只测了一个 epoch。
3. AdamW 测成 L2 正则，语义混淆。

#### 9. 实践与验证

单步/多步手写公式比对；scheduler 序列逐点断言；累积等效性 `assert_close`；注入漏 zero_grad 断言梯度膨胀。无 GPU 依赖。

#### 10. 评分标准

I039 评分：不合格只跑训练；合格能单步比对；良好能覆盖 scheduler、累积与 AdamW；优秀/专家能通过故障注入验证并解释偏差修正与 decay 语义。

### I040. 测试设计：AMP/恢复

- 难度：中等级
- 知识域：K06 AMP/恢复
- 考察能力：前置条件、后置条件、断言与错误处理
- 题型：代码/伪代码/配置分析
- 面试频率：高频
- 前置知识：autocast、scaler、快照、随机状态
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- AMP 测试 oracle：FP16 + scaler 与 FP32 的 loss 轨迹在小步数内接近；溢出时 scaler 应跳过本次更新。
- 恢复测试 oracle：从 checkpoint N 恢复后继续的 loss 序列应与不中断基线在容差内一致。
- 故障注入：注入 `inf` 到梯度触发溢出，断言 scaler scale 减半且参数未更新；漏存 scaler 断言恢复后 scale 不一致。
- 输入矩阵：FP16/BF16、有无 scaler、溢出/无溢出、恢复在不同 step 边界。

#### 2. 问题分析

为 AMP/恢复设计测试。两个不变量：(1) 溢出时参数保持不变且 scale 下降；(2) 恢复后 scaler 与 RNG 状态连续。边界反例：若测试只在无溢出情况下验证，漏掉溢出跳过逻辑，实际训练遇到 spike 时可能错误更新或在 fp16 下静默 nan。

#### 3. 具体答案

面试中可以这样回答：

> 我分两块测。AMP：对照 FP32 与 FP16 的 loss 前若干步接近；故障注入一个巨大 loss 触发 inf，断言 `scaler.step` 跳过了更新、参数没变、scale 减半，`scaler.update` 后 scale 处于合理范围。恢复：跑到 step N 存 checkpoint，记录之后的 loss 与 scale 序列；从 N 恢复再跑，比较两条序列在容差内一致，并断言 `scaler.get_scale` 和 RNG 状态恢复正确。BF16 单独测不需要 scaler 的路径。

#### 4. 技术洞察

```
overflow: found_inf=True ⇒ skip step; scale /= 2
no overflow for growth_interval ⇒ scale *= growth_factor
resume: load model+optim+scaler+rng → loss_seq matches baseline
```

````python
scaler.scale(loss).backward()
scaler.unscale_(opt)
with torch.no_grad(): p.grad.fill_(float("inf"))   # 注入溢出
before = p.clone()
scaler.step(opt); scaler.update()
assert torch.equal(p, before) and scaler.get_scale() < prev_scale
````

#### 5. 图文说明

| 用例 | oracle | 注入 |
|---|---|---|
| 数值 | 与 FP32 接近 | 降精度算子 |
| 溢出 | 跳过+scale减半 | 梯度 inf |
| 恢复 | 序列一致 | 漏存 scaler |
| BF16 | 无需 scaler | 误加 scaler |

#### 6. 拓展知识

`GradScaler` 的 `growth_interval` 与 `backoff_factor` 可配置；BF16 一般不需 scaler；恢复的严格 bitwise 一致还依赖确定性算法。

#### 7. 常见追问及回答要点

1. 如何注入溢出？——把梯度或 loss 置 inf。
2. 恢复比什么？——loss 与 scale 序列、RNG 状态。
3. BF16 要测什么？——无需 scaler 下的数值正确性。

#### 8. 易错点与反例

1. 只测无溢出，跳过逻辑漏检。
2. 恢复只比 loss 不比 scale/RNG，状态续接错误。
3. 溢出测试没断言参数不变，错误更新漏检。

#### 9. 实践与验证

溢出单测断言参数不变与 scale 减半；恢复序列对比；BF16 路径单独测。GPU 数字 [待验证]。

#### 10. 评分标准

I040 评分：不合格不分 FP16/BF16；合格能对比 FP32；良好能注入溢出并测恢复序列；优秀/专家能覆盖 scale/RNG 连续性与确定性要求并量化容差。

### I041. 测试设计：分布式

- 难度：中等级
- 知识域：K07 分布式
- 考察能力：正确性、性能、成本、运维与迁移条件
- 题型：对比与取舍
- 面试频率：高频
- 前置知识：rank、DDP、FSDP、collective、容错
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- 分布式测试 oracle：DDP 的全局梯度应等于单卡大 batch 的梯度；各 rank 参数在 `step` 后一致。
- 输入矩阵：world_size 2/4、数据能否被 world_size 整除、有无 unused 参数、是否梯度累积。
- 故障注入：杀掉一个 rank，观察其余 rank 是否超时；注入集合通信不一致，断言测试捕获挂起。
- 指标：参数一致性、样本覆盖、通信/计算时间重叠比。

#### 2. 问题分析

为分布式设计测试。两个不变量：(1) 各 rank 更新后参数逐位一致；(2) 各 rank 采样并集等于全集且无重叠。边界反例：只在单机模拟（未真正多进程）测试，掩盖了集合通信顺序不一致导致的挂起，只有真多进程 + 超时断言才能覆盖。

#### 3. 具体答案

面试中可以这样回答：

> 我用“单卡大 batch”作为 DDP 的 oracle：把 DDP 的全局梯度 `all_reduce` 后与单卡等效 batch 的梯度比对。参数一致性上每步 `all_gather` 参数断言所有 rank 相等。数据上收集各 rank 索引断言并集为全集、交集为空。故障注入包括杀死一个 rank 断言其余在超时内退出，以及让一个 rank 跳过一次 collective 断言触发超时。我会设短超时让测试快速暴露挂起。

#### 4. 技术洞察

```
g_ddp = (1/W) Σ_r g_r  ≟  g_single_batch(B·W)
∀r,r': params_r == params_r'
set(⋃ idx_r) == full; ⋂ idx_r == ∅
```

````python
# 启动 2 进程，设 timeout=30s
dist.init_process_group("gloo", timeout=timedelta(seconds=30))
...
t = [p.detach().clone() for p in model.parameters()]
dist.all_reduce(grad); grad /= world_size
assert torch.allclose(grad, grad_single_big_batch)
````

#### 5. 图文说明

| 用例 | oracle | 注入 |
|---|---|---|
| 梯度 | 单卡大 batch | 少调 collective |
| 参数 | all_gather 一致 | 改一个 rank |
| 数据 | 并集/交集 | 重复分片 |
| 容错 | 超时退出 | kill rank |

#### 6. 拓展知识

gloo 可在 CPU 上跑多进程测试，便于 CI；NCCL 需 GPU；`torch.multiprocessing.spawn` 或 `pytest` 的 multiprocessing 插件可组织多进程用例；`store` 与 `init_method` 需端口隔离避免 CI 冲突。

#### 7. 常见追问及回答要点

1. 为什么梯度要平均？——对应单卡大 batch 均值 loss。
2. 如何快速暴露挂起？——设短超时。
3. CPU 上怎么测？——用 gloo 后端跑多进程。

#### 8. 易错点与反例

1. 只在单进程模拟，集合通信 bug 漏检。
2. 无超时，挂起测试一直卡住。
3. 只比 loss 不比参数，错位未发现。

#### 9. 实践与验证

2 进程 gloo 梯度/参数一致性；索引并集交集断言；kill rank 超时断言。GPU/NCCL 部分 [待验证]。

#### 10. 评分标准

I041 评分：不合格只跑单机；合格能测参数一致；良好能设 oracle 与超时并注入故障；优秀/专家能用 CPU 后端组织多进程测试并覆盖 unused/累积场景。

### I042. 测试设计：编译栈

- 难度：中等级
- 知识域：K08 编译栈
- 考察能力：正确性、性能、成本、运维与迁移条件
- 题型：对比与取舍
- 面试频率：高频
- 前置知识：Dynamo、AOTAutograd、Inductor、guard、缓存
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- 编译测试 oracle：`torch.compile` 后模型输出与 eager 在容差内一致；eager 是参考。
- 输入矩阵：动态/静态 shape、图内/图外算子、dtype、device，以及是否触发 CUDA graphs。
- 指标：重编译次数、graph break 数量、加速比；故障注入：动态控制流触发 graph break。
- 复现：固定输入时重编译应为 0。

#### 2. 问题分析

为编译栈设计测试。两个不变量：(1) 编译输出的数值与 eager 一致；(2) 固定输入下编译一次后不再重编译。边界反例：只测了一个 shape，编译“看起来正常”，但第二个 shape 触发重编译甚至产生不一致结果，测试应覆盖多 shape。

#### 3. 具体答案

面试中可以这样回答：

> 我用 eager 作 oracle，对编译后的模型在相同输入上 `assert_close`。输入矩阵覆盖静态与动态 shape、不同 dtype、含 graph break 的算子。指标上我会统计重编译次数：固定输入重复调用应为 0；改变 shape 时若开了动态 shape 也不应爆炸式重编译。故障上我注入 `.item()` 或数据依赖控制流，断言 `torch._dynamo.explain` 报告 graph break，证明检测有效。

#### 4. 技术洞察

```
oracle: compiled(x) == eager(x)  (atol)
metric: recompile_count(fixed x) == 0
        graph_breaks == 0 (理想)
inject: .item()/data-dependent → graph break
```

````python
f = torch.compile(model)
assert torch.allclose(f(x), model_eager(x), atol=1e-4)
counts = torch._dynamo.utils.counters["stats"]   # 观测
````

#### 5. 图文说明

| 用例 | oracle/指标 | 注入 |
|---|---|---|
| 数值 | 与 eager 一致 | 不支持算子 |
| 重编译 | 固定输入=0 | 变 shape |
| graph break | explain 计数 | .item() |
| 动态 shape | 多 shape 一致 | 未标记动态 |

#### 6. 拓展知识

`torch._dynamo.config` 可调重编译上限用于测试；CUDA graphs 需静态输入，测试时避免随机形状；`cache_size_limit` 影响缓存淘汰。

#### 7. 常见追问及回答要点

1. eager 为什么是 oracle？——编译是优化，语义应与 eager 等价。
2. 如何数重编译？——用 dynamo counters 或 explain。
3. graph break 怎么测？——注入触发点断言 explain 报告。

#### 8. 易错点与反例

1. 只测一个 shape，动态行为漏检。
2. 不比对数值，编译错误结果上线。
3. 忽略重编译，性能退化无告警。

#### 9. 实践与验证

多 shape 数值一致；固定输入重编译=0；注入 graph break 断言 explain。GPU 数字 [待验证]。

#### 10. 评分标准

I042 评分：不合格只调用 compile；合格能比对数值；良好能统计重编译与 graph break；优秀/专家能注入退化、覆盖动态 shape 并给出加速比指标。

### I043. 测试设计：性能

- 难度：中等级
- 知识域：K09 性能
- 考察能力：正确性、性能、成本、运维与迁移条件
- 题型：对比与取舍
- 面试频率：高频
- 前置知识：关键路径、带宽、算力、通信、显存、统计
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- 性能测试 oracle：基线版本或理论 roofline；指标包括 step 时间 P50/P95、吞吐、峰值显存。
- 输入矩阵：batch size、worker 数、精度、是否编译；回归判定用阈值而非绝对值。
- 故障注入：人为加同步、减小 batch，断言指标退化被捕获。
- 复现：固定输入、预热、多次采样。

#### 2. 问题分析

为性能设计测试。两个不变量：(1) 同配置下多次测量方差小；(2) 相对基线的退化超过阈值即判定回归。边界反例：CI 机器负载波动导致 P50 稳定但 P95 抖动，只看均值会误判通过，需同时设尾延迟阈值。

#### 3. 具体答案

面试中可以这样回答：

> 我把性能当回归测试：固定输入与配置，预热后多次采样，记录 step 时间 P50/P95、吞吐与峰值显存。oracle 是基线或 roofline 预测，判定用相对阈值，比如 P50 超过基线 X% 即失败。输入矩阵覆盖 batch、worker、精度、编译开关。故障注入是在热路径加一次 `.item()`，断言 P95 明显上升，证明测试灵敏。CI 上我会用足够多的重复次数压低噪声，或改用相对同一机器基线的比值。

#### 4. 技术洞察

```
metric = {t_p50, t_p95, throughput, peak_mem}
regress = metric / baseline > 1 + threshold
warmup=True; iters≫1; 报告方差
```

````python
ts = [time_step() for _ in range(50)]
p50, p95 = np.percentile(ts, 50), np.percentile(ts, 95)
assert p50 < baseline_p50 * 1.05
assert p95 < baseline_p95 * 1.15
````

#### 5. 图文说明

| 指标 | 用途 | 注入 |
|---|---|---|
| P50 | 稳态 | 加同步 |
| P95 | 抖动 | 数据长尾 |
| 吞吐 | 扩展性 | 减 batch |
| 峰值显存 | 容量 | 漏释放 |

#### 6. 拓展知识

性能 CI 常用同一机器跑基线与候选做比值，剔除硬件差异；尾延迟常由 allreduce 抖动或 checkpoint 写盘造成；显存回归用 `max_memory_allocated` 断言。

#### 7. 常见追问及回答要点

1. 为什么用相对阈值？——跨机器绝对时间不可比。
2. 如何压噪声？——预热+多次采样+同机基线。
3. 尾延迟为何重要？——均值掩盖周期性卡顿。

#### 8. 易错点与反例

1. 只测一次，冷启动当稳态。
2. 只看 P50，P95 退化漏检。
3. 用绝对阈值，换机器即误报。

#### 9. 实践与验证

采样曲线报告 P50/P95；注入同步断言 P95 上升；跨 commit 比较比值。硬件相关数字 [待验证]。

#### 10. 评分标准

I043 评分：不合格只报一个时间；合格能测吞吐；良好能设相对阈值与尾延迟；优秀/专家能注入退化验证灵敏度、控制噪声并建立显存回归门禁。

### I044. 测试设计：导出部署

- 难度：中等级
- 知识域：K10 导出部署
- 考察能力：正确性、性能、成本、运维与迁移条件
- 题型：对比与取舍
- 面试频率：高频
- 前置知识：state_dict、Export、runtime、schema、迁移
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- 导出测试 oracle：导出图/ONNX 与 eager 在同一输入下输出一致。
- 输入矩阵：静态/动态 shape、dtype、opset 版本、运行时（ORT/TensorRT）。
- 故障注入：把动态维写死，断言不同 shape 报错；漏 eval 断言 dropout 导致输出不稳定。
- 指标：数值最大偏差、加载成功率、跨运行时一致性。

#### 2. 问题分析

为导出部署设计测试。两个不变量：(1) 导出物在声明输入域内与 eager 数值等价；(2) 动态轴在多个 shape 下都能正确推理。边界反例：只在一个 shape 验证，动态轴声明了但未测第二个 shape，实际上图仍是特化的，上线换长度即错。

#### 3. 具体答案

面试中可以这样回答：

> 我用 eager 作 oracle，对导出物做数值对比。输入矩阵覆盖静态与动态 shape、不同 dtype、不同 opset 与运行时。动态轴我至少测两个不同的 batch/序列长度，断言都能推理且结果正确。故障注入是把动态维写死或漏 eval，前者断言换 shape 报错、后者断言输出不稳定，证明测试覆盖到位。跨运行时我比对 ORT 与 TensorRT 输出，偏差在容差内。

#### 4. 技术洞察

```
oracle: exported(x) ≈ eager(x)
dynamic: exported(x1) ok and exported(x2) ok, x1.shape≠x2.shape
runtime parity: ORT(x) ≈ TRT(x) (atol)
```

````python
for s in [8, 16, 32]:
    x = torch.randn(2, s)
    assert torch.allclose(model(x), ort_run(x), atol=1e-4)
````

#### 5. 图文说明

| 用例 | oracle | 注入 |
|---|---|---|
| 静态数值 | eager | 不支持算子 |
| 动态 shape | 多 shape 一致 | 写死维度 |
| eval | 输出稳定 | 漏 eval |
| 运行时 | ORT↔TRT | 版本差异 |

#### 6. 拓展知识

不同 opset 支持的算子集合不同；TensorRT 的量化校准会改变数值；`torch.export` 的 `dynamic_shapes` 约束比 ONNX 动态轴更强，可在导出期捕获形状问题。

#### 7. 常见追问及回答要点

1. 为什么测两个 shape？——验证动态轴真正生效。
2. 跨运行时为何会有偏差？——融合顺序与量化差异。
3. 漏 eval 怎么测？——多次推理断言输出一致（dropout 应关闭）。

#### 8. 易错点与反例

1. 只测一个 shape，动态轴形同虚设。
2. 不比对数值，导出错误结果上线。
3. 不测跨运行时，部署端偏差漏检。

#### 9. 实践与验证

多 shape 数值对比；漏 eval 稳定性测试；ORT/TRT 交叉对比。运行时数字 [待验证]。

#### 10. 评分标准

I044 评分：不合格只导出不验证；合格能比 eager；良好能测动态 shape 与 eval；优秀/专家能跨运行时对比并注入形状/eval 错误验证覆盖。

### I045. 测试设计：自定义算子

- 难度：中等级
- 知识域：K11 自定义算子
- 考察能力：正确性、性能、成本、运维与迁移条件
- 题型：对比与取舍
- 面试频率：高频
- 前置知识：schema、dispatcher、stream、autograd、ABI
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- 自定义算子测试 oracle：参考实现（eager 组合算子）与 `gradcheck` 是双重 oracle。
- 输入矩阵：连续/非连续输入、不同 dtype、边界尺寸、多输入、原地/非原地。
- 故障注入：错误 backward、错误别名标注、错误 stream，测试应捕获。
- 指标：数值最大偏差、梯度一致性、多 stream 下无竞争。

#### 2. 问题分析

为自定义算子设计测试。两个不变量：(1) 前向输出与参考实现一致；(2) 反向梯度与 `gradcheck` 一致且形状匹配输入。边界反例：只测了连续输入，非连续输入下 kernel 假设 stride 连续，读到错误数据，测试应包含非连续输入。

#### 3. 具体答案

面试中可以这样回答：

> 前向我用 eager 组合算子作参考，`assert_close` 对比，覆盖连续与非连续输入、多 dtype 和边界尺寸。反向用 `gradcheck` 校验，并断言梯度形状与输入一致。故障注入包括改错 backward、把本应原地写标成 pure，断言 gradcheck 或输出对比变红。若算子涉及 stream，我会在多 stream 下运行并在同步后比对结果，确认无竞争。

#### 4. 技术洞察

```
fwd: op(x) ≈ ref(x)
bwd: gradcheck 通过, grad.shape == x.shape
functional: 原地标注 ⇒ 功能化应正确拷贝
stream: run on two streams, sync, compare
```

````python
x = torch.randn(4, 8, dtype=torch.double, requires_grad=True)
assert torch.allclose(op(x), ref(x), atol=1e-5)
assert torch.autograd.gradcheck(op, (x,))
xt = x.detach().t().requires_grad_(True)     # 非连续
assert torch.allclose(op(xt.copy()), ref(xt.copy()))
````

#### 5. 图文说明

| 用例 | oracle | 注入 |
|---|---|---|
| 前向 | 参考实现 | 错误公式 |
| 反向 | gradcheck | 错误 backward |
| 布局 | 连续/非连续 | 假设连续 |
| stream | 多流比对 | 用默认流 |

#### 6. 拓展知识

功能化要求别名标注正确；`register_fake` 影响编译期形状推导；ABI 兼容影响扩展能否加载；`torch.compile` 下跑一遍能发现 meta 缺失。

#### 7. 常见追问及回答要点

1. 双重 oracle 意义？——前向和后向分别独立验证。
2. 非连续输入为什么重要？——kernel 若假设连续会读错。
3. stream 怎么测？——多流运行后同步比对。

#### 8. 易错点与反例

1. 只测连续输入，stride 假设错误漏检。
2. 只测前向不测反向。
3. 用默认 stream，与相邻算子乱序未暴露。

#### 9. 实践与验证

前向参考对比；gradcheck；非连续输入；多 stream 同步比对；compile 下运行。GPU 数字 [待验证]。

#### 10. 评分标准

I045 评分：不合格只测前向一次；合格能对比参考；良好能加 gradcheck 与布局覆盖；优秀/专家能注入别名/stream 错误、测 compile 兼容并给出偏差指标。

### I046. 测试设计：测试治理

- 难度：中等级
- 知识域：K12 测试治理
- 考察能力：正确性、性能、成本、运维与迁移条件
- 题型：对比与取舍
- 面试频率：高频
- 前置知识：oracle、证据、日志、回滚、门禁、复盘
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- 测试治理的元测试：用变异测试（mutation testing）注入代码缺陷，检验测试集能否捕获。
- oracle 覆盖评估：每个断言是否绑定可判定的真值；统计有多少测试是“仅不抛异常”的弱断言。
- flaky 治理指标：同一用例多次运行的通过率，低于阈值即隔离。
- 门禁指标：测试分层覆盖（冒烟/关键/全量）与失败阻断级别。

#### 2. 问题分析

为测试治理本身设计测试。两个不变量：(1) 每个关键模块都有绑定 oracle 的断言；(2) 注入一个已知缺陷时至少一个测试变红。边界反例：测试集很大但全是“能跑通”，变异测试的 kill rate 接近 0，说明测试无效，规模不代表质量。

#### 3. 具体答案

面试中可以这样回答：

> 我治理测试本身分三层。第一层是 oracle 审计：扫描测试，标记只断言 shape 或不抛异常的弱断言，要求关键路径必须有数值或性质 oracle。第二层是变异测试：往被测代码注入缺陷，衡量 kill rate，kill 率低说明覆盖不足，补用例。第三层是稳定性：同一用例跑多次记录通过率，flaky 的隔离并定位根因。最后把结果接入门禁分级，关键模块 kill 率与 flaky 率达标才放行。

#### 4. 技术洞察

```
oracle_coverage = bound_assert_tests / total_tests
mutation_kill_rate = killed_mutants / injected_mutants
flaky_rate = 1 − pass_rate(repeats)
gate: layers = {smoke, critical, full}
```

````python
def test_softmax_has_oracle():
    src = inspect.getsource(test_softmax)
    assert ("allclose" in src) or ("assert_close" in src)   # 元测试
````

#### 5. 图文说明

| 层 | 指标 | 失败处置 |
|---|---|---|
| oracle 审计 | 弱断言占比 | 补 oracle |
| 变异测试 | kill rate | 补用例 |
| 稳定性 | flaky 率 | 隔离+定位 |
| 门禁 | 分层覆盖 | 分级阻断 |

#### 6. 拓展知识

`mutmut`/`cosmic-ray` 可自动做 Python 变异测试；pytest 的 `--reruns` 暴露 flaky；覆盖率工具只能说明执行过，不能说明断言有效，必须配合 oracle 审计。

#### 7. 常见追问及回答要点

1. 覆盖率够吗？——不够，覆盖不等于有效断言。
2. 如何量化测试有效性？——变异测试 kill rate。
3. flaky 怎么处理？——隔离随机源、重复运行统计。

#### 8. 易错点与反例

1. 以测试数量取胜，弱断言无捕捉力。
2. 只看行覆盖，oracle 缺失却绿灯。
3. flaky 被忽略，门禁间歇性红。

#### 9. 实践与验证

对关键模块跑变异测试得 kill rate；统计弱断言比例；重复运行得 flaky 率。无 GPU 依赖的元测试可本地跑 [待验证]。

#### 10. 评分标准

I046 评分：不合格以测试数量为质量；合格能审计 oracle；良好能用变异测试量化有效性；优秀/专家能建立 flaky 治理与门禁分级的闭环并给出指标。

### I047. 故障定位：张量/设备

- 难度：中等级
- 知识域：K01 张量/设备
- 考察能力：正确性、性能、成本、运维与迁移条件
- 题型：对比与取舍
- 面试频率：高频
- 前置知识：shape、stride、dtype、device
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- 张量/设备类故障的典型症状：`RuntimeError: Expected all tensors on same device`、shape 不匹配、`view` 报 stride 错、以及无报错的静默脏数据。
- 分层假设：设备层（CPU/CUDA 混用）、布局层（连续/非连续/stride）、dtype 层（提升/截断）、别名层（视图共享 storage）。
- 止损手段：在关键边界统一 `.to(device)`、显式 `contiguous()`、加断言。
- 根因常是“隐式假设”：某算子默认输入连续或同设备，而数据流某处破坏了它。

#### 2. 问题分析

按症状分层建假设树。两个不变量：(1) 报错行输入的 device/dtype/shape 与算子要求一致；(2) 视图与其基张量共享 storage。边界反例：`as_strided` 越界不报错但读到脏数据，属于最难定位的“静默”分支，假设树要包含内存越界一项。

#### 3. 具体答案

面试中可以这样回答：

> 我先分层定位。看到 device 报错，就在报错行打印两个张量的 device/dtype/shape，往上游追到第一次离开预期设备的地方，通常是某个常量或缓存没搬。看到 `view` 报 stride 错，说明上游有 transpose/切片，改成 `reshape` 或先 `contiguous`。最麻烦的是静默脏数据，我会检查是否有 `as_strided` 越界或视图写别名。止损是把设备/布局归一放到模块入口，根因是某处隐式假设，防护是加断言和单测。

#### 4. 技术洞察

```
symptom → 假设树:
  device?  → 打印 device，追边界
  shape?   → broadcast 规则
  stride?  → is_contiguous
  alias?   → data_ptr 比较
  overflow?→ as_strided 越界
```

````python
print(x.device, x.dtype, x.shape, x.stride(), x.is_contiguous())
# 视图别名检测
assert x.data_ptr() != y.data_ptr() or x.storage_offset() == y.storage_offset()
````

#### 5. 图文说明

| 症状 | 层 | 止损 |
|---|---|---|
| device mismatch | 设备 | 边界统一 to |
| view 报错 | 布局 | contiguous |
| shape 错 | 广播 | 尾部对齐 |
| 静默脏数据 | 别名/越界 | 断言+避免 as_strided |

#### 6. 拓展知识

`as_strided` 与 `expand` 的 stride 0 是静默风险来源；`torch.compile` 对布局有假设，非连续输入可能触发回退；诊断可开 `TORCH_SHOW_CPP_STACKTRACES` 看 C++ 栈。

#### 7. 常见追问及回答要点

1. 如何快速定位 device 错？——在报错行打印输入属性并追边界。
2. view 报错怎么办？——reshape 或 contiguous。
3. 静默脏数据如何查？——检查 as_strided/别名与越界。

#### 8. 易错点与反例

1. 到处 `.to` 掩盖根因，性能下降。
2. 用 `view` 处理非连续张量，报错才改。
3. as_strided 越界无报错，数据污染难查。

#### 9. 实践与验证

构造三类失败：跨设备相加、transpose 后 view、as_strided 越界；用打印与断言逐一确认根因；修复后加回归单测。无 GPU 依赖。

#### 10. 评分标准

I047 评分：不合格只会改报错行；合格能分层定位；良好能有假设树与止损；优秀/专家能处理静默脏数据并建立断言防护与回归。

### I048. 故障定位：autograd

- 难度：中等级
- 知识域：K02 autograd
- 考察能力：正确性、性能、成本、运维与迁移条件
- 题型：对比与取舍
- 面试频率：高频
- 前置知识：requires_grad、leaf、view、detach、反向
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- autograd 类故障症状：`.grad is None`、反向报图已释放、nan 梯度、version counter 报错。
- 分层假设：图连通性、叶子身份、in-place 版本、数值范围。
- 止损：开 anomaly 模式、注册参数 hook 打印 grad norm、临时 `retain_graph`。
- 根因多为“意外断链”或“数值溢出”，前者是结构问题，后者是数值问题。

#### 2. 问题分析

建假设树。两个不变量：(1) loss 到目标参数的图连通；(2) 反向期张量版本号未变。边界反例：梯度为 None 有时是参数真的没参与（条件分支），有时是图断链，两者修复方式不同，必须用 `grad_fn` 与 `allow_unused` 区分。

#### 3. 具体答案

面试中可以这样回答：

> 我先看 `loss.requires_grad` 和 `loss.grad_fn`：为 None 说明图没建，多半是 no_grad/detach/全冻结。若 loss 有 grad_fn 而某参数 grad 为 None，用 `torch.autograd.grad(loss, p, allow_unused=True)` 确认是否连通，不连通就沿 forward 找 detach/item/numpy。nan 梯度开 anomaly 模式定位到具体算子，常见是 log(0)、除零或 fp16 上溢。version counter 报错则是 in-place 修改，改为 out-of-place 或克隆。

#### 4. 技术洞察

```
symptom → 假设:
  grad None  → (a) 图未建 (b) 断链 (c) 未参与
  nan grad   → 数值：log(0)/div0/overflow
  graph freed→ 二次 backward
  version    → in-place 破坏
```

````python
torch.autograd.set_detect_anomaly(True)
for n, p in model.named_parameters():
    if p.requires_grad:
        p.register_hook(lambda g, n=n: print(n, g.norm()))
````

#### 5. 图文说明

| 症状 | 假设 | 定位手段 |
|---|---|---|
| grad None | 断链/未用 | allow_unused, grad_fn |
| nan | 数值溢出 | anomaly, hook |
| graph freed | 二次反向 | retain_graph |
| version | in-place | 代码审查 |

#### 6. 拓展知识

`anomaly` 会打印前向栈但显著变慢，适合开发期；数值问题也可用 fp64 重跑对照；梯度 hook 是低侵入的观测点。

#### 7. 常见追问及回答要点

1. None 到底是断链还是没用？——`allow_unused` 与 grad_fn 判断。
2. nan 怎么定位？——anomaly + 逐层 hook。
3. 为什么 in-place 会报错？——版本号保护反向一致性。

#### 8. 易错点与反例

1. 把断链当数值问题查，方向错。
2. 不用 hook 只猜，效率低。
3. in-place 改回 out-of-place 后忘检查性能影响。

#### 9. 实践与验证

分别复现 None/nan/version 三类并验证定位手段有效；修复后加回归用例。无 GPU 依赖。

#### 10. 评分标准

I048 评分：不合格只重跑；合格能看 requires_grad；良好能用 allow_unused/anomaly 分层；优秀/专家能区分断链与数值根因并建立 hook 观测与回归。

### I049. 故障定位：Module/状态

- 难度：中等级
- 知识域：K03 Module/状态
- 考察能力：正确性、性能、成本、运维与迁移条件
- 题型：对比与取舍
- 面试频率：高频
- 前置知识：参数、buffer、模式和 state_dict
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- Module 故障症状：eval 未生效（dropout 仍随机）、state_dict 键不匹配、buffer 停在 CPU、参数共享断开。
- 分层假设：模式层、状态层（键/buffer）、设备层、别名层。
- 止损：显式 `model.eval()`/`.to(device)`、核对 `state_dict().keys()`。
- 根因多为模式/状态在某条路径未被正确设置或保存。

#### 2. 问题分析

建假设树。两个不变量：(1) `state_dict` 键集合两端一致；(2) 模式与设备覆盖所有子模块与 buffer。边界反例：用普通 Python list 存子模块，参数未注册，`state_dict` 不含它们，加载“成功”但权重随机，是状态层静默失败。

#### 3. 具体答案

面试中可以这样回答：

> 推理结果不稳定，我先确认 `model.eval()` 是否递归生效，打印 `model.training` 和各子模块的 training。若是加载后行为不对，就对比 `state_dict` 键集合，看 missing/unexpected。参数在 GPU 而 buffer 在 CPU，会导致推理报 device 错，我检查 buffer 是否真的注册且随 `.to` 迁移。参数共享断开则用 `is` 断言两处是同一对象。止损是显式设模式与设备，根因是某层/某 buffer 没被纳入状态管理。

#### 4. 技术洞察

```
symptom → 假设:
  eval 无效     → training 标志
  加载错配      → state_dict key diff
  device 错     → buffer .device
  共享断开      → id(param)
```

````python
print(model.training, [m.training for m in model.modules()])
a, b = model.state_dict().keys(), ckpt.keys()
print(a ^ b)                       # 差异键
for n, buf in model.named_buffers(): print(n, buf.device)
````

#### 5. 图文说明

| 症状 | 层 | 止损 |
|---|---|---|
| eval 无效 | 模式 | 显式 eval |
| 键不匹配 | 状态 | 核对 keys |
| buffer CPU | 设备 | to(device) |
| 共享断开 | 别名 | is 断言 |

#### 6. 拓展知识

`named_modules`/`named_buffers` 是诊断利器；`register_buffer(persistent=False)` 不存但随设备迁移；DDP 会同步 buffer，若不同步需注意模式与统计一致。

#### 7. 常见追问及回答要点

1. eval 为何要递归？——子模块也要切模式。
2. buffer 为什么可能不在 GPU？——未注册或用了普通属性。
3. 共享如何断？——赋值时拷贝而非引用。

#### 8. 易错点与反例

1. 只对顶层 eval，子模块仍训练态。
2. list 存层，参数未注册。
3. 加载不检查 keys，静默错配。

#### 9. 实践与验证

打印 training/keys/device；构造 list 存层复现未注册；评估模式多次推理断言输出一致。无 GPU 依赖。

#### 10. 评分标准

I049 评分：不合格只重跑；合格能查模式与 keys；良好能分层覆盖 buffer 与共享；优秀/专家能定位静默未注册/未迁移并建立断言防护。

### I050. 故障定位：数据管道

- 难度：中等级
- 知识域：K04 数据管道
- 考察能力：正确性、性能、成本、运维与迁移条件
- 题型：对比与取舍
- 面试频率：高频
- 前置知识：Dataset、Sampler、collate、padding、版本
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- 数据管道故障症状：样本重复/丢失、训练卡住（worker 死锁）、worker OOM、噪声大（数据抖动）。
- 分层假设：Sampler 层、worker 层、collate 层、pin/预取层。
- 止损：降 `num_workers` 复现、关 `persistent_workers`、检查索引覆盖。
- 根因：分片不互补、worker 种子未隔离、collate 结构不一致、pin 内存过大。

#### 2. 问题分析

建假设树。两个不变量：(1) 一个 epoch 索引不重不漏；(2) 每个 worker 独立且可复现。边界反例：卡住不一定是死锁，可能是某 worker 在 `__getitem__` 里做了阻塞 I/O，主进程一直等该 batch，症状像死锁。

#### 3. 具体答案

面试中可以这样回答：

> 样本重复我会先核实索引：收集一个 epoch 的全部索引，看是否有重复或缺失。如果 `IterableDataset` 多 worker 未分片，就会重复，需要按 worker_id 切分。训练卡住我会降 worker 数、关持久 worker，看是否缓解；再检查 `__getitem__` 是否有阻塞调用或跨进程共享状态。worker OOM 多半是预取过多或 pin 内存太大，减 `prefetch_factor`。止损是先把配置退到安全值，再逐步恢复定位根因。

#### 4. 技术洞察

```
symptom → 假设:
  重复/丢失 → Sampler/IterableDataset 分片
  卡住      → worker 阻塞/死锁
  OOM       → prefetch/pin
  噪声      → collate/种子
```

````python
# 索引覆盖
seen = []
for b in DataLoader(ds, batch_size=32, num_workers=4, shuffle=True):
    seen += b.tolist()
assert sorted(seen) == list(range(len(ds)))
````

#### 5. 图文说明

| 症状 | 假设 | 止损 |
|---|---|---|
| 重复 | 分片不全 | 按 worker 切分 |
| 卡住 | worker 阻塞 | 降 worker |
| OOM | 预取/pin | 减 prefetch |
| 抖动 | 种子/collate | 固定种子 |

#### 6. 拓展知识

`worker_init_fn` 隔离随机源；`persistent_workers` 省重启但有状态残留风险；`timeout` 参数可在 worker 卡住时抛错；共享内存（shared memory）不足会导致 worker 崩溃。

#### 7. 常见追问及回答要点

1. 卡住一定是死锁吗？——不一定，可能是阻塞 I/O。
2. 重复怎么定位？——索引集合比对。
3. OOM 从哪减？——先减 prefetch 与 pin。

#### 8. 易错点与反例

1. 直接怀疑模型，实际是数据管道。
2. IterableDataset 多 worker 未切分，样本翻倍。
3. prefetch 过大，pinned 内存耗尽。

#### 9. 实践与验证

索引覆盖单测；降 worker 复现卡住；逐步增 prefetch 找 OOM 阈值。多进程数字 [待验证]。

#### 10. 评分标准

I050 评分：不合格只会重启；合格能查索引；良好能分层处理卡住与 OOM；优秀/专家能区分阻塞与死锁、设计分片断言并给出止损梯度。

### I051. 故障定位：训练/优化器

- 难度：中等级
- 知识域：K05 训练/优化器
- 考察能力：输入矩阵、oracle、故障注入与指标
- 题型：测试/实验与验证
- 面试频率：高频
- 前置知识：forward、backward、累积、optimizer、scheduler
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- 训练/优化器故障症状：loss nan、loss 不降（plateau）、loss 爆炸、参数不更新。
- 分层假设：数据层（异常样本）、数值层（溢出/除零）、超参层（lr/累积缩放）、优化器层（状态/nan）。
- 止损：降 lr、加梯度裁剪、跳过异常 step（skip-step）。
- 根因多为数值或配置问题，需从数据与数值两侧夹逼。

#### 2. 问题分析

建假设树。两个不变量：(1) 每步梯度范数在合理范围；(2) 参数确实被更新。边界反例：loss 不降不一定是优化器问题，可能所有参数被冻结或 lr=0，测试断言“参数变化”即可快速区分。

#### 3. 具体答案

面试中可以这样回答：

> loss nan 我先加梯度裁剪和 anomaly 定位到算子，常见是除零、log(0) 或 fp16 上溢；再检查是否有异常样本（极端值/坏标签）。loss 不降我会断言参数是否真的变化：如果参数没变，是冻结或 lr=0；如果变化但 loss 不降，可能是 lr 太大或太小、累积缩放错误。loss 爆炸先降 lr、加裁剪，检查优化器状态是否有 nan。止损是降 lr + 裁剪 + 跳步，根因从数据和数值两侧定位。

#### 4. 技术洞察

```
symptom → 假设:
  nan      → 除零/log0/溢出/坏样本
  plateau  → lr 0/冻结/太小
  爆炸     → lr 大/更新无 clip/状态 nan
  不更新   → 梯度 None/参数冻结
```

````python
gn = torch.nn.utils.clip_grad_norm_(model.parameters(), 1.0)
if not torch.isfinite(gn):
    optimizer.zero_grad(set_to_none=True); continue   # skip-step
````

#### 5. 图文说明

| 症状 | 假设 | 止损 |
|---|---|---|
| nan | 数值/坏样本 | clip+skip |
| plateau | lr/冻结 | 查参数变化 |
| 爆炸 | lr 大 | 降 lr+clip |
| 不更新 | 梯度 None | 查 requires_grad |

#### 6. 拓展知识

skip-step 机制在检测到 inf 梯度时跳过本次更新，与 AMP scaler 的跳过可叠加；lr warmup 可缓解初期爆炸；梯度范数曲线是核心观测指标。

#### 7. 常见追问及回答要点

1. nan 从哪查？——anomaly + 数据检查。
2. 不降怎么快速区分？——断言参数是否变化。
3. 爆炸如何处理？——降 lr、裁剪、warmup。

#### 8. 易错点与反例

1. 只调 lr 不查坏样本，nan 反复。
2. 参数冻结误判为优化器 bug。
3. 累积 loss 未缩放，有效 lr 偏大。

#### 9. 实践与验证

记录 grad norm 与 lr 曲线；注入坏样本复现 nan；lr=0 复现 plateau；确认修复后曲线正常。无 GPU 依赖部分本地跑。

#### 10. 评分标准

I051 评分：不合格只调 lr；合格能看梯度；良好能用假设树区分 数值/配置/数据 根因；优秀/专家能设计 skip-step 与曲线观测并定位坏样本。

### I052. 故障定位：AMP/恢复

- 难度：中等级
- 知识域：K06 AMP/恢复
- 考察能力：输入矩阵、oracle、故障注入与指标
- 题型：测试/实验与验证
- 面试频率：高频
- 前置知识：autocast、scaler、快照、随机状态
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- AMP/恢复故障症状：fp16 下 loss nan、scaler scale 卡在极小、恢复后 loss 跳跃、scale 频繁减半。
- 分层假设：dtype 层（敏感算子被降精度）、scaler 层（溢出处理）、恢复层（状态未续接）。
- 止损：对敏感算子强制 fp32、切 bf16、提高 scaler 初始 scale 或改 backoff。
- 根因：敏感算子降精度、scale 太大导致上溢、恢复漏状态。

#### 2. 问题分析

建假设树。两个不变量：(1) 敏感算子（softmax/norm/loss）在 fp32 执行；(2) 恢复后 scaler 与 RNG 连续。边界反例：bf16 动态范围大但也有精度问题，某些归约用 bf16 尾数不足会累积偏差，不能简单认为“换 bf16 就好”。

#### 3. 具体答案

面试中可以这样回答：

> fp16 nan 我先排查是否敏感算子被降精度：把 LayerNorm/softmax/loss 强制 fp32，或在 autocast 内手动升精度。如果 scale 一直往下掉，说明上溢频繁，我会降低初始 scale 或减小 lr。恢复后 loss 跳变，多半是漏存 scaler 或 RNG，导致 scale 与随机性重置，我会核对 `scaler.state_dict` 与 RNG 恢复。切 bf16 前先验证归约精度是否可接受，不能假定它一定更好。

#### 4. 技术洞察

```
symptom → 假设:
  fp16 nan   → 敏感算子降精度/上溢
  scale↓     → 上溢频繁
  resume跳变 → scaler/rng 未恢复
  bf16偏差   → 归约尾数不足
```

````python
with torch.autocast("cuda", dtype=torch.float16):
    x = model(x)
    x = layer_norm(x.float()).to(x.dtype)     # 强制 fp32 归约
````

#### 5. 图文说明

| 症状 | 层 | 止损 |
|---|---|---|
| nan | dtype | 敏感算子 fp32 |
| scale↓ | scaler | 降初始 scale |
| 恢复跳变 | 恢复 | 补 scaler/rng |
| bf16 偏差 | 精度 | 归约升精度 |

#### 6. 拓展知识

BN/LayerNorm 的方差在 fp16 下易失真，常强制 fp32；`GradScaler` 的 `init_scale` 与 `backoff_factor` 可调；bf16 与 fp32 指数位相同但尾数少，归约类需谨慎。

#### 7. 常见追问及回答要点

1. fp16 nan 首要怀疑？——敏感算子降精度。
2. scale 为何一直降？——上溢频繁，需调初始值。
3. 恢复跳变根因？——状态未续接。

#### 8. 易错点与反例

1. 把 nan 都归因于 lr，忽略精度。
2. 恢复只存模型，scale 重置。
3. 盲切 bf16，归约偏差累积。

#### 9. 实践与验证

对照 fp32 与 fp16 逐层误差；注入上溢观察 scale 变化；恢复序列对比。GPU 数字 [待验证]。

#### 10. 评分标准

I052 评分：不合格只调 lr；合格知道 scaler 作用；良好能分层定位精度/scale/恢复；优秀/专家能处理敏感算子与 bf16 归约并设计 scale/rng 一致性验证。

### I053. 故障定位：分布式

- 难度：中等级
- 知识域：K07 分布式
- 考察能力：输入矩阵、oracle、故障注入与指标
- 题型：测试/实验与验证
- 面试频率：高频
- 前置知识：rank、DDP、FSDP、collective、容错
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- 分布式故障症状：集合通信挂起/超时、各 rank loss 分叉、内存不均衡、NCCL 报错。
- 分层假设：初始化层（backend/world_size）、通信层（顺序/次数）、数据层（分片）、数值层（梯度不一致）。
- 止损：设短超时快速暴露、降 world_size 复现、检查是否有条件分支跳过 collective。
- 根因多为“集合通信不一致”或“数据/参数不同步”。

#### 2. 问题分析

建假设树。两个不变量：(1) 所有 rank 的集合通信调用序列一致；(2) 各 rank 数据互补、参数同步。边界反例：某 rank 因 OOM 先挂，其他 rank 在 collective 处等待直至 NCCL 超时，日志最后一行指向通信，真实根因却是内存，需结合各 rank 日志交叉判断。

#### 3. 具体答案

面试中可以这样回答：

> 挂起我先看是否所有 rank 都到了同一个 collective：最常见的是条件分支让某些 rank 跳过了通信。我会检查 `if rank == 0` 之类的代码是否包住了 collective，先把它改成所有 rank 一起调用。然后设短超时让挂起尽快暴露，并收集所有 rank 日志交叉定位——往往是某个 rank 先因 OOM 或异常退出。loss 分叉则检查数据是否互补、参数是否同步，用 `all_gather` 抽查。止损是去掉分支、降规模复现，根因回到通信一致性与数据分片。

#### 4. 技术洞察

```
symptom → 假设:
  挂起/超时 → collective 不一致/某 rank 先挂
  分叉      → 数据重叠/参数未同步
  内存不均  → 分片/消息尺寸不均
  NCCL 错   → 拓扑/驱动/网络
```

````python
# 错误：只有 rank0 通信
if rank == 0: dist.all_reduce(x)
# 正确：所有 rank 调用
dist.all_reduce(x)
````

#### 5. 图文说明

| 症状 | 假设 | 止损 |
|---|---|---|
| 挂起 | collective 不一致 | 去分支 |
| 超时 | 某 rank 先挂 | 短超时+日志 |
| 分叉 | 数据/参数 | all_gather |
| 内存不均 | 消息尺寸 | 均衡分片 |

#### 6. 拓展知识

NCCL 超时由 `init_process_group(timeout=...)` 与 `NCCL_TIMEOUT` 控制；`TORCH_NCCL_ASYNC_ERROR_HANDLING` 可让 NCCL 错误立即失败而非挂起；弹性训练需外部编排处理成员变更。

#### 7. 常见追问及回答要点

1. 挂起为什么难查？——真实根因可能在另一 rank 的早期错误。
2. 如何快速暴露？——设短超时。
3. 分叉怎么定位？——all_gather 参数/数据比对。

#### 8. 易错点与反例

1. `if rank==0` 包住 all_reduce，其余 rank 挂起。
2. 只用 rank0 日志，漏掉真实故障源。
3. 无超时，测试/训练长时间卡死。

#### 9. 实践与验证

2 进程故意让一个 rank 跳过 collective，断言短超时触发；交叉收集日志定位先挂 rank；all_gather 参数断言一致。GPU/NCCL [待验证]。

#### 10. 评分标准

I053 评分：不合格只重启；合格知道要所有 rank 参与；良好能设超时并交叉日志定位；优秀/专家能区分挂起根因、处理不对称消息并验证数据/参数同步。

### I054. 故障定位：编译栈

- 难度：中等级
- 知识域：K08 编译栈
- 考察能力：输入矩阵、oracle、故障注入与指标
- 题型：测试/实验与验证
- 面试频率：高频
- 前置知识：Dynamo、AOTAutograd、Inductor、guard、缓存
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- 编译栈故障症状：编译后变慢、结果不一致、训练中频繁重编译、graph break 增多、首次编译超时。
- 分层假设：捕获层（graph break）、守卫层（guard miss 重编译）、数值层（融合/降精度差异）、后端层（不支持算子）。
- 止损：关闭 compile 回退 eager 验证是否由编译引入、开 `explain`。
- 根因多为输入属性不稳或算子不被支持。

#### 2. 问题分析

建假设树。两个不变量：(1) 固定输入下重编译为 0；(2) 编译与 eager 数值一致。边界反例：结果不一致可能来自 Inductor 的自动调优选择了不同算法，或 fp16 融合顺序改变，未必是 bug，但必须用容差与 eager 对齐来判定。

#### 3. 具体答案

面试中可以这样回答：

> 编译后变慢我先用 `torch._dynamo.explain` 看 graph break 和 guard。若固定输入还重编译，说明输入属性在变，比如 batch 或形状浮动，我会固定形状或用动态 shape。结果不一致我就关掉 compile 跑 eager 对照，确认是否编译引入；若只是容差问题，用 `assert_close` 定阈值。首次编译慢是正常的一次性开销，但若每步都慢，多半是重编译。止损是回退 eager 隔离变量。

#### 4. 技术洞察

```
symptom → 假设:
  变慢     → graph break/重编译
  结果不同 → 后端算法/融合差异
  反复编译 → guard miss
  首编超时 → max-autotune
```

````python
exp = torch._dynamo.explain(fn)(x)
print(exp.graph_break_count, exp.compile_times, exp.break_reasons)
````

#### 5. 图文说明

| 症状 | 假设 | 止损 |
|---|---|---|
| 变慢 | break/重编译 | explain |
| 不一致 | 后端差异 | 关 compile 对照 |
| 反复编译 | guard miss | 固定/动态 shape |
| 首编超时 | autotune | 换 mode |

#### 6. 拓展知识

`max-autotune` 首编很慢但稳态快；`reduce-overhead` 用 CUDA graphs 对动态形状不友好；`torch._dynamo.config.suppress_errors=True` 可回退但会掩盖问题。

#### 7. 常见追问及回答要点

1. 如何区分编译引入的慢？——关 compile 对照。
2. graph break 怎么看？——explain 的 break_reasons。
3. 结果不一致一定是 bug？——不一定，先看容差与后端。

#### 8. 易错点与反例

1. 把首编开销当稳态，误判退化。
2. 结果不一致直接归因编译，未对照 eager。
3. 动态形状未标记，反复重编译。

#### 9. 实践与验证

explain 统计 break 与重编译；eager/compile 数值对比；固定输入重编译=0 断言。GPU 数字 [待验证]。

#### 10. 评分标准

I054 评分：不合格只会关 compile；合格能看 explain；良好能区分重编译与数值差异；优秀/专家能定位 guard miss、处理动态 shape 并用容差判定后端差异。

### I055. 故障定位：性能

- 难度：中等级
- 知识域：K09 性能
- 考察能力：输入矩阵、oracle、故障注入与指标
- 题型：测试/实验与验证
- 面试频率：高频
- 前置知识：关键路径、带宽、算力、通信、显存、统计
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- 性能故障症状：吞吐下降、GPU 利用率低、step 时间抖动大、显存峰值上升。
- 分层假设：数据层（取数）、同步层（item/sync）、计算层（kernel 效率）、通信层（allreduce）、容量层（显存/碎片）。
- 止损：先定位瓶颈段，再针对性优化；用基线与 A/B 对照。
- 根因常是重叠缺失、同步引入或算子低效。

#### 2. 问题分析

建假设树。两个不变量：(1) 关键路径段决定吞吐；(2) 稳态均值与尾延迟都要看。边界反例：GPU 利用率高但吞吐低，可能是 kernel 太小、启动开销占比高（launch-bound），此时利用率指标会误导，需看 kernel 时长分布。

#### 3. 具体答案

面试中可以这样回答：

> 我先用 profiler 把 step 拆段，找关键路径。GPU 空转且 worker 忙是数据瓶颈；成片同步空洞是同步瓶颈，常见于每步 `.item()`。若 kernel 多而短，是小 kernel 启动开销，考虑算子融合或编译。通信占比高则优化 bucket 或换并行策略。尾延迟大我会看是否有周期性写盘或 allreduce 抖动。止损是先消除重叠缺失，再谈算法，全程用基线和 A/B 对照验证收益。

#### 4. 技术洞察

```
symptom → 假设:
  GPU 空闲   → 数据/同步
  利用率高但慢→ 小 kernel/launch-bound
  抖动       → 尾延迟来源
  显存↑      → 分配/碎片
```

````python
with torch.profiler.profile(activities=[CUDA], ...) as p:
    step()
print(p.key_averages().table(sort_by="cuda_time_total", row_limit=15))
````

#### 5. 图文说明

| 症状 | 假设 | 止损 |
|---|---|---|
| GPU 空闲 | 数据/同步 | pin/去 item |
| 高利用率慢 | launch-bound | 融合/编译 |
| 抖动 | 尾延迟 | 查周期任务 |
| 显存↑ | 碎片 | 复用/释放 |

#### 6. 拓展知识

launch-bound 与 memory-bound/compute-bound 是三类不同瓶颈；CUDA graphs 可摊薄启动开销；显存碎片用 `memory_summary` 看 reserved 与 allocated 差距。

#### 7. 常见追问及回答要点

1. 利用率高为何还慢？——可能是小 kernel 启动开销。
2. 抖动从哪来？——周期性任务或通信。
3. 如何验证优化有效？——A/B 对照与尾延迟。

#### 8. 易错点与反例

1. 只信利用率，漏 launch-bound。
2. 只看均值，尾延迟未查。
3. 无基线，优化无法证明有效。

#### 9. 实践与验证

profiler 分段；A/B 对照；尾延迟分布；显存峰值对比。GPU 数字 [待验证]。

#### 10. 评分标准

I055 评分：不合格只会加资源；合格能定位段；良好能区分三类瓶颈与尾延迟；优秀/专家能识别 launch-bound、用 A/B 验证并处理显存碎片。

### I056. 故障定位：导出部署

- 难度：中等级
- 知识域：K10 导出部署
- 考察能力：输入矩阵、oracle、故障注入与指标
- 题型：测试/实验与验证
- 面试频率：高频
- 前置知识：state_dict、Export、runtime、schema、迁移
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- 导出部署故障症状：导出报错、运行时结果不一致、加载失败、性能不及预期。
- 分层假设：算子层（不支持）、形状层（动态轴）、数值层（量化/融合）、环境层（版本/ABI）。
- 止损：回退到上一个可用导出物、缩小输入复现、对比 eager。
- 根因多为不支持算子、形状特化或版本不匹配。

#### 2. 问题分析

建假设树。两个不变量：(1) 导出图在声明输入域内与 eager 等价；(2) 产物能在目标 runtime 加载。边界反例：导出成功但运行时结果差，可能是量化校准数据分布不对，未必是图错误，需用代表性数据校准并对比数值。

#### 3. 具体答案

面试中可以这样回答：

> 导出报错我先看是不是不支持算子：数据依赖形状的算子（nonzero、动态索引）常无法导出，需要改写。运行时不一致我用 eager 作 oracle，在多个 shape 上对比；若只在特定输入差，可能是量化或融合造成，需要用代表性校准数据并调节容差。加载失败看版本与 opset/ABI 是否匹配。止损是回退上一个可用产物，根因从算子、形状、数值、环境四个方向收敛。

#### 4. 技术洞察

```
symptom → 假设:
  导出报错 → 不支持算子/数据依赖形状
  结果不一致→ 量化/融合/动态轴
  加载失败 → opset/ABI/版本
  性能差   → 未量化/未融合
```

````python
try:
    torch.export.export(model, (x,))
except torch.export.DynamicDimConstraintError as e:
    print("shape constraint:", e)
````

#### 5. 图文说明

| 症状 | 假设 | 止损 |
|---|---|---|
| 导出失败 | 算子不支持 | 改写算子 |
| 结果不一致 | 量化/融合 | eager 对照 |
| 加载失败 | 版本/ABI | 对齐版本 |
| 性能差 | 未优化 | 量化/TRT |

#### 6. 拓展知识

`torch.export` 报 `DynamicDimConstraintError` 提示需标注动态维；TensorRT 用校准集决定量化 scale；ONNX 算子集版本不同支持范围不同。

#### 7. 常见追问及回答要点

1. 数据依赖形状怎么办？——改写为静态或自定义算子。
2. 量化偏差怎么控？——代表性校准集与容差。
3. 加载失败首要排查？——版本与 ABI。

#### 8. 易错点与反例

1. 只测一个 shape，动态问题漏检。
2. 无校准集量化，偏差大。
3. 版本不匹配，加载报错。

#### 9. 实践与验证

多 shape 对比；量化前后数值；版本矩阵测试加载。运行时数字 [待验证]。

#### 10. 评分标准

I056 评分：不合格只会重导；合格能对比 eager；良好能分层定位算子/形状/数值/环境；优秀/专家能处理量化校准、动态约束并建立版本矩阵验证。

### I057. 故障定位：自定义算子

- 难度：中等级
- 知识域：K11 自定义算子
- 考察能力：输入矩阵、oracle、故障注入与指标
- 题型：测试/实验与验证
- 面试频率：高频
- 前置知识：schema、dispatcher、stream、autograd、ABI
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- 自定义算子故障症状：调用崩溃、结果错误、反向错误、多流下偶发错误、编译回退。
- 分层假设：schema 层（别名/可变）、kernel 层（越界/stride 假设）、stream 层（乱序/竞争）、反向层（公式/save）。
- 止损：与参考实现对比、单流运行排除竞争、gradcheck 查反向。
- 根因多为别名标注错误、stride 假设、stream 未用当前流或反向公式错。

#### 2. 问题分析

建假设树。两个不变量：(1) schema 标注与 kernel 行为一致；(2) kernel 在当前 stream 执行且无越界。边界反例：偶发错误常见于多流竞争或未同步，单流下测不出，必须多流复现并加事件同步验证。

#### 3. 具体答案

面试中可以这样回答：

> 结果错误我先和参考实现对比，缩小到具体输入。若只在非连续输入错，是 kernel 假设了连续 stride，需要按 stride 计算或先 contiguous。崩溃多看向量越界和类型假设。偶发错误我会怀疑 stream：确认用 `getCurrentCUDAStream` 并检查跨流依赖是否有事件同步。反向错用 gradcheck 定位，并检查 `save_for_backward` 是否完整、别名标注是否正确。止损是回退到参考实现，根因逐层收敛。

#### 4. 技术洞察

```
symptom → 假设:
  错(连续OK) → stride 假设
  崩溃       → 越界/类型
  偶发       → stream 竞争
  反向错     → 公式/save/别名
```

````cpp
auto stream = at::cuda::getCurrentCUDAStream();
kernel<<<grid, block, 0, stream>>>(...);   // 必须用当前流
````

#### 5. 图文说明

| 症状 | 假设 | 止损 |
|---|---|---|
| 非连续错 | stride | 按 stride 计算 |
| 崩溃 | 越界 | 边界检查 |
| 偶发 | stream | 事件同步 |
| 反向错 | 公式/别名 | gradcheck |

#### 6. 拓展知识

功能化要求别名标注正确，否则原地写会被漏拷贝；`register_fake` 缺失导致编译回退；ABI 不匹配导致加载失败；cuda-memcheck 可查越界。

#### 7. 常见追问及回答要点

1. 偶发错误为何难查？——多流竞争，单流不复现。
2. 非连续输入为何错？——kernel 假设连续 stride。
3. 反向如何快速校验？——gradcheck。

#### 8. 易错点与反例

1. 只测连续输入，stride bug 漏检。
2. 用默认 stream，偶发数据竞争。
3. 别名标错，功能化漏拷贝。

#### 9. 实践与验证

参考对比；多流复现+事件同步；gradcheck；cuda-memcheck。GPU 结果 [待验证]。

#### 10. 评分标准

I057 评分：不合格只重跑；合格能对比参考；良好能分层定位 stride/stream/反向；优秀/专家能用多流与 gradcheck 复现并处理功能化/别名问题。

### I058. 故障定位：测试治理

- 难度：中等级
- 知识域：K12 测试治理
- 考察能力：输入矩阵、oracle、故障注入与指标
- 题型：测试/实验与验证
- 面试频率：高频
- 前置知识：oracle、证据、日志、回滚、门禁、复盘
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- 测试治理故障症状：CI 间歇失败（flaky）、假绿（漏检 bug）、门禁误拦、回归逃逸上线。
- 分层假设：用例层（oracle 弱/状态耦合）、环境层（随机性/依赖）、门禁层（分级/超时）、流程层（逃逸复盘缺失）。
- 止损：隔离 flaky、临时放宽但登记、补最小复现。
- 根因多为 oracle 缺失、状态未隔离、门禁设计缺陷。

#### 2. 问题分析

建假设树。两个不变量：(1) 结论确定：相同输入相同结果；(2) 门禁能捕获已知缺陷。边界反例：假绿不一定是测试写得少，可能是断言与 oracle 脱节，测试“覆盖”了代码但没判对，需用变异测试量化。

#### 3. 具体答案

面试中可以这样回答：

> flaky 我先重复运行统计通过率，定位随机源（种子、时间、顺序、外部依赖），隔离后修根因而不是加 retry。假绿我用变异测试：注入缺陷看测试是否变红，kill 率低说明 oracle 弱，补数值或性质断言。门禁误拦则检查阈值与环境差异，改成相对基线与分级阻断。回归逃逸后补最小复现并登记为永久回归用例。止损是隔离问题用例，根因从 oracle、隔离、门禁、流程四层收敛。

#### 4. 技术洞察

```
symptom → 假设:
  flaky    → 随机源/状态耦合
  假绿     → oracle 缺失
  误拦     → 阈值/环境
  逃逸     → 门禁缺口/无回归
metric: flaky_rate, mutation_kill_rate, escape_rate
```

````python
@pytest.mark.flaky(reruns=0)     # 先禁用 retry，暴露真实 flaky
def test_x(): ...
# 变异测试: 注入缺陷 → 断言测试变红
````

#### 5. 图文说明

| 症状 | 假设 | 止损 |
|---|---|---|
| flaky | 随机源 | 隔离+修根因 |
| 假绿 | 弱 oracle | 变异测试 |
| 误拦 | 阈值 | 相对基线 |
| 逃逸 | 门禁缺口 | 补回归 |

#### 6. 拓展知识

retry 会掩盖 flaky，应定位而非重试；变异测试衡量测试有效性；逃逸复盘要把 bug 转成回归用例并明确门禁升级路径。

#### 7. 常见追问及回答要点

1. flaky 该 retry 还是修？——先定位根因，retry 只做临时缓冲。
2. 如何发现假绿？——变异测试 kill rate。
3. 逃逸后做什么？——补回归用例并升级门禁。

#### 8. 易错点与反例

1. 加 retry 掩盖 flaky。
2. 以覆盖率为质量指标，弱断言假绿。
3. 逃逸不复盘，同一 bug 反复。

#### 9. 实践与验证

重复运行得 flaky 率；变异测试得 kill 率；把历史逃逸转回归用例并验证能捕获。无 GPU 依赖部分本地跑 [待验证]。

#### 10. 评分标准

I058 评分：不合格用 retry 掩盖；合格能隔离 flaky；良好能用变异测试发现假绿并分级门禁；优秀/专家能建立逃逸到回归的闭环并给出量化指标。

### I059. 性能影响：张量/设备

- 难度：中等级
- 知识域：K01 张量/设备
- 考察能力：输入矩阵、oracle、故障注入与指标
- 题型：测试/实验与验证
- 面试频率：高频
- 前置知识：shape、stride、dtype、device
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- 张量显存由元素数乘元素字节数决定：`numel × itemsize`，fp32 为 4B、fp16/bf16 为 2B、int8 为 1B；视图不新增显存。
- 连续性影响访存效率：非连续张量的逐元素算子可能退化为低效 gather，有效带宽下降。
- 拷贝成本取决于设备与带宽：H2D 走 PCIe/NVLink，D2D 走显存带宽，跨设备 `.to` 是真实拷贝。
- `.item()`/`.cpu()` 触发同步，代价是打断流水线，而不只是拷贝本身的大小。

#### 2. 问题分析

量化张量/设备操作的资源代价。两个不变量：(1) 一个张量的静态显存等于 `numel × itemsize`（连续且独占时）；(2) 拷贝时间 ≈ `字节数 / 有效带宽`。边界反例：视图看起来“多了一个张量”但不增加显存；而 `expand` 虽然 width 变大仍只占一份存储，用元素数估显存会高估。

#### 3. 具体答案

面试中可以这样回答：

> 张量的显存我按 `numel × itemsize` 估，fp32 每元素 4 字节，换 fp16 直接减半。视图、transpose、expand 不新增显存，`expand` 尤其容易被高估。拷贝方面，跨设备 `.to` 是真实带宽受限操作，时间和字节数成正比；同设备同 dtype 是空操作。非连续张量会让下游 kernel 变慢，所以我尽量在便宜的位置一次性 `contiguous`。同步调用如 `.item()` 的代价是打断流水线，比它搬的数据更重要。

#### 4. 技术洞察

```
mem = numel × itemsize
bytes_H2D = numel × itemsize
t_copy ≈ bytes / effective_BW
view/expand: 0 extra bytes (共享 storage)
```

````python
x = torch.empty(1024, 1024, dtype=torch.float32)      # 4 MiB
y = x.t()                                             # 0 额外显存
z = x.half()                                          # 2 MiB
assert x.numel() * x.element_size() == x.real.size() ...
````

#### 5. 图文说明

| 操作 | 显存增量 | 时间代价 |
|---|---|---|
| view/transpose | 0 | ~0 |
| expand | 0 | ~0 |
| .half() | 减半 | 带宽受限 |
| .to(cuda) | 等于大小 | 拷贝带宽 |
| .item() | 0 | 同步停顿 |

#### 6. 拓展知识

`channels_last` 不改变元素数但改变访存模式；`pin_memory` 的花费是主机锁定内存不换页；显存分配器有最小对齐与碎片，实际占用可能高于理论值。

#### 7. 常见追问及回答要点

1. expand 为什么不算显存？——stride 0 共享存储。
2. H2D 时间怎么估？——字节数除以有效带宽。
3. item 的代价是什么？——同步，不是数据量。

#### 8. 易错点与反例

1. 用 `numel` 估 expand 后的显存，高估。
2. 忽略非连续导致 kernel 退化。
3. 只算拷贝字节，忽略同步停顿。

#### 9. 实践与验证

用 `element_size()*numel()` 对比 `max_memory_allocated` 增量验证视图不占新显存；对 `.to` 计时拟合带宽；对非连续与连续输入对比算子耗时。GPU 数字 [待验证]。

#### 10. 评分标准

I059 评分：不合格不知字节估算；合格能按 numel×itemsize 估显存；良好能区分视图/拷贝/同步代价；优秀/专家能拟合拷贝带宽、分析连续性与单位成本并给出优化取舍。

### I060. 性能影响：autograd

- 难度：中等级
- 知识域：K02 autograd
- 考察能力：输入矩阵、oracle、故障注入与指标
- 题型：测试/实验与验证
- 面试频率：高频
- 前置知识：requires_grad、leaf、view、detach、反向
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- autograd 的显存代价来自保存的中间激活：反向要用到 forward 的中间结果，默认全部保留至反向结束。
- 反向计算量约为前向的两倍（每个算子要算对输入的梯度，部分还要算对权重），是主要时间开销。
- `no_grad`/`inference_mode` 不建图，省下激活显存与建图开销；`retain_graph` 会延长图生命周期、增加显存驻留。
- 激活检查点用重算换显存：forward 不存中间激活，backward 重算，显存降但算力增。

#### 2. 问题分析

量化自动微分的资源代价。两个不变量：(1) 反向时间 ≈ 2× 前向（同规模算子）；(2) 驻留显存 ≈ 激活 + 参数 + 梯度 + 优化器状态。边界反例：小模型但深序列时激活是显存主导；若只用 `no_grad` 推理，激活不驻留，但训练必须有图，不能简单用推理显存估训练。

#### 3. 具体答案

面试中可以这样回答：

> autograd 的时间和显存都要算。时间上反向约是前向的两倍，因为每个算子要算多个输入的梯度。显存上主要是保存的激活，层数越多、序列越长越大；参数梯度优化器状态是固定部分。推理用 `no_grad` 或 `inference_mode` 可以省掉激活和建图。显存不够就用激活检查点，用重算换空间，代价是时间增加。`retain_graph` 会让图驻留更久，非必要不保留。

#### 4. 技术洞察

```
t_bwd ≈ 2 × t_fwd
mem_train ≈ act(B,L,H,layers) + P + P + opt_state
checkpoint: mem ∝ sqrt(layers)（分块时），time ↑
```

````python
with torch.no_grad():          # 推理，无激活驻留
    y = model(x)
y = torch.utils.checkpoint.checkpoint(block, x)   # 训练省显存
````

#### 5. 图文说明

| 项 | 量级 | 措施 |
|---|---|---|
| 激活 | ∝ B·L·H·layers | checkpoint |
| 反向时间 | ~2× 前向 | 融合/编译 |
| 推理 | 无激活 | no_grad |
| 图驻留 | 与图层数 | 避免 retain_graph |

#### 6. 拓展知识

checkpoint 可只对部分层启用；recompute 的粒度影响时间；`torch.func` 的函数式反向不写 `.grad` 但同样占激活显存；混合精度可减半激活字节。

#### 7. 常见追问及回答要点

1. 反向为何约两倍？——要算多个输入的梯度。
2. 推理能省什么？——激活与建图。
3. checkpoint 换到什么？——用时间换显存。

#### 8. 易错点与反例

1. 用推理显存估训练，严重低估。
2. 到处 `retain_graph`，图长期驻留。
3. 对全部层开 checkpoint，时间暴涨收益不足。

#### 9. 实践与验证

对比 `no_grad` 与训练的峰值显存；测前向与反向时间比接近 2；开 checkpoint 前后测显存与时间。GPU 数字 [待验证]。

#### 10. 评分标准

I060 评分：不合格只知 backward；合格能说反向约两倍；良好能分析激活主导与 checkpoint 取舍；优秀/专家能量化各分量、给出重算粒度与单位成本并验证。

### I061. 性能影响：Module/状态

- 难度：中等级
- 知识域：K03 Module/状态
- 考察能力：现象分层、假设树、止损、根因与复发防护
- 题型：故障排查与事故复盘
- 面试频率：高频
- 前置知识：参数、buffer、模式和 state_dict
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- 模型状态显存 ≈ 参数 + 梯度 + 优化器状态；Adam 为每参数维护一阶矩和二阶矩，状态是参数的两倍。
- 混合精度训练中参数与激活常以 fp16 存，而优化器状态常以 fp32 存以保数值稳定，导致状态占比更高。
- `state_dict` 序列化大小 ≈ 参数张量字节数之和，含 buffer；eval 模式不保存激活，显存显著低于训练。
- 参数共享会减少实际存储，但 `state_dict` 中可能出现多个键指向同一张量。

#### 2. 问题分析

量化 Module 状态的资源。两个不变量：(1) 训练显存中静态部分 ≈ `P×(1 参数 + 1 梯度 + 2 Adam状态)`（fp32 时）；(2) `state_dict` 字节数与参数+持久 buffer 的元素数成正比。边界反例：参数共享时用键的个数估显存会高估，因为多个键共享同一 storage。

#### 3. 具体答案

面试中可以这样回答：

> 训练静态显存我按参数量乘系数估：参数一份、梯度一份，Adam 再加两倍的状态，所以 fp32 下约四倍参数量；如果用 fp16 存参数激活、fp32 存状态，状态占比更高。推理时 eval 不存激活，只剩参数和 buffer，显存低很多。`state_dict` 大小等于参数加持久 buffer 的字节数，我可以用它估磁盘和加载开销。参数共享时要按唯一 storage 计，别按键数。

#### 4. 技术洞察

```
mem_train ≈ P·(1 + 1 + 2)·bs  (Adam, fp32)
mem_eval  ≈ (P + buffers)·bs
state_dict_bytes ≈ Σ numel(·)×itemsize
```

````python
mem = sum(p.numel() * p.element_size() for p in model.parameters())
sd_bytes = sum(v.numel() * v.element_size() for v in model.state_dict().values())
````

#### 5. 图文说明

| 状态 | 系数 | 备注 |
|---|---|---|
| 参数 | 1× | fp16 可减半 |
| 梯度 | 1× | 与参数同 |
| Adam | 2× | fp32 状态 |
| buffer | 视大小 | eval 也在 |

#### 6. 拓展知识

ZeRO/FSDP 把优化器状态与参数分片，单卡显存随 world_size 下降；8-bit 优化器状态可再减；`state_dict` 加载峰值可能因同时持有新旧两份而暂时翻倍。

#### 7. 常见追问及回答要点

1. Adam 为什么显存多？——两个动量状态。
2. eval 省什么？——激活。
3. 共享参数怎么算？——按唯一 storage。

#### 8. 易错点与反例

1. 只算参数，漏 Adam 状态与梯度。
2. 用键数估共享参数显存，高估。
3. 加载时新旧 state_dict 同时驻留 OOM。

#### 9. 实践与验证

用参数量×系数估训练显存并与 `max_memory_allocated` 对比；测 `state_dict` 序列化字节；eval 与 train 峰值对比。GPU 数字 [待验证]。

#### 10. 评分标准

I061 评分：不合格只算参数；合格能量化参数+梯度+状态；良好能区分 train/eval 与 state_dict 大小；优秀/专家能处理共享参数与分片、给出显存预算并验证。

### I062. 性能影响：数据管道

- 难度：中等级
- 知识域：K04 数据管道
- 考察能力：现象分层、假设树、止损、根因与复发防护
- 题型：故障排查与事故复盘
- 面试频率：高频
- 前置知识：Dataset、Sampler、collate、padding、版本
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- 数据管道吞吐 ≈ `num_workers / 单样本预处理时间`，但要扣除主进程消费与队列同步开销。
- 预取显存/内存 ≈ `num_workers × prefetch_factor × batch 字节`，pinned 内存计入主机内存而非显存。
- 延迟与吞吐是两件事：预取能隐藏取数延迟，但若预处理本身慢于消费，吞吐被 CPU 限制。
- 变长序列的 padding 放大有效计算量，属数据层引入的隐性算力成本。

#### 2. 问题分析

量化数据管道的资源与吞吐。两个不变量：(1) 稳态下 worker 供给速率 ≥ 消费速率时 GPU 不空转；(2) 预取队列占用内存与 worker 数、prefetch 因子成正比。边界反例：`num_workers` 太大不一定更快，进程调度与内存拷贝开销会抵消，甚至因 pinned 内存耗尽而 OOM。

#### 3. 具体答案

面试中可以这样回答：

> 数据管道我给两个指标：供给吞吐和内存占用。吞吐上，如果 `__getitem__` 和增强慢，就加 `num_workers`，但存在拐点，超过后进程调度和拷贝开销反噬。内存上，预取队列约等于 `worker 数 × prefetch_factor × batch 字节`，pinned 内存太多会挤爆主机。延迟用预取隐藏，所以 GPU 利用率回升说明重叠生效。变长序列我会分桶，避免 padding 放大无效计算。单位成本上，我按“每样本 CPU 时间”和“每样本内存”衡量。

#### 4. 技术洞察

```
supply = workers / t_preprocess
queue_mem ≈ workers × prefetch_factor × batch_bytes
GPU idle ⇐ supply < demand
padding_efficiency = Σ len / (B × max_len)
```

````python
loader = DataLoader(ds, num_workers=8, prefetch_factor=2,
                    pin_memory=True, persistent_workers=True)
# 扫描 num_workers 找吞吐拐点
````

#### 5. 图文说明

| 指标 | 公式 | 手段 |
|---|---|---|
| 供给吞吐 | workers/t_prep | 增 worker/加速预处理 |
| 预取内存 | w×pf×batch | 减 pf/worker |
| 重叠 | GPU 空转率 | pin+非阻塞 |
| padding 效率 | Σlen/(B·max) | 分桶 |

#### 6. 拓展知识

`persistent_workers` 省进程重建但占常驻内存；共享内存不足会让 worker 崩溃；把 CPU 密集增强下沉到 GPU（如 DALI）是另一条路径。

#### 7. 常见追问及回答要点

1. worker 越多越快吗？——存在拐点，之后反噬。
2. 预取占多少内存？——worker×prefetch×batch。
3. padding 为何是成本？——无效 token 参与计算。

#### 8. 易错点与反例

1. 无脑堆 worker，吞吐不升反降。
2. prefetch 过大致内存 OOM。
3. 变长不分桶，padding 效率低。

#### 9. 实践与验证

扫描 worker 与 prefetch 记录吞吐和内存；测 GPU 空转率验证重叠；统计 padding 效率。多进程数字 [待验证]。

#### 10. 评分标准

I062 评分：不合格只设 worker；合格能量化供给吞吐；良好能算预取内存与重叠效果；优秀/专家能找拐点、算单位成本并处理 padding 效率。

### I063. 性能影响：训练/优化器

- 难度：中等级
- 知识域：K05 训练/优化器
- 考察能力：现象分层、假设树、止损、根因与复发防护
- 题型：故障排查与事故复盘
- 面试频率：高频
- 前置知识：forward、backward、累积、optimizer、scheduler
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- 优化器 step 的时间成本约为参数量的常数倍：SGD 是逐元素乘加，Adam 还要维护两个动量与偏差修正。
- Adam 的每参数状态是两倍，更新读写量约为参数的 4~6 倍，可能成为带宽受限操作。
- scheduler.step 只改几十个标量，时间可忽略；梯度裁剪需一次全局范数归约，成本与参数量成正比。
- 优化器状态若以 fp32 存，读写带宽压力更大，是 step 时间的一部分。

#### 2. 问题分析

量化训练/优化器的性能影响。两个不变量：(1) step 时间随参数量线性增长；(2) 优化器状态读写是带宽受限而非算力受限。边界反例：小模型 + 大 batch 时，step 时间占比可能不可忽略（相对前反向），而大模型时前反向主导，不能一概说“优化器可忽略”。

#### 3. 具体答案

面试中可以这样回答：

> step 的成本我按“对每个参数做几次读写”估。SGD 大约读参数读梯度写参数；Adam 还要读两个状态并写回，加上偏差修正，读写量大致是参数的几倍，通常是显存带宽受限。scheduler 只改标量可忽略，梯度裁剪多一次全局归约。我会看 step 时间占总 step 的比例：大模型前反向主导，优化器占比小；小模型大 batch 时占比可能上升。单位成本上用“每参数更新字节数”衡量，减少它的办法是降状态精度或分片。

#### 4. 技术洞察

```
SGD:  read p, read g, write p
Adam: read p,g,m,v; write p,m,v; bias-correct
step_bytes ≈ params × k × itemsize   (k≈6 for Adam)
t_step ≈ step_bytes / BW  (bandwidth-bound)
```

#### 5. 图文说明

| 优化器 | 状态 | 相对读写 |
|---|---|---|
| SGD | 0 | ~3× |
| SGD+mom | 1 | ~4× |
| Adam | 2 | ~6× |

```python
# 若 step 占比异常高，考虑降状态精度或分片
```

#### 6. 拓展知识

`foreach`/`fused` 优化器用向量化减少 kernel 启动；8-bit 优化器降低状态带宽；ZeRO 分片摊薄单卡 step 读写；loss scaling 与 unscale 增加少量开销。

#### 7. 常见追问及回答要点

1. Adam 为什么比 SGD 贵？——多两个状态读写。
2. step 是带宽还是算力受限？——通常带宽受限。
3. 何时 step 占比大？——小模型大 batch。

#### 8. 易错点与反例

1. 假定优化器永远可忽略，小模型下误判。
2. 用 fp32 状态不评估带宽压力。
3. 逐参数循环更新，kernel 启动开销大。

#### 9. 实践与验证

测单步 step 时间与参数量关系验证线性；对比 SGD/Adam 的 step 占比；用 profiler 看优化器 kernel 时间。GPU 数字 [待验证]。

#### 10. 评分标准

I063 评分：不合格认为优化器免费；合格能量化 step 成本随参数增长；良好能分析带宽受限与状态读写；优秀/专家能给出单位成本优化（精度/分片/向量化）并验证。

### I064. 性能影响：AMP/恢复

- 难度：中等级
- 知识域：K06 AMP/恢复
- 考察能力：现象分层、假设树、止损、根因与复发防护
- 题型：故障排查与事故复盘
- 面试频率：高频
- 前置知识：autocast、scaler、快照、随机状态
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- fp16/bf16 在支持张量核心的硬件上可显著提高矩阵乘吞吐，并让激活显存减半。
- AMP 的额外开销来自 GradScaler 的缩放、unscale 与溢出检测，以及 dtype 转换，通常相对收益较小。
- checkpoint 的大小由模型状态决定；写入或读取的时间受磁盘带宽限制，大模型下不可忽略。
- 溢出跳过会浪费一个 step 的计算，频繁溢出会拖慢有效训练速度。

#### 2. 问题分析

量化 AMP 与恢复的性能影响。两个不变量：(1) 稳态下 AMP 的 step 时间应低于纯 fp32（张量核心收益大于转换开销）；(2) checkpoint 写/读时间 ≈ `字节数 / 磁盘带宽`。边界反例：小算子为主的模型算术强度低，AMP 的 dtype 转换开销可能抵消张量核心收益，此时反而变慢。

#### 3. 具体答案

面试中可以这样回答：

> AMP 的收益主要在两处：矩阵乘走张量核心提速，激活显存减半让 batch 能开更大。代价是 scaler 的缩放、unscale 和 dtype 转换，通常很小。但小算子为主的模型可能因转换开销抵消收益，我会 A/B 对比。溢出频繁会浪费 step，要调 scale 初始值。checkpoint 的时间是字节数除以磁盘带宽，大模型写盘可能阻塞训练，我会异步或降频保存。恢复同理，读盘时间要算进重启成本。

#### 4. 技术洞察

```
speedup ≈ FLOPs_tensorcore / FLOPs_fp32  (当 tensor-core-bound)
mem_act: fp16 = fp32 / 2
ckpt_time ≈ ckpt_bytes / disk_BW
overflow_step_waste = skipped_steps / total_steps
```

#### 5. 图文说明

| 项 | 影响 | 备注 |
|---|---|---|
| 张量核心 | 提速 | 算力受限时明显 |
| 激活 | 减半 | 可开大 batch |
| scaler | 小开销 | 缩放/检测 |
| checkpoint | 磁盘受限 | 可降频/异步 |

#### 6. 拓展知识

bf16 无需 scaler 但尾数少；`async checkpoint` 把写盘放后台线程；Tensor Core 效率依赖对齐与形状；小 batch 时张量核心利用率低。

#### 7. 常见追问及回答要点

1. AMP 何时不提速？——小算子/低算术强度。
2. 溢出代价？——浪费一个 step。
3. checkpoint 时间怎么算？——字节/磁盘带宽。

#### 8. 易错点与反例

1. 假定 AMP 必快，小模型实测更慢。
2. 频繁溢出，有效吞吐下降。
3. 同步写大 checkpoint，训练周期抖动。

#### 9. 实践与验证

A/B 对比 fp32/fp16 step 时间与显存；统计溢出率；测 checkpoint 写读时间。GPU/磁盘数字 [待验证]。

#### 10. 评分标准

I064 评分：不合格只知 AMP 快；合格能说张量核心与显存收益；良好能分析转换/溢出/checkpoint 代价；优秀/专家能给出 A/B 方法与异步保存、量化单位成本。

### I065. 性能影响：分布式

- 难度：中等级
- 知识域：K07 分布式
- 考察能力：现象分层、假设树、止损、根因与复发防护
- 题型：故障排查与事故复盘
- 面试频率：高频
- 前置知识：rank、DDP、FSDP、collective、容错
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- 分布式通信成本 ≈ 消息字节数 × AllReduce 次数 / 互联带宽，读作 `2(W−1)/W × bytes / BW`（ring AllReduce）。
- 扩展效率 = `W × t_single / t_W`；通信占比高时扩展效率随 world_size 下降。
- FSDP 用分片换显存，代价是每层前向 all-gather、反向 reduce-scatter，通信量更大。
- 计算与通信重叠（bucket、backward 中通信）决定有效利用率，重叠不好则通信暴露在关键路径。

#### 2. 问题分析

量化分布式的性能影响。两个不变量：(1) 单步通信时间与参数量和 world_size 成正比；(2) 显存随分片下降。边界反例：world_size 增大后计算摊销更好但通信上升，存在最优规模；盲目加卡可能因通信主导而扩展效率低于线性。

#### 3. 具体答案

面试中可以这样回答：

> 我把分布式成本拆成计算和通信。通信上，AllReduce 的成本与参数量、world_size 成正比，除以互联带宽；ring 算法的系数约为 `2(W−1)/W`。扩展效率我用 `W×t_single/t_W` 衡量，随规模增大若效率下降说明通信占比升高，有最优规模。FSDP 省显存但通信更多，适合显存瓶颈的场景。关键是重叠：bucket 和反向中通信能把通信藏进计算，重叠不好就暴露在关键路径。单位成本上看“每卡每秒有效吞吐”。

#### 4. 技术洞察

```
t_ar ≈ 2(W-1)/W × bytes/BW + latency
eff(W) = W × t1 / tW
FSDP: 每层 all-gather(fwd) + reduce-scatter(bwd)
```

#### 5. 图文说明

| 项 | 随 W | 手段 |
|---|---|---|
| AllReduce | 增大 | bucket/重叠 |
| 显存 | 下降(FSDP) | 分片 |
| 扩展效率 | 下降 | 找最优 W |
| 重叠 | 提升 | 反向通信 |

#### 6. 拓展知识

AllReduce 有 ring/tree 算法，小消息用 tree、大消息用 ring；`gradient_as_bucket_view` 省拷贝；NVLink 与 PCIe 带宽差异巨大，拓扑影响通信。

#### 7. 常见追问及回答要点

1. 扩展效率怎么算？——W×t1/tW。
2. 何时通信主导？——消息大、带宽低、重叠差。
3. FSDP 的代价？——通信更多。

#### 8. 易错点与反例

1. 盲目加卡，通信主导反而更慢。
2. 忽略重叠，通信暴露。
3. 用平均而非有效吞吐衡量。

#### 9. 实践与验证

扫 world_size 测扩展效率曲线；对比有无 bucket/重叠；测 FSDP 与 DDP 显存与时间。GPU/NCCL 数字 [待验证]。

#### 10. 评分标准

I065 评分：不合格只知加卡；合格能算通信成本；良好能分析扩展效率与重叠；优秀/专家能推导 AllReduce 系数、给 FSDP/DDP 取舍并找最优规模。

### I066. 性能影响：编译栈

- 难度：中等级
- 知识域：K08 编译栈
- 考察能力：现象分层、假设树、止损、根因与复发防护
- 题型：故障排查与事故复盘
- 面试频率：高频
- 前置知识：Dynamo、AOTAutograd、Inductor、guard、缓存
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- 编译的收益来自算子融合、减少 kernel 启动与更好的内存复用；代价是首次编译时间与重编译风险。
- 首次编译时间受图的复杂度与 `mode` 影响，`max-autotune` 会枚举多种 kernel 配置，首编可能到分钟级。
- 稳态收益要看 step 时间下降与 kernel 数减少；重编译会周期性引入长尾。
- CUDA graphs（`reduce-overhead`）把多 kernel 合并为一次图执行，大幅降低启动开销但要求静态。

#### 2. 问题分析

量化编译栈的性能影响。两个不变量：(1) 稳态 step 时间应低于或接近 eager（否则编译收益为负）；(2) 编译产物只在首次或重编译时产生长尾。边界反例：训练脚本每步输入形状变化，重编译开销贯穿全程，平均下来比 eager 还慢，此时编译反而有害。

#### 3. 具体答案

面试中可以这样回答：

> 编译的账要分首次和稳态。首次编译有一次性开销，`max-autotune` 尤其大；稳态收益来自算子融合和减少 kernel 启动，我会比 step 的 P50：编译后应下降。CUDA graphs 能把小 kernel 的启动开销合并，适合 launch-bound 场景。风险是重编译：输入形状或 dtype 一变就重编，形成长尾，平均性能可能倒退，所以我固定形状或用动态 shape。单位成本上，我用“稳态 step 时间 + 首编摊销”衡量。

#### 4. 技术洞察

```
amortized = first_compile_time/N + steady_step_time
gain = (eager_step - compiled_step)/eager_step
kernel_count ↓ ⇒ launch overhead ↓
CUDA graph: N launches → 1 graph launch
```

````python
torch._dynamo.reset()
# 首编计时
# 稳态计时 100 步取 P50/P95
````

#### 5. 图文说明

| 项 | 影响 | 手段 |
|---|---|---|
| 融合 | 减 kernel | Inductor |
| 启动开销 | 降 | CUDA graphs |
| 首编 | 一次性 | 摊销 |
| 重编译 | 长尾 | 固定/动态 shape |

#### 6. 拓展知识

`mode=reduce-overhead` 用 CUDA graphs 对动态形状不友好；Inductor 的 autotune 缓存可持久化；graph break 会切碎融合机会，降低收益。

#### 7. 常见追问及回答要点

1. 首编开销如何摊销？——除以运行步数。
2. 何时编译有害？——频繁重编译或图太碎。
3. CUDA graphs 适合什么？——launch-bound 静态形状。

#### 8. 易错点与反例

1. 只看稳态忽略首编，小步数训练收益为负。
2. 输入形状变化，重编译贯穿全程。
3. 图太碎，融合收益有限。

#### 9. 实践与验证

分别测首编与稳态 step；统计 kernel 数与重编译次数；对比 eager/compile 的 P50/P95。GPU 数字 [待验证]。

#### 10. 评分标准

I066 评分：不合格只知 compile 快；合格能分首编与稳态；良好能分析融合/启动/重编译；优秀/专家能算摊销成本、给 CUDA graphs 取舍并量化收益。

### I067. 性能影响：性能

- 难度：中等级
- 知识域：K09 性能
- 考察能力：现象分层、假设树、止损、根因与复发防护
- 题型：故障排查与事故复盘
- 面试频率：高频
- 前置知识：关键路径、带宽、算力、通信、显存、统计
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- roofline 模型：算子时间下界为 `max(FLOPs/峰值算力, Bytes/峰值带宽)`，算术强度决定受限于哪一侧。
- 显存带宽是大多数 elementwise/norm 算子的上限；GEMM 属算力受限。
- 有效带宽利用率 = 实际字节吞吐 / 峰值带宽；低利用率说明访存效率差。
- 尾延迟来自周期性任务（checkpoint、评估）或通信抖动，均值会掩盖。

#### 2. 问题分析

量化端到端性能上限。两个不变量：(1) 每个算子的时间不低于 roofline 下界；(2) 全 step 吞吐不超过关键资源上限。边界反例：GPU 利用率高但吞吐低，说明处于 launch-bound，roofline 假设算子足够大，不适用，需要合并 kernel。

#### 3. 具体答案

面试中可以这样回答：

> 我用 roofline 估每个算子的下界：算算术强度，判断是算力还是带宽受限。elementwise 和 norm 通常带宽受限，我会算有效带宽利用率，偏低就说明访存低效。GEMM 看算力利用率。整个 step 的吞吐受关键资源限制，我用 profiler 找瓶颈段。如果利用率高但吞吐低，可能是 kernel 太小、启动开销主导，这时合并 kernel 或编译更有效。尾延迟我会单独看 P95，找周期性 checkpoint 或通信抖动。

#### 4. 技术洞察

```
t_min(op) = max(FLOPs/peak_FLOPS, Bytes/peak_BW)
intensity = FLOPs/Bytes
BW_util = actual_Bytes_per_s / peak_BW
```

#### 5. 图文说明

| 算子类 | 受限 | 优化 |
|---|---|---|
| elementwise/norm | 带宽 | 融合 |
| GEMM | 算力 | 张量核心 |
| 小 kernel | 启动 | CUDA graphs |
| 周期任务 | 尾延迟 | 异步/降频 |

#### 6. 拓展知识

FlashAttention 通过分块提高算术强度、降低 HBM 访问；融合把多个带宽受限算子并成一个，减少读写；Nsight Compute 可测 SOL（speed of light）利用率。

#### 7. 常见追问及回答要点

1. 如何判断受限于哪侧？——算术强度与 roofline 拐点比较。
2. 有效带宽低说明什么？——访存模式差或 kernel 太小。
3. 尾延迟来源？——周期任务/通信抖动。

#### 8. 易错点与反例

1. 对所有算子套 roofline，忽略小 kernel。
2. 只看均值，尾延迟未查。
3. 用峰值算力估实际，忽略利用率。

#### 9. 实践与验证

算算术强度分类算子；测有效带宽利用率；A/B 融合前后；单独统计 P95。GPU 数字 [待验证]。

#### 10. 评分标准

I067 评分：不合格只会看时间；合格能用 roofline；良好能区分带宽/算力/启动受限并给手段；优秀/专家能算有效利用率、处理尾延迟并量化融合收益。

### I068. 性能影响：导出部署

- 难度：中等级
- 知识域：K10 导出部署
- 考察能力：现象分层、假设树、止损、根因与复发防护
- 题型：故障排查与事故复盘
- 面试频率：高频
- 前置知识：state_dict、Export、runtime、schema、迁移
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- 推理性能指标：单请求延迟（latency）与整体吞吐（throughput），二者常需权衡（batch 增大提吞吐升延迟）。
- 量化（int8/fp16）可降显存与带宽、提吞吐，但可能损失精度；需要校准。
- 导出后经 runtime（ORT/TensorRT）可做算子融合与 kernel 自动调优，通常优于原生 PyTorch 推理。
- 动态 shape 支持影响 runtime 的优化空间，固定 shape 常能获得更高性能。

#### 2. 问题分析

量化导出部署的性能影响。两个不变量：(1) 单请求延迟随 batch 增大而上升；(2) 量化降低每字节成本、提高吞吐。边界反例：为追求吞吐把 batch 开到很大，延迟超 SLA；线上常需在延迟约束下最大化吞吐，是受约束优化而非单目标。

#### 3. 具体答案

面试中可以这样回答：

> 推理我看延迟和吞吐两个指标，并按 SLA 权衡。延迟随 batch 增长，吞吐也随 batch 增长，所以我在延迟约束下最大化吞吐。量化能降带宽和显存、提吞吐，但要校准并验证精度损失在可接受范围。导出到 ORT/TRT 后还能做融合和 kernel 调优，通常比原生快。动态 shape 会限制优化，能固定就固定。单位成本我用“每千 token 的延迟与显存”衡量。

#### 4. 技术洞察

```
throughput = batch / latency(batch)
constraint: latency(batch) ≤ SLA ⇒ maximize throughput
quant: bytes/elem ↓, BW pressure ↓, accuracy risk ↑
```

#### 5. 图文说明

| 变量 | 延迟 | 吞吐 |
|---|---|---|
| batch↑ | 上升 | 上升 |
| 量化 | 降 | 升 |
| 融合 | 降 | 升 |
| 动态 shape | 升 | 降 |

#### 6. 拓展知识

连续批处理（continuous batching）在 LLM 服务中兼顾吞吐与延迟；KV cache 占用显存随序列增长；TensorRT 的 profile 需要为动态 shape 设多个优化档。

#### 7. 常见追问及回答要点

1. 延迟与吞吐如何权衡？——延迟约束下最大化吞吐。
2. 量化风险？——精度损失需校准验证。
3. 动态 shape 代价？——限制运行时优化。

#### 8. 易错点与反例

1. 只追吞吐，延迟超 SLA。
2. 量化不校准，精度崩。
3. 动态 shape 未设 profile，性能差。

#### 9. 实践与验证

扫描 batch 得延迟-吞吐曲线；量化前后精度与吞吐对比；固定 vs 动态 shape 性能对比。GPU/runtime 数字 [待验证]。

#### 10. 评分标准

I068 评分：不合格只报吞吐；合格能区分延迟与吞吐；良好能处理量化与融合收益；优秀/专家能在 SLA 约束下优化、给 KV cache/连续批处理取舍并验证精度。

### I069. 性能影响：自定义算子

- 难度：中等级
- 知识域：K11 自定义算子
- 考察能力：现象分层、假设树、止损、根因与复发防护
- 题型：故障排查与事故复盘
- 面试频率：高频
- 前置知识：schema、dispatcher、stream、autograd、ABI
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- 自定义算子的成本包括 kernel 执行时间与启动开销；融合多个算子可减少 HBM 往返与启动次数。
- 小算子受限于启动开销（launch-bound），大算子受限于带宽或算力。
- 融合的收益来自减少中间张量的读写：若不融合，每个中间结果都要写回并再读。
- 自定义 kernel 的性能需与 PyTorch 原生/编译后端对比，避免“手写未必更快”。

#### 2. 问题分析

量化自定义算子的性能影响。两个不变量：(1) 融合后中间张量读写次数减少；(2) 小算子时间不低于启动开销。边界反例：手写一个简单 elementwise 算子未必快于 PyTorch，因为原生已高度优化且可能被融合；自定义的价值在原生没有的融合或算法。

#### 3. 具体答案

面试中可以这样回答：

> 自定义算子的账有两个变量：执行时间和启动开销。融合多个算子能省去中间张量的写回与再读，减少 HBM 往返，收益明显。小算子则受启动开销限制，融合或 CUDA graphs 能合摊。我总是和 PyTorch 原生或 Inductor 的结果对比，因为原生往往已经优化，自定义只在原生没有的融合或特殊算法时有优势。单位成本上用“每元素带宽”和有效利用率衡量。

#### 4. 技术洞察

```
fused: read once, write once  → bytes ≈ (in+out)
unfused N ops: bytes ≈ Σ reads+writes
launch-bound if t_exec < t_launch
```

````cpp
// 融合 add+relu：一次读入，一次写出
// 对比两次 kernel 的中间写回
````

#### 5. 图文说明

| 场景 | 瓶颈 | 手段 |
|---|---|---|
| 多 elementwise | 带宽 | 融合 |
| 小 kernel | 启动 | CUDA graphs |
| 特殊算法 | 算法 | 自定义 |
| 已有原生 | — | 先对比 |

#### 6. 拓展知识

Inductor 能自动融合多数 elementwise；自定义适合 flash-attention 类算法级优化；融合要注意寄存器/共享内存占用，过度融合会降低 occupancy。

#### 7. 常见追问及回答要点

1. 融合为何快？——减少中间读写。
2. 自定义一定更快吗？——不一定，要与原生对比。
3. 何时用自定义？——原生无对应融合或算法。

#### 8. 易错点与反例

1. 手写简单算子，未对比原生。
2. 过度融合，occupancy 下降。
3. 忽略启动开销，小算子优化方向错。

#### 9. 实践与验证

对比自定义与原生/Inductor 时间；统计读写字节验证融合收益；测不同融合粒度的 occupancy。GPU 数字 [待验证]。

#### 10. 评分标准

I069 评分：不合格以为手写必快；合格能与原生对比；良好能分析融合读写收益；优秀/专家能平衡融合与 occupancy、量化有效利用率并给自定义时机判断。

### I070. 性能影响：测试治理

- 难度：中等级
- 知识域：K12 测试治理
- 考察能力：现象分层、假设树、止损、根因与复发防护
- 题型：故障排查与事故复盘
- 面试频率：高频
- 前置知识：oracle、证据、日志、回滚、门禁、复盘
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- 测试套件的成本 = 用例数 × 单例时间 × 重复次数；CI 上还有排队与资源占用成本。
- flaky 的成本是重跑与人工排查的时间，隐含在“假失败”里。
- 覆盖率工具衡量执行，不衡量断言有效性；无效用例仍消耗时间。
- 分层门禁（冒烟/关键/全量）用不同深度平衡反馈速度与保护强度。

#### 2. 问题分析

量化测试治理的性能影响。两个不变量：(1) 套件时间随用例数与单例成本线性增长；(2) 有效保护来自 oracle 覆盖而非用例数。边界反例：为提速盲目减少用例可能降低 kill rate，反而让缺陷逃逸成本更高；优化应优先删无效/重复用例、并行化，而非砍关键用例。

#### 3. 具体答案

面试中可以这样回答：

> 我把测试成本拆成运行时、资源占用和 flaky 重跑。运行时随用例数线性增长，我会用分层门禁：冒烟快速反馈，关键路径全跑，全量按夜跑。优化手段是删掉重复和弱断言用例、把独立用例并行、缓存依赖。flaky 的成本是重跑加排查，我会优先治理而不是加 retry。单位成本上我看“每次合并的 CI 分钟数”和“每个捕获缺陷的 CI 成本”。减少用例要保证 kill rate 不降，避免省时间换逃逸。

#### 4. 技术洞察

```
suite_time = Σ cases × repeats (+ queue)
flaky_cost ≈ rerun_rate × case_time
value = bugs_caught / CI_minutes
layers: smoke ≪ critical ≪ full
```

#### 5. 图文说明

| 层 | 深度 | 频率 |
|---|---|---|
| 冒烟 | 关键算子 | 每次提交 |
| 关键路径 | 端到端 | 每次合并 |
| 全量 | 全覆盖 | 夜跑 |

#### 6. 拓展知识

pytest-xdist 并行；测试缓存与依赖隔离；flaky 检测靠重复运行统计；变异测试用来判断删用例是否安全。

#### 7. 常见追问及回答要点

1. 如何提速不降保护？——删无效用例、并行、分层。
2. flaky 成本？——重跑与排查。
3. 如何判断可删用例？——变异测试 kill rate。

#### 8. 易错点与反例

1. 盲目砍用例，kill rate 下降。
2. 加 retry 掩盖 flaky，成本转移。
3. 全量每次都跑，CI 排队。

#### 9. 实践与验证

测各层套件时间；统计 flaky 重跑率；变异测试确认删用例后 kill rate。CI 数字 [待验证]。

#### 10. 评分标准

I070 评分：不合格只堆用例；合格能分层；良好能算套件与 flaky 成本；优秀/专家能用 kill rate 指导精简、平衡反馈速度与保护并量化 CI 成本。

### I071. 状态恢复：张量/设备

- 难度：中等级
- 知识域：K01 张量/设备
- 考察能力：资源预算、吞吐、尾延迟、容量阈值与单位成本
- 题型：性能/容量/成本
- 面试频率：高频
- 前置知识：shape、stride、dtype、device
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- 张量/设备的状态包括 device、dtype、layout（strided/sparse）、memory format（contiguous/channels_last）以及 pinned 属性；这些不都随 `state_dict` 保存。
- `torch.save` 保存张量时会记录其设备与 dtype；`map_location` 在加载时重映射设备，是跨设备恢复的关键参数。
- 恢复后若设备与原来不同，需要把参数、buffer 与优化器状态一起搬迁，遗漏任一项都会在 forward 时报 device 错。
- 编译器与后端可能缓存设备相关状态（如 cudnn autotune），这些不在 checkpoint 内，恢复后需重新预热。

#### 2. 问题分析

把张量/设备状态看作一组可恢复属性。两个不变量：(1) 恢复后所有进入计算的张量处于目标设备与 dtype；(2) 布局/连续性满足下游算子要求。边界反例：原模型在 `cuda:0`，恢复机器只有 `cuda:1`，若加载时不 `map_location`，`load_state_dict` 会把参数绑在旧设备上，直到第一次 forward 才暴露 device 错。

#### 3. 具体答案

面试中可以这样回答：

> 恢复张量/设备状态我关注三件事：设备、dtype、布局。加载时用 `map_location` 把参数映射到目标设备，然后显式把模型和 buffer 一起 `.to(device)`。dtype 要保持一致，混合精度下参数可能是 fp16 或 fp32 的状态，不能混淆。布局上，若原模型用了 channels_last 或某些视图假设，恢复后要保持或重建。编译器/后端的设备缓存不在 checkpoint 里，恢复后需要重新预热。

#### 4. 技术洞察

````python
sd = torch.load(path, map_location={"cuda:0": "cuda:1"}, weights_only=False)
model.load_state_dict(sd["model"])
model = model.to("cuda:1")
opt.load_state_dict(sd["optim"])   # 状态也需在 cuda:1
# 检查 buffer
for n, b in model.named_buffers():
    assert b.device == torch.device("cuda:1")
````

#### 5. 图文说明

| 属性 | 是否入 checkpoint | 恢复动作 |
|---|---|---|
| device | 记录原设备 | map_location + to |
| dtype | 是 | 校验一致 |
| layout/format | 部分 | 重建 |
| 编译缓存 | 否 | 重新预热 |

#### 6. 拓展知识

`weights_only=True` 更安全但仅支持张量与基本类型；分片模型需按 shard 恢复设备；`pin_memory` 的 pinned 属性不随 state_dict 保存。

#### 7. 常见追问及回答要点

1. 为什么要 map_location？——把保存设备映射到运行设备。
2. 优化器状态也要搬吗？——要，否则 device 错。
3. 编译缓存能存吗？——不在 checkpoint，需重建。

#### 8. 易错点与反例

1. 只 `load_state_dict` 不 `to`，buffer 留在旧设备。
2. dtype 不一致，加载后精度行为改变。
3. channels_last 未恢复，性能下降。

#### 9. 实践与验证

在不同设备间保存/加载，断言所有参数与 buffer 在目标设备；校验 dtype；测恢复后性能是否回到基线（预热后）。GPU 数字 [待验证]。

#### 10. 评分标准

I071 评分：不合格直接加载；合格能用 map_location；良好能覆盖 buffer/dtype/布局；优秀/专家能处理跨设备与编译缓存重建并验证设备一致性。

### I072. 状态恢复：autograd

- 难度：中等级
- 知识域：K02 autograd
- 考察能力：资源预算、吞吐、尾延迟、容量阈值与单位成本
- 题型：性能/容量/成本
- 面试频率：高频
- 前置知识：requires_grad、leaf、view、detach、反向
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- autograd 不提供跨进程保存的反向图：计算图是运行时对象，无法序列化到 checkpoint，恢复后必须重新 forward 建图。
- 与自动微分相关的可恢复状态主要是 RNG（dropout、随机算子）与确定性设置（`use_deterministic_algorithms`）。
- `retain_graph`/hook 是进程内的，进程重启后失效。
- 反向的数值正确性依赖前向的确定性与一致的参数，因此恢复等价性本质上是“重新建图 + 恢复 RNG”。

#### 2. 问题分析

定义 autograd 的状态恢复边界。两个不变量：(1) 恢复后重新 forward 构建的图与中断前的图在结构上等价（相同随机性）；(2) RNG 与确定性配置在恢复后被正确设置。边界反例：若中断发生在 backward 中途，梯度已部分写入 `.grad`，autograd 无法“继续”反向，只能丢弃本次反向、回到 step 边界重算。

#### 3. 具体答案

面试中可以这样回答：

> autograd 没有可以保存的反向图，图是运行时的，恢复时必须重新 forward 建图。所以我要恢复的是影响图的随机性：CPU/CUDA RNG 状态，以及 dropout 等随机算子的行为。确定性算法设置也要恢复，否则数值可能漂移。如果中断发生在 backward 过程中，梯度是半成品，无法续算，我的策略是丢弃这次反向，回滚到上一个完整 step 边界重新来。这样恢复后重新建图与原来的图等价。

#### 4. 技术洞察

```
无法序列化: grad_fn 图、hook、retain_graph 状态
可恢复: rng_state, deterministic config, seed
中断在 backward ⇒ 梯度不完整 ⇒ 回滚到 step 边界
```

````python
torch.set_rng_state(sd["rng_cpu"])
torch.cuda.set_rng_state_all(sd["rng_cuda"])
torch.use_deterministic_algorithms(True)   # 保持数值一致
````

#### 5. 图文说明

| 对象 | 可保存 | 恢复方式 |
|---|---|---|
| grad_fn 图 | 否 | 重新 forward |
| hook | 否 | 重新注册 |
| RNG | 是 | set_rng_state |
| 确定性 | 配置 | 重新设置 |

#### 6. 拓展知识

dropout 的 RNG 若未恢复，恢复后的随机序列与不中断基线不同；`torch.manual_seed` 只影响 CPU/当前设备；分布式下每个 rank 需各自的 RNG 状态。

#### 7. 常见追问及回答要点

1. 图能存吗？——不能，必须重建。
2. 恢复等价的关键？——RNG 与确定性。
3. 中断在 backward 怎么办？——回滚 step 边界。

#### 8. 易错点与反例

1. 以为能保存图，恢复后直接 backward 报错。
2. RNG 未恢复，dropout 序列错位。
3. 中断在反向未回滚，梯度半成品。

#### 9. 实践与验证

从 checkpoint 恢复后重新 forward+backward，比较 loss 与梯度范数是否与不中断基线一致（容差内）；打开确定性算法比较严格一致 [待验证]。

#### 10. 评分标准

I072 评分：不合格以为图可存；合格知道要重建；良好能恢复 RNG 与确定性；优秀/专家能定义中断在反向的回滚语义并验证梯度等价。

### I073. 状态恢复：Module/状态

- 难度：中等级
- 知识域：K03 Module/状态
- 考察能力：资源预算、吞吐、尾延迟、容量阈值与单位成本
- 题型：性能/容量/成本
- 面试频率：高频
- 前置知识：参数、buffer、模式和 state_dict
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- Module 的持久状态是 `state_dict`：参数与持久 buffer；模式（train/eval）与普通属性不自动保存。
- `load_state_dict` 需要先构造结构一致的模型；buffer 如 BN running stats 必须恢复，否则推理输出偏移。
- 参数共享关系不随 `state_dict` 自动恢复，需要在构造时重新建立。
- 冻结状态（`requires_grad`）是运行时配置，恢复后需重新设置。

#### 2. 问题分析

定义 Module 的状态恢复。两个不变量：(1) 恢复后模型输出与保存时一致（同输入同模式）；(2) 参数、buffer、共享关系、冻结标志全部恢复到位。边界反例：BN 的 running_mean/var 若不恢复，推理时用默认零/一统计，输出与训练结束时不一致，且 `strict=False` 可能静默通过。

#### 3. 具体答案

面试中可以这样回答：

> Module 恢复我先用同一份代码构造结构，然后 `load_state_dict` 加载参数和 buffer。BN 的 running stats 是 buffer，必须恢复，否则推理统计不对。参数共享关系要在构造时重建，加载不会自动连上别名。模式我用 `eval()` 或 `train()` 显式设置，冻结的层重新设 `requires_grad=False`。加载后我会用同输入断言输出与保存时一致，并检查 buffer 设备。

#### 4. 技术洞察

````python
model = Net()                     # 结构一致
missing, unexpected = model.load_state_dict(sd, strict=True)
model.eval()                      # 模式
for n, p in model.named_parameters():
    if n in frozen: p.requires_grad = False
assert torch.allclose(model(x), saved_output)
````

#### 5. 图文说明

| 状态 | 在 state_dict | 恢复动作 |
|---|---|---|
| 参数 | 是 | load |
| buffer | 是 | load |
| 共享 | 否 | 重建 |
| 模式/冻结 | 否 | 显式设置 |

#### 6. 拓展知识

`strict=True` 保证键集合一致；参数共享在量化/剪枝后可能断裂；DDP/FSDP 的 state_dict 可能是分片的，需要聚合或 DCP 处理。

#### 7. 常见追问及回答要点

1. buffer 为什么关键？——BN 统计影响推理。
2. 共享怎么恢复？——构造时绑定同一 Parameter。
3. 模式要存吗？——不存，运行时设置。

#### 8. 易错点与反例

1. 漏恢复 BN buffer，推理偏移。
2. 未重建共享，两处权重独立。
3. `strict=False` 吞掉缺失键。

#### 9. 实践与验证

保存后重新构造加载，同输入断言输出一致；检查 running stats；断言共享对象身份；检查冻结标志。无 GPU 依赖。

#### 10. 评分标准

I073 评分：不合格只 load 参数；合格能加载参数+buffer；良好能恢复模式、共享与冻结；优秀/专家能处理分片 state_dict 并用输出一致性验证。

### I074. 状态恢复：数据管道

- 难度：中等级
- 知识域：K04 数据管道
- 考察能力：资源预算、吞吐、尾延迟、容量阈值与单位成本
- 题型：性能/容量/成本
- 面试频率：高频
- 前置知识：Dataset、Sampler、collate、padding、版本
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- 数据管道的可恢复状态包括：当前 epoch、Sampler 的 epoch/seed、shuffle 的随机状态、以及（若需 mid-epoch 恢复）当前 batch 位置与 worker 状态。
- `DistributedSampler.set_epoch(epoch)` 决定该 epoch 的 shuffle 序列；不恢复 epoch 会重复或跳过数据。
- 标准 `DataLoader` 不提供 mid-epoch 断点续训；`torchdata` 的 `StatefulDataLoader` 或自定义 sampler 才能恢复 batch 级位置。
- worker 的增强随机性依赖 `worker_init_fn` 派生种子，恢复时要保证派生一致。

#### 2. 问题分析

定义数据管道的状态恢复。两个不变量：(1) 恢复后数据消费位置与中断前衔接，不重不漏；(2) 随机增强序列与不中断基线一致。边界反例：只恢复到 epoch 边界但训练在 epoch 中途崩溃，则会重复消费本 epoch 已见样本，导致数据分布偏移；严格 mid-epoch 恢复需要保存 batch 位置。

#### 3. 具体答案

面试中可以这样回答：

> 数据管道的恢复我至少保存 epoch 和全局 step，恢复时 `sampler.set_epoch(epoch)` 让 shuffle 序列对齐。若要 mid-epoch 精确恢复，我会用 `StatefulDataLoader` 或自定义可序列化的 sampler，保存当前 batch 位置，worker 增强种子也按同一规则派生。这样恢复后从断点续读，不重不漏。若只恢复到 epoch 边界，我会明确这是有意的近似，并记录重复消费量。RNG 状态要和数据管道一起恢复。

#### 4. 技术洞察

```
state = {epoch, global_step, sampler_epoch, worker_seeds, (batch_index)}
恢复: set_epoch(epoch); 若 mid-epoch 则 seek batch_index
```

````python
sd = torch.load(path)
sampler.set_epoch(sd["epoch"])
if "batch_index" in sd:
    loader = stateful_loader(state=sd["loader"])
````

#### 5. 图文说明

| 状态 | 粒度 | 缺失后果 |
|---|---|---|
| epoch | epoch | 重复/跳过数据 |
| batch_index | step | mid-epoch 重复 |
| worker seed | epoch | 增强错位 |
| RNG | 全局 | 顺序错乱 |

#### 6. 拓展知识

`StatefulDataLoader` 支持任意断点恢复；`persistent_workers` 与状态恢复交互需注意；分布式下每个 rank 的 sampler 状态独立。

#### 7. 常见追问及回答要点

1. 恢复粒度？——epoch 简单，mid-epoch 精确但需状态。
2. 为什么 set_epoch？——对齐 shuffle 序列。
3. worker 增强如何一致？——派生种子规则固定。

#### 8. 易错点与反例

1. 恢复不 set_epoch，shuffle 重复。
2. mid-epoch 崩溃只回 epoch 边界，重复消费。
3. worker 种子规则变更，增强不一致。

#### 9. 实践与验证

跑到中途保存并恢复，收集恢复前后索引断言衔接无重复；比较增强后样本是否与基线一致。多进程数字 [待验证]。

#### 10. 评分标准

I074 评分：不合格只存模型；合格能恢复 epoch；良好能处理 mid-epoch 与 worker 种子；优秀/专家能用 StatefulDataLoader 精确恢复并验证不重不漏。

### I075. 状态恢复：训练/优化器

- 难度：中等级
- 知识域：K05 训练/优化器
- 考察能力：资源预算、吞吐、尾延迟、容量阈值与单位成本
- 题型：性能/容量/成本
- 面试频率：高频
- 前置知识：forward、backward、累积、optimizer、scheduler
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- 训练/优化器可恢复状态：优化器 `state_dict`（Adam 的 m/v/step）、scheduler 的 `last_epoch`/`base_lrs`、梯度累积的 micro-step 计数。
- 恢复必须让优化器状态与参数对应：`param_groups` 顺序一致，否则动量错配。
- scheduler 的 `last_epoch` 决定下一步 lr；未恢复会出现 lr 跳变。
- 若使用了梯度累积，恢复点应对齐累积边界，避免半个累积窗口的梯度丢失或错配。

#### 2. 问题分析

定义训练/优化器的状态恢复。两个不变量：(1) 恢复后下一步的 lr 与动量与不中断基线一致；(2) 参数与优化器状态的对应关系保持不变。边界反例：恢复在累积窗口中间，若直接把 micro-step 计数清零，实际等效 batch 变小，梯度尺度与设计不符，收敛轨迹偏移。

#### 3. 具体答案

面试中可以这样回答：

> 优化器恢复我保存并加载 `optimizer.state_dict()`，它含动量和 step 计数；scheduler 保存 `last_epoch`，恢复后下一步 lr 才对。关键是参数顺序：恢复时重建的模型与优化器分组必须与保存时一致，否则动量错配到别的参数。若用梯度累积，我会把累积窗口对齐到 step 边界，或者把 micro-step 计数也存下来，避免半个窗口的尺度问题。我会断言恢复后 lr 与动量与基线一致。

#### 4. 技术洞察

````python
torch.save({"optim": opt.state_dict(), "sched": sched.state_dict(),
            "acc_step": acc_step}, path)
opt.load_state_dict(sd["optim"])
sched.load_state_dict(sd["sched"])
acc_step = sd["acc_step"]        # 对齐累积窗口
assert sched.get_last_lr() == expected_lr
````

#### 5. 图文说明

| 状态 | 内容 | 缺失后果 |
|---|---|---|
| optim | m/v/step | 动量清零 |
| sched | last_epoch | lr 跳变 |
| 分组顺序 | 参数索引 | 动量错配 |
| acc_step | 累积计数 | 尺度错 |

#### 6. 拓展知识

`Adam` 的 `step` 计数影响偏差修正，恢复后必须大于 0；`ReduceLROnPlateau` 还依赖指标历史；分片优化器（ZeRO）需 DCP 聚合恢复。

#### 7. 常见追问及回答要点

1. 为什么存 scheduler？——last_epoch 决定 lr。
2. 分组顺序为何关键？——状态按索引绑定。
3. 累积窗口怎么对齐？——存 acc_step 或回滚到边界。

#### 8. 易错点与反例

1. 只存优化器不存 scheduler，lr 跳变。
2. 分组顺序变化，动量错配。
3. 累积窗口中间恢复，尺度错。

#### 9. 实践与验证

保存恢复后断言 lr 序列与动量状态一致；比较后续 loss 与基线；累积场景比较等效梯度。无 GPU 依赖部分可本地验证。

#### 10. 评分标准

I075 评分：不合格只存模型；合格能存优化器；良好能恢复 scheduler 与分组；优秀/专家能处理累积窗口对齐与分片状态并验证 lr/动量一致。

### I076. 状态恢复：AMP/恢复

- 难度：中等级
- 知识域：K06 AMP/恢复
- 考察能力：资源预算、吞吐、尾延迟、容量阈值与单位成本
- 题型：性能/容量/成本
- 面试频率：高频
- 前置知识：autocast、scaler、快照、随机状态
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- AMP 可恢复状态：`GradScaler.state_dict()`（scale 与 growth_tracker）与训练 RNG 状态。
- scaler 的 scale 是训练动力学的一部分，恢复后丢失会让溢出处理行为改变。
- 溢出历史影响后续 scale 增长：未恢复会导致 scale 从初值重新爬升。
- 恢复顺序：先建模型、优化器，再加载 scaler，最后恢复 RNG，才能保证第一次 step 等价。

#### 2. 问题分析

定义 AMP/恢复的状态恢复。两个不变量：(1) 恢复后 scaler 的 scale 与 growth_tracker 与中断前一致；(2) RNG 使随机算子序列连续。边界反例：若恢复时先跑了一步再加载 scaler，第一步用了错误 scale，可能误判溢出并让 scale 偏离，恢复不再等价。

#### 3. 具体答案

面试中可以这样回答：

> AMP 恢复我保存 `scaler.state_dict()` 和 RNG。加载顺序是：先重建模型和优化器并加载它们的 state，然后 `scaler.load_state_dict` 恢复 scale 和 growth_tracker，最后恢复 RNG，之后才开始训练。这样第一步的缩放和随机性与中断前一致。我会断恢复后的 `get_scale()` 与保存值相等。BF16 不用 scaler 时，主要保证 RNG 恢复即可。

#### 4. 技术洞察

````python
torch.save({"scaler": scaler.state_dict(),
            "rng_cpu": torch.get_rng_state(),
            "rng_cuda": torch.cuda.get_rng_state_all()}, path)
scaler.load_state_dict(sd["scaler"])
torch.set_rng_state(sd["rng_cpu"]); torch.cuda.set_rng_state_all(sd["rng_cuda"])
assert scaler.get_scale() == saved_scale
````

#### 5. 图文说明

| 状态 | 内容 | 缺失后果 |
|---|---|---|
| scaler | scale, tracker | 溢出行为变 |
| RNG | 随机序列 | dropout 错位 |
| 顺序 | 加载次序 | 首步不等价 |
| 溢出历史 | tracker | scale 爬升异常 |

#### 6. 拓展知识

`growth_tracker` 记录连续无溢出次数，决定何时增长 scale；`init_scale` 太小会让训练初期保守但稳定；恢复严格等价依赖确定性算法。

#### 7. 常见追问及回答要点

1. 为什么存 scaler？——scale 影响溢出处理。
2. 加载顺序为何重要？——先训练会污染 scale。
3. BF16 要恢复什么？——RNG。

#### 8. 易错点与反例

1. 漏存 scaler，scale 重置。
2. 先训练后加载，首步尺度错。
3. RNG 未恢复，随机序列断。

#### 9. 实践与验证

恢复后断言 scale 与保存值相等；比较恢复后续 loss 与基线；BF16 路径单独测 RNG 一致性。GPU 数字 [待验证]。

#### 10. 评分标准

I076 评分：不合格不分 scaler；合格能存 scaler；良好能处理加载顺序与 RNG；优秀/专家能覆盖溢出历史/确定性并验证尺度连续性。

### I077. 状态恢复：分布式

- 难度：中等级
- 知识域：K07 分布式
- 考察能力：资源预算、吞吐、尾延迟、容量阈值与单位成本
- 题型：性能/容量/成本
- 面试频率：高频
- 前置知识：rank、DDP、FSDP、collective、容错
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- 分布式 checkpoint 要处理分片：FSDP/ZeRO 下每个 rank 只持有部分参数与优化器状态，需要聚合或分布式保存。
- `torch.distributed.checkpoint`（DCP）支持分片保存与在不同 world_size 下 re-shard。
- 恢复时若 world_size 变化，分片需重新划分；直接加载旧分片会错位。
- 各 rank 的 RNG 状态与 sampler 状态需要分别保存。

#### 2. 问题分析

定义分布式的状态恢复。两个不变量：(1) 恢复后各 rank 的参数/优化器分片正确对齐；(2) 允许 world_size 变化时能 re-shard。边界反例：把 FSDP 模型用普通 `state_dict` 保存，只有 rank 0 拿到完整聚合参数或每个 rank 只得本地分片，换 world_size 加载时参数缺失或错位。

#### 3. 具体答案

面试中可以这样回答：

> 分布式恢复我用 `torch.distributed.checkpoint`：它能按分片保存，并在加载时按新的 world_size 重新分片，所以换卡数也能恢复。保存时各 rank 一起调用 save，加载时一起 load 到对应分片。每个 rank 的 RNG 和 sampler 状态单独存并单独恢复。若用 FSDP，我用它的 `state_dict` 接口而不是裸参数，保证分片语义正确。恢复后我会 all_gather 断言各 rank 参数一致。

#### 4. 技术洞察

````python
import torch.distributed.checkpoint as dcp
dcp.save({"model": model.state_dict(), "optim": opt.state_dict()}, ckpt_id)
dcp.load(state, storage_reader=..., checkpoint_id=ckpt_id)
# 支持在不同 world_size 下 reshard
````

#### 5. 图文说明

| 状态 | 分片 | 恢复要点 |
|---|---|---|
| 参数 | 按 rank | DCP reshard |
| 优化器 | 按 rank | DCP reshard |
| RNG | 每 rank | 单独存 |
| sampler | 每 rank | set_epoch |

#### 6. 拓展知识

DCP 支持异步保存与多个 storage backend；FSDP 的 `FULL_SHARD` 下参数无副本，需聚合才能导出；弹性训练成员变更需 rendezvous 与 re-shard 配合。

#### 7. 常见追问及回答要点

1. 换卡数能恢复吗？——DCP re-shard 可以。
2. FSDP 怎么存？——用其 state_dict 接口。
3. RNG 为什么要每 rank？——各 rank 随机序列独立。

#### 8. 易错点与反例

1. 裸 state_dict 存 FSDP，分片缺失。
2. 只 rank0 存，其余分片丢。
3. 换 world_size 未 re-shard，错位。

#### 9. 实践与验证

用 DCP 存后在相同与不同 world_size 下加载，断言参数一致；all_gather 校验各 rank；检查 RNG/sampler 恢复。GPU/NCCL [待验证]。

#### 10. 评分标准

I077 评分：不合格只存 rank0；合格能存分片；良好能用 DCP re-shard；优秀/专家能处理 world_size 变化、异步保存与弹性恢复并验证一致性。

### I078. 状态恢复：编译栈

- 难度：中等级
- 知识域：K08 编译栈
- 考察能力：资源预算、吞吐、尾延迟、容量阈值与单位成本
- 题型：性能/容量/成本
- 面试频率：高频
- 前置知识：Dynamo、AOTAutograd、Inductor、guard、缓存
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- `torch.compile` 的编译缓存是进程内的，重启后失效；checkpoint 不含编译产物与 guard 状态。
- 恢复后第一次调用会重新捕获图并编译，产生与首次启动相同的一次性开销。
- guard 依赖输入属性（shape/dtype/device），恢复后输入属性需与之前一致才能命中缓存；否则重编译。
- CUDA graphs 的捕获也在恢复后重做，要求静态形状与地址。

#### 2. 问题分析

定义编译栈的恢复边界。两个不变量：(1) 编译产物不入 checkpoint，恢复后必须重建；(2) 输入属性一致时重建后的缓存行为与原来一致。边界反例：恢复后若输入形状与训练早期不同（例如恢复时用了不同 batch），会触发额外重编译，恢复期的性能曲线与原来不同。

#### 3. 具体答案

面试中可以这样回答：

> 编译缓存不进 checkpoint，恢复后要重新捕获和编译，所以我会把这一步算进恢复成本。第一次调用会有编译长尾，之后稳态应与原来一致。为保证缓存命中，恢复时输入形状、dtype、设备要和原来一致，否则 guard 失败重编译。CUDA graphs 也要重新捕获，需要静态形状和地址。我可以在恢复后跑几步预热再进入正式训练，避免把编译长尾计入性能统计。

#### 4. 技术洞察

```
checkpoint ⊅ compile cache, guards, cuda graphs
恢复: first call → capture → compile (一次性)
预热若干步后再衡量性能
```

#### 5. 图文说明

| 对象 | 是否入 checkpoint | 恢复动作 |
|---|---|---|
| 编译产物 | 否 | 重新编译 |
| guard | 否 | 重建 |
| CUDA graph | 否 | 重新捕获 |
| 预热 | — | 恢复后跑几步 |

#### 6. 拓展知识

Inductor 的 autotune 缓存可持久化到磁盘以跨进程复用；`torch._dynamo.config` 的缓存策略影响重编译；动态 shape 减少恢复期重编译。

#### 7. 常见追问及回答要点

1. 编译缓存能存吗？——不能，需重建。
2. 恢复为何可能变慢？——编译长尾与重编译。
3. 如何避免？——固定输入属性并预热。

#### 8. 易错点与反例

1. 把恢复首步耗时当稳态。
2. 输入属性变化，恢复后重编译。
3. 忽略 CUDA graph 重新捕获成本。

#### 9. 实践与验证

恢复后测首步与稳态 step，若首步显著大则是编译开销；固定输入断言重编译为 0；对比恢复前后稳态时间。GPU 数字 [待验证]。

#### 10. 评分标准

I078 评分：不合格以为缓存可存；合格知道要重编；良好能处理 guard 与预热；优秀/专家能持久化 autotune 缓存、处理动态 shape 并区分首步与稳态。

### I079. 状态恢复：性能

- 难度：中等级
- 知识域：K09 性能
- 考察能力：资源预算、吞吐、尾延迟、容量阈值与单位成本
- 题型：性能/容量/成本
- 面试频率：高频
- 前置知识：关键路径、带宽、算力、通信、显存、统计
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- 重启后的性能恢复依赖预热：cuBLAS/cuDNN 句柄初始化、autotune 选择、分配器缓存填充都在首次调用发生。
- cudnn benchmark（`torch.backends.cudnn.benchmark=True`）会在首次遇到某形状时搜索算法并缓存，重启后需重搜。
- 显存分配器的缓存与碎片状态不跨进程，恢复后可能因分配模式不同而峰值不同。
- 严谨的性能对比要求相同的预热长度与相同的输入序列。

#### 2. 问题分析

定义性能状态的恢复。两个不变量：(1) 稳态性能在重启后应与之前一致（硬件与环境不变）；(2) 首步包含一次性初始化，不用于比较。边界反例：不加预热比较重启前后耗时，会把初始化算进去，得出“重启变慢”的错误结论。

#### 3. 具体答案

面试中可以这样回答：

> 重启后性能不会立刻回到稳态，因为有一堆一次性的初始化：cuBLAS/cuDNN 句柄、算法 autotune、分配器缓存。所以我会在恢复后跑固定的预热步数再开始测量，测 P50/P95 而不是首步。如果开了 cudnn benchmark，重启后要重新搜索并缓存。为让对比公平，重启前后用相同的预热长度与相同输入序列。单位成本上，我关心稳态吞吐而不是冷启动数字。

#### 4. 技术洞察

```
cold_start: 句柄初始化 + autotune + 分配器填充
warmup = fixed steps, discard
compare: same warmup + same inputs
cudnn.benchmark=True ⇒ 首次搜索并缓存
```

````python
for _ in range(50): step()      # 预热，丢弃
torch.cuda.synchronize()
# 之后测稳态 P50/P95
````

#### 5. 图文说明

| 状态 | 生命周期 | 恢复动作 |
|---|---|---|
| 库句柄 | 进程内 | 预热 |
| autotune 缓存 | 进程内 | 重新搜索 |
| 分配器缓存 | 进程内 | 预热填充 |
| 硬件/环境 | 持久 | 保持一致 |

#### 6. 拓展知识

cudnn benchmark 对动态形状会反复搜索，宜固定形状；CUDA graphs 也需重新捕获；容器/驱动版本变化会改变稳态，需纳入对比条件。

#### 7. 常见追问及回答要点

1. 为什么要预热？——跳过一次性初始化。
2. autotune 会跨进程吗？——不会，需重搜。
3. 如何公平比较？——相同预热与输入。

#### 8. 易错点与反例

1. 用首步耗时比较重启前后。
2. 动态形状使 autotune 反复搜索。
3. 分配器碎片导致峰值不同误判泄漏。

#### 9. 实践与验证

重启前后各预热相同步数再测稳态；对比 autotune 是否重新触发；检查显存峰值。GPU 数字 [待验证]。

#### 10. 评分标准

I079 评分：不合格用冷启动比较；合格知预热；良好能处理 autotune 与分配器状态；优秀/专家能设计公平对比条件并量化稳态恢复。

### I080. 状态恢复：导出部署

- 难度：中等级
- 知识域：K10 导出部署
- 考察能力：资源预算、吞吐、尾延迟、容量阈值与单位成本
- 题型：性能/容量/成本
- 面试频率：高频
- 前置知识：state_dict、Export、runtime、schema、迁移
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- 部署侧的“状态”是模型产物（导出图/权重）及其版本；生产要求可回滚到上一稳定版本。
- 导出物与运行时版本绑定：ONNX opset、TensorRT 引擎与驱动/库版本相关，跨版本可能不兼容。
- 权重与其代码结构需匹配；只回滚权重不回滚代码可能错配。
- 产物需要版本标签、校验和与来源记录，以支持审计与快速回滚。

#### 2. 问题分析

定义导出部署的状态恢复。两个不变量：(1) 任一历史产物可在其目标 runtime 上复现；(2) 回滚是原子且可验证的。边界反例：回滚只换权重文件但 runtime 引擎（如 TRT plan）未换，权重与引擎图不匹配，推理出错；必须按“权重+图+引擎+版本”成套回滚。

#### 3. 具体答案

面试中可以这样回答：

> 部署恢复我按“成套产物”管理：导出图/权重、对应的 runtime 引擎、以及版本元数据一起打标签。回滚时整套切换，而不是只换权重，避免图和权重错配。产物要能在目标 runtime 复现，所以我会记录 opset/引擎版本/驱动并做兼容性测试。恢复后我用一组基准输入对比输出，确认与上一版本在容差内一致。审计上保留校验和与来源，支持快速定位和回滚。

#### 4. 技术洞察

```
artifact = {weights, graph/onnx, runtime_engine, version_meta}
rollback = atomic swap of whole artifact
verify: bench_inputs → outputs ≈ previous (atol)
```

#### 5. 图文说明

| 产物 | 版本绑定 | 回滚要点 |
|---|---|---|
| 权重 | 代码结构 | 与图成套 |
| 图/ONNX | opset | 成套 |
| 引擎 | 驱动/库 | 成套 |
| 元数据 | 来源 | 校验和 |

#### 6. 拓展知识

TensorRT 引擎常与具体 GPU 架构和库版本绑定，通常不跨机器移植，需在目标机重新构建；ONNX 更可移植但算子支持受限；灰度发布先小流量验证再全量。

#### 7. 常见追问及回答要点

1. 为什么成套回滚？——权重与图/引擎需匹配。
2. 引擎能跨机器吗？——通常不能，需重建。
3. 如何验证回滚正确？——基准输入对比输出。

#### 8. 易错点与反例

1. 只换权重不换引擎，图权重错配。
2. 跨版本直接用旧产物，不兼容。
3. 无版本元数据，回滚定位慢。

#### 9. 实践与验证

用基准输入对比回滚前后输出；在目标 runtime 上验证产物可加载；检查元数据完整。运行时数字 [待验证]。

#### 10. 评分标准

I080 评分：不合格只换权重；合格能成套管理；良好能处理版本兼容与验证；优秀/专家能设计灰度与原子回滚、记录元数据并保证可复现。

### I081. 状态恢复：自定义算子

- 难度：中等级
- 知识域：K11 自定义算子
- 考察能力：接口、状态机、隔离、灰度、回滚与治理
- 题型：系统设计/工程落地
- 面试频率：高频
- 前置知识：schema、dispatcher、stream、autograd、ABI
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- 自定义算子以扩展形式加载（`torch.ops.load_library`/`.so`），其状态包括已编译的 kernel、注册的 schema 与 autograd 规则。
- 扩展与 PyTorch 主版本存在 ABI 兼容要求，跨版本可能加载失败；编译产物通常不能跨版本复用。
- 进程重启后扩展需重新加载与注册，缓存（如编译的 .cu 产物）可持久化以加速。
- 自定义算子的 autograd 注册也需在加载后重建，否则反向缺失。

#### 2. 问题分析

定义自定义算子的状态恢复。两个不变量：(1) 扩展在目标 PyTorch 版本上可加载且 schema/autograd 注册完整；(2) 编译缓存可复用但需版本校验。边界反例：把在一个 PyTorch 版本编译的扩展拿到另一版本加载，ABI 不兼容导致崩溃或 `undefined symbol`，必须按版本重建。

#### 3. 具体答案

面试中可以这样回答：

> 自定义算子的恢复分两部分：编译产物和注册。进程重启后扩展要重新加载并重新注册 schema 和 autograd，否则前向能用但反向缺失。编译缓存（如 ninja 的 .so）可以持久化加速，但我会校验 PyTorch 版本与 ABI，不兼容就重建。ABI 不匹配会导致加载失败或符号缺失。恢复后我会跑一组参考用例，对比自定义算子与参考实现的输出和梯度，确认注册完整。

#### 4. 技术洞察

```
load: torch.ops.load_library(so) → 注册 schema/kernel/autograd
cache: .so/.cu 可持久化, 需版本校验
ABI: 与 torch 主版本绑定, 跨版本重建
```

````python
torch.ops.load_library("build/mylib.so")
out = torch.ops.mylib.scale(x, 2.0)
assert gradcheck(...)             # 恢复后验证 autograd 注册
````

#### 5. 图文说明

| 状态 | 是否持久 | 恢复动作 |
|---|---|---|
| 编译 .so | 可缓存 | 版本校验后复用 |
| schema 注册 | 进程内 | 重新注册 |
| autograd | 进程内 | 重新注册 |
| ABI | 版本绑定 | 不兼容则重建 |

#### 6. 拓展知识

`TORCH_LIBRARY` 支持静态注册；`pybind11`/torch extension 构建产物带版本后缀；CUDA 扩展需匹配 CUDA 与 torch 版本；jit 编译在首次调用时才编译。

#### 7. 常见追问及回答要点

1. 重启后要重新注册什么？——schema 与 autograd。
2. 编译缓存能复用吗？——可，但需版本校验。
3. ABI 不匹配会怎样？——加载失败/符号缺失。

#### 8. 易错点与反例

1. 只加载扩展忘注册 autograd，反向缺失。
2. 跨版本复用 .so，崩溃。
3. 缓存未校验版本，行为不确定。

#### 9. 实践与验证

重启后重新加载扩展并跑 gradcheck 与参考对比；跨版本加载测试确认报错；缓存命中前后性能对比。GPU 结果 [待验证]。

#### 10. 评分标准

I081 评分：不合格只加载 .so；合格知道要重新注册；良好能处理版本/ABI 与缓存；优秀/专家能设计版本校验与自动重建、用 gradcheck 验证并量化缓存收益。

### I082. 状态恢复：测试治理

- 难度：中等级
- 知识域：K12 测试治理
- 考察能力：接口、状态机、隔离、灰度、回滚与治理
- 题型：系统设计/工程落地
- 面试频率：高频
- 前置知识：oracle、证据、日志、回滚、门禁、复盘
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- 测试治理的“状态恢复”是复现失败：需要保存种子、环境指纹、输入数据与产物快照。
- 失败复现要求确定性：固定种子、确定性算法、隔离外部依赖。
- 事故复盘需要现场快照：日志、指标、当时的 model/optimizer checkpoint。
- 回归用例从失败现场提炼，成为永久防护。

#### 2. 问题分析

定义测试治理的状态恢复。两个不变量：(1) 任一失败都能用保存的状态在本地复现；(2) 复盘产出回归用例并接入门禁。边界反例：只有一句“CI 挂了”而无种子、输入与日志，失败无法复现，导致反复出现同类问题却无法定位。

#### 3. 具体答案

面试中可以这样回答：

> 我把“可复现”作为失败恢复的核心：每个用例固定种子并记录环境指纹，失败时把输入、日志和相关 checkpoint 一起归档。这样任何人都能在本地用同一状态复现。复盘时我基于现场快照定位根因，并把最小复现提炼成回归用例接入门禁。若失败依赖随机性，我先隔离随机源再复现，而不是加 retry 掩盖。这样失败现场→复现→根因→回归形成闭环。

#### 4. 技术洞察

```
snapshot = {seed, env_fingerprint, inputs, checkpoint, logs}
reproduce: same snapshot → same failure
postmortem → minimal repro → regression test → gate
```

````python
def test_x(record):
    seed = 12345; torch.manual_seed(seed)
    record(seed=seed, env=fingerprint())
````

#### 5. 图文说明

| 现场 | 保存内容 | 用途 |
|---|---|---|
| 种子 | seed | 复现随机 |
| 环境 | 版本/驱动 | 复现环境 |
| 输入 | 数据快照 | 最小复现 |
| checkpoint | 模型状态 | 现场还原 |

#### 6. 拓展知识

确定性算法与固定种子共同保证复现；外部依赖（网络/时间）需 mock；现场快照要版本化，避免被后续提交覆盖；事故复盘记录根因与防护。

#### 7. 常见追问及回答要点

1. 复现需要什么？——种子、环境、输入、日志。
2. 随机失败怎么处理？——隔离随机源，不加 retry。
3. 复盘产出什么？——回归用例与防护。

#### 8. 易错点与反例

1. 失败只留一句“挂了”，无法复现。
2. 依赖未 mock，本地不复现。
3. 不提炼回归，同类问题反复。

#### 9. 实践与验证

用归档的种子与输入在本地复现失败；把最小复现转成回归用例并验证能捕获；检查环境指纹记录完整。无 GPU 依赖部分本地跑 [待验证]。

#### 10. 评分标准

I082 评分：不合格无现场快照；合格能保存种子与日志；良好能用快照复现并提炼回归；优秀/专家能建立失败→复现→根因→回归闭环并治理随机依赖。

### I083. 实现取舍：张量/设备

- 难度：中等级
- 知识域：K01 张量/设备
- 考察能力：接口、状态机、隔离、灰度、回滚与治理
- 题型：系统设计/工程落地
- 面试频率：高频
- 前置知识：shape、stride、dtype、device
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- 张量层的工程取舍包括：是否用 `Tensor` 子类、内存布局（channels_first/last）、设备迁移的集中化位置、以及是否大量使用视图。
- 子类（如子类化 Tensor）提供语义扩展但引入兼容性与性能风险，多数场景优先用组合而非继承。
- channels_last 对卷积后端友好，但对逐元素算子可能引入重排；布局选择需按主算子类型定。
- 设备迁移若散落各处会带来隐式拷贝与同步；集中在边界层则可控。

#### 2. 问题分析

为张量/设备层设计接口与隔离。两个不变量：(1) 设备/布局约定在模块边界统一，内部不混用；(2) 视图的使用不引入隐式别名 bug。边界反例：为每个算子内部各写 `.to(device)` 看似防御，实则隐藏了真正的设备来源问题，还在热路径引入拷贝与同步，性能下降。

#### 3. 具体答案

面试中可以这样回答：

> 我在张量层的取舍是：能用组合就不用 Tensor 子类，避免兼容性坑；设备迁移集中在一个边界层做，内部约定所有输入同设备同 dtype，而不是每个算子各自 `.to`。布局按主算子选，卷积重则 channels_last，纯逐元素则 contiguous。视图能省显存但别名风险大，我在需要复制的语义处显式 clone。接口上定义清楚的 dtype/device 契约，配合断言。回滚上，布局或子类改动如果出问题，能通过开关切回旧路径。

#### 4. 技术洞察

```
interface: fn(x: Tensor) -> Tensor, x 约定同 device/dtype
isolation: 边界层统一 to/contiguous
tradeoff: view(0 copy, alias risk) vs clone(safe, cost)
```

#### 5. 图文说明

| 取舍 | 选项 A | 选项 B | 依据 |
|---|---|---|---|
| 扩展 | 组合 | 子类 | 兼容性 |
| 迁移 | 边界集中 | 各处 to | 可控性 |
| 布局 | channels_last | contiguous | 主算子 |
| 视图 | view | clone | 别名风险 |

#### 6. 拓展知识

Tensor 子类与 `torch.compile`、autograd 的交互复杂；`__torch_function__` 是扩展语义的标准入口；布局切换要测性能回归。

#### 7. 常见追问及回答要点

1. 为什么少用子类？——兼容性与编译/自动微分风险。
2. 迁移放哪？——边界层集中。
3. 视图何时变 clone？——需要独立语义时。

#### 8. 易错点与反例

1. 各处 `.to` 掩盖来源，性能差。
2. 子类化破坏编译。
3. 视图别名导致意外共享写入。

#### 9. 实践与验证

对边界层加设备/dtype 断言；A/B 布局切换测性能；构造别名场景验证 clone 语义。GPU 数字 [待验证]。

#### 10. 评分标准

I083 评分：不合格无约定；合格能集中迁移；良好能按主算子选布局并权衡视图；优秀/专家能设计接口契约、灰度布局/子类改动并验证回滚。

### I084. 实现取舍：autograd

- 难度：中等级
- 知识域：K02 autograd
- 考察能力：接口、状态机、隔离、灰度、回滚与治理
- 题型：系统设计/工程落地
- 面试频率：高频
- 前置知识：requires_grad、leaf、view、detach、反向
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- 自动微分的实现取舍：优先用组合的原生算子（自动获得反向），必要时写 `autograd.Function`，或实现更底层的双反向。
- in-place 省显存但有别名与版本号风险；功能化（functionalization）把 in-place 转纯函数以支持编译。
- 激活检查点用重算换显存，粒度可选择；`no_grad` 用于明确不需要梯度的路径。
- 自定义 Function 需正确 `save_for_backward`，否则中间量释放导致反向错误。

#### 2. 问题分析

为自动微分层设计接口与隔离。两个不变量：(1) 反向语义与数值正确（可用 gradcheck 验证）；(2) 内存与计算代价在可接受范围。边界反例：为省显存大量用 in-place，虽然前向能跑，但反向因版本号不一致报错，或与编译的功能化冲突，得不偿失。

#### 3. 具体答案

面试中可以这样回答：

> 我的原则是能用原生算子组合就不手写反向，组合天然有正确梯度。确需自定义时我用 `autograd.Function`，明确 `save_for_backward`，并用 gradcheck 验证。in-place 我要谨慎：省显存但引入别名和版本风险，还会和编译的功能化打架，倾向于 out-of-place 或让功能化处理。显存紧张就用激活检查点，选合适粒度。路径隔离上，需要梯度和不需要梯度的代码用 `no_grad` 分开。

#### 4. 技术洞察

```
组合算子: 自动反向, 首选
autograd.Function: 手写, save_for_backward, gradcheck 验证
in-place: 省显存, alias/version 风险
checkpoint: 重算换显存, 粒度可调
```

#### 5. 图文说明

| 取舍 | 省 | 代价 |
|---|---|---|
| 组合算子 | 开发 | 无 |
| 自定义 Fn | 灵活 | 需正确反向 |
| in-place | 显存 | 别名/版本风险 |
| checkpoint | 显存 | 时间 |

#### 6. 拓展知识

功能化是编译与自动微分的桥梁；双反向（`create_graph`）代价高；`torch.func` 提供函数式 API 便于变换。

#### 7. 常见追问及回答要点

1. 何时写自定义反向？——原生无对应或性能需要。
2. in-place 的风险？——别名与版本号。
3. 检查点粒度？——按显存-时间权衡。

#### 8. 易错点与反例

1. 手写反向未 gradcheck，梯度错。
2. in-place 导致反向报错。
3. 检查点全局开启，时间暴涨。

#### 9. 实践与验证

gradcheck 校验自定义反向；对比 in-place 与 out-of-place 的显存与正确性；测检查点粒度的影响。GPU 数字 [待验证]。

#### 10. 评分标准

I084 评分：不合格偏爱手写反向；合格知优先组合；良好能权衡 in-place/检查点；优秀/专家能用 gradcheck 与功能化建立正确性防护并灰度自定义反向。

### I085. 实现取舍：Module/状态

- 难度：中等级
- 知识域：K03 Module/状态
- 考察能力：接口、状态机、隔离、灰度、回滚与治理
- 题型：系统设计/工程落地
- 面试频率：高频
- 前置知识：参数、buffer、模式和 state_dict
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- Module 设计的取舍：参数用 `nn.Parameter`，状态用 buffer；子模块用 `ModuleList/ModuleDict` 而非普通容器以正确注册。
- `state_dict` 的键设计影响可迁移性；过多非持久 buffer 会丢失状态，过少会让状态泄漏到外部。
- hook 用于观测/干预但增加复杂度，需保证清理避免泄漏。
- 模式（train/eval）与冻结（requires_grad）是运行时状态，应集中管理。

#### 2. 问题分析

为 Module 层设计接口与隔离。两个不变量：(1) 所有需要持久化的状态都在 `state_dict`；(2) 子模块与参数正确注册，优化器能发现全部可训练参数。边界反例：把层放进 Python list，参数未注册，优化器漏掉它们，训练“正常”但那些层永不更新，属接口设计缺陷。

#### 3. 具体答案

面试中可以这样回答：

> 我在 Module 层坚持：可训练张量一律 `nn.Parameter`，需要随模型保存与迁移但不训练的量用 `register_buffer`；层容器用 `ModuleList/ModuleDict`，保证注册到 `_modules`。`state_dict` 的键我保持稳定，避免随意改名影响迁移。hook 只用于必要观测，用完清理。模式与冻结集中在一个入口设置，保证所有子模块一致。这样优化器、保存、迁移都能正确覆盖全部状态。

#### 4. 技术洞察

```
Parameter → _parameters → optimizer 发现
register_buffer → _buffers → state_dict/设备迁移
ModuleList/Dict → _modules → 递归注册
```

#### 5. 图文说明

| 需求 | 正确做法 | 错误做法 |
|---|---|---|
| 可训练 | Parameter | 普通 Tensor |
| 持久状态 | buffer | self.x |
| 动态层 | ModuleList | list |
| 状态迁移 | state_dict 稳定键 | 随意改名 |

#### 6. 拓展知识

`persistent=False` 用于可重建的缓存；tied weights 要求同一 Parameter 被多处引用；hook 的 `remove()` 避免泄漏；DDP 依赖注册的稳定性。

#### 7. 常见追问及回答要点

1. buffer 与 Parameter 区别？——是否训练与是否进优化器。
2. 为什么用 ModuleList？——正确注册。
3. hook 要注意什么？——清理与性能。

#### 8. 易错点与反例

1. list 存层，优化器漏参数。
2. 状态用普通属性，迁移丢失。
3. hook 不清理，内存泄漏。

#### 9. 实践与验证

断言 `named_parameters` 覆盖全部可训练张量；`state_dict` 键稳定；验证 buffer 随设备迁移。无 GPU 依赖。

#### 10. 评分标准

I085 评分：不合格用普通容器；合格能区分 param/buffer；良好能设计稳定 state_dict 与集中模式管理；优秀/专家能处理共享参数、hook 治理并验证注册完整性。

### I086. 实现取舍：数据管道

- 难度：中等级
- 知识域：K04 数据管道
- 考察能力：接口、状态机、隔离、灰度、回滚与治理
- 题型：系统设计/工程落地
- 面试频率：高频
- 前置知识：Dataset、Sampler、collate、padding、版本
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- 数据管道取舍：`map-style Dataset` 便于随机访问与分布式采样；`IterableDataset` 适合流式/超大数据但不能随机访问。
- worker 数与 prefetch 影响吞吐与内存；`persistent_workers` 省重启但有状态残留。
- `pin_memory` + 非阻塞拷贝把 H2D 与计算重叠；GPU 侧增强（如 DALI）可进一步卸载 CPU。
- collate 的复杂度影响 worker 时间；变长序列分桶能提升 padding 效率。

#### 2. 问题分析

为数据管道设计接口与隔离。两个不变量：(1) 采样与预处理接口在数据源变化时保持稳定；(2) 资源（worker/pin/prefetch）可通过配置调节而不改代码。边界反例：为追吞吐把增强全塞进 worker，CPU 成为瓶颈且难以扩展；把重增强迁到 GPU 或换 DALI 才是可扩展方向。

#### 3. 具体答案

面试中可以这样回答：

> 我按数据特性选 Dataset 类型：需要随机访问和分布式采样用 map-style；流式大数据用 IterableDataset 并手动按 worker/rank 分片。预处理接口保持稳定，重增强考虑迁到 GPU 或 DALI，避免 CPU 成为瓶颈。资源用配置控制：worker 数、prefetch、pin_memory 都可调，找吞吐拐点。collate 尽量轻，变长序列用分桶。隔离上，把随机种子派生规则固定，保证可复现。

#### 4. 技术洞察

```
map-style: random access, Sampler-friendly
Iterable: streaming, manual shard
pin + non_blocking: overlap H2D
GPU augment (DALI): offload CPU
```

#### 5. 图文说明

| 场景 | Dataset | 增强位置 |
|---|---|---|
| 随机访问 | map-style | CPU/GPU |
| 流式 | Iterable | GPU |
| 变长 | map + 分桶 | — |
| 大吞吐 | map + 多 worker | GPU |

#### 6. 拓展知识

DALI 提供 GPU 预处理流水线；`StatefulDataLoader` 支持恢复；共享内存限制 worker 数上限；分桶需与 sampler 配合。

#### 7. 常见追问及回答要点

1. map vs Iterable？——随机访问需求。
2. 何时迁到 GPU？——CPU 预处理成瓶颈。
3. worker 数上限？——共享内存与内存。

#### 8. 易错点与反例

1. CPU 增强过重，加卡不加速。
2. Iterable 未分片，重复。
3. collate 过重，worker 成瓶颈。

#### 9. 实践与验证

扫描 worker/prefetch 找拐点；A/B 对比 CPU 与 GPU 增强；测 padding 效率。多进程数字 [待验证]。

#### 10. 评分标准

I086 评分：不合格只设 worker；合格能选 Dataset；良好能配置资源并考虑 GPU 增强；优秀/专家能按瓶颈迁移增强、设计分片与分桶并验证吞吐。

### I087. 实现取舍：训练/优化器

- 难度：中等级
- 知识域：K05 训练/优化器
- 考察能力：接口、状态机、隔离、灰度、回滚与治理
- 题型：系统设计/工程落地
- 面试频率：高频
- 前置知识：forward、backward、累积、optimizer、scheduler
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- 优化器选择取舍：SGD 通用但慢，Adam/AdamW 收敛快但状态占显存；大模型常用 AdamW + cosine/warmup。
- 参数分组：对 bias/归一化参数关闭 weight decay 是常见实践。
- 梯度累积用于在显存受限下模拟大 batch，需缩放 loss。
- scheduler 语义（step/epoch）与 warmup 影响稳定性。

#### 2. 问题分析

为训练/优化器设计接口与治理。两个不变量：(1) 超参与分组规则确定、可复现；(2) 梯度累积与学习率缩放保持等效 batch 语义。边界反例：换优化器只改类名而不调整 lr/warmup，收敛曲线完全不同，属“改一处未改配套”的取舍失误。

#### 3. 具体答案

面试中可以这样回答：

> 优化器我按规模和稳定性选：小模型或追求泛化用 SGD+动量，大模型用 AdamW 配 cosine 和 warmup。分组上对 bias 和 norm 参数关 weight decay。显存不够就用梯度累积模拟大 batch，并按累积步数缩放 loss，同时相应调整 lr。换优化器时我把它当成一次实验：配套调 lr/warmup，用开关灰度，出问题能回滚。所有超参和分组规则集中管理，保证可复现。

#### 4. 技术洞察

```
SGD+momentum: 泛化, 需调 lr
AdamW: 快, 状态 2P, decay 解耦
accum: loss/K, 等效 batch B·K
warmup + cosine: 稳定
```

#### 5. 图文说明

| 取舍 | SGD | AdamW |
|---|---|---|
| 收敛 | 慢 | 快 |
| 显存 | 低 | 高(2P) |
| 泛化 | 常更好 | 需调 |
| 配套 | lr/warmup | lr/warmup |

#### 6. 拓展知识

8-bit Adam 省状态；梯度累积需与 DDP 的 no_sync 配合；warmup 长度与 batch 相关；loss scaling 与累积缩放要统一。

#### 7. 常见追问及回答要点

1. 何时用 AdamW？——大模型快速收敛。
2. 分组为什么要关 wd？——一维参数不宜收缩。
3. 累积怎么等效？——loss 除 K 并相应调 lr。

#### 8. 易错点与反例

1. 换优化器不调 lr，收敛异常。
2. 累积不缩放 loss，尺度错。
3. 分组规则不确定，不可复现。

#### 9. 实践与验证

A/B 对比优化器与 lr 曲线；累积等效性 allclose；灰度切换并记录回滚。GPU 数字 [待验证]。

#### 10. 评分标准

I087 评分：不合格只改类名；合格能选优化器；良好能分组与处理累积缩放；优秀/专家能配套调 lr/warmup、灰度回滚并量化收敛差异。

### I088. 实现取舍：AMP/恢复

- 难度：中等级
- 知识域：K06 AMP/恢复
- 考察能力：接口、状态机、隔离、灰度、回滚与治理
- 题型：系统设计/工程落地
- 面试频率：高频
- 前置知识：autocast、scaler、快照、随机状态
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- 精度取舍：FP16 配 GradScaler 精度高但范围窄；BF16 范围大通常免 scaler，尾数少。
- checkpoint 方案取舍：完整快照（含优化器/调度器/RNG）恢复等价但体积大；仅权重轻量但恢复不完整。
- scaler 的初始 scale 与 backoff 影响溢出频率与训练速度。
- 敏感算子需强制 fp32，写作白名单或手动升精度。

#### 2. 问题分析

为 AMP/恢复设计接口与治理。两个不变量：(1) 精度策略与算子白名单确定，数值可控；(2) checkpoint 覆盖恢复所需的全部状态。边界反例：为省空间只存权重，恢复后优化器动量与 scaler 重置，loss 出现台阶，恢复不等价。

#### 3. 具体答案

面试中可以这样回答：

> 精度上我按硬件和数据选：追求数值稳就 BF16，老硬件或需要更高尾数精度就 FP16 配 scaler。敏感算子用白名单强制 fp32。checkpoint 我倾向于完整快照：模型、优化器、调度器、scaler、RNG 和进度，保证恢复等价；只在明确接受近似时才存纯权重。scaler 初始 scale 和 backoff 我会按溢出率调，避免频繁跳过。所有配置集中，灰度上线，异常可回滚到 FP32。

#### 4. 技术洞察

```
FP16: 高尾数, 窄范围, 需 scaler
BF16: 宽范围, 免 scaler, 尾数少
ckpt full: 等价恢复, 大
ckpt weights-only: 轻, 不等价
```

#### 5. 图文说明

| 取舍 | 选项 | 代价 |
|---|---|---|
| 精度 | FP16/BF16 | scaler/尾数 |
| 快照 | 完整/权重 | 体积/等价性 |
| 白名单 | 强制 fp32 | 少量开销 |
| scale | 初始/backoff | 溢出率 |

#### 6. 拓展知识

scaler 状态必须入 checkpoint；bf16 归约类算子仍需谨慎；8-bit 优化器可缩小快照；恢复等价还依赖确定性算法。

#### 7. 常见追问及回答要点

1. FP16 还是 BF16？——按硬件与数值需求。
2. 快照存什么？——恢复等价所需全部状态。
3. 何时可只存权重？——接受近似或仅推理时。

#### 8. 易错点与反例

1. 只存权重，恢复不等价。
2. 敏感算子未白名单，数值偏差。
3. scale 初始过大，频繁溢出。

#### 9. 实践与验证

对比完整与权重快照的恢复等价性；敏感算子升精度前后误差；调节 scale 看溢出率。GPU 数字 [待验证]。

#### 10. 评分标准

I088 评分：不合格混淆精度；合格能选 FP16/BF16；良好能设计完整快照与白名单；优秀/专家能权衡快照体积与等价性、灰度回滚并量化溢出率。

### I089. 实现取舍：分布式

- 难度：中等级
- 知识域：K07 分布式
- 考察能力：接口、状态机、隔离、灰度、回滚与治理
- 题型：系统设计/工程落地
- 面试频率：高频
- 前置知识：rank、DDP、FSDP、collective、容错
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- 并行策略取舍：DDP 简单但每卡存完整模型；FSDP 分片参数/梯度/优化器显存友好但通信多；张量并行切分单层矩阵，适合超大层。
- 混合并行（数据 + 张量 + 流水）用于超大规模，复杂度高。
- 通信后端与拓扑（NVLink/InfiniBand）决定策略上限。
- 灰度上线并行策略需小规模验证扩展效率与正确性。

#### 2. 问题分析

为分布式设计接口与治理。两个不变量：(1) 各 rank 参数与状态同步；(2) 显存与通信在预算内。边界反例：模型能放进单卡显存就用 FSDP 反而因通信开销降低吞吐，属过度工程；策略选择要按显存缺口决定是否需要分片。

#### 3. 具体答案

面试中可以这样回答：

> 我按显存缺口选策略：单卡放得下就 DDP，最简单；单卡放不下用 FSDP 分片参数梯度和优化器状态；单层特别大才上张量并行。超大规模再叠流水并行。接口上把并行策略封装，训练代码不变，方便切换与灰度。上线先小规模验证扩展效率和正确性，再放量；若通信主导导致变慢，回退 DDP 或减少并行度。治理上记录每次策略变更与性能，支持回滚。

#### 4. 技术洞察

```
DDP: 全量副本, AllReduce 梯度
FSDP: 分片参数/梯度/优化器, all-gather + reduce-scatter
TP: 切单层矩阵, 通信频繁
选择: 单卡显存够 → DDP; 不够 → FSDP
```

#### 5. 图文说明

| 策略 | 显存 | 通信 | 场景 |
|---|---|---|---|
| DDP | 高 | 中 | 放得下 |
| FSDP | 低 | 高 | 放不下 |
| TP | 低 | 很高 | 单层大 |

#### 6. 拓展知识

FSDP 的 `FULL_SHARD`/`SHARD_GRAD_OP`/`HYBRID_SHARD` 在通信与显存间取舍；DCP 支持不同 world_size 的恢复；张量并行常与序列并行配合。

#### 7. 常见追问及回答要点

1. 何时用 FSDP？——单卡放不下。
2. 何时用 TP？——单层矩阵太大。
3. 如何灰度？——封装策略，小规模验证。

#### 8. 易错点与反例

1. 能用 DDP 却上 FSDP，通信拖慢。
2. 策略写死，无法回滚。
3. 未验证扩展效率就放量。

#### 9. 实践与验证

对比 DDP/FSDP 的显存、吞吐、扩展效率；小规模正确性验证；灰度开关测试回滚。GPU/NCCL 数字 [待验证]。

#### 10. 评分标准

I089 评分：不合格只知 DDP；合格能按显存选策略；良好能封装切换与灰度；优秀/专家能设计混合并行、验证扩展效率并支持回滚。

### I090. 实现取舍：编译栈

- 难度：中等级
- 知识域：K08 编译栈
- 考察能力：接口、状态机、隔离、灰度、回滚与治理
- 题型：系统设计/工程落地
- 面试频率：高频
- 前置知识：Dynamo、AOTAutograd、Inductor、guard、缓存
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- 编译取舍：eager 稳定但慢；`torch.compile` 提速但有编译开销与图断裂风险。
- mode 取舍：`default` 均衡，`reduce-overhead` 用 CUDA graphs（静态形状），`max-autotune` 首编慢但稳态快。
- 动态 shape 与静态 shape：动态泛化但可能重编译，静态特化但形状固定。
- 编译适合热点算子；全模型编译可能因 graph break 收益有限。

#### 2. 问题分析

为编译栈设计接口与治理。两个不变量：(1) 编译与 eager 数值一致；(2) 稳态收益为正，长尾可控。边界反例：全模型编译遇到大量 graph break，收益被切碎甚至为负，此时应只编译热点子模块或回退 eager。

#### 3. 具体答案

面试中可以这样回答：

> 我把 compile 当可选加速层，用开关控制，默认能回退 eager。热点模块用 `default` 或 `max-autotune`，launch-bound 且静态形状用 `reduce-overhead`。动态形状场景开动态 shape 避免重编译。上线前验证数值一致与稳态收益，并监控重编译与 graph break；若收益为负就回退。接口上把编译封装在模块边界，训练代码无感。灰度先小流量，异常回滚。

#### 4. 技术洞察

```
eager: 稳, 慢
compile default: 均衡
reduce-overhead: CUDA graphs, 静态
max-autotune: 首编慢, 稳态快
收益 = (eager - compiled)/eager, 需 > 0
```

#### 5. 图文说明

| mode | 首编 | 稳态 | 适用 |
|---|---|---|---|
| default | 中 | 快 | 通用 |
| reduce-overhead | 中 | 快 | launch-bound |
| max-autotune | 慢 | 最快 | 固定形状 |
| eager | 0 | 慢 | 回退 |

#### 6. 拓展知识

graph break 会限制融合；autotune 缓存可持久化；动态 shape 用符号形状泛化；`torch._dynamo.config` 可调缓存与错误处理。

#### 7. 常见追问及回答要点

1. 何时回退 eager？——收益为负或频繁重编译。
2. 全模型还是热点？——热点收益更确定。
3. mode 怎么选？——按形状与瓶颈。

#### 8. 易错点与反例

1. 全模型编译，graph break 多。
2. 动态形状未开，重编译爆炸。
3. 无开关，出问题难回退。

#### 9. 实践与验证

A/B 对比 eager/compile 数值与稳态；统计 graph break 与重编译；灰度开关测试回滚。GPU 数字 [待验证]。

#### 10. 评分标准

I090 评分：不合格无脑 compile；合格能按 mode 选；良好能处理动态形状与回退；优秀/专家能封装灰度、验证收益并处理 graph break 治理。

### I091. 实现取舍：性能

- 难度：中等级
- 知识域：K09 性能
- 考察能力：不变量、二阶影响、证据缺口与演进路线
- 题型：综合开放/专家追问
- 面试频率：高频
- 前置知识：关键路径、带宽、算力、通信、显存、统计
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- 性能优化的取舍：先补重叠、再降精度/融合、最后算法改动；每一步都可能牺牲通用性或精度。
- 延迟与吞吐、显存与算力、精度与速度都是相互制约的目标。
- 容量阈值决定 batch/并发上限；单位成本（每样本时间/内存）用于比较方案。
- 优化要有度量：基线、A/B、尾延迟。

#### 2. 问题分析

为性能优化设计取舍框架。两个不变量：(1) 优化前后同输入可比较；(2) 不引入正确性回归。边界反例：为提吞吐无限增大 batch，虽吞吐上升但显存超限或延迟超 SLA，优化不可持续；必须在约束下最优化。

#### 3. 具体答案

面试中可以这样回答：

> 我把性能优化当受约束优化：约束是显存上限和延迟 SLA，目标是吞吐或单位成本。顺序是先消除重叠缺失（几乎无损），再降精度和融合（有小代价），最后才动算法（代价最大）。每一步都用基线和 A/B 验证，并检查正确性没回归。容量上找 batch 的显存拐点，延迟上看尾延迟。若某优化在单位成本上更贵或触发 SLA，就回退。灰度放量，异常回滚。

#### 4. 技术洞察

```
maximize throughput s.t. mem ≤ cap, latency ≤ SLA
order: overlap → precision/fusion → algorithm
cost/elem = time or bytes per unit work
```

#### 5. 图文说明

| 优化 | 收益 | 代价 |
|---|---|---|
| 补重叠 | 大 | 近零 |
| 降精度/融合 | 中 | 精度/通用性 |
| 算法改动 | 视情况 | 大/风险 |
| 增 batch | 吞吐 | 显存/延迟 |

#### 6. 拓展知识

roofline 指导优化方向；尾延迟来自周期任务；显存碎片影响容量；优化要防止过拟合到特定形状。

#### 7. 常见追问及回答要点

1. 优化顺序？——重叠优先，算法最后。
2. 如何判容量？——batch 扫描找拐点。
3. 约束在哪？——显存与 SLA。

#### 8. 易错点与反例

1. 无基线就优化，无法证明。
2. 只提吞吐忽略 SLA。
3. 优化引入正确性回归。

#### 9. 实践与验证

基线与 A/B；扫描 batch 找拐点；测尾延迟；回归校验数值。GPU 数字 [待验证]。

#### 10. 评分标准

I091 评分：不合格只调参；合格能补重叠；良好能按代价排序并受约束优化；优秀/专家能建立单位成本模型、处理尾延迟/容量并灰度回滚。

### I092. 实现取舍：导出部署

- 难度：中等级
- 知识域：K10 导出部署
- 考察能力：不变量、二阶影响、证据缺口与演进路线
- 题型：综合开放/专家追问
- 面试频率：高频
- 前置知识：state_dict、Export、runtime、schema、迁移
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- 导出格式取舍：`torch.export` 强类型可序列化；ONNX 跨框架可移植；TorchScript 旧但成熟。
- 运行时取舍：ONNX Runtime 通用，TensorRT 快但与硬件/版本绑定。
- 版本与回滚：产物需版本化，回滚成套（权重+图+引擎）。
- 动态 shape 与性能：固定形状更快，动态更通用。

#### 2. 问题分析

为导出部署设计接口与治理。两个不变量：(1) 导出物与 eager 数值一致；(2) 产物可版本化并原子回滚。边界反例：为追求极致性能用绑定具体 GPU 的 TRT 引擎，换机器即失效；若部署环境异构，需保留通用备选路径。

#### 3. 具体答案

面试中可以这样回答：

> 格式我按可移植性和性能选：跨框架用 ONNX，强类型序列化用 `torch.export`，追求极致性能且环境固定用 TensorRT。运行时和产物成套版本化，回滚时整套切换。动态 shape 按需，固定形状优先因为更快。接口上把 runtime 封装，上层不感知具体引擎，便于灰度与切换。上线先小流量验证数值与延迟，异常回退到上一稳定产物或原生 PyTorch。

#### 4. 技术洞察

```
torch.export: 强类型, 可序列化
ONNX: 跨框架, opset
TensorRT: 快, 硬件/版本绑定
rollback: 成套原子切换
```

#### 5. 图文说明

| 格式 | 可移植 | 性能 | 绑定 |
|---|---|---|---|
| torch.export | 高 | 中 | 低 |
| ONNX | 高 | 中 | 低 |
| TensorRT | 低 | 高 | 高 |

#### 6. 拓展知识

TRT 引擎常需在目标机构建；量化校准影响精度；ONNX opset 决定算子覆盖；灰度用影子流量对比。

#### 7. 常见追问及回答要点

1. 选哪个格式？——按可移植与性能需求。
2. 回滚怎么做？——成套原子切换。
3. 动态 shape 取舍？——通用 vs 性能。

#### 8. 易错点与反例

1. 用 TRT 引擎跨机器，失效。
2. 回滚只换权重，错配。
3. 动态 shape 未设 profile，性能差。

#### 9. 实践与验证

多格式数值对比；目标 runtime 加载验证；灰度影子流量对比延迟与输出。运行时数字 [待验证]。

#### 10. 评分标准

I092 评分：不合格只选最快的；合格能按需求选格式；良好能版本化与原子回滚；优秀/专家能设计异构备选路径、灰度对比并验证数值与性能。

### I093. 实现取舍：自定义算子

- 难度：中等级
- 知识域：K11 自定义算子
- 考察能力：不变量、二阶影响、证据缺口与演进路线
- 题型：综合开放/专家追问
- 面试频率：高频
- 前置知识：schema、dispatcher、stream、autograd、ABI
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- 自定义算子取舍：优先用原生/组合，原生无对应或需要融合/算法优化时才自定义。
- 实现路径：`torch.library` 纯 Python + 外部 kernel、C++/CUDA extension、或编译后端融合。
- 维护成本：自定义算子需处理 schema、autograd、fake、ABI、跨版本。
- 融合收益与 occupancy 需权衡。

#### 2. 问题分析

为自定义算子设计取舍与治理。两个不变量：(1) 自定义实现与参考数值一致且反向正确；(2) 收益（时间/显存）为正且维护成本可接受。边界反例：为一个简单 elementwise 写 CUDA 扩展，代码量与维护成本远超收益，且原生可能已融合，属过度工程。

#### 3. 具体答案

面试中可以这样回答：

> 我先问“原生或编译能不能解决”，能就不自定义。确实需要时，我优先用 `torch.library` 封装的组合算子，只有性能关键才写 CUDA 扩展。自定义要配套 schema、autograd、fake 和 ABI 处理，维护成本高，所以收益必须明确：更少的 kernel 或算法级优化。上线前用 gradcheck 与参考对比，测融合收益与 occupancy。版本兼容上记录编译环境，跨版本重建。

#### 4. 技术洞察

```
优先: 原生组合 / Inductor 融合
其次: torch.library 封装
最后: CUDA extension
cost: schema+autograd+fake+ABI+版本
```

#### 5. 图文说明

| 方案 | 开发 | 性能 | 维护 |
|---|---|---|---|
| 原生组合 | 低 | 好 | 低 |
| torch.library | 中 | 好 | 中 |
| CUDA ext | 高 | 最好 | 高 |

#### 6. 拓展知识

Inductor 能自动融合多数 elementwise；flash-attention 类属算法级价值；CUDA 扩展需匹配 CUDA/torch 版本；功能化与 fake 是编译兼容前提。

#### 7. 常见追问及回答要点

1. 何时自定义？——原生无对应或关键性能。
2. 实现路径？——优先 torch.library。
3. 维护成本？——schema/autograd/fake/ABI。

#### 8. 易错点与反例

1. 简单算子也写 CUDA，过度工程。
2. 忘 fake，编译回退。
3. 跨版本复用扩展，加载失败。

#### 9. 实践与验证

gradcheck 与参考对比；A/B 融合收益与 occupancy；版本矩阵加载测试。GPU 结果 [待验证]。

#### 10. 评分标准

I093 评分：不合格偏爱手写 kernel；合格能先问原生；良好能权衡收益与维护并处理编译兼容；优秀/专家能设计版本化扩展、验证收益并灰度。

### I094. 实现取舍：测试治理

- 难度：中等级
- 知识域：K12 测试治理
- 考察能力：不变量、二阶影响、证据缺口与演进路线
- 题型：综合开放/专家追问
- 面试频率：高频
- 前置知识：oracle、证据、日志、回滚、门禁、复盘
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- 测试治理取舍：覆盖深度与反馈速度、用例数量与维护成本、严格门禁与开发效率。
- 分层门禁（冒烟/关键/全量）平衡速度与保护。
- oracle 强度决定测试价值；弱断言浪费运行时间。
- flaky 用 retry 会掩盖问题，应用根治替代。

#### 2. 问题分析

为测试治理设计取舍框架。两个不变量：(1) 关键路径有强 oracle 保护；(2) 反馈时间在可接受范围。边界反例：为求快把关键用例降级到冒烟层，缺陷逃逸成本远高于省下的 CI 时间，取舍失衡。

#### 3. 具体答案

面试中可以这样回答：

> 我的取舍原则是“关键路径强保护、非关键快反馈”。关键模块必须有数值或性质 oracle 并进阻断门禁；边缘用例可放夜跑或非阻断。避免用 retry 掩盖 flaky，先定位根因。用例数量不是目标，kill rate 和逃逸率才是；定期用变异测试审计，删掉无效用例。门禁分级并可配置，团队能按风险调深度。治理上记录逃逸事故并回补回归用例，逐步升级门禁。

#### 4. 技术洞察

```
gate = {smoke: fast, critical: strong, full: nightly}
value = bugs_caught / CI_minutes
flaky: 根治 > retry
audit: mutation kill rate
```

#### 5. 图文说明

| 维度 | 偏保守 | 偏激进 |
|---|---|---|
| 门禁 | 全量阻断 | 分层阻断 |
| oracle | 强 | 弱 |
| retry | 禁用 | 常用 |
| 用例数 | 精简 | 堆量 |

#### 6. 拓展知识

变异测试衡量有效性；逃逸复盘升级门禁；覆盖率不等于保护；CI 成本要纳入 ROI。

#### 7. 常见追问及回答要点

1. 快与保护如何平衡？——分层门禁。
2. flaky 怎么办？——根治而非 retry。
3. 如何证明有效？——kill rate 与逃逸率。

#### 8. 易错点与反例

1. 关键用例降级，逃逸。
2. retry 掩盖 flaky。
3. 以用例数论质量。

#### 9. 实践与验证

分层门禁耗时统计；变异测试 kill 率；逃逸事故回补回归。CI 数字 [待验证]。

#### 10. 评分标准

I094 评分：不合格只堆用例；合格能分层；良好能用 kill rate 与逃逸率权衡；优秀/专家能设计可配置门禁、治理 flaky 并建立逃逸到门禁升级的闭环。

### I095. 代码审查：张量/设备

- 难度：中等级
- 知识域：K01 张量/设备
- 考察能力：不变量、二阶影响、证据缺口与演进路线
- 题型：综合开放/专家追问
- 面试频率：高频
- 前置知识：shape、stride、dtype、device
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- 张量/设备代码审查的核心不变量：参与同一算子的张量 device/dtype/shape 兼容；视图不产生意外别名；隐式拷贝不在热路径。
- 常见缺陷：CPU/CUDA 混用、非连续输入喂给假设连续的算子、`.to` 散落导致隐式同步、in-place 修改共享 storage。
- 二阶影响：一处 `.to` 可能引入同步打断流水线；一处 `contiguous` 可能让显存翻倍。
- 演进：布局/设备约定应集中管理，便于后续换设备或布局而不改业务代码。

#### 2. 问题分析

审查代码时建立检查清单。两个不变量：(1) 计算前所有张量在预期设备/dtype/layout；(2) 无隐式拷贝或别名写入。边界反例：`.to(device)` 在同设备上看似无害，但若参数是 pinned 或需要连续，可能触发拷贝；审查需区分“空操作”与“隐式拷贝”。

#### 3. 具体答案

面试中可以这样回答：

> 我审查张量代码先看四点：设备、dtype、连续性、别名。设备上追每个张量的来源，确认在边界层统一 `.to`，而不是散落在热路径；dtype 检查是否有隐式提升或截断；连续性看是否有 transpose/切片后直接 `view`；别名看是否有 in-place 写视图影响基张量。我会指出 `.to` 在热路径可能引入同步、`contiguous` 可能翻倍显存这类二阶影响。建议把设备和布局约定集中，方便后续演进。

#### 4. 技术洞察

```
checklist:
  device: 所有输入一致? 边界层集中?
  dtype:  隐式提升? 截断?
  layout: 非连续 → view? contiguous 代价?
  alias:  in-place 写视图?
second-order: .to 同步, contiguous 显存
```

````python
# review: 非连续后直接 view
y = x.transpose(0, 1).view(-1)      # 审查应报错风险
# 建议
y = x.transpose(0, 1).reshape(-1)   # 或 .contiguous().view
````

#### 5. 图文说明

| 检查项 | 缺陷 | 二阶影响 |
|---|---|---|
| device | 混用 | 同步/报错 |
| dtype | 截断 | 精度 |
| layout | view 报错 | 拷贝显存 |
| alias | 脏写 | 难查 |

#### 6. 拓展知识

`channels_last` 切换要评估主算子收益；`pin_memory` 只对 H2D 有意义；Tensor 子类会改变审查面；编译对布局有假设。

#### 7. 常见追问及回答要点

1. 热路径 `.to` 有何问题？——隐式同步。
2. contiguous 的代价？——可能翻倍显存。
3. 如何防别名？——需要独立语义时 clone。

#### 8. 易错点与反例

1. 非连续输入 `view` 报错。
2. 设备散落 `.to` 拖慢。
3. in-place 写视图污染基张量。

#### 9. 实践与验证

加设备/dtype/连续性断言；构造 transpose+view 复现；测 `.to` 是否触发拷贝（`data_ptr` 比较）。无 GPU 依赖部分本地验证。

#### 10. 评分标准

I095 评分：不合格只看逻辑；合格能查 device/dtype；良好能覆盖连续性、别名与二阶影响；优秀/专家能提出集中约定与演进方案并给验证。

### I096. 代码审查：autograd

- 难度：中等级
- 知识域：K02 autograd
- 考察能力：不变量、二阶影响、证据缺口与演进路线
- 题型：综合开放/专家追问
- 面试频率：高频
- 前置知识：requires_grad、leaf、view、detach、反向
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- autograd 代码审查不变量：需要梯度的路径不被 `no_grad`/`detach`/numpy 切断；不 in-place 破坏待反向的张量；自定义 Function 反向正确。
- 常见缺陷：在 `no_grad` 内算 loss、用 `.item()` 参与计算、`retain_graph` 滥用、未 `save_for_backward`。
- 二阶影响：`detach` 可能让某些参数静默不更新；`retain_graph` 增加显存驻留。
- 演进：优先用组合算子，减少手写反向，便于维护与编译。

#### 2. 问题分析

审查自动微分代码。两个不变量：(1) 从 loss 到每个待训参数的图连通；(2) 反向数值正确。边界反例：`x.detach()` 后接 `requires_grad_(True)` 可重新建图，看似断开实则接上，审查要区分“真断链”与“重新接入”。

#### 3. 具体答案

面试中可以这样回答：

> 审查 autograd 我先确认 loss 到参数的图没被切断：找 `no_grad`、`detach`、`.item()`、`.numpy()`。然后看有没有对需反传张量做 in-place，这会让反向报版本号错误。自定义 Function 检查 `save_for_backward` 是否完整、反向返回数量与形状是否正确。`retain_graph` 若被滥用会显著增加显存，建议只在必要时用。我会建议尽量用组合算子替代手写反向，便于维护和编译，并加 gradcheck 兜底。

#### 4. 技术洞察

```
risk points:
  no_grad/detach/item/numpy → 断链
  in-place → version counter 报错
  autograd.Function → save/返回约束
  retain_graph → 显存驻留
evolution: 组合算子 > 手写反向
```

````python
# review: detach 后忘记接回
feat = model(x).detach()
loss = head(feat)          # 主干不更新，审查应指出
````

#### 5. 图文说明

| 检查项 | 缺陷 | 二阶影响 |
|---|---|---|
| no_grad | 断链 | 参数不更新 |
| in-place | 报错 | 反向失败 |
| Function | 反向错 | 梯度错 |
| retain_graph | 显存 | 驻留增加 |

#### 6. 拓展知识

`torch.func` 函数式 API 便于审查；功能化替代 in-place 以兼容编译；gradcheck 作为审查兜底；anomaly 模式辅助定位。

#### 7. 常见追问及回答要点

1. 如何发现静默断链？——追图连通性与参数 grad。
2. in-place 何时可接受？——不涉及需反传张量。
3. 手写反向有何风险？——公式/save/返回约束。

#### 8. 易错点与反例

1. detach 后主干不更新。
2. in-place 导致反向报错。
3. Function 返回梯度数量错。

#### 9. 实践与验证

对审查路径跑 gradcheck；断言关键参数 `.grad` 非 None；构造断链复现。无 GPU 依赖。

#### 10. 评分标准

I096 评分：不合格只看前向；合格能查断链；良好能覆盖 in-place、Function 与显存影响；优秀/专家能提出组合算子演进与 gradcheck 兜底并验证连通性。

### I097. 代码审查：Module/状态

- 难度：中等级
- 知识域：K03 Module/状态
- 考察能力：不变量、二阶影响、证据缺口与演进路线
- 题型：综合开放/专家追问
- 面试频率：高频
- 前置知识：参数、buffer、模式和 state_dict
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- Module 代码审查不变量：可训练量用 Parameter、持久状态用 buffer、子模块正确注册；`state_dict` 键稳定。
- 常见缺陷：用普通 list/dict 存层、状态用普通属性、随意改名破坏迁移、hook 未清理。
- 二阶影响：未注册的参数让优化器漏更新；非持久 buffer 丢失统计。
- 演进：状态契约稳定，便于 checkpoint 迁移与分布式封装。

#### 2. 问题分析

审查 Module 代码。两个不变量：(1) `named_parameters/named_buffers` 覆盖所有需要的状态；(2) `state_dict` 键稳定。边界反例：`self.cache = x` 存的是普通属性，训练时更新、保存时丢失，恢复后行为不一致；若它其实是统计量，应是 buffer。

#### 3. 具体答案

面试中可以这样回答：

> 审查 Module 我先看三张字典的归属：可训练张量是不是 Parameter，持久状态是不是 buffer，子层是不是 ModuleList/Dict。用普通 list 存层会导致参数不注册、优化器漏掉。`state_dict` 的键我会检查是否稳定，避免随意改名影响跨版本迁移。hook 用完要 `remove`。模式与冻结是否集中设置。我会建议明确状态契约，方便 checkpoint 与分布式封装演进。

#### 4. 技术洞察

```
checklist:
  Parameter vs 普通 Tensor
  buffer vs 普通属性
  ModuleList/Dict vs list/dict
  state_dict 键稳定
  hook 清理
```

````python
# review
self.layers = [Linear(), Linear()]     # 不注册
# 建议
self.layers = nn.ModuleList([...])
self.register_buffer("running", torch.zeros(d))
````

#### 5. 图文说明

| 检查项 | 缺陷 | 二阶影响 |
|---|---|---|
| list 存层 | 未注册 | 优化器漏更 |
| 普通属性 | 不保存 | 恢复丢失 |
| 改名 | 迁移断 | 加载错配 |
| hook | 未清理 | 泄漏 |

#### 6. 拓展知识

tied weights 需同一 Parameter 引用；`persistent=False` 适可重建缓存；DDP/FSDP 依赖注册稳定；hook 可在调试用但应清理。

#### 7. 常见追问及回答要点

1. 如何发现未注册参数？——named_parameters 覆盖检查。
2. 状态该用 buffer 吗？——持久且随迁移则用。
3. 键稳定性为何重要？——跨版本迁移。

#### 8. 易错点与反例

1. list 存层漏更新。
2. 统计量用普通属性，恢复丢失。
3. hook 未清理泄漏。

#### 9. 实践与验证

断言 named_parameters/buffers 覆盖预期；state_dict 键稳定性快照；检查 hook 清理。无 GPU 依赖。

#### 10. 评分标准

I097 评分：不合格只看 forward；合格能查注册；良好能覆盖 buffer、键稳定与 hook；优秀/专家能定义状态契约、支持迁移与分布式并验证覆盖。

### I098. 代码审查：数据管道

- 难度：中等级
- 知识域：K04 数据管道
- 考察能力：不变量、二阶影响、证据缺口与演进路线
- 题型：综合开放/专家追问
- 面试频率：高频
- 前置知识：Dataset、Sampler、collate、padding、版本
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- 数据管道代码审查不变量：一个 epoch 样本不重不漏；worker 随机可复现；collate 输出结构稳定。
- 常见缺陷：`IterableDataset` 多 worker 未分片、worker 种子未设、collate 未处理变长、pin_memory 与设备不匹配。
- 二阶影响：worker 种子不隔离导致增强相关；collate 结构变化影响下游。
- 演进：数据集接口与增强分离，便于换数据源与迁 GPU 增强。

#### 2. 问题分析

审查数据管道代码。两个不变量：(1) 索引/样本覆盖完整且可复现；(2) collate 输出契约稳定。边界反例：`num_workers=0` 时用全局随机种子工作正常，切到多 worker 后每个 worker 继承同一状态，增强序列重复，审查需检查 `worker_init_fn`。

#### 3. 具体答案

面试中可以这样回答：

> 审查数据管道我先看覆盖：一个 epoch 是否不重不漏，`IterableDataset` 多 worker 是否按 worker/rank 分片。再看随机性：`worker_init_fn` 是否给每个 worker 派生不同种子，否则增强会重复。collate 我检查是否处理变长和 pad/ignore，输出结构是否稳定。`pin_memory` 是否与设备匹配。我会建议把数据接口与增强解耦，方便后续换数据源或把增强迁到 GPU。

#### 4. 技术洞察

```
checklist:
  覆盖: epoch 不重不漏; Iterable 分片
  复现: worker_init_fn 派生种子
  collate: 变长/pad/ignore
  pin: 与设备匹配
evolution: 数据接口 ⟂ 增强
```

````python
# review
def worker_init_fn(wid):
    np.random.seed(base + wid)      # 必须
loader = DataLoader(ds, num_workers=4, worker_init_fn=worker_init_fn)
````

#### 5. 图文说明

| 检查项 | 缺陷 | 二阶影响 |
|---|---|---|
| 覆盖 | 重复/缺失 | 分布偏移 |
| 种子 | 未隔离 | 增强相关 |
| collate | pad 未 ignore | loss 污染 |
| pin | 不匹配 | 无意义开销 |

#### 6. 拓展知识

`StatefulDataLoader` 支持恢复；分桶提升 padding 效率；GPU 增强（DALI）卸载 CPU；`persistent_workers` 与状态交互。

#### 7. 常见追问及回答要点

1. 如何验证不重不漏？——索引集合比对。
2. worker 种子为什么重要？——避免增强相关。
3. collate 易错点？——pad/ignore 与结构稳定。

#### 8. 易错点与反例

1. Iterable 多 worker 重复。
2. 未设 worker 种子，增强重复。
3. pad 位参与 loss。

#### 9. 实践与验证

索引覆盖单测；worker 种子复现测试；collate 结构断言。多进程数字 [待验证]。

#### 10. 评分标准

I098 评分：不合格只看能取数；合格能查覆盖；良好能覆盖种子、collate 与 pin；优秀/专家能解耦数据与增强、支持恢复与 GPU 增强并给验证。

### I099. 代码审查：训练/优化器

- 难度：中等级
- 知识域：K05 训练/优化器
- 考察能力：不变量、二阶影响、证据缺口与演进路线
- 题型：综合开放/专家追问
- 面试频率：高频
- 前置知识：forward、backward、累积、optimizer、scheduler
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- 训练/优化器代码审查不变量：`step` 前梯度完整且已清零上一轮；scheduler 频率正确；累积缩放正确；clip 在 unscale 后。
- 常见缺陷：忘 `zero_grad`、scheduler 每 batch 调、累积 loss 未缩放、clip 放错位置、优化器只包部分参数。
- 二阶影响：忘清零让梯度膨胀；频率错让 lr 曲线偏移；只包部分参数让某些层冻结。
- 演进：训练步封装成单一函数，便于审查、测试与复用。

#### 2. 问题分析

审查训练/优化器代码。两个不变量：(1) 每次 `step` 消费的梯度是本轮干净的；(2) 优化器覆盖全部待训参数且 scheduler 语义一致。边界反例：梯度累积时在 micro-batch 内 `optimizer.step()`，等效 batch 被破坏，审查需确认 `step` 只在外层窗口末调用。

#### 3. 具体答案

面试中可以这样回答：

> 审查训练循环我先看梯度生命周期：是否在每轮前 `zero_grad`，`step` 是否只在完整梯度后调用。梯度累积时确认层级：inner 只 backward，outer 才 step，且 loss 按累积步数缩放。scheduler 频率要与设计一致，逐 batch 调会加速衰减。优化器是否覆盖所有待训参数，别漏掉某些子模块。裁剪位置要在 unscale 之后。我会建议把整个 step 封装成一个函数，便于审查、单测和复用。

#### 4. 技术洞察

```
checklist:
  zero_grad 每轮?
  step 在完整梯度后?
  accum: inner backward / outer step, loss/K
  scheduler 频率?
  optimizer 覆盖全部参数?
  clip 在 unscale 后?
```

````python
# review: 累积内错调 step
for i, (x, y) in enumerate(loader):
    loss = criterion(model(x), y) / K
    loss.backward()
    if (i + 1) % K == 0:
        optimizer.step(); optimizer.zero_grad(set_to_none=True)
````

#### 5. 图文说明

| 检查项 | 缺陷 | 二阶影响 |
|---|---|---|
| zero_grad | 忘调用 | 梯度膨胀 |
| step 时机 | 累积内 step | batch 变小 |
| scheduler | 频率错 | lr 偏移 |
| 参数覆盖 | 漏层 | 冻结 |
| clip | 位置错 | 失效 |

#### 6. 拓展知识

`set_to_none=True` 省内存并利于 unused 检测；AMP 下 step 换成 scaler；`foreach` 优化器加速；DDP 累积配 `no_sync`。

#### 7. 常见追问及回答要点

1. 如何确认清零？——逐轮查 zero_grad。
2. 累积如何等效？——loss/K，外层 step。
3. clip 放哪？——unscale 后。

#### 8. 易错点与反例

1. 忘 zero_grad，梯度累积。
2. 累积内 step，等效 batch 错。
3. 优化器漏参数，部分层冻结。

#### 9. 实践与验证

记录每步 grad norm 与 lr 曲线；断言参数变化；累积等效 allclose。无 GPU 依赖部分本地验证。

#### 10. 评分标准

I099 评分：不合格只看 forward/loss；合格能查清零与顺序；良好能覆盖累积、scheduler、参数覆盖与 clip；优秀/专家能封装 step、加曲线观测并验证等价性。

### I100. 代码审查：AMP/恢复

- 难度：中等级
- 知识域：K06 AMP/恢复
- 考察能力：不变量、二阶影响、证据缺口与演进路线
- 题型：综合开放/专家追问
- 面试频率：高频
- 前置知识：autocast、scaler、快照、随机状态
- 关联题目：同知识域前序题、后续难度题和 K12 交叉题
- 事实状态：[已确认][待验证]

#### 1. 背景知识

- AMP/恢复代码审查不变量：autocast 与 scaler 设备一致；敏感算子 fp32；unscale 在 clip 前；checkpoint 含 scaler 与 RNG。
- 常见缺陷：autocast 内外混算、clip 在 scale 前、漏存 scaler/RNG、恢复顺序错。
- 二阶影响：漏 scaler 让恢复后溢出行为变；clip 位置错让阈值失效。
- 演进：把 AMP 与 checkpoint 封装，配置化，便于切精度与恢复策略。

#### 2. 问题分析

审查 AMP/恢复代码。两个不变量：(1) 缩放/还原/裁剪/更新顺序正确；(2) checkpoint 覆盖恢复所需全部状态。边界反例：`clip_grad_norm_` 若在 `scaler.unscale_` 之前调用，梯度仍是放大后的，阈值实际被放大，裁剪失效，审查需确认调用次序。

#### 3. 具体答案

面试中可以这样回答：

> 审查 AMP 我先看顺序：forward/loss 在 `autocast` 内，`scaler.scale(loss).backward()`，然后 `unscale_`，clip 必须在 unscale 之后，再 `scaler.step`、`scaler.update`。敏感算子是否强制 fp32。恢复部分检查 checkpoint 是否含 scaler、RNG、优化器、scheduler 和进度，且加载顺序正确，避免先训练污染状态。我会建议把 AMP 与 checkpoint 封装、配置化，便于切换精度与调整恢复策略，并用序列对比验证恢复等价。

#### 4. 技术洞察

```
checklist:
  autocast/scaler device 一致?
  敏感算子 fp32?
  unscale → clip → step → update 顺序?
  ckpt = model+optim+sched+scaler+rng+step?
  恢复顺序先于训练?
```

````python
scaler.scale(loss).backward()
scaler.unscale_(optimizer)
torch.nn.utils.clip_grad_norm_(model.parameters(), 1.0)   # 必须在 unscale 后
scaler.step(optimizer); scaler.update()
````

#### 5. 图文说明

| 检查项 | 缺陷 | 二阶影响 |
|---|---|---|
| device 一致 | 缩放无效 | 数值错 |
| 敏感算子 | 降精度 | nan |
| 顺序 | clip 在前 | 阈值失效 |
| ckpt 完整 | 漏 scaler | 恢复不等价 |
| 加载顺序 | 先训练 | 状态污染 |

#### 6. 拓展知识

`GradScaler` 与 bf16 取舍；`weights_only` 与 scaler 序列化；溢出率监控；恢复严格等价需确定性。

#### 7. 常见追问及回答要点

1. clip 为何在 unscale 后？——阈值针对真实梯度。
2. ckpt 最少存什么？——恢复等价所需全部。
3. 加载顺序为何重要？——避免污染 scale/RNG。

#### 8. 易错点与反例

1. clip 在 scale 前，失效。
2. 漏存 scaler，恢复不等价。
3. 先训练后恢复，状态污染。

#### 9. 实践与验证

断言调用次序；恢复后序列对比；scale 一致性断言。GPU 数字 [待验证]。

#### 10. 评分标准

I100 评分：不合格只看能跑；合格能查顺序；良好能覆盖敏感算子、ckpt 完整与加载顺序；优秀/专家能封装配置化 AMP/恢复、用序列对比验证并监控溢出。

