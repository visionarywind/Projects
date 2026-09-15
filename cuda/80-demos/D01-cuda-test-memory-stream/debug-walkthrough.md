# D01 调试 walkthrough

- 文档目的：解释 80-demos/D01-cuda-test-memory-stream/debug-walkthrough.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 80-demos/D01-cuda-test-memory-stream/debug-walkthrough.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


1. 先看 dispatcher 的 GPU ordinal/过滤输出（静态确认：[tests/cuda_test/dispatch_main.cpp:22-50]）。
2. 若初始化失败，沿 `cuapiInit`→`cuiInit` 检查 globals、TLS、UVM/UVA 和 primary memmgr。
3. 若 kernel 失败，检查 function/stream 是否同一 context、参数 metadata、CNP capability 和 launch syscall setup。
4. 若提交失败，按 `streamBeginPush`、HAL finalize/control、`streamEndPush` 和 marker 状态定位。
5. 工具启用时额外检查 callback begin/end、debugger blocking、profiler wait 和 memcheck allocation。

这是静态调试路线；没有实际失败日志。

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
