# M02 并行状态与 Pipeline 调度

- 文档目的：解释 Megatron Core 如何从并行配置建立 process groups，并用 schedule 驱动 microbatch。
- 对应源码版本：`8190837c2b6ce176a431bc2a6ffd3439507648a7`
- 证据状态：静态源码分析；未进行 GPU/NCCL 动态验证。
- 最后更新：2026-09-11

## 模块边界

M02 包含 `megatron/core/parallel_state.py` 与 `megatron/core/pipeline_parallel/`。前者负责 rank 拓扑、通信组、全局 buffer 和销毁；后者负责 forward/backward 顺序、microbatch bookkeeping、P2P 激活/梯度通信和梯度 finalize 的调用时机。它不拥有模型参数更新，也不负责构造数据集。

## 主链

```text
initialize_megatron
  -> torch.distributed.init_process_group
  -> initialize_model_parallel
    -> RankGenerator
    -> create_group(NCCL/Gloo)
  -> get_forward_backward_func
    -> no-pipeline / PP / interleaved schedule
      -> forward_step + backward_step
      -> P2P communicator
      -> finalize_model_grads
```

## 并行轴

| 轴 | 作用 | 主要状态 |
|---|---|---|
| TP | 切分单个权重/算子 | tensor model parallel group |
| PP | 切分层和 stage | pipeline group、P2P |
| VP | 每个 PP rank 持有多个虚拟 chunk | virtual pipeline rank/world size |
| DP | 复制模型并归约梯度 | data parallel group |
| CP | 切分序列激活 | context parallel group，梯度归约复用 DP 域 |
| EP | 切分 routed experts | expert model/data groups |
| GTP | 权重 rematerialization 轴 | GTP weight remat group |

## 关键不变量

- `torch.distributed` 必须先初始化；world size 必须能被模型并行规模整除。
- VP 只有在 PP 大于 1 时合法。
- GTP rematerialization 与多个 distributed-optimizer instance 当前由初始化逻辑禁止组合。
- decoder 与 expert rank generator 产生的 PP group 必须一致。
- schedule 的 model/data iterator 数量必须与选择的调度类型匹配。

## 证据索引

- 并行组初始化：[parallel_state.py:600-1029]
- 调度选择与输出释放：[schedules.py:53-220]
- 无 pipeline 调度：[schedules.py:723-928]
- 非交错 pipeline：[schedules.py:2147-约 2660]
- D01 调用：[examples/run_simple_mcore_train_loop.py:246-266]

## 未验证项

实际 rank placement、NCCL communicator 建立顺序、通信 overlap 和多节点性能需要相应硬件与 launcher 验证。