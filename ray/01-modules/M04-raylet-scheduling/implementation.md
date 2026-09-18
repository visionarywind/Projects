# Raylet 调度与资源：实现

## 入口到副作用
NodeManager 管理 worker、资源、节点；lease managers 完成局部和集群选择。

## 正常
依赖满足 → RequestWorkerLease → 本地匹配或 spillback → worker 注册 → PushNormalTask

## 分支与异常
- 资源不足、placement group、labels、租约重用、spillback、节点变化。
- 不可调度、worker/Raylet 断连、placement group 删除。

## 清理
DestroyWorker、disconnect/failure、node removal 释放资源。

## 数据与执行上下文
- ResourceRequest、WorkerLease、NodeID、scheduling class。
- 每个 Raylet 管理本地资源，RPC 与调度循环并发。
