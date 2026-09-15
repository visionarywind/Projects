# M06 风险与技术债

- 文档目的：解释 01-modules/M06-module-launch/risks-and-debt.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M06-module-launch/risks-and-debt.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


| 风险 | 证据 | 影响 |
|---|---|---|
| 参数 metadata 缺失 | `[src/api/apilaunch.c:141-147]` | 现代 kernelParams 被拒绝 |
| packed/unpacked 双路径 | `[src/api/apilaunch.c:155-205]` | 兼容复制和生命周期复杂 |
| capture 失败使 graph invalidated | `[src/api/apilaunch.c:270-278]` | 后续 API 需观察 capture invalidated |
| launch 异步引用 | `[src/cui/cuilaunch.c:163-218]` | function/module/memobj 不能过早释放 |
| module unload 反向清理 | shared ELF、function、syscall、UVM、CNP cache 和 tools 状态需按顺序撤销 | 部分失败/重复 unload 的观察点跨多个所有权域 |
| graph 动态 context | launch stream 可能不在 instantiate context 集合，且 API stream 指针临时替换 | 锁数组、marker 和 UVM rollback 必须保持一致 |
| instantiate 锁数组错误回收 | `ctxLocks` 写入 exec 后，QMD pool 注册失败仍进入 Error；Error 先 free 数组再 destroy graph | 静态上可能出现释放后再次 lock/unlock；需注入失败并用 ASan 验证（[src/cui/cuigraph.c:3457-3492,1144-1187]） |
| launch 失败未恢复 API stream | `replaceApiStream` 只在成功提交后恢复；node launch 失败直接跳到 Done | graph exec 可能留下临时 stream 指针，影响后续 launch；需构造失败节点验证（[src/cui/cuigraph.c:4116-4157]） |
| scheduler backing 生命周期 | device graph nodes/indexes 是 `CUmemobj` + host backing，依赖 stream tracking 和 context lock | 过早释放会导致设备读取悬挂；destroy 必须走 memobj 同步边界（[src/cui/cuigraph.c:1898-1932,1035-1064]） |

**深度状态**：API launch、参数、tracking、setup、module/ELF/JIT、graph capture/instantiate/launch/unload/update 和资源 reverse path 已完成静态主线；架构 QMD/ABI 具体字段、compiler/gpgpucomp、设备执行和上述失败风险复现仍未闭合。

## Graph 专项风险

- **调度选择依赖架构能力**：QMD chaining 是否可用由 CNP、context、设备能力和工具开关共同决定；只修改一个边判断可能改变整个 scheduling group、cache 和 scheduler node 布局（静态证据：[src/cui/cuigraph.c:2630-2923]）。
- **device scheduler backing 是设备可见状态**：host `deviceNodes/indexes` 与 device `CUmemobj` 必须在异步 graph work 完成后释放；不能按普通 host graph metadata 处理（[src/cui/cuigraph.c:1835-1933,1978-2051]）。
- **capture invalidation 的传播范围**：多 stream、captured event、TLS mode 和 global active count 共同决定 capture 是否可结束；错误路径必须清事件、摘 stream 并销毁 graph（[src/cui/cuistream.c:2180-2268,2288-2311]）。
- **exec update 不是重新编译**：更新数组按 `perGraphNodeId` 对齐，memset lowering 后还需保持 kernel 数量与原节点映射一致；改变 topology、function、memory type、context 或资源形状必须拒绝（[src/cui/cuigraph.c:4619-4915]）。
- **已识别的错误恢复疑点**：instantiate 的 semaphore-pool 注册失败可能与已写入的 `ctxLocks` 清理顺序冲突；launch 节点失败可能跳过 API stream 指针恢复。这两项仍是静态疑点，需故障注入/ASan 或 GPU 回归确认，不能写成必现 bug（[src/cui/cuigraph.c:3457-3492,4116-4157]）。

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
