# M02 风险与测试

- 文档目的：解释 01-modules/M02-runtime-context/risks-and-debt.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M02-runtime-context/risks-and-debt.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## 风险

1. **初始化部分成功**：新增初始化资源若未加入 `Error` 反向回滚，会泄漏或让后续状态假初始化；证据：[src/cui/cuiinit.c:3201-3208]。
2. **context 版本错配**：legacy API 在 v3020 context 上会返回 `CUDA_ERROR_INVALID_CONTEXT`，反之亦然；证据：[src/cui/cuiinit.c:3010-3025]。
3. **销毁竞态**：context destroy 假定调用者已处理 TLS；不遵守该前置条件可能留下 current context 悬挂，证据：[src/cui/cuictx.c:367-370]。
4. **sticky error 延迟观察**：错误可能在后续 API 才出现，测试若只检查发起调用会漏报，证据：[src/cui/cuiinit.c:3031-3037]。

## 测试证据

`basic_sanity.cu:init` 先验证未初始化状态，再 `cuInit(0)`，检查 globals、PID 和设备数；随后直接创建 context、分配/释放 memobj 并销毁 context（静态确认：[tests/cuda_test/basic_sanity.cu:49-100]）。测试未运行。

## 深度状态

初始化顺序、检查策略和 context 生命周期已完成静态深读；TLS 内部实现、context deinitialize 全量对象清单、各平台退出路径和 sticky error 产生端仍为下一批。

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
