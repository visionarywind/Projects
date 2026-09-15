# M03 执行流程

- 文档目的：解释 01-modules/M03-training-runtime/execution-flows.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/megatron-lm HEAD 3703d4e33（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M03-training-runtime/execution-flows.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## 作业启动

```text
parse args/YAML
  -> initialize_megatron
    -> distributed/model parallel/random seed
    -> optional dependency compilation/TP communicator
  -> setup_model_and_optimizer
  -> build datasets and iterators
  -> train
```

## 正常训练迭代

```text
set train mode
  -> determine forward_only / scheduled microbatches
  -> train_step
    -> clear gradients
    -> schedule forward/backward
    -> finalize gradients
    -> optimizer.step
    -> aggregate loss/norm/skip flags
    -> scheduler.step
  -> increment progress
  -> log / checkpoint / validate according to intervals
```

## 分支

- PP/VP 改变 model list 和 schedule。
- DDP/FSDP/FSDP2 改变梯度同步和参数 materialization。
- dynamic microbatch 改变每步 microbatch 数。
- CUDA graph、activation offload、fault tolerance 和 async save 可包裹主路径。

## 异常与清理

初始化失败通常在 distributed、依赖编译或 model construction 阶段向外抛出。optimizer skip 会保留训练状态并跳过 scheduler 推进。退出时由训练循环协调 async checkpoint finalize、profiling 和 distributed 状态；源码静态分析未确认所有异常分支都执行同一清理序列。

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
