# Memory 与 MemoryPool：风险与技术债

- 对应源码版本：`b8dce2b2f23849e8e99350c20d7eaac3c129caba`
- 规则：下表是静态审计项，不把未运行验证的风险写成已确认 bug。

| 风险/技术债 | 影响 | 证据与状态 | 建议验证/缓解 |
|---|---|---|---|
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
| `Util::SplayTree` 的删除和 value ownership 未确认 | 自动 pool 注册表可能存在删除、析构和并发理解偏差 | [`src/hal/m3d/memMgr.h:37-39`]、`memMgr.cpp` 调用点；未知 | 读取实现并补 registry 生命周期图 |
| Core access map、allocation set 与 HAL pool lock 分属不同锁域 | 可能存在跨层锁序、访问更新与销毁竞态 | [`src/musa/core/memoryPool.h:85-152`]、HAL pool `m_Lock`；风险为推断 | 画锁序图，使用 TSAN/故障注入验证 |
| M3D 子模块和内核驱动不可见 | 无法从当前仓库证明 page allocation、fence、真实错误码和硬件副作用 | `src/hal/m3d` 适配边界；明确未验证 | 在对应版本 M3D 源码和目标机上补证，不把适配调用当硬件结果 |

## 当前不应声称的结论

- 不能把 `2 MiB` 或 `32 MiB` chunk quantum 当作硬件最小页大小。
- 不能把 `lazyFreeCount` 当作 event/semaphore/command completion count。
- 不能把 `UpdateUserPools` 描述为扫描全部 deferred free 的后台线程。
- 不能把 Core reuse attribute 描述为已经改变 HAL allocator policy。
- 不能把 `CreateGpuMemory` 返回成功等同于 GPU 物理页已经在硬件侧可见。
