# GCS、RPC、PubSub 控制面：调用链

## 主链
```text
GcsServer Start/DoStart → managers/services → RPC/KV/PubSub → node/actor/task state
```

## 实现落点
GcsServer 初始化 manager 和服务并注册 RPC；状态写入 storage 后发布事件。

## 失败链
```text
依赖/输入失败 → storage/RPC failure、heartbeat timeout、订阅断开。 → 重试、失败传播或清理
```
