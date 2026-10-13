# HAL/M3D 适配：测试策略

- 文档目的：hal m3d：测试入口、缺口与建议用例（含未验证标注）。
- 适用范围：M09 模块的《HAL/M3D 适配：测试策略》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


- 对应版本：`5.3.0 develop`；以下检查全部基于静态源码，**未在本环境执行任何构建或运行**。

## 静态检查

### 内存分配与释放

- 分配入口的三条规模校验（`size==0`、`size+alignment-1` 回绕、deviceLocal 超 `totalGlobalMem`）必须发生在选池与落池之前 [src/hal/m3d/memMgr.cpp:85-96]。
- 调用方指定池时必须做 `MakeKey` 比对，不等即拒绝，避免把请求落到属性不符的池 [src/hal/m3d/memMgr.cpp:103-105]。
- 池对象 `Init` 失败要 `delete`，`new` 失败要返回 `errorOutOfMemory`，两条失败分支都不得登记进注册表 [src/hal/m3d/memMgr.cpp:191-196]。
- `MemMgr::Free` 没有错误码出口，路径上的 `MUSA_ASSERT` 是唯一防护 [src/hal/m3d/memMgr.cpp:157-160]；而 `NDEBUG` 下该宏退化为空操作 [src/util/utils.h:25-28]，所以 release 构建的防护强度必须单独验证。
- 释放时在 `m_SegmentTracker` 查不到区间只会打 `LOG_WARN` 并返回，测试要能观察到这类静默失败 [src/hal/m3d/memoryPool.cpp:265-269]。
- `TrimPool` 只回收 `isLeftMost && isRightMost` 的整块空闲 chunk，含 busy 子段的 chunk 不应被释放 [src/hal/m3d/memoryPool.cpp:502-532]。

### 对象生命周期

- 所有 `Create*` 遵循 `new` → `Init` → 失败 `delete`，可用 `CreateQueue` 作为模板核对 [src/hal/m3d/device.cpp:77-92]。
- HAL 对象销毁必须走 `IBase::Destroy()` 而不是 `delete`，Core 侧由 `HalPtr` 保证 [src/musa/core/halPtr.h:26]。
- 内部池只归还不销毁，销毁点仅在 `MemMgr` 析构；用例应覆盖"归还两次""归还后仍使用" [src/hal/m3d/memMgr.cpp:71-79]。

### 状态机与提交

- `Begin`/`End`/`Reset` 的非法状态必须返回 `errorIllegalState`，且 `m_State` 不得被改成可用状态 [src/hal/m3d/cmdBuffer.cpp:146-148]。
- user queue 带 fence 的提交必须整体拒绝，不得部分执行 [src/hal/m3d/queue.cpp:185-188]。
- 超出固件上限的 wait/signal 信号量被拆成单独调用，提交失败时要检查已单独 wait 的信号量是否补偿（源码未见补偿逻辑）[src/hal/m3d/queue.cpp:229-237]。

## 运行验证（未执行）

工程提供 `MUSA_BUILD_UT` 开关 [CMakeLists.txt:29]，打开后会 `enable_testing()` 并 FetchContent 拉取 googletest [CMakeLists.txt:146-154]，随后 `add_subdirectory(unittest)` [CMakeLists.txt:166-168]。

**未验证点**：本快照中 `unittest/` 目录不存在（`ls unittest` 无此目录），因此 `MUSA_BUILD_UT=ON` 是否能在本快照配置成功未知；需要在完整检出的仓库里确认。

参考命令（本知识库未执行）：

```bash
cmake -S . -B build -DMUSA_BUILD_UT=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

目标环境还需要私有子模块 `src/hal/m3d/m3d`、MUSA 工具链、驱动与 GPU。查询类路径可用 [`../../80-demos/D01-muinfo`](../../80-demos/D01-muinfo) 观察，拷贝/异步路径见 [`../../80-demos/D02-memory-copy`](../../80-demos/D02-memory-copy)；上述 demo 是否在本环境跑过，以 demo 自身文档为准。

## 文档仓库检查

```bash
git -C .. diff --check -- musa
```

该命令只检查本文档仓库的空白与行尾，不证明 MUSA 源码可构建、更不证明硬件行为正确。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/hal/m3d/memMgr.cpp`（5 处，例：[src/hal/m3d/memMgr.cpp:85-96]）
- `src/hal/m3d/memoryPool.cpp`（2 处，例：[src/hal/m3d/memoryPool.cpp:265-269]）
- `src/hal/m3d/queue.cpp`（2 处，例：[src/hal/m3d/queue.cpp:185-188]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
