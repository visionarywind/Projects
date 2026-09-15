# CUDA 项目总览：模块完成后的阅读入口

- 文档目的：解释 00-overview/README.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](analysis-state.md)。
## 结论摘要

本页聚焦 00-overview/README.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


当前知识库覆盖 M01–M10 的导航、源码证据和主执行轨迹。核心结论均来自静态源码阅读；没有构建、GPU 或外部工具验证。

- 版本/范围：[项目定位](project-overview.md)
- 架构/运行时：[总体架构](architecture.md)、[运行时模型](runtime-model.md)
- 证据/状态：[证据索引](evidence-index.md)、[分析状态](analysis-state.md)
- 模块入口：[模块注册表](../01-modules/module-registry.md)
- 端到端：[D01](../80-demos/D01-cuda-test-memory-stream/README.md)、[跨模块串联](../90-cross-module/system-wiring.md)
- 资源专题：[GPU 显存池化](../01-modules/M04-memory-uvm/gpu-memory-pooling.md)、[Graph 资源生命周期](../01-modules/M06-module-launch/graph-resource-lifecycle.md)

推荐路径：M01→M02→M04→M05→M06→M03/M07；外围工具读 M08，OpenCL 读 M09，构建验证读 M10。

## 相关文档
- [项目入口](../README.md)
- [分析状态](analysis-state.md)
- [源码证据索引](evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
