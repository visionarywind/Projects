# M05 GCS、RPC、PubSub 控制面

- 版本：HEAD `cfe4725d23`；已完成 GCS server 生命周期与服务初始化的静态代表分析；动态执行与测试未验证。

## 结论摘要

GCS 是控制面，不是对象内容或用户任务执行面。`GcsServer::Start/DoStart` 建立节点、资源、作业、Actor、placement group、task、worker、runtime-env、KV 和 PubSub 等 manager/service，再由 `RegisterRpcServices` 暴露 RPC；`Stop` 负责逆向关闭。[已确认：`src/ray/gcs/gcs_server.cc:81-106,292-497,916+`]

## 启动与服务

```text
GcsServer constructor
→ Start
→ GetOrGenerateClusterId
→ DoStart
→ manager/service initialization
→ RegisterRpcServices
→ node/worker/actor/task/placement updates
→ PubSub/KV consumers
→ Stop
```

初始化点包括 KV service `InitKVService`、PubSub `InitPubSubHandler`、runtime env manager `InitRuntimeEnvManager` 以及 worker/autoscaler/task manager。[`gcs_server.cc:916+`]

## 数据与边界

- protobuf Node/Actor/Task/PlacementGroup records、cluster ID、KV 和 PubSub channel 属于控制元数据。
- 节点资源/心跳、Actor 状态和任务状态由 GCS manager 维护；对象 bytes 仍属于 M03。
- Raylet/CoreWorker 通过 RPC client 读取、注册、watch 或发布状态；RPC 完成不等于数据任务完成。

## 分支、错误与清理

- 节点重连、heartbeat 超时、PubSub subscriber 断开和 manager 初始化失败改变控制面状态。
- storage/RPC failure 由 client/reconnect/retry 边界处理；具体策略依服务和配置而异。
- `Stop` 关闭 RPC、PubSub、KV 和 managers；控制面退出不自动证明对象内容已经回收。
- token auth 是新增 RPC/endpoint 的安全约束；新增 dashboard runtime_env 回显必须脱敏。[`.claude/rules/security.md`]

## 深度审计

|对象|入口落地|正常|分支|异常|清理|数据|上下文|证据|Demo|状态|
|---|---|---|---|---|---|---|---|---|---|---|
|GCS server|Start/DoStart/RegisterRpcServices|service→state→publish|reconnect/subscriber/config|storage/RPC/heartbeat|Stop|protobuf/KV/PubSub|独立 server async RPC|`gcs_server.cc` 代表区间|D01 间接|动态未验证|

## 相关文档

[实现](implementation.md) · [接口](interfaces.md) · [M02](../M02-core-worker/README.md) · [M04](../M04-raylet-scheduling/README.md)

## 未解决问题

具体 protobuf RPC 方法、持久化后端、故障转移和控制面压力需专项源码/测试；本页已不再把服务方法标为未定位。

## 下一步阅读建议

先读 `GcsServer::DoStart` 的 manager 初始化，再按 Node/Actor/Task/PlacementGroup manager 进入具体 RPC。

## 源码证据摘要

`src/ray/gcs/gcs_server.cc:81-106,292-497,916+`；`src/ray/gcs/`；`src/ray/rpc/`；`src/ray/pubsub/`。
