# 快速上手

- 文档目的：解释 99-roadmap/quick-start.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/sglang HEAD 78be4b50af（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 99-roadmap/quick-start.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## 目标

先用最小成本确认源码、安装和测试入口，再决定是否启动模型。不要把进程启动成功误认为模型 ready，也不要把 Python import 成功误认为 GPU 路径正确。

## 推荐顺序

1. 阅读仓库 `CLAUDE.md` 和 `sglang/00-overview/analysis-state.md`。
2. 确认 Python 3.10+、PyTorch、CUDA/目标硬件和模型可用性。
3. 按仓库指导安装：`python3 -m pip install -e "python"`。**未验证**。
4. 运行环境诊断：`python3 -m sglang.check_env`。**未验证**。
5. 先运行无 GPU 依赖的静态检查或 focused unit test。
6. 有模型和 GPU 后再运行 D01 或 server；观察 ready、scheduler、KV pool 和输出状态。

## 第一条源码阅读线

```text
examples/runtime/engine/offline_batch_inference.py
  -> ServerArgs
  -> Engine
  -> TokenizerManager
  -> Scheduler
  -> ScheduleBatch
  -> ForwardBatch/ModelRunner
  -> BatchStrOutput/ReqState
```

## 不要跳过的判断

- `process started` 不等于 scheduler ready；
- tokenized request 不等于 scheduler-side `Req`；
- `next_token_ids` 不等于最终文本；
- CUDA Graph eligibility 不等于 graph replay 一定成功；
- 未运行命令必须保留“未验证”标记。

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
