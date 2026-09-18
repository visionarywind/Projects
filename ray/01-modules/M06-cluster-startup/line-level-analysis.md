# 启动、集群、Autoscaler、runtime env 与 Jobs：行级分析

## 证据锚点
- `python/ray/_private/worker.py:1439-1505`、`python/ray/scripts/scripts.py:800-1318,1976-2095`、autoscaler/runtime_env/job_manager

## 结论
- 入口负责验证/归一化，副作用发生于：init 区分已有集群和本地启动；scheduler 计算供需，agent 设置缓存环境，JobManager 管理作业。
- 分支：auto-init、token、runtime env cache、扩缩容、job reconnect。
- 错误：连接/runtime env/node/job failure；历史命令不代表成功。

动态行为未运行，不写成已验证。
