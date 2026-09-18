# DAG、Workflow 与 Experimental channels：源码地图

|层次|证据|作用|状态|
|---|---|---|---|
|入口|`python/ray/dag/dag_node.py:33-724`、`compiled_dag_node.py:813-3285`、`experimental/channel/common.py:61-688`|公共/模块入口|已确认|
|实现|DAGNode 保存节点；CompiledDAG 将 graph 编译为 channel/worker graph，Monitor 管运行期状态。|真实状态改变|已确认|
|测试|python/ray/dag/|回归边界|已定位、未运行|

阅读顺序：入口 → 实现函数体 → 异常/清理 → 测试。
