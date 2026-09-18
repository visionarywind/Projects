# 启动、集群、Autoscaler、runtime env 与 Jobs：测试边界

|路径|覆盖目标|
|---|---|
|正常|ray.init/CLI → cluster connection → runtime env → job/task；autoscaler reconcile resource demand|
|分支|auto-init、token、runtime env cache、扩缩容、job reconnect。|
|异常|连接/runtime env/node/job failure；历史命令不代表成功。|
|清理|shutdown、job stop、缩容和 reference cleanup。|

测试位置：python/ray/autoscaler/。当前只定位源码/测试目录，未启动上游构建、集群或硬件，结果为未验证。
