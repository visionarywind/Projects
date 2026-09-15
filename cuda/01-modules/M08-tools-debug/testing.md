# M08 测试与验证

- 文档目的：解释 01-modules/M08-tools-debug/testing.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M08-tools-debug/testing.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## 可静态核对的路径

- launch callback 的 begin/end 配对及 skip 行为可由 `cuilaunch.c` 分支核对（静态确认：[src/cui/cuilaunch.c:468-503,779-817]）。
- profiler mode 与 `CUPTI_PROFILE_MODE` 环境变量读取可由 profiler 源码核对（静态确认：[src/profiler/profiler_common.c:21-56,85-96]）。
- memcheck 的 device error entry/table 分配、下载和失败清理可由 `memcheck.c` 核对（静态确认：[src/devtools/memcheck/memcheck.c:120-180,206-256]）。

## 运行状态

debugger attach、integrated memcheck、profiler counter 和工具注入都需要外部客户端、驱动权限或 GPU；本项目未执行，结果统一为未验证。

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
