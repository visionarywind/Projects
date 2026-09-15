# M08 风险与技术债

- 文档目的：解释 01-modules/M08-tools-debug/risks-and-debt.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M08-tools-debug/risks-and-debt.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


| 风险 | 静态依据 | 后果 |
|---|---|---|
| callback 递归/重入 | debugger 有 TLS callback inactive 检查 `[src/devtools/debugger/cudbgapi.c:487-495]` | 工具 callback 可能重入 driver |
| begin/end 不配对 | `cuiLaunch` 的显式契约 `[src/cui/cuilaunch.c:468-475,789-800]` | 工具看到错误的 launch 生命周期 |
| 工具改变 launch | `pSkipInternalLaunch` `[src/cui/cuilaunch.c:476-485,1044-1078]` | 主路径可能被跳过但仍需清理 |
| 额外 device 依赖 | memcheck error/table `[src/devtools/memcheck/memcheck.c:120-256]` | 工具资源可能早于 GPU 完成释放 |
| 外部协议不可见 | debugger RPC/IPC 变量 `[src/devtools/debugger/cudbgdriver.c:103-198]` | 无法静态确认 attach 与 RM 协议 |
| 工具资源与 context 销毁竞态 | memcheck device table/error entry `[src/devtools/memcheck/memcheck.c:120-256]` | 可能在异步 launch 完成前释放或遗留 device 资源 |
| profiler 全局模式与 launch counter 脱节 | mode mutex 与 push 内 perfmon 分属不同路径 `[src/profiler/profiler_common.c:21-96]`；`[src/cui/cuilaunch.c:635-710]` | host 状态完成不代表设备计数器完成 |

**深度等级：中→深（host-side）。** 主 launch callback、skip/blocking 控制、debugger 状态闸门、memcheck device 镜像和 profiler 插入点已覆盖；外部客户端协议、完整 memcheck patch/helper 生成链、DRS 运行时合并、具体 profiler backend 和设备端完成语义仍未闭合。

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
