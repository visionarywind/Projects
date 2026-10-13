# HAL/M3D 适配：开发指南

- 文档目的：hal m3d：常见修改场景的落地入口、注意点与验证方式。
- 适用范围：M09 模块的《HAL/M3D 适配：开发指南》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


## 修改前

1. 锁定源码快照再改文档：工程版本由 `version_string`/`version_status` 给出，当前为 `5.3.0 develop` [CMakeLists.txt:33-35]。
2. 判断改动落在哪一层：`src/hal/*.h` 接口头、`src/hal/m3d/*.cpp` 适配实现、`src/hal/m3d/CMakeLists.txt` 构建、或 `src/hal/m3d/m3d` 私有子模块（本知识库不改）[src/hal/m3d/CMakeLists.txt:22-23]。
3. 动 `MemMgr`/`MemoryPool` 之前先读 [`risks-and-debt.md`](risks-and-debt.md) 与 [`../../99-roadmap/rca-splaytree-memmgr.md`](../../99-roadmap/rca-splaytree-memmgr.md)，`m_PoolRefs` 的查找语义已在其中记录，不要在未读结论的情况下"顺手修"。

## 修改中

- **新增 HAL 接口方法**：`Hal::IDevice` 一类纯虚接口新增成员后，`Hal::M3d` 下的实现类必须同步给出 override，否则该实现类无法实例化 [src/hal/halDevice.h:279-310]。
- **新增 queue family**：家族名到引擎类型的映射目前在四处独立比对，改动必须同时覆盖 `Device::ConstructQueueFamilies` [src/hal/m3d/device.cpp:1113-1140]、`CmdBuffer::Init` [src/hal/m3d/cmdBuffer.cpp:85-129]、`CmdPool::Init` [src/hal/m3d/cmdPool.cpp:29-47]、`Queue::Init` [src/hal/m3d/queue.cpp:103-149]。
- **新增/修改编译宏**：`set()` 与 `add_definitions()` 两段必须同时改，漏改会出现 CMake 变量与预处理器宏不一致 [src/hal/m3d/CMakeLists.txt:1-21]、[src/hal/m3d/CMakeLists.txt:41-54]。
- **新增底层结果码**：先补 `ResultCast` 的映射表，再让调用方消费；未映射值会落到表末默认分支 [src/hal/m3d/common.h:44-173]。
- **改内存池登记**：`CreatePoolNoLock` 内部会构造对象并虚调用 `Init`，且整段在 `MemMgr::m_Lock` 下执行，注意不要引入池反向回调 `MemMgr` 的路径 [src/hal/m3d/memMgr.cpp:173-198]。
- **改同步语义**：`Queue::WaitIdle` 是直接转发，没有额外缓冲；在没有硬件证据时不要改写它的语义 [src/hal/m3d/queue.cpp:420-422]。

## 修改后

1. 静态自查：

```bash
rg -n "new (Queue|CmdBuffer|Memory|MemoryPool|Device)" src/hal/m3d
rg -n "delete p" src/hal/m3d
rg -n "ShouldNotReachHere|HAL_M3D_NOT_IMPLEMENTED" src/hal/m3d
```

2. 有授权的构建/运行验证命令与限制见 [`testing.md`](testing.md)；本知识库不声称任何命令已成功执行。
3. 回填本目录文档的行号引用，格式统一为 [src/<相对源码根路径>:起始-结束]，改完跑 `verify_kb.py` 与 `verify_content.py`。
4. 涉及 `src/hal/m3d/m3d` 内部行为的改动只能标注边界，不得凭推测补写。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/hal/m3d/CMakeLists.txt`（3 处，例：[src/hal/m3d/CMakeLists.txt:22-23]）
- `src/hal/m3d/queue.cpp`（2 处，例：[src/hal/m3d/queue.cpp:103-149]）
- `src/hal/halDevice.h`（1 处，例：[src/hal/halDevice.h:279-310]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
