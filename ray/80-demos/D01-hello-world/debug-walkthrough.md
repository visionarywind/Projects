# D01 调试教程

- 版本：HEAD `cfe4725d23`；调试点为源码建议，未执行。

## 结论摘要

最有价值的断点是 RemoteFunction `_remote`、`worker.core_worker.submit_task`、`worker.get_objects` 和 `ray.get` 的错误翻译处；这些点能分别观察提交、跨进程边界、对象获取和错误表面。

## 断点与观察

1. `python/ray/remote_function.py:384-415`：观察首次 tracing、descriptor、pickle/export。
2. `remote_function.py:527-574`：观察扁平参数、资源/重试选项和返回 ref。
3. `python/ray/_private/worker.py:2996-3010`：观察对象获取和 `RayTaskError`/`ObjectLostError`。
4. `src/ray/core_worker/core_worker_process.cc:231-285`：观察 worker 初始化和 Raylet client（需 C++ build/debug symbols）。
5. 打开 Ray worker/driver 日志并记录 worker PID、task ID、ObjectRef、node ID；未运行时不填具体值。

## 故障练习

- 将函数体改为 `raise RuntimeError("demo")`，观察 `ray.get` 异常（未执行）。
- 传入不可 pickle 对象，观察 export/serialization 错误（未执行）。
- 传 `timeout=0` 给 `ray.get`，观察 timeout 行为（未执行）。

## 相关文档
[失败路径](failure-paths.md) · [修改练习](modification-exercises.md)

## 源码证据摘要
`remote_function.py:384-415,527-574`；`worker.py:2996-3010`。

## 未解决问题
需要真实日志字段和动态调用栈，不能用静态推断替代。

## 下一步阅读建议
在具备环境后先运行正常路径，再运行上述三个故障练习。
