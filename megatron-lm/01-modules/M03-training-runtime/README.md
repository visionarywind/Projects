# M03 训练运行时

- 文档目的：追踪从 `pretrain` 到单次迭代、优化器更新、日志和退出的控制流。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：`3703d4e33a3a2b2d11ebcc8e41f45af7ce7d1eda`
- 证据状态：静态源码分析；未运行 GPU 训练。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M03-training-runtime/README.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。

## 边界

M03 位于 `megatron/training`，负责作业初始化、模型/优化器装配、数据 iterator 连接、训练/验证迭代、动态 microbatch、日志、checkpoint 和退出条件。模型计算属于 M01，schedule 属于 M02，参数更新实现属于 M05。

## 主控制流

```text
pretrain
  -> initialize_megatron
  -> setup_model_and_optimizer
  -> build_train_valid_test_datasets/iterators
  -> train
    -> get_forward_backward_func
    -> train_step
      -> forward_backward_func
      -> optimizer.step
      -> scheduler.step
    -> training_log / save_checkpoint / validation
```

## 关键状态

- `args`/config：启动参数和 Transformer 配置
- model list：PP/VP chunk 集合
- optimizer/scheduler：参数更新和学习率状态
- data iterators：train/valid/test
- iteration/sample tokens：进度计数
- rerun/fault/profiling/CUDA graph 状态：控制特殊执行路径

## 证据

- `pretrain`：[megatron/training/training.py:1530-约 1630]
- `get_model`：[training.py:2435-约 2630]
- optimizer setup：[training.py:2632-约 2860]
- `train_step`：[training.py:3092-约 3390]
- `train`：[training.py:4309-约 5620]
- dataset iterator：[training.py:5627-约 5790]

## 当前限制

异步 checkpoint finalize、fault injection、FSDP 和 RL 扩展仅有静态入口证据；CUDA Graph 的 shared pool/static buffer/reset、optimizer replay、NCCL MemPool 注册和实际互斥条件仍未通过运行验证。详见 [CUDA Graph 与显存池生命周期](../../90-cross-module/cuda-graph-resource-lifecycle.md)。

## 文档元数据（规范补充）

- 文档目的：说明 `01-modules/M03-training-runtime/README.md` 的源码分析范围、结论和维护入口。
- 适用范围：当前项目对应模块/入口的静态源码与测试分析。
- 对应源码版本：以本项目 `00-overview/analysis-state.md` 或同页版本字段为准。
- 证据状态：静态源码证据；未执行的构建、测试、GPU、网络或多进程行为保持“未验证”。
- 最后更新：2026-09-15
- 前置阅读：本项目根 README 与 `00-overview/analysis-state.md`。
- 后续阅读：本模块/示例的实现、测试和风险页面。

## 深度审计

| 分析对象 | 入口落地 | 正常路径 | 分支 | 异常 | 清理 | 数据生命周期 | 执行上下文 | 行级证据 | Demo 映射 | 状态/缺口 |
|---|---|---|---|---|---|---|---|---|---|---|
| `01-modules/M03-training-runtime/README.md` | 已完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 已映射或不适用 | 部分完成：动态行为、边界或专用变体仍需验证 |

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
