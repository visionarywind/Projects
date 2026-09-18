# DAG、Workflow 与 Experimental channels：行级分析

## 证据锚点
- `python/ray/dag/dag_node.py:33-724`、`compiled_dag_node.py:813-3285`、`experimental/channel/common.py:61-688`

## 结论
- 入口负责验证/归一化，副作用发生于：DAGNode 保存节点；CompiledDAG 将 graph 编译为 channel/worker graph，Monitor 管运行期状态。
- 分支：sync/async reader、batch、backpressure、teardown、error propagation。
- 错误：节点异常、channel closed、取消、编译约束。

动态行为未运行，不写成已验证。
