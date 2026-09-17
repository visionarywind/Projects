# D01 失败路径与清理

- 版本：HEAD `cfe4725d23`；静态分析，未注入故障。

## 失败矩阵

| 阶段 | 失败 | 可见结果 | 清理状态 |
|---|---|---|---|
| import/init | `_raylet` 缺失、连接失败 | import/init exception | 待验证 |
| decorator | async function/非法 option | ValueError/validation error | 无任务提交 |
| export | 函数不可 pickle | serialization exception | export 中止，待确认缓存状态 |
| submit | 资源/连接/RPC 失败 | submit 或 get 错误 | Core/Raylet cleanup 待补 |
| execute | 用户函数抛异常 | `ray.get` 抛 cause 类型 | task/object error 生命周期待补 |
| get | timeout/object lost | GetTimeoutError/RayError | 引用与对象回收待补 |
| exit | driver 异常结束 | runtime/process cleanup | 未验证 |

## 已确认错误转换

`ray.get` 遇到 `RayTaskError` 调用 `as_instanceof_cause()` 后抛出；ObjectLostError 会触发 object-store usage 日志后继续抛出。[`worker.py:2999-3010`]

## 相关文档
[错误模型](../../00-overview/global-error-model.md) · [调试](debug-walkthrough.md)

## 源码证据摘要
`remote_function.py:107-112,409-415`；`worker.py:2937-2942,2999-3010`。

## 未解决问题
重试次数、worker crash、owner death 和退出清理需要专项测试/日志。

## 下一步阅读建议
先实现 RuntimeError 练习并记录 driver/worker 日志。
