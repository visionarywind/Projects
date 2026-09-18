# Dashboard、观测、调试与认证：测试边界

|路径|覆盖目标|
|---|---|
|正常|Dashboard head/agent → modules → GCS/state/metrics/events → HTTP/gRPC response|
|分支|模块加载、节点离线、分页、exporter、token auth。|
|异常|认证/GCS/采集超时和 handler 错误。|
|清理|stop 取消采集并关闭 HTTP/subscriptions。|

测试位置：python/ray/dashboard/。当前只定位源码/测试目录，未启动上游构建、集群或硬件，结果为未验证。
