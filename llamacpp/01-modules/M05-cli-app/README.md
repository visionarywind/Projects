# M05 - CLI 与统一应用

- 文档目的：解释 01-modules/M05-cli-app/README.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/llamacpp HEAD 093a2f86c（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M05-cli-app/README.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


> 状态：首版入口层分析。

## 统一入口

`app/llama.cpp` 用静态 command table 将 `serve`、`cli`、`download` 以及隐藏的 bench/quantize/perplexity 等名称映射到实现函数；匹配后设置 `LLAMA_APP_CMD` 并调用函数。[app/llama.cpp:64-78] [app/llama.cpp:129-148]

## CLI 边界

`tools/cli` 产出可复用 `llama-cli-impl` 和 executable；它与 common、server impl 共享基础设施，CLI 的具体 prompt loop 和采样处理在 `tools/cli/cli*.cpp`，需在专题变更时继续追踪。[tools/cli/CMakeLists.txt:1-27]

## 最小运行路径

`llama-cli -m MODEL -p PROMPT -n N` 是产品入口；它最终调用 M02 的 model/context/decode，读取 logits 并通过 common sampler 输出 token。命令来源已确认于根开发指导，但本批未执行。

## 错误与影响

统一入口未知 command 返回 1；子命令错误由子实现返回。修改 command table 会影响 aliases、router child re-invocation 和安装二进制名称；修改 CLI loop 会影响 tokenizer/template/sampler 行为。

---

**页尾：** CLI 的全量交互状态机尚未逐行展开。

## 文档元数据（规范补充）

- 文档目的：说明 `01-modules/M05-cli-app/README.md` 的源码分析范围、结论和维护入口。
- 适用范围：当前项目对应模块/入口的静态源码与测试分析。
- 对应源码版本：以本项目 `00-overview/analysis-state.md` 或同页版本字段为准。
- 证据状态：静态源码证据；未执行的构建、测试、GPU、网络或多进程行为保持“未验证”。
- 最后更新：2026-09-15
- 前置阅读：本项目根 README 与 `00-overview/analysis-state.md`。
- 后续阅读：本模块/示例的实现、测试和风险页面。

## 深度审计

| 分析对象 | 入口落地 | 正常路径 | 分支 | 异常 | 清理 | 数据生命周期 | 执行上下文 | 行级证据 | Demo 映射 | 状态/缺口 |
|---|---|---|---|---|---|---|---|---|---|---|
| `01-modules/M05-cli-app/README.md` | 已完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 已映射或不适用 | 部分完成：动态行为、边界或专用变体仍需验证 |

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
