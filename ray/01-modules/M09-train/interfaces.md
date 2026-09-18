# Ray Train：接口契约

## 输入
调用方须提供可验证、可序列化且符合生命周期的输入；入口证据：`python/ray/train/base_trainer.py:128-936`、`data_parallel_trainer.py:29-603`、TrainController/WorkerGroup。

## 输出与副作用
- BaseTrainer.fit → controller → WorkerGroup → training_loop → metrics/checkpoint
- 核心数据：ScalingConfig、RunConfig、TrainContext、Checkpoint、Result。

## 失败
TrainingFailedError、worker death、user exception、checkpoint failure。

跨模块修改需同步 binding/proto/日志/metrics/测试。
