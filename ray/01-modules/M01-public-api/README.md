# M01 公共 API 与句柄

- 文档目的：解释用户 API 如何变成远程任务/Actor/对象操作。
- 适用范围：`python/ray` 公共入口及 C++/Java API 边界；本页重点为 Python task 主线。
- 对应源码版本：HEAD `cfe4725d23`。证据状态：Python task 代表链已静态确认；动态执行未验证。
- 前置阅读：[模块注册表](../module-registry.md)。后续阅读：[实现](implementation.md)。

## 结论摘要

`ray` 包把 `_raylet` 中的 ID/ObjectRef 和 `_private.worker` 中的 `init/get/put/remote/shutdown/wait` 提升为公共 API；`@ray.remote` 产生 `RemoteFunction`，其 `.remote()` 最终调用 `worker.core_worker.submit_task` 并立即返回一个或多个 ObjectRef；`ray.get` 归一化引用后调用 `worker.get_objects`，将 `RayTaskError` 还原成 cause 抛出。[已确认：`python/ray/__init__.py:80-130`、`remote_function.py:90-182,355-574`、`worker.py:2881-3029`]

## 初学者解释

普通函数调用会在当前进程立即返回值；Ray 远程函数调用先返回“未来结果”的 ObjectRef。Ray runtime 决定在哪个 worker 执行函数；`ray.get(ref)` 阻塞等到对象在本地可用，再反序列化成 Python 值。

## 职责与非职责

- 职责：API 参数/选项、函数描述、序列化、ObjectRef、错误表面、自动初始化/Client mode hooks。
- 非职责：资源匹配算法（M04）、对象字节传输/存储（M03）、集群元数据（M05）。

## 声明到落地

```text
@ray.remote
→ worker.remote overload/option validation
→ RemoteFunction.__init__ 创建 remote proxy
→ RemoteFunction._remote 检查连接、导出函数、归一化资源/重试/调度选项
→ _raylet.pyx submit_task：序列化参数并构造 CTaskOptions
→ CoreWorker::SubmitTask：生成 TaskID、TaskSpec，TaskManager::AddPendingTask
→ NormalTaskSubmitter：解析依赖、排队、申请 worker lease、PushNormalTask RPC
→ worker 执行并写入返回对象
→ ObjectRef
→ ray.get
→ worker.get_objects
→ Python value 或 RayError
```

精确证据：`RemoteFunction.__init__` 在 90-182 行保存函数/选项并将 `remote` 指向 `_remote_proxy`；`_remote` 在 355-574 行扁平化参数并调用 `core_worker.submit_task`。Cython binding 在 `_raylet.pyx:3938-4032` 转换资源、调度、重试和 labels，并调用 `CCoreWorkerProcess.GetCoreWorker().SubmitTask`；C++ 入口在 `src/ray/core_worker/core_worker.cc:2056-2135`，随后异步投递 `NormalTaskSubmitter::SubmitTask`（`src/ray/core_worker/task_submission/normal_task_submitter.cc:33-504`）。`get` 在 2986-3029 行归一化输入、调用 `get_objects`、翻译错误并返回值。

## 关键状态与不变量

| 状态 | 不变量/作用 |
|---|---|
| `_function_descriptor` | 首次调用后由实际函数与 UUID 建立；用于 worker 查找函数 |
| `_last_export_cluster_and_job` | cluster/job 改变时重新 pickle/export，避免跨 driver 复用旧定义 |
| `_default_options` | decorator 级默认项；`.options()` 复制并覆盖，不应原地污染调用 |
| ObjectRef | 逻辑结果引用；不是 Python 值本身 |
| global worker connection | `_remote` 与 `get/put` 在副作用前必须 `check_connected()` |

## 分支与错误

- client mode 可在 368-370 行改走 client conversion。
- generator task 根据 `num_returns` 返回 `ObjectRefGenerator`。[444-465,560-569]
- placement group/strategy 在 477-516 行选择。
- `get` 对 CompiledDAGRef、列表、单一 ObjectRef 分支处理，并拒绝混合非法类型。[2962-2994]
- `RayTaskError` 通过 `as_instanceof_cause()` 抛给调用者；其他 `RayError` 直接抛出。[2999-3010]

## 深度审计

| 分析对象 | 入口落地 | 正常路径 | 分支 | 异常 | 清理 | 数据生命周期 | 执行上下文 | 行级证据 | Demo 映射 | 状态/缺口 |
|---|---|---|---|---|---|---|---|---|---|---|
| Python remote task API | 已到 `submit_task` | 已追踪 | client/generator/placement 已覆盖 | get 错误已覆盖 | worker/core 清理待 M02 | ObjectRef 代表路径已覆盖 | Driver/worker 已区分 | 已有 | D01 1-13 | 静态深化完成，动态未验证：C++/Java 与 Actor 全链待补 |

## 相关文档
[接口](interfaces.md) · [调用链](call-chains.md) · [D01](../../80-demos/D01-hello-world/README.md)

## 源码证据摘要
`python/ray/__init__.py:80-130`；`python/ray/remote_function.py:90-182,355-574`；`python/ray/_private/worker.py:2881-3029`。

## 未解决问题
`core_worker.submit_task` 的 Cython/C++ 绑定和 Actor 创建/方法调用、跨语言描述符仍需单独补充。

## 下一步阅读建议
读 implementation，再沿 M02 的 CoreWorker 提交和 M04 调度继续。
