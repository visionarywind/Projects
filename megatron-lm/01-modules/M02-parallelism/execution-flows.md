# M02 执行流程

- 文档目的：解释 01-modules/M02-parallelism/execution-flows.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/megatron-lm HEAD 3703d4e33（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M02-parallelism/execution-flows.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## 初始化

1. launcher 设置 `RANK/WORLD_SIZE/LOCAL_RANK`。
2. training initialize 建立 torch distributed。
3. `initialize_model_parallel` 检查规模，创建 rank groups 和 accessor 状态。
4. 模型、数据 loader、optimizer 从 accessor 取得需要的通信域。

## 一次无 pipeline 迭代

```text
train_step
  -> optimizer.zero_grad
  -> forward_backward_no_pipelining
    -> microbatch 1..N: forward_step
    -> backward（最后一个 microbatch 外触发同步）
    -> finalize_model_grads
  -> optimizer.step
```

## 一次非交错 PP 迭代

```text
warmup: recv_forward -> forward -> send_forward
steady: recv_forward -> forward -> send_forward
        recv_backward -> backward -> send_backward
cooldown: recv_backward -> backward -> send_backward
```

具体通信可能因首尾 stage、双向 overlap、重计算和 variable sequence length 分支而不同；上图是控制结构而非每个分支的精确调用序列。

## 异常与清理

- 不合法并行规模在 group 创建前抛出 `RuntimeError`/`AssertionError`。
- 非交错 schedule 启用不支持的 `overlap_p2p_comm` 会抛出 `ValueError`。
- 进程组销毁通过 `destroy_model_parallel` 清理全局 group 和 global memory buffer。
- NCCL、CUDA 和 autograd 的底层异常向 schedule/training 上层传播；本仓库未进行动态故障验证。

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
