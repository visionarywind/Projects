# CUDA 项目总览：模块完成后的阅读入口

当前知识库覆盖 M01–M10 的导航、源码证据和主执行轨迹。核心结论均来自静态源码阅读；没有构建、GPU 或外部工具验证。

- 版本/范围：[项目定位](project-overview.md)
- 架构/运行时：[总体架构](architecture.md)、[运行时模型](runtime-model.md)
- 证据/状态：[证据索引](evidence-index.md)、[分析状态](analysis-state.md)
- 模块入口：[模块注册表](../01-modules/module-registry.md)
- 端到端：[D01](../80-demos/D01-cuda-test-memory-stream/README.md)、[跨模块串联](../90-cross-module/system-wiring.md)

推荐路径：M01→M02→M04→M05→M06→M03/M07；外围工具读 M08，OpenCL 读 M09，构建验证读 M10。
