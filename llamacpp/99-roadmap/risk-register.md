# 风险登记

- 文档目的：解释 99-roadmap/risk-register.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/llamacpp HEAD 093a2f86c（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 99-roadmap/risk-register.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


| ID | 风险 | 触发条件 | 缓解/验证 | 状态 |
|---|---|---|---|---|
| R01 | 行号与源码漂移 | 更新 commit | 重新核对 evidence index | 已知 |
| R02 | GGUF metadata/tensor 不匹配 | 新模型转换或 loader 改动 | converter + dummy + 实际模型质量测试 | 已知 |
| R03 | backend/硬件差异 | 改 ggml 或 device split | `test-backend-ops` + 目标硬件 CI | 已知 |
| R04 | context/KV 约束导致失败 | cache type、flash-attn、split 组合改变 | 检查 `llama_init_from_model` 错误分支 | 已知 |
| R05 | server queue/thread 回归 | handler、slot、response 改动 | 非流式/流式/断开/并行 pytest | 已知 |
| R06 | 默认文件暴露 | 新增 MCP/model save/load 能力 | 保持默认关闭并审查权限 | 已知 |
| R07 | UI/server API 不一致 | endpoint/schema 改动 | UI check/e2e + server tests | 已知 |
| R08 | 未验证文档被误读为事实 | 没有执行模型 Demo | 明确“未验证”并记录输出 | 当前存在 |
| R09 | stream ring 丢失旧 offset | 大响应或晚重连 | 检查 4 MiB/TTL/offset 错误路径 | 已知 |
| R10 | 重 post-processing 阻塞推理 | server context 线程新增工作 | 将 JSON/template 留在 HTTP 层 | 已知 |

---

**页尾：** 风险严重度尚未按生产部署环境重新排序。

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
