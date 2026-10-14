# K10：日志、指标、Tracing、dump、SLO 与诊断证据链

## 边界

本知识域讨论如何采集、关联和使用证据来定位异构资源故障，以及如何把稳定性目标落成可观测的 SLO/SLI 与告警阈值。

讨论对象是“证据的生产、关联与消费”，不是错误分类本身（那是 K09）、也不是容量模型（那是 K11）。日志、指标、Tracing、dump 四类证据、统一诊断记录字段、跨层时间线和 SLO/错误预算是本域的事实边界。

特定厂商的 profiling/dump 工具、采样率和开销必须绑定目标版本。文档确认了应记录的字段和排查方向，但没有实测观测开销，因此性能结论按 `[待验证]` 处理。

## 核心机制

### 1. 四类证据的分工

`[推断]` 把可观测性拆成四类互补证据，各自回答不同问题：

| 支柱 | 回答的问题 | 关键字段 | 已知代价 |
|---|---|---|---|
| 日志 | 发生了什么 | 原始错误、阶段、Context/Stream | 量大、影响延迟 |
| 指标 | 何时异常、影响面多大 | SLO、成功率、队列、水位 | 采样与聚合 |
| Tracing | 卡在哪一跳 | 跨层耗时、trace_id | 采样率 |
| dump | 当时持有什么 | 句柄、引用、池状态 | 很重、按需触发 |

它们不是互相替代：日志是离散事件，Tracing 提供跨层因果与耗时，指标给出时间序列和水位，dump 给出某一刻的资源快照。

### 2. 统一诊断记录

`[已确认]` 跨层建议统一记录：时间戳、线程、设备、Context、Stream、Graph/Model、pool、VA、size、API 阶段、原始错误码、映射后错误码和资源句柄。[cann/90-cross-module/error-boundaries.md:47-49]

把它写成一个诊断记录结构：

```text
record = {
  ts, thread, device, context, stream, graph_or_model,
  pool, va, size, api_stage,
  raw_error, mapped_error, resource_handle,
  trace_id
}
```

**关联键**是这套记录的核心价值：同一请求必须在四类证据里共享设备、Context、Stream、Graph/Model、请求 ID 和时间戳。缺关联键，指标、Trace 和 dump 无法对齐到同一次调用。

### 3. 从上到下的排查方向

`[已确认]` 调试指南要求先固定版本、SoC、Toolkit、Driver/Firmware 和构建选项，记录 API、阶段、线程、设备、Context、Stream、Graph/Model 和原始错误码，找到第一个失败边界，再沿 `ACL → Runtime → HAL → SDK/kernel` 或 `GE → Runtime` 反查。[cann/99-roadmap/debugging-guide.md:14-21]

`[已确认]` 每层记录 API 名称、设备 ID、模型/Graph ID、Context/Stream、底层错误码、线程和时间戳，从最上层失败点反查最后一个成功边界。[cann/90-cross-module/cross-module-call-chains.md:54-56]

规范化成流程：

```text
SLO 异常(指标)
  -> 影响面(日志分组)
  -> 定位跳数(Tracing)
  -> 持有快照(dump)
  -> 反查最后成功边界
  -> 最小复现 -> 回归
```

### 4. SLO / SLI / 错误预算的公式

`[推断]` 把服务的稳定性目标落成可计算指标：

```text
SLI           = good_events / total_events
error_budget  = 1 - SLO
burn_rate     = observed_error_rate / (1 - SLO)
time_to_exhaust = remaining_budget / burn_rate
```

异构资源上的 SLI 不能只看平均延迟，至少区分：

```text
success_rate
latency{ p50, p95, p99 }
queue_depth / pending_tasks
memory_watermark（分层：池/路径/属性）
device_error_rate
recovery_time
```

### 5. 错误预算与告警阈值

`[推断]` 告警不应基于瞬时单点，而应基于错误预算消耗速率。多窗口燃烧率的原则：

```text
fast window: 1h  burn_rate 高  -> 立即告警（快速发现大面积故障）
slow window: 6h  burn_rate 中  -> 工单告警（慢速持续劣化）
```

这样把“什么时候该叫人”从主观阈值变成预算约束。具体窗口长度和倍率属于组织策略，不是文档事实。

### 6. DFX 旁路不变量

`[已确认]` 日志和 DFX 能力属于旁路诊断设施，不改变资源所有权；不能以 `ACL_SUCCESS` 或 `GRAPH_SUCCESS` 推断设备任务已完成。[cann/00-overview/global-error-model.md:39-43]

```text
enabling_logging / enabling_dump
  ⇒ 不得改变 resource ownership 或完成语义
```

因此观测设施不能为了采集而加锁、延迟释放或提前复用资源；dump 应按需、可采样、可关闭。

### 7. 证据链是可验证的，不是“看日志”

把证据链做成可判定的对象：

```python
def diagnose(slo_alert):
    window = group_by_correlation_key(slo_alert.records)
    for req in window.requests:
        assert complete_fields(req, REQUIRED)      # 字段完整率
        assert aligned(req.logs, req.traces, req.dump)  # 关联一致性
    first_failure = min_by_layer(window.requests)  # 第一失败边界
    return replay(first_failure)                    # 最小复现
```

`replay` 是否成功、字段是否完整，都是可测的 oracle；这比“把日志都打出来”更强。

### 8. 失败模式

- 只留顶层错误码：丢失第一个失败边界。
- 四类证据无关联键：无法把指标、Trace 和 dump 对齐到同一请求。
- 观测改变行为：为采集加锁或改生命周期，引入观测副作用。
- 只看平均延迟：尾延迟和队列深度被掩盖。
- dump 无节制：开销过重，反而影响稳定性。

### 9. 指标基数、直方图与尾延迟

`[推断]` 尾延迟依赖直方图而不是平均值。分位数不能跨设备、跨版本、跨 pool 无脑聚合，否则会掩盖某个具体维度上的劣化：

```text
latency_hist{device, version, pool, api_stage}   # 保留可归因维度
p99 = quantile(hist, 0.99)                        # 由直方图近似，非均值外推
```

维度选择是基数与可归因性的取舍：维度太少无法定位，维度太全则基数爆炸、采集开销上升。应优先保留能对应到 K09 错误类别和 K11 资源路径的维度。

### 10. 观测自身作为受控对象

`[已确认]` 日志与 DFX 是旁路设施，不改变资源所有权。[cann/00-overview/global-error-model.md:39-43]

`[推断]` 由此推出一个工程原则：观测设施本身要有开关、采样率和开销预算，并作为发布门禁的一部分被验证。dump 常开、日志全量、Tracing 100% 采样都可能把“诊断能力”变成“稳定性风险”。

```text
observation_cost ≤ observation_budget
dump on-demand ∨ sampled；tracing sampled；logging leveled
```

### 11. 异构资源 SLO 的落点

`[推断]` 把 SLO 落到分层资源上时，要避免“一个百分比概括所有维度”：

```text
SLO 要绑定: { 设备型号/版本, pool/路径, 请求类别, 故障域 }
SLI 要区分: 成功/明确失败/UNKNOWN（与 K09 三态一致）
告警要区分: 大面积快速故障(fast burn) vs 局部持续劣化(slow burn)
```

`[已确认]` 文档要求诊断优先保存设备 ID、Context/Stream、模型/Graph 和调用阶段，这些正是让 SLI 可归因的最小组件。[cann/00-overview/global-error-model.md:39-43]

## 常见失效条件

- 只采集平均延迟或只记录顶层错误，无法重建跨层时间线。
- 指标、日志、Trace 各用不同 ID，无法关联同一请求。
- 为观测而修改资源所有权或完成语义（违反旁路不变量）。
- 告警只看瞬时阈值，忽略错误预算燃烧率，误报或漏报。
- dump 全量常开，观测开销本身成为稳定性风险。

## 关键不变量

```text
I1  同一请求在日志/指标/Trace/dump 中共享关联键
I2  诊断记录包含 raw_error 与 mapped_error，且带 api_stage
I3  开启日志/dump 不改变 resource ownership 与完成语义
I4  SLO 至少区分成功率与尾延迟，不单看均值
I5  告警阈值可由错误预算燃烧率推导，而非主观单点
I6  从最上层失败点可反查到最后一个成功边界
```

## 与其他知识域的连接

- **K09 错误与恢复**：K09 决定“保留哪些错误上下文”，K10 决定“如何采集与关联”；两域共享三态判定所需的证据。
- **K11 性能与容量**：SLO 的尾延迟、水位和队列深度是容量模型的输出与输入。
- **K12 生产演进**：发布/迁移/灾备需要可回滚判据和观测门禁，K10 提供证据。
- **K05 异步完成与可见性**：完成证据是否可信，取决于能否区分提交成功与设备完成。
- **K03 生命周期**：dump 里“谁还引用资源”是生命周期不变量是否成立的关键证据。

## 题目索引

本域对应题目（按 `知识域：` 字段确认）：`HRS-B011`、`HRS-I010`、`HRS-I015`、`HRS-A010`、`HRS-E010`。

- `HRS-B011` 概念题：用日志、指标、Tracing、dump 串起可复现证据链。
- `HRS-I010` 分析题：搭建可追溯的诊断证据链与可观测性体系。
- `HRS-I015` 分析题：把 SLO、错误预算与告警阈值落到异构资源上。
- `HRS-A010` 设计题：可观测性体系如何支撑根因定位。
- `HRS-E010` 高级题：全链路、可作证的证据体系设计。

## 证据入口

- `[已确认]` `cann/90-cross-module/error-boundaries.md:47-49`：建议跨层统一记录的时间戳、设备、Context/Stream、阶段与错误码字段。
- `[已确认]` `cann/00-overview/global-error-model.md:39-43`：诊断原则与“DFX 为旁路，不改变资源所有权”。
- `[已确认]` `cann/99-roadmap/debugging-guide.md:14-27`：固定环境、找到第一失败边界、沿层反查与断点建议。
- `[已确认]` `cann/90-cross-module/cross-module-call-chains.md:54-56`：每层记录 API/设备/模型/Context/Stream/错误/线程/时间戳。
- `[已确认]` `cann/90-cross-module/runtime-trace.md` 与 `cann/99-roadmap/performance-guide.md:14-21`：分离编译/加载/首轮/稳态/同步/传输/卸载时间，用 profiling/trace 区分各层。
- `[推断]`：四支柱分工、SLO/错误预算公式与告警策略由上述诊断原则推导；`[待验证]`：真实系统的观测开销、dump 内容与采集完整性。

## 学习提示

先读 `HRS-B011` 建立四支柱与统一关联键的概念，再用 `HRS-I010` 设计采集与关联链路。重点学 `HRS-I015`：如何把 SLO、错误预算和告警阈值落到分层资源水位上。最后用 `HRS-A010`/`HRS-E010` 检验：当把观测加厚时，如何用旁路不变量保证它不改变资源所有权和完成语义，并把证据链做成可复现、可回归、可作证的对象。
