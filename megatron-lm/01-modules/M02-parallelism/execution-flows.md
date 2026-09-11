# M02 执行流程

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