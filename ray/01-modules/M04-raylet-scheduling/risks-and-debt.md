# Raylet 调度与资源：风险与技术债

## 风险
- 不可调度、worker/Raylet 断连、placement group 删除。
- 每个 Raylet 管理本地资源，RPC 与调度循环并发。
- 异步引用、资源或 actor 清理不对称可能造成泄漏/悬挂。

## 证据缺口
动态集群、跨节点故障、压力和性能尚未验证；测试边界：src/ray/raylet/。

## 维护
修改时同步更新 source-map、call-chains、testing 和跨模块索引，并保留已确认/推断/未验证标签。
