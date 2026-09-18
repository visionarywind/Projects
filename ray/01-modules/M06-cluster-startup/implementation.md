# 启动、集群、Autoscaler、runtime env 与 Jobs：实现

## 入口到副作用
init 区分已有集群和本地启动；scheduler 计算供需，agent 设置缓存环境，JobManager 管理作业。

## 正常
ray.init/CLI → cluster connection → runtime env → job/task；autoscaler reconcile resource demand

## 分支与异常
- auto-init、token、runtime env cache、扩缩容、job reconnect。
- 连接/runtime env/node/job failure；历史命令不代表成功。

## 清理
shutdown、job stop、缩容和 reference cleanup。

## 数据与执行上下文
- ClusterID、NodeState、resource demand、runtime env cache、JobID。
- CLI/driver、head、node agent、autoscaler loop。
