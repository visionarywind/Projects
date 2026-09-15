# M08 数据结构

- 文档目的：解释 01-modules/M08-tools-debug/data-structures.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M08-tools-debug/data-structures.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


| 对象 | 作用 |
|---|---|
| callback enable table | 按 domain/cbid 快速判断是否 dispatch |
| debugger context/module/grid maps | 将 driver 对象映射到调试 target 状态 |
| memcheck error/table buffers | device 错误信息和全局 allocation 镜像 |
| profiler global object | profile mode、mutex、perfmon 状态 |

## 所有权与异步边界

- callback enable table 和 callback 参数由 tools/etbl 层维护，但其中的 context、stream、module、function 指针借用 M02/M05/M06 对象；工具不能把 callback 返回视为对象所有权转移。
- memcheck `errorEntry`、device allocation table 和 host mirror 是与 `CUctx`/device memory 绑定的临时资源；创建失败应沿调用者回滚，context/record 销毁后才可释放 device 侧镜像（静态确认：[src/devtools/memcheck/memcheck.c:120-256]）。
- debugger shared variables 的生命周期跨越 attach session，具体 client/RPC 持有者不在当前树中；`cudbgApiCheck` 只证明访问前置条件，不证明跨进程同步。
- profiler global object 的 mutex 和 mode 是进程级状态，perfmon 命令却嵌入具体 launch push；模式销毁与 GPU counter 完成之间存在未闭合的异步边界。

工具对象与 M02/M04/M06 共享生命周期；callback 不等价于 GPU completion。具体注册/注销和外部 client 协议仍未知（静态确认：[src/etbl/tools/tools_callbacks_internal.h:22-41]；[src/devtools/debugger/cudbgapi.c:401-557]）。

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
