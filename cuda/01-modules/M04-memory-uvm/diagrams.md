# M04 结构图

- 文档目的：解释 01-modules/M04-memory-uvm/diagrams.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M04-memory-uvm/diagrams.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


```mermaid
flowchart TB
 A[CUDA pointer] --> D[CUmemdesc]
 D --> O[CUmemobj]
 O --> B[CUmemblock]
 B --> H[DMAL/HAL resource]
 O --> U[UVA/UVM registry]
 O -. shared .-> B
```

多个 memobj 可以共享 backing block；pointer registry 则可能有多个 address/view。

## Suballocator 状态

```mermaid
flowchart LR
 R[request + CUmemdesc] --> C{可 suballocate?}
 C -- 否 --> B[new dedicated memblock]
 C -- 是 --> T[compatible radix tree]
 T --> F{best-fit free region?}
 F -- 是 --> S[split / attach memobj]
 F -- 否 --> N[new generic-size memblock]
 N --> S
 S --> U[memobj in use]
 U --> X[free region + coalesce]
 X --> E{last memobj?}
 E -- 否 --> T
 E -- 是 --> D[DMAL free + VA release]
```

`CUheap` 的 VA 管理不在此状态机内；它只在 new block 的 reserve/map 和 pointer lookup 阶段参与。

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
