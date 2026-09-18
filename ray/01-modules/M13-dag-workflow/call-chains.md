# DAG、Workflow 与 Experimental channels：调用链

## 主链
```text
DAGNode bind → execute/compile → channel writer/reader → downstream → output
```

## 实现落点
DAGNode 保存节点；CompiledDAG 将 graph 编译为 channel/worker graph，Monitor 管运行期状态。

## 失败链
```text
依赖/输入失败 → 节点异常、channel closed、取消、编译约束。 → 重试、失败传播或清理
```
