# Dashboard、观测、调试与认证：行级分析

## 证据锚点
- `python/ray/dashboard/dashboard.py:33-107`、`head.py:49-551`、StateAPI/Reporter

## 结论
- 入口负责验证/归一化，副作用发生于：head 启动模块，StateAPI 聚合状态，Reporter 采集指标；auth 和脱敏是边界约束。
- 分支：模块加载、节点离线、分页、exporter、token auth。
- 错误：认证/GCS/采集超时和 handler 错误。

动态行为未运行，不写成已验证。
