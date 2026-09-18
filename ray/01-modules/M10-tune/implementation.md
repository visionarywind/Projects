# Ray Tune：实现

## 入口到副作用
Tuner 解析 trainable/config，controller 驱动 trial，search/scheduler 决策。

## 正常
Tuner.fit → controller → Trial step → result/checkpoint → stop

## 分支与异常
- ASHA/PBT、resume、资源、实验目录、异步结果。
- trainable/trial/save-restore/scheduler failure。

## 清理
cleanup 停止 trials 并清理 actors/目录。

## 数据与执行上下文
- Experiment、Trial、Result、Checkpoint、search/scheduler state。
- driver Tuner、controller loop、trial tasks/actors。
