# M04 设计

- 文档目的：解释 01-modules/M04-memory-uvm/design.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M04-memory-uvm/design.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


M04 采用 descriptor→object→backing block→HAL/DMAL resource 的分层，使统一地址、共享 backing、内部 suballocation 和外部导入能复用同一内存管理基础。`CUmemdesc` 记录语义，`CUmemobj` 记录 API-visible 切片，`CUmemblock` 表示一次实际 backing。

## 池化策略

源码中的通用池化是 `suballocator.c` 管理的 block 内空闲区，而不是独立的 `CUmemoryPool` 对象：可兼容的 descriptor 共用一个 radix tree；tree 以空闲区大小为 key，best-fit 取出并按 alignment 切分，free 时相邻区合并。HAL 仅决定建议 block size/alignment；真正的显存申请仍由 DMAL 负责。详见 [GPU 显存池化](gpu-memory-pooling.md)。

释放必须验证 API source 和 base pointer，并在异步使用完成后再释放；UVA registry 是 pointer lookup 的全局索引。最后一个 memobj 释放后，空闲区 tree 节点和整个 memblock 的 backing 按顺序销毁。

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
