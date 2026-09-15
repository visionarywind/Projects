# M06 行级分析

- 文档目的：解释 01-modules/M06-module-launch/line-level-analysis.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M06-module-launch/line-level-analysis.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


- `[src/cui/cuilaunch.c:164-218]`：收集 function、syscall、context、const bank、module/function-scope memobj 的 launch tracking。
- `[src/cui/cuilaunch.c:231-285]`：local memory、syscall callback、HAL launchCheck、const bank/QMD 和 tools setup。
- `[src/cui/cuilaunch.c:318-334]`：架构 ABI const-bank/grid 编码和 launchInitialize。
- `[src/cui/cuilaunch.c:419-503]`：context lock、launchCount、gridid、tools begin 和 setup。
- `[src/cui/cuilaunch.c:582-817]`：参数 finalize、push、HAL control、同步/错误回滚和 launch end。
- `[src/cui/cuimod.c:1835-1891]`：ELF image copy、shared ELF hash/refcount。
- `[src/cui/cuimod.c:2363-2548]`：JIT、module load transaction、UVM/syscall/CNP 顺序和失败卸载。
- `[src/cui/cuimod.c:2767-2943]`：module unload reverse path、shared ELF/syscall/UVM/tool 清理。
- `[src/cui/cuigraph.c:3304-3492]`：graph instantiate 的 clone/flatten/context lock/QMD pool registration。
- `[src/cui/cuigraph.c:4056-4162]`：graph launch 的动态 context lock、marker、UVM running 和 rollback。
- `[src/cui/cuistream.c:2180-2268]`：capture graph 创建、TLS/global mode 计数、unjoined/invalidation 检查和失败销毁。
- `[src/cui/cuigraph.c:1835-1933]`：per-context QMD、constant-bank、HAL staging 和 scheduler host/device backing。
- `[src/cui/cuigraph.c:1621-1809,2054-2141]`：kernel node resource slice、internal stream/marker 和 instantiate traversal。
- `[src/cui/cuigraph.c:3495-3575,3741-3880]`：kernel group memory tracking 与拓扑 node launch。
- `[src/cui/cuigraph.c:1035-1064,1093-1205]`：exec resource teardown、stream detach、QMD/constant-bank/marker/memobj 释放。
- `[src/api/apigraph.c:1332-1420]`、`[src/cui/cuigraph.c:4619-4915]`：graph exec update 的 topology 和参数约束。
- `[src/cui/cuigraph.c:3457-3492,4116-4157]`：instantiate 锁数组、launch API stream 恢复的静态风险点。

## Graph 关键行级证据

- `[src/api/apistream.c:538-575]`：begin-capture wrapper 校验 mode、stream/context 和 null stream。
- `[src/cui/cuistream.c:2180-2211]`：创建 capture graph，保存 origin、TLS、mode、invalidation 和 event 状态。
- `[src/api/apilaunch.c:252-286]`：capturing kernel 不走普通 push，而验证参数、创建 node、更新 capture info。
- `[src/cui/cuistream.c:2218-2268]`：end capture 检查 wrong-thread、invalidated、unjoined，并清理 stream/event 或销毁 graph。
- `[src/cui/cuigraph.c:2320-2437]`：递归 flatten child/conditional graph，重挂节点和 source/sink 边。
- `[src/cui/cuigraph.c:2534-2627]`：将可由 kernel 实现的 memset 展开为 kernel node chain，并保存原节点映射。
- `[src/cui/cuigraph.c:2630-2923]`：pushbuffer/QMD chaining/scheduler 选择、拓扑排序、group/cache 和 scheduler node 插入。
- `[src/cui/cuigraph.c:1835-1933,1978-2051]`：per-context 执行资源与 device scheduler node/index 编码。
- `[src/cui/cuigraph.c:2054-2141]`：internal stream、node resource、scheduler 参数 finalize 的实例化遍历。
- `[src/cui/cuigraph.c:3495-3731]`：kernel group tracking、QMD patch/finalize、semaphore 初始化和 pushbuffer 提交。
- `[src/cui/cuigraph.c:3741-3898]`：节点依赖 marker、不同 node type launch 和 leaf completion 汇聚。
- `[src/cui/cuigraph.c:4056-4162]`：多 context lock、前次 completion、UVM running、拓扑 launch 和失败 rollback。
- `[src/api/apigraph.c:1332-1420]`、`[src/cui/cuigraph.c:4619-4915]`：exec update 的 topology/node ID/参数兼容性检查。

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
