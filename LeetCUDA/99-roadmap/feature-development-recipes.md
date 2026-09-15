# 功能开发配方

- 文档目的：解释 99-roadmap/feature-development-recipes.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：`main` / `4513b31`。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-10
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 99-roadmap/feature-development-recipes.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。

## 新增普通 kernel

1. 复制 `kernels/elementwise/` 的最小 Python + `.cu` 结构。
2. 明确 dtype/device/shape/stride/contiguous、空输入和尾部契约。
3. 在 binding 中检查输入并导出符号。
4. 写 torch reference 和小尺寸 correctness。
5. 加 warmup、同步、计时，再写 README。
6. 运行 pre-commit（未验证）。

## 修改布局或 tile

先画 global/shared/register 数据流；保留原版本作为 correctness baseline；覆盖非倍数尺寸、不同架构和 sanitizer；只有 correctness 通过后才比较性能。

## 修改 PyTorch API

同步更新 `.cu/.cc` 导出、Python 参数、示例、错误信息和测试。不要在异步 kernel 完成前保存裸指针或让输入 tensor 生命周期结束。

## 修改 Interview 公共头文件

列出所有 include 消费者，按每个 `--arch` 编译，并运行至少一个 correctness phase；公共宏的改动可能影响多个阶段。

## 证据

`[CONTRIBUTE.md:5-31]`；`[kernels/elementwise/elementwise.py:9-66]`；`[kernels/nms/nms.cu:126-189]`。

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
