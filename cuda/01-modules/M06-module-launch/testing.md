# M06 测试与验证

- 文档目的：解释 01-modules/M06-module-launch/testing.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M06-module-launch/testing.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


- `basic_sanity:kernel` 通过内部 module/function 查找 `gpuIncrement`，检查 CNP 能力、launchCount、push count 和结果（静态确认：[tests/cuda_test/basic_sanity.cu:133-163,169-234]）。
- capture 分支的 graph node 创建和失败失效逻辑由 API common 实现，但当前没有运行时 capture 结果（静态确认：[src/api/apilaunch.c:252-286]）。
- 参数 metadata 缺失、非法 grid、function/stream 跨 context 属于应覆盖的错误路径；源码检查存在，测试执行未验证。

## 资源生命周期覆盖

当前 `tests/cuda_test/tests.nvmk` 未注册专用 CUDA Graph/capture 测试。`channel`/`marker` 测试中的 queued-dependency graph 是提交层内部图，不等同于 `cuGraph*` API。应新增以下可注入/可运行场景：

1. capture begin → kernel/memcpy/memset → end 的成功路径，以及 wrong-thread、unjoined 和 invalidated capture 的销毁路径；
2. 多 kernel/conditional graph instantiate 的 QMD、constant-bank、internal stream、marker 和 scheduler backing 分配/释放；
3. graph launch 重复执行、跨 context launch stream、节点中途失败和 destroy-after-launch；
4. QMD semaphore pool、`memobjAlloc` 或 `cuiStreamCreate` 失败时的 instantiate rollback；
5. `cuGraphExecUpdate` 的 topology/function/memcpy/memset 参数拒绝和成功更新。

以上均为测试建议或静态覆盖，当前没有 GPU/nvmake 执行结果。

不要把测试中的成功预期写成设备行为已证实；当前工作区缺少 nvmake、GPU 和外部依赖运行条件。

## Graph 验证矩阵

| 场景 | 静态覆盖 | 应验证的行为 | 当前状态 |
|---|---|---|---|
| capture kernel/memcpy/memset | `apilaunch.c`、`apistream.c` | 节点依赖、end capture graph 输出 | 未运行 |
| 多 stream/event capture | `cuistream.c` capture fields/nextPushDeps | transitive capture、cross-graph invalidation、unjoined 返回 | 未运行 |
| child/conditional graph | `cuigraph.c:2320-2437,1978-2051` | flatten、body roots/end、scheduler backing | 未运行 |
| memset lowering | `cuigraph.c:2534-2627` | kernel chain 数量和 update 映射 | 未运行 |
| 重复 graph launch | `cuigraph.c:4056-4162` | 前次 completion wait、marker reuse、UVM running | 未运行 |
| multi-context / alternate stream | `cuigraph.c:4075-4157` | 临时 lock、API stream 替换与失败恢复 | 未运行 |
| exec update | `apigraph.c:1332-1420`、`cuigraph.c:4619-4915` | topology/function/memory type/shape 拒绝和兼容更新 | 未运行 |
| destroy-after-launch / OOM 注入 | `cuigraph.c:1035-1205,3304-3492` | tracking 边界、资源逆序释放、无 UAF | 未运行 |

当前测试注册表没有专用 CUDA Graph API case；channel/marker 的 queued-dependency graph 只能覆盖提交基础设施，不等同于 Graph capture/instantiate/exec 语义。源码证明的是预期控制流，GPU、HAL/RM 和故障注入均未执行。

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
