# M04 代表性例子

- 文档目的：解释 01-modules/M04-memory-uvm/examples.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M04-memory-uvm/examples.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


- interior pointer 传给 `cuMemFree`：能找到对象但不等于 base pointer，因此返回 invalid value，保护 backing block。
- `cuMemFree(0)`：按 wrapper 语义成功返回，不进入对象释放。
- 固定 device VA 或 external sharing：禁止普通 suballocation，避免地址/所有权冲突。

### Best-fit 示例（静态推演）

假设同一 descriptor-compatible tree 有 64 KiB、256 KiB 两个 free region，请求经 alignment 取整后为 48 KiB。`radixTreeFindGEQ` 选择 64 KiB 区，切出 48 KiB 给 memobj，把剩余 16 KiB 插回 tree；释放该 memobj 时，若 16 KiB 与它相邻，两段会合并回 64 KiB。实际 block size、alignment 和物理地址由目标 HAL/DMAL 决定，示例不代表运行结果（机制证据：[src/cui/suballocator.c:163-220,343-405]）。

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
