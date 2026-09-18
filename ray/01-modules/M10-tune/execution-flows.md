# Ray Tune：执行流

## 正常流
```text
入口 → 配置/参数归一化 → 内部状态创建 → Tuner.fit → controller → Trial step → result/checkpoint → stop → 结果/事件
```

## 分支
ASHA/PBT、resume、资源、实验目录、异步结果。

## 异常
trainable/trial/save-restore/scheduler failure。

## 终止
cleanup 停止 trials 并清理 actors/目录。
