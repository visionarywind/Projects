# 启动、集群、Autoscaler、runtime env 与 Jobs：调用链

## 主链
```text
ray.init/CLI → cluster connection → runtime env → job/task；autoscaler reconcile resource demand
```

## 实现落点
init 区分已有集群和本地启动；scheduler 计算供需，agent 设置缓存环境，JobManager 管理作业。

## 失败链
```text
依赖/输入失败 → 连接/runtime env/node/job failure；历史命令不代表成功。 → 重试、失败传播或清理
```
