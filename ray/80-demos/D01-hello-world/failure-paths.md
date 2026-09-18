# D01 失败路径与清理

- 版本：HEAD `cfe4725d23`；静态分析，未注入故障或运行脚本。

## 失败矩阵

|阶段|失败|可见结果|静态清理边界|
|---|---|---|---|
|import/init|`_raylet` 缺失、连接失败|import/init exception|未创建任务；worker/init 负责连接失败退出|
|decorator|async function/非法 option|ValueError/validation error|不会进入 task submission|
|export|函数不可 pickle|serialization exception|函数 export 中止；缓存细节需运行日志|
|submit|资源/连接/RPC/依赖失败|submit 或后续 get 错误|CoreWorker pending task、lease client 和 Raylet failure handler 处理失败/重试|
|execute|用户函数抛异常|`ray.get` 抛还原后的 cause|task error 写入返回对象/错误状态，由 task/object 生命周期处理|
|get|timeout/object lost|GetTimeoutError/RayError|`get_objects` 结束等待或报告对象错误；引用释放时序需运行验证|
|exit|driver 异常结束|runtime/process cleanup|CoreWorker、Raylet client、ObjectRef 和子进程 cleanup 的精确时序需运行验证|

## 已确认错误转换

`ray.get` 遇到 `RayTaskError` 调用 `as_instanceof_cause()` 后抛出；ObjectLostError 会触发 object-store usage 日志后继续抛出。[`worker.py:2999-3010`]

提交链中已定位的失败边界为：`NormalTaskSubmitter` 的 dependency resolution、worker lease cancellation/rejection、runtime environment setup、placement-group removal、worker startup failure、remote/local Raylet failure。[`normal_task_submitter.cc:33-504`]

## 相关文档

[错误模型](../../00-overview/global-error-model.md) · [调试](debug-walkthrough.md) · [M02](../../01-modules/M02-core-worker/README.md)

## 源码证据摘要

`remote_function.py:107-112,409-415`；`worker.py:2937-2942,2999-3010`；`_raylet.pyx:3938-4032`；`core_worker.cc:2056-2135`；`normal_task_submitter.cc:33-504`。

## 未解决问题

重试次数、worker crash、owner death、跨节点传输和退出清理仍需专项运行测试；静态失败边界已完成。

## 下一步阅读建议

先用修改练习触发 RuntimeError，再对照 M02/M03/M04 的 failure handler 观察 driver/worker/Raylet 日志。
