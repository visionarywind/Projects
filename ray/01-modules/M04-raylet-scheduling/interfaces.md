# Raylet 调度与资源：接口契约

## 输入
调用方须提供可验证、可序列化且符合生命周期的输入；入口证据：`src/ray/raylet/node_manager.cc:199-373,575-609,1415-`、local/cluster lease manager。

## 输出与副作用
- 依赖满足 → RequestWorkerLease → 本地匹配或 spillback → worker 注册 → PushNormalTask
- 核心数据：ResourceRequest、WorkerLease、NodeID、scheduling class。

## 失败
不可调度、worker/Raylet 断连、placement group 删除。

跨模块修改需同步 binding/proto/日志/metrics/测试。
