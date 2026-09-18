# GCS、RPC、PubSub 控制面：执行流

## 正常流
```text
入口 → 配置/参数归一化 → 内部状态创建 → GcsServer Start/DoStart → managers/services → RPC/KV/PubSub → node/actor/task state → 结果/事件
```

## 分支
cluster ID、节点重连、订阅者慢/断开、服务恢复。

## 异常
storage/RPC failure、heartbeat timeout、订阅断开。

## 终止
Stop 关闭 RPC、订阅和 managers；不等同于内容立即删除。
