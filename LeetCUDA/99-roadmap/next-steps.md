# 下一步计划

- 文档目的：解释 99-roadmap/next-steps.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：`main` / `4513b31`。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-10
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 99-roadmap/next-steps.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。

## 源码深挖

1. 为 M06/M07 补深层 `.cu/.cuh` 的实际符号、layout、shared/register 生命周期和架构条件。
2. 为 M02-M05 逐目录核对空输入、尾部、stride、dtype、stream 和 launch error。
3. 重新核对 `.gitmodules` 和 submodule commit。

## 自动验证

在具备 GPU 后按顺序执行：

1. 记录 Python/PyTorch/CUDA/driver/GPU。
2. elementwise 最小 correctness。
3. NMS 固定回归 + 随机 sweep。
4. interview 单架构 build/smoke。
5. sanitizer，再做 benchmark/profile。

所有命令当前均未验证。

## 知识库维护

每次源码变更同步更新模块 README/source-map/testing、跨模块影响图、证据索引和 analysis-state；用脚本检查相对链接和引用行号漂移。

## 完成标准

文档覆盖、源码证据、链接、图节点真实性、未验证标记、测试结果和未解决问题都通过最终验收。

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
