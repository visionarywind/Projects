# M04 实现：memdesc、memobj、memblock 和 VA

## Device allocation

`cuapiMemAlloc_common` 将输出指针置零，获取 TLS/current context，拒绝未初始化、capture 冲突、NULL 输出和零大小，然后构造通用 device `CUmemdesc`。它在 context lock 下调用 `memobjAlloc`；成功后调用 `memglobalsRegisterMemobj` 建立全局/P2P 关系，再通知工具并从 memobj 读取设备指针（静态确认：[src/api/apimem.c:52-118]）。全局登记失败会在重新获取 context lock 后释放 memobj，避免半登记对象泄漏（静态确认：[src/api/apimem.c:100-111]）。

## Backing and suballocation

`memobjAllocMemblockBacking` 先调用 `isValidAllocation`，再针对 CUDA memobj sharing、IPC 和 external host address 计算 block offset；可能复用共享 block 或 IPC hash-table block（静态确认：[src/cui/memobj.c:210-263]）。随后由 device HAL 计算 generic block size，并根据 `canSuballocate` 与 requested size 决定 `noSuballoc`（静态确认：[src/cui/memobj.c:265-280]）。

`memdescIsCompatible` 对 location、cache、type、layout、depth、owner、映射、API source、页大小、固定地址、压缩和 handle types 等字段逐项比较；这使子分配复用必须满足属性完全兼容，而不是只比较大小（静态确认：[src/cui/memobj.c:113-163]）。

## Free

`cuapiMemFree_common` 允许没有 current context 的场景，但要求全局 UVA manager 存在；先在 unified VA 查找，再在当前 memmgr 按 device pointer 查找。它检查 API source、sharing source memobj、base pointer，注销 P2P 映射，锁住对象 context 并同步后通知 tools、释放 memobj，最后倾倒 profiler 日志（静态确认：[src/api/apimem.c:231-339]）。

## Host/pinned memory

Host allocation 使用单独 common wrapper，把 location 设为 host、mapHost 设为 host VA，再进入 memobj 体系；因此 host pointer 也不是简单的 libc malloc 结果，而是由 memobj 记录、查找和释放（静态确认：[src/cui/cuimem.c:189-230]；API 入口：[src/api/apimem.c:352-414]）。

## UVM manager、managed allocation 与异步 stream

`uvmManagerCreate` 为全局 UVM manager 建立 mutex、逐设备调用 DMAL UVM 初始化，并根据设备能力决定 kernel-mode UVM、zero-copy fallback、P2P 过滤和 primary memmgr 需求；随后创建 streams/namespaces hash、managed/non-managed heaps，初始化 abstraction layer，再注册全局 stream。初始化中任何 UVM device、hash 或 heap 失败都会进入统一清理路径（静态确认：[src/cui/cuiuvm.c:619-839]）。

managed 属性不是单个 flag：`CUuvmManagedAttr` 保存 read-mostly、实际/用户可见 preferred location、last prefetch、accessed-by bitvector 和 parent memblock；managed variables namespace 则用 mutex 和字符串 hash 复用跨 module 的变量对象（静态确认：[src/cui/cuiuvm.c:54-90]）。这意味着 managed allocation 的属性更新、拆分/合并和 backing 生命周期必须与 parent memblock 对齐。

UVM 记录 memobj 最近/首个附着 stream；切换 stopped 状态时在 UVM manager lock 下调用 abstraction layer，维护 running stream 计数，失败则重新把已停止 stream 恢复为 running，并在成功后处理 host access changes（静态确认：[src/cui/cuiuvm.c:2550-2669]）。具体 page fault/migration 的最终处理落在 `uvmGetAL()` 选择的 kernel-driver backend，当前快照不能把 Lite 与 UVM 8 的运行时选择写成已验证事实。

## UVM DAG

UVM DAG 是进程级全局同步图，节点 ID 在进程生命周期内不复用；节点记录 owner stream/context、GPU/CPU work 类型、stream 相邻关系、event wait/record 和 null-stream barrier。DAG 明确不处理 IPC event（静态确认：[src/cui/cuiuvmdag.c:1-25,71-153]）。因此 stream/event/context synchronization 不只是 channel flush，还要沿 DAG 依赖传播。

## P2P bookkeeping

`CUpeerMap` 为每个 peer `CUmemmgr` 保存两类访问权限的独立引用计数；首次 retain 和最后 release 分别向上层报告边沿，计数归零才移除 entry，destroy access 则一次清除该 peer 的全部权限（静态确认：[src/cui/peermap.c:12-134]）。这是 P2P enable/disable 的软件 bookkeeping；真正 peer VA map/unmap 仍由 memblock/DMAL backend 完成。

## UVM abstraction layer 与映射边界

`uvmInitAL` 根据 managed-memory zero-copy 强制选项、Mac 轻量模式以及普通平台路径填充函数指针表；普通平台还根据 `globals.useUvm8Api` 在 UVM8 与 KdLite channel 接口之间选择。故 `uvmMapSubmemblock`/`uvmUnmapSubmemblock` 的公共入口只固定 manager lock 和 AL 调用契约，不能据此断言所有机器使用同一 fault/migration 实现（静态确认：[src/cui/cuiuvm.c:2313-2428,1252-1285]）。

`uvmRegisterMemobj` 对非 CUDA-memobj sharing 对象先在 manager lock 下附着首个 stream，并记录 first/last attached stream，再调用 AL 的 `memobjRegister`；backend 失败时撤销当前 stream 附着。该顺序说明 stream tracking 是 UVM 注册的一部分，而不是注册成功后的附加统计；`uvmRegisterMemobjFinalize`/`uvmUnregisterMemobj` 负责后续完成/撤销状态（静态确认：[src/cui/cuiuvm.c:1311-1390]）。

Kd8 映射路径还处理 host page split、non-managed heap range 的重叠/重复映射检测，以及 SKED/reflected memory 的 dynamic-parallelism 区域映射；这些条件失败会在进入更低层映射前返回错误。它说明 submemblock mapping 同时承担页粒度和设备执行特性的适配，但实际 fault、迁移和 RM 操作仍位于外部/后端边界，当前树未闭合（静态确认：[src/cui/cuiuvm.c:3950-4235]；后端结论为推断/未知）。

