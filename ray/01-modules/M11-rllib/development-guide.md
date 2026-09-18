# RLlib：开发指南

1. 修改前阅读 ``rllib/algorithms/algorithm.py:211-4863`、`algorithm_config.py:109-6238`、EnvRunnerGroup/Learner`，确认状态所有权、线程/actor 上下文和失败传播。
2. 修改中保持正常、重试、取消与 cleanup 对称；参数变化同步 binding/proto/日志/metrics。
3. 修改后运行模块最小测试 target；本次未运行。

不要把历史命令或静态文档当动态成功证据。
