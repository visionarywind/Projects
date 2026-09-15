# M01 设计原则

- 文档目的：解释 01-modules/M01-api-abi/design.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M01-api-abi/design.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


M01 将公开 ABI 稳定性与内部实现解耦：公开旧入口通过 v2/common wrapper 归一到 `CUdeviceptr_v2`、内部 context 和 CUI 对象；wrapper 负责参数和输出初始化，CUI 负责状态与资源（静态确认：[inc/cuda.h:62-170]、[src/api/apimem.c:51-118]）。

设计约束：API 入口必须保留旧符号兼容；context/stream/function 句柄必须校验来源与所属 context；异步对象的释放不能由 wrapper 直接决定。导出名、生成 API table 和 `cuda_master.def` 是同一 ABI 契约的不同构建表现。

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
