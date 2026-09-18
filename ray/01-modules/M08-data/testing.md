# Ray Data：测试边界

|路径|覆盖目标|
|---|---|
|正常|read_datasource → logical plan → StreamingExecutor → block tasks → Dataset|
|分支|map/batches、streaming、repartition、split、cache、retry。|
|异常|datasource/decode、pickle object column、task failure、OOM。|
|清理|executor shutdown 取消 stages 并释放 refs/cache。|

测试位置：python/ray/data/tests/。当前只定位源码/测试目录，未启动上游构建、集群或硬件，结果为未验证。
