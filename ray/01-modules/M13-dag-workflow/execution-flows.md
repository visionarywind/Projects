# DAG、Workflow 与 Experimental channels：执行流

## 正常流
```text
入口 → 配置/参数归一化 → 内部状态创建 → DAGNode bind → execute/compile → channel writer/reader → downstream → output → 结果/事件
```

## 分支
sync/async reader、batch、backpressure、teardown、error propagation。

## 异常
节点异常、channel closed、取消、编译约束。

## 终止
CompiledDAG teardown、monitor stop、channel cleanup。
