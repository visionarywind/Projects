# M04 Raylet 调度与资源

- 版本：HEAD `cfe4725d23`；已完成代表实现链静态分析；动态执行与测试未验证。

## 结论摘要

Raylet 不是抽象的“调度器名称”，而是由 NodeManager、WorkerPool、资源管理器和 local/cluster lease manager 协作完成节点内匹配、集群 spillback、worker 生命周期和任务派发。任务从 CoreWorker 的 `NormalTaskSubmitter` 进入 lease 请求，Raylet grant 后才进入 worker push。[已确认代表源码]

## 代表链

```text
NormalTaskSubmitter::RequestNewWorkerIfNeeded
→ lease policy 选择目标节点
→ NodeManager/ClusterLeaseManager RequestWorkerLease
→ local resource/worker availability match
→ WorkerPool grant/register
→ PushNormalTask
→ task execution
```

- NodeManager 构造和启动：`src/ray/raylet/node_manager.cc:199-373`
- worker/node/job 失败处理：`node_manager.cc:575-609` 及相邻 handler
- worker availability：`node_manager.cc:1415+`
- 集群/本地租约：`src/ray/raylet/scheduling/local_lease_manager.cc`、`cluster_lease_manager.cc`

## 状态与所有权

| 状态 | 维护者 | 作用 |
|---|---|---|
| ResourceSet/ResourceRequest | local resource manager | 可用与已占用资源 |
| Worker/WorkerLease | NodeManager/WorkerPool | worker 分配和回收 |
| NodeID/heartbeat state | NodeManager/GCS | 节点存活和集群视图 |
| scheduling class | CoreWorker/Raylet | 任务队列和 worker 复用键 |

Raylet 只负责调度/节点侧控制；任务返回对象的内容由 Object Manager/Object Store 管理，GCS 保存控制面记录。

## 分支与错误

- 本地资源可满足时优先本地 lease；否则按 locality 和集群策略 spillback/redirect。
- placement group、label selector、资源不足和 worker lease 重用改变选择路径。
- worker 启动失败、Raylet 断连、节点删除和 placement group 删除会使 lease/task 进入失败、重试或清理路径。
- 具体重试次数和最终错误由 CoreWorker task manager 与上层 options 共同决定，未做动态验证。

## 深度审计

|对象|入口落地|正常|分支|异常|清理|数据|上下文|证据|Demo|状态|
|---|---|---|---|---|---|---|---|---|---|---|
|Raylet scheduling|NodeManager/lease managers|lease→worker→push|local/cluster/spillback/placement|worker/node/unschedulable|DestroyWorker/node removal|resources/lease/node state|Raylet RPC+调度循环|代表文件与符号已列|D01/D02 间接|动态未验证|

## 相关文档

[实现](implementation.md) · [调用链](call-chains.md) · [M02](../M02-core-worker/README.md) · [M05](../M05-gcs-control-plane/README.md)

## 未解决问题

跨节点真实 spillback、资源竞争、公平性、Raylet 故障和压力性能需运行测试；当前静态证据已覆盖代表控制路径。

## 下一步阅读建议

先读 M02 `NormalTaskSubmitter`，再对照 NodeManager 和两个 lease manager，最后看对象传输的 M03。

## 源码证据摘要

`src/ray/raylet/node_manager.cc:199-373,575-609,1415+`；`src/ray/raylet/scheduling/local_lease_manager.cc`；`cluster_lease_manager.cc`。
