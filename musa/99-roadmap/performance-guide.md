# 性能指南

- 文档目的：从源码可直接观察到的热路径出发，列出本代码库的性能关注点、测量维度与修改红线。
- 适用范围：`source/musa` 快照（5.3.0 develop）的内存管理、命令提交、API 固定开销三段。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）。
- 证据状态：**全部为静态分析，未验证**——本快照没有执行任何 benchmark、profiler 或目标机运行，本文不给出任何数字。
- 最后更新：2026-10-07
- 前置阅读：[`../90-cross-module/performance-critical-paths.md`](../90-cross-module/performance-critical-paths.md)
- 后续阅读：[`rca-splaytree-memmgr.md`](rca-splaytree-memmgr.md)、[`testing-recipes.md`](testing-recipes.md)

## 结论摘要

从代码结构看，性能敏感点集中在三处：**内存池查找**（每次分配/释放都要持 `m_Lock` 走一次 splay 树）、**命令提交与合并**（每条流一个提交线程，合并上限 32 条，依赖解析会遍历同 context 的其他流）、**每个 API 的固定开销**（`InitPlatform` + `ApiTrace` + 可选 tools 回调）。优先测量而不是猜测：先测 API CPU 时间、排队延迟、提交延迟、GPU 执行时间、同步等待时间与分配/回收耗时，再决定改哪一段。

---

## 1. 热路径地图

| 层 | 热点 | 频率特征 | 证据 |
|---|---|---|---|
| Driver | 每次 API 调用的 `InitPlatform` 前置检查 | 每调用一次 | [src/driver/internal.h:307-320] |
| Driver | 每次 API 调用的 `ApiTrace` 构造/析构 | 每调用一次 | [src/driver/mu_wrappers_generated.cpp:16-46] |
| Core | 默认流依赖遍历（遍历 context 内所有非阻塞流） | 每次默认流提交 | [src/musa/core/context.cpp:2036-2076] |
| Core | `Stream::QueueCommand` 的容量自旋与 `m_SubmitMtx` | 每条命令 | [src/musa/core/stream.cpp:1056-1071] |
| Core | `Stream::AsyncSubmit` 的合并/提交循环 | 每提交一批 | [src/musa/core/stream.cpp:1159-1310] |
| HAL | `MemMgr::Allocate` / `MemMgr::Free` 的池查找 | 每次分配/释放 | [src/hal/m3d/memMgr.cpp:81-147] |

---

## 2. 内存分配与释放

**分配**：`MemMgr::Allocate` 先做尺寸/溢出/总量校验，未指定池时用五字段 `MakeKey` 查 `m_PoolRefs`，命中失败或字段不一致就 `CreatePoolNoLock` 建池，最后 `FullAllocate` [src/hal/m3d/memMgr.cpp:81-147]。注意查找结果靠 117-121 行的字段逐一比对兜底——这不是可省的冗余，而是 `Get` 返回"最近节点"语义下的正确性保障。

**释放**：`MemMgr::Free` 同样按内存属性重建键并 `Get`，随后直接 `pMemoryPool->Free(...)`，只有一条 `MUSA_ASSERT` 保护 [src/hal/m3d/memMgr.cpp:149-161]。release 构建下断言被裁剪，这条路径的正确性完全依赖键命中。

**共同瓶颈**：`GetPool`/`FindPool`/`Allocate`/`Free` 四处都要 `std::lock_guard<std::mutex> lg(m_Lock)` [src/hal/m3d/memMgr.cpp:36-44]，即**分配与释放串行化在一把锁上**；锁内还要走一次 `SplayTree::Splay` 的旋转 [src/util/utilSplayTree.h:105-146]。多线程小对象分配场景下，这里是首个值得用 profiler 确认的位置。

> 静态分析，未验证：锁竞争比例、splay 旋转次数、池命中率都必须实测。

---

## 3. 命令提交与合并

1. **容量背压**：`QueueCommand` 在 `m_AsyncCount >= streamAsyncCapacity` 时 `yield` 自旋，容量来自 `Platform::Get().GetSettings().streamAsyncCapacity` [src/musa/core/stream.cpp:1056-1064]。容量调小→调用线程被拖慢；容量调大→积压命令占用内存。
2. **依赖解析**：命令发往默认流时，会遍历该 context 下**所有**非 barrier、非 non-blocking 的流并逐个 `RecordDependency`；barrier 流同理 [src/musa/core/context.cpp:2036-2076]。流数量越多，单次提交的固定成本越高。
3. **合并提交**：`AsyncSubmit` 把可合并命令攒进 `m_MergingList`，达到 `mergingListMaxSize = 32` 或引擎就绪才提交 [src/musa/core/stream.cpp:1286-1307]。合并减少提交次数，但会推迟单条命令的下发时机——小 kernel 高吞吐与低延迟之间就是这个常量在权衡。
4. **合并条件**：`DispatchCommand::CanMergeTo` 会因为 kernel 带 copy job（`hasDpCe`）或使用 spill 内存而拒绝合并 [src/musa/core/command/dispatchCommand.cpp:66-80]，这类 kernel 走独立提交路径。
5. **拷贝命令**：`AsyncMemcpyCommand::Submit` 遍历 `mergingList` 录入的参数并写 cmdBuffer，返回值逐条检查 [src/musa/core/command/AsyncMemcpyCommand.cpp:129-201]。
6. **同步等待**：`Stream::WaitFinish` 只等最后一条命令，但每次都会顺带调用 `UpdateUserPools` 触发池裁剪 [src/musa/core/stream.cpp:1118-1127]，高频 synchronize 会把 trim 成本带进等待路径。

---

## 4. 每次 API 调用的固定开销

- `InitPlatform`：检查 TLS 是否销毁、入口检查、`GetPlatform`，在**每个** `muapi*` 函数开头执行 [src/driver/internal.h:307-320]。
- `ApiTrace`：包装层为每次调用构造 guard，析构时写回返回值并在失败时落 `SetLastError` [src/driver/mu_wrappers_generated.cpp:16-46]。
- tools 回调：`toolsCallbackEnabled` 分支会构造 `params` 结构并计算 `ApiSeqNum`；只有订阅者存在时才进入 [src/driver/mu_wrappers_generated.cpp:16-46]。

结论（静态分析，未验证）：微基准里"空 API 调用"的基线成本应先单独测出来，否则容易把固定开销误判成业务路径开销。

---

## 5. 观测与日志开关

- `MUSA_LOG` 是位掩码，`Setting::Get()` 用函数内 `static` 缓存，只读一次环境变量；`std::stoul` 在非法输入时可抛异常（风险登记已列）[src/util/utilLog.h:92-101]。
- `tprintf(LOG_CMD, ...)` 覆盖入队、出队、合并、提交四个位置，**打开 `cmd` 位会直接在提交热路径上产生格式化与 IO**。修改热路径时禁止新增无界日志。
- 时间戳与 correlation（`SetQueuedTimestamp`、`SetSubmittedTimestamp`、`MUpti::AssignKernelToKick`）在提交路径内联执行，属于可采样的观测点；要评估其成本，需要单独开关对比 [src/musa/core/stream.cpp:1159-1310]。

---

## 6. 测量方案（建议，未执行）

1. **API CPU 时间**：对 `muMemcpyAsync`、`muMemAlloc_v2` 这类单次调用做循环计时，先扣掉第 4 节的固定开销。
2. **排队与提交延迟**：用 `queued`/`submitted` 时间戳差值观察 `m_CommandList` 停留时间 [src/musa/core/stream.cpp:1056-1104]。
3. **分配/释放**：单线程与多线程各跑一遍，观察 `m_Lock` 竞争（第 2 节）。
4. **流数量扫描**：固定命令数，扫描 context 内流数量，观察默认流依赖遍历的放大效应 [src/musa/core/context.cpp:2036-2076]。
5. **合并阈值**：把 32 这个常量作为实验变量（改前先读第 7 节红线）[src/musa/core/stream.cpp:1286-1307]。

以上全部为**建议**，本快照未执行，任何结果都不得写成已验证数据。

---

## 7. 修改红线

1. 改 `MemMgr` 查找逻辑前必须先看 [`rca-splaytree-memmgr.md`](rca-splaytree-memmgr.md)：`Get` 的"最近节点"语义与 `Find` 的空值语义不同，去掉字段比对会直接引出错池。
2. 改合并/依赖扫描前，必须做多流、多设备、同步/异步四象限对比，只测单流会得到相反结论。
3. 任何在 `QueueCommand`、`AsyncSubmit`、`MemMgr::Allocate` 中新增的日志、断言、锁，都要在关观测开关的构建下复测。
4. 性能结论一律标注环境与开关状态；本文件所有判断在拿到目标机数据前都是**静态分析，未验证**。

## 相关文档

- [`../90-cross-module/performance-critical-paths.md`](../90-cross-module/performance-critical-paths.md)
- [`risk-register.md`](risk-register.md)
- [`../00-overview/build-and-deploy.md`](../00-overview/build-and-deploy.md)

## 源码证据摘要

- 内存：[src/hal/m3d/memMgr.cpp:81-147]、[src/hal/m3d/memMgr.cpp:149-161]、[src/hal/m3d/memMgr.cpp:36-44]、[src/util/utilSplayTree.h:105-146]
- 提交：[src/musa/core/stream.cpp:1056-1071]、[src/musa/core/stream.cpp:1159-1310]、[src/musa/core/stream.cpp:1286-1307]、[src/musa/core/context.cpp:2036-2076]
- 合并：[src/musa/core/command/dispatchCommand.cpp:66-80]、[src/musa/core/command/AsyncMemcpyCommand.cpp:129-201]
- 固定开销与观测：[src/driver/internal.h:307-320]、[src/driver/mu_wrappers_generated.cpp:16-46]、[src/util/utilLog.h:92-101]

## 未解决问题

1. 缺少任何实测数据，本文的"热点"只是代码结构上的候选。
2. HAL 队列提交（`Hal::Queue::Submit` 之后）在私有子模块内，无法评估设备侧耗时。
3. 合并阈值、流数量对吞吐/延迟的影响没有实验记录。

## 下一步阅读建议

拿到目标机后先做第 6 节的五项测量，再回写 [`../90-cross-module/performance-critical-paths.md`](../90-cross-module/performance-critical-paths.md)；在那之前不要修改第 7 节红线中的任何一处。
