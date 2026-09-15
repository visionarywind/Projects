# D01 最小 Core GPT 训练

- 文档目的：记录 `examples/run_simple_mcore_train_loop.py` 的端到端静态轨迹。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：`3703d4e33a3a2b2d11ebcc8e41f45af7ce7d1eda`
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 80-demos/D01-simple-mcore-training/README.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


- 状态：已完成静态阅读，未运行。

## 一句话

D01 是一个绕过完整 training 参数系统的 Core API 示例：直接初始化分布式、构造小 GPT、用 MockGPTDataset 训练 5 步，并保存/恢复 distributed checkpoint。

## 控制流

```text
initialize_distributed(TP=2, PP=1)
  -> model_parallel_cuda_manual_seed
  -> model_provider
  -> DDP wrap
  -> Adam
  -> get_train_data_iterator
  -> get_forward_backward_func
  -> 5 iterations
    -> zero_grad
    -> schedule forward/backward
    -> finalize_model_grads
    -> Adam.step
  -> save_distributed_checkpoint
  -> load_distributed_checkpoint
```

## 重点源码

- 初始化：[examples/run_simple_mcore_train_loop.py:28-53]
- 模型：[examples/run_simple_mcore_train_loop.py:56-78]
- 数据：[examples/run_simple_mcore_train_loop.py:81-120]
- forward/loss：[examples/run_simple_mcore_train_loop.py:123-165]
- checkpoint：[examples/run_simple_mcore_train_loop.py:168-217]
- 主循环：[examples/run_simple_mcore_train_loop.py:220-283]

## 未验证

尚未确认本机 GPU、NCCL、依赖和 helper 编译状态，因此不记录 loss 数值或 checkpoint 文件结果。

## 文档元数据（规范补充）

- 文档目的：说明 `80-demos/D01-simple-mcore-training/README.md` 的源码分析范围、结论和维护入口。
- 适用范围：当前项目对应模块/入口的静态源码与测试分析。
- 对应源码版本：以本项目 `00-overview/analysis-state.md` 或同页版本字段为准。
- 证据状态：静态源码证据；未执行的构建、测试、GPU、网络或多进程行为保持“未验证”。
- 最后更新：2026-09-15
- 前置阅读：本项目根 README 与 `00-overview/analysis-state.md`。
- 后续阅读：本模块/示例的实现、测试和风险页面。

## 深度审计

| 分析对象 | 入口落地 | 正常路径 | 分支 | 异常 | 清理 | 数据生命周期 | 执行上下文 | 行级证据 | Demo 映射 | 状态/缺口 |
|---|---|---|---|---|---|---|---|---|---|---|
| `80-demos/D01-simple-mcore-training/README.md` | 已完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 已映射或不适用 | 部分完成：动态行为、边界或专用变体仍需验证 |

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
