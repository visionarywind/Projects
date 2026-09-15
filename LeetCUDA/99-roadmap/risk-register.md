# 风险登记

- 文档目的：解释 99-roadmap/risk-register.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：`main` / `4513b31`。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-10
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 99-roadmap/risk-register.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


| ID | 风险 | 影响 | 缓解/状态 |
|---|---|---|---|
| R01 | 架构/工具链矩阵未统一 | 编译失败或 no kernel image | 构建前固定 GPU、CUDA、SM；未验证 |
| R02 | 输入检查不一致 | 非法访问或错误结果 | 每个 binding 声明契约；逐模块补测 |
| R03 | 异步错误延迟 | 错误归因错误 | 同步/检查 launch；interview 已有 `check()` |
| R04 | layout/swizzle 不匹配 | Tensor Core 错误/性能退化 | 对照实例化和 reference |
| R05 | NMS mask/resolve 边界 | 错误保留集或显存压力 | ties、N=0、大 N、sanitizer |
| R06 | 第三方 submodule 漂移 | API/header 变化 | 记录 commit，构建前初始化固定版本 |
| R07 | 历史 benchmark 被误读 | 错误性能结论 | 标记硬件/版本/未验证 |
| R08 | 无统一 CI/harness | 回归不易发现 | 建议后续添加分层 smoke/correctness |

证据入口：[global-error-model](../00-overview/global-error-model.md)、[error-boundaries](../90-cross-module/error-boundaries.md)。

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
