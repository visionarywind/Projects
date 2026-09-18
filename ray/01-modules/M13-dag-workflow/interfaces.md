# DAG、Workflow 与 Experimental channels：接口契约

## 输入
调用方须提供可验证、可序列化且符合生命周期的输入；入口证据：`python/ray/dag/dag_node.py:33-724`、`compiled_dag_node.py:813-3285`、`experimental/channel/common.py:61-688`。

## 输出与副作用
- DAGNode bind → execute/compile → channel writer/reader → downstream → output
- 核心数据：DAGNode、ExecutableTask、ChannelContext、Reader/Writer、ObjectRef。

## 失败
节点异常、channel closed、取消、编译约束。

跨模块修改需同步 binding/proto/日志/metrics/测试。
