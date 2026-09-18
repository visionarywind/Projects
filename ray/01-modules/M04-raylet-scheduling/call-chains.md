# Raylet 调度与资源：调用链

## 主链
```text
依赖满足 → RequestWorkerLease → 本地匹配或 spillback → worker 注册 → PushNormalTask
```

## 实现落点
NodeManager 管理 worker、资源、节点；lease managers 完成局部和集群选择。

## 失败链
```text
依赖/输入失败 → 不可调度、worker/Raylet 断连、placement group 删除。 → 重试、失败传播或清理
```
