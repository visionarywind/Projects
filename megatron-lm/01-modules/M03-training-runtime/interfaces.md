# M03 接口、数据与上下文

## 接口

| 接口 | 输入 | 输出/副作用 |
|---|---|---|
| `pretrain` | config container、dataset provider、model type、forward step | 启动作业并进入 train |
| `get_model` | provider、model type、wrap/config/PG | model list |
| `setup_model_and_optimizer` | provider、model type、config | model、optimizer、scheduler |
| `train_step` | forward step、iterator、model、optimizer、schedule | reduced losses、skip、grad norm 等 |
| `train` | model/optimizer/data iterators | 迭代直到 stop |

## 生命周期

`args/config` 在初始化后由各模块共享；model list 贯穿 schedule、日志和 checkpoint；data iterator 由 M04 创建并由 train loop 消费；optimizer state 由 M05 持有。M03 不应假设 logits 已 gather，也不应在 schedule 之外重复执行 pipeline 通信。

## 执行上下文

训练上下文可能包括：CUDA device、distributed group、autocast/grad context、no-sync、activation checkpoint、CUDA graph capture、fault injector 和 telemetry。哪些上下文同时生效由参数组合决定，当前未动态穷举。