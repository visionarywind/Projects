# D01 源码执行轨迹

- 版本：HEAD `cfe4725d23`；静态轨迹，未运行。

## 结论摘要

D01 从 Python 文件进入公共导出，装饰器创建 RemoteFunction；真正调用发生在 `main`，`.remote()` 经过 `_remote` 完成连接检查、函数导出和 CoreWorker 提交；`ray.get` 再访问对象并打印值。[已确认代表 API 代码；C++ 动态跳转未完成]

## 步骤

1. **导入（脚本 1）**：`import ray` 执行 `_raylet` 导入并把 `init/get/put/remote/shutdown/wait` 导出到包命名空间。[`python/ray/__init__.py:80-130`]
2. **定义（3-5）**：`@ray.remote` 将函数包装为 `RemoteFunction`；构造保存原函数、默认 options、descriptor 和 `_remote_proxy`。[`remote_function.py:90-182`]
3. **提交（9）**：`hello_world.remote()` 进入 `_remote`；检查连接，首次在 cluster/job 中 pickle/export 函数，flatten 参数，整理 `num_returns/resources/retries/scheduling_strategy`，调用 `core_worker.submit_task`。[`remote_function.py:355-574`]
4. **调度/执行（跨模块）**：CoreWorker 将请求交给 Raylet，worker 执行函数体返回字符串；该段源码精确提交/执行符号待下一批补齐。[推断]
5. **获取（9）**：`ray.get` 将单 ref 规范化为列表，调用 `worker.get_objects`；成功返回值，错误转成 `RayTaskError` 或其他 `RayError`。[`worker.py:2881-3029`]
6. **输出（9）**：`print` 输出返回字符串；本轮未执行，输出仍标为未验证。
7. **退出（12-13）**：脚本结束；Ray runtime/driver 清理的精确 shutdown 是否隐式发生待运行验证。[未知]

## 相关文档
[数据状态](data-and-state-trace.md) · [调试](debug-walkthrough.md)

## 源码证据摘要
`hello_world.py:1-13`；`remote_function.py:355-574`；`worker.py:2881-3029`。

## 未解决问题
需补 Cython `_raylet`、CoreWorker submit、Raylet dispatch、object store materialization 和 process cleanup。

## 下一步阅读建议
用此步骤 ID 反查 M01-M04，不要把推断段写成已确认。
