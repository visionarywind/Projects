# Ray Train：行级分析

## 证据锚点
- `python/ray/train/base_trainer.py:128-936`、`data_parallel_trainer.py:29-603`、TrainController/WorkerGroup

## 结论
- 入口负责验证/归一化，副作用发生于：trainer 将 config/scaling 转为 controller 和 worker group，各 worker 执行 loop 并报告结果。
- 分支：故障恢复、checkpoint、dataset、resource scaling、rank。
- 错误：TrainingFailedError、worker death、user exception、checkpoint failure。

动态行为未运行，不写成已验证。
