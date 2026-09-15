# M03 - common 应用基础设施

- 文档目的：解释 01-modules/M03-common-app-infra/README.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/llamacpp HEAD 093a2f86c（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M03-common-app-infra/README.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


> 状态：首版边界层；实现细节按专题文档增量展开。

## 职责

common 将 runtime 之上的应用语义集中到 `llama-common`：参数解析、日志/console、chat template、Jinja/PEG、grammar/JSON schema、sampling helper、download/cache、speculative、subprocess 等。[common/CMakeLists.txt:1-184]

## 数据边界

输入通常来自 argv、HTTP JSON 或模型缓存；输出是 typed common params、已格式化 prompt、token 序列、采样参数和 response 辅助对象。server 文档明确 JSON/template/tokenization 在 HTTP worker，inference thread 只接收 native C++ task 数据。[tools/server/README-dev.md:86-103]

## Chat/采样路径

server completion handler 从 JSON 取 prompt，调用 `process_mtmd_prompt` 或 `tokenize_input_prompts`，再调用 schema evaluator 形成 task params。[tools/server/server-context.cpp:4275-4321] 生成结果由上层 sampler/response state 处理，最后变成 JSON 或 SSE。[tools/server/server-context.cpp:4348-4414]

## 约束与风险

- llama.cpp 使用 `common/jinja`，不要引入 Minja。[AGENTS.md:83-86]
- 模板改变会改变 prompt token 数、assistant generation prompt 和测试固定结果。
- 下载/cache 和 subprocess 受平台、安全和网络影响；不能把本地未验证的缓存状态写成保证。

## 修改影响

改 parser/template/schema 需覆盖 server API、CLI 行为和相应 Python/C++ tests；改 sampler 需检查 completion、server、benchmark 和质量路径。CMake 中 common 依赖 llama、vendor、httplib 和线程。[common/CMakeLists.txt:1-184]

---

**页尾：** 本页不将应用层便利函数描述成公共 llama C API。

## 文档元数据（规范补充）

- 文档目的：说明 `01-modules/M03-common-app-infra/README.md` 的源码分析范围、结论和维护入口。
- 适用范围：当前项目对应模块/入口的静态源码与测试分析。
- 对应源码版本：以本项目 `00-overview/analysis-state.md` 或同页版本字段为准。
- 证据状态：静态源码证据；未执行的构建、测试、GPU、网络或多进程行为保持“未验证”。
- 最后更新：2026-09-15
- 前置阅读：本项目根 README 与 `00-overview/analysis-state.md`。
- 后续阅读：本模块/示例的实现、测试和风险页面。

## 深度审计

| 分析对象 | 入口落地 | 正常路径 | 分支 | 异常 | 清理 | 数据生命周期 | 执行上下文 | 行级证据 | Demo 映射 | 状态/缺口 |
|---|---|---|---|---|---|---|---|---|---|---|
| `01-modules/M03-common-app-infra/README.md` | 已完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 已映射或不适用 | 部分完成：动态行为、边界或专用变体仍需验证 |

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
