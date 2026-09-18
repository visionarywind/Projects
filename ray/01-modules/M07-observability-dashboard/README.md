# M07 Dashboard、观测、调试与认证

- 版本：HEAD `cfe4725d23`；Dashboard head/agent、State API、Reporter 和安全边界已静态确认；动态执行与测试未验证。

## 结论摘要

Dashboard head 启动模块和 HTTP/gRPC 服务，StateAPIManager 聚合控制面状态，ReporterAgent 采集节点/process metrics；事件、stats、debugger 和 token auth 是观测/运维边界，不是任务执行算法。[`dashboard.py:33-107`; `head.py:49-551`; `state_aggregator.py:61-697`; `reporter_agent.py:475-2212`]

## 代表链

```text
Dashboard.run / DashboardHead.run
→ module registration
→ GCS/state subscriptions or polling
→ StateAPIManager aggregation
→ ReporterAgent metrics/events
→ HTTP/gRPC handler response
```

## 分支、安全与清理

模块加载、节点离线、分页/filter、metrics exporter、debugger、认证和 runtime_env 回显改变 handler 路径。新 gRPC endpoint 必须传播 token；Dashboard 浏览器回显 runtime_env 时必须脱敏 `env_vars` 凭据。[`.claude/rules/security.md`]

Dashboard stop 应取消采集任务、关闭 HTTP server、RPC clients 和 agent subscriptions；真实关闭顺序未运行验证。

## 深度审计

|对象|入口落地|正常|分支|异常|清理|数据|上下文|证据|Demo|状态|
|---|---|---|---|---|---|---|---|---|---|---|
|M07|head/state/reporter 已定位|启动→采集→聚合→响应|module/node/auth/filter/exporter|GCS/auth/采集 timeout|server/tasks/subscriptions stop|state/events/metrics/token|head+node agents|代表源码已列|D01 间接|动态未验证|

## 相关文档

[错误模型](../../00-overview/global-error-model.md) · [调试指南](../../99-roadmap/debugging-guide.md) · [实现](implementation.md)

## 源码证据摘要

`python/ray/dashboard/dashboard.py:33-107`; `head.py:49-551`; `state_aggregator.py:61-697`; `modules/reporter/reporter_agent.py:475-2212`; `src/ray/observability/`; `doc/source/ray-security/token-auth.md`。

## 未解决问题

具体 HTTP/gRPC handlers、event pipeline、metrics exporter、debugger 和 auth RPC 的动态权限/失败行为仍需专项测试；静态服务边界已建立。

## 下一步阅读建议

从 DashboardHead.run 读启动，再沿 StateAPIManager/ReporterAgent 追踪数据生产者和 token 传播。
