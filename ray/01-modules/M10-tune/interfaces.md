# Ray Tune：接口契约

## 输入
调用方须提供可验证、可序列化且符合生命周期的输入；入口证据：`python/ray/tune/tuner.py:43-425`、`tuner_internal.py:63-698`、TuneController/Trial。

## 输出与副作用
- Tuner.fit → controller → Trial step → result/checkpoint → stop
- 核心数据：Experiment、Trial、Result、Checkpoint、search/scheduler state。

## 失败
trainable/trial/save-restore/scheduler failure。

跨模块修改需同步 binding/proto/日志/metrics/测试。
