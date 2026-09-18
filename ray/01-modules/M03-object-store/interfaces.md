# Object Store 与 Object Manager：接口契约

## 输入
调用方须提供可验证、可序列化且符合生命周期的输入；入口证据：`src/ray/object_manager/object_manager.cc:66-718`、`python/ray/_private/worker.py:3032-3081`。

## 输出与副作用
- put/get → ObjectManager Pull/Push → Plasma/filesystem → chunk receive → available/free
- 核心数据：ObjectID、buffer、chunk、spill URI、ownership。

## 失败
对象丢失、传输失败、ObjectStoreFull、owner/node failure。

跨模块修改需同步 binding/proto/日志/metrics/测试。
