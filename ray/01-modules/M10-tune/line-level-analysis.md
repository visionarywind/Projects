# Ray Tune：行级分析

## 证据锚点
- `python/ray/tune/tuner.py:43-425`、`tuner_internal.py:63-698`、TuneController/Trial

## 结论
- 入口负责验证/归一化，副作用发生于：Tuner 解析 trainable/config，controller 驱动 trial，search/scheduler 决策。
- 分支：ASHA/PBT、resume、资源、实验目录、异步结果。
- 错误：trainable/trial/save-restore/scheduler failure。

动态行为未运行，不写成已验证。
