# D01-S06 修改练习

- 文档目的：解释 80-demos/D01-nms-python/modification-exercises.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/LeetCUDA HEAD 4513b31（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 80-demos/D01-nms-python/modification-exercises.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


1. **增加 launch error check**：在两个 kernel launch 后加入一致的 CUDA 检查，记录错误归因是否更清晰；更新 M08/M10 的错误边界。
2. **检查非 contiguous 输入**：传入切片/转置 boxes，确认 binding 是否接受、`.index_select(...).contiguous()` 覆盖了哪一段；不要把结果预先当作支持承诺。
3. **固定 ties**：构造相同 score，打印 `order_t` 和返回原始 index，验证 stable sort 语义。
4. **面积语义实验**：选择小框和 threshold 边界，同时调用 GPU、torchvision 和 `nms.cc` 参考，记录差异来源。
5. **mask 容量实验**：逐步增大 N，记录分配、kernel、CPU copy 和显存；在资源不足时停止，不把 O(N²/32) 视为无限可扩展。
6. **racecheck**：在明确有 CUDA 环境后运行 sanitizer，并把实际版本/输出写入 testing 文档。

所有练习当前未执行；修改只应同步更新源码证据、测试结果和风险登记。

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
