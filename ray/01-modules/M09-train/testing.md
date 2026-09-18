# Ray Train：测试边界

|路径|覆盖目标|
|---|---|
|正常|BaseTrainer.fit → controller → WorkerGroup → training_loop → metrics/checkpoint|
|分支|故障恢复、checkpoint、dataset、resource scaling、rank。|
|异常|TrainingFailedError、worker death、user exception、checkpoint failure。|
|清理|WorkerGroup.shutdown 和 controller stop。|

测试位置：python/ray/train/tests/。当前只定位源码/测试目录，未启动上游构建、集群或硬件，结果为未验证。
