# 启动、集群、Autoscaler、runtime env 与 Jobs：风险与技术债

## 风险
- 连接/runtime env/node/job failure；历史命令不代表成功。
- CLI/driver、head、node agent、autoscaler loop。
- 异步引用、资源或 actor 清理不对称可能造成泄漏/悬挂。

## 证据缺口
动态集群、跨节点故障、压力和性能尚未验证；测试边界：python/ray/autoscaler/。

## 维护
修改时同步更新 source-map、call-chains、testing 和跨模块索引，并保留已确认/推断/未验证标签。
