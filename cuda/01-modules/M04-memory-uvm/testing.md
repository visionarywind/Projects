# M04 测试、开发与风险

- 文档目的：解释 01-modules/M04-memory-uvm/testing.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M04-memory-uvm/testing.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## 测试证据

- `basic_sanity:init` 直接创建 v3020 context，使用 driver 内部 `memobjAlloc` 分配 2 MiB device memobj，随后逐个 `memobjFree` 并销毁 context（静态确认：[tests/cuda_test/basic_sanity.cu:49-100]）。
- `basic_sanity:kernel` 使用 portable + mapped host allocation，取得 device pointer，查找 memobj 并验证 host/device/UVA片段关系（静态确认：[tests/cuda_test/basic_sanity.cu:169-209]）。
- `unittest_memcpy.cpp` 覆盖 pageable host、device、同步 memcpy、跨 context 和三维 descriptor；P2P 测试在 GPU 少于 2 个时 waive（静态确认：[tests/cuda_test/memcpy/unittest_memcpy.cpp:43-85,87-170]）。测试均未执行。

## 池化测试映射

- `tests/cuda_test/memmgr/tests.nvmk:1-4` 只注册 `memmgrCreate`、UVA disable 和 range-overlap；它们不能证明 suballocator 的复用或碎片行为。
- `tests/cuda_test/memobj/unittest_memobj_alloc.hpp:170-255` 记录了已有 memblock 复用、`noSuballoc`、大于可用区和全局禁用开关等分区，但仍使用 `membins/membin` 术语。当前源码实际索引是 `CUsuballocatorRadixTree`，相关计划应更新或标记为遗留。
- 应补充运行断言：两次兼容小分配的 `memblock->serial` 相同；释放后相邻 `memRegions` 合并；不兼容 descriptor/固定地址/共享对象不复用；最后一个对象释放后 DMAL block 释放；`memblockGetInfo` 的碎片率与最大 free region 一致。

## 修改配方

1. 新内存 API：在 API wrapper 构造完整 `CUmemdesc`，明确 `apiSource`，再进入 memobj；不要直接调用底层 RM。
2. 新 backing 类型：更新 `isValidAllocation`、compatibility 比较、HAL block size 和失败释放。
3. 修改映射：同时检查 UVA lookup、P2P register/unregister、host pointer lookup 和 free 的 base-pointer 语义。
4. 修改异步释放：确认 launch memory tracking 和 context synchronize 的顺序，不能在 GPU 仍可能访问时回收 backing。

## 风险

- descriptor 字段遗漏会错误复用不兼容 block（证据：[src/cui/memobj.c:126-163]）。
- global registration 失败而不回滚会留下局部对象（证据：[src/api/apimem.c:105-111]）。
- 释放 interior pointer 被拒绝是契约，不应为“方便”放宽（证据：[src/api/apimem.c:315-319]）。
- UVM page-fault/迁移路径跨越当前源码边界，任何性能或一致性结论都需运行时证据。
- `CUmembins` 在头文件和旧测试计划中存在，但未找到当前初始化/使用路径；不能以“5 个 bin 已启用”作为测试前提（静态证据：[src/cuda_mem.h:173-182]；[src/cui/memmgr.h:21-23,85-86]）。

## 相关文档
- [项目入口](../../README.md)
- [分析状态](../../00-overview/analysis-state.md)
- [源码证据索引](../../00-overview/evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](../../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
