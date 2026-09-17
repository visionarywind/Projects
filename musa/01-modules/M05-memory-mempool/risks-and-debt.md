# Memory 与 MemoryPool：风险与技术债

- 对应源码版本：`b8dce2b2f23849e8e99350c20d7eaac3c129caba`
- 规则：下表是静态审计项，不把未运行验证的风险写成已确认 bug。

| 风险/技术债 | 影响 | 证据与状态 | 建议验证/缓解 |
|---|---|---|---|
| POSIX IPC pool `mmap` 失败判断使用 `nullptr` 而不是 `MAP_FAILED` | `mmap` 失败时返回 `(void*)-1`，当前分支可能把无效地址当作成功，随后访问 `owners` 触发崩溃或破坏状态 | [`src/musa/core/memoryPool.cpp:491-503,551-562`]；静态代码风险，未运行验证 | 使用 `MAP_FAILED` 判断，并在失败路径关闭 fd、清理共享对象；增加 syscall 故障注入 |
| IPC shared-memory 初始化的中途失败缺少统一回滚 | `shm_open`/`ftruncate`/`mmap` 任一步失败后可能遗留 fd、名称或对象；`std::call_once` 还会固化失败状态，后续调用不会重试 | [`src/musa/core/memoryPool.cpp:516-569`]；静态代码风险，未运行验证 | 使用 RAII/统一 cleanup；明确 `call_once` 失败后的重试策略 |
| IPC `owners` 计数使用跨进程共享整数但只由进程内 mutex 保护 | 不同进程同时 import/destroy 时，`owners += 1`/`--owners` 可能丢失更新，导致共享对象过早或过晚 unlink | [`src/musa/core/memoryPool.cpp:486-503,575-603`]；跨进程竞态为推断 | 使用进程共享同步原语或设计幂等生命周期；跨进程压力测试 |
| imported pool 只映射 IPC metadata，没有建立 HAL pool handle | imported pool 可以作为句柄被创建/销毁和部分查询，但不能直接承载普通 pool allocation；API 能力边界容易被误读 | [`src/musa/core/memoryPool.cpp:38-42,473-510`]、[`src/driver/mu_mempool.cpp:153-169,288-319`]；静态已确认 | 明确文档/API contract；验证所有 imported-handle 操作的错误码 |
| `muMemPoolDestroy` 删除 pool 前未显式检查 live allocations | 若调用方销毁仍有 allocation 的 pool，可能让 allocation 的 `m_pPool` 指针悬空，后续 Memory destructor 再访问已释放 pool | [`src/driver/mu_mempool.cpp:118-147`]；需结合 pool ownership 进一步验证 | destroy 前检查 `m_MemoryAllocations` 非空并返回错误，或定义强制清理语义；补 live-allocation 测试 |
| Core pool 的三个 reuse attribute 当前只看到保存/返回，尚未看到参与 HAL 选择或 dependency 检查 | 用户设置属性可能与实际复用行为不一致；语义边界不清 | [`src/musa/core/memoryPool.cpp:101-198`]；属性存储已确认，实际调度效果未知 | 全局查找字段读取者；补充 async event/false-dependency 用例 |
| `m_SegmentTracker` 以 `{base,size}` 精确查找 | free size 若未按 allocation granularity 对齐，可能找不到 busy range，造成回收失败或残留 | [`src/hal/m3d/memoryPool.cpp:214-259`]、[`src/musa/core/memory.cpp:360-379,431-460`]；契约已确认 | 对大 alignment、suballocation offset、错误 size 做单测和硬件测试 |
| `FullAllocate` 只在 `errorNotFound` 后扩一个 chunk并重试一次 | 底层 chunk 创建成功但第二次 split 失败时，错误传播和已增加的 pool 状态需审计 | [`src/hal/m3d/memoryPool.cpp:82-95`]；静态路径已确认，异常行为未运行 | 加入故障注入，检查新 chunk 是否完整回收 |
| assured-fit 默认只有限尝试 bucket 首个 segment | 第一个候选因地址 alignment 不满足时可能产生额外 chunk 或碎片 | [`src/hal/m3d/memoryPool.cpp:97-151`]；算法已确认，性能影响未测 | 比较 assured-fit/best-fit，采集 split、reserved、延迟指标 |
| 完整 chunk 的 lazy reuse 可长期保留 reserved memory | burst 后进程可能保留显存，影响其他 allocator/进程 | [`src/hal/m3d/memoryPool.cpp:318-331`]；逻辑已确认，实际曲线未测 | 结合 release threshold 和显式 trim 做压力测试 |
| `TrimPool` 只能释放完整空闲 chunk | 一个 chunk 只剩小段 busy allocation 时，即使大部分空闲也无法释放 | [`src/hal/m3d/memoryPool.cpp:480-510`]；实现已确认 | 暴露 chunk-level fragmentation 指标；设计搬迁/compaction 前先确认 API 语义 |
| `UpdateUserPools` 只遍历 `m_UserPools` | 自动 pool/internal pool 不一定由同一 trim 触发点回收，容易误读全局 trim 语义 | [`src/hal/m3d/memMgr.cpp:229-235`]；遍历范围已确认 | 分别追踪自动、user、internal pool 的 trim 入口和生命周期 |
| `pPool->SetStream(this)` 是 pool 级字段 | 多 stream 共享 current pool 时，mapping/paging 使用 stream 可能发生时序或并发问题 | [`src/musa/core/stream.cpp:561-570`]；风险为推断 | 多 stream async alloc/free + pool access map 压测，检查锁序和 command ordering |
| async free 依赖 DisableAccess 后的 callback | callback 未完成前不能销毁 virtual/physical object；异常退出和 stream destroy 需保证 callback 处理 | [`src/musa/core/stream.cpp:634-671`]；生命周期链已确认，异常路径未验证 | 注入 stream error、destroy-before-wait、callback failure 场景 |
| graph free node Init 当前为 no-op | 非法 pointer 或错误 graph resource 可能延迟到 graph exec 构建/执行阶段才报告 | [`src/musa/core/node/graphMemoryFreeNode.cpp:6-13`]；行为已确认，影响为推断 | 补 graph free invalid pointer、重复 free、跨 graph pointer 测试 |
| 普通 graph command switch 中 mem alloc/free 调用被注释 | 维护者可能误把 graph memory 当普通 stream command，导致错误文档或修复方向 | graph exec/UniversalManager 路径已确认；普通 switch 代码需保留上下文 | 统一 graph memory 文档和回归测试入口 |
| internal memory 使用 `InitPrealloc`，释放由 `FreeInternalMem` 显式完成 | 若调用方绕过专用释放入口，Core destructor 不会自动完成 pool free | [`src/musa/core/device.cpp:1110-1152`]、[`src/musa/core/memory.cpp:360-379`]；实现已确认 | 封装 ownership，增加 debug assertion 和 leak test |
| HAL pool 析构遇到 busy segment 只记录告警后清理 | 进程退出/异常销毁时可能掩盖 live allocation、未完成 command 或 use-after-free | [`src/hal/m3d/memoryPool.cpp:63-80`]；静态行为已确认 | 在 debug build 让 busy cleanup 可失败或输出 allocation 详情 |
| 自动 pool registry 的 `SplayTree::Find` 比较方向与通常 BST 方向相反 | 精确查找可能在某些树形下走错子树，导致已有 pool 未命中或错误返回；需结合实际插入/树形验证 | [`src/util/utilSplayTree.h:95-100,215-226`]；静态实现风险，未运行复现 | 为 SplayTree 增加定向单测；核对比较方向和 sentinel 语义 |
| `SplayTree::Get` 对不存在 key 返回 splay 后的根节点，而不是空指针 | 调用方若未再次比较 key，可能把相邻 key 的 pool 当成候选；当前 `MemMgr::Allocate` 有属性比较，但其他调用点需审计 | [`src/util/utilSplayTree.h:87-93`]、[`src/hal/m3d/memMgr.cpp:36-43,116-133`]；行为已确认，影响需验证 | 所有 `Get` 调用统一封装精确 lookup；补不存在 key 和相邻 key 测试 |
| 自动 pool registry 的 `Delete(..., true)` 会 delete value | 误用 `deleteValue` 或重复从其他容器释放可能造成 double free；所有权必须与 registry 分类保持一致 | [`src/util/utilSplayTree.h:182-212`]、[`src/hal/m3d/memMgr.cpp:20-34,200-209`]；所有权路径已确认 | 为 user/automatic/internal pool 分别画 ownership 图并加 teardown 测试 |
| Core access map、allocation set 与 HAL pool lock 分属不同锁域 | 可能存在跨层锁序、访问更新与销毁竞态 | [`src/musa/core/memoryPool.h:85-152`]、HAL pool `m_Lock`；风险为推断 | 画锁序图，使用 TSAN/故障注入验证 |
| `muMemPoolDestroy` 可删除当前 device pool 而未回退 `m_CurrentMemoryPool` | `Device::GetMemoryPool()` 后续优先返回已释放的 raw pointer，可能触发 use-after-free；需要验证 setter/destroy API contract | [`src/musa/core/device.h:87-95,224-228`]、[`src/driver/mu_device.cpp:328-348`]、[`src/driver/mu_mempool.cpp:118-150`]；静态风险，未运行 | destroy 前禁止 current pool，或将 current pointer 原子回退 default；补 set-current→destroy→async alloc 测试 |
| imported pool 通过 `ValidatePool`，但 `muMemSetMemPool` 无条件访问 `pMemPool->Hal()->GetInfo()` | imported wrapper 没有 `m_pHalPool`，将其设为 current 时可能空指针解引用；静态风险，未运行 | [`src/musa/core/device.h:93-95`]、[`src/driver/mu_mempool.cpp:504-553`] | 在 set-current 入口显式拒绝 imported/无 HAL pool；补 imported pool 各 location/type 用例 |
| host/NUMA current pool 是 Platform raw pointer，setter 不检查 pool 所属/生命周期 | 删除或替换 user pool 后 current slot 可能残留悬空指针，后续 host allocation 读取失效对象 | [`src/musa/core/platform.cpp:575-645`]、[`src/driver/mu_mempool.cpp:504-553`]；静态风险，未运行 | setter 做 ValidatePool/归属检查，destroy 时清理 current slots；补 host/NUMA 生命周期测试 |
| imported pointer 的 `pool` 参数未见 ValidatePool、类型或 fromMempool 校验 | 任意可解析 device 的 pool handle 可能被接受，API contract 与实际 ownership 边界不清 | [`src/driver/mu_mempool.cpp:358-390`]；源码观察，影响未验证 | 明确 contract；验证错误 pool、跨设备 pool、普通 pool 和 imported pool 的错误码 |
| imported pool 析构路径可能不关闭 dup fd，且 exporter 先销毁时没有明显 unlink 后继路径 | fd 泄漏或 POSIX shm 名称残留的可能性增加；需结合进程顺序和资源观察验证 | [`src/musa/core/memoryPool.cpp:473-510,575-609`]；静态风险，未运行 | imported cleanup 无条件 close；设计明确 unlink owner；做 exporter/importer 交错退出测试 |


- 不能把 `2 MiB` 或 `32 MiB` chunk quantum 当作硬件最小页大小。
- 不能把 `lazyFreeCount` 当作 event/semaphore/command completion count。
- 不能把 `UpdateUserPools` 描述为扫描全部 deferred free 的后台线程。
- 不能把 Core reuse attribute 描述为已经改变 HAL allocator policy。
- 不能把 `CreateGpuMemory` 返回成功等同于 GPU 物理页已经在硬件侧可见。
