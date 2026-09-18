# Dashboard、观测、调试与认证：调用链

## 主链
```text
Dashboard head/agent → modules → GCS/state/metrics/events → HTTP/gRPC response
```

## 实现落点
head 启动模块，StateAPI 聚合状态，Reporter 采集指标；auth 和脱敏是边界约束。

## 失败链
```text
依赖/输入失败 → 认证/GCS/采集超时和 handler 错误。 → 重试、失败传播或清理
```
