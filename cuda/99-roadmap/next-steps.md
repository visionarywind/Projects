# 后续路线

- 文档目的：解释 99-roadmap/next-steps.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 99-roadmap/next-steps.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


1. 为 M04 suballocator 和 M06 Graph 补充 nvmake/GPU 运行验证及 instantiate/launch 失败注入，核对 README、调用链、数据结构、行级证据和风险页之间的链接与证据等级。
2. 更新总入口/模块注册表（若有遗漏），特别检查 M09 public/internal refcount、event marker aggregation、GL/external memobj 所有权和 M10 no-result/timeout/waived 结果语义是否被导航页引用。
3. 若需要继续深挖，优先读取 MODS、package、CI 和 OpenCL enqueue/worker 的源码；将它们作为补充，不回退已经完成的 M08–M10 host-side 结论。
4. 在完整外部树和硬件环境中再执行构建与 DVS；把日志作为新的已验证证据。

当前阶段的结论仍以静态确认、推断、未知和未验证为主；DMAL/RM/firmware、compiler/gpgpucomp、nvcc 生成物、外部 debugger/profiler 客户端和 GPU 行为没有被本轮运行验证。

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
