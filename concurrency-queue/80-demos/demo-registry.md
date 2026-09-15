# Demo 注册表

- 文档目的：列出真实 Demo 及其证据边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/concurrency-queue HEAD 683b9e3（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-10
- 前置阅读：[总入口](../README.md)
- 后续阅读：[D01](D01-unit-test-smoke/README.md)
## 结论摘要

本页聚焦 80-demos/demo-registry.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


| ID | Demo | 真实性 | 用途 | 执行状态 |
|---|---|---|---|---|
| D01 | unit test smoke | 仓库真实 executable | 端到端正确性和资源生命周期 | native smoke/`test_threaded` 已验证 |
| D02 | benchmark run | 仓库真实 executable/script | 性能测量和结果提取 | 未验证 |

D01 是默认学习路径；D02 只在需要性能问题时进入。两者都不能把“源码存在命令”写成“当前环境执行成功”。

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
