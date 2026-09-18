# DAG、Workflow 与 Experimental channels：测试边界

|路径|覆盖目标|
|---|---|
|正常|DAGNode bind → execute/compile → channel writer/reader → downstream → output|
|分支|sync/async reader、batch、backpressure、teardown、error propagation。|
|异常|节点异常、channel closed、取消、编译约束。|
|清理|CompiledDAG teardown、monitor stop、channel cleanup。|

测试位置：python/ray/dag/。当前只定位源码/测试目录，未启动上游构建、集群或硬件，结果为未验证。
