# GCS、RPC、PubSub 控制面：接口契约

## 输入
调用方须提供可验证、可序列化且符合生命周期的输入；入口证据：`src/ray/gcs/gcs_server.cc:292-497,916-`。

## 输出与副作用
- GcsServer Start/DoStart → managers/services → RPC/KV/PubSub → node/actor/task state
- 核心数据：protobuf records、KV、PubSub channel、cluster ID。

## 失败
storage/RPC failure、heartbeat timeout、订阅断开。

跨模块修改需同步 binding/proto/日志/metrics/测试。
