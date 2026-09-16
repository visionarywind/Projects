# Memory 与 MemoryPool：测试与验证策略

- 对应源码版本：`b8dce2b2f23849e8e99350c20d7eaac3c129caba`
- 当前状态：以下运行测试均未在本环境执行；策略用于下一轮目标机/构建环境验证。

## 1. 静态检查

### 1.1 调用链和对象登记

- 核对 `muapiMemAlloc_v2`、`muMemAllocAsync`、`muMemFreeAsync`、pool API 是否都经过导出/accessor 表。
- 核对 `Context::CreateMemory` 只有在 `Memory::Init` 成功后才把对象加入 Context 和 Platform tracker。[`src/musa/core/context.cpp:1037-1086`]
- 核对 async alloc 的每个失败分支是否解除 `Bind`、销毁 virtual memory、释放 physical shared pointer。[`src/musa/core/stream.cpp:554-600`]
- 核对 async free 的 callback 是否保留 virt/pool 生命周期直到 paging command 完成。[`src/musa/core/stream.cpp:634-671`]

### 1.2 HAL allocator 不变量

- `MakeKey(allocInfo) == MakeKey(poolInfo)` 才允许 `FullAllocate`。[`src/hal/m3d/memoryPool.cpp:82-95`]
- `ResourceSplit` 后 busy range 必须插入 virtual `m_SegmentTracker`，free 时用相同 `{base,size}` 删除。[`src/hal/m3d/memoryPool.cpp:214-259,358-413`]
- 左右 merge 后，只有 `isLeftMost && isRightMost` 才允许进入完整 chunk reuse/destroy。[`src/hal/m3d/memoryPool.cpp:318-331`]
- `TrimPool` 不应释放含 busy segment 的 chunk。[`src/hal/m3d/memoryPool.cpp:480-510`]

### 1.3 统计和属性

- `USED_MEM_*` 对应 Core requested bytes，`RESERVED_MEM_*` 对应 HAL total chunk bytes，不要在断言中混用。[`src/musa/core/memoryPool.cpp:380-427`]
- `RELEASE_THRESHOLD` 影响 trim；reuse count/size limit 是另一套条件。[`src/musa/core/memoryPool.cpp:118-123,430-437`、`src/hal/m3d/memoryPool.cpp:318-331`]
- 继续追踪三个 reuse attribute 的所有读取者，若没有读取者，应将其行为列为兼容性缺口而不是已实现特性。

## 2. 建议的 allocator 单元测试（需要目标构建环境）

| 用例 | 关注点 | 预期观察 |
|---|---|---|
| 小于 chunk quantum 的多次分配 | 2 MiB/32 MiB 量化、split | reserved chunk 次数和 free size 可解释 |
| 请求大于 quantum | `AlignUp` 和单次 chunk 扩容 | chunk size 为对齐后的 request，不无限循环扩容 |
| 大 alignment | prefix segment、精确 free range | prefix 可复用，free 不因 size 不匹配而丢失 |
| 左邻居/右邻居/两侧均空闲 | merge 逻辑 | segment 数量减少，完整 chunk 可触发 reuse/destroy |
| busy 子段存在时 trim | chunk-level trim 边界 | 大部分空闲但含 busy 的 chunk 不释放 |
| reuse count/size limit | `ResourceRemove` | 完整 chunk 在不同阈值下保留或 destroy |
| 自动 pool 属性隔离 | key type/heap/property/view/NUMA | 不兼容 allocation 不共享 pool |
| pool handle 属性不匹配 | 显式 pool validation | 返回 invalid value，不污染 pool |

## 3. stream-ordered async 测试

1. 在同一 stream 上 alloc → kernel/memcpy → free，确认 free 不早于前序 command。
2. 在不同 stream 上共享 current pool，确认 `SetStream`、access mapping 和 dependency 语义。
3. alloc 后立即查询/使用地址，区分“VA 已保留”和“mapping 已完成”。
4. DisableAccess/paging 失败时检查 callback 是否仍安全清理 physical 和 virtual 对象。
5. stream destroy、context destroy、设备错误发生在 callback pending 时，检查是否等待和是否泄漏。
6. 显式 `muMemPoolTrimTo` 与 `Wait/WaitFinish` 的组合，确认 trim 只回收完整空闲 chunk。

## 4. graph memory 测试

1. capture alloc → use → free → graph instantiate/launch，确认 physical 创建发生在 graph execution 而非 capture。
2. 重复 launch 同一 graph，确认 virtual address、physical binding 和 resource ownership 符合设计。
3. graph free node invalid pointer、重复 free、跨 graph pointer。
4. graph execution 中 alloc/paging/peer mapping 任一步失败，检查 rollback 和 signal。
5. graph resource destroy 前后查询 graph pool `USED/RESERVED`，确认 deferred physical 与 virtual segment 的统计差异。

## 5. 运行命令（未执行）

以下命令只是远端项目 README/CMake 记录的参考，不能作为本知识库已通过的测试：

```bash
cmake -S . -B build -DMUSA_BUILD_UT=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

目标环境还需要私有子模块、M3D/驱动依赖、工具链和兼容 GPU。当前知识库未执行上述命令，也未运行 `muInfo`、gdb、性能测试或硬件验证。

## 6. 最终文档仓库检查

在文档写回完成后运行：

```bash
git -C /home/mtuser/workspace/Projects diff --check -- musa
```

该命令只检查本文档仓库的 whitespace，不证明 MUSA 远端源码可构建或硬件行为正确。
