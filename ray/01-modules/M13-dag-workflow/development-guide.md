# DAG、Workflow 与 Experimental channels：开发指南

1. 修改前阅读 ``python/ray/dag/dag_node.py:33-724`、`compiled_dag_node.py:813-3285`、`experimental/channel/common.py:61-688``，确认状态所有权、线程/actor 上下文和失败传播。
2. 修改中保持正常、重试、取消与 cleanup 对称；参数变化同步 binding/proto/日志/metrics。
3. 修改后运行模块最小测试 target；本次未运行。

不要把历史命令或静态文档当动态成功证据。
