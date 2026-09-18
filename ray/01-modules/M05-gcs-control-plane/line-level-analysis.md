# GCS、RPC、PubSub 控制面：行级分析

## 证据锚点
- `src/ray/gcs/gcs_server.cc:292-497,916-`

## 结论
- 入口负责验证/归一化，副作用发生于：GcsServer 初始化 manager 和服务并注册 RPC；状态写入 storage 后发布事件。
- 分支：cluster ID、节点重连、订阅者慢/断开、服务恢复。
- 错误：storage/RPC failure、heartbeat timeout、订阅断开。

动态行为未运行，不写成已验证。
