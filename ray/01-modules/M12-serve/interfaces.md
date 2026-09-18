# Ray Serve：接口契约

## 输入
调用方须提供可验证、可序列化且符合生命周期的输入；入口证据：`python/ray/serve/api.py:82-166,902-1010`、`controller.py:1139-1196`、router/replica/deployment_state。

## 输出与副作用
- serve.start → controller deploy → DeploymentState → router → replica → response
- 核心数据：Application、DeploymentTargetState、ReplicaID、route metadata。

## 失败
deploy/start/health/route/user exception。

跨模块修改需同步 binding/proto/日志/metrics/测试。
