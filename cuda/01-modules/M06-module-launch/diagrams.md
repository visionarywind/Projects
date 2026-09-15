# M06 数据结构图

- 文档目的：解释 01-modules/M06-module-launch/diagrams.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M06-module-launch/diagrams.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


```mermaid
flowchart TB
 I[module image] --> M[CUmod]
 M --> F[CUfunc + metadata]
 F --> LD[CUIlaunchData]
 LD --> P[packed params]
 LD --> Q[QMD/const bank]
 LD --> S[stream push]
 LD -. capture .-> N[graph kernel node]
```

`CUfunc` 持有持久 metadata/引用；`CUIlaunchData` 绑定一次 launch，并被 tools/HAL 使用。

## Graph exec 资源

```mermaid
flowchart LR
 G[CUgraph capture] --> C[clone/flatten]
 C --> CT[per-context CUIgraphCtxData]
 CT --> Q[QMD + constant-bank]
 CT --> S[internal streams + markers]
 CT --> D[scheduler host/device backing]
 Q --> L[graph launch tracking]
 S --> L
 D --> L
 L --> X[completion marker / UVM DAG]
 X --> R[destroy: detach + deallocate + memobjFree]
```

`CUIgraphCtxData` 是资源聚合点：device scheduler backing 是 `CUmemobj`，其余数组和 staging 为 host-side allocations；所有资源都必须等待异步 work 的 marker/tracking 边界后再释放（静态确认：[src/cui/cuigraph.h:218-267]；[src/cui/cuigraph.c:1835-1933,1035-1064]）。

## Graph 编译与执行图

```mermaid
flowchart TB
    B[cuStreamBeginCapture] --> G[CUIgraph: nodes + DAG + capture state]
    G --> E[cuStreamEndCapture: join/invalidation checks]
    E --> C[clone exec]
    C --> F[flatten child/conditional]
    F --> L[lower memset]
    L --> T[topological scheduling]
    T --> P[pushbuffer groups]
    T --> Q[QMD chaining]
    T --> D[device scheduler nodes]
    P --> R[internal stream + QMD/marker]
    Q --> R
    D --> R
    R --> X[cuGraphLaunch]
    X --> W[marker/UVM/tracking]
    W --> Z[destroy and reverse release]
```

```mermaid
flowchart LR
    N[CUIgraphNode] --> E1[dependencies]
    N --> E2[dependentNodes]
    N --> S[ctx + internal stream]
    N --> M[completionMarker]
    Ctx[CUIgraphCtxData] --> Q[QMD array]
    Ctx --> B[constant-bank/HAL staging]
    Ctx --> K[kernel streams + ctx marker]
    Ctx --> D2[deviceNodes + index array + memobj backing]
```

图中 scheduler 节点代表 device-side successor/predecessor 状态机；其具体 kernel 代码和硬件完成语义不在当前源码快照内，不能据此宣称设备端运行已验证。

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
