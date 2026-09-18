# DAG、Workflow 与 Experimental channels：风险与技术债

## 风险
- 节点异常、channel closed、取消、编译约束。
- driver graph、compiled tasks、monitor。
- 异步引用、资源或 actor 清理不对称可能造成泄漏/悬挂。

## 证据缺口
动态集群、跨节点故障、压力和性能尚未验证；测试边界：python/ray/dag/。

## 维护
修改时同步更新 source-map、call-chains、testing 和跨模块索引，并保留已确认/推断/未验证标签。
