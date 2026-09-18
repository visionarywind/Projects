# Ray Train：调用链

## 主链
```text
BaseTrainer.fit → controller → WorkerGroup → training_loop → metrics/checkpoint
```

## 实现落点
trainer 将 config/scaling 转为 controller 和 worker group，各 worker 执行 loop 并报告结果。

## 失败链
```text
依赖/输入失败 → TrainingFailedError、worker death、user exception、checkpoint failure。 → 重试、失败传播或清理
```
