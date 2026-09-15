# M07 行级分析

- 文档目的：解释 01-modules/M07-kernel-syscall/line-level-analysis.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M07-kernel-syscall/line-level-analysis.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


- `[src/cui/cuilaunch.c:177-199]`：function syscall imports 与 CNP module imports 合并，逐个追踪 syscall function memobj。
- `[src/cui/cuilaunch.c:255-269]`：在 HAL launchCheck 前调用 syscall callback，失败返回 launch resource error。
- `[src/cui/cuilaunch.c:291-316]`：cooperative launch 的 CNP/occupancy patch 条件。
- `[src/cui/cuisyscall.c:约 430-560]`：架构 syscall asm module、extern PC 表和 trampoline link。
- `[src/cui/cuisyscall.c:1151-1227]`：serialized `OnKernelLaunch` callback、shared-memory window 和 const-bank update。
- `[src/cui/cuisyscall.c:1229-1322]`：module import refcount、OnModuleLoad、timeout ISR/trap activation。
- `[src/cui/cuimod.c:2880-2921]`：unload 时 syscall refcount、ISR 和 trap deactivation。
- `[cudaSyscalls.nvmk:46-71,111-160]`：按架构 nvcc/bin2c 生成 CUDA/OpenCL syscall cubin arrays。
- `[tests/cuda_test/basic_sanity.cu:136-145]`：device kernel 递归 child launch 的编译条件。
- `[tests/cuda_test/basic_sanity.cu:214-225]`：测试按 CNP capability 计算 expected 并同步观察结果。

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
