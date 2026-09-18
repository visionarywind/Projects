# Dashboard、观测、调试与认证：风险与技术债

## 风险
- 认证/GCS/采集超时和 handler 错误。
- head 与 node agent 分进程，async handlers 与 RPC 并发。
- 异步引用、资源或 actor 清理不对称可能造成泄漏/悬挂。

## 证据缺口
动态集群、跨节点故障、压力和性能尚未验证；测试边界：python/ray/dashboard/。

## 维护
修改时同步更新 source-map、call-chains、testing 和跨模块索引，并保留已确认/推断/未验证标签。
