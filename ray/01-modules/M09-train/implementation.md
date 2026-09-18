# Ray Train：实现

## 入口到副作用
trainer 将 config/scaling 转为 controller 和 worker group，各 worker 执行 loop 并报告结果。

## 正常
BaseTrainer.fit → controller → WorkerGroup → training_loop → metrics/checkpoint

## 分支与异常
- 故障恢复、checkpoint、dataset、resource scaling、rank。
- TrainingFailedError、worker death、user exception、checkpoint failure。

## 清理
WorkerGroup.shutdown 和 controller stop。

## 数据与执行上下文
- ScalingConfig、RunConfig、TrainContext、Checkpoint、Result。
- driver、controller loop、training worker actors。
