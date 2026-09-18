# Ray Serve：测试边界

|路径|覆盖目标|
|---|---|
|正常|serve.start → controller deploy → DeploymentState → router → replica → response|
|分支|滚动更新、autoscaling、reconfigure、ingress、batching、health。|
|异常|deploy/start/health/route/user exception。|
|清理|shutdown/delete 停 controller/router/replicas。|

测试位置：python/ray/serve/tests/。当前只定位源码/测试目录，未启动上游构建、集群或硬件，结果为未验证。
