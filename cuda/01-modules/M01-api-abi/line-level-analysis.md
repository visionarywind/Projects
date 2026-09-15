# M01 行级分析

- 文档目的：解释 01-modules/M01-api-abi/line-level-analysis.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M01-api-abi/line-level-analysis.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


- `[src/api/apiinit.c:19-47]`：先初始化全局 mutex，再拒绝非零 flags，最后进入 `cuiInit`；非法参数不会触发完整初始化。
- `[src/api/apimem.c:52-61]`：先把输出指针置零，使错误返回不会暴露未初始化地址。
- `[src/api/apimem.c:89-117]`：在 context lock 下分配 memobj，解锁后做 global registration；registration 是对象可见性的提交点。
- `[src/api/apimem.c:231-339]`：free 先按 unified/device pointer 查找，再校验 source/base pointer，成功同步后才 `memobjFreeNoSync`。
- `[src/api/apilaunch.c:252-286]`：capture 失败销毁新节点或使 capture 失效。

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
