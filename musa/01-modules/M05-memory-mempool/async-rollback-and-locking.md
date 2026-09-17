# M05 专题：异步分配回滚、访问更新与锁域

- 对应源码版本：`b8dce2b2f23849e8e99350c20d7eaac3c129caba`
- 最后更新：2026-09-17
- 证据状态：本节重新只读核对远端 HEAD、`Stream::AsyncMemAlloc/AsyncMemFree` 和 Core pool 字段；访问更新依据此前读取的同版本实现。运行、故障注入及硬件结果未验证。
- 前置阅读：[实现分析](implementation.md)、[执行流](execution-flows.md)

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

因此阶段 4 失败时，前面的 map 更新已经生效；当前函数没有显示恢复旧 map 或撤销前面已提交 paging 的逻辑。`GetAccess` 读取的是 Core map，不是硬件映射完成状态。[src/musa/core/memoryPool.cpp:201-299]

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

## 6. 建议验证矩阵（未执行）

- VA 成功后强制 physical Init 失败：检查 tracker/set、logical bytes、virtual segment 回收。
- peer open 或 CmdPaging 失败：检查 Unbind、peer resource、副作用与原始错误码。
- PROT_NONE paging 入队后强制 callback 提交失败：观察 backing、VA、tracker 是否残留，不预设正确补偿行为。
- SetAccess 中第二个 allocation 的 paging 失败：比较 Core map、前一 allocation 和后一 allocation 的状态。
- SetAccess snapshot 与 async free callback 交错：核对外层锁/同步约束，再做 TSAN/定向压力测试。
- callback pending 时 destroy pool：验证 raw pool 生命周期，区分 API contract 与实现防护。
- 核对 CallbackCommand 清空 callback 的时机，确认自引用是否被打破。

本节不修改远端源码，也不声称以上测试通过。
