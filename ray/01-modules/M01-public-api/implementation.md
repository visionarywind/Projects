# M01 实现机制

## 结论摘要

RemoteFunction 不是“把函数直接发到某节点”，而是保存函数、默认选项和导出状态；调用时完成连接检查、函数 pickle/export、参数扁平化、runtime env/资源/重试/调度策略整理，再进入 Cython CoreWorker 的 `submit_task`。[已确认]

## 实现组件表

| 组件 | 声明/定义 | 创建者 | 核心状态 | 实际副作用 |
|---|---|---|---|---|
| `RemoteFunction` | `remote_function.py:40-585` | `ray.remote` helper | function/descriptor/options/export generation | export 函数并提交任务 |
| global worker | `_private/worker.py` | init/connect path | connection、job/cluster、core_worker | 访问 CoreWorker/对象 |
| ObjectRef | `_raylet` extension，`__init__.py:87-104` 导出 | submit/put | object ID/ownership metadata | 表示结果并参与引用生命周期 |
| `get` | `worker.py:2881-3029` | 公共 API | refs、timeout | 阻塞获取并抛出远端错误 |

## 算法伪代码

```text
submit(f,args,opts):
  ensure connected
  if first use in cluster/job: descriptor=f+uuid; pickle(f); export(f)
  merge defaults; normalize retries/returns/resources/placement
  flattened = flatten_signature(args, kwargs)
  refs = core_worker.submit_task(descriptor, flattened, opts...)
  return ref | refs | generator
```

## 资源与线程

`_inject_lock` 只保护 tracing/signature 的一次注入，不是远程任务执行锁；实际执行发生在 worker 进程。函数 pickle 和 export 属于 Driver 侧，ObjectRef 结果生命周期跨 M01/M02/M03。

## 正常/异常路径

| 阶段 | 正常 | 异常 |
|---|---|---|
| 定义 | 保存函数与选项 | async function 直接拒绝（107-112） |
| export | pickle 并登记 cluster/job | pickle 失败抛序列化错误（409-415） |
| submit | 返回 ObjectRef | 未连接/选项非法/调度后续失败 |
| get | 返回 Python value | timeout、RayTaskError、ObjectLostError |

## 相关文档
[README](README.md) · [调用链](call-chains.md)

## 源码证据摘要
`remote_function.py:90-182,355-574`；`worker.py:2881-3081`。

## 未解决问题
Cython `_raylet` 到 C++ `CoreWorker::SubmitTask` 的精确签名和引用所有权需后续补证。

## 下一步阅读建议
进入 M02 CoreWorker。
