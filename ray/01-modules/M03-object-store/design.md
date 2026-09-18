# Object Store 与 Object Manager：设计与边界

- 源码锚点：`src/ray/object_manager/object_manager.cc:66-718`、`python/ray/_private/worker.py:3032-3081`
- 证据：静态源码已确认；真实运行未验证。

## 目标与边界
入口 ``src/ray/object_manager/object_manager.cc:66-718`、`python/ray/_private/worker.py:3032-3081`` 通过内部状态达到真实副作用，不把 API wrapper 当作实现终点。

## 设计不变量
1. 控制元数据、数据内容和资源所有权分开。
2. 异步结果的完成、失败、取消均可观察。
3. cleanup 覆盖正常、取消和依赖进程失联。

## 上下文
ObjectManager RPC、IO 与 Plasma 回调并发。
