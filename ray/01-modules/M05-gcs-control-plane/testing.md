# GCS、RPC、PubSub 控制面：测试边界

|路径|覆盖目标|
|---|---|
|正常|GcsServer Start/DoStart → managers/services → RPC/KV/PubSub → node/actor/task state|
|分支|cluster ID、节点重连、订阅者慢/断开、服务恢复。|
|异常|storage/RPC failure、heartbeat timeout、订阅断开。|
|清理|Stop 关闭 RPC、订阅和 managers；不等同于内容立即删除。|

测试位置：src/ray/gcs/。当前只定位源码/测试目录，未启动上游构建、集群或硬件，结果为未验证。
