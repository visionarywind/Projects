# M06 执行流程

- 文档目的：解释 01-modules/M06-module-launch/execution-flows.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M06-module-launch/execution-flows.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


```text
module image → ELF/fatbinary/JIT parse → CUmod → CUfunc metadata
→ API parameter validation/pack → CUIlaunchData
→ capture: graph node
→ normal: setup → QMD/const bank → stream push
```

launch setup 失败不应留下未跟踪的 syscall、const bank 或 UVM running 状态；已进入 push 后统一经 `streamEndPush`，最后发 launch-end callback。

## Graph exec

```text
capture → node/dependency graph → end validation
→ clone/flatten → per-context resource allocation
→ internal streams + completion markers + QMD/constant-bank
→ optional scheduler host/device backing
→ launch: API marker + UVM running + topological node push
→ completion markers
→ destroy: detach streams + release QMD/const/memobj/staging
```

失败路径需要同时覆盖 capture graph 销毁、instantiate 部分资源回滚、launch UVM rollback、临时 context lock 和 API stream 替换恢复；当前后两者存在静态疑点，尚未运行验证。

## Graph 完整执行流

```text
capture begin
  → graph origin/TLS/event state
  → operation: validate + create node + update stream capture deps
  → capture end: origin/thread/invalidation/join checks
  → instantiate: clone → flatten child/conditional → lower memset
  → assign contexts → topological scheduling
  → allocate per-context QMD/const-bank/HAL/marker/stream
  → optional device scheduler backing
  → launch: lock contexts → wait previous exec
  → UVM DAG running → API marker dependency
  → topological node launch
  → per-node marker → per-context completion marker
  → destroy: propagate completion QMD → detach/release
```

capture 的并发模型使用 origin stream 的 marker mutex 保护同一 capture graph 的 stream/event 状态；非 relaxed capture 还保存 TLS 关联，global capture 维护全局 active count。capture invalidation 或 end 时发现 unjoined stream 会清理所有参与 stream/event，并销毁未交付的 graph（静态确认：[src/cui/cuistream.c:2180-2268]）。

instantiate 后的节点 stream 通常是 graph-owned internal stream；若它与 graph 的 API stream 属于同 context，launch 期间可临时把这些节点切到实际 API stream，提交后恢复。launch 失败路径需要同步恢复该临时状态、UVM running 和动态 context lock；当前恢复分支存在静态待验证疑点（[src/cui/cuigraph.c:4056-4162]）。

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
