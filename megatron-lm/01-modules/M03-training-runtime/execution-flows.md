# M03 执行流程

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