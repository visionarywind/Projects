# Raylet 调度与资源：测试边界

|路径|覆盖目标|
|---|---|
|正常|依赖满足 → RequestWorkerLease → 本地匹配或 spillback → worker 注册 → PushNormalTask|
|分支|资源不足、placement group、labels、租约重用、spillback、节点变化。|
|异常|不可调度、worker/Raylet 断连、placement group 删除。|
|清理|DestroyWorker、disconnect/failure、node removal 释放资源。|

测试位置：src/ray/raylet/。当前只定位源码/测试目录，未启动上游构建、集群或硬件，结果为未验证。
