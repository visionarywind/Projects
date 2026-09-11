# 运行时轨迹

## 正式 GPT 训练静态轨迹

```text
pretrain_gpt.py
  -> train_valid_test_datasets_provider
  -> model_provider/gpt_builder
  -> pretrain
    -> initialize_megatron
    -> setup_model_and_optimizer
    -> build_train_valid_test_data_iterators
    -> train
      -> train_step
        -> get_batch/forward_step
        -> GPTModel.forward
        -> schedule backward/finalize
        -> optimizer.step
      -> save/validate/log/exit
```

## D01 静态轨迹

D01 绕过正式参数系统，但覆盖 Core 模型、mock 数据、schedule、grad finalize 和 distributed checkpoint。它适合作为验证 M01-M05 接口的最小链，但不覆盖正式 checkpoint tracker、异步保存、复杂 PP/VP 或真实数据。