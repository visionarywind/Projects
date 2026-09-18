# Ray Tune：测试边界

|路径|覆盖目标|
|---|---|
|正常|Tuner.fit → controller → Trial step → result/checkpoint → stop|
|分支|ASHA/PBT、resume、资源、实验目录、异步结果。|
|异常|trainable/trial/save-restore/scheduler failure。|
|清理|cleanup 停止 trials 并清理 actors/目录。|

测试位置：python/ray/tune/tests/。当前只定位源码/测试目录，未启动上游构建、集群或硬件，结果为未验证。
