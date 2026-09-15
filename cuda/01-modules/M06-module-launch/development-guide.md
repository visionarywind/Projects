# M06 开发指南

- 文档目的：解释 01-modules/M06-module-launch/development-guide.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M06-module-launch/development-guide.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


1. 修改 module metadata 时同步 ELF parser、function creation、parameter validation/packing 和 debugger registration。
2. 修改 launchData 字段时同步 graph instantiate、HAL launchCheck、ABI encode 和 QMD。
3. 修改 launch setup 时保留 syscall→HAL check→const bank/QMD→push 的顺序。
4. 对失败路径检查 UVM DAG rollback、streamEndPush、tools launch-end 和 function transient state。
5. 修改 Graph instantiate 时同步检查 per-context QMD/constant-bank/marker/internal stream、scheduler backing 和 context lock 数组；修改 destroy 时按 reverse path 释放。
6. 修改 capture/update 时覆盖 origin/transitive stream、capture invalidation、per-graph-node ID 和 topology/function/memory-type 约束。
7. JIT/ELF 依赖外部 compiler/toolchain，本工作区仅做静态分析；Graph 运行时和失败注入测试也尚未执行。

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
