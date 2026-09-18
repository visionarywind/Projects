# Dashboard、观测、调试与认证：实现

## 入口到副作用
head 启动模块，StateAPI 聚合状态，Reporter 采集指标；auth 和脱敏是边界约束。

## 正常
Dashboard head/agent → modules → GCS/state/metrics/events → HTTP/gRPC response

## 分支与异常
- 模块加载、节点离线、分页、exporter、token auth。
- 认证/GCS/采集超时和 handler 错误。

## 清理
stop 取消采集并关闭 HTTP/subscriptions。

## 数据与执行上下文
- state records、events、metrics、token。
- head 与 node agent 分进程，async handlers 与 RPC 并发。
