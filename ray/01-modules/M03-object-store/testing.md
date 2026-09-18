# Object Store 与 Object Manager：测试边界

|路径|覆盖目标|
|---|---|
|正常|put/get → ObjectManager Pull/Push → Plasma/filesystem → chunk receive → available/free|
|分支|本地 Plasma、spill、pull 合并、取消、节点移除。|
|异常|对象丢失、传输失败、ObjectStoreFull、owner/node failure。|
|清理|CancelPull、FreeObjects、Stop 终止传输并释放 buffers。|

测试位置：src/ray/object_manager/。当前只定位源码/测试目录，未启动上游构建、集群或硬件，结果为未验证。
