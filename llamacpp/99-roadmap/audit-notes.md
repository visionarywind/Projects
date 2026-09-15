# M01-M09 文档页头/页尾模板与深度审计说明

- 文档目的：解释 99-roadmap/audit-notes.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/llamacpp HEAD 093a2f86c（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 99-roadmap/audit-notes.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


本文件不是模块文档，而是本批分析方法说明。

## 证据规则

- 已确认：能在当前 HEAD 的源码、CMake、测试或开发文档中定位到的事实。
- 推断：由构建边界、调用关系或多个已确认事实归纳出的解释。
- 未知：本批没有源码证据或没有实际运行结果的内容。
- 行号格式统一为 `[相对路径:起始行-结束行]`。

## 覆盖矩阵

| Demo | M01 | M02 | M03 | M04 | M05 | M06 | M07 | M08 | M09 |
|---|---|---|---|---|---|---|---|---|---|
| D01 server chat | graph/backend 边界 | decode/model | tokenize/schema | route/task/slot | - | 可选 MTMD 分支 | - | pytest/CTest | - |
| D02 CLI | graph/backend 边界 | load/context/decode | params/sampler | - | command dispatch | - | model source | build/命令 | - |

## 审计范围

- **实现深度：** D01 已穿过 HTTP handler、task/result、slot batch、`llama_decode`、context graph compute 和 ggml scheduler；未穿过每个 model graph/kernel。
- **Demo 深度：** D01/D02 已给出可执行命令、主路径、错误和模块映射；没有模型和 build，因此无运行输出。
- **证据一致性：** 所有首版文档使用固定 HEAD 和相对路径；自动行号校验未执行。
- **断链：** 根 README 的内部链接已按目录生成；没有运行 Markdown link checker。
- **风险：** server 外部文件默认关闭、SSE ring 有界、context 错误清理等关键风险已记录；router/MCP/UI 仍需专项。

## 维护协议

源码变更后先更新 `analysis-state.md` 版本快照，再用 `grep`/编译器/LSP 或人工 Read 校验受影响行号；运行命令后将 Demo 状态和结果追加到对应页面，而不是删除“未验证”声明。

---

**页尾：** 该说明记录当前审计结论，不表示全仓库已经逐文件审计。

## 相关文档
- [项目入口](../README.md)
- [分析状态](../00-overview/analysis-state.md)
- [源码证据索引](../00-overview/evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
