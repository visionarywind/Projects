# CoreWorker、任务提交与执行：风险与技术债

## 风险
- 依赖/ runtime env / worker 启动失败、不可调度、Raylet 失联。
- driver/worker CoreWorker 的 IO service 与 RPC 回调并发。
- 异步引用、资源或 actor 清理不对称可能造成泄漏/悬挂。

## 证据缺口
动态集群、跨节点故障、压力和性能尚未验证；测试边界：src/ray/core_worker/。

## 维护
修改时同步更新 source-map、call-chains、testing 和跨模块索引，并保留已确认/推断/未验证标签。
