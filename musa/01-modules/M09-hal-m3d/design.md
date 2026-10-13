# HAL/M3D 适配：设计分析

- 文档目的：hal m3d：设计意图、职责边界与关键取舍。
- 适用范围：M09 模块的《HAL/M3D 适配：设计分析》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


## 设计问题

这一层要解决的问题很具体：上层 `src/musa/core` 只认识 `Hal::I*` 纯虚接口，下层是一套不随 MUSA 公共 ABI 变动的 `IM3d::*` 私有接口；两者的数据布局、错误码、对象生命周期都不同。HAL/M3D 目录用一组 `Hal::M3d::*` 实现类做中间层，把接口族、结果码和句柄销毁方式统一起来 [src/hal/m3d/common.h:44-173]、[src/hal/m3d/base.cpp:6-8]。

由此产生的代价是每个对象都要维护"HAL 视角的成员 + M3D 视角的成员"两套状态：`Memory` 用成员保存 `Device&`、尺寸/属性，同时持有顶层 `m_M3dGpuMemory` 与 prealloc 时的 `m_M3dSourceGpuMemory` [src/hal/m3d/memory.h:58-73]。

## 设计取舍

| 取舍 | 好处 | 代价 | 证据状态 |
|---|---|---|---|
| 接口与实现分离：`IPlatform` 纯虚，`Hal::M3d::Platform` 实现并额外持有 `m_Devices` 等私有成员 [src/hal/m3d/platform.h:16-47] | 换底层实现不改 Core | 调用层级多一层跳转 | 已确认 |
| 池用 `MakeKey` 把五个属性字段打包成一个键做全局索引 [src/hal/m3d/memMgr.h:98-104] | 相同属性的分配共享 chunk | 任一字段不一致就退化成新池 | 已确认 |
| `ChunkAllocate` 先向设备要整块内存，再由 `SubAllocate` 切分 [src/hal/m3d/memoryPool.cpp:156-176] | 减少底层创建次数 | 保留空闲 chunk 会占用显存 | 已确认 |
| 空闲整块用 `m_ReuseCountLimit`/`m_ReuseSizeLimit` 决定保留还是销毁 [src/hal/m3d/memoryPool.h:141-142] | 突发负载下复用 chunk | 长期进程可能持有保留内存 | 已确认 |
| 池内用 `std::recursive_mutex` 成员 `m_Lock`，池注册表另有 `MemMgr::m_Lock` [src/hal/m3d/memoryPool.h:144] | 池内操作可重入 | 两把锁的顺序未在代码中显式说明 | 已确认（锁序未验证） |
| 引擎类型靠家族名字符串匹配，未知名字直接 `ShouldNotReachHere()` [src/hal/m3d/queue.cpp:103-149] | 映射表直观 | 同一套比对在 queue/cmdBuffer/cmdPool 重复三份 | 已确认 |
| 所有向下结果经 `ResultCast` 全表转换为 `Hal::Result` [src/hal/m3d/common.h:44-173] | 错误码出口单一 | 新增底层结果码必须手工补表 | 已确认 |

## 不变量

1. 只有 `MakeKey(allocInfo) == MakeKey(m_Info)` 时才允许在该池上分配，违反即 `MUSA_ASSERT` [src/hal/m3d/memoryPool.cpp:86-87]。
2. 调用方显式指定池时必须通过键比对，否则返回 `errorInvalidValue` [src/hal/m3d/memMgr.cpp:103-105]。
3. `CmdBuffer` 状态机违规（非 `Initial` 调 `Begin`、非 `Recording` 调 `End`、`Pending` 调 `Reset`）一律返回 `errorIllegalState` [src/hal/m3d/cmdBuffer.cpp:146-148]。
4. 内部池只归还不销毁，真正释放发生在 `MemMgr` 析构 [src/hal/m3d/memMgr.cpp:71-79]。
5. `MemMgr` 析构按 user pools → splay tree → internal pools 三段清理 [src/hal/m3d/memMgr.cpp:20-34]。
6. `Platform` 拥有全部 `Device`，析构时逐个 `delete` [src/hal/m3d/platform.cpp:95-100]。

第 1、2、3 条由断言与显式返回值支持；第 4、5、6 条只在正常析构路径上验证过读法，并发销毁与异常退出下的行为未验证。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/hal/m3d/memMgr.cpp`（3 处，例：[src/hal/m3d/memMgr.cpp:103-105]）
- `src/hal/m3d/common.h`（2 处，例：[src/hal/m3d/common.h:44-173]）
- `src/hal/m3d/memoryPool.cpp`（2 处，例：[src/hal/m3d/memoryPool.cpp:156-176]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
