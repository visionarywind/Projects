# Ray Tune：调用链

## 主链
```text
Tuner.fit → controller → Trial step → result/checkpoint → stop
```

## 实现落点
Tuner 解析 trainable/config，controller 驱动 trial，search/scheduler 决策。

## 失败链
```text
依赖/输入失败 → trainable/trial/save-restore/scheduler failure。 → 重试、失败传播或清理
```
