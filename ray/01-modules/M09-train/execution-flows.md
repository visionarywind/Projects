# Ray Train：执行流

## 正常流
```text
入口 → 配置/参数归一化 → 内部状态创建 → BaseTrainer.fit → controller → WorkerGroup → training_loop → metrics/checkpoint → 结果/事件
```

## 分支
故障恢复、checkpoint、dataset、resource scaling、rank。

## 异常
TrainingFailedError、worker death、user exception、checkpoint failure。

## 终止
WorkerGroup.shutdown 和 controller stop。
