# K11：性能、容量、碎片、成本与降级

## 边界

本知识域讨论如何为异构资源系统建模吞吐、尾延迟、队列、内存水位、碎片、通信和成本，并据此设计可验证的降级与容量规划。

讨论对象是“容量与成本决策”，不是分配器内部的实现细节（那是 K04）、也不是可观测性本身（那是 K10）。分层可用量模型、碎片成因、池/缓存阈值、成本-复用取舍和降级阶梯，是本域的事实边界。

特定厂商的 HBM 容量、阈值取值和实测收益必须绑定目标版本。文档确认了各池/缓存的策略与阈值来源，但没有设备性能数据，因此吞吐、带宽和成本收益的结论都按 `[待验证]` 处理，不能凭空承诺百分比。

## 核心机制

### 1. 分层可用量模型

“显存还有空闲却分配失败”要先按路径分层，而不是用全局空闲字节数下结论：

```text
available(request) = free_on_path(request)
  where path = pool_match(size, type, policy) ? pool : ordinary
  and free_on_path depends on fragmentation, attributes, cache/shrink
```

`[已确认]` 超过 2 MiB 或不满足 KernelMemoryPool 条件的请求会走普通 Runtime→Driver/HAL 路径；不能用固定池的 `usedSize_` 推断全局 HBM 可用量。[cann/90-cross-module/pooling-and-resource-management.md:35-43]

### 2. 碎片：为什么总量够却失败

`[已确认]` Runtime KernelMemoryPool 在固定 2 MiB HBM backing 上做 first-fit 分割，源码未显示释放时对相邻块执行合并，因此碎片会随请求顺序累积。[cann/90-cross-module/pooling-and-resource-management.md:35-43]

```text
[ used ][ free ][ used ][ free ]   total_free 足够
request 大于任一 free 块  ->  分配失败（外部碎片）
```

`[已确认]` SOMA 的 FREE Segment 用按 `(size, basePtr)` 升序集合的 `lower_bound` 选择最小满足块（Best-Fit，同尺寸按低地址）；当前 `TryToReuse` 直接返回 nullptr，cached 段候选重用尚未接入统一入口。[cann/90-cross-module/pooling-and-resource-management.md:51-65]

不同策略有不同的碎片与成本曲线：first-fit 不合并强调复用效率但长期小块请求碎片化；Best-Fit 减少浪费但可能留下小块残留。

### 3. 阈值、扩展粒度与 shrink

`[已确认]` Driver V3 cache allocator 由页大小推导 alloc threshold、shrink threshold、2 MiB expand granularity 和 page alignment，并可 shrink 回收 backing。[cann/90-cross-module/pooling-and-resource-management.md:67-71]

`[已确认]` 当 cache 地址空间暂时没有合适 area 时，扩展大小按对齐公式计算，再扩展 normal backing 后重试：

```c
expand_size = svm_align_up(svm_align_up(size, ca->strategy.expand_granularity),
                           ca->strategy.alloc_gran);
```

随后 `cache_expand_once -> cache_expand -> cache_malloc_raw -> svm_normal_malloc -> svm_ga_add_range` 再重新分配。[cann/01-modules/M04-driver/driver-memory-pool-analysis.md:1162-1179]

因此“`OUT_OF_MEMORY`”在 V3 cache 首次出现时**不一定表示物理内存耗尽**，可能只是“已有 cache range 没有适配 area，需要扩展”。

### 4. 队列与延迟：Little 定律

`[推断]` 稳态下队列深度、到达率和等待时间满足：

```text
L = λ · W        (队列深度 = 到达率 × 平均停留时间)
```

这条关系把“排队/限流”与“尾延迟”联系起来：当资源水位接近上限，`W` 非线性上升，P99 先恶化。所以容量不能只用平均吞吐描述，必须同时看队列深度和尾延迟。

### 5. 成本-复用取舍

```text
higher reuse         -> lower capacity cost, higher fragmentation risk
aggressive shrink    -> lower resident cost, higher realloc churn
larger pool backing  -> fewer expansions, higher resident footprint
```

这是一个多目标权衡，不存在“所有指标同时最优”。工程问题变成：在给定 SLO（尾延迟、OOM 率）和成本约束下，调池大小、对齐、批处理和 shrink 阈值。

### 6. 降级阶梯

当资源紧张时，按优先级逐级降级，而不是直接失败或无限扩容：

```text
L0 正常          : 全精度、全并发
L1 批处理/合并    : 减少请求数与分配次数
L2 排队/限流      : 背压，保护关键路径
L3 降精度/迁移    : 非关键任务降级或迁到其他设备/节点
L4 明确拒绝       : 返回结构化原因，不静默失败
```

`[推断]` 每一级都要记录降级原因，并用精度/质量预算和结果 oracle 验证，而不是“看起来还能跑”。

### 7. 测量必须分层

`[已确认]` 性能指南要求分离编译、模型加载、首轮、稳态、同步、传输和卸载时间；记录 shape/batch、Stream/Event 数、设备型号、频率、构建选项和 DFX 开销；每次优化比较正确性、峰值内存、尾延迟和错误恢复。[cann/99-roadmap/performance-guide.md:14-21]

`[已确认]` 性能关键路径指出主机逐任务下发、Host-device 拷贝、未对齐 buffer、KernelMemoryPool first-fit 碎片、SOMA 往返、Stream/Event 竞争和 DFX 开销都可能成为瓶颈，但都需压力测试确认。[cann/90-cross-module/performance-critical-paths.md:25-33]

所以容量结论必须绑定“哪一段、什么配置、什么负载”，不能跨层外推。

### 8. 失败模式

- 用总空闲判定可分配：忽略碎片和属性。
- 跨层外推：用固定池 `usedSize_` 代表全局 HBM。
- 认为 shrink 一定省成本：带来重分配抖动和尾延迟上升。
- 只增加资源不定位瓶颈：掩盖调度、同步或通信瓶颈。
- 承诺无依据的百分比收益：缺少可复现实验和目标版本。

### 9. 容量模型与利用率

`[推断]` 容量不是一个标量，而是在 SLO 约束下的可行工作点：

```text
capacity(SLO) = max work_rate
  s.t. success_rate ≥ SLO.success
       p99_latency ≤ SLO.latency
       watermark ≤ SLO.watermark
```

利用率过高会先推高排队时间（Little 定律），再触发 OOM 或限流。因此规划时应保留 headroom，而不是把平均利用率推向 100%：

```text
target_utilization < 1 - headroom，headroom 用于突发、碎片和降级
```

### 10. 水位与过载保护

`[推断]` 过载保护按分层水位触发，与 K04 的池/路径对应：

```text
水位: pool_watermark | path_watermark | queue_depth | pending_tasks
策略: admit | queue | backpressure | shed | reject
```

`[已确认]` 请求是否进入 cache 取决于 size、align、flag、NUMA、设备能力和当前状态，所以水位也必须按路径分别统计，不能用单一全局值。[cann/01-modules/M04-driver/driver-memory-pool-analysis.md:1128-1156]

### 11. 成本模型

`[推断]` 成本至少包含三部分，且都随策略变化：

```text
cost = resident_cost(占用 backing) + churn_cost(重分配/扩展) + tail_cost(尾延迟惩罚)
```

激进 shrink 降低 resident，却抬高 churn 和尾延迟；大 backing 减少扩展，却抬高 resident。成本治理的目标是在 SLO 约束下最小化总成本，而不是单独压低某一项。

### 12. 碎片度量与验证 oracle

`[推断]` 用可计算的碎片率辅助归因，避免只能定性描述：

```text
fragmentation = 1 - (largest_free_block / total_free)
fails_attributable = 失败能归因到 碎片/属性/路径/shrink 之一
```

`[待验证]` 实验设计：

| 输入 | 步骤 | 期望 | oracle |
|---|---|---|---|
| 固定池(first-fit 不合并) | 交替小块后请求大块 | 大请求失败 | 失败可归因到碎片 |
| Best-Fit 池 | 混合大小请求 | 残留更小但有碎片 | 分配-释放配对闭合 |
| 可调 shrink 阈值 | 提高阈值 | resident 回收但重分配增多 | 尾延迟不劣化到违反 SLO |

失败判据：若“总空闲够”时仍失败却无法归因，或释放后计数不闭合，测试失败。真实 HBM 碎片、阈值效果与降级收益需设备验证。

## 常见失效条件

- 凭空承诺百分比收益，或只增加资源而不定位瓶颈。
- 把 `OUT_OF_MEMORY` 一律当成物理内存耗尽，忽略 cache range 扩展路径。
- 用平均吞吐评估容量，忽略队列深度和尾延迟。
- 忽略属性/路径：页类型、节点、module 不匹配导致目标 backing 不可用。
- 降级无预算、无 oracle：降级后结果不可验证，静默错误。
- 只测一次大拷贝或单点负载，外推稳态容量。

## 关键不变量

```text
I1  分配-释放配对闭合；释放后计数归零
I2  available 必须按 path 计算，不能用全局空闲字节数代表
I3  "总空闲够但失败" 必须可归因到 碎片/属性/路径/shrink 之一
I4  shrink 不得以违反 SLO（尾延迟、OOM 率）为代价
I5  降级必须带原因、精度/质量预算和结果 oracle
I6  每次优化同时比较正确性、峰值内存、尾延迟与恢复
```

## 与其他知识域的连接

- **K04 内存与地址空间**：池/缓存策略、对齐和 backing 是容量模型的分层对象。
- **K06 调度与隔离**：优先级、配额和背压决定谁先被降级或限流。
- **K07 / K08 传输与通信**：带宽、拥塞和尾延迟是容量模型的重要组成。
- **K10 可观测性**：SLO/SLI、水位和队列指标是容量决策的输入与验证。
- **K12 生产演进**：容量规划要跨版本、跨节点，并支持迁移与回滚。
- **K02 设备能力**：dtype、内存容量和拓扑决定可用容量与降级空间。

## 题目索引

本域对应题目（按 `知识域：` 字段确认）：`HRS-B012`、`HRS-I011`、`HRS-A011`、`HRS-A014`、`HRS-E011`。

- `HRS-B012` 概念题：显存有空闲却分配失败，碎片、容量、成本与降级如何权衡。
- `HRS-I011` 分析题：容量、碎片、成本与降级的稳定性权衡。
- `HRS-A011` 设计题：建立性能容量与成本模型并做降级决策。
- `HRS-A014` 设计题：资源水位与过载保护机制。
- `HRS-E011` 高级题：容量、成本与可靠性之间的长期权衡治理。

## 证据入口

- `[已确认]` `cann/90-cross-module/pooling-and-resource-management.md:35-43`：KernelMemoryPool first-fit 不合并与“不能推断全局 HBM”。
- `[已确认]` `cann/90-cross-module/pooling-and-resource-management.md:51-65`：SOMA FREE Segment 的 Best-Fit 与 `TryToReuse` 现状。
- `[已确认]` `cann/90-cross-module/pooling-and-resource-management.md:67-71`：Driver V3 阈值来源与 shrink。
- `[已确认]` `cann/01-modules/M04-driver/driver-memory-pool-analysis.md:1162-1179`：cache miss 的扩展公式与扩展链路。
- `[已确认]` `cann/90-cross-module/performance-critical-paths.md:25-33`：潜在瓶颈清单（需压力测试确认）。
- `[已确认]` `cann/99-roadmap/performance-guide.md:14-21`：分层测量方法与“同时比较正确性/峰值内存/尾延迟”。
- `[推断]`：分层可用量模型、Little 定律与降级阶梯由上述策略推导；`[待验证]`：真实碎片率、阈值效果与降级收益。

## 学习提示

先读 `HRS-B012` 建立“碎片/路径/属性/shrink”四类归因，再用 `HRS-I011` 学分层可用量与成本-复用取舍。重点掌握 `HRS-A011`：如何把吞吐、尾延迟、水位和成本写成一个可验证的模型。最后用 `HRS-A014`/`HRS-E011` 检验过载保护与长期治理：降级和扩容都必须绑定 SLO 与 oracle，任何收益声明都要能复现，而不是凭经验给一个百分比。
