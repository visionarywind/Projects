# 测试配方

- 文档目的：解释 99-roadmap/testing-recipes.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/sglang HEAD 78be4b50af（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 99-roadmap/testing-recipes.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## 静态文档检查

本批已执行：递归 Markdown 本地链接检查、fenced block 平衡检查和 `git diff --check`。当前结果：70 个 Markdown、256 个本地 Markdown/源码引用，其中 199 个文档链接和 57 个源码证据链接均可解析；未发现链接/fence/whitespace 错误。

## 本轮模块覆盖

M11-M18 已建立静态模块文章和测试入口地图：speculative、multimodal、KV transfer/HiCache、quant/MoE/LoRA、IPC、kernel/platform、Rust/router/gateway 和 suite/benchmark。它们只证明源码和测试入口已定位；未执行真实 GPU、模型、跨机传输、Rust build、完整 CI 或端到端服务。


仓库指导示例：

```bash
python3 test/registered/unit/mem_cache/test_radix_cache_unit.py
pytest test/registered/unit/mem_cache/ -v
```

这些命令本批未执行，不能写成通过。

## Scheduler 重点

- abort 与 metrics；
- decode retraction；
- mixed chunked prefill；
- priority scheduling；
- prefill delayer；
- chunk gate；
- batch overlap。

具体路径登记在 M04 文档，均未在本批执行。

## 运行测试的记录格式

记录 commit、硬件、模型、命令、环境变量、stdout/stderr、耗时和是否清理残留进程。GPU、多卡、NCCL/RCCL/HCCL、CUDA Graph 和端到端输出必须单独标记。

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
