# 启动、集群、Autoscaler、runtime env 与 Jobs：数据结构与所有权

## 核心数据
ClusterID、NodeState、resource demand、runtime env cache、JobID。

## 抽象生命周期
```text
Created → Pending/Registered → Running/Available → Completed
                         ↘ Failed/Cancelled → Cleaned
```

句柄不等于内容所有权；异步回调必须在完成、失败、取消路径归还引用。
