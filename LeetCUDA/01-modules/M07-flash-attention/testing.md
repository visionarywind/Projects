# M07 测试

- 文档目的：解释 01-modules/M07-flash-attention/testing.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/LeetCUDA HEAD 4513b31（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M07-flash-attention/testing.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


覆盖 B/H/N/D 小组合、D=32/64/128/256 及更大 D 的过滤行为；比较 unfused torch、SDPA、官方 flash-attn（若安装）和不同 accumulator。检查 causal/非 causal（如接口支持）、非整 tile、极值输入、误差阈值和输出 layout。当前未在真实 CUDA 环境执行。

README 给出的依赖是 flash-attn、PyTorch 和 CUDA 的特定版本范围，需按目标环境确认。[kernels/flash-attn/README.md:106-118]

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
