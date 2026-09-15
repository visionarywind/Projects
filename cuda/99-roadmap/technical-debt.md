# 技术债

- 文档目的：解释 99-roadmap/technical-debt.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 99-roadmap/technical-debt.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


- 源码目录缺少 Git 元数据，无法绑定具体 commit/branch。
- 外部 common/RM/compiler/firmware 和 nvmake 配置不完整，多个动态分派只能到边界。
- M06 的 ELF/JIT、Graph 失败注入/设备执行、M07 syscall/asm 生成链、M04 UVM fault/migration/DMAL 物理实现、M05 GPFIFO/marker 设备消费尚未闭合。
- M04 旧测试计划仍以 `membins/membin` 描述池化，但当前实现使用 `CUsuballocatorRadixTree`；需要清理术语并补充 best-fit、split/coalesce 和碎片验证。
- OpenCL enqueue/interop 对象销毁、tools 注册/attach 协议、MODS/CI/package 仍需源码核对。
- 所有构建、DVS、GPU、OpenCL、debugger、profiler 结果未验证。

下一步以 `00-overview/analysis-state.md` 的模块表和 `99-roadmap/next-steps.md` 为准。

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
