# 构建、CI、proto/codegen 与测试：风险与技术债

## 风险
- 依赖/编译/proto/ABI/test timeout failure。
- 开发机或 CI worker 的编译/测试进程。
- 异步引用、资源或 actor 清理不对称可能造成泄漏/悬挂。

## 证据缺口
动态集群、跨节点故障、压力和性能尚未验证；测试边界：CI/Bazel targets（未运行）。

## 维护
修改时同步更新 source-map、call-chains、testing 和跨模块索引，并保留已确认/推断/未验证标签。
