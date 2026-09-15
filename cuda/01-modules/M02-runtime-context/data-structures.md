# M02 数据结构与状态

- 文档目的：解释 01-modules/M02-runtime-context/data-structures.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M02-runtime-context/data-structures.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


| 对象 | 关键状态 | 持有/并发 |
|---|---|---|
| `CUglobals` | initialized、devices、managers、initMutex | 进程级；初始化锁保护 |
| `CUItlsThreadData` | current context、线程 API 状态 | 每线程 TLS |
| `CUctx` | ACTIVE/DESTROYED、API version、sticky error、memmgr/stream/module | context 持有；API 操作按 context 锁同步 |
| primary context/TSG | primary ownership、attach references | context create/destroy 策略管理 |

初始化失败按反向顺序释放 manager；TLS push 失败会销毁刚创建的 context。sticky error 由异步路径产生、在初始化检查处观察；具体全部产生点未闭合（静态确认：[src/cui/cuiinit.c:2910-3039,3060-3208]；[src/api/apictx.c:67-84]）。

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
