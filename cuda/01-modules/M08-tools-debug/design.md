# M08 设计

- 文档目的：解释 01-modules/M08-tools-debug/design.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M08-tools-debug/design.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


M08 采用旁路插桩而非复制 runtime：callback 通过 enable table 快速判断，debugger 通过状态检查和 attach 协议访问运行时，profiler 通过全局 mode/perfmon 影响 launch，memcheck 通过 patch 和额外 memory table 检测访问。

工具能力可能改变主路径（blocking、skip launch、code patch），因此必须记录其控制权而非只记录日志功能。

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
