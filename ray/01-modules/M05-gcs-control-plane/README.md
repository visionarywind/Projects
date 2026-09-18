# M05 GCS、RPC 与 PubSub

- 目的：说明集群控制面和跨进程协议边界。
- 版本：HEAD `cfe4725d23`；证据状态：目录、proto、BUILD 边界已确认，具体服务方法待补。
- 前置：[M04](../M04-raylet-scheduling/README.md)。后续：[M06](../M06-cluster-startup/README.md)。

## 结论摘要

GCS、gcs_rpc_client、rpc、pubsub 和 protobuf 组成 Ray 的控制面：服务发现、任务/节点/worker 元数据、RPC 契约和事件传播由此承载；它与对象字节数据面分开。[已确认目录/BUILD；具体流为推断]

```text
CoreWorker/Raylet → RPC client/protobuf → GCS service
Dashboard/State/Event → GCS/pubsub/observability
```

## 深度审计

| 分析对象 | 入口落地 | 正常路径 | 分支 | 异常 | 清理 | 数据生命周期 | 执行上下文 | 行级证据 | Demo 映射 | 状态/缺口 |
|---|---|---|---|---|---|---|---|---|---|---|
| GCS/RPC | 目录与 proto 已定位 | 未完成 | 未完成 | 未完成 | 未完成 | 未完成 | 控制面推断 | 部分 | D01 间接 | 静态深化完成，动态未验证 |

## 相关文档
[依赖地图](../../00-overview/dependency-map.md) · [接口契约](../../90-cross-module/interface-contracts.md)

## 源码证据摘要
`src/ray/gcs/`、`src/ray/gcs_rpc_client/`、`src/ray/rpc/`、`src/ray/pubsub/`、`src/ray/protobuf/`。

## 未解决问题
需补 GCS server 初始化、具体 RPC method、pubsub 生命周期、认证 token 传播和故障重连。

## 下一步阅读建议
先读 `src/ray/gcs/BUILD.bazel` 和 protobuf 定义。
