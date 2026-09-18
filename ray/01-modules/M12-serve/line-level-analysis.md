# Ray Serve：行级分析

## 证据锚点
- `python/ray/serve/api.py:82-166,902-1010`、`controller.py:1139-1196`、router/replica/deployment_state

## 结论
- 入口负责验证/归一化，副作用发生于：controller 管理 deployment state，router 选择 replica，replica 执行用户 callable。
- 分支：滚动更新、autoscaling、reconfigure、ingress、batching、health。
- 错误：deploy/start/health/route/user exception。

动态行为未运行，不写成已验证。
