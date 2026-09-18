# GCS、RPC、PubSub 控制面：风险与技术债

## 风险
- storage/RPC failure、heartbeat timeout、订阅断开。
- 独立 GCS server 的 async handlers 与 storage/pubsub 回调并发。
- 异步引用、资源或 actor 清理不对称可能造成泄漏/悬挂。

## 证据缺口
动态集群、跨节点故障、压力和性能尚未验证；测试边界：src/ray/gcs/。

## 维护
修改时同步更新 source-map、call-chains、testing 和跨模块索引，并保留已确认/推断/未验证标签。
