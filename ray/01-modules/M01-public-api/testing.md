# 公共 API 与句柄：测试边界

|路径|覆盖目标|
|---|---|
|正常|@ray.remote → RemoteFunction._remote → _raylet submit_task → CoreWorker → ObjectRef → ray.get|
|分支|client mode、generator、placement group、单/列表引用和 timeout。|
|异常|序列化失败、RayTaskError、ObjectLostError、GetTimeoutError。|
|清理|显式 shutdown 清理 driver 状态；动态未验证。|

测试位置：python/ray/tests/。当前只定位源码/测试目录，未启动上游构建、集群或硬件，结果为未验证。
