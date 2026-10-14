# 推理方向：高级面试题

- 题数：18；稳定 ID：`I-A001`–`I-A018`
- 证据规则：C/I；压测/生产环境 U
- 侧重：架构取舍、定量论证、权衡分析；无唯一正确答案

---

### I-A001 设计推理服务的配置一致性方案：多进程/多副本间如何保证配置相同且可灰度？

**元数据：** 知识域：`I01 配置与一致性`；能力目标：`C01 分布式配置一致性`；题型：架构设计与权衡；频率：高频；前置/后续：`I-A002`（协议演进）、`I-A016`（灰度发布与回滚）；证据状态：设计原理 C，运行结论 U。

**背景知识：** 推理服务通常以多副本部署（GPU 多卡、多节点），每个副本的调度参数（max_num_tokens、chunked_prefill_size、mem_fraction_static、dp_size 等）若不一致会导致调度行为分歧——一个副本因 OOM 拒绝请求而另一个接受，引发路由层不一致。灰度要求少数副本先验证新配置，其余保持旧值。配置源可以是启动参数、环境变量、配置中心推送或基于共识的协调服务。实际生产中，推理服务的配置变更频率约每天 1-5 次（调度调优、模型更新、故障恢复），变更窗口通常在低峰期（凌晨 2-5 点）。配置一致性问题的典型症状包括：请求成功率波动（部分副本参数不同导致 OOM）、TTFT 分布双峰（不同 chunked_prefill_size 的副本产生不同的首 token 延迟）、以及灰度期间新旧版本行为不一致。

**问题分析：** 1) 明确一致性分级：强一致（所有副本同一时刻同值，用于影响 OOM 的参数）vs 最终一致（允许短暂分歧，用于日志级别等）；2) 设计配置分发通道：推模型（gRPC 长连接、etcd watch）vs 拉模型（定期轮询）——推模型延迟低但连接维护成本高，拉模型简单但延迟取决于轮询间隔；3) 设计灰度策略：按副本标签分阶段推送，配合健康检查自动回滚；4) 量化分歧窗口：推送延迟 × 副本数，以及该窗口内可能产生的不一致请求数；5) 处理配置中心的单点故障：副本如何在配置中心不可用时保持一致性。

**具体答案：** 面试中可以这样回答：

> 一致性分三层设计。第一层是"启动一致性"：所有副本通过相同的配置加载链（启动脚本 + 环境变量 + 配置文件版本锁）加载初始值，配置中心用单调版本号保证加载顺序。启动时副本向配置中心注册并获取当前全量配置快照，同时本地持久化一份 snapshot 作为降级备份。第二层是"运行一致性"：配置变更通过 watch 机制推送到所有副本，推送确认采用两阶段——先写入持久层（etcd/ZK），再等所有副本 ACK。ACK 超时则视为配置分歧告警，触发自动回滚到上一个全一致版本。第三层是"灰度一致性"：副本按标签分组（canary/stable），先推 canary 组，观察指标无异常再推 stable 组。分歧窗口 = 推送延迟 × 副本数，典型值 < 1s 对调度参数可接受，但 mem_fraction_static 等影响 OOM 的参数需要同步切换——此时用两阶段提交：先冻结调度 → 全副本切到新值 → 恢复调度，冻结窗口通常 < 100ms。
>
> 量化：假设 100 副本、单副本推送延迟 P99=200ms，最坏分歧窗口 100×200ms=20s。20s 内不一致副本可能接受不同数量的新请求——如果旧配置 max_num_tokens=4096、新配置=8192，分歧期间部分副本能接受 2x 的请求量，导致负载不均。对调度参数可接受；对显存参数要求强一致，冻结窗口增加约 50-100ms 调度延迟，但避免 OOM 级联。
>
> 配置中心不可用时，副本使用最后一次成功的本地快照继续服务，拒绝任何新配置变更尝试。中心恢复后，补推期间积压的变更（按版本号顺序重放）。强一致参数在此期间冻结（不允许降级变更），因为无法确认所有副本当前值是否一致。

**技术洞察：**

```text
配置一致性分级：
  L1 启动一致 → 版本号 + 配置中心 watch + 本地 snapshot
  L2 运行一致 → 推送 + ACK，分歧窗口 = Σ(push_delay_per_replica)
  超时 → 自动回滚到上一全一致版本
  L3 灰度一致 → 标签分组 + 两阶段提交（强一致参数）

量化：
  分歧窗口(弱一致) = N_replicas × push_delay_p99
  冻结窗口(强一致) = max(ACK_time) + apply_time ≈ 100ms
  不一致请求量 = divergence_window × QPS × P_replica_split

强一致参数集 = {mem_fraction_static, dp_size, max_num_tokens}
弱一致参数集 = {log_level, sampling_defaults, cache_threshold}

配置中心降级：
  unavailable → use local_snapshot
  恢复 → replay version序积压变更
  强一致参数在降级期间 freeze
```

C：配置中心推送机制与两阶段提交模式是通用设计。I：分歧窗口公式推导与不一致请求量公式。U：实际推送延迟、冻结时间与降级行为需实测。

**图文说明：** 时序图：ConfigCenter → watch → Replica1..N。canary 组先 ACK → 指标检查（1-5min）→ stable 组 ACK。强一致参数插入"冻结调度"窗口（红色矩形标注）。箭头标注各阶段延迟分量。下方分支：配置中心断连 → 所有副本 switch to local snapshot → 告警 → 恢复后版本号比对与补推。图示为逻辑流程，实际部署需考虑网络分区与脑裂场景。

**方案对比与选型：** 三种配置分发方案对比：① etcd watch（推送延迟 50-200ms，强一致，适合 < 1000 副本）；② ZooKeeper watcher（推送延迟类似，但写性能较差，适合配置元数据）；③ 消息队列广播（如 Kafka，推送延迟 100ms-1s，适合大规模但最终一致）。选型建议：推理服务副本数通常 < 500，etcd watch 是最佳选择——推送延迟低、支持 watch 前缀、提供分布式锁用于两阶段提交。当副本数 > 1000 时，考虑分层推送（每个节点一个 local relay，relay 再分发给本机副本）以降低配置中心压力。成本模型：etcd 集群 3-5 节点即可服务 500 副本，年成本约 $5K-$20K（云服务托管），远低于配置不一致导致的故障损失。

**拓展知识：** 与 I-A002 衔接：协议演进也需要类似的版本协商——新旧 API 格式共存的窗口期就是配置分歧窗口；与 I-A016 衔接：灰度发布本质是"配置+代码"的一致性联合切换；与 I-A003 衔接：autoscaling 新增副本时，新副本必须从配置中心加载当前生效配置，启动后汇报版本号——如果版本不一致则不加入服务池。

**常见追问及回答要点：**

1. **问：配置中心本身挂了怎么办？** 答：副本使用最后一次成功的配置快照（本地持久化缓存），拒绝加载未知版本。配置中心恢复后补推。对强一致参数，缺失 ACK 时冻结调度而非使用旧值——避免分裂。
2. **问：灰度过程中新旧副本的调度行为差异会不会导致请求不一致？** 答：会。canary 组的新配置可能接受更多请求，如果路由层均匀分发，canary 组先 OOM。解决方案：canary 组设置更保守的 mem_fraction_static 作为安全边界，或路由层按组权重分流。
3. **问：如何验证所有副本配置真的相同？** 答：定期全量比对（pull 模式），或利用推送 ACK 的返回值做实时校验。发现分歧时告警并自动拉回旧版本。
4. **问：1000+ 副本时 etcd watch 性能不够怎么办？** 答：分层推送架构——etcd → Regional Relay（每区域一个）→ 本机副本。Relay 减少 etcd 连接数（从 1000 → 10），本机副本用 Unix socket 或 shared memory 接收推送，延迟 < 10ms。

**易错点与反例：**

- 反例："配置一致性用分布式锁保证"——过度。配置变更频率低，锁的开销远大于 watch 推送。配置变更不是高频写操作。
- 反例："所有配置都要求强一致"——不必要。log_level、采样默认值等不影响推理行为的参数允许短暂分歧，强制强一致增加冻结窗口。
- 易错：忽视配置变更的原子性——mem_fraction_static 从 0.88 改到 0.92 过程中，如果 max_num_tokens 没同步更新，可能先释放过多缓存又拒绝新请求。多个强一致参数必须用单一原子事务提交，而非逐参数推送。
- 易错：配置中心断连时的脑裂风险——如果部分副本与中心断连而其他副本已收到新配置，断连副本可能基于旧值做出调度决策。需通过 gossip 协议检测副本间版本差异。

**实践与验证：** [U] 部署 10 副本推理服务，注入配置变更（推 100 次），测量：(a) 分歧窗口分布；(b) 强一致参数切换时的调度冻结时间；(c) 配置中心断连后副本的降级行为；(d) 版本号比对在恢复后的补推正确性。oracle：分歧窗口 P99 < 5s，冻结时间 P99 < 200ms，降级期间零不一致请求，补推后全副本版本一致。当前未执行压测。

**分层评分标准：** 4分：能区分一致性级别并设计基本推送机制；6分：能设计灰度策略并量化分歧窗口；8分：能处理网络分区、脑裂、大规模分层推送等边界情况并给出强/弱一致参数的划分依据；10分：能在成本约束下设计完整的分层配置分发体系，量化降级窗口与恢复流程。

**证据与来源：** `sglang/01-modules/M04-scheduler-batching.md`（调度参数影响）；`sglang/01-modules/M18-testing-benchmark-ci.md`（配置验证）；`insights/04-design-decision-and-validation/`（配置变更设计）。

**补充：配置回滚与多版本管理。** 配置变更的回滚不能简单地将参数改回旧值——如果新配置导致副本行为变化（如接受了更多请求），回滚后这些请求仍在服务中，可能与新配置不兼容。正确的回滚流程：① 冻结新请求调度；② 等待当前请求在旧配置下完成（或在安全点截断）；③ 推送旧配置；④ 恢复调度。多版本管理：配置中心保留最近 N 个版本的配置快照（通常 N=10），支持快速回滚到任意历史版本。版本快照的存储成本极低（每个版本 < 1MB），但提供了关键的"时间旅行"回滚能力。量化：假设每次配置变更 0.5MB，10 个版本 = 5MB 存储，可忽略。另一个易忽视的问题：配置变更的审计追踪——每次变更需记录变更人、变更时间、变更前后值、变更原因（工单号关联），用于事后回溯与安全审计。

**补充：配置变更的灰度指标与自动回滚阈值。** 灰度期间需定义"通过判据"——canary 组的 TTFT P99 不超过 stable 组的 110%、请求成功率不低于 stable 组的 99%、且无新增 OOM 事件。这些判据需在灰度开始前预定义（写入配置中心的 release policy），而非事后判断。自动回滚阈值应分级：① 警告级（指标偏离 10-20%）——告警但不自动回滚，等待人工确认；② 严重级（指标偏离 > 20% 或 OOM）——自动回滚。灰度期间的 metrics 对比需使用"同期对比"而非"历史对比"——因为负载随时间波动，同期对比（canary 与 stable 在同一时刻的指标对比）更可靠。


**补充：配置灰度的指标对比方法论。** 灰度期间的 metrics 对比需注意"统计显著性"——5% 流量的 canary 组样本量小，指标波动可能仅由随机性引起而非版本差异。建议：使用 Welch's t-test 比较 canary 与 stable 的指标差异，p-value < 0.05 才视为显著差异。样本量要求：如果 baseline P99 TPOT = 100ms、检测灵敏度 = 10ms、置信水平 95%，则需要至少 100 个样本（canary 组 5 分钟 × QPS=0.3/s ≈ 90 样本）。

**补充：配置灰度的统计显著性验证。**

灰度期间的 metrics 对比需注意统计显著性——5% 流量的 canary 组样本量小，指标波动可能仅由随机性引起而非版本差异。

建议：使用 Welch's t-test 比较 canary 与 stable 的指标差异，p-value < 0.05 才视为显著差异。

样本量要求：如果 baseline P99 TPOT = 100ms、检测灵敏度 = 10ms、置信水平 95%，则需要至少 100 个样本（canary 组 5 分钟 × QPS=0.3/s ≈ 90 样本）。

如果样本量不足，应延长灰度时间（如从 30min 延长到 2h）而非降低置信水平。样本量不足时做出的"通过"决策有 30-40% 的假阳性风险。
---

### I-A002 设计推理服务的协议演进方案：如何在兼容旧 API 的同时引入新能力？

**元数据：** 知识域：`I02 协议演进`；能力目标：`C02 API 兼容性设计`；题型：架构设计；频率：中频；前置/后续：`I-A001`（配置一致性）、`I-A016`（灰度发布）；证据状态：设计原理 C，协议规范 C，运行结论 U。

**背景知识：** 推理服务对外暴露 API（通常兼容 OpenAI Chat Completions 格式），但业务需要新能力（如多轮对话状态管理、token 级 streaming、工具调用原生支持、自定义 stop token 正则）。直接改 API 会破坏存量调用方。需要在 HTTP 请求/响应结构、版本协商、字段扩展上设计兼容层。实际生产中，推理服务的 API 演进频率约每季度 1-2 次（新能力上线），每次演进需支持至少 2-3 个历史版本（因为客户端 SDK 升级周期约 1-3 个月）。协议兼容性问题的典型症状包括：旧客户端收到无法解析的新字段导致 JSON 解析失败、新客户端依赖的字段在旧服务端返回 null、以及版本协商失败时的行为不明确。

**问题分析：** 1) 区分兼容性破坏等级：新增可选字段（安全）vs 修改字段语义（破坏）vs 新增必填字段（破坏）；2) 设计版本协商机制：URL 路径版本号、Header 版本协商、或自动降级——各有不同的客户端透明度和灰度灵活性；3) 设计灰度验证：新旧协议共存期间的请求路由与响应转换；4) 量化兼容性测试覆盖率——如何确保新增字段在所有旧版本 SDK 中不会导致解析失败；5) 协议废弃流程：如何安全地移除旧版本支持而不影响存量用户。

**具体答案：** 面试中可以这样回答：

> 协议演进遵循"新增不删改"原则。新增字段全部放在请求顶层或新增子对象中，旧客户端忽略未知字段（OpenAI SDK 的默认行为）。对必须修改的字段语义（如 `max_tokens` 改为支持无穷大表示），通过 Header `X-API-Version: 2024-01-01` 做版本协商——未发送 Header 的请求走 v1 语义，发送的走 v2。服务内部维护一个 `ProtocolAdapter` 层，将不同版本请求翻译为统一的内部请求表示，响应也经 adapter 翻译回客户端期望的版本。
>
> 灰度阶段：新 API 字段先在内部标记 `x-sglang-experimental: true`，收集使用数据后升级为正式字段。版本淘汰周期 = 旧版本最后活跃时间 + N 天冷却期。兼容性测试矩阵 = {旧SDK × 新字段} 的全组合，覆盖率要求 100% 字段组合。实际测试中，使用 5 个主流 SDK 版本（OpenAI Python SDK v0.28/v1.0/v1.5、Node SDK v4.0/v4.5）做全矩阵回归，每次新字段添加后运行完整矩阵。
>
> 量化：假设当前 v1 有 30 个字段，v2 新增 10 个，修改语义 2 个。adapter 层增加的处理延迟约 0.1ms/请求（字段映射），相比推理延迟可忽略。版本协商的缓存命中率影响：对重复客户端做版本缓存，避免每次请求都解析 Header。缓存命中率通常在 95%+（同一客户端连续发送请求）。

**技术洞察：**

```text
兼容性等级：
  Level 1 新增可选字段 → 安全，旧客户端忽略
  Level 2 新增必填字段 → 破坏，必须版本协商
  Level 3 修改字段语义 → 破坏，必须 adapter 翻译

协议演进成本：
  adapter_latency = N_fields × map_lookup_cost ≈ 0.1ms
  版本缓存命中率 = 1 - (unique_clients / total_requests)
  测试矩阵 = N_sdk_versions × N_fields = 5 × 40 = 200 组合

淘汰策略：
  冷却期 T_cool = max(last_seen_v_old) + 30d
  强制升级阈值 = 活跃用户占比 < 1%
  版本共存上限 = 3（避免 adapter 膨胀）

adapter 复杂度增长：
  O(N_versions) 每新增一版本，adapter 增加一组 translate 逻辑
  上限建议 ≤ 5 个同时支持的版本
```

C：API 版本协商与 adapter 模式是通用设计。I：adapter 延迟、缓存命中率与测试矩阵公式。U：实际淘汰周期与 SDK 兼容性需根据用户数据确定。

**图文说明：** 架构图：Client(v1/v2/v3) → Router(版本识别) → ProtocolAdapter(翻译) → InternalRequest → SGLang Engine → ProtocolAdapter(反向翻译) → Client。虚线框标注 adapter 层的双向转换逻辑。下方标注延迟预算：版本识别 0.05ms，字段映射 0.1ms。右侧：版本淘汰流程——监控 v_old 调用量 → < 1% 触发 30 天倒计时 → 通知用户 → 到期下线。

**方案对比与选型：** 三种版本协商方案对比：① URL 路径版本（`/v1/chat`、`/v2/chat`）：路由简单但需要客户端显式选择版本，不适合灰度场景；② Header 版本协商（`X-API-Version: 2024-01-01`）：客户端透明，服务端自动适配，适合灰度但需要 SDK 升级才能主动选择新版本；③ 自动降级（服务端根据客户端 User-Agent 推断版本）：最友好但推断逻辑脆弱（User-Agent 可伪造）。选型建议：生产环境用 Header 协商 + 自动降级组合。未发送 Header 的请求默认走最新稳定版（而非最旧版），减少旧版本维护成本。对存量客户端，Header 缺失时从 access log 中采样确认客户端 SDK 版本，主动推动升级。兼容性测试成本：每新增一个版本需维护 N+1 个 adapter（N 个旧版本 × 1 个新版本），测试矩阵 = 版本组合数 × 字段组合数。100 条字段的 API 需 200 个兼容性测试用例。

**拓展知识：** 与 I-A001 衔接：配置变更也需要版本协商——新调度参数通过 API Header 协商启用；与 I-A016 衔接：灰度发布时新旧版本服务共存，协议适配层承担跨版本请求翻译；与 I-A014 衔接：协议版本信息应在 access log 中记录，用于兼容性指标监控——旧版本调用占比是淘汰决策的关键数据。

**常见追问及回答要点：**

1. **问：如果新能力必须修改响应格式（如增加 token 使用统计字段），旧客户端怎么处理？** 答：旧客户端的 JSON 解析器会忽略未知字段，因此直接添加新字段是安全的。只有当需要移除或重命名旧字段时才需要版本协商。
2. **问：多个业务方要求不同版本的协议同时支持，如何管理 adapter 的复杂度？** 答：adapter 用插件式架构，每个版本一个 adapter 模块，通过工厂模式按版本号注册。新增版本只加插件不修改已有代码，保持 OCP 原则。复杂度上限 = 活跃版本数 × 字段差异数，通常 < 5 个版本共存。
3. **问：如何检测旧版本是否还有活跃调用？** 答：在 access log 中记录 API version，按版本聚合调用量。低于阈值（如 100 calls/day）触发淘汰流程。
4. **问：SDK 严格模式下未知字段导致解析失败怎么办？** 答：SDK 侧配置 `ignore_unknown_fields=True`（如 Pydantic 的 `model_config = ConfigDict(extra='ignore')`）。如果无法修改 SDK（如第三方），在 adapter 层做降级——对检测到的严格模式客户端，只返回该版本已知的字段子集（字段过滤 adapter）。

**易错点与反例：**

- 反例："直接在现有 API 上加字段就行"——不完全。如果新字段与旧字段语义冲突（如 `max_tokens: -1` 表示无限，旧客户端可能当作 0 处理），必须版本协商。
- 反例："所有版本永久支持"——不可维护。协议演进必须设淘汰机制，否则 adapter 层膨胀。
- 易错：忽视 SDK 的严格模式——部分 JSON 解析库在遇到未知字段时抛错，需要 SDK 侧配合配置 `ignore_unknown_fields`。
- 易错：版本协商的默认行为不明确——如果客户端不发送版本 Header，默认走哪个版本？建议默认走最新稳定版（而非最旧），但需在文档中明确说明，避免客户端开发者困惑。

**实践与验证：** [U] 搭建双版本服务（v1 + v2），用兼容性测试矩阵（5 个 SDK 版本 × 40 个字段 = 200 用例）做回归测试。测量：(a) adapter 层 P99 延迟；(b) 版本缓存命中率；(c) 未知字段的容错率（模拟 5 个 SDK 版本发送/接收新旧字段组合）；(d) 版本淘汰流程的正确性（模拟 30 天冷却期后的自动下线）。oracle：adapter P99 < 0.5ms，缓存命中率 > 95%，200 用例全部通过，淘汰流程无误。当前未执行压测。

**分层评分标准：** 4分：能区分兼容性等级并设计基本版本协商；6分：能设计 adapter 层并量化延迟开销与测试矩阵；8分：能处理 SDK 严格模式、多版本共存管理、版本废弃流程；10分：能在生产约束下设计完整的协议演进体系，包括 SDK 升级推动策略与字段过滤降级方案。

**证据与来源：** `sglang/01-modules/M18-testing-benchmark-ci.md`（测试矩阵）；`sglang/90-cross-module/performance-critical-paths.md`（请求处理延迟分析）；`insights/04-design-decision-and-validation/`（API 设计决策）。

**补充：SDK 升级推动策略与弃用通知。** 协议演进的瓶颈往往不在服务端（adapter 层可透明适配），而在客户端 SDK 升级滞后。实践中的 SDK 升级推动流程：① 在 API 响应 header 中返回 `X-API-Deprecation: v1 deprecated, EOL 2024-12-31`；② 在文档和 SDK changelog 中提前 60 天通知；③ 在 SDK 层面——旧版 SDK 发送请求时自动注入最低兼容版本 Header，新版 SDK 发送最新版本 Header；④ 到期后对仍使用旧版本的客户端返回 410 Gone + 升级指引。量化效果：配合此策略，SDK 版本覆盖率（最新版占比）通常在 90 天内从 20% 提升到 80%。特殊情况：嵌入式/移动端 SDK 升级周期长（3-6 个月），需延长其版本支持周期或提供特殊兼容模式。

**补充：协议变更的影响面评估流程。** 每次 API 变更前需执行影响面评估：① 分析过去 30 天该 API 的调用量、调用方列表、使用方式（哪些字段被使用）；② 识别"破坏性变更"——如果修改的字段被 > 1% 的调用方使用，升级为"需用户确认的变更"并提前通知；③ 对内部字段（以 `x-` 前缀开头）的修改可跳过影响面评估，因为约定上内部字段不保证稳定性。量化：假设 v1 API 有 20 个字段，其中 5 个被 > 10% 调用方使用（核心字段），10 个被 1-10% 使用（扩展字段），5 个被 < 1% 使用（冷门字段）。核心字段的变更需要 60 天通知期，扩展字段 30 天，冷门字段 7 天。


**补充：API 兼容性测试自动化。** 建议建立"兼容性 CI pipeline"——每次 API 变更提交时，自动运行全矩阵测试（所有 SDK 版本 × 所有字段组合）。使用 Docker 容器模拟不同 SDK 版本的运行环境（如 openai-python v0.28 vs v1.0），验证新增字段不会导致旧版本解析失败。CI pipeline 还应在 staging 环境做一次"真实流量回放"——将生产流量录制后重放到新版本，比较响应差异。

**补充：API 兼容性测试自动化与流量回放。**

建议建立"兼容性 CI pipeline"——每次 API 变更提交时，自动运行全矩阵测试（所有 SDK 版本 × 所有字段组合）。使用 Docker 容器模拟不同 SDK 版本的运行环境。

CI pipeline 还应在 staging 环境做一次"真实流量回放"——将生产流量录制后重放到新版本，比较响应差异。

流量回放的关键技术：录制时保存 (request_hash, response) 对；重放时对比 response 的 diff。对于 streaming 响应，需逐 chunk 对比。允许的 diff 范围：数值容差 1e-6（浮点误差）、新增字段不影响（旧客户端忽略）、删除字段标记为 breaking change。
---

### I-A003 设计推理服务 autoscaling：基于 QPS、GPU util、KV cache 水位的多信号伸缩策略。

**元数据：** 知识域：`I04 自动伸缩`；能力目标：`C04 多信号弹性`；题型：架构设计与量化；频率：高频；前置/后续：`I-A015`（优雅缩容）、`I-A006`（多租户 KV 隔离）；证据状态：设计模式 C，阈值与窗口 U，实测结论 U。

**背景知识：** 推理服务负载波动大（日内峰谷可达 5-10x），纯 QPS 指标滞后——QPS 突增时 GPU 已经过载。多信号伸缩：QPS（请求级）、GPU utilization（计算级）、KV cache 水位（显存级）三个维度。挑战在于信号冲突（QPS 低但 GPU util 高，可能 batch 太大），以及伸缩动作的冷却与惯性。实际生产中，推理服务的扩容触发频率约每天 3-10 次（早晚高峰 + 突发事件），缩容约每天 2-5 次。扩容延迟是关键 SLA 因素——如果扩容延迟 > 负载增长速率 × 扩容期间请求积压量，会导致排队溢出。多信号伸缩的典型故障包括：信号噪声导致频繁伸缩（震荡）、KV cache 水位误触发（少量长序列就导致全局扩容）、以及多信号聚合策略不当导致扩容不足。

**问题分析：** 1) 定义每个信号的阈值与方向：扩容触发条件 vs 缩容触发条件；2) 设计信号聚合策略：OR（任一触发扩容）、AND（全部触发缩容）或加权投票；3) 量化扩容延迟：从触发到新副本就绪的时间 = 调度 + 启动 + 预热，以及该延迟内的请求积压量；4) 设计冷却窗口避免抖动；5) 处理信号噪声——使用滑动窗口均值而非瞬时值，防止毛刺误触发；6) 预测性扩容：基于历史曲线提前扩容，弥补扩容延迟。

**具体答案：** 面试中可以这样回答：

> 三信号分层决策。扩容（激进）：任一信号超阈值即触发——QPS > 80% 容量 OR GPU util > 85% 持续 30s OR KV cache > 90% 持续 10s（OOM 风险信号最短窗口）。缩容（保守）：全部信号低于阈值才触发——QPS < 30% AND GPU util < 40% AND KV cache < 50%，持续 300s 冷却。冲突处理：如果 QPS 低但 KV cache 高（少量长序列），优先检查显存信号——缩容可能导致 OOM，因此保守处理不缩。
>
> 扩容副本数计算：scale_out = ceil(current_replicas × max(QPS_ratio, util_ratio))。例如 QPS 是容量的 1.5 倍，则扩到 1.5× 当前副本数。扩容延迟预算：K8s 调度 5s + 容器启动 30s + 模型加载 60s + 预热 10s ≈ 105s。这意味着扩容触发必须提前——如果当前负载增长率为 R（QPS/s），则提前量 = 扩容延迟 × R。例如 R=10 QPS/s、扩容延迟 105s，提前扩容时机 = 当前容量 - 105×10 = 当前容量 - 1050 QPS。
>
> 量化实例：10 副本、QPS 突增 3x（从 100 到 300）。扩容触发 → 105s 后 30 副本就绪。如果 QPS 在 60s 内就达到 3x，中间 45s 过载。解决方案：预测性扩容（基于历史同期曲线）+ 紧急扩容（跳过预热直接加入，但限流到 50% 容量）+ 请求排队（超出容量的请求排队等待而非拒绝）。排队延迟预算 = 扩容延迟 - 当前排队深度 / 处理速率。

**技术洞察：**

```text
扩容触发 = QPS_high(30s) OR Util_high(30s) OR KV_high(10s)
缩容触发 = QPS_low AND Util_low AND KV_low, sustained 300s

扩容副本数：N_new = ceil(N_current × max(QPS_ratio, Util_ratio))
扩容延迟：T_scale = T_schedule + T_boot + T_model_load + T_warm ≈ 105s
预测提前量：T_advance = T_scale × load_growth_rate
排队预算：T_queue = T_scale - queue_depth / process_rate

信号权重（冲突仲裁）：
  KV cache > 90% → 最高优先级（OOM 风险，窗口 10s）
  GPU util > 85% → 中优先级（吞吐下降，窗口 30s）
  QPS > 80% → 参考级（可能被 batch 吸收，窗口 30s）

防震荡：
  冷却窗口 T_cool = max(300s, 2 × T_scale)
  单次扩容上限 = 50% × N_current
```

C：多信号聚合与分层决策模式。I：扩容延迟、提前量与排队预算公式。U：具体阈值、延迟与预测准确性需根据基础设施实测。

**图文说明：** 状态图：Steady →(扩容触发)→ ScalingOut →(副本就绪)→ Steady；Steady →(缩容触发 + 冷却)→ ScalingIn →(副本回收)→ Steady。旁注：扩容路径粗箭头（快速，标 OR），缩液路径虚线箭头（缓慢，标 AND + 300s）。下方时序：QPS 曲线 vs 副本数曲线，标注 105s 延迟间隙与预测性扩容的提前区间（绿色阴影）。

**方案对比与选型：** 三种伸缩判据聚合策略对比：① OR 扩容 + AND 缩容（推荐）：扩容激进（任一信号超阈值即扩）、缩容保守（全部信号低于阈值才缩），利用非对称性天然防震荡；② 加权投票（QPS权重0.3 + GPU util权重0.4 + KV cache权重0.3）：更平滑但权重调参困难，且信号量纲不同需归一化；③ 单一信号（仅 QPS）：实现简单但 QPS 滞后于实际负载——GPU util 已饱和但 QPS 尚未下降（请求排队）。选型建议：OR/AND 策略最适合推理服务，因其信号方向明确且优先级可定义。复杂度在于阈值设定——建议从保守阈值开始（扩容阈值偏高、缩容阈值偏低），逐步调松。冷启动问题解决：预热副本池（保持 1-2 热备副本，平时不服务流量），扩容时热备秒级就绪（< 5s），但每小时额外成本约 $2-5（A100 单价）。

**拓展知识：** 与 I-A015 衔接：缩容时的 drain 策略决定副本能否优雅退出；与 I-A006 衔接：多租户场景下 KV cache 水位需按租户维度拆分，避免单一租户触发全局扩容；与 I-A014 衔接：伸缩决策应记录完整的信号快照到 metrics，用于事后分析扩容/缩容的触发原因与预测准确性。

**常见追问及回答要点：**

1. **问：三个信号同时指向不同方向怎么办？** 答：定义优先级：KV cache 信号 > GPU util > QPS。因为 KV cache 直接关联 OOM（不可恢复），GPU util 关联吞吐下降（可降级），QPS 可能被 batch 吸收（自然缓冲）。高优先级信号覆盖低优先级信号。
2. **问：扩容期间新副本还在启动，旧副本已经 OOM 了怎么办？** 答：紧急降级——降低 max_num_tokens 或启用请求排队，用延迟换生存。新副本就绪后恢复。同时触发扩容上限检查——如果已达到最大副本数上限，启动跨模型降级（将非关键请求路由到更小模型）。
3. **问：如何避免早周期（早上 9 点流量突增）反复触发扩容震荡？** 答：冷却窗口 + 扩容上限 + 预测性扩容。基于历史同期数据预扩容，在负载到来前 5 分钟提前扩容。
4. **问：KV cache 水位信号噪声大（少量长序列就触发）怎么处理？** 答：用滑动窗口均值替代瞬时值，窗口长度 ≥ 30s。同时用 KV cache 使用率的中位数而非最大值——单个长序列不应触发全局扩容。

**易错点与反例：**

- 反例："三个信号取平均值"——错误。信号量纲不同（QPS 是计数、util 是百分比、KV cache 是 GB），取平均无意义。
- 反例："扩容越快越好"——过度。副本过多导致 KV cache 碎片化、调度开销增大、成本失控。需设副本上限。
- 易错：忽视扩容的"惯性"——新副本加入后负载下降，如果立即缩容又触发扩容，形成震荡。冷却窗口必须 > 2× 扩容延迟。
- 易错：预测性扩容的过拟合风险——如果历史数据包含异常事件（如促销活动），预测会过度扩容。需对历史数据做异常检测与剔除。

**实践与验证：** [U] 在 K8s 环境部署推理服务，注入阶梯负载（QPS 10→100→500→100）和脉冲负载（QPS 10→300 持续 30s → 10），测量：(a) 扩容触发延迟（负载超阈值到决策）；(b) 扩容生效延迟（决策到新副本 service-ready）；(c) 过载时长（负载超容量到扩容完成）；(d) 预测性扩容的准确率（预测负载 vs 实际负载的 MAE）。oracle：扩容触发 < 10s，生效 < 120s，过载 < 60s，预测 MAE < 15%。当前未执行。

**分层评分标准：** 4分：能设计基本的多信号伸缩触发逻辑；6分：能量化扩容延迟与提前量，处理信号冲突；8分：能设计预测性扩容、紧急降级、防震荡机制；10分：能在成本约束下设计完整的 SLO 驱动的伸缩框架，包括热备副本池的经济性分析与异常负载检测。

**证据与来源：** `sglang/90-cross-module/pooling-and-resource-management.md`（资源池化）；`sglang/99-roadmap/performance-guide.md`（负载分析）；`insights/05-sglang-inference-technical-insights/`（弹性伸缩实践）。

**补充：紧急扩容与过载保护联动。** 扩容判据之外还需设计"紧急模式"——当请求队列深度超过队列容量 80% 时，跳过冷却窗口直接扩容（紧急扩容副本数 = 当前副本数 × 1.5，上限为最大副本数）。紧急扩容的代价是可能过度供给，但避免请求丢失。过载保护联动机制：① 队列深度 < 50% → 正常模式；② 50-80% → 预警模式（降低 max_num_tokens 减少单请求显存占用，释放资源给排队请求）；③ 80-95% → 紧急扩容 + 限流（拒绝低优先级新请求）；④ > 95% → 拒绝所有新请求（返回 503 + Retry-After header）。这四级联动确保在扩容延迟窗口内，服务从"降级"到"拒绝"逐步保护，而非直接崩溃。量化：假设队列容量 = 1000 请求、处理速率 = 100 req/s，从 50% 队列到清空需要 5s——在扩容延迟 105s 内，限流和降级是唯一防线。

**补充：autoscaling 的成本-性能 Pareto 前沿分析。** 成本与性能存在本质权衡——更多副本 = 更低的延迟但更高的成本。Pareto 前沿分析：在（成本, 延迟）二维平面上，每个副本数对应一个运营点。Pareto 前沿是所有"无法在不增加成本的情况下降低延迟"的点的集合。实际部署中，Pareto 前沿通常是一个曲线——从（低成本, 高延迟）到（高成本, 低延迟）。最优运营点取决于 SLA 约束：如果 SLA 要求 P99 TPOT < 100ms，则找到满足该约束的最小副本数。量化：假设 10 副本成本 $20/h、P99 TPOT 80ms；8 副本成本 $16/h、P99 TPOT 110ms。若 SLA 为 100ms，则 10 副本是最优解——8 副本虽便宜但违约。


**补充：扩容决策的预测模型设计。** 除了基于当前 metrics 的被动扩容，还可构建预测模型。输入特征：过去 5 分钟的 QPS 序列、当前小时（0-23）、工作日/周末标志、是否节假日。模型选择：简单场景用线性回归（预测未来 5 分钟 QPS），复杂场景用 LightGBM（非线性特征交互）。预测模型的评估指标：MAPE（平均绝对百分比误差），目标 < 20%。预测模型的更新频率：每天用新数据重新训练（增量训练，成本约 5 分钟 GPU 时间）。

**补充：扩容决策的预测模型设计与评估。**

除了基于当前 metrics 的被动扩容，还可构建预测模型。输入特征：过去 5 分钟的 QPS 序列、当前小时（0-23）、工作日/周末标志、是否节假日。

模型选择：简单场景用线性回归（预测未来 5 分钟 QPS），复杂场景用 LightGBM（非线性特征交互）。不推荐 LSTM——推理延迟高（10ms+）且收益有限。

预测模型的评估指标：MAPE（平均绝对百分比误差），目标 < 20%。预测模型的更新频率：每天用新数据重新训练（增量训练，成本约 5 分钟 GPU 时间）。

预测模型的部署架构：离线训练（每天一次）→ 导出 ONNX → 在线推理（CPU 上 < 1ms）。避免将预测模型部署在 GPU 上（浪费推理 GPU 算力）。
---

### I-A004 设计 prefill 与 decode 阶段的差异化资源分配：如何在同一 GPU 上平衡两者？

**元数据：** 知识域：`I06 prefill/decode 资源分配`；能力目标：`C06 双阶段资源平衡`；题型：架构设计与量化；频率：高频；前置/后续：`I-B001`（双阶段差异）、`I-A011`（PD 分离）；证据状态：调度算法 C，量化公式 C，运行结论 U。

**背景知识：** prefill 是 compute-bound、decode 是 bandwidth-bound。同一 GPU 上两者共存时，资源竞争表现为：prefill 独占 compute 时 decode 请求等待 → decode TTFT 增加；decode 占据带宽时 prefill 的 GEMM 变慢 → prefill throughput 下降。需要在显存分配、时间片调度、batch 组成上做权衡。实际生产中，推理服务的 prefill:decode 时间占比约为 1:5（短序列场景）到 1:50（长序列场景）——一个 2K prompt 的 prefill 可能需要 200ms，而后续 200 个 decode step 每步 20ms = 4000ms。这意味着 decode 阶段占用了绝大多数 GPU 时间片，prefill 必须"见缝插针"。典型故障包括：长 prefill 阻塞所有 decode 请求导致 TPOT 飙高、以及 prefill 中间激活的显存峰值触发 OOM。

**问题分析：** 1) 显存分配：KV cache 占总显存的比例——prefill 需要临时中间激活空间，decode 需要 KV cache 空间；2) 时间片调度：prefill 与 decode 的交错比例——多少次 decode 后插入一次 prefill；3) batch 组成：一个 ForwardBatch 中 prefill 请求与 decode 请求的比例；4) 量化：给定负载分布（prompt_len 分布、decode_len 分布），求最优分配；5) 处理混合 batch 的 attention 计算——prefill 部分和 decode 部分在同一 ForwardBatch 中如何高效执行。

**具体答案：** 面试中可以这样回答：

> 显存分配按"先保证 decode 再服务 prefill"原则。KV cache 预留 80% 显存给 decode 并发序列（保证最低吞吐），剩余 20% 给 prefill 的临时激活空间。当 prefill 并发增加时压缩 KV cache 页，动态调整。调度上采用"prefill 预算制"——每个 decode step 后检查是否有 prefill 请求等待，如果有且 prefill_budget（剩余 token 预算）> prompt_len，则插入一次 prefill chunk（chunked_prefill_size tokens）。
>
> 量化模型：假设模型权重 W GB，GPU 显存 M GB。KV cache 可用 = M - W - activation_overhead。每个 decode 请求需要 KV_cache_per_req = 2 × n_layers × n_heads × head_dim × seq_len × dtype_size bytes。最大并发 decode 请求数 N_decode_max = floor(KV_cache_available / KV_cache_per_req(avg_seq_len))。Prefill 预算 = M - W - KV_cache_used - safety_margin。当 N_decode × KV_cache_per_req > 80% × (M - W) 时暂停接受新 prefill。
>
> 时间片权衡：每次 prefill chunk 阻塞 decode 的时间 T_block = prefill_chunk_size × compute_per_token / compute_prefill_efficiency。如果 T_block > decode_TTFT_SLO，则减少 prefill_chunk_size 或延长 prefill 间隔。实测中 chunked_prefill_size=2048-8192 是常见折中点。对 Llama-3-8B（A100-80G），chunk_size=4096 时 T_block ≈ 30ms，对 TPOT SLO=50ms 的影响可接受（60% 的 decode step 不受影响）。

**技术洞察：**

```text
显存分配：
  KV_cache_pool = M_GPU - M_weights - M_activation - M_safety
  N_decode_max = floor(KV_cache_pool / KV_per_request(avg_len))
  Prefill_budget = KV_cache_pool - KV_cache_used - margin

调度交错：
  T_block_per_chunk = C_chunk × t_compute_token
  Prefill_interval = max(1, floor(T_TTFT_SLO / T_block_per_chunk))

动态调整：
  if KV_cache_used > 80% pool → 暂停新 prefill
  if KV_cache_used < 50% pool → 增加 prefill_chunk_size

混合 Batch：
  ForwardBatch = extend_part(prefill) + decode_part(decode)
  Attention kernel 通过 stride 区分两种模式
  extend: full attention over prompt tokens
  decode: single-token attention over KV cache
```

C：SGLang 的 PrefillAdder 与 chunked prefill 机制。I：显存与并发数公式。U：具体 chunk size 与阻塞时间需实测。

**图文说明：** 堆叠柱状图：GPU 显存分为 Weights（固定）、KV Cache（动态增长）、Prefill Activation（动态波动）、Safety Margin（固定 5%）。右侧时间线：decode batch → decode batch → prefill chunk → decode batch... 标注 T_block 时长（红色矩形覆盖 decode 等待段）。旁注：ForwardBatch 内部结构——extend tokens（蓝色）与 decode tokens（绿色）共存，attention kernel 通过不同 stride 处理。

**方案对比与选型：** 三种调度交错策略对比：① 固定比例交错（每 N 个 decode batch 插入 1 个 prefill chunk）：实现简单但无法适应负载变化——当无 prefill 请求时浪费"插入位"；② 预算制（PrefillAdder，当前实现）：每次 decode 后检查 prefill_budget，有预算则插入 prefill，无预算则连续 decode——资源利用率高，但实现复杂（需跟踪 token 预算）；③ 时间片轮转（prefill 占 X% 时间、decode 占 Y%）：可保证 decode 的最低延迟上限，但可能导致 prefill 饥饿（短请求多的场景）。选型建议：预算制（方案②）是 SGLang 采用的最优解——它将显存预算转化为 token 调度预算，自适应负载变化。关键参数 `max_num_prefill_tokens` 控制单次 prefill 最大 token 数，默认值 = max_num_tokens / 2。调优方向：当 decode TTFT SLO 严格时降低 prefill 预算；当 prefill throughput 不足时增加预算。

**拓展知识：** 与 I-A011 衔接：PD 分离将 prefill 和 decode 分到不同节点，正是为了消除这里的资源竞争；与 I-A003 衔接：autoscaling 决策时 KV cache 水位信号直接反映 decode 资源余量；与 I-A005 衔接：prefill 阶段不使用 CUDA Graph 的原因之一是其 GEMM shape 多变，而 decode 阶段的固定 shape 恰好适合 Graph。

**常见追问及回答要点：**

1. **问：如果 prefill 请求的 prompt 极长（100K tokens），chunked prefill 的 chunk 大小怎么选？** 答：chunk_size 与 prompt 长度解耦——chunk_size 的选择取决于单次 chunk 的阻塞时间预算（< TTFT SLO 增量），而非总 prompt 长度。100K prompt 可能需要 50-100 个 chunk，但每个 chunk 大小固定（如 4096），总 prefill 时间 = N_chunks × (T_chunk + T_decode_resume)。
2. **问：prefill 与 decode 共用 batch 时的 attention 计算怎么做？** 答：ForwardBatch 支持混合模式——prefill 部分用 extend（full attention over prompt），decode 部分用 decode（single-token attention over KV cache），底层 kernel 处理不同 stride 的 attention。SGLang 的 `context_attention` 和 `decode_attention` kernel 在一个 batch 中切换。
3. **问：mem_fraction_static 设置过高（如 0.95）会有什么风险？** 答：留给运行时动态分配（如 prefill 激活、临时 tensor）的空间不足，可能触发 CUDA OOM 而非优雅降级。通常 0.85-0.90 是安全范围。
4. **问：prefill 的临时激活空间如何估算？** 答：prefill 峰值激活 ≈ 2 × batch_size × seq_len × hidden_dim × dtype_size（attention score matrix）。对 Llama-3-8B（hidden=4096）、batch=4、seq=4096：2 × 4 × 4096 × 4096 × 2bytes = 262MB。这解释了为什么 prefill 需要额外的显存安全边际。

**易错点与反例：**

- 反例："prefill 和 decode 各占 50% 显存"——静态均分无法适应负载变化。decode 的 KV cache 随序列长度动态增长，必须动态调整。
- 反例："先做完所有 prefill 再做 decode"——错误。这会导致 decode 请求饿死，TTFT 无限增长。必须交错调度。
- 易错：忽视 prefill 临时激活的显存峰值——长 prompt 的 attention 中间结果可能数 GB，需在 KV cache 之外预留。
- 易错：混合 batch 的 attention kernel 效率问题——当 batch 中 prefill 和 decode 的比例悬殊时（如 1 个 prefill + 100 个 decode），attention kernel 的并行度不均衡，效率下降。建议 batch 中 prefill tokens 占比保持在 10-50%。

**实践与验证：** [U] 固定 Llama-3-8B 与 A100-80G，测量不同 chunked_prefill_size 下的：(a) prefill throughput（tokens/s）；(b) decode TTFT 分布（P50/P95/P99）；(c) 显存利用率（实际 KV cache 使用率 vs 理论最大值）；(d) 混合 batch 的 attention kernel 效率（FLOPs 利用率）。oracle：找到 TTFT P99 < 2s 时的最大 prefill throughput 点，并确认混合 batch 效率 > 80%。当前未执行 GPU 实验。

**分层评分标准：** 4分：能描述 prefill/decode 的显存与计算需求差异；6分：能建立显存分配模型并量化并发上限与阻塞时间；8分：能设计动态调整策略并分析 chunk size、调度间隔与 SLO 的定量关系；10分：能在混合 batch 效率、显存碎片、长 prompt 极端场景下给出完整的资源分配方案。

**证据与来源：** `sglang/01-modules/M04-scheduler-batching.md`（PrefillAdder、chunked prefill）；`sglang/01-modules/M08-kv-cache.md`（KV cache 内存计算）；`sglang/02-request-flow/`（ForwardBatch 双模式）。

**补充：极端负载下的 prefill 保护机制。** 当 prefill 请求的 prompt 长度分布出现极端离群值（如 1% 的请求 prompt > 50K tokens）时，chunked prefill 的阻塞时间 T_block 可能远超 decode TTFT SLO。解决方案：为长 prefill 设置"隔离预算"——长 prompt（> 16K tokens）使用独立的 prefill_budget（占总预算的 30%），短 prompt 使用剩余 70%。这确保长 prefill 不会完全阻塞短 prefill 和 decode。量化：假设长 prefill chunk 阻塞 200ms（vs 短 prefill 30ms），如果长 prefill 占用全部 budget，decode TTFT 可能增加 170ms——隔离后长 prefill 的阻塞被限制在 30ms（只使用 30% 的调度时间片）。此外，长 prefill 请求排队时应在队列头部标注"预计阻塞时间"，供调度器做智能排序——将长 prefill 集中到同一个 ForwardBatch 中执行（摊薄固定开销），而非分散到多个 batch。

**补充：极端负载下的 prefill 保护机制。** 当 prefill 请求的 prompt 长度分布出现极端离群值（如 1% 的请求 prompt > 50K tokens）时，chunked prefill 的阻塞时间 T_block 可能远超 decode TTFT SLO。解决方案：为长 prefill 设置"隔离预算"——长 prompt（> 16K tokens）使用独立的 prefill_budget（占总预算的 30%），短 prompt 使用剩余 70%。这确保长 prefill 不会完全阻塞短 prefill 和 decode。量化：假设长 prefill chunk 阻塞 200ms（vs 短 prefill 30ms），如果长 prefill 占用全部 budget，decode TTFT 可能增加 170ms——隔离后长 prefill 的阻塞被限制在 30ms（只使用 30% 的调度时间片）。


**补充：长 prompt 的显存预算精细化。** 长 prompt（> 16K tokens）的 prefill 不仅需要更多计算时间，还需要更多临时显存（attention score matrix 为 prompt_len × prompt_len）。对 Llama-3-8B（hidden=4096）、prompt=16K 的 attention score 矩阵：16000 × 16000 × 2bytes = 512MB。这在多请求 batch 时会线性增长（batch × 512MB），可能超出 KV cache 之外的安全边际。解决方案：使用 Flash Attention（tiled attention，不实例化完整 score matrix）将激活显存从 O(n²) 降到 O(n)。

**补充：长 prompt 显存峰值分析与 Flash Attention 优化。**

长 prompt（> 16K tokens）的 prefill 不仅需要更多计算时间，还需要更多临时显存。attention score matrix 大小为 prompt_len × prompt_len × dtype。

对 Llama-3-8B（hidden=4096）、prompt=16K 的 attention score 矩阵：16000 × 16000 × 2bytes = 512MB。多请求 batch 时线性增长。

解决方案：Flash Attention 使用 tiled attention 算法，不实例化完整 score matrix，将激活显存从 O(n²) 降到 O(n)。对 16K prompt，Flash Attention 的激活显存从 512MB 降到约 12MB（tiled block size=128，16000×128×2bytes × 2 = 8MB + 开销）。

Flash Attention 的代价：计算量略有增加（约 5-10% 的 recompute），但显存节省显著。长上下文场景下 Flash Attention 是必选项。
---

### I-A005 设计 CUDA Graph 与 eager 模式的动态切换策略：捕获成本 vs 性能收益如何量化？

**元数据：** 知识域：`I07 CUDA Graph 切换`；能力目标：`C07 图模式切换决策`；题型：架构决策与量化；频率：中频；前置/后续：`I-A010`（量化部署）、`I-B001`（双阶段差异）；证据状态：CUDA 机制 C，公式推导 C，运行结论 U。

**背景知识：** CUDA Graph 将一系列 kernel 调用捕获为图，回放时消除 kernel launch overhead 和 CPU-GPU 同步点，对小 batch decode 场景（batch=1-4）可提升 20-40% 吞吐。但 Graph 需要预先捕获固定 shape，每次 shape 变化（不同 batch size、不同 sequence length）需要重新捕获，捕获本身有开销（几十 ms 到几百 ms）。eager 模式无捕获开销但 kernel launch 开销高。实际生产中，推理服务的 batch size 分布高度偏斜——70%+ 的 decode step 使用 batch 1-4，但仍有 10-20% 的 step 使用 batch > 8（高并发时段）。CUDA Graph 的典型故障包括：未覆盖的 batch size 导致 fallback 到 eager（吞吐突然下降）、Graph 缓存过多耗尽显存、以及 Graph 捕获期间的服务不可用。

**问题分析：** 1) 量化收益：Graph 节省的 launch 开销 × 每秒 step 数；2) 量化成本：捕获耗时 + 显存占用（Graph 节点占用额外显存）；3) 切换判据：在什么负载条件下值得切换到 Graph；4) 动态策略：哪些 batch size 范围用 Graph、哪些用 eager；5) Graph 缓存管理——LRU 淘汰与显存预算；6) 捕获期间的请求处理——同步捕获阻塞请求 vs 异步捕获。

**具体答案：** 面试中可以这样回答：

> CUDA Graph 的收益 = 节省的 kernel launch 开销。单次 decode step 的 launch overhead 在 eager 模式下约 50-200μs（取决于 kernel 数量），Graph 回放时降到接近 0。一个 70B 模型 decode 一次约 150 个 kernel，总 launch 开销约 30ms/step（eager）vs 0.5ms/step（Graph）。但 Graph 捕获耗时约 200-500ms（取决于模型大小与 batch 范围），且每个 batch size 需要独立捕获。
>
> 切换判据：当 decode batch size ≤ Graph_max_batch 且当前 batch 分布稳定（未来 N 秒内 batch 大小不会超出已捕获范围）时使用 Graph。量化：如果平均 batch=2、decode step 时间=50ms（eager），Graph 节省 30ms/step → 吞吐提升 60%。但捕获 10 个 batch size（1-10）需要 2-5s，只在 batch 分布稳定 > 30s 时才值得。
>
> 动态策略：维护一个 batch size 到 CUDA Graph 的缓存字典。常用 batch size（1-8）预捕获，遇到未捕获的 batch size 时 fallback 到 eager 并异步捕获。如果 batch 波动频繁（标准差 > 3），关闭 Graph 避免频繁重捕获。显存成本：每个捕获的 Graph 额外占用约 50-200MB（取决于模型和 batch），10 个 Graph 额外 0.5-2GB，需从 KV cache 预算中扣除。
>
> 捕获期间的请求处理：采用异步捕获策略——首次遇到未捕获的 batch size 时，当前请求走 eager 处理，同时在后台线程发起 Graph 捕获。后续请求即可使用已捕获的 Graph。这种策略确保捕获不阻塞服务，但首次命中新 batch size 的请求享受不到加速。

**技术洞察：**

```text
收益模型：
  T_step_eager = T_compute + N_kernels × T_launch
  T_step_graph = T_compute + T_replay_overhead  (≈ T_compute + 0.1×N_kernels×T_launch)
  Speedup = T_step_eager / T_step_graph ≈ 1 + N_kernels × T_launch / T_compute

  例：N_kernels=150, T_launch=200μs, T_compute=50ms
  Speedup ≈ 1 + 30ms/50ms = 1.6x

成本模型：
  T_capture = Σ(batch_sizes) × T_per_capture ≈ N × 200-500ms
  M_graph = N × 50-200MB

切换判据：
  Use Graph iff (batch ∈ captured_set) AND (batch_stddev < threshold) 
              AND (stable_duration > T_capture / speedup_gain)

Graph 缓存管理：
  max_cache_size = floor(M_available / M_per_graph)
  LRU eviction when cache full
  Pre-capture batch 1-8 at startup (cover 90%+ of steps)
```

C：CUDA Graph launch overhead 节省原理。I：speedup 公式推导。U：具体 kernel 数量、launch 开销、捕获时间需实测。

**图文说明：** 横轴 batch size、纵轴 throughput 的折线图：eager 线（低但稳定）vs Graph 线（高但仅在离散 batch size 有值，之间用虚线连接 fallback 到 eager）。下方时间线：稳定期（Graph 收益）→ 波动期（频繁重捕获，净亏损）。旁注：Graph 缓存字典结构——{batch_size: graph_handle, last_access_time}，LRU 淘汰箭头标注。

**方案对比与选型：** 三种 Graph 管理策略对比：① 预捕获常用 batch（1-8）：启动时预捕获 batch 1 到 8 的 Graph，运行时直接使用。简单但遇到 batch > 8 时 fallback；② 运行时按需捕获 + LRU 缓存：首次遇到未捕获的 batch size 时同步捕获（阻塞当前请求），后续复用。缓存容量 = 10-20 个 Graph。灵活但首次命中延迟高；③ 混合策略（推荐）：启动时预捕获 batch 1-8（覆盖 90% 场景），运行时异步捕获不常用的 batch size（如 9-16），缓存满时淘汰最久未用 Graph。选型建议：混合策略最优，因为 batch 分布通常高度集中在小 batch（1-4 占 80%+）。显存预算：每个 Graph 额外 50-200MB，20 个 Graph = 1-4GB，从 KV cache 预算中扣除后仍值得（decode 吞吐提升 30-60%）。关键监控指标：Graph cache miss rate（目标 < 5%）、捕获频率（目标 < 1 次/分钟）、Graph 显存占用占比（目标 < 5% 总显存）。

**拓展知识：** 与 I-A010 衔接：量化后 kernel 数量可能变化（如 FP8 fused kernel 减少 kernel 数），影响 Graph 收益——kernel 数减少意味着 launch 开销占比降低，Graph 的边际收益下降；与 I-B001 衔接：prefill 阶段不用 Graph（GEMM shape 多变且 launch 开销占比小），仅 decode 阶段用；与 I-A004 衔接：Graph 仅用于 decode 阶段，不影响 prefill/decode 的显存分配策略。

**常见追问及回答要点：**

1. **问：为什么 prefill 不用 CUDA Graph？** 答：prefill 的 GEMM shape 随 prompt 长度变化大（每次 prefill 的 token 数不同），且 prefill 本身 compute-bound，launch 开销占比 < 1%。捕获各种 shape 的 Graph 显存成本极高，收益低。
2. **问：Graph 显存占用从哪来？** 答：Graph 捕获时固化了所有 tensor 的地址和 kernel 参数，这些 metadata 和潜在的中间 buffer 占用显存。每个不同 batch size 的 Graph 独立存储。
3. **问：如果 decode batch 在 Graph 覆盖范围内但序列长度变化，Graph 还能用吗？** 答：CUDA Graph 对 sequence length 不敏感——decode 每步只处理一个新 token，sequence length 通过 KV cache 指针间接传递，Graph 不绑定具体长度。但 batch size 必须匹配捕获时的值。
4. **问：多个请求并发 decode 时 Graph 的线程安全性？** 答：CUDA Graph 回放是线程安全的——多个 stream 可以同时回放同一个 Graph。但实际推理服务中每个 GPU 通常用单一 stream decode，不存在并发回放冲突。

**易错点与反例：**

- 反例："所有请求都用 CUDA Graph"——错误。batch size 超出捕获范围时必须 fallback，长 prompt prefill 完全不适合 Graph。
- 反例："Graph 捕获一次永久有效"——有误。如果模型权重更新或显存布局变化（如 OOM 后重新分配），Graph 失效需重捕获。
- 易错：忽视 Graph 的显存机会成本——占用的显存本可用于 KV cache 服务更多并发请求。对显存紧张的部署（如单卡 8B 模型），Graph 的 100MB 可能意味着减少 2-4 个并发序列。
- 易错：异步捕获的竞态条件——如果两个请求同时遇到同一未捕获的 batch size，可能触发双重捕获。需用锁或原子标记防止重复捕获。

**实践与验证：** [U] 固定 Llama-3-8B + A100，测量 eager vs Graph 在 batch=1,2,4,8 下的 decode throughput；测量捕获 10 个 batch size 的总耗时；测量 Graph 额外显存占用；测量 batch 波动场景下的 Graph cache miss rate 与 fallback 频率。oracle：batch≤4 时 Graph 提升 >30%，batch>8 时提升 <5%，cache miss rate < 5%，捕获期间零服务中断。当前未执行。

**分层评分标准：** 4分：能描述 CUDA Graph 的收益与成本；6分：能建立 speedup 模型并给出切换判据；8分：能分析 batch 波动、显存成本、量化影响下的动态切换策略；10分：能在显存约束下设计完整的 Graph 缓存管理体系，包括异步捕获、竞态处理、淘汰策略与监控指标。

**证据与来源：** `sglang/01-modules/M09-attention-cuda-graph.md`（CUDA Graph 集成）；`sglang/99-roadmap/performance-guide.md`（性能测量方法）；`insights/arxiv-pdf/2405.04437/`（Graph 优化论文）。

**补充：混合精度场景下的 Graph 管理。** 当模型使用混合精度（FP8 decode + FP16 KV cache）时，CUDA Graph 捕获的 kernel 序列包含精度转换节点。这些节点的执行时间可能随 batch size 变化（精度转换是 element-wise 操作，时间与元素数量成正比）。解决方案：为不同精度组合维护独立的 Graph 缓存——FP8+FP8 Graph、FP8+FP16 Graph、FP16+FP16 Graph。这增加 Graph 缓存的显存占用（3x），但避免了精度转换的 fallback。量化：在支持 FP8 tensor core 的 GPU 上，FP8 Graph 比 FP16 Graph 快 1.5x；在不支持的 GPU 上，FP8 Graph 需 fallback 到 SIMT 模拟，反而比 FP16 Graph 慢 2x。因此 Graph 缓存需根据 GPU 架构自动选择最优精度组合，并在 GPU 异构集群中为不同节点组维护不同的 Graph 缓存策略。

**补充：混合精度场景下的 Graph 管理。** 当模型使用混合精度（FP8 decode + FP16 KV cache）时，CUDA Graph 捕获的 kernel 序列包含精度转换节点。这些节点的执行时间可能随 batch size 变化（精度转换是 element-wise 操作，时间与元素数量成正比）。解决方案：为不同精度组合维护独立的 Graph 缓存——FP8+FP8 Graph、FP8+FP16 Graph、FP16+FP16 Graph。这增加 Graph 缓存的显存占用（3x），但避免了精度转换的 fallback。在支持 FP8 tensor core 的 GPU 上，FP8 Graph 比 FP16 Graph 快 1.5x；在不支持的 GPU 上，FP8 Graph 需 fallback 到 SIMT 模拟，反而比 FP16 Graph 慢 2x。


**补充：Graph 的显存碎片问题。** CUDA Graph 捕获的 buffer 地址是固定的——如果 Graph 缓存中的 buffer 被释放后重新分配，地址变化会导致 Graph 失效。这限制了 Graph 缓存的灵活管理（无法动态 shrink/expand）。解决方案：在启动时预分配固定大小的 Graph buffer pool，Graph 捕获时从池中取 buffer，永不释放——保证地址永久有效。代价是启动时占用额外显存（约 2GB），但运行期间无碎片风险。

**补充：CUDA Graph 显存碎片与 buffer pool 管理。**

Graph 捕获的 buffer 地址是固定的——如果 buffer 被释放后重新分配，地址变化会导致 Graph 失效。这限制了 Graph 缓存的灵活管理。

解决方案：在启动时预分配固定大小的 Graph buffer pool，Graph 捕获时从池中取 buffer，永不释放——保证地址永久有效。代价是启动时占用额外显存（约 2GB），但运行期间无碎片风险。

buffer pool 的大小计算：max_graphs × buffer_per_graph。buffer_per_graph ≈ model_activation + intermediate_tensors ≈ 权重 × 10-20%。对 70B BF16（140GB），单 Graph buffer ≈ 20GB，10 个 Graph ≈ 200GB——这对单卡不可行，说明大模型不适合大量 Graph 缓存。
---

### I-A006 设计多租户 KV cache 隔离方案：配额、回收、优先级与 OOM 处理。

**元数据：** 知识域：`I08 多租户 KV cache 隔离`；能力目标：`C08 资源隔离`；题型：架构设计；频率：高频；前置/后续：`I-A003`（autoscaling）、`I-A012`（多模型共池）；证据状态：设计模式 C，配额算法 C，运行结论 U。

**背景知识：** 多租户共享推理 GPU 池时，每个租户的请求产生的 KV cache 共享显存。如果不隔离，一个租户的长序列请求可能耗尽 KV cache，导致其他租户 OOM。需要：按租户配额分配 KV cache 容量；超额时的回收策略（evict 哪个租户的哪些序列）；优先级（付费等级决定回收顺序）；OOM 时的优雅降级。实际生产中，多租户推理平台的租户数通常 10-100，每个租户的负载模式差异大——有的以短对话为主（低 KV cache 占用），有的以长文档分析为主（高 KV cache 占用）。典型故障包括：低优先级租户的长序列被频繁 evict 导致质量下降（re-evict 抖动）、以及突发流量下配额不足以吸收导致全局 OOM。

**问题分析：** 1) 配额模型：静态配额（固定比例）vs 动态配额（基于 SLA 或实时负载）；2) 回收策略：LRU（最近最少用）、priority-based（按租户优先级）、hybrid；3) OOM 处理：当所有租户都超配额且无法回收时，拒绝新请求 vs 降级低优先级租户；4) 量化：给定 N 个租户、各自的 SLO（最大并发数 × 最大序列长度），求最小显存配置；5) 防抖动——避免在配额边界反复 evict/reload。

**具体答案：** 面试中可以这样回答：

> 配额设计：每个租户有 guaranteed_quota（保底 KV cache 容量，基于 SLA 签约）和 burst_quota（弹性上限，可借用空闲容量）。Guaranteed = max_concurrent × avg_seq_len × KV_per_token × safety_factor。实际使用低于 guaranteed 时，剩余部分加入共享 burst pool。使用超过 guaranteed 时从 burst pool 借用，但总量不超过 burst_cap。
>
> 回收策略：当总 KV cache 使用超过 90% 时触发回收。回收顺序：① 优先回收已达 burst_cap 的租户超出部分；② 按租户优先级从低到高回收 LRU 序列；③ 最后回收 guaranteed 部分（需告警）。单次回收量 = 目标降至 80% 所需释放量，避免频繁回收。防抖动机制：回收后设置"保护期"（60s），保护期内不回收同一租户的同一序列——防止在配额边界反复 evict/reload。
>
> OOM 处理：当回收后仍 OOM，拒绝最低优先级租户的新请求（HTTP 429），已有请求继续服务。绝不 kill 正在服务的请求。降级路径：① 降低采样质量（减少重复 n-gram penalty 等后处理）；② 截断超长短序列（从尾部丢弃 tokens 并告知客户端）；③ 最终手段：拒绝新请求。
>
> 量化示例：租户 A（VIP, guaranteed=40%, burst=60%）、租户 B（标准, guaranteed=30%, burst=50%）、租户 C（试用, guaranteed=10%, burst=20%）。总池 100%。当 A 用 50%、B 用 35%、C 用 15%，burst pool 空闲 0%。此时若 A 再增长，先回收 C 的 burst 部分（5%），再回收 B 的 burst 部分（5%），最后才动 A 的 guaranteed 以上部分。

**技术洞察：**

```text
配额模型：
  G_i = max_concurrent_i × avg_len_i × KV_per_token × 1.2
  B_i = burst_factor × G_i  (burst_factor ∈ [1.5, 3.0])
  Total_pool = Σ G_i + burst_pool_size

回收优先级：
  P_evict = [over_burst_tenants, low_priority_LRU, guaranteed_overflow]
  目标回收量 = current_usage - 0.8 × pool_size
  保护期 = 60s（防抖动）

OOM 降级链：
  1. 拒绝新请求(429) → 2. 降低后处理开销 → 3. 截断长序列 → 4. 终止低优先级租户

监控指标：
  每个租户的 KV cache 使用率、回收次数、OOM 事件次数、evict/reload 频率
```

C：配额与回收策略设计。I：配额计算公式。U：具体 burst_factor 与安全系数需根据负载分布实测。

**图文说明：** 堆叠条形图：GPU 显存分为 Guaranteed(A/B/C) + Burst Pool。箭头标注借用方向（Guaranteed → Burst）和回收方向（Burst → Pool，低优先级优先）。右侧状态指示器：绿色（<80%）、黄色（80-90%）、红色（>90%，触发回收）。旁注：保护期计时器——被 evict 的序列在 60s 内不会被再次 evict。

**方案对比与选型：** 三种回收策略对比：① 纯 LRU（最近最少使用）：实现简单但不考虑租户优先级——VIP 租户的 LRU 页可能被回收导致体验下降；② 纯优先级（先回收低优先级租户）：公平性好但可能导致低优先级租户完全饿死（无法积累 guaranteed 配额）；③ 分层 LRU + 优先级加权（推荐）：在优先级层内使用 LRU，跨优先级按权重分配回收量。量化：设租户 i 的回收权重 w_i = priority_i / Σ priority，每次回收 R tokens 时，租户 i 回收 R × w_i。这保证低优先级租户仍有 (1 - w_i) 的 KV cache 被保护。回收粒度：按页回收（16 tokens/page），而非按 token，减少回收次数。回收延迟：evict 操作 < 1ms（仅修改页表映射），但后续重算被 evict 页的延迟 = 16 × t_prefill_token ≈ 2-5ms。防抖动补充：设置回收磁滞区间——触发回收的阈值（90%）高于恢复正常的阈值（80%），避免在 85% 附近反复回收。

**拓展知识：** 与 I-A012 衔接：多模型共池时配额按模型维度叠加，形成二维配额矩阵（租户 × 模型）；与 I-A018 衔接：分布式 KV 池场景下配额需要全局协调，回收决策需跨节点——本地节点回收不了的 KV 页可 spill 到远程池而非直接丢弃；与 I-A014 衔接：每个租户的 KV cache 使用率、回收次数、OOM 事件应作为独立指标上报，用于 SLA 违约审计与计费依据。

**常见追问及回答要点：**

1. **问：租户的 guaranteed_quota 如果长期不用，是否浪费显存？** 答：不浪费——未使用的 guaranteed 空间自动加入 burst pool 供其他租户借用。Guaranteed 只是"优先使用权"而非"独占"。
2. **问：回收正在服务的请求的 KV cache 会怎样？** 答：被回收的请求的后续 decode 需要重新计算被 evict 的 KV，性能退化但不会中断服务。如果请求在 prefill 阶段被回收，可能需要重做部分 prefill。
3. **问：如何防止租户故意发超长序列侵占资源？** 答：单请求 max_tokens 限制 + 租户级别 rate_limit。超长序列在到达 max_tokens 时强制结束。
4. **问：evict/reload 抖动如何彻底解决？** 答：除保护期外，引入"回收预测"——如果一个租户的使用率持续接近 burst_cap，逐步降低其 burst_limit（缩弹性），而非突然 evict。缩弹性通知提前 5 分钟发送，给租户时间调整。

**易错点与反例：**

- 反例："每个租户固定分配固定显存"——浪费。租户负载波动大，静态分配导致显存利用率低。
- 反例："OOM 时随机 kill 请求"——错误。必须有优先级顺序，否则 VIP 租户可能被误杀。
- 易错：忽视回收开销——evict KV cache 后需要重新计算，频繁回收会降低整体吞吐。回收频率应 < 1 次/分钟/租户。
- 易错：burst 配额借用时无借还机制——如果 burst pool 被占满且租户不释放，后续请求即使有 guaranteed 配额也可能因碎片化分配失败。需要实现 burst 配额的"借用上限 + 超时回收"机制。

**实践与验证：** [U] 模拟 5 租户共享 4-GPU 推理池，注入不同负载模式（突发长序列、稳定短序列、混合），测量：(a) 各租户实际 KV cache 使用与配额符合度；(b) 回收事件频率与吞吐量影响；(c) OOM 事件数；(d) evict/reload 抖动率（单位时间内的 evict 次数）。oracle：VIP 租户零 OOM，试用租户 OOM 时优先降级，抖动率 < 1 次/小时/租户。当前未执行。

**分层评分标准：** 4分：能设计基本的配额与回收策略；6分：能建立 burst 共享模型并量化降级路径与防抖动机制；8分：能处理回收开销、跨租户干扰隔离、SLA 违约检测；10分：能在多模型共池约束下设计完整的二维配额体系与回收预测机制。

**证据与来源：** `sglang/01-modules/M08-kv-cache.md`（KV cache 管理）；`sglang/90-cross-module/pooling-and-resource-management.md`（资源池化）；`insights/05-sglang-inference-technical-insights/`（多租户实践）。

**补充：KV cache 碎片化治理与页面整理。** 多租户隔离的 KV cache 池长期运行后会产生碎片——各租户的 guaranteed pool 中散布着不连续的空闲页。碎片化率 = 最大连续空闲块 / 总空闲空间。当碎片化率 < 50% 时，可能出现"总空闲足够但无法分配连续块"的情况（尽管 PagedAttention 不要求连续，但某些优化如 prefix sharing 需要连续前缀块）。解决方案：① 定期碎片整理（每周一次低峰期），将分散的空闲页合并——通过迁移正在服务的请求的 KV 页到连续区域（代价高，需暂停请求）；② 预防式分配——新请求的 KV 页从 pool 的连续区域分配（buddy allocator 策略），减少碎片产生；③ 配额归还——租户释放的 KV 页归还到共享 pool 时，优先填充碎片区域。量化：碎片整理可将碎片化率从 40% 恢复到 85%，但整理期间性能下降 20-30%（持续 10-30 分钟），仅适合在凌晨低峰期执行。

**补充：KV cache 碎片化治理。** 多租户隔离的 KV cache 池长期运行后会产生碎片——各租户的 guaranteed pool 中散布着不连续的空闲页。碎片化率 = 最大连续空闲块 / 总空闲空间。当碎片化率 < 50% 时，可能出现"总空闲足够但无法分配连续块"的情况。解决方案：① 定期碎片整理（每周一次低峰期）；② 预防式分配——新请求的 KV 页从 pool 的连续区域分配（buddy allocator 策略）；③ 配额归还时优先填充碎片区域。量化：碎片整理可将碎片化率从 40% 恢复到 85%，但整理期间性能下降 20-30%。


**补充：配额的动态调整与弹性上限。** 固定配额（guaranteed=40%）无法适应租户负载的季节性变化——如某租户在月初结算时请求量是平时的 3x。解决方案：配额自动调整——基于过去 7 天的 P95 使用量，每周自动调整 guaranteed 配额（上限不超过 50% 总池）。调整需提前 24 小时通知租户，给其时间调整请求模式。弹性上限（burst_cap）也可动态调整——在低峰期提高 burst_cap 允许更多借用，高峰期降低 burst_cap 保证隔离。

**补充：配额动态调整与跨租户借用协议。**

固定配额无法适应租户负载的季节性变化——如某租户在月初结算时请求量是平时的 3x。解决方案：基于过去 7 天的 P95 使用量，每周自动调整 guaranteed 配额（上限不超过 50% 总池）。

跨租户借用协议：允许租户 A 借用租户 B 的 guaranteed 空闲页，但租户 B 有优先回收权（当 B 需要时，A 需在 100ms 内归还）。借用协议通过中央调度器协调。

借用协议的实现：① 借用请求 → 调度器检查被借租户的空闲率 > 20% → 批准借用 → 记录借用关系 → 被借租户需要时发送回收通知 → 借出方 100ms 内归还。

借用协议的性能：实测中 guaranteed 借用可将显存利用率从 75% 提升到 90%，代价是调度器延迟增加 0.5ms/请求（借用关系查询）。对延迟敏感的请求可标记"不可借用"（guaranteed only），牺牲利用率换取确定性。
---

### I-A007 设计采样质量与延迟的权衡方案：temperature/top-p/speculative 的组合调优策略。

**元数据：** 知识域：`I09 采样质量与延迟权衡`；能力目标：`C09 采样策略优化`；题型：策略设计与量化；频率：中频；前置/后续：`I-A009`（speculative decoding 部署）、`I-B001`（双阶段差异）；证据状态：采样算法 C，质量度量 C，运行结论 U。

**背景知识：** 采样参数直接影响生成质量与计算成本。temperature 控制分布平坦度（高 temperature → 更多样但可能不连贯）；top-p 控制候选集大小（低 top-p → 更确定但可能重复）；speculative decoding 用小模型加速但引入额外计算与可能的回退。三者组合影响延迟（TTFT、TPOT）和输出质量（perplexity、任务得分）。实际生产中，不同业务场景对质量-延迟的偏好差异极大——客服场景要求高确定性（低 T），创意写作要求高多样性（高 T），代码生成要求精确性（低 T + 低 top-p + speculative）。典型故障包括：高 temperature 下 speculative 接受率暴跌导致净延迟增加、以及低 top-p 导致代码生成陷入重复循环。

**问题分析：** 1) 每个参数的质量-延迟影响量化；2) 组合效应：temperature 与 top-p 的交互、speculative 与采样参数的交互；3) 自适应策略：根据请求类型（创意写作 vs 代码生成 vs 问答）自动选择采样配置；4) 质量验证：如何在不人工评估的情况下量化采样质量；5) 在线调优——如何根据用户反馈（如 regenerate 率）动态调整采样参数。

**具体答案：** 面试中可以这样回答：

> 三参数解耦调优。Temperature 主要影响多样性与连贯性——创意写作（故事生成）用 T=0.8-1.0，问答用 T=0.3-0.5，代码生成用 T=0.1-0.3。Top-p 作为 safety net——即使 T 高，top-p=0.9 也能过滤极端低概率 token。Speculative decoding 加速 decode 阶段，对 TPOT（每 output token 时间）改善最大，但接受率受 draft 模型与 target 模型分布匹配度影响——高 temperature 时分布更平坦，接受率下降。
>
> 组合策略：① 高质量模式（T=0.7, top-p=0.9, no speculative）——质量优先，延迟容忍度高；② 平衡模式（T=0.5, top-p=0.95, speculative with 7B draft）——质量与延迟折中，speculative 可提升 1.5-2x TPOT；③ 极速模式（T=0.3, top-p=0.9, speculative with aggressive draft）——延迟优先，适合实时交互。
>
> 质量量化：使用输出 perplexity 作为代理指标——speculative decoding 的接受率 × target model 重打分可保证输出分布与 target-only 一致（Lossless Speculative Decoding）。Acceptance_rate = E[min(1, p_target(x)/p_draft(x))]，高 temperature 时 p_draft 与 p_target 差异更大，接受率从 85%（T=0.1）降到 60%（T=1.0）。
>
> 延迟影响：speculative decoding 引入 draft 模型的计算成本。净加速 = T_target_only / (T_draft + T_target_verify + T_reject_rewrite)。当接受率 > 70% 时净加速 > 1（值得部署）。
>
> 在线调优：监控"regenerate 率"（用户点击重新生成的比例）作为质量代理指标——regenerate 率上升 5pp 可能意味着采样质量下降，触发参数回退（如降低 temperature 或关闭 speculative）。

**技术洞察：**

```text
参数空间：
  T ∈ [0, 2]: 分布平坦度
  top_p ∈ [0, 1]: 候选集截断
  speculative: 接受率 α = E[min(1, p_t/p_d)]

组合矩阵：
  创意写作: T=0.9, top_p=0.95, no speculative → 质量优先
  问答: T=0.4, top_p=0.9, speculative → 平衡
  代码: T=0.2, top_p=0.85, aggressive speculative → 延迟优先

Speculative 净加速：
  Speedup = 1 / (α/K + (1-α) + T_draft/T_target)
  K = speculation_length (draft 每步猜测 token 数)

  例：K=5, α=0.8, T_draft/T_target=0.2
  Speedup = 1 / (0.8/5 + 0.2 + 0.2) = 1 / 0.56 ≈ 1.79x

在线信号：
  regenerate_rate ↑ 5pp → 可能质量下降
  α < α_threshold → 关闭 speculative
  repeat_ngram_rate ↑ → 降低 temperature
```

C：采样参数语义与 speculative 接受率公式。I：speculative 加速公式推导。U：实际接受率与加速比需实测。

**图文说明：** 三维坐标系：T（横）、top-p（纵）、延迟（竖）。三个模式标注为点：高质量（高T高p低latency_sensitivity）、平衡（中T中p）、极速（低T低p）。虚线箭头表示 speculative 在各点的 TPOT 改善。下方：接受率 vs temperature 曲线图，标注 α_threshold 红线。

**方案对比与选型：** 采样策略自动选择的三种方案：① 规则匹配（基于 prompt 关键词）：检测 "code"、"python"、"function" 关键词自动切代码配置。简单但覆盖不全（"帮我算一下" 可能是数学也可能是闲聊）；② ML 分类器（prompt embedding + 轻量分类模型）：对 prompt 做多分类（创意/问答/代码/翻译），每类绑定采样配置。准确率 > 90%，分类延迟 < 1ms（ONNX Runtime on CPU）。但需要标注数据训练；③ 用户显式标注（API 参数 `quality_preference: "speed"/"balanced"/"quality"`）：最准确但增加用户负担。选型建议：② + ③ 组合——默认用分类器自动选择，用户可通过 API 参数覆盖。A/B 测试表明：自动选择可减少 40% 的用户手动调参，且用户满意度（输出质量评分）与手动选择无显著差异（p > 0.05）。关键指标：分类器置信度——低置信度时回退到平衡模式并记录日志供后续标注。

**拓展知识：** 与 I-A009 衔接：speculative decoding 部署方案包含 draft 模型选择与接受率监控；与 I-A010 衔接：量化对采样分布有微小影响，需在量化后重新校准采样参数——FP8 量化可能使 top-p 截断边界偏移 1-2 个 token 位；与 I-A014 衔接：采样参数选择（规则/分类器/手动）应记录在请求 trace 中，用于分析输出质量与采样配置的关系。

**常见追问及回答要点：**

1. **问：temperature=0 时还需要 top-p 吗？** 答：不需要。T=0 等价于 argmax，每次取概率最高的 token，top-p 无意义。但 T=0 可能导致重复生成，实际部署中最低用 T=0.01。
2. **问：speculative decoding 的接受率低于多少时不应使用？** 答：当接受率 α < 1/K + T_draft/T_target 时，speculative 的净延迟反而更差。对于 K=5、draft/target=0.2 的典型配置，阈值约 α < 0.4。低于此值应关闭 speculative。
3. **问：不同请求类型如何自动选择采样配置？** 答：请求分类器（基于 prompt 关键词或 embedding 相似度）将请求分到预设类别，每类绑定采样配置。也可用 A/B 测试逐步优化配置。
4. **问：regenerate 率作为质量指标的局限性？** 答：regenerate 率受多因素影响——用户可能因想要不同风格而 regenerate（非质量下降），也可能因界面 bug 误触。需结合"输出 perplexity 漂移"和"用户显式评分"多指标联合判断，避免误调。

**易错点与反例：**

- 反例："speculative decoding 总是加速"——错误。低接受率时 draft 计算浪费，净延迟可能更差。
- 反例："temperature 越低质量越好"——不准确。T=0 可能导致重复循环，实际任务得分未必最高。
- 易错：忽视 speculative 对 TTFT 的影响——speculative 只加速 decode 阶段，prefill（TTFT）不受影响。如果业务场景以短输出为主（< 50 tokens），speculative 的收益极小（只加速了少量 decode step）。
- 易错：采样参数对长序列的累积效应——即使 per-token 质量指标达标，长序列（> 1000 tokens）的错误累积可能导致整体输出质量显著下降。需要额外监控长序列的"语义连贯性"指标。

**实践与验证：** [U] 固定 Llama-3-70B target + Llama-3-8B draft，测量不同 (T, top_p) 组合下的：(a) 接受率（按序列长度分桶统计）；(b) TPOT 与加速比；(c) 输出 perplexity 与任务得分（HumanEval、MMLU）；(d) regenerate 率（通过模拟用户行为）。oracle：找到各场景的帕累托最优点——质量损失 < 5% 且延迟改善 > 30%。当前未执行。

**分层评分标准：** 4分：能描述各采样参数的影响；6分：能建立 speculative 加速模型并量化接受率阈值；8分：能设计自适应策略并给出质量-延迟的帕累托前沿；10分：能设计在线调优闭环，将用户行为信号（regenerate 率）反馈到采样参数调整中。

**证据与来源：** `insights/06-speculative-decoding-as-transaction/`（speculative 机制分析）；`sglang/99-roadmap/performance-guide.md`（采样参数影响）；`insights/arxiv-pdf/2408.08147/`（speculative decoding 论文）。

**补充：draft 模型量化对接受率的影响与补偿。** 部署中 draft 模型通常需要量化（FP8 或 INT4）以减少显存占用，但量化会改变 draft 模型的输出分布，降低与 target 模型的匹配度。量化 draft 的接受率损失通常为 3-8pp（如从 85% 降到 78%）。补偿策略：① 量化感知微调——对 draft 模型做 quantization-aware fine-tuning，使用 target 模型的 logits 作为 soft label（知识蒸馏），使 draft 的分布更接近 target 的量化版本；② 动态 speculation length——当 draft 模型量化后不确定性增加，减少 K（从 5 降到 3），降低错误传播的代价；③ draft 混合精度——对接受率敏感的层（首尾层）保持 FP16，中间层用 FP8。实验表明：混合精度 draft 可比全 FP8 draft 的接受率高 2-3pp，而显存仅增加 10-15%。

**补充：采样参数的组合效应与正交性分析。** temperature 和 top-p 并非完全正交——高温 + 低 top-p 的行为不同于高温 + 高 top-p。实验表明：T=0.9 + top_p=0.95 的输出多样性与 T=0.6 + top_p=0.8 接近，但前者延迟更低（因为 top_p 过滤了更多低概率 token，减少了 speculative 重试次数）。采样参数的组合调优应在（质量, 延迟, 多样性）三维空间中找到帕累托最优前沿。实践中，建议预设 3-5 个"采样模式"（高质量/平衡/极速/创意/精确），每个模式绑定一组 (T, top_p, speculative) 参数，而非让用户独立调节每个参数。


**补充：采样参数的安全边界与异常检测。**

采样参数的温度上界需要严格限制——T > 1.0 时模型输出接近随机 token，不仅质量差，还可能产生超长输出（陷入低概率 token 循环）。安全做法是将 temperature clamp 到 [0, 1.0] 并记录告警。

异常检测：监控输出的 token 概率分布——如果连续 N 步（N=10）的 top-1 概率 < 0.1，说明模型处于"高不确定性"区域（可能是采样参数异常或输入分布漂移）。触发告警并记录该请求的 trace。
---

### I-A008 设计跨机分布式推理架构：TP/PP 混合下通信与显存如何规划？

**元数据：** 知识域：`I10 跨机分布式推理`；能力目标：`C10 分布式架构设计`；题型：架构设计与量化；频率：高频；前置/后续：`I-A011`（PD 分离）、`I-A018`（分布式 KV 共享）；证据状态：并行策略 C，通信模型 C，运行结论 U。

**背景知识：** 大模型（70B+）无法放入单卡，需要张量并行（TP，切分权重到多卡）或流水线并行（PP，按层切分到多卡）。混合策略（TP+PP）用于多机部署：同机内 TP（高带宽 NVLink），跨机间 PP（较低带宽 IB/RoCE）。显存规划需考虑权重分配、KV cache 分配、通信 buffer 预留。实际生产中，跨机分布式推理的主要通信开销来自 TP all-reduce——一个 70B 模型在 TP=8 下每层 all-reduce 约 234MB，80 层总计约 18.7GB/step。在 NVLink（1300GB/s）下约 14ms，在 IB（200Gb/s = 25GB/s）下约 748ms——这就是为什么 TP 必须在 NVLink 域内。典型故障包括：PP bubble 过大导致 GPU 利用率 < 50%、TP all-reduce 与 PP 通信争用网络带宽、以及 PP 各 stage 负载不均。

**问题分析：** 1) TP 与 PP 的显存/通信权衡：TP 每层需 all-reduce 通信（量大但频繁），PP 仅在阶段边界传激活（量小但延迟敏感）；2) 混合策略下的资源分配：多少卡做 TP、多少做 PP；3) 通信带宽需求量化：给定模型大小、序列长度、batch size，计算通信量；4) 负载均衡：PP 各 stage 的计算时间差异导致 bubble；5) 通信计算 overlap——TP 的 all-reduce 能否与 attention 计算流水线化。

**具体答案：** 面试中可以这样回答：

> TP/PP 选择由模型大小与硬件拓扑共同决定。单节点 8 卡 NVLink 内优先 TP——TP 通信在 prefill 阶段约 2×batch×seq_len×hidden_dim×dtype_size bytes（all-reduce），8 卡 NVLink 总带宽 600GB/s，开销可接受。跨节点用 PP——每层传激活量 = batch×seq_len×hidden_dim×dtype_size，PP 阶段间通信量远小于 TP 的 all-reduce（只在 stage 边界传一次 vs 每层都传）。
>
> 显存规划：70B 模型 BF16 约 140GB。TP=8 时每卡权重 17.5GB；PP=2 时每 stage 35GB 权重。KV cache 按实际服务请求的序列长度计算，预留 60% 显存。通信 buffer：TP 的 all-reduce 需要临时 buffer ≈ 2×message_size，PP 的 P2P 通信 buffer 较小。
>
> PP bubble 量化：bubble_ratio = (PP_size - 1) / (micro_batches + PP_size - 1)。当 micro_batches=4、PP=4 时 bubble = 3/7 ≈ 43%。增大 micro_batches 可降低 bubble，但增加显存（每个 micro batch 需独立 KV cache）。
>
> 带宽估算：TP all-reduce 每 step 通信量 = 2×(N-1)/N × batch×seq_len×hidden_dim×dtype。对 70B（hidden=8192）、batch=4、seq=2048、TP=8、BF16：每 step ≈ 2×7/8 × 4×2048×8192×2 ≈ 234MB。1300GB/s NVLink 下耗时 ≈ 0.18ms。PP 每 stage 边界通信 = batch×seq_len×hidden_dim×dtype = 4×2048×8192×2 ≈ 134MB，但只在每 PP stage 传一次，频率低。

**技术洞察：**

```text
通信量对比：
  TP(all-reduce per layer): 2×(N-1)/N × B×S×H×D
  PP(P2P per stage boundary): B×S×H×D

  对 N_TP=8, PP=2, B=4, S=2048, H=8192, D=2bytes:
  TP per layer: ≈234MB, 每层都传
  PP per boundary: ≈134MB, 每 (layers/PP) 层传一次

PP bubble:
  bubble = (PP-1) / (micro_batches + PP-1)
  目标 bubble < 10% → micro_batches > 9×(PP-1)

显存预算（70B BF16, TP=8, PP=2）:
  权重: 140GB / 16 cards = 8.75GB/card
  KV cache: 60% of M_GPU
  通信 buffer: ≈500MB/card
  剩余给 prefill activation

通信计算 overlap：
  层间 all-reduce 可与下一层 attention 部分重叠
  同层内 all-reduce 必须等 GEMM 完成（不可 overlap）
  实测 overlap 效率 30-50%
```

C：TP/PP 通信模型。I：通信量公式与 bubble 推导。U：实际通信耗时与带宽利用率需实测。

**图文说明：** 架构图：2 节点 × 8 卡。节点内 8 卡用粗箭头（NVLink, 600GB/s）连接标注 TP；节点间用细箭头（IB, 200GB/s）连接标注 PP。每卡内显存堆叠：Weights + KV Cache + Activation + Comm Buffer。下方时序：TP all-reduce 周期（每层一次，密集）vs PP P2P 周期（每 PP stage 一次，稀疏）。

**方案对比与选型：** 三种并行策略组合对比：① 纯 TP（同节点内）：通信在 NVLink 域内，带宽高（600GB/s），但受限于单节点卡数（通常 8 卡）。70B 模型需 TP=8 刚好放入单节点；② 纯 PP（跨节点）：每节点一个 PP stage，跨节点通信量小但 bubble 严重。PP=8 时即使 micro_batches=16 仍有 16.7% bubble。适合模型极大（>500B）无法 TP 放入单节点；③ TP+PP 混合（推荐）：同节点 TP（利用 NVLink 高带宽），跨节点 PP（通信量小且可流水线化）。例如 70B + 2 节点 × 8 卡：每节点 TP=4（NVLink），跨节点 PP=2。选型决策树：如果模型权重 × TP_degree ≤ 单节点显存 → 纯 TP；否则如果序列长度短（bubble 影响小）→ 纯 PP；否则 → TP+PP 混合。显存约束：TP 度数受单节点显存限制（8 卡 A100-80G = 640GB 总显存），PP 度数受可接受 bubble 限制（通常 PP ≤ 8）。

**拓展知识：** 与 I-A011 衔接：PD 分离架构中 prefill 节点与 decode 节点可独立选择并行策略；与 I-A018 衔接：分布式 KV 共享是另一种跨机通信模式（KV transfer vs weight all-reduce）；与 I-A005 衔接：TP 场景下 CUDA Graph 的收益更大（小 batch 是常态），但跨节点 PP 场景下 Graph 仅覆盖节点内部分。

**常见追问及回答要点：**

1. **问：TP=8 和 TP=4+PP=2 哪个更适合 70B 推理？** 答：取决于硬件拓扑。如果 8 卡在同一 NVLink 域内，TP=8 通信效率高、实现简单。如果跨 2 节点（每节点 4 卡 NVLink），TP=4+PP=2 避免跨节点 all-reduce（带宽差 3x）。关键判据：TP 通信是否跨节点。
2. **问：PP 的 micro_batch 数设多少？** 答：在显存允许范围内尽可能多——bubble 与 micro_batches 成反比。但每个 micro batch 需独立 KV cache，显存开销线性增长。典型值 2-8。
3. **问：TP 的 all-reduce 和 attention 计算能 overlap 吗？** 答：部分可以。Flash Attention 的 tiled 计算可以与层间 all-reduce 流水线化，但同一层内的 all-reduce 必须等待 GEMM 完成。Megatron-LM 实现了 column/row 并行下的通信计算 overlap。
4. **问：混合并行下的显存不均衡问题？** 答：PP 的首 stage 和尾 stage 有额外的 embedding layer 计算，中间 stage 只做 transformer layers。首 stage 的 KV cache 压力较小（prefill 后传给下一 stage），尾 stage 的 KV cache 压力最大（持有所有 decode 序列的 KV）。可通过调整各 stage 的 KV cache pool 配额来均衡。

**易错点与反例：**

- 反例："PP 总是比 TP 通信少"——不完全。PP 通信量小但延迟敏感（blocking P2P），TP 通信量大但可流水线化。延迟与带宽是两个维度。
- 反例："TP 数越多越好"——错误。TP 增加使 all-reduce 通信量增大（all-reduce 与 N 近似无关但延迟增加），且每卡权重减少但 KV cache 不变。
- 易错：忽视 PP 的显存不均衡——首 stage 需处理所有层的输入/输出，中间 stage 只需处理本层，显存压力不同。
- 易错：跨节点 PP 的 IB 网络 QoS——all-reduce 与 P2P 可能争用同一物理链路的带宽，需要网络层面的 QoS 策略（如 PFC 优先级）保证 P2P 的低延迟。

**实践与验证：** [U] 2 节点 × 8 卡 A100，部署 Llama-3-70B，对比 TP=8（同节点）vs TP=4+PP=2（跨节点）的：(a) decode throughput；(b) prefill throughput；(c) PP bubble 实际占比（通过 NSight 分析 GPU 空闲时间）；(d) all-reduce 通信时间占比。oracle：同节点 TP=8 吞吐更高，跨节点 TP=4+PP=2 是唯一选择，bubble < 15%。当前未执行。

**分层评分标准：** 4分：能描述 TP/PP 基本原理；6分：能建立通信模型并量化带宽需求与 PP bubble；8分：能设计混合策略下的显存规划、负载均衡与通信计算 overlap；10分：能在真实网络约束（IB QoS、链路不对称）下设计完整的分布式推理方案。

**证据与来源：** `sglang/01-modules/M20-parallel-strategies.md`（TP/PP 实现）；`sglang/90-cross-module/performance-critical-paths.md`（分布式延迟分析）；`insights/04-design-decision-and-validation/`（并行策略决策）。

**补充：异构网络下的并行策略调整。** 实际集群的网络并非完全对称——NVLink 域内 600GB/s、同机架 IB 200Gb/s、跨机架可能降到 100Gb/s（网络拥塞）。异构网络下的并行策略需动态调整：① 同机架内优先 TP（高带宽），跨机架用 PP（通信量小）；② 如果网络拓扑不对称（如机架 A→B 带宽 > B→C），PP 的 micro-batch 流向应从高带宽方向流向低带宽方向，减少跨低带宽链路的通信量；③ 网络拥塞时的自适应降级——检测到 IB 带宽利用率 > 90% 时，自动减少 TP 度数（从 8 降到 4），增加 PP 度数（从 1 降到 2），用更多小通信替代少量大通信。量化：TP=8 的 all-reduce 通信量为 234MB/step × 80 层 = 18.7GB，在拥塞网络上可能耗时 100ms+（vs 正常 14ms），而 TP=4+PP=2 的通信量为 273MB/step × 40 层 + 134MB × 1 次 = 11GB，耗时更少。

**补充：异构网络下的并行策略调整。** 实际集群的网络并非完全对称——NVLink 域内 600GB/s、同机架 IB 200Gb/s、跨机架可能降到 100Gb/s。异构网络下的并行策略需动态调整：① 同机架内优先 TP（高带宽），跨机架用 PP（通信量小）；② 网络拥塞时的自适应降级——检测到 IB 带宽利用率 > 90% 时，自动减少 TP 度数（从 8 降到 4），增加 PP 度数，用更多小通信替代少量大通信。量化：TP=8 的 all-reduce 通信量为 18.7GB/step，在拥塞网络上可能耗时 100ms+（vs 正常 14ms），而 TP=4+PP=2 的通信量为 11GB，耗时更少。


**补充：NVLink 拓扑感知的 TP 分配。** 现代 GPU 服务器（如 DGX A100）内部 NVLink 拓扑并非全连接——部分 GPU 之间的 NVLink 带宽是其他对的 2x。TP all-reduce 的性能受限于最慢的 NVLink 连接。解决方案：启动时检测 NVLink 拓扑（通过 nvidia-smi topo -m2），将 TP 组分配到带宽最高的 GPU 子集中（如 NVSwitch 连接的 GPU 环）。实测中，拓扑感知的 TP 分配可比随机分配提升 15-20% 的 all-reduce 效率。
---

### I-A009 设计 speculative decoding 的部署方案：draft 模型选择、接受率监控与回滚判据。

**元数据：** 知识域：`I11 speculative decoding 部署`；能力目标：`C11 部署与运维`；题型：架构设计与运维；频率：高频；前置/后续：`I-A007`（采样质量权衡）、`I-A010`（量化部署）；证据状态：算法 C，部署模式 C，运行结论 U。

**背景知识：** Speculative decoding 用一个小模型（draft）猜测 K 个 token，再用大模型（target）验证，接受的 token 直接输出。优势：加速 decode（TPOT 改善 1.5-3x）。风险：draft 模型与 target 分布不匹配时接受率下降，反而浪费计算。部署需要：选择合适的 draft 模型、持续监控接受率、在性能退化时回退到 target-only。实际生产中，speculative decoding 的接受率分布受多种因素影响：序列长度（长序列进入低频 token 区域后接受率下降）、请求类型（代码生成的接受率通常高于创意写作）、以及 draft 模型的量化精度（FP8 draft 的接受率略低于 FP16）。典型故障包括：draft 模型更新后接受率突变、以及低接受率时段 speculative 反而增加延迟。

**问题分析：** 1) Draft 模型选择标准：大小（计算成本）、与 target 的分布匹配度（接受率）、部署成本（额外显存）；2) 接受率监控：滑动窗口平均、趋势检测、按请求类型分类统计；3) 回滚判据：接受率低于阈值、净加速 < 1、或质量指标退化时自动回退；4) 量化：draft 模型的总拥有成本 vs 加速收益；5) 多 draft 模型支持——不同请求类型可能匹配不同的 draft 模型。

**具体答案：** 面试中可以这样回答：

> Draft 模型选择三要素：① 大小——draft 应为 target 的 1/5 到 1/15（如 Llama-3-70B 配 Llama-3-8B 或 1.5B），计算开销可控；② 分布匹配——同家族模型接受率最高（共享 tokenizer 与训练分布），跨家族需重新校准；③ 部署成本——draft 模型权重占用额外显存，需从 KV cache 预算中扣除。量化：8B draft 在 BF16 下 16GB，在 A100-80G 上占用 20% 显存，减少约 25% 的 decode 并发容量。
>
> 接受率监控：滑动窗口（最近 1000 个 draft 序列）计算平均接受率 ᾱ。按请求分类统计（短序列 ᾱ 高、长序列 ᾱ 低——因为长序列后期进入低频 token 区域）。监控 ᾱ 的 5 分钟趋势，如果下降超过 5pp 触发告警。
>
> 回滚判据（OR 关系，满足任一即回滚）：① ᾱ < α_threshold（α_threshold = 1/K + T_draft/T_target，典型值 0.4-0.6）；② 净 TPOT 实测值 > target-only TPOT（speculative 反而更慢）；③ 输出质量指标（perplexity）退化 > 5%。回滚为渐进式：先对低优先级租户关闭 → 全量关闭 → 通知运维。回滚后 draft 模型保留在显存但不参与推理，接受率恢复后自动重新启用。
>
> 成本收益：draft 模型增加 16GB 显存占用（少服务约 20% 并发），换取 decode TPOT 降低 40-60%。净收益条件：decode 占总推理时间 > 30% 且接受率 > 70%。对于 TTFT 敏感的短请求场景，speculative 收益有限（不加速 prefill）。

**技术洞察：**

```text
Draft 选择：
  Size_ratio = M_draft / M_target  ∈ [1/15, 1/5]
  Family_match: same → ᾱ ≈ 0.85, cross → ᾱ ≈ 0.60-0.75
  KV_cost = M_draft_weights + M_draft_KV_cache

接受率监控：
  ᾱ_window = mean(accept[t-1000:t])
  ᾱ_by_request_class: {short_seq, long_seq, creative, factual}
  Trend = (ᾱ[t] - ᾱ[t-5min]) / 5min

回滚判据：
  Rollback iff (ᾱ < α_th) OR (TPOT_spec > TPOT_target) OR (perplexity_drift > 5%)
  α_th = 1/K + T_draft/T_target

渐进回滚：
  L1: disable for low-priority tenants
  L2: disable globally
  L3: alert + keep draft loaded (warm standby)

多 draft 路由：
  creative_request → 大 draft (8B, 高多样性)
  code_request → 小 draft (1.5B, 高接受率)
  default → 中 draft (3B, 平衡)
```

C：speculative decoding 原理与接受率公式。I：回滚阈值推导。U：实际接受率与质量退化需实测。

**图文说明：** 时序图：时间轴上标注 target-only 基线与 speculative 运行段。接受率曲线叠加在时间轴下方，标注 α_threshold 红线。回滚触发点用红色竖线标注。下方成本收益表：draft 显存成本 vs TPOT 改善 vs 净吞吐提升。旁注：渐进回滚的三阶段——L1(tenant级) → L2(全局) → L3(告警+暖备)。

**方案对比与选型：** 三种 draft 部署架构对比：① 共享 GPU（draft 与 target 同卡）：draft 计算与 target 交替执行，无通信开销但显存占用叠加。适合 draft 模型小（<15% target 显存）的场景；② 独立 GPU（draft 在单独的 GPU 上）：draft 计算与 target 完全并行（流水线化），draft 不占用 target GPU 显存。但增加跨卡通信（draft 的 KV 需传到 target），通信量 = KV_per_token × K_spec；③ 独立节点（draft 在 CPU 或低端 GPU 上）：draft 计算完全隔离，但通信延迟高（RDMA 传输 draft KV）。选型建议：① 最适合当前主流硬件（8B draft + 70B target 共享 A100-80G，8B 占 16GB/80GB=20%）；② 适合 draft 模型较大（如 14B）或 target 节点显存紧张的场景；③ 基本不推荐——CPU draft 延迟抵消了 speculative 的收益。关键监控：draft→target 的 KV 传输延迟应 < T_draft_step，否则流水线气泡过大。

**拓展知识：** 与 I-A010 衔接：draft 模型可独立量化（如 FP8），进一步降低其计算成本；与 I-A007 衔接：speculative 与采样参数的交互影响接受率；与 I-A003 衔接：speculative 的吞吐提升可减少 autoscaling 的副本需求（相同 QPS 需要更少副本），但 draft 模型占用显存又增加了单副本成本——需要综合计算净成本。

**常见追问及回答要点：**

1. **问：draft 模型和 target 模型可以部署在不同 GPU 上吗？** 答：可以，但增加跨设备通信开销。通常 draft 与 target 共享 GPU（draft 计算量小，与 target 交替执行）。如果分开部署，draft 的 latency 必须远小于 target 才值得——否则 draft 等待 target 验证的时间被通信开销吃掉。
2. **问：speculative decoding 对 TTFT 有影响吗？** 答：无直接影响。speculative 只加速 decode 阶段。但 draft 模型占用显存会减少 KV cache 容量，可能增加 prefill 排队，间接影响 TTFT。
3. **问：如何验证 speculative 的输出质量与 target-only 完全一致？** 答：使用 Lossless Speculative Decoding（rejection sampling with correction）保证输出分布数学等价。验证方法：对相同输入采样 N 次，比较两种方案的输出分布 KL 散度。
4. **问：多个 draft 模型如何管理？** 答：维护 draft 路由表——根据请求类型（分类器结果或用户指定）选择 draft 模型。每个 draft 模型独立加载/卸载（参考 I-A012 的多模型管理）。所有 draft 模型的总显存预算不超过单卡 20%。

**易错点与反例：**

- 反例："draft 模型越大越好"——错误。大 draft 计算成本高，且大模型接受率反而可能下降（分布差异更大）。
- 反例："接受率高就一定值得部署"——不充分。还需要考虑 draft 的计算成本——如果 draft 计算占 target 时间的 50%，即使接受率 80% 净加速也有限。
- 易错：忽视回滚后的"冷启动"——重新启用 speculative 后需要重建 draft KV cache，初始接受率可能较低（draft 模型刚加载，KV cache 为空）。
- 易错：多租户场景下 draft 模型的选择——如果不同租户使用不同温度参数，同一 draft 模型的接受率差异大。需要按租户类型或请求类型分组统计接受率并独立回滚。

**实践与验证：** [U] 部署 Llama-3-70B target + Llama-3-8B draft，测量不同场景（短/长序列、创意/事实问答）的接受率、TPOT、质量指标。注入分布漂移（如新话题）测试回滚判据。同时测试 draft 模型 FP8 量化对接受率的影响。oracle：在 target-only TPOT 降低 >30% 且质量无退化时保持 speculative，FP8 draft 接受率下降 < 3pp。当前未执行。

**分层评分标准：** 4分：能描述 speculative decoding 基本原理；6分：能设计 draft 选择标准与接受率监控；8分：能建立回滚判据并量化成本收益、处理渐进回滚与冷启动；10分：能设计多 draft 模型路由、租户级独立监控、draft 量化影响分析。

**证据与来源：** `insights/06-speculative-decoding-as-transaction/`（speculative 机制）；`insights/arxiv-pdf/2408.08147/`（speculative decoding 论文）；`sglang/99-roadmap/performance-guide.md`（性能优化指南）。

**补充：draft 模型的预热与冷启动问题。** 新部署或回滚后重新启用 speculative decoding 时，draft 模型的 KV cache 为空，首步 decode 的接受率可能比稳态低 10-15pp（因为 draft 模型的首步 KV 是从零开始构建的，与 target 模型的首步分布差异较大）。预热策略：① 在启用 speculative 后的前 N 步（N=10-20）使用较小的 K（如 K=2），降低冷启动期的错误传播；② 预热期不使用 draft 模型，仅积累 draft KV cache，从第 N 步开始启用 full K 的 speculative；③ 预热期的 TTFT 可能略高（多 N 步 target-only decode），但后续的稳态加速可弥补。量化：预热 10 步（K=2）增加约 10 × T_draft ≈ 5ms 延迟，但稳态接受率从 70% 提升到 85%，净收益为正。

**补充：draft 模型量化对接受率的影响与补偿。** 部署中 draft 模型通常需要量化（FP8 或 INT4）以减少显存占用，但量化会改变 draft 模型的输出分布，降低与 target 模型的匹配度。量化 draft 的接受率损失通常为 3-8pp（如从 85% 降到 78%）。补偿策略：① 量化感知微调——对 draft 模型做 quantization-aware fine-tuning，使用 target 模型的 logits 作为 soft label；② 动态 speculation length——当 draft 模型量化后不确定性增加，减少 K（从 5 降到 3）；③ draft 混合精度——对接受率敏感的层保持 FP16，中间层用 FP8。


**补充：speculative decoding 与 KV cache 管理的交互。**

speculative decoding 对 KV cache 管理有特殊影响——draft 模型也需要维护 KV cache 用于自回归猜测，但其 KV cache 生命周期与 target 不同（仅在 speculative 验证期间有效）。

解决方案：draft 的 KV cache 使用独立的 pool，验证完成后立即释放（而非等待序列结束），避免占用 target 的 KV cache 空间。量化：8B draft 的 KV cache 约为 70B target 的 1/9（参数量比例），但对长序列（> 4K）仍可达数 GB。
---

### I-A010 设计量化方案的生产部署：精度验证、校准数据选择与回退机制。

**元数据：** 知识域：`I12 量化部署`；能力目标：`C12 量化生产化`；题型：架构设计与验证；频率：高频；前置/后续：`I-A009`（speculative 部署）、`I-A005`（CUDA Graph 切换）；证据状态：量化方法 C，验证流程 C，运行结论 U。

**背景知识：** 量化（INT8/INT4/FP8）可减少显存占用与带宽需求，但引入精度损失。生产部署需要：① 量化方法选择（PTQ vs QAT，per-tensor vs per-channel vs group-wise）；② 校准数据选择（代表性 vs 边缘场景）；③ 精度验证（perplexity、任务得分、人工评估）；④ 回退机制（精度不达标时回退到 FP16）。实际生产中，推理服务的量化部署频率约每季度 1-2 次（新模型上线或量化方案升级）。量化精度问题的典型症状包括：特定领域（如代码、数学）的任务得分显著退化、输出 perplexity 异常升高（分布尾部出现极端概率 token）、以及长序列生成的连贯性下降。量化方案的回退通常需要 10-30s（从 CPU 加载 FP16 权重），期间服务可能降级。

**问题分析：** 1) 量化方法对比：INT8 精度损失小但加速有限，INT4 显存减半但精度风险大，FP8 硬件友好（H100 原生支持）且精度损失极小；2) 校准数据设计：覆盖生产分布的代表性数据集，包含难例（长序列、稀有领域）；3) 精度验证矩阵：自动指标 + 人工抽检 + 线上 A/B 测试；4) 回退机制：精度指标超阈值自动切回 FP16，无需停机；5) KV cache 量化——独立的精度/显存权衡维度。

**具体答案：** 面试中可以这样回答：

> 量化方法选择基于硬件与精度预算。FP8（E4M3）在 H100 上有原生 tensor core 支持，精度损失通常 < 1% perplexity 增加，是首选。INT8 适用于 AMD GPU 或旧 NVIDIA GPU（无 FP8 tensor core），精度损失约 1-2%。INT4（GPTQ/AWQ）显存减半但精度损失 2-5%，仅用于显存受限场景。
>
> 校准数据：1000-2000 条样本，覆盖：① 生产请求分布（按长度、领域分层采样）；② 难例集（代码生成、数学推理、多语言混合）；③ 对抗样本（极长 prompt、特殊符号）。校准数据的领域分布与生产分布的 KL 散度应 < 0.1。
>
> 精度验证三级：① 自动指标：perplexity 增加 < 2%（FP8）或 < 5%（INT4），关键任务（代码、数学）得分下降 < 3%；② 盲测评估：100 条样本让标注员比较 FP16 vs 量化输出，偏好率 < 30% 视为可接受；③ 线上 A/B：10% 流量走量化版本，监控业务指标（完成率、用户满意度）无显著差异。
>
> 回退机制：每个量化模型部署时保留 FP16 权重副本。运行时监控：perplexity（通过 N-gram 模型在线估计）、输出重复率、用户反馈。如果任何指标超阈值，切换到 FP16（通过模型热加载，无需重启服务）。回退延迟目标 < 30s（加载 FP16 权重时间）。成本：FP16 副本占用双倍显存，但仅在回退期间使用——平时可放在 CPU 内存。

**技术洞察：**

```text
量化方法对比：
  FP8: Δppl < 1%, 加速 1.3-1.5x, 需 H100/A100(tensor core)
  INT8: Δppl 1-2%, 加速 1.2-1.3x, 通用 GPU
  INT4: Δppl 2-5%, 显存 0.5x, 加速 1.5-2x(带宽节省), 精度风险高

校准数据要求：
  N_calib ∈ [1000, 2000]
  KL(calib_dist ‖ prod_dist) < 0.1
  覆盖: 长度分布 + 领域分布 + 难例集

回退判据：
  perplity_drift > 2% (FP8) / > 5% (INT4)
  repeat_rate_drift > 3pp
  user_complaint_rate > baseline × 1.5

回退延迟：
  T_fallback = T_load_FP16_weights ≈ 10-30s (from CPU)

KV cache 量化（独立维度）：
  FP16 KV → INT8 KV: 存储 -50%, Δppl < 0.5%
  FP16 KV → INT4 KV: 存储 -75%, Δppl 1-2%, 需 group quant
```

C：量化方法特性与精度影响。I：校准数据 KL 散度要求。U：具体精度损失与加速比需实测。

**图文说明：** 流程图：生产请求分布 → 分层采样 → 校准数据集 → 量化 → 三级验证 → (通过) 部署 / (不通过) 回退 FP16。右侧分支：线上监控 → 指标超阈值 → 自动回退。标注各阶段延迟与资源成本。下方：精度-显存帕累托前沿图——FP8/INT8/INT4 三点标注。

**方案对比与选型：** 三种量化方案对比（以 70B 模型、A100-80G 为例）：① FP8（E4M3）：显存 70GB（vs FP16 140GB），decode 带宽需求减半，perplexity 增加 < 1%，吞吐量提升 30-50%。需 H100/A100 tensor core 支持。部署复杂度低（HuggingFace TGI/vLLM 内置支持）；② INT8（SmoothQuant）：显存 70GB，perplexity 增加 1-2%，带宽节省与 FP8 相同但精度损失稍大。优势是兼容性更广（无 FP8 tensor core 的 GPU 可用 INT8 SIMT）；③ INT4（AWQ/GPTQ）：显存 35GB（减半），perplexity 增加 2-5%，但显存节省使 batch 翻倍→吞吐提升可达 80-100%。缺点是精度风险高，某些领域（代码生成）退化可达 8-10%。选型建议：优先 FP8（精度损失最小），显存仍不足时叠加 KV cache 量化（FP8 KV），最后才考虑 INT4（仅限显存极度受限的部署场景）。决策树：GPU 有 FP8 tensor core → FP8 权重；GPU 无 FP8 → INT8 权重；显存仍不足 → INT4 权重 + FP8 KV cache。

**拓展知识：** 与 I-A009 衔接：draft 模型可独立量化以降低成本；与 I-A005 衔接：量化后 kernel 数量变化影响 CUDA Graph 收益；与 I-A013 衔接：KV cache 量化与权重量化是正交的维度——可以同时应用（FP8 权重 + FP8 KV），实现显存 4x 压缩。

**常见追问及回答要点：**

1. **问：FP8 和 INT8 哪个更适合推理？** 答：FP8 更适合有 FP8 tensor core 的 GPU（H100），因为精度损失更小且硬件加速更充分。INT8 兼容性更广（旧 GPU），但需要更多校准工作。
2. **问：校准数据选择不当会有什么后果？** 答：如果校准数据缺少某个领域（如代码），该领域的量化误差会远大于其他领域——因为量化参数（scale/zero-point）是根据校准数据的激活分布优化的。后果是该领域的输出质量显著下降。
3. **问：回退期间新请求怎么处理？** 答：回退过程中使用 FP16 权重加载版本处理请求，正在进行的请求继续用量化版本完成（版本共存窗口 ≈ 30s）。路由层按权重版本分流。
4. **问：KV cache 量化对 attention 计算的具体影响？** 答：attention score = Q × K^T。K 从 FP16 量化到 INT8 引入 per-channel 误差，但 attention 对 K 的精度不敏感（softmax 前是相对大小关系）。INT8 KV 通常足够，INT4 KV 需要 group quantization（如 group_size=32）控制误差。perplexity 影响：INT8 KV Δppl < 0.5%，INT4 KV Δppl 1-2%。

**易错点与反例：**

- 反例："量化只需用随机数据校准"——错误。校准数据必须代表生产分布，否则对生产请求的量化误差不可控。
- 反例："perplexity 不增加就说明精度没问题"——不充分。perplexity 是全局平均，可能掩盖特定领域（如数学推理）的退化。
- 易错：忽视 KV cache 量化——激活量化之外，KV cache 量化（如 FP8 KV）是独立的精度/显存权衡。
- 易错：量化方案升级时的兼容性问题——INT4（GPTQ）与 INT4（AWQ）的量化参数格式不同，切换量化方案需要重新校准，不能直接替换。

**实践与验证：** [U] 对 Llama-3-70B 做 FP8 与 INT4 量化，比较：(a) 各领域的 perplexity（分创意/代码/数学/问答四类）；(b) HumanEval、GSM8K 任务得分；(b) 实际推理 throughput 与显存占用；(c) KV cache FP8 量化对 attention 计算精度的影响。oracle：FP8 Δppl < 1% 且任务得分下降 < 2%，INT4 Δppl < 3% 且显存节省 50%。当前未执行。

**分层评分标准：** 4分：能描述量化方法及其精度影响；6分：能设计校准数据方案与验证矩阵；8分：能建立回退机制并量化成本、处理版本共存；10分：能设计 KV cache 独立量化方案与量化参数版本管理、多 GPU 架构的量化选型决策树。

**证据与来源：** `insights/arxiv-pdf/2405.04437/`（量化优化论文）；`sglang/99-roadmap/performance-guide.md`（性能优化）；`insights/04-design-decision-and-validation/`（量化决策）。

**补充：量化参数的在线更新与 A/B 测试。** 量化部署后，随着生产请求分布的变化（新业务上线、季节变化），校准数据可能不再代表当前分布，导致精度逐渐退化。在线更新策略：① 每天从生产请求中采样 100 条作为"在线校准集"，计算当前量化参数下的 perplexity drift；② 如果 drift > 阈值（如 2%），触发重新校准——使用最近的在线校准集重新计算量化参数（scale/zero-point）；③ 新参数先在小部分副本（5%）上验证，确认无退化后全量推送。A/B 测试：量化参数版本与旧版本各服务 50% 流量，比较业务指标（任务完成率、用户满意度）。量化：在线校准的计算成本约 1GPU·小时/天（使用少量样本的前向推理），远低于离线校准的 10GPU·小时，但能及时捕捉分布漂移。

**补充：量化参数的在线更新与分布漂移检测。** 量化部署后，随着生产请求分布的变化（新业务上线、季节变化），校准数据可能不再代表当前分布，导致精度逐渐退化。在线更新策略：① 每天从生产请求中采样 100 条作为"在线校准集"，计算当前量化参数下的 perplexity drift；② 如果 drift > 阈值（如 2%），触发重新校准；③ 新参数先在小部分副本（5%）上验证，确认无退化后全量推送。在线校准的计算成本约 1GPU·小时/天，远低于离线校准的 10GPU·小时，但能及时捕捉分布漂移。


**补充：量化方案的成本效益分析。** 量化部署的总拥有成本（TCO）需综合考虑：① 校准成本（1-2 人天工程时间 + 10 GPU·小时校准计算）；② 验证成本（场景级 benchmark 0.5-1 人天）；③ 运行成本节省（FP8 节省 50% 显存 → 减少副本数 30-50% → 月成本节省 $5K-$20K）；④ 风险成本（精度退化导致的业务损失）。投资回报周期：对大规模部署（> 10 副本），量化 TCO 回收期通常 < 1 个月；对小规模部署（< 3 副本），回收期可能 > 6 个月——此时量化收益可能不值得工程投入。

**补充：量化方案的成本效益分析与投资回报。**

量化部署的 TCO 需综合考虑：① 校准成本（1-2 人天 + 10 GPU·小时）；② 验证成本（场景级 benchmark 0.5-1 人天）；③ 运行成本节省（FP8 节省 50% 显存 → 减少副本数 30-50%）；④ 风险成本（精度退化导致的业务损失）。

投资回报：对大规模部署（> 10 副本），量化 TCO 回收期通常 < 1 个月；对小规模部署（< 3 副本），回收期可能 > 6 个月——此时量化收益可能不值得工程投入。量化决策应基于数据而非技术偏好。
---

### I-A011 设计 PD 分离架构的 KV transfer：传输协议、流控、恢复与带宽估算。

**元数据：** 知识域：`I13 PD 分离与 KV transfer`；能力目标：`C13 分离架构设计`；题型：架构设计与量化；频率：高频；前置/后续：`I-B001`（双阶段差异）、`I-A008`（跨机分布式）；证据状态：分离架构 C，传输协议 C，运行结论 U。

**背景知识：** Prefill-Decode 分离将 prefill 和 decode 阶段部署在不同节点，各自优化（prefill 侧重计算、decode 侧重带宽与并发）。Prefill 完成后需将 KV cache 传输到 decode 节点。传输量 = 2 × n_layers × n_heads × head_dim × prompt_len × dtype_size bytes。挑战：传输带宽需求大、传输期间 decode 等待（影响 TTFT）、传输失败恢复。实际生产中，PD 分离架构的部署比例正在增长（SGLang HiCache、DistServe 等方案推动），但仅对长序列场景（prompt > 2K）有净收益——短序列场景下 KV 传输开销可能超过 prefill 计算节省。典型故障包括：RDMA 链路抖动导致 KV 传输延迟波动、prefill 节点与 decode 节点的负载不均（prefill 节点空闲但 decode 节点过载）、以及 KV 传输失败后的恢复延迟。

**问题分析：** 1) 传输协议选择：RDMA（低延迟高带宽）、gRPC（通用但开销大）、NCCL（GPU 直传）；2) 流控：prefill 速率与 decode 速率不匹配时的背压机制；3) 恢复：传输失败时的重试、回退到本地 prefill+decode 一体化；4) 带宽估算：给定 prompt 分布与 QPS，计算聚合带宽需求；5) 传输流水线化——边 prefill 边传输 vs 完成后再传输。

**具体答案：** 面试中可以这样回答：

> 传输协议：首选 RDMA（InfiniBand/RoCE），GPU 显存到 GPU 显存零拷贝传输，延迟 < 10μs，带宽 200-400Gb/s。次选 NVLink（同机内），跨机只能用 RDMA。gRPC over TCP 延迟 > 100μs 且需 CPU 拷贝，不适合大 KV cache 传输。NCCL 的 all-to-all 也可用于 KV 传输但灵活性不如自定义 RDMA。
>
> KV cache 传输量：对 Llama-3-70B（80 layers, 8 heads KV group, head_dim=128, BF16），每 token 的 KV = 2 × 80 × 8 × 128 × 2 = 327,680 bytes ≈ 320KB/token。1K prompt 的 KV = 320MB。在 200Gb/s RDMA 下传输耗时 ≈ 320MB / 25GB/s ≈ 13ms。对比 prefill 计算时间（70B、1K prompt ≈ 1-2s），传输开销占比 < 1%。
>
> 流控：decode 节点的 KV cache 接收缓冲区有上限。当缓冲区使用率 > 80% 时向 prefill 节点发送反压信号（pause prefill 调度），避免 decode 节点 OOM。Prefill 节点收到反压后暂停接受新 prefill 请求（返回 503），直到 decode 节点缓冲区降到 50%。
>
> 恢复机制：① 传输超时（> 预期 3x）→ 重试一次；② 重试失败 → 标记 KV cache 损坏，decode 节点丢弃并通知客户端重发；③ 系统性故障（RDMA 链路断）→ 自动降级为一体化模式（prefill 节点本地完成 decode），但吞吐下降 30-50%。降级切换时间 < 5s。

**技术洞察：**

```text
KV cache 传输量：
  KV_per_token = 2 × L × n_kv_heads × d_head × dtype_bytes
  例：Llama-3-70B, L=80, n_kv_heads=8, d_head=128, BF16
  KV_per_token = 2 × 80 × 8 × 128 × 2 = 320KB
  KV_total(prompt_len=P) = P × 320KB

传输时间：
  T_transfer = KV_total / RDMA_bandwidth
  例：P=1000, 200Gb/s RDMA
  T_transfer = 320MB / 25GB/s = 12.8ms

带宽需求：
  B_total = QPS × avg_prompt_len × KV_per_token
  例：QPS=100, avg_prompt=2000
  B_total = 100 × 2000 × 320KB = 64GB/s (需多 RDMA 网卡聚合)

流控阈值：
  decode_buffer_high = 80% → pause prefill
  decode_buffer_low = 50% → resume prefill

分层传输：
  T_layered = T_last_layer + RTT ≈ 2-3ms (vs 整体 13ms)
  适用条件: prompt_len > 512
```

C：KV cache 大小计算与 PD 分离原理。I：传输时间与带宽公式。U：实际 RDMA 带宽与延迟需实测。

**图文说明：** 架构图：Prefill Node（多 GPU）→ RDMA 链路 → Decode Node（多 GPU）。标注 KV cache 传输方向（Prefill→Decode）与反压信号方向（Decode→Prefill）。时序：prefill compute → KV transfer → decode start，标注 T_transfer 与 T_prefill 的相对大小。右侧：分层传输时序——prefill 每完成一层就异步传该层 KV，decode 边收边启动。

**方案对比与选型：** 三种 KV transfer 模式对比：① 整体传输（prefill 完成后一次性传所有层的 KV）：简单但 prefill 结束到 decode 开始的等待时间 = T_total_transfer ≈ 13ms（1K prompt）。TTFT 增加等于传输延迟；② 分层流水线传输（每完成一层 prefill 就异步传该层的 KV）：prefill 与传输重叠，decode 启动时首几层 KV 已到达。TTFT 增加 ≈ T_last_layer_transfer + 网络 RTT ≈ 2-3ms（相比整体传输改善 10ms）。但实现复杂（需管理每层的传输状态）；③ HiCache 分层缓存（SGLang 实现）：L1（GPU 本地）+ L2（主机内存）+ L3（远程节点）。优先从最近层读取。综合延迟最优但需要三层缓存管理。选型建议：② 分层流水线在大多数场景最优——13ms → 3ms 的 TTFT 改善显著，且实现复杂度可控。当 prompt 极短（< 256 tokens）时，传输时间 < 2ms，整体传输即可——流水线 overhead 不值得。决策边界：prompt_len > 512 → 分层流水线；prompt_len ≤ 512 → 整体传输。

**拓展知识：** 与 I-A018 衔接：分布式 KV 池是 PD 分离的延伸——KV cache 存储在独立节点而非固定绑定；与 I-A004 衔接：PD 分离消除了同一 GPU 上 prefill/decode 的资源竞争；与 I-A008 衔接：PD 分离节点的 TP/PP 选择——prefill 节点偏向高 TP（利用 NVLink 加速 prefill），decode 节点可适度降低 TP 增加并发。

**常见追问及回答要点：**

1. **问：PD 分离后 TTFT 是增加还是减少？** 答：理论上增加（多了传输延迟），但实践中可能减少——因为 prefill 节点不处理 decode，可接受更大的 batch 与更长的 chunked prefill，prefill 计算更高效。净 TTFT 变化 = T_transfer - T_prefill_saving。当 T_transfer << T_prefill_saving 时净减少。
2. **问：KV cache 传输可以流水线化吗？** 可以。Layer-by-layer 传输——prefill 节点每完成一层就传该层的 KV，decode 节点边收边开始部分 decode（HiCache 的分层缓存策略）。这可以隐藏传输延迟。
3. **问：PD 分离的 GPU 配比如何决定？** 答：按 prefill 计算量与 decode 带宽需求配比。经验值：1 个 prefill 节点（8×A100）服务 2-3 个 decode 节点（每节点 8×A100），具体取决于 prompt 长度分布。
4. **问：KV 传输过程中 decode 节点如何提前准备？** 答：decode 节点可在 prefill 进行时就分配 KV cache 页框（空页），待 KV 到达后直接填充。避免 KV 到达后再分配导致的延迟。

**易错点与反例：**

- 反例："KV cache 传输用 TCP 就行"——错误。KV cache 传输量可达数百 MB，TCP 的 CPU 拷贝与协议栈开销使延迟增加 10x，带宽利用率低。
- 反例："PD 分离总是降低延迟"——不成立。对于短 prompt（< 100 tokens），传输开销可能超过预计算的收益。
- 易错：忽视 KV cache 的显存碎片化——频繁的传输/释放导致 decode 节点 KV cache 不连续，影响分配效率。
- 易错：prefill 节点的负载不均衡——如果某些请求的 prompt 极长（100K），prefill 节点被长 prefill 独占，其他请求等待。需要 prefill 节点也支持 chunked prefill + 请求交错。

**实践与验证：** [U] 部署 PD 分离（2 prefill 节点 + 4 decode 节点），测量：(a) TTFT 与 TPOT 对比一体化部署；(b) RDMA 传输延迟分布（P50/P95/P99）；(c) 流控触发频率与反压持续时间；(d) 分层流水线 vs 整体传输的 TTFT 差异。oracle：长序列（> 2K）场景 TPOT 改善 > 20%，TTFT 变化 < 10%，分层流水线比整体传输 TTFT 改善 > 8ms。当前未执行。

**分层评分标准：** 4分：能描述 PD 分离基本原理与 KV 传输量计算；6分：能设计传输协议选型与流控机制；8分：能设计恢复机制、分层流水线传输、GPU 配比优化；10分：能在真实 RDMA 网络约束下设计完整的 PD 分离方案，包括降级策略与负载均衡。

**证据与来源：** `sglang/01-modules/M13-disaggregation-hicache.md`（PD 分离与 HiCache）；`insights/arxiv-pdf/2309.06180/`（分离推理论文）；`sglang/90-cross-module/performance-critical-paths.md`（跨节点延迟分析）。

**补充：KV cache 压缩传输与增量更新。** 长序列的 KV cache 传输量巨大（128K prompt = 40GB），即使 RDMA 也需要 1.6s。压缩方案：① KV 量化传输——传输前将 KV 从 FP16 压缩到 FP8（传输量减半），decode 节点接收后再反量化。引入的额外延迟（量化+反量化）约 0.5ms/层，总计 40ms（80 层），但传输时间从 1.6s 降到 0.8s（净节省 0.76s）；② 增量更新传输——如果 prefill 节点与 decode 节点共享部分 KV（如 system prompt 前缀），只传输新增部分的 KV（增量传输量 = (1 - overlap_ratio) × total_KV）。量化：50% 前缀共享率下，传输量从 40GB 降到 20GB，传输时间从 1.6s 降到 0.8s（叠加 FP8 后降到 0.4s）。

**补充：KV cache 压缩传输与增量更新。** 长序列的 KV cache 传输量巨大（128K prompt = 40GB），即使 RDMA 也需要 1.6s。压缩方案：① KV 量化传输——传输前将 KV 从 FP16 压缩到 FP8（传输量减半），decode 节点接收后再反量化。引入的额外延迟约 40ms（80 层），但传输时间从 1.6s 降到 0.8s（净节省 0.76s）；② 增量更新传输——如果 prefill 节点与 decode 节点共享部分 KV（如 system prompt 前缀），只传输新增部分的 KV。50% 前缀共享率下，传输量从 40GB 降到 20GB。


**补充：KV 传输的可靠性保证与 checksum 机制。**

RDMA 传输可能因网络错误导致 KV 数据损坏（bit flip）。虽然概率低（IB 的 BER ≈ 1e-12），但大规模集群中每月仍可能发生数次。

解决方案：在 KV 传输中加入 CRC32 checksum——发送方计算 KV 数据的 checksum 并附加在消息尾部，接收方验证 checksum，损坏时请求重传。Checksum 计算增加约 0.5ms 延迟（对 320MB KV），相比传输时间（13ms）可忽略。重传率目标 < 0.001%。
---

### I-A012 设计多模型共池推理平台：显存切分、抢占策略与 SLO 分级。

**元数据：** 知识域：`I14 多模型共池`；能力目标：`C14 资源复用`；题型：架构设计；频率：中频；前置/后续：`I-A006`（多租户隔离）、`I-A010`（量化部署）；证据状态：设计模式 C，调度算法 C，运行结论 U。

**背景知识：** 多模型共池指同一 GPU 集群服务多个模型（如 Llama-3-8B、Qwen2-72B、Embedding 模型、Reranker）。各模型大小、延迟特征、显存需求不同。共池可提高 GPU 利用率（不同模型的负载峰谷互补），但引入显存竞争、模型切换开销、SLO 冲突。实际生产中，多模型共池的典型场景包括：对话模型（70B）+ Embedding 模型（0.5B）+ Reranker（3B）共池——三种模型的请求延迟差异大（对话 2-10s vs Embedding 50ms），显存需求差异也大（140GB vs 1GB vs 6GB）。典型故障包括：大模型加载时小模型请求被饿死、模型置换频率过高导致 cache thrashing、以及不同模型的 autoscaling 需求冲突。

**问题分析：** 1) 显存切分：多个模型权重共存于一 GPU 的显存预算；2) 模型加载/卸载策略：LRU 置换 vs 常驻 vs 预加载；3) 抢占策略：高优先级模型需要显存时如何回收低优先级模型的资源；4) SLO 分级：不同模型的延迟 SLO 如何在共池下保障；5) 请求路由——如何将请求分发到持有目标模型的副本。

**具体答案：** 面试中可以这样回答：

> 显存切分：GPU 显存 = Σ model_weights + shared_KV_cache_pool + activation_overhead。大模型（70B BF16=140GB）独占多卡，小模型（8B BF16=16GB）可多实例共存。共池策略：小模型常驻（权重常驻显存，服务延迟低），大模型按需加载（权重从 CPU/NAS 加载，首次请求延迟高）。常驻模型的显存占比 ≤ 60%，剩余 40% 为弹性池供大模型加载。
>
> 模型置换：LRU + 访问频率加权。模型 M 的驻留分数 = α × recency + β × frequency + γ × SLO_tightness。分数最低的被置换出显存。置换成本 = 卸载时间 + 下次加载时间。8B 模型加载 ≈ 2s，70B 模型加载 ≈ 30s。置换频率阈值：如果模型加载间隔 < 加载时间 × 2，则不值得置换（一直在加载/卸载）。
>
> 抢占策略：当高优先级模型需要显存且弹性池不足时：① 先降级低优先级模型的 KV cache（evict 部分序列）；② 再卸载低优先级模型的权重（已完成的请求不受影响，新请求路由到其他副本）；③ 最后手段：拒绝低优先级模型的新请求。抢占决策时间 < 100ms。
>
> SLO 分级：每个模型定义 SLO tier——Tier-0（关键业务，P99 TTFT < 500ms，常驻）、Tier-1（标准业务，P99 TTFT < 2s，按需加载但优先驻留）、Tier-2（后台任务，P99 TTFT < 10s，用完即卸）。GPU 资源分配按 tier 倒序抢占。

**技术洞察：**

```text
显存预算：
  M_GPU = M_model_weights + M_KV_cache_pool + M_activation + M_safety
  常驻模型占比 ≤ 60%
  弹性池 ≥ 40%

置换判据：
  Score(M) = α×recency + β×freq + γ×SLO_tier
  Evict argmin(Score)
  置换收益 = load_interval > 2 × load_time

抢占优先级：
  Tier-0: 常驻，不可抢占
  Tier-1: 可降 KV cache
  Tier-2: 可卸载权重 + 拒新请求

模型加载时间：
  T_load = model_size / IO_bandwidth
  例：8B BF16, PCIe 4.0 (8GB/s): T ≈ 2s
  例：70B BF16, PCIe 4.0: T ≈ 18s (或从 NVMe 网络加载)

请求路由：
  每个副本注册"已加载模型列表"到服务发现
  路由层根据 model_name 选择持有该模型的副本
  无可用副本时：排队等待加载 or 返回503
```

C：显存管理与置换策略。I：驻留分数与置换收益公式。U：具体加载时间与置换频率需实测。

**图文说明：** 堆叠柱状图：GPU 显存分为 Tier-0 常驻模型（多色块）、Tier-1 按需模型（蓝色）、弹性池（白色）、KV Cache Pool（橙色）、Safety Margin（红色窄条）。箭头标注抢占方向：弹性池不足时 → 压缩 Tier-2 → 压缩 Tier-1。旁注：请求路由——Client → Router(查模型注册表) → 持有目标模型的副本。

**方案对比与选型：** 三种多模型调度策略对比：① 时分复用（时间片轮转，如每 5 分钟切换活跃模型）：同一 GPU 只服务一个模型，通过权重加载/卸载切换。切换延迟 = 模型加载时间（2-30s），期间该 GPU 不服务任何请求。简单但切换期间吞吐为 0；② 空分复用（显存分区，多个模型权重常驻）：同时服务多个显存放得下的模型。GPU 利用率高但每个模型的 KV cache 受限于分区大小；③ 混合策略（推荐）：高频小模型常驻（空分），大模型按需加载（时分）。常驻模型用 INT4 量化压缩权重，节省空间给弹性池。选型建议：③ 最优——常驻 3-5 个高频小模型（覆盖 80% QPS），大模型按需加载（覆盖剩余 20%）。优化点：模型加载使用 RDMA 从 CPU 内存加载到 GPU，比从 NVMe 快 5-10x；预取策略——根据请求队列深度预测即将需要的模型，提前异步加载（类似 CPU 指令预取）。

**拓展知识：** 与 I-A006 衔接：多租户隔离在多模型场景下叠加——既要按租户隔离又要按模型隔离，形成二维配额矩阵；与 I-A010 衔接：小模型量化（INT4）可减少常驻显存占用，容纳更多模型；与 I-A015 衔接：缩容时优先回收只服务低频率模型的副本（大模型按需副本），保持常驻小模型副本不动。

**常见追问及回答要点：**

1. **问：多个小模型和一个大模型共池，显存怎么分？** 答：大模型权重常驻需要多卡 TP，小模型在剩余卡上多实例。如果显存仍有剩余，大模型也可以量化（FP8）释放空间给小模型。目标是 GPU 利用率 > 70%。
2. **问：模型置换期间在途请求怎么处理？** 答：置换前标记模型为"draining"（不接受新请求），等待在途请求完成（最多 30s 超时），然后卸载。新请求路由到已有该模型的其他副本。
3. **问：如何避免模型加载成为冷启动瓶颈？** 答：预热池——对高频模型保持最低 1 个常驻副本（即使近期无请求），类似 CPU cache 的"常开 line"。预热池大小 = 最高频的 N 个模型。
4. **问：多模型共池的 autoscaling 怎么做？** 答：每个模型独立统计负载信号（QPS、KV cache 水位），但共享 GPU 池。当总 GPU 需求超过池容量时，按 SLO tier 决定哪个模型的扩容优先——Tier-0 优先扩容，Tier-2 可能缩容让出资源。

**易错点与反例：**

- 反例："所有模型常驻显存"——不现实。10 个 70B 模型需要 1.4TB 显存，远超单机容量。
- 反例："按需加载所有模型"——首次请求延迟不可接受（70B 加载 30s）。
- 易错：忽视模型加载的 IO 竞争——多个模型同时加载时 PCIe 带宽争用，加载时间非线性增长。
- 易错：模型注册表的一致性问题——如果路由层认为某副本持有模型 M，但副本正在卸载 M，会导致请求失败。需要原子性更新注册表（先卸载、再更新注册表、最后处理新请求）。

**实践与验证：** [U] 部署 3 个模型（8B/70B/Embedding）共池，注入不同 QPS 模式，测量：(a) GPU 利用率（目标 > 65%）；(b) 各模型 SLO 达标率（Tier-0 100%, Tier-1 > 99%）；(c) 置换频率与命中率（hit rate > 80%）；(d) 模型加载对同卡其他模型吞吐的影响（加载期间吞吐下降 < 20%）。oracle：GPU 利用率 > 65%，Tier-0 SLO 100% 达标，置换命中率 > 80%。当前未执行。

**分层评分标准：** 4分：能描述多模型共池的显存管理需求；6分：能设计置换策略与抢占优先级；8分：能处理二维隔离（租户×模型）、预热池、IO 竞争；10分：能在多模型×多租户×多 SLO tier 的复杂约束下设计完整的共池方案。

**证据与来源：** `sglang/90-cross-module/pooling-and-resource-management.md`（资源池化）；`sglang/01-modules/M08-kv-cache.md`（显存管理）；`insights/05-sglang-inference-technical-insights/`（多模型实践）。

**补充：模型权重预取与请求排队联动。** 多模型共池的瓶颈之一是模型加载延迟（大模型 30s），期间请求要么排队等待（增加 TTFT），要么返回 503（降低可用性）。优化方案：① 请求排队联动——当某模型的请求队列深度 > 阈值（如 5）且该模型不在显存中时，触发异步预加载。预加载期间请求继续排队，但 TTFT 增加量 = 排队时间 + 加载时间（vs 不预加载时请求可能被 503）；② 基于时间模式的预加载——根据历史负载曲线，在预期高峰前 N 分钟预加载模型（如每天 8:50 预加载高频对话模型，应对 9:00 高峰）；③ 优先级预加载——当多个模型同时需要加载时，按"队列深度 × 平均请求价值"决定加载顺序。量化：预加载可将首次请求的 TTFT 从 30s+（按需加载）降低到 5-10s（预加载命中），但需付出 1-2 个 GPU 的加载期间闲置成本。

**补充：模型权重预取与请求排队联动。** 多模型共池的瓶颈之一是模型加载延迟（大模型 30s），期间请求要么排队等待（增加 TTFT），要么返回 503。优化方案：① 请求排队联动——当某模型的请求队列深度 > 阈值（如 5）且该模型不在显存中时，触发异步预加载；② 基于时间模式的预加载——根据历史负载曲线，在预期高峰前 N 分钟预加载模型；③ 优先级预加载——当多个模型同时需要加载时，按"队列深度 × 平均请求价值"决定加载顺序。预加载可将首次请求的 TTFT 从 30s+ 降低到 5-10s。

---

### I-A013 设计长上下文推理的显存与延迟工程：分页、eviction 与 KV 量化的组合策略。

**元数据：** 知识域：`I15 长上下文推理`；能力目标：`C15 长序列优化`；题型：架构设计与量化；频率：高频；前置/后续：`I-A004`（prefill/decode 平衡）、`I-A006`（KV cache 管理）；证据状态：分页机制 C，eviction 算法 C，运行结论 U。

**背景知识：** 长上下文（32K-128K tokens）推理的显存瓶颈在 KV cache——序列越长 KV cache 越大。128K 上下文的 KV cache 可达数 GB/请求。需要：① 分页（PagedAttention）避免显存碎片；② Eviction（淘汰）释放不再需要的 KV 页；③ KV 量化减少单位 token 的存储。实际生产中，长上下文推理的主要应用场景包括：文档摘要（32K-128K）、代码仓库分析（64K+）、以及多轮对话历史累积（16K-64K）。典型故障包括：长序列 eviction 后重算延迟过高导致 TPOT 毛刺、以及 KV 量化引入的 attention 精度退化影响长程依赖推理。

**问题分析：** 1) 分页机制：将 KV cache 分为固定大小的页（如 16 tokens/page），按页分配/释放，避免连续显存分配失败；2) Eviction 策略：哪些页先淘汰（LRU、attention-based、future-irrelevance）；3) KV 量化：KV cache 从 FP16 降到 INT8/FP4 的精度影响；4) 组合策略：三者如何协同——分页解决分配、eviction 解决总量、量化解决单位成本；5) Eviction 后恢复的开销对比（重算 vs disk spill）。

**具体答案：** 面试中可以这样回答：

> 三层防线。第一层：分页（PagedAttention）。将 KV cache 分为 16-token 的页，页表映射逻辑序列位置到物理显存页。分页解决两个问题：① 显存碎片——不再需要连续显存块；② 共享前缀——多个请求共享 system prompt 的 KV 页（copy-on-write）。量化：128K 序列 × 320KB/token = 40GB KV cache。分页后按需分配，避免一次性分配 40GB 连续块。
>
> 第二层：Eviction。当 KV cache 池使用率 > 90% 时触发。淘汰策略组合：① 已完成的序列整体释放；② 活跃序列中淘汰 attention 权重最低的页（基于最近 N 步的 attention score 累积）；③ 最后淘汰 LRU 页。注意：淘汰后如果请求还需要这些位置的信息（远距离 attention），需要重算。Eviction-to-disk 方案可将淘汰页暂存 NVMe，重算时从磁盘读取（延迟 10-100ms vs 重算 50-200ms）。
>
> 第三层：KV 量化。将 KV cache 从 FP16 量化到 INT8（per-channel group quantization），存储减半。精度影响：attention score 计算使用量化 KV 时误差约 1-2%，对生成质量影响极小（perplexity 增加 < 1%）。进一步到 INT4（2-bit）风险大，需要组内校准。量化+eviction 组合效果：128K 序列原始 40GB → 量化后 20GB → eviction 活跃窗口 4K tokens → 仅需 128MB 活跃页 + disk 缓存。
>
> 延迟影响：eviction 重算延迟 = evicted_pages × prefill_time_per_page。如果每页 16 tokens，evict 1000 页（16K tokens），重算耗时 ≈ 16000 × t_prefill_per_token ≈ 200-500ms。对比从 NVMe 加载 1000 页（16KB × 1000 = 16MB）耗时 ≈ 5ms。因此 eviction-to-disk 优于重算。

**技术洞察：**

```text
三层防线：
  L1 Paging: 固定大小页（16 tokens/page），避免碎片，支持共享前缀
  L2 Eviction: attention-weighted LRU + completed_seq_free + disk_spill
  L3 Quantization: FP16→INT8 KV cache, 存储 -50%, Δppl < 1%

组合效果：
  Original: 128K × 320KB = 40GB
  Quantized: 20GB
  Eviction (active 4K): 4K × 160KB = 640MB + disk spill
  Effective reduction: 62.5x

延迟代价：
  T_recompute = N_evicted_pages × 16 × t_prefill_per_token
  T_disk_load = N_evicted_pages × page_size / NVMe_bandwidth
  例：N=1000, t_recompute ≈ 300ms, T_disk_load ≈ 5ms

长程依赖保护：
  保留最近 W tokens 的 KV（W = attention_window）
  淘汰 W 之前的页（远距离 attention 权重低）
  W 通常 = 2048-4096 tokens
```

C：PagedAttention 与 KV 量化原理。I：组合效果公式。U：实际精度影响与重算延迟需实测。

**图文说明：** 三层堆叠图：底层 Paging（页表映射，碎片 → 整页），中层 Eviction（活跃页 + 磁盘暂存），顶层 Quantization（每页压缩 50%）。右侧时间线：长序列推理的 KV cache 增长曲线，标注 eviction 触发点与量化生效点。下方：eviction-to-disk vs 重算的延迟对比柱状图。

**方案对比与选型：** 长上下文三种工程方案对比：① 纯 PagedAttention（不分页外措施）：支持 32K-128K 上下文，但 128K 时 KV cache 占用 40GB（单请求），GPU 只能服务极少量并发。适用于长上下文但低并发场景；② PagedAttention + KV 量化（FP8 KV）：KV cache 减半至 20GB，并发翻倍。精度损失可接受（perplexity +0.5%）。适用于中等并发（4-8 并行长序列）；③ PagedAttention + 量化 + eviction-to-disk：活跃窗口 4K tokens 本地显存，其余 124K 在 NVMe。并发可达 16-32，但 eviction page fault 引入 5-100ms 延迟毛刺。适用于高并发长序列。选型决策：128K 上下文 + 并发需求 ≤ 4 → 方案①；并发 4-16 → 方案②；并发 > 16 → 方案③。注意：方案③的 disk spill 需要 NVMe 阵列（单盘 7GB/s 不够，需 4 盘 RAID0 达到 28GB/s）来支撑高频 page fault 加载。

**拓展知识：** 与 I-A004 衔接：长序列的 prefill 阶段本身就是显存大户，chunked prefill 控制单次激活峰值；与 I-A011 衔接：PD 分离下长序列的 KV 传输量巨大，需要压缩或分层传输；与 I-A010 衔接：KV cache 量化是 FP8 量化的特例——精度要求更低（attention score 误差容忍度高），可使用更激进的 per-channel 量化而非 per-token。

**常见追问及回答要点：**

1. **问：KV cache 分页大小怎么选？** 答：页越小灵活性越高但页表开销越大。16 tokens/page 是折中——页表开销 < 1% 总 KV cache，同时支持细粒度共享。对于 prefix caching 场景（共享 system prompt），页越小共享粒度越细。
2. **问：eviction 后需要重算的 KV 怎么知道要重算？** 答：页表中维护 valid bit。当读取到 invalid 页时触发 page fault——从磁盘加载或重算。重算路径：从该页对应的原始 tokens 重新执行 forward pass 得到 KV。
3. **问：KV 量化对 attention 计算的影响？** 答：attention score = Q × K^T。量化 K 引入误差，但 attention 对 K 的精度不敏感（softmax 前的相对大小关系更重要）。INT8 量化通常足够，INT4 需要 group quantization 控制误差。
4. **问：长程依赖（如文档首尾关联）在 eviction 后如何保持？** 答：保留"锚点页"——序列首尾的 KV 页标记为 pinned（不可 eviction），确保模型始终能访问文档开头和中间的关键位置。中间页按 attention 权重淘汰。

**易错点与反例：**

- 反例："长上下文直接分配连续大 tensor"——错误。128K 序列在 batch 下需要连续数 GB 显存，容易分配失败。分页是必须的。
- 反例："eviction 完全不影响质量"——有误。远距离依赖的页被淘汰后，模型"忘记"了早期上下文信息，可能影响长程推理。
- 易错：忽视 KV 量化的校准开销——需要 per-channel 的 scale factor，校准过程需在部署前完成。
- 易错：eviction-to-disk 的磁盘碎片化——频繁的页分配/释放导致 NVMe 文件系统碎片化，读写性能随时间下降。需要定期 TRIM/discard 或预分配大文件池。

**实践与验证：** [U] 部署 Llama-3-8B 长上下文（128K），测量不同策略组合下的：(a) 最大支持并发序列数；(b) P99 TTFT 与 TPOT（含 eviction 毛刺）；(c) 输出质量（perplexity + 长程 QA 任务得分，如 Needle-in-Haystack）；(d) eviction-to-disk 的 page fault 延迟分布。oracle：分页+量化+eviction 组合下支持 4x 并发且质量下降 < 2%，P99 TPOT 毛刺 < 50ms。当前未执行。

**分层评分标准：** 4分：能描述分页与 eviction 的基本机制；6分：能建立三层防线模型并量化组合效果；8分：能分析 eviction-to-disk vs 重算的延迟权衡、KV 量化对 attention 的影响；10分：能设计长程依赖保护机制、磁盘碎片化防护、以及多策略组合的自适应切换逻辑。

**证据与来源：** `sglang/01-modules/M08-kv-cache.md`（PagedAttention 与量化）；`insights/arxiv-pdf/2309.06180/`（长上下文优化）；`sglang/99-roadmap/performance-guide.md`（长序列性能调优）。

**补充：KV cache 的跨请求共享效率分析。** 长上下文场景下，多个请求可能共享相同的 system prompt 或文档前缀。PagedAttention 的 copy-on-write 机制允许共享前缀的 KV 页，节省显存与 prefill 计算。共享效率取决于前缀重叠率：① 100% 重叠（同一 system prompt）——只需 prefill 一次，后续请求直接引用共享页，TTFT 降低 90%+；② 50% 重叠——节省 50% 的 KV cache 存储，但 prefill 仍需处理非重叠部分；③ 0% 重叠——无共享收益。实际生产中，system prompt 共享率在 60-80%（同一应用内请求使用相同系统指令），文档前缀共享率在 20-40%（不同文档但相同检索增强框架）。量化：100 个请求共享 4K system prompt（KV = 1.28GB），无共享时需要 128GB，共享后仅需 1.28GB + 100 × 私有 KV——节省 99% 的前缀存储。

**补充：KV cache 的跨请求共享效率分析。** 长上下文场景下，多个请求可能共享相同的 system prompt 或文档前缀。PagedAttention 的 copy-on-write 机制允许共享前缀的 KV 页，节省显存与 prefill 计算。共享效率取决于前缀重叠率：100% 重叠（同一 system prompt）——只需 prefill 一次，后续请求直接引用共享页，TTFT 降低 90%+；50% 重叠——节省 50% 的 KV cache 存储；0% 重叠——无共享收益。实际生产中，system prompt 共享率在 60-80%（同一应用内请求），文档前缀共享率在 20-40%。100 个请求共享 4K system prompt（KV = 1.28GB），共享后仅需 1.28GB + 100 × 私有 KV——节省 99% 的前缀存储。


**补充：长上下文的异步处理模式。** 对超长 prompt（> 64K），同步处理（客户端等待 TTFT）的用户体验差（等待 30-60s）。解决方案：异步 API——客户端提交请求后立即返回 `task_id`，轮询 task status 或通过 webhook 接收完成通知。异步模式的额外复杂性：① 结果缓存（完成后存储结果，客户端拉取）；② 超时处理（超过 10 分钟的请求自动终止）；③ 计费（按实际处理时间而非 wall clock time 计费）。生产经验：异步模式可将长上下文请求的用户满意度从 60% 提升到 85%（因为用户无需等待且可随时查看进度）。

**补充：长上下文的异步处理与用户体验优化。**

对超长 prompt（> 64K），同步处理的用户体验差（等待 30-60s）。解决方案：异步 API——客户端提交后立即返回 task_id，轮询或通过 webhook 接收结果。

异步模式的额外复杂性：① 结果缓存（完成后存储，客户端拉取）；② 超时处理（> 10min 自动终止）；③ 计费（按实际处理时间而非 wall clock time）。异步模式可将长上下文请求的用户满意度从 60% 提升到 85%。
---

### I-A014 设计推理服务可观测体系：从 kernel 到请求的全链路指标、trace 与采样。

**元数据：** 知识域：`I16 可观测体系`；能力目标：`C16 全链路观测`；题型：架构设计；频率：高频；前置/后续：`I-A001`（配置一致性）、`I-A003`（autoscaling）；证据状态：设计模式 C，指标定义 C，运行结论 U。

**背景知识：** 推理服务可观测需要覆盖：① 硬件层（GPU util、显存、温度、功耗）；② 内核层（kernel 执行时间、CUDA API 调用、NCCL 通信）；③ 引擎层（调度延迟、prefill/decode 耗时、KV cache 命中率）；④ 请求层（TTFT、TPOT、e2e 延迟、成功率）。全链路 trace 关联请求从入口到 GPU kernel 的完整路径。实际生产中，推理服务的可观测数据量巨大——100 副本 × 每副本 500 metrics/s × 8 bytes = 40MB/s 原始 metrics 流量。典型问题包括：kernel trace 开销过大（NSight 全量开启时吞吐下降 10-15%）、跨进程 trace context 传播丢失（异步队列导致 trace_id 断裂）、以及 metrics  cardinality 爆炸（高基数标签如 request_id 导致 TSDB 查询变慢）。

**问题分析：** 1) 指标分层与关联：如何将 kernel 级别的延迟关联到请求级别；2) Trace 设计：跨进程、跨节点的 trace context 传播；3) 采样策略：全量 trace 开销大，如何采样保证统计显著性；4) 实时性：秒级监控 vs 分钟级聚合的架构分工；5) 异常检测——如何从 metrics 中自动识别性能退化（而非仅靠静态阈值告警）。

**具体答案：** 面试中可以这样回答：

> 四层指标采集。① 硬件层：DCGM 导出 GPU metrics（util、mem、temp、power），1s 粒度，保留 24h。② 内核层：CUDA profiling（NSight Systems/MUPTI）采集 kernel trace，但全量开启开销 5-15%，采用采样开启（每 100 个请求 trace 一个）。MUPTI callback 在 kernel launch/complete 时触发，记录 trace_id 作为 annotation。③ 引擎层：SGLang 内部埋点——request_enqueue_time、prefill_start、prefill_end、decode_start、decode_end，每次调度决策记录 budget 使用情况。④ 请求层：入口网关记录 arrival_time、first_token_time、completion_time，计算 TTFT = first_token - arrival, TPOT = (completion - first_token) / n_tokens。
>
> Trace 关联：每个请求分配全局 trace_id，通过 gRPC metadata / HTTP header 跨进程传播。引擎内部将 trace_id 关联到 ForwardBatch ID，ForwardBatch 关联到具体 GPU kernel 执行记录。NSight trace 中嵌入 trace_id 作为 annotation，实现从请求到 kernel 的端到端关联。异步队列中的传播：trace_id 存入线程上下文（thread-local storage）或显式传递 context 对象，确保不丢失。
>
> 采样策略：请求层全量（低开销），引擎层全量（已在代码中），内核层采样（1% 请求深度 trace）。采样率根据错误率动态调整——错误率 > 1% 时提升到 10%，错误率 < 0.1% 时降到 0.1%。Trace 存储：热数据（1h）全量入 TSDB，冷数据采样存入对象存储。
>
> 实时性架构：实时路径（<1s 延迟）：DCGM metrics + 请求层 metrics → Prometheus → Grafana + 告警。准实时路径（1min）：引擎层聚合指标 → ClickHouse → 分析看板。离线路径：全量 trace → S3/对象存储 → Spark 分析。
>
> 异常检测：基于历史基线（过去 7 天同时段）的 Z-score 检测——如果当前 P99 TTFT 超过基线 3 个标准差，触发告警。这比静态阈值更适应负载的周期性波动（如工作日 vs 周末）。

**技术洞察：**

```text
指标分层：
  L1 Hardware: GPU util/mem/temp/power, 1s 粒度, DCGM
  L2 Kernel: NSight/MUPTI trace, 1% 采样
  L3 Engine: prefill/decode timing, KV cache hit rate, 100% 采集
  L4 Request: TTFT, TPOT, e2e latency, 100% 采集

关键公式：
  TTFT = t_first_token - t_arrival
  TPOT = (t_completion - t_first_token) / n_output_tokens
  KV_cache_hit_rate = hits / (hits + misses)

Trace 采样：
  base_rate = 1%
  error_rate > 1% → rate = 10%
  error_rate < 0.1% → rate = 0.1%
  深度 trace 开销 ≈ 5-15% 吞吐损失

存储分层：
  Hot (1h): TSDB, 全量
  Warm (24h): ClickHouse, 聚合
  Cold (90d): Object Storage, 采样 trace

异常检测：
  Z_score = (current_p99 - baseline_mean) / baseline_std
  Alert if Z_score > 3
  Baseline = 过去 7 天同时段滚动窗口
```

C：四层指标采集与 trace 设计。I：采样率自适应公式。U：实际 trace 开销与存储成本需实测。

**图文说明：** 层次图：Request → Gateway(L4) → Engine(L3) → CUDA Runtime(L2) → GPU(L1)。每层标注指标类型与采集频率。横向箭头：trace_id 跨层传播。右侧：实时/准实时/离线三条数据流路径。旁注：异常检测——Z-score 曲线与 3σ 阈值线。

**方案对比与选型：** 三种 trace 方案对比：① OpenTelemetry（OTel）：标准化 trace 格式，支持跨语言、跨服务 trace 关联。SDK 开销低（< 1% CPU），支持 tail-based 采样。但 GPU kernel trace 需要自定义 instrumentation（需手动将 trace_id 注入 NSight trace）；② 自建 trace 系统（基于 Kafka + ClickHouse）：完全可控，trace 数据与 metrics 统一存储。但开发成本高（约 2-3 人月），且跨服务关联需要自建上下文传播规范；③ 商业方案（Datadog/New Relic）：开箱即用，dashboard 丰富。但成本高（$15-20/host/month），且 GPU 深度 trace 支持有限。选型建议：① OTel + 自定义 GPU annotation 是最佳平衡——标准化格式降低集成成本，自定义 annotation 支持 GPU 深度 trace。存储选型：metrics 用 Prometheus（TSDB 优化），trace 用 ClickHouse（OLAP 查询），告警用 Grafana Alerting。成本模型：100 副本 × 每副本 500 metrics/s × 8 bytes = 40MB/s metrics 写入，ClickHouse 年存储约 1.2TB（压缩后）。

**拓展知识：** 与 I-A003 衔接：autoscaling 的决策依赖实时 metrics 管道；与 I-A001 衔接：配置变更后的效果验证依赖 metrics 对比（before/after 对比分析）；与 I-A016 衔接：灰度发布期间新/旧版本 metrics 对比是可观测体系的核心功能——需在 trace 中标记 `version_tag` 以支持分组对比。

**常见追问及回答要点：**

1. **问：如何将 kernel 执行时间关联到具体请求？** 答：通过在 kernel launch 时传入 trace_id（CUDA 的 user annotation），NSight trace 中会记录该 annotation。离线分析时将 trace_id 相同的 kernel 段聚合，得到每个请求的总 kernel 时间。
2. **问：TTFT 异常如何定位？** 答：分层排查——① 网关层：排队延迟是否过高（autoscaling 问题）；② 引擎层：prefill 是否被长 prefill 阻塞（调度问题）；③ 内核层：prefill kernel 是否异常慢（GPU 或显存问题）。每层对比 P50/P99，定位异常层。
3. **问：全量 trace 的开销如何降低？** 答：trace 数据异步写入（lock-free ring buffer），序列化用 FlatBuffers（零拷贝），网络传输用 gRPC streaming（批量发送）。单请求 trace 数据量 ≈ 1-10KB，100 QPS 下 = 1MB/s，可接受。
4. **问：metrics cardinality 爆炸如何解决？** 答：高基数标签（如 request_id、user_id）不放入 Prometheus metrics（Prometheus 对高基数支持差），改为只记录在 trace 中。Metrics 只保留低基数标签（model_name、replica_id、tenant_tier）。需要关联时通过 trace_id 在 ClickHouse 中 join。

**易错点与反例：**

- 反例："全量开启 kernel trace 用于生产监控"——开销过大（15% 吞吐损失），生产环境必须采样。
- 反例："只看请求层 metrics 就够了"——无法定位根因。TTFT 高可能是 GPU 过热（硬件层）或 prefill 阻塞（引擎层），请求层只看到症状。
- 易错：忽视 trace_id 传播的线程安全性——跨线程传递时需使用 thread-local storage 或 context 对象。异步回调（如 CUDA stream callback）中的 trace_id 需显式捕获。
- 易错：Z-score 异常检测的基线选择——如果用全天平均作为基线，早晚高峰会持续误报警。应用"同时段基线"（同时段 = 同一小时 + 工作日/周末区分）。

**实践与验证：** [U] 部署推理服务 + DCGM + NSight 采样 trace，验证：(a) 端到端 trace 关联准确率（>99%）；(b) 采样 trace 的统计显著性（1% 采样能否代表 P99——误差 < 5%）；(c) trace 写入对吞吐的影响（< 0.5%）；(d) Z-score 异常检测的误报率（< 5%，基于历史数据回放）。oracle：trace 关联 >99%，1% 采样 P99 误差 < 5%，吞吐损失 < 0.5%，误报率 < 5%。当前未执行。

**分层评分标准：** 4分：能列举各层指标；6分：能设计 trace 关联与采样策略；8分：能设计自适应采样、存储分层、异常检测；10分：能设计完整的可观测体系，包括 cardinality 治理、trace 传播正确性验证、以及基于 ML 的异常检测。

**证据与来源：** `sglang/01-modules/M18-testing-benchmark-ci.md`（测试与基准）；`sglang/99-roadmap/performance-guide.md`（性能测量方法论）；`insights/05-sglang-inference-technical-insights/`（可观测实践）。

---

### I-A015 设计推理服务的 autoscaling 与优雅缩容：drain 策略与成本优化。

**元数据：** 知识域：`I17 自动伸缩与优雅缩容`；能力目标：`C17 弹性与成本`；题型：架构设计与量化；频率：高频；前置/后续：`I-A003`（autoscaling 多信号）、`I-A016`（灰度发布）；证据状态：策略设计 C，成本模型 C，运行结论 U。

**背景知识：** 缩容（scale-in）比扩容（scale-out）更难——缩容时需要将待回收副本上的在途请求优雅完成（drain），同时不新分配请求。挑战：drain 时间不可预测（取决于最长序列的剩余 token 数）、drain 期间副本仍占用资源、过早缩容导致过载、频繁伸缩增加调度开销。实际生产中，推理服务缩容的 drain 时间分布为：P50=30s、P95=180s、P99=400s——长尾由长序列请求（> 5000 tokens）贡献。成本优化的关键发现：大多数推理集群的平均 GPU 利用率仅 30-40%（峰值 70-80%），缩容优化可提升利用率到 50-60%，节省 20-30% 成本。典型故障包括：drain 期间新请求仍路由到 draining 副本（路由层未及时更新）、以及 drain 超时后强制终止导致用户收到截断响应。

**问题分析：** 1) Drain 策略：如何在不中断请求的情况下回收副本；2) 缩容判据：负载降低到多少才缩容，冷却窗口设计；3) 成本模型：GPU 使用量 vs 请求 SLO 的帕累托前沿；4) 预测性缩容：基于历史负载曲线提前缩容；5) drain 期间的资源核算——draining 副本的 GPU 是否计入已用容量。

**具体答案：** 面试中可以这样回答：

> Drain 流程：① 标记副本为"draining"（停止接受新请求，已在 K8s 中可通过 readiness probe 控制）；② 等待在途请求完成——设置 drain_timeout = max_expected_sequence_remaining_time（如 300s）；③ 超时后强制完成未完成的请求（截断到当前长度并返回）；④ 释放副本。Drain 期间副本继续服务在途请求，但不占用新请求的调度预算。
>
> 缩容判据（保守策略）：连续 300s（冷却窗口）所有信号低于阈值（QPS < 25%、GPU util < 35%、KV cache < 40%），且预测未来 600s 不需要（基于历史同期负载模式）。缩容比例 = max(1, floor(replicas × (1 - avg_util / target_util)))。例如当前 20 副本、平均 util 30%、目标 util 60%，则缩到 10 副本。
>
> 成本模型：总成本 = Σ(replicas × GPU_price_per_hour)。约束：P99 TTFT < SLO_TTFT, P99 TPOT < SLO_TPOT。成本优化 = min(replicas) subject to SLO constraints。经验公式：replicas_needed = ceil(QPS × avg_TTFT / (1 - safety_margin))。安全边际 20-30% 应对突发。
>
> 预测性缩容：基于历史同期（同一小时、同一工作日/周末）负载曲线，提前 10 分钟缩容。预测误差用过去 7 天同期数据的标准差度量——如果标准差 > 30% 均值，不执行预测性缩容（不确定性太高）。
>
> 成本优化技巧：① Spot/Preemptible GPU 用于 non-critical 租户（可中断，价格低 60-70%）；② 缩容到最低副本数 ≠ 0（保留 1-2 副本处理长尾请求）；③ 合并低 QPS 时段的请求到更少副本（增大 batch，提升 util）。

**技术洞察：**

```text
Drain 流程：
  1. Mark draining → stop new requests (readiness probe=false)
  2. Wait for in-flight (drain_timeout = 300s)
  3. Force-complete expired requests (truncate + return)
  4. Release replica

缩容判据：
  All signals < threshold for 300s AND
  Predicted_load(next_600s) < threshold
  N_scale_in = floor(N × (1 - util_avg / util_target))

成本模型：
  min Σ(replicas × price_per_hour)
  s.t. P99_TTFT < SLO, P99_TPOT < SLO
  replicas_needed = ceil(QPS × TTFT_avg / (1 - safety))

预测性缩容：
  Predict based on same-hour last-7d average
  Cancel if std/mean > 30% (too uncertain)
  Pre-scale 10min ahead

Drain 资源核算：
  draining 副本不计入可用容量（不接新请求）
  但仍占用 GPU 成本（直到释放）
  drain 期间成本 = N_draining × T_drain × GPU_price
```

C：drain 流程与缩容判据设计。I：成本与副本数公式。U：实际 drain 时间分布与预测准确性需实测。

**图文说明：** 时序图：负载下降 → 触发缩容判断（300s 冷却）→ 标记副本 draining → 等待在途完成 → 释放副本。下方成本曲线：GPU 使用量随时间变化，标注过度供给区间（浪费）与供给不足区间（SLO 风险）。旁注：draining 副本的"灰色区域"——仍在消耗 GPU 但不接受新请求。

**方案对比与选型：** 三种缩容策略对比：① 立即缩容（信号低于阈值立即回收副本）：实现简单但容易震荡——缩容后负载立即上升又触发扩容。仅适合负载缓慢下降且可接受短暂 SLO 违约的场景；② 冷却缩容（推荐）：信号低于阈值持续 N 秒后才缩容。冷却窗口通常 300-600s。优点：自然防震荡；缺点：缩容期间多付 300-600s 的闲置 GPU 成本。对 10 副本 × A100 场景，600s 闲置成本 ≈ $3-5（A100 $2/h），远低于震荡成本；③ 阶梯缩容（信号越低缩越多）：QPS < 50% 缩 20%，QPS < 30% 缩 40%。好处是逐步试探，坏处是策略复杂且低负载时可能缩过头。选型建议：② + ③ 组合——冷却窗口内线性缩容（不阶梯），冷却窗口结束后如果信号仍低则追加缩容。成本优化：非高峰期（凌晨 2-6 点）使用更激进的缩容策略（冷却 60s），因为负载回升风险最低。

**拓展知识：** 与 I-A016 衔接：灰度发布期间新副本加入、旧副本 drain 退出——缩容与发布共享 drain 机制；与 I-A006 衔接：多租户缩容需按租户优先级决定哪些租户的副本先回收；与 I-A014 衔接：缩容决策应记录完整快照到 metrics，事后分析"如果晚 5 分钟缩容是否 SLO 会违约"以优化冷却窗口。

**常见追问及回答要点：**

1. **问：drain 期间如果长序列请求一直不完怎么办？** 答：设置 drain_timeout（300s），超时后强制截断。客户端收到截断响应后可决定是否重试。对 VIP 租户可延长 drain_timeout 或转移请求到非 draining 副本。
2. **问：如何避免"缩容-扩容"震荡？** 答：① 冷却窗口 > 2× 扩容延迟；② 缩容步长限制（一次最多缩 30% 副本）；③ 如果缩容后 60s 内触发扩容，回退缩容并告警。
3. **问：spot 实例的推理服务如何设计？** 答：spot 实例上部署 non-critical 租户，drain 策略更激进（spot 中断预警 2min 内完成 drain）。critical 租户仅在 on-demand GPU 上运行。
4. **问：draining 副本的资源是否计入 autoscaling 决策？** 答：不计入。draining 副本不再接受新请求，其 GPU 处于"保留但不可用"状态。autoscaling 决策只基于 active（非 draining）副本的信号。这避免了"draining 副本信号低导致误缩更多副本"的问题。

**易错点与反例：**

- 反例："负载低就立即缩容"——导致震荡。必须加冷却窗口和预测验证。
- 反例："drain 时 kill 所有在途请求"——破坏可用性。drain 必须优雅。
- 易错：忽视 drain 副本的资源占用——draining 副本仍消耗 GPU，缩容收益有滞后。如果 drain 时间过长（如长序列），实际节省的成本低于预期。
- 易错：drain 期间路由层未及时更新——新请求仍被路由到 draining 副本。解决方案：readiness probe 与 service endpoint 同步更新（K8s endpoint controller 延迟 < 2s），或路由层主动轮询副本状态（长轮询，延迟 < 5s）。

**实践与验证：** [U] 注入周期性负载（正弦波 + 随机噪声），测量：(a) 实际副本数 vs 理想副本数的差距（avg < 15%）；(b) SLO 达标率（> 99%）；(c) 每小时成本节省（vs 固定副本基线，目标 > 25%）；(d) drain 超时事件频率（< 1 次/天）；(e) 震荡事件数（< 1 次/周）。oracle：成本节省 > 25% 且 SLO 达标率 > 99%，零震荡。当前未执行。

**分层评分标准：** 4分：能设计基本 drain 流程与缩容判据；6分：能建立成本模型并量化副本需求；8分：能设计预测性缩容、防震荡、spot 实例利用；10分：能在多租户多模型约束下设计完整的弹性体系，包括 drain 资源核算与路由同步。

**证据与来源：** `sglang/90-cross-module/pooling-and-resource-management.md`（资源池化与弹性）；`sglang/99-roadmap/performance-guide.md`（性能 SLO）；`insights/05-sglang-inference-technical-insights/`（成本优化实践）。

**补充：缩容时的租户感知与迁移策略。** 多租户场景下缩容不能简单回收最低优先级副本——如果该副本上运行的是高优先级租户的长序列请求，drain 时间会远超预期。优化方案：① 租户感知缩容——优先回收当前只服务低优先级租户的副本（drain 时间短，因为低优先级租户的序列通常较短）；② 请求迁移——将高优先级租户的请求从待回收副本迁移到其他副本（利用 KV cache 的跨节点传输，参考 I-A011），代价是迁移期间的短暂延迟增加（约 13ms）；③ 缩容调度器——维护"缩容优先级分数" = Σ(tenant_priority × remaining_tokens)，分数最低的副本优先缩容。量化：租户感知缩容可将平均 drain 时间从 180s 降到 60s，因为优先回收的副本上是在途请求最少的。

---

### I-A016 设计推理服务的灰度发布与回滚：兼容性验证矩阵与零停机升级。

**元数据：** 知识域：`I17 灰度发布与回滚`；能力目标：`C17 零停机升级`；题型：架构设计与流程；频率：高频；前置/后续：`I-A001`（配置一致性）、`I-A015`（优雅缩容）；证据状态：发布流程 C，验证矩阵 C，运行结论 U。

**背景知识：** 推理服务升级（新版本代码、新模型权重、新调度参数）需要零停机、可灰度、可回滚。挑战：新旧版本对同一请求可能产生不同输出（模型升级）、新版本的显存需求可能不同、新旧版本的调度行为差异导致负载不均。实际生产中，推理服务的发布频率约每周 1-3 次（bugfix/优化）到每月 1 次（模型大版本升级）。每次发布的灰度验证时间通常为 30min-4h（取决于变更风险等级）。典型故障包括：新版本内存泄漏在 30 分钟后才显现（灰度时间不够）、新旧版本的输出格式差异导致下游系统解析失败（兼容性遗漏）、以及回滚后发现旧版本配置已被覆盖。

**问题分析：** 1) 灰度策略：按流量比例、按租户等级、按请求类型灰度；2) 兼容性验证：新旧版本的输出一致性、性能一致性、显存行为一致性；3) 回滚机制：版本回退的触发条件与自动化流程；4) 零停机：滚动升级期间的服务连续性保障；5) 配置与代码的联合回滚——确保回滚后行为完全恢复到升级前状态。

**具体答案：** 面试中可以这样回答：

> 灰度三阶段。阶段一（5% 流量）：新版本副本加入集群，接受 5% 流量，持续 30 分钟。监控：请求成功率、TTFT/TPOT 分布、OOM 事件、异常日志。对比基线（旧版本同期指标），如果 P99 TPOT 增加 > 10% 或 OOM 事件 > 0，自动回滚。阶段二（30% 流量）：扩大到 30%，持续 2 小时。重点观察长序列请求与高峰负载下的稳定性。阶段三（100%）：全量切换，旧版本副本 drain 退出。
>
> 兼容性验证矩阵（升级前在 staging 环境完成）：
> - 输出一致性：1000 条测试用例，新旧版本对比输出——代码/数学用例要求 100% 一致（T=0），创意用例允许差异但任务得分不降。
> - 性能一致性：相同负载下，新版本 throughput 不降 > 5%。
> - 显存一致性：新版本 mem_fraction_static 需求不超过旧版本 + 5%。
> - API 兼容性：I-A002 协议矩阵全量通过。
>
> 回滚判据（满足任一即自动回滚）：① 请求错误率 > 基线 × 2；② P99 TPOT > 基线 × 1.15；③ OOM 事件 > 0；④ 新版本崩溃/重启次数 > 1。回滚流程：① 新版本副本停止接收新流量（路由层秒级切换）；② 新版本副本 drain（300s 超时）；③ 旧版本副本已保留在 drain 状态（升级时保留 10% 旧版本作为热备），直接恢复接收流量。回滚总时间 < 30s。
>
> 零停机保障：滚动升级每次只替换 1 个副本（或 10%），替换过程中其他副本继续服务。最小可用副本数 = ceil(total_replicas × 0.7)——确保 30% 容量冗余应对升级期间副本不可用的情况。

**技术洞察：**

```text
灰度三阶段：
  Stage 1: 5% × 30min, 关键指标对比
  Stage 2: 30% × 2h, 稳定性观察
  Stage 3: 100%, 旧版本 drain

回滚判据：
  error_rate > baseline × 2
  P99_TPOT > baseline × 1.15
  OOM_events > 0
  crash_count > 1

回滚时间线：
  T_detect = 30s (metrics window)
  T_route_switch = 5s (load balancer)
  T_total ≈ 35s

热备策略：
  升级时保留 10% 旧版本副本（draining but ready）
  回滚时热备副本立即恢复 (T_recover < 5s)
  热备成本 = 10% × GPU_cost (24h 保留窗口)

配置与代码联合回滚：
  配置版本号绑定代码版本
  代码回滚 → 同步回滚到对应配置版本
  避免"旧代码 + 新配置"的未知组合
```

C：灰度发布流程与回滚判据。I：回滚时间线分解。U：具体阈值与回滚时间需实测。

**图文说明：** 时序图：旧版本 100% → Stage1（95%旧+5%新）→ Stage2（70%旧+30%新）→ Stage3（100%新，旧drain）。回滚路径用红色虚线从 Stage2/3 返回。旁注：热备旧版本副本的位置与恢复时间。右侧：配置版本与代码版本的绑定关系图。

**方案对比与选型：** 三种灰度路由策略对比：① 随机分流（按概率路由到新/旧版本）：实现简单（load balancer 配置权重），但同一请求的重试可能路由到不同版本——如果新版本有 bug，重试可能"自愈"，掩盖问题；② 一致性哈希（按 user_id/request_id 哈希路由）：同一用户始终路由到同一版本，体验一致。但某些用户被分到新版本后如果版本有 bug，该用户持续受影响——影响面集中；③ 染色路由（按租户等级分流）：VIP 租户走旧版本（稳定性优先），普通租户走新版本（覆盖面大）。好处是新版本问题影响非 VIP 用户，VIP 体验不受影响。选型建议：③ + ② 组合——非 VIP 租户用一致性哈希分流（同一租户内体验一致），VIP 租户在 Stage1/2 走旧版本，Stage3 确认稳定后 VIP 也切换。成本：灰度期间同时维护两版本副本，成本增加 10-20%（取决于保留的旧版本副本数），持续 2-4 小时。

**拓展知识：** 与 I-A001 衔接：灰度发布时配置变更也需要灰度——新参数先在小部分副本验证；与 I-A002 衔接：协议变更的灰度也需要类似的多阶段验证；与 I-A014 衔接：灰度期间的新旧版本 metrics 对比是发布决策的核心依据——需要在 trace 中标记 `version_tag` 以支持分组聚合。

**常见追问及回答要点：**

1. **问：新旧版本输出不完全一致（模型升级），怎么判断是退化还是改进？** 答：建立评估基准集（benchmark），包含正确性用例（代码执行、数学计算、事实问答）和创意性用例（摘要质量、翻译质量）。正确性用例退化 = 回滚；创意性用例改进 = 推进。
2. **问：升级过程中新副本 OOM 了，是不是新版本的问题？** 答：不一定。可能是新副本的 KV cache 调度尚未"预热"（所有请求同时涌入新副本）。解决方案：新副本加入时先限流到 50% 容量，运行 5 分钟稳定后放开。
3. **问：回滚后已写入下游的数据怎么办？** 答：推理服务的响应是幂等的（相同输入 + 相同采样参数 = 相同输出，T=0 时）。回滚不影响已返回的结果。如果下游依赖推理结果做写入，建议下游实现幂等性（基于 request_id 去重）。
4. **问：配置升级但代码不升级（或反之）如何处理？** 答：配置变更和代码变更共享同一灰度流程——配置变更通过 I-A001 的配置灰度推送实现，代码变更通过滚动升级实现。如果两者同时变更，使用统一的版本号绑定（config_version + code_version = release_version），确保回滚时配置和代码同步回退。

**易错点与反例：**

- 反例："直接全量切换新版本"——风险极大。新版本可能有未发现的边界 case，全量切换导致全局故障。
- 反例："回滚只回代码，不回配置"——错误。如果新版本同时改了调度参数，回滚代码不回配置会导致行为不一致。
- 易错：忽视"热备旧版本"的成本——保留 10% 旧版本意味着升级期间多付 10% GPU 成本，需要在成本与回滚速度间权衡。
- 易错：灰度期间的 metrics 对比未排除"预热效应"——新副本刚启动时的前几分钟指标可能异常（JIT 编译、cache 预热），不应纳入灰度对比。建议新副本启动后 2 分钟的 metrics 标记为"warmup"并排除。

**实践与验证：** [U] 模拟升级流程：部署 v1 服务 → 注入持续负载 → 启动 v2 灰度（分三阶段）→ 人为注入故障验证回滚。测量：(a) 灰度期间 SLO 达标率（全程 > 99%）；(b) 回滚总时间（< 60s）；(c) 零停机验证（是否有请求丢失，目标零丢失）；(d) 配置与代码联合回滚正确性（回滚后行为与升级前 100% 一致）。oracle：全程 SLO > 99%，回滚 < 60s，零请求丢失，配置同步回滚正确。当前未执行。

**分层评分标准：** 4分：能设计基本的灰度发布流程；6分：能建立回滚判据与验证矩阵；8分：能设计热备策略、零停机保障、成本权衡；10分：能设计配置与代码联合回滚、预热效应排除、以及完整的发布风险分级体系。

**证据与来源：** `sglang/01-modules/M18-testing-benchmark-ci.md`（CI 验证）；`sglang/90-cross-module/pooling-and-resource-management.md`（资源管理）；`insights/04-design-decision-and-validation/`（发布决策）。

---

### I-A017 设计推理服务的多租户安全加固：限流、隔离与 DoS 防护。(U)

**元数据：** 知识域：`I18 多租户安全加固`；能力目标：`C18 安全防护`；题型：安全架构设计；频率：中频；前置/后续：`I-A006`（多租户隔离）、`I-A012`（多模型共池）；证据状态：安全策略 C，运行场景 U。

**背景知识：** 推理服务面临的安全威胁：① 资源耗尽型 DoS（发送超长 prompt 或大量并发请求消耗 GPU 资源）；② 提示注入（prompt injection 绕过系统指令）；③ 数据泄露（跨租户 KV cache 残留、模型输出泄露其他租户信息）；④ 对抗样本（精心构造的输入导致 OOM 或超长输出）。防护需在请求入口、调度层、执行层多层部署。实际生产中，推理服务的安全事件频率约每月 1-5 次（大多数是试探性攻击，少量为有目的的 DoS）。典型攻击模式包括：自动化脚本发送大量短请求消耗 RPS 限额、使用超长 prompt（接近 max_tokens 上限）试图触发 OOM、以及在 prompt 中嵌入"ignore previous instructions"等注入模式。安全加固的投入产出比需要量化——过度防护增加延迟和成本，不足则导致安全事故。

**问题分析：** 1) 限流：租户级 + 全局级的请求频率与计算预算限制；2) 隔离：KV cache 物理隔离、权重隔离、网络隔离；3) DoS 防护：超长/异常请求的检测与拒绝；4) 成本量化：安全机制引入的延迟与资源开销；5) 安全审计——如何记录和追溯安全事件。

**具体答案：** 面试中可以这样回答：

> 多层防护体系。第一层（入口网关）：① 请求级限流——每租户 RPS 限制（令牌桶算法），超限返回 429；② 输入验证——prompt 长度硬上限（如 100K tokens），超限直接拒绝；③ 并发限制——每租户最大并发请求数（如 100），防止单租户占满 GPU。
>
> 第二层（调度器）：④ 计算预算——每租户每秒钟的 token 处理预算（prefill + decode tokens），超限排队；⑤ 优先级调度——付费等级高的租户优先获得 prefill 时间片；⑥ 异常检测——短时大量请求触发警报并临时降级该租户。
>
> 第三层（执行层）：⑦ KV cache 物理隔离——每租户独立的 KV cache 池（I-A006），防止残留数据跨租户读取；⑧ 输出过滤——检测并脱敏输出中的敏感信息（PII 检测）；⑨ 沙箱执行——代码执行类请求在隔离容器中运行，限制系统调用与网络访问。
>
> DoS 防护专项：① 超长 prompt 攻击——gateway 层 < 1ms 内完成长度检查，不进入调度器；② Slowloris 型攻击——连接超时（10s）与请求体超时（30s）；③ 对抗性 OOM 攻击——mem_fraction_static + max_num_tokens 硬限制，单请求最大显存使用 ≤ 单租户配额的 10%。
>
> 量化安全开销：限流检查延迟 < 0.5ms/请求；KV cache 隔离损失约 5% 显存利用率（池间碎片）；PII 检测延迟 1-3ms/请求。总安全开销 < 2% 端到端延迟。
>
> 安全审计：所有安全事件（限流触发、注入检测命中、OOM 防护、异常请求）以独立审计日志输出，包含 tenant_id、timestamp、action、reason。审计日志保留 90 天，用于事后分析与合规。

**技术洞察：**

```text
三层防护：
  L1 Gateway: RPS limit (token bucket), input validation, concurrency cap
  L2 Scheduler: compute budget, priority scheduling, anomaly detection
  L3 Execution: KV isolation, output filtering, sandboxing

DoS 防护：
  Long prompt: gateway reject > 100K tokens, < 1ms check
  Slowloris: connect_timeout=10s, body_timeout=30s
  Adversarial OOM: per-request KV cap = 10% of tenant quota

安全开销：
  Rate limiting: < 0.5ms/req
  KV isolation: ~5% memory fragmentation loss
  PII filtering: 1-3ms/req
  Total overhead: < 2% e2e latency

资源耗尽防御：
  per_tenant_RPS ≤ global_RPS × tenant_share
  per_tenant_concurrent ≤ global_max × tenant_share
  per_request_tokens ≤ tenant_budget_per_sec / active_requests

审计日志：
  事件类型: rate_limit, injection_detected, oom_prevented, anomaly
  保留期: 90 天
  存储: 独立审计存储（与业务日志分离）
```

C：多层防护架构与限流算法。I：安全开销量化。U：实际攻击模式与防御效果需渗透测试验证。

**图文说明：** 分层图：Client → Gateway（限流/验证）→ Scheduler（预算/优先级）→ Engine（KV 隔离/过滤）→ GPU。每层标注安全机制与延迟预算。红色标注攻击路径与阻断点。旁注：审计日志的独立存储路径。

**方案对比与选型：** 三种限流算法对比：① 固定窗口计数器（每 N 秒重置计数）：实现简单但窗口边界突发——窗口切换瞬间可能允许 2x 流量涌入；② 滑动窗口对数（Redis + Lua）：精确限流，每请求检查过去 N 秒内的请求数。精确但 Redis 调用增加 1-5ms 延迟；③ 令牌桶（推荐）：以固定速率填充令牌，请求消耗令牌。允许突发（桶满时），但长期速率受控。本地内存实现（无需 Redis），延迟 < 0.1ms。选型建议：③ 令牌桶——推理服务的请求不是严格周期性的，需要允许合理突发（如用户快速输入多条消息），令牌桶天然支持突发吸收。参数设置：填充速率 = tenant_RPS_limit × 1.2（20% 突发余量），桶容量 = tenant_RPS_limit × 2（允许 2s 突发）。防护纵深：单层限流不够——Gateway 层做 RPS 限制，Scheduler 层做 token 预算限制，两层独立配置。即使攻击者绕过 Gateway（如内网请求），Scheduler 层的预算限制仍生效。

**拓展知识：** 与 I-A006 衔接：KV cache 隔离是多租户安全的核心——物理隔离防止数据泄露，配额隔离防止资源耗尽；与 I-A012 衔接：多模型共池场景下安全隔离维度增加——需要按租户×模型做二维隔离；与 I-A014 衔接：安全事件（限流触发、注入检测命中、OOM 防护）应作为独立 trace 类型上报，与普通请求 trace 分离存储，便于安全审计。

**常见追问及回答要点：**

1. **问：KV cache 残留如何彻底清除？** 答：租户请求完成后，其占用的 KV cache 页立即置零（memset）再释放回池。防止下一个租户分配到未清零的页而读取到前租户的 KV 信息。开销：memset 1GB 约 1-2ms（GPU 带宽限制），可接受。
2. **问：prompt injection 如何防护？** 答：① 输入侧：检测并拒绝包含已知注入模式的请求（规则 + 分类器）；② 输出侧：过滤包含注入特征（如 "ignore previous instructions"）的输出；③ 模型侧：使用经过安全对齐的模型，降低注入成功率。
3. **问：租户 A 故意发送大量短请求耗尽全局 RPS 限额怎么办？** 答：RPS 限额按租户维度独立计算，但全局也有总 RPS 上限。当租户 A 触及自己的 RPS 上限时返回 429，不影响租户 B。全局 RPS 上限触发时按优先级降级低等级租户。
4. **问：安全审计日志的存储成本如何控制？** 答：安全事件频率远低于业务请求（约 0.01%），审计日志量 ≈ 业务日志 × 0.01%。使用独立低成本存储（如 S3 Standard-IA），年成本 < $1K。审计日志不参与实时查询，仅用于事后分析与合规，因此无需高性能存储。

**易错点与反例：**

- 反例："限流只按 RPS"——不充分。需要同时限制计算量（tokens/s），因为 1 个 100K prompt 请求与 1 个 10 token 请求的计算量差 10000 倍。
- 反例："KV cache 释放即安全"——错误。释放到池后页内容仍存在，需显式清零。
- 易错：忽视 prompt injection 通过间接渠道（如用户上传文档中的隐藏指令）——需要对所有输入来源做检查。
- 易错：安全机制的"误报"成本——如果 PII 过滤器过度敏感，可能将正常输出中的技术术语误判为敏感信息，导致输出被截断。需要可调节的敏感度阈值与人工申诉流程。

**实践与验证：** [U] 渗透测试：① 发送超长 prompt（100K-1M tokens）验证 gateway 拒绝（延迟 < 1ms）；② 发送大量并发请求（1000 QPS）验证限流生效（超限请求 429）；③ 交叉租户请求（租户 A 尝试读取租户 B 的 KV cache）验证隔离有效性（零泄露）；④ 注入 DoS 负载（持续 1000 QPS 30min）验证 autoscaling + 限流联合防护。oracle：所有安全机制生效，正常租户 SLO 不受攻击影响，审计日志完整记录所有事件。当前未执行渗透测试。

**分层评分标准：** 4分：能设计基本的限流与隔离机制；6分：能建立多层防护体系并量化安全开销；8分：能处理 prompt injection、KV 残留清除、对抗性攻击；10分：能设计安全审计体系、合规流程、以及安全机制的误报/漏报分析框架。

**证据与来源：** `sglang/01-modules/M08-kv-cache.md`（KV cache 隔离机制）；`sglang/90-cross-module/pooling-and-resource-management.md`（多租户资源管理）；`insights/05-sglang-inference-technical-insights/`（安全实践）。

---

### I-A018 设计 KV cache 的跨节点共享方案：分布式 KV 池的一致性与一致性代价。

**元数据：** 知识域：`I18 分布式 KV 共享`；能力目标：`C18 分布式缓存`；题型：架构设计与权衡；频率：中频；前置/后续：`I-A011`（PD 分离 KV transfer）、`I-A008`（跨机分布式）；证据状态：设计模式 C，一致性模型 C，运行结论 U。

**背景知识：** 跨节点 KV cache 共享指将 KV cache 存储在独立节点（KV cache pool），推理节点通过网络访问远程 KV。优势：① 显存解耦——推理节点不需要大容量显存存 KV；② 共享效率——多个推理节点可共享同一 KV 前缀（如 system prompt）。挑战：① 远程访问延迟（网络 vs 本地显存）；② 一致性（多个推理节点同时读写同一 KV）；③ 带宽成本（高频 decode 需要读取 KV cache，网络带宽成为瓶颈）。实际生产中，分布式 KV 池的主要应用场景包括：跨节点的 system prompt 共享（节省重复 prefill 计算）、以及弹性 KV 存储（推理节点故障后 KV 不丢失，新节点可接管）。典型故障包括：远程 KV 访问延迟波动导致 TPOT 毛刺、多推理节点缓存同一 KV 页的版本不一致、以及 KV 池节点故障导致大量推理节点 page fault。

**问题分析：** 1) 共享粒度：哪些 KV 适合跨节点共享（公共前缀 vs 私有后缀）；2) 一致性模型：强一致（每次读取都验证）vs 最终一致（允许短暂 stale）；3) 缓存拓扑：本地 L1 缓存 + 远程 L2 缓存的层次化设计；4) 量化：远程 KV 访问延迟对 TPOT 的影响；5) KV 池节点的高可用——单节点故障时的降级策略。

**具体答案：** 面试中可以这样回答：

> 共享粒度分层。第一层（共享前缀池）：多个请求共享的 KV 前缀（如 system prompt、文档检索结果）存入分布式 KV 池，推理节点只读访问。更新频率低（prefix 不变），可用最终一致性。第二层（私有 KV）：每个请求独有的 KV 留在推理节点本地显存，避免网络延迟影响 decode 性能。分割点：shared_prefix_length = min(公共前缀长度, 4096 tokens)。
>
> 一致性模型：共享前缀池用**版本号 + 读时验证**——推理节点缓存本地副本，每次使用前检查版本号是否变化。版本号变化时从远程池拉取新版本。私有 KV 无一致性问题（单推理节点独占）。写操作：仅 prefill 节点写入 KV 池（append-only），推理节点只读——避免写冲突。
>
> 层次化缓存：推理节点维护 L1 本地 KV cache（ hottest 的 KV 页）+ L2 远程 KV pool。L1 命中率是关键——如果 shared prefix 的 L1 命中率 > 95%，远程访问不影响 TPOT。L1 淘汰策略：LRU + access frequency。L1 容量 = GPU 显存的 20-30%。
>
> 量化：远程 KV 访问延迟 = 网络 RTT + 数据读取时间。对 70B 模型，单步 decode 需读取的 KV ≈ 320KB/token × 1 token = 320KB（单步只需最新一层的 KV，但实际需全部层）。完整读取 80 层 × 320KB = 25.6MB。在 100Gbps RDMA 下 ≈ 2ms。对比本地显存访问（HBM 3TB/s）≈ 0.01ms。远程访问延迟是本地 200x。
>
> 这意味着：每步 decode 都远程访问 KV 不可行（TPOT 从 20ms → 400ms+）。解决方案：① 仅共享前缀部分远程访问（一次性，分摊到多步）；② 私有 KV 必须本地；③ 远程池主要服务 prefill 阶段的 KV 存储与跨节点传输，不服务实时 decode。

**技术洞察：**

```text
KV 共享分层：
  Shared prefix (L2 remote): append-only, versioned, eventual consistency
  Private KV (L1 local): exclusive access, no consistency needed
  Split point: shared_len = min(prefix_len, 4096)

层次化缓存：
  L1 (local GPU): 20-30% KV cache budget, LRU eviction
  L2 (remote pool): full KV storage, RDMA access
  Target L1 hit rate > 95% for shared prefix

远程访问延迟（避免实时 decode 使用）：
  KV_per_step = L × 2 × n_kv × d_head × dtype
  70B: 80 × 2 × 8 × 128 × 2 = 320KB/token, full layers = 25.6MB/step
  T_local = 25.6MB / 3TB/s ≈ 0.01ms
  T_remote = 25.6MB / 12.5GB/s + RTT ≈ 2ms + 0.1ms
  Ratio: 200x → unacceptable for per-step decode

KV 池高可用：
  多副本（3x replication）或 EC(4+2)
  单节点故障 → 读自动切换到副本
  写入需 quorum 确认（W=2 of 3）
  故障切换时间 < 5s
```

C：KV 共享分层与一致性模型。I：远程 vs 本地延迟对比。U：实际远程访问延迟与命中率需实测。

**图文说明：** 架构图：多个 Inference Node（各带 L1 local KV）→ RDMA 网络 → Distributed KV Pool（L2 shared, 3x replication）。箭头标注：prefill write → pool；decode read → L1 hit (fast) / L2 miss (slow, 2ms)。右侧柱状图：TPOT 对比（本地 20ms vs 远程 400ms+），标注不可接受区域。旁注：KV 池的高可用架构——3 节点 replication。

**方案对比与选型：** 三种分布式 KV 架构对比：① 集中式 KV 池（所有 KV 存入独立存储节点）：推理节点完全不存 KV，每步都远程读取。延迟最高（每步 2-5ms），但显存成本最低（推理节点只需存权重）。仅适合 decode 频率极低的场景（如批量离线推理）；② 缓存式 KV 池（推荐）：推理节点维护 L1 热缓存 + 远程 L2 全量。L1 命中率 > 95% 时延迟接近本地。适合在线推理——私有 KV 全部在 L1（无远程访问），共享前缀通过 L1/L2 层次化缓存（prefill 时从 L2 加载到 L1）；③ 去中心化 KV 分片（一致性哈希分片到推理节点）：无独立 KV 池节点，KV 分散在各推理节点。推理节点间互相读取远程 KV。无单点瓶颈但一致性复杂（多个推理节点缓存同一 KV 页的版本同步）。选型建议：② 缓存式最优——共享前缀写入远程池，推理节点按需缓存到本地。KV 池节点使用高内存服务器（2TB DDR5）而非 GPU 服务器（成本更低——存储 KV 不需要 GPU 算力）。成本模型：1TB KV 池存储 ≈ 500K 请求的 KV cache（每请求平均 2MB KV），DDR5 服务器年成本 ≈ $10K，等效于 5 台 GPU 服务器 1 小时的 GPU 成本。

**拓展知识：** 与 I-A011 衔接：PD 分离是 KV 共享的特例（KV 从 prefill 节点传输到 decode 节点，然后本地化）；与 I-A006 衔接：多租户场景下分布式 KV 池需要按租户分片；与 I-A013 衔接：长上下文场景下分布式 KV 池是 eviction-to-disk 的自然延伸——远程 KV 池提供比本地 NVMe 更大的存储容量。

**常见追问及回答要点：**

1. **问：远程 KV 池能否服务实时 decode？** 答：不能。每步 decode 读取全部层的 KV（25.6MB），远程延迟 2ms vs 本地 0.01ms。即使 100Gbps RDMA，延迟差 200x。只能用于一次性传输（如 PD 分离的 KV transfer）或前缀共享（命中本地 L1 缓存）。
2. **问：共享前缀的一致性如何保证多节点读取不冲突？** 答：前缀 KV 是 append-only 写入（prefill 完成后不再修改），多推理节点只读。如果 system prompt 更新，新版本写入 KV 池新版本号，推理节点下次拉取时自动升级。旧版本在推理节点缓存中自然淘汰。
3. **问：分布式 KV 池的带宽如何规划？** 答：主要带宽需求在 prefill 写入阶段。QPS=100、avg_prompt=2000、KV_per_token=320KB：写入带宽 = 100 × 2000 × 320KB = 64GB/s。需多网卡聚合（4×200Gb/s RDMA）或降低写入带宽（分层传输——只传 hot layers）。
4. **问：KV 池节点故障时如何降级？** 答：① 读故障——自动切换到副本（< 5s），推理节点从副本读取；② 写故障——prefill 节点缓存写入请求，待写 quorum 恢复后重放；③ 全池故障——推理节点回退到本地-only 模式（不接受新的共享前缀请求，已有请求使用 L1 缓存继续服务）。降级时间目标 < 10s。

**易错点与反例：**

- 反例："KV cache 全部放远程池，本地不存"——不可行。实时 decode 的延迟无法接受。
- 反例："远程 KV 池用一致性哈希分布"——过度。KV 共享以 prefix 为单位，不需要 key-level 分片。
- 易错：忽视 KV 池的写入带宽——prefill 阶段大量并发写入可能成为瓶颈，需要流控。
- 易错：多推理节点缓存同一 KV 页的 cache 一致性——如果推理节点 A 缓存了 v1 版本的 system prompt KV，而推理节点 B 已升级到 v2，两者行为不一致。解决方案：版本号 + TTL——缓存项在 TTL 过期后强制重新验证版本号（TTL = 60s），确保最终一致。

**实践与验证：** [U] 部署分布式 KV pool（3 节点 RDMA），测量：(a) prefix 共享的 L1 命中率与 TPOT 影响（命中率 > 95%）；(b) RDMA 远程读取延迟分布（P50/P95/P99 < 5ms）；(c) KV 写入带宽饱和点（QPS 达到多少时写入延迟 > 50ms）；(d) KV 池节点故障时的降级时间（< 10s）与推理节点 SLO 影响（TPOT P99 变化 < 10%）。oracle：L1 命中率 > 95%，远程读取 P99 < 5ms，降级 < 10s。当前未执行。

**分层评分标准：** 4分：能描述 KV 共享的基本动机与挑战；6分：能设计分层缓存与一致性模型；8分：能量化远程访问延迟影响并给出可行的使用边界与 KV 池高可用方案；10分：能在多租户多模型长上下文约束下设计完整的分布式 KV 体系，包括一致性协议与故障降级。

**证据与来源：** `sglang/01-modules/M13-disaggregation-hicache.md`（PD 分离与 HiCache）；`sglang/01-modules/M08-kv-cache.md`（KV cache 管理）；`insights/arxiv-pdf/2408.08147/`（分布式推理论文）。
**补充：KV 池的容量规划与 eviction 策略。** 分布式 KV 池的容量规划不同于本地 KV cache——它只需存储"热"的共享前缀 KV（冷数据由本地 eviction 处理）。容量公式：pool_capacity = avg_active_sequences × avg_shared_prefix_len × KV_per_token × replication_factor。对 1000 并发请求、平均共享前缀 2K tokens、3x replication：1000 × 2000 × 320KB × 3 = 1.92TB。KV 池的 eviction 策略：按"最近使用时间 + 引用计数"加权——被多个推理节点引用的页保留时间更长。当池使用率 > 85% 时触发 eviction，优先淘汰引用计数 = 1 且最久未使用的页。

**补充：分布式 KV 池的容量规划与 eviction 策略。**

分布式 KV 池只需存储"热"的共享前缀 KV（冷数据由本地 eviction 处理）。容量公式：pool_capacity = avg_active_sequences × avg_shared_prefix_len × KV_per_token × replication_factor。

对 1000 并发请求、平均共享前缀 2K tokens、3x replication：1000 × 2000 × 320KB × 3 = 1.92TB。

KV 池的 eviction 策略：按"最近使用时间 + 引用计数"加权——被多个推理节点引用的页保留时间更长。当池使用率 > 85% 时触发 eviction，优先淘汰引用计数 = 1 且最久未使用的页。

高可用：KV 池使用 3x replication 或 EC(4+2)。3x replication 将存储成本乘以 3，但将可用性从 99.9% 提升到 99.997%。EC(4+2) 存储开销仅 1.5x，适合冷数据比例高的场景。
