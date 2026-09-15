# 性能指南

- 文档目的：解释 99-roadmap/performance-guide.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 99-roadmap/performance-guide.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


- 先区分同步边界：`cuMemFree` 显式同步，普通 launch 通常异步；debugger/profiler 可能额外阻塞。
- 观察 stream 并发策略、QMD 轮换、marker 和 channel push，避免以 API 返回时间代表 GPU 完成。
- memory suballocation 由 descriptor 兼容性、固定地址、sharing 和 generic block size 决定；改动需关注碎片与 backing 复用。
- profiler/perfmon 和工具 callback 会改变并发、WFI 和 push 时序，性能数据必须注明工具配置。

本页不提供实测数字；当前没有 GPU/profile 结果。

## 相关文档
- [项目入口](../README.md)
- [分析状态](../00-overview/analysis-state.md)
- [源码证据索引](../00-overview/evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
