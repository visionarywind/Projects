# 公共 API 与句柄：开发指南

1. 修改前阅读 ``python/ray/__init__.py`、`python/ray/remote_function.py:355-574`、`python/ray/_private/worker.py:2881-3081``，确认状态所有权、线程/actor 上下文和失败传播。
2. 修改中保持正常、重试、取消与 cleanup 对称；参数变化同步 binding/proto/日志/metrics。
3. 修改后运行模块最小测试 target；本次未运行。

不要把历史命令或静态文档当动态成功证据。
