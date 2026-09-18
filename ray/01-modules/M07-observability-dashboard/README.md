# M07 Dashboard、观测、调试与认证

- 版本：HEAD `cfe4725d23`；静态代表链已补充；动态执行与测试仍未验证。

## 结论摘要

Ray 将 Dashboard、State/metrics/events、分布式调试和 token authentication 分布在 Python dashboard、`_common/observability`、C++ observability/stats 及 security 文档/实现中；它们消费控制面和任务事件，而不是任务执行算法本身。[已确认目录/文档]

## 安全边界

token 必须由新 RPC 端到端传播；Dashboard 回显 runtime_env 时需防止把 `env_vars` 中凭据泄露给浏览器。该约束来自仓库安全规则，新增 endpoint/RPC 时必须复核。[已确认：`.claude/rules/security.md`]

## 深度审计

| 对象 | 入口 | 正常 | 分支 | 异常 | 清理 | 数据 | 上下文 | 行证据 | Demo | 状态 |
|---|---|---|---|---|---|---|---|---|---|---|
| M07 | 目录已定位 | 未完成 | 未完成 | 未完成 | 未完成 | 部分 | 服务/agent | 部分 | D01 task events 间接 | 静态深化完成，动态未验证 |

## 相关文档
[错误模型](../../00-overview/global-error-model.md) · [调试指南](../../99-roadmap/debugging-guide.md)

## 源码证据摘要
`python/ray/dashboard/`；`python/ray/_common/observability/`；`src/ray/observability/`；`src/ray/stats/`；`doc/source/ray-security/token-auth.md`。

## 未解决问题
Dashboard head/agent 进程、State API、metrics exporter、event pipeline、debugger 和 auth RPC 需拆解。

## 下一步阅读建议
从 Dashboard 启动与 HTTP/gRPC handlers 开始，再追踪事件生产者。
