# 构建、CI、proto/codegen 与测试：测试边界

|路径|覆盖目标|
|---|---|
|正常|workspace/toolchain → proto generation → C++/extension → wheel → test target|
|分支|debug/release、平台/Python/GPU、cache、optional deps。|
|异常|依赖/编译/proto/ABI/test timeout failure。|
|清理|sandbox/cache/artifacts 由 job 生命周期清理。|

测试位置：CI/Bazel targets（未运行）。当前只定位源码/测试目录，未启动上游构建、集群或硬件，结果为未验证。
