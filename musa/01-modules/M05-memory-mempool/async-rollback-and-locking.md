# M05 专题：异步分配回滚、访问更新与锁域

- 文档目的：《M05 专题：异步分配回滚、访问更新与锁域》的分析、证据与结论。
- 适用范围：M05 模块的《M05 专题：异步分配回滚、访问更新与锁域》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：本节重新只读核对远端 HEAD、`Stream::AsyncMemAlloc/AsyncMemFree` 和 Core pool 字段；访问更新依据此前读取的同版本实现。运行、故障注入及硬件结果未验证。
- 最后更新：2026-09-17
- 前置阅读：[实现分析](implementation.md)、[执行流](execution-flows.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)

## 1. 成功路径不是单个原子事务

```text
AsyncMemAlloc
  -> 选择 pool / SetStream
  -> CreateMemory：保留 VA，登记 tracker/set，增加 logical bytes
  -> physical::Init：创建 physical backing
  -> virt::Bind(spPhysical)
  -> ModifyAccess：组装 paging 并入队
  -> 返回 status
```

前三个资源阶段分别提交状态。VA 成功登记并不等于 physical 初始化成功；Bind 成功也不等于 paging 成功；paging 入队成功不等于目标 GPU mapping 已完成。[src/musa/core/stream.cpp:555-600]、[src/musa/core/memoryPool.cpp:380-410,301-345]

## 2. 失败回滚矩阵

| 失败阶段 | 已建立状态 | 可见回滚 | 尚需验证 |
|---|---|---|---|
| pool 为空 | 没有 VA；status 为 OOM | 后续失败分支仍调用 `pPool->DestroyMemory(virt)` | null pool 分支是否可达，以及空指针 cleanup 风险 |
| CreateMemory 失败 | 输出 virt/VA 在失败路径置空/零 | 仍构造 physical wrapper，但跳过 Init；调用 DestroyMemory(null) | cleanup 返回值被忽略；原始 status 保留 |
| physical Init 失败 | virtual allocation 已进入 tracker/set | DestroyMemory(virt)；local physical shared pointer 离开作用域 | 下层 physical Init 部分初始化后的 destructor 是否完整清理 |
| ModifyAccess 失败 | VA、physical、Bind 已建立 | Unbind，再 DestroyMemory(virt) | Unbind 返回值未处理；peer open/paging 部分副作用是否回滚 |

证据：[src/musa/core/stream.cpp:564-598]、[src/musa/core/memoryPool.cpp:380-427]。

`param.virtAddress` 在 physical 初始化之前已写入，失败 cleanup 后本函数没有再清零。因此这只是内部参数残留观察，不能直接断言 public API 在失败时会向调用方发布失效地址；仍需核对 driver 的输出赋值条件。[src/musa/core/stream.cpp:572-600]

## 3. async free：unmap 与回收是两次提交

```text
DisableAccess
  -> CmdPaging(PROT_NONE)
  -> success
CallbackCommand 创建 / 设置 callback
  -> ResolveDependencyAndQueueCommand
  -> callback 执行
       -> DestroyPhysMemories
       -> 非 graph：DestroyMemory(virt)
```

若 DisableAccess 返回错误，不创建 cleanup callback；若 paging 已成功入队但 callback 提交返回错误，本函数没有显示补偿回收路径。这是两个不同的失败窗口，应分别注入错误，不能笼统声称 async free 失败会恢复原映射或一定释放对象。[src/musa/core/stream.cpp:634-658]

callback 捕获 `[virt, command, pPool]`：`command` 是 shared pointer，但 `virt` 与 `pPool` 是 raw pointer。捕获不延长 MemoryPool 生命周期，也不直接增加 virtual Memory 的 shared ownership；virt 存活依赖 tracker/graph 等外部 owner。pool 提前 destroy 风险不能由 callback 存在来消除。[src/musa/core/stream.cpp:638-655]

callback 还持有自身 command 的 shared pointer；是否形成长期引用环取决于 CallbackCommand 执行后是否清空 callback，当前引用结构不能单独证明泄漏，需要继续核对 callback 执行实现。

## 4. SetAccess 不是事务式权限更新

Core `SetAccess` 的顺序是：

1. 先校验全部 descriptors；
2. 持 access-map mutex 修改 map；
3. 对非 imported pool，持 allocation-set mutex 收集 raw `(virt, physical, size)`；
4. 释放 set mutex 后逐项 `ModifyAccess`；遇错误中止。

因此阶段 4 失败时，前面的 map 更新已经生效；当前函数没有显示恢复旧 map 或撤销前面已提交 paging 的逻辑。`GetAccess` 读取的是 Core map，不是硬件映射完成状态。[src/musa/core/memoryPool.cpp:326-360]

收集 raw pointers 后释放 allocation-set lock，不等于保留 shared ownership；与 async free callback 交错时是否有外层同步保障，需要核对调用上下文，不能直接宣布并发 use-after-free 已复现。

## 5. 锁域与保障范围

| 锁/机制 | 可见保护内容 | 不提供的保证 |
|---|---|---|
| `m_LocationAccessMapMutex` | map 更新、读取、paging 参数生成 | 不等待 GPU mapping 完成 |
| `m_MemoryAllocationsMutex` | allocation set、Create/Destroy 的 logical bytes 更新 | 不能让释放锁后的 raw snapshot 自动存活 |
| HAL pool `m_Lock` | segment/bucket/tracker、chunk 统计组合更新 | 不管理 Core pool handle 生命周期 |
| stream command ordering | paging 与 callback 的队列顺序 | 不自动拥有 raw pool pointer |
| IPC process-local mutex | 当前进程 IPC 字段访问 | 不提供跨进程 owners 原子性 |

Core 字段证据：[src/musa/core/memoryPool.h:105,121-149]；访问流程证据：[src/musa/core/memoryPool.cpp:201-427]；HAL allocator 证据：[src/hal/m3d/memoryPool.cpp:82-95,214-259]。

`SetStream` 是无锁 raw-pointer 赋值。普通 AsyncMemAlloc 调用 ModifyAccess 时显式传入 `this`，并非该调用直接读取 `m_pStream`；而 SetAccess 对已有 allocations 调用 ModifyAccess 时使用 pool 的 `m_pStream`。因此并发分析应聚焦 SetStream 与 SetAccess 的交错，而不是把全部 async paging 都归因于 pool-global stream。[src/musa/core/stream.cpp:569,591]、[src/musa/core/memoryPool.cpp:268-272]、[src/musa/core/memoryPool.h:105]

## 6. 自动 pool registry（SplayTree）对分配/释放回滚的影响

同步普通分配的 `MemMgr::Free` 路径受 `SplayTree` 缺陷影响：`Search` 分支方向反转（[src/util/utilSplayTree.h:216-225]）使 `Find` 对已存在键也 miss（[src/util/utilSplayTree.h:97-100]），`FindPool` 解引用 `Find(...)->m_Value` 即崩（[src/hal/m3d/memMgr.cpp:41-44]）；`Get` 对缺失键返回最近节点、空树返回空值（[src/util/utilSplayTree.h:90-93]），`Free`（[src/hal/m3d/memMgr.cpp:149-161]）在 release 下可能错池或空指针。RCA 见 [`../../99-roadmap/rca-splaytree-memmgr.md`](../../99-roadmap/rca-splaytree-memmgr.md)。

对本节回滚矩阵的影响：

- 异步 alloc/free 与 graph 路径使用 Core user pool（`m_UserPools` 列表），不经过 splay tree；因此第 2、3 节的回滚矩阵（`CreateMemory`→`Init`→`Bind`→`ModifyAccess` 与 `DisableAccess`→callback）不受该缺陷直接威胁。
- 同步普通 `muMemAlloc`/`muMemFree` 经 `MemMgr::Allocate`（[src/hal/m3d/memMgr.cpp:81-147]）与 `MemMgr::Free`（[src/hal/m3d/memMgr.cpp:149-161]）成对经过 splay tree。`Free` 的错池风险与回滚语义叠加：向错误 pool 归还区间不会报错（[src/hal/m3d/memMgr.cpp:158-160]），错误 pool 的 `m_SegmentTracker` 被污染，其后续 `FullAllocate`/`Free` 的精确区间查找（[src/hal/m3d/memoryPool.cpp:225-270]）可能命中重叠或不存在区间。
- 即使 pool 选对，`Free` 的 `m_SegmentTracker` 精确查找（[src/hal/m3d/memoryPool.cpp:225-270]）若因 size 未按 granularity 对齐而 miss，只打印 warning 后返回（[src/hal/m3d/memoryPool.cpp:265-269]），busy segment 不会被回收，形成静默泄漏；这与 SplayTree 缺陷相互独立，需要分别验证。

验证矩阵补充（未执行）：

- 构造 `SplayTree` 定向单测：插入 N 个键后逐个 `Find` 已存在键（当前全部 miss）、`Find` 缺失键（应返回空）、`Get` 缺失键（确认"最近"行为）。
- 在 release 构建下对同步 `muMemAlloc` → `muMemFree` 循环压测，配合分配/释放区间日志确认没有错池归还与区间重叠。
- 对 `Free` 传入错误 size（未对齐）做故障注入，确认 `m_SegmentTracker.find` miss 后 busy segment 的回收缺口。
- 修复优先级按 RCA：`Search` 分支反转 → `MemMgr` 四处查找补判空 → `IMemMgr` 接口语义文档化 → 单测；修复后本节回滚矩阵的同步路径前提才成立。

## 7. 建议验证矩阵（未执行）

- VA 成功后强制 physical Init 失败：检查 tracker/set、logical bytes、virtual segment 回收。
- peer open 或 CmdPaging 失败：检查 Unbind、peer resource、副作用与原始错误码。
- PROT_NONE paging 入队后强制 callback 提交失败：观察 backing、VA、tracker 是否残留，不预设正确补偿行为。
- SetAccess 中第二个 allocation 的 paging 失败：比较 Core map、前一 allocation 和后一 allocation 的状态。
- SetAccess snapshot 与 async free callback 交错：核对外层锁/同步约束，再做 TSAN/定向压力测试。
- callback pending 时 destroy pool：验证 raw pool 生命周期，区分 API contract 与实现防护。
- 核对 CallbackCommand 清空 callback 的时机，确认自引用是否被打破。

本节不修改远端源码，也不声称以上测试通过。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/musa/core/stream.cpp`（5 处，例：[src/musa/core/stream.cpp:555-600]）
- `src/hal/m3d/memMgr.cpp`（5 处，例：[src/hal/m3d/memMgr.cpp:41-44]）
- `src/musa/core/memoryPool.cpp`（4 处，例：[src/musa/core/memoryPool.cpp:380-427]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
