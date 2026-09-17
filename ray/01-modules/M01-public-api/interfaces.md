# M01 接口契约

## 结论摘要

- `.remote()` 返回 ObjectRef、ObjectRef 列表或 ObjectRefGenerator，返回形态由 `num_returns`/generator 决定。[已确认：`remote_function.py:444-465,560-569`]
- `ray.get(ref)` 返回单值，列表输入保持顺序；timeout 可触发 GetTimeoutError，远程任务错误立即传播。[已确认：`worker.py:2881-2943`]
- `ray.put(value)` 调用 worker 的对象写入并返回 ObjectRef；store 满时保留 ObjectStoreFullError。[已确认：`worker.py:3032-3081`]

## 边界契约

| API | 输入 | 输出 | 副作用/错误 |
|---|---|---|---|
| `ray.init` | address/config/runtime env | context/info | 启动或连接运行时 |
| `ray.remote` | function/class/options | RemoteFunction/ActorClass | 定义时通常不提交 |
| `.remote` | args/kwargs | ObjectRef(s) | export function、submit task |
| `ray.get` | ref(s), timeout | Python value(s) | 阻塞、pull/deserialize、抛远端错误 |
| `ray.put` | Python value | ObjectRef | serialize/store/pin |
| `ray.shutdown` | 无 | 无 | 断开并清理 driver 状态 |

## 相关文档
[实现](implementation.md) · [M03](../M03-object-store/README.md)

## 源码证据摘要
`python/ray/__init__.py:87-130`；`remote_function.py:527-574`；`worker.py:2881-3081`。

## 未解决问题
Java/C++ API 的行为等价性和 ABI 版本边界尚未逐接口验证。

## 下一步阅读建议
读 call-chains 和 D01。
