# Dashboard、观测、调试与认证：执行流

## 正常流
```text
入口 → 配置/参数归一化 → 内部状态创建 → Dashboard head/agent → modules → GCS/state/metrics/events → HTTP/gRPC response → 结果/事件
```

## 分支
模块加载、节点离线、分页、exporter、token auth。

## 异常
认证/GCS/采集超时和 handler 错误。

## 终止
stop 取消采集并关闭 HTTP/subscriptions。
