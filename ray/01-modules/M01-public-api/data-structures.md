# M01 数据结构

## 结论摘要

M01 的主要数据结构是函数描述、序列化函数、任务选项和 ObjectRef。函数本体在 Driver export，调用参数被 flatten，结果只以引用返回；真正字节存储归 M03。[已确认/推断]

| 结构 | 创建 | 生命周期 | 消费者 |
|---|---|---|---|
| `PythonFunctionDescriptor` | 首次 `_remote` | cluster/job 内提交 | CoreWorker/GCS worker lookup |
| `_pickled_function` | `pickle_dumps` | export 后供 runtime 使用 | worker/function manager |
| `task_options` | decorator/options | 单次或默认调用 | submit_task/Raylet |
| `ObjectRef` | submit_task/get_objects 返回 | 直到引用释放/对象回收 | Driver、worker、Object Manager |

## 相关文档
[README](README.md) · [接口](interfaces.md)

## 源码证据摘要
`remote_function.py:90-170,382-415,527-574`；`worker.py:2996-3030`。

## 未解决问题
ObjectRef ownership、reference counting、spill 和跨节点传输由 M03 补充。

## 下一步阅读建议
读 M03 Object Store。
