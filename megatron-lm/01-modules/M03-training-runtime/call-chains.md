# M03 调用链

```text
pretrain_gpt.main
  -> pretrain
    -> initialize_megatron
    -> setup_model_and_optimizer
      -> get_model
      -> get_megatron_optimizer
      -> get_optimizer_param_scheduler
    -> build_train_valid_test_data_iterators
    -> train
      -> get_forward_backward_func
      -> train_step
        -> schedule
        -> optimizer.step
        -> opt_param_scheduler.step
```

## 责任分界

- M03 决定何时调用各组件。
- M02 决定 microbatch 前后向及 PP 通信。
- M05 决定梯度更新是否成功及 optimizer state。
- M04 只提供 iterator，不决定训练次数或 checkpoint 间隔。