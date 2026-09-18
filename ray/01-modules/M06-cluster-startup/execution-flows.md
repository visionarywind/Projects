# 启动、集群、Autoscaler、runtime env 与 Jobs：执行流

## 正常流
```text
入口 → 配置/参数归一化 → 内部状态创建 → ray.init/CLI → cluster connection → runtime env → job/task；autoscaler reconcile resource demand → 结果/事件
```

## 分支
auto-init、token、runtime env cache、扩缩容、job reconnect。

## 异常
连接/runtime env/node/job failure；历史命令不代表成功。

## 终止
shutdown、job stop、缩容和 reference cleanup。
