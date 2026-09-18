# GCS、RPC、PubSub 控制面：实现

## 入口到副作用
GcsServer 初始化 manager 和服务并注册 RPC；状态写入 storage 后发布事件。

## 正常
GcsServer Start/DoStart → managers/services → RPC/KV/PubSub → node/actor/task state

## 分支与异常
- cluster ID、节点重连、订阅者慢/断开、服务恢复。
- storage/RPC failure、heartbeat timeout、订阅断开。

## 清理
Stop 关闭 RPC、订阅和 managers；不等同于内容立即删除。

## 数据与执行上下文
- protobuf records、KV、PubSub channel、cluster ID。
- 独立 GCS server 的 async handlers 与 storage/pubsub 回调并发。
