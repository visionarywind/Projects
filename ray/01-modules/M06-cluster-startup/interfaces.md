# 启动、集群、Autoscaler、runtime env 与 Jobs：接口契约

## 输入
调用方须提供可验证、可序列化且符合生命周期的输入；入口证据：`python/ray/_private/worker.py:1439-1505`、`python/ray/scripts/scripts.py:800-1318,1976-2095`、autoscaler/runtime_env/job_manager。

## 输出与副作用
- ray.init/CLI → cluster connection → runtime env → job/task；autoscaler reconcile resource demand
- 核心数据：ClusterID、NodeState、resource demand、runtime env cache、JobID。

## 失败
连接/runtime env/node/job failure；历史命令不代表成功。

跨模块修改需同步 binding/proto/日志/metrics/测试。
