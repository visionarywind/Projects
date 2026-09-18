# M12 Ray Serve

- 文档目的：说明 Serve application、controller、deployment state、router、replica 和 handle 的静态实现链。
- 对应源码版本：HEAD `cfe4725d23`（2026-09-17）。
- 证据状态：API、controller、router、replica、deployment state 代表链已静态确认；运行和测试未验证。
- 前置阅读：[模块注册表](../module-registry.md)。后续阅读：[实现](implementation.md) 与 [D03](../../80-demos/D03-serve-handle/README.md)。

## 结论摘要

`serve.run` 将 application 提交到 Serve controller；controller 更新 `DeploymentState`，创建或调整 replica actors；router 根据可用 replica 处理 `DeploymentHandle` 请求，replica 执行用户 callable。[`api.py:82-166,902-1010`; `controller.py:134-1988`; `router.py:538-1660`; `replica.py:1080-3637`; `deployment_state.py:2952-5924`]

## 代表链

```text
Deployment.bind
→ serve.run
→ ServeController.deploy_applications
→ DeploymentState target/reconcile
→ replica actor start/health
→ router choose replica
→ DeploymentHandle.remote
→ user method / response
```

## 分支与清理

滚动更新、autoscaling、reconfigure、HTTP/gRPC ingress、batching、placement、health check 和 backpressure 改变状态机。部署或 replica 失败时 controller 可重试、替换或标记 unhealthy；用户异常沿 response 返回。`serve.shutdown/delete` 终止 controller/router/replica 和 metrics，但真实时序未验证。

## 深度审计

|对象|入口落地|正常|分支|异常|清理|数据|上下文|证据|Demo|状态|
|---|---|---|---|---|---|---|---|---|---|---|
|M12|API/controller/state/router/replica|run→reconcile→route→response|update/autoscale/health/ingress|deploy/replica/user failure|shutdown/delete|Application/ReplicaID/route metadata|driver/controller/proxy/replica|代表源码已列|D03|动态未验证|

## 相关文档

[项目架构](../../00-overview/architecture.md) · [实现](implementation.md) · [D03](../../80-demos/D03-serve-handle/README.md)

## 源码证据摘要

`python/ray/serve/api.py:82-166,902-1010`；`python/ray/serve/_private/controller.py:134-1988`；`router.py:538-1660`；`replica.py:1080-3637`；`deployment_state.py:2952-5924`。

## 未解决问题

HTTP/gRPC 入口、路由 backpressure、graceful shutdown、replica failure recovery 和动态部署性能仍需运行专项测试；静态代表链已闭合。

## 下一步阅读建议

用 D03 先读 `serve.run`/`DeploymentHandle`，再追 controller reconciliation 和 replica state。
