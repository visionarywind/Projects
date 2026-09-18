# DAG、Workflow 与 Experimental channels：实现

## 入口到副作用
DAGNode 保存节点；CompiledDAG 将 graph 编译为 channel/worker graph，Monitor 管运行期状态。

## 正常
DAGNode bind → execute/compile → channel writer/reader → downstream → output

## 分支与异常
- sync/async reader、batch、backpressure、teardown、error propagation。
- 节点异常、channel closed、取消、编译约束。

## 清理
CompiledDAG teardown、monitor stop、channel cleanup。

## 数据与执行上下文
- DAGNode、ExecutableTask、ChannelContext、Reader/Writer、ObjectRef。
- driver graph、compiled tasks、monitor。
