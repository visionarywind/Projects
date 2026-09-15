# M07 风险与技术债

- 文档目的：解释 01-modules/M07-kernel-syscall/risks-and-debt.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M07-kernel-syscall/risks-and-debt.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


| 项目 | 状态 |
|---|---|
| 生成链不可见 | `cudaAsm.nvmk`/`cudaSyscalls.nvmk` 依赖外部工具和配置 |
| 架构 binary 差异 | Kepler–Ampere 各有 SASS/QMD/launch 文件 |
| CNP 异步依赖 | child launch 可能继续使用父 launch 资源 |
| 外部边界 | compiler/gpgpucomp、RM、firmware 未在树中完整出现 |
| trampoline/device ABI | 原函数 PC 经 extern symbol table 链入架构 trampoline | cubin、编译器和设备侧调用约定不在当前手写 C 源码中闭合 |
| import refcount | module bitset 驱动 0→1 初始化、0 时停用 | callback 失败与卸载回滚需要跨 module/context 检查 |
| 生成物选择 | `cudaSyscalls.nvmk` 按 `DEV_ARCHS` 生成并嵌入每架构 cubin | 未运行 nvcc/bin2c，不能把产物匹配标为已验证 |

证据：运行时初始化/回调/销毁见 `[src/cui/cuisyscall.c:124-205,378-734,1157-1347]`；生成源清单见 `[cudaSyscalls.nvmk:46-71,111-160]`；CNP 测试见 `[tests/cuda_test/basic_sanity.cu:136-145,214-225]`。模块深度已达到 host 侧静态主线，设备侧 queue/ABI 仍未知。

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
