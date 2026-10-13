# Memory 与 MemoryPool：开发指南

- 文档目的：memory mempool：常见修改场景的落地入口、注意点与验证方式。
- 适用范围：M05 模块的《Memory 与 MemoryPool：开发指南》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


- 模块：M05 Memory / MemoryPool
- 本文是面向改动者与审查者的操作约束清单；只引用已读源码事实，不替代硬件验证。

## 1. 修改前的准备

1. 锁定当前源码提交（本地快照无 Git 元数据，先记录文件行号基线），改动后必须用 `rg -n` 更新本模块全部 `[path:start-end]` 引用。
2. 判断改动落在哪一层：
   - Driver 入口（`mu_memory.cpp`）：影响 API 参数校验和错误码映射；
   - Core（`memory.{h,cpp}`/`memoryPool.{h,cpp}`）：影响对象生命周期与统计；
   - HAL（`halMemoryPool.h`/`m3d/memoryPool.*`/`m3d/memory.cpp`）：影响 chunk/segment 算法与 M3D 适配。
3. 先画调用链与资源所有权（谁持有 shared_ptr、谁是 raw pointer、谁负责 HAL `Free`），再动代码。本模块核心所有权边界见 [`data-structures.md`](data-structures.md) 第 5、7 节。

## 2. 必须保持的不变量

| 不变量 | 依据 | 改动注意 |
|---|---|---|
| `FullAllocate` 只接受 `MakeKey(allocInfo) == MakeKey(poolInfo)` | [src/hal/m3d/memoryPool.cpp:85-98] | 别引入"近似匹配 pool" |
| virtual busy range 分配时插入 `m_SegmentTracker`，释放时用相同 `{base,size}` 删除 | [src/hal/m3d/memoryPool.cpp:225-270,380-435] | free size 必须与分配时按 granularity 对齐后的值一致 |
| 完整 chunk 的 free 走 `ResourceRemove` 的 lazyFreeCount/reuse limit 判定 | [src/hal/m3d/memoryPool.cpp:340-354] | `lazyFreeCount` 不是 command/event 计数 |
| Core `Memory` 存活到 async callback 完成才能交还 pool | [src/musa/core/stream.cpp:608-653] | callback 捕获的是 raw `virt`/`pPool` |
| internal memory 用 `InitPrealloc` 创建、`FreeInternalMem` 显式释放 | [src/musa/core/device.cpp:1174-1220] | 不要依赖普通 `Memory` 析构 |

## 3. 常见错误与规避

1. **池 key 字段遗漏**：`MakeKey` 由 property/view/type/heap/NUMA 五字段拼成（[src/hal/m3d/memMgr.h:98-115]），少填一个字段就产生不同 key，"池明明存在却查不到"。
2. **`SplayTree::Get` 不是精确查找**：缺失键返回最近节点，空树返回空值。`MemMgr::Allocate` 靠后续五字段比对兜底；新增 `Get` 调用点必须做精确性校验。`SplayTree::Find` 更危险：`Search` 分支反转（[src/util/utilSplayTree.h:215-226]）导致已存在键也 miss 并返回 `nullptr`，`FindPool` 解引用即崩；详见 [`../../99-roadmap/rca-splaytree-memmgr.md`](../../99-roadmap/rca-splaytree-memmgr.md)。
3. **POSIX `mmap` 返回值**：失败是 `MAP_FAILED`（`(void*)-1`）不是 `nullptr`。`InitFromHandle` 已用 `MAP_FAILED` 判断（[src/musa/core/memoryPool.cpp:586-597]）；新增 mmap 调用处不要写 `== nullptr`。
4. **对齐粒度**：async/pool 分配的 size 进入 HAL 前按 device minimum allocation granularity 向上取整；free 时也要再次对齐，否则 interval lookup 会 miss。[src/musa/core/memory.cpp:556-586,485-505]
5. **统计口径**：`USED_MEM_*`（Core logical bytes）与 `RESERVED_MEM_*`（HAL chunk bytes）不是同一指标，断言/文档中不要混用。
6. **stream/pool 生命周期**：`SetStream` 是 pool 级 raw 赋值；多 stream 共享 pool 时修改 `SetAccess` 的既有 allocation 更新路径要检查 `m_pStream` 的有效性。
7. **IPC 共享状态**：`owners` 是跨进程共享内存整数，当前只由进程内 mutex 保护；新增 IPC 操作不要假设跨进程原子性。

## 4. 修改中

- 保持 v1/v2/`_ptds`/`_ptsz` wrapper 风格（`mu_memory.cpp` 的模式）。
- 新建异步资源时必须明确：完成条件是什么、由谁持有、失败时回滚到哪一步。参考 [`async-rollback-and-locking.md`](async-rollback-and-locking.md) 的回滚矩阵。
- HAL 错误必须通过既有转换边界（`HalToMuResult`/`M3dToHalResult`）返回，不要在 driver 层手写映射。
- 不改变同步语义除非有硬件证据：普通 `muMemAlloc` 返回前 chunk 必须已存在。

## 5. 修改后

1. 静态检查：`git -C <docs> diff --check -- musa` 检查空白；更新所有受影响的 `[path:start-end]`。
2. 运行验证器（只关注本模块文件）：
   ```bash
   cd /var/folders/xy/9w9pq8216b19ttfmlr7md1j00000gn/T/opencode
   python3 verify_kb.py
   python3 verify_content.py
   ```
3. 在授权目标机做最小构建与专项测试（命令模板见 [`testing.md`](testing.md)），未授权前不执行任何安装脚本。
4. 新增风险条目先登记到 [`risks-and-debt.md`](risks-and-debt.md)，未运行验证的项必须标注"未验证"。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/hal/m3d/memoryPool.cpp`（2 处，例：[src/hal/m3d/memoryPool.cpp:85-98]）
- `src/musa/core/stream.cpp`（1 处，例：[src/musa/core/stream.cpp:608-653]）
- `src/musa/core/device.cpp`（1 处，例：[src/musa/core/device.cpp:1174-1220]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
