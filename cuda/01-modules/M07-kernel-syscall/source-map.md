# M07 源码地图、数据与风险

- 文档目的：解释 01-modules/M07-kernel-syscall/source-map.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M07-kernel-syscall/source-map.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## 文件地图

- `src/syscalls/assert`：device assert 相关资源。
- `src/syscalls/cnp`：child/nested launch 支持。
- `src/syscalls/graphs`：graph 相关 device support。
- `src/syscalls/malloc`、`minimalloc`、`hma-malloc`：device allocation 路径候选。
- `src/syscalls/printf`、`profile`、`debugger`：device-side observability。
- `src/kernels/x86_64`、`i686`：内建 kernel/架构变体。
- `src/asm`：架构汇编、模板和生成输入/输出。
- `src/cui/cuisyscall.c`：CUI 到 syscall 资源管理接口。

## 关键数据

- syscall resource bundle：launch 期间的 device-side syscall 地址、常量或 buffer（具体字段待核对）。
- CNP launch queue：child launch 请求的排队状态（类型声明见：[src/cuda_types.h:168-181]）。
- generated cubin/assembly：构建阶段产物，不应当当作当前源码的唯一证据。

## 风险

1. CNP、printf、device malloc 可能引入额外 launch/memory 依赖，必须纳入 tracking。
2. 架构 SASS/QMD 差异使同一 syscall 不能假定跨 GPU binary-compatible。
3. 外部 compiler/gpgpucomp 缺失时无法验证 device ABI、寄存器/constant-bank 布局和生成结果。

**深度状态**：目录和 launch 连接已完成；syscall 逐函数 ABI、kernel 生成命令、asm 到最终 cubin、CNP queue 消费未完成。

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
