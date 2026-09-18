# 公共 API 与句柄：行级分析

## 证据锚点
- `python/ray/__init__.py`、`python/ray/remote_function.py:355-574`、`python/ray/_private/worker.py:2881-3081`

## 结论
- 入口负责验证/归一化，副作用发生于：RemoteFunction 保存描述符和选项，扁平化参数后提交；get 调用 worker.get_objects 并翻译 RayTaskError。
- 分支：client mode、generator、placement group、单/列表引用和 timeout。
- 错误：序列化失败、RayTaskError、ObjectLostError、GetTimeoutError。

动态行为未运行，不写成已验证。
