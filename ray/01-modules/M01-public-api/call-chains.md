# M01 调用链

## 远程函数

```text
hello_world.remote()
→ RemoteFunction.remote proxy [remote_function.py:172-182]
→ RemoteFunction._remote [355-574]
→ global_worker.core_worker.submit_task [537-556]
→ M02 Cython/C++ CoreWorker（待精确绑定证据）
→ M04 Raylet 调度
→ Worker 执行 hello_world
→ M03 保存结果对象
```

## 获取结果

```text
ray.get(ref)
→ worker.get [2881-3029]
→ worker.get_objects [2996-2998]
→ CoreWorker/Object Manager
→ deserialize value 或 RayError
→ RayTaskError.as_instanceof_cause / return value
```

## 初始化

```text
ray.init
→ worker.init [1439-1505+]
→ 地址解析：显式地址/RAY_ADDRESS/本地发现
→ 连接已有集群或启动本地 runtime
→ CoreWorkerProcess 初始化 [core_worker_process.cc:231-285]
```

## 相关文档
[README](README.md) · [跨模块调用链](../../90-cross-module/cross-module-call-chains.md)

## 源码证据摘要
见各链中行号。

## 未解决问题
所有带“待精确绑定证据”的跳转都不能算完整调用链，下一批需读取 `_raylet.pyx` 和 C++ submitter。

## 下一步阅读建议
进入 M02 与 D01 execution trace。
