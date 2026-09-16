# 运行时模型

## 对象层次

```text
Platform(singleton)
  └─ Device[ordinal]
      ├─ primary Context
      ├─ optional user Context / green Context
      ├─ MemoryPool / copy managers
      └─ HAL device
Context
  ├─ default/barrier/per-thread Stream
  ├─ Memory / Module / Event / Graph / Node
  └─ CriticalBase + dependency state
Stream
  ├─ engine queues/cmd pools/semaphores
  ├─ waiting/inflight/command lists
  └─ submit/wait threads
```

## 初始化与延迟初始化

`muapiInit` 触发 `Platform::Init`；Platform 创建可见 Device。Device 的若干重资源在 `GetDevice` 时通过 `LaterInit` 按 `call_once` 完成，包括 finalize、engine 映射、primary context 和 copy manager。[src/driver/mu_context.cpp:121-133] [src/musa/core/platform.cpp:84-140] [src/musa/core/device.cpp:856-1085]

## 当前上下文模型

Driver API 通过 `TlsCtxTop()` 读取线程局部当前 context；显式句柄经过 `Platform::ValidateContext` 后再转型。[src/driver/mu_context.cpp:15-29] 这意味着 API 的许多参数合法性依赖调用线程的 context 栈，而非仅依赖全局 device。

## 命令状态模型

源码至少出现 `queued`、`submitted`、`completed` 等状态；Stream 将命令放入 `m_CommandList`，异步提交线程取出并调用 `Submit`。[src/musa/core/stream.cpp:1048-1220] 完整状态转移和错误回收需结合 `command.cpp` 逐行确认。

## 生命周期边界

- Platform：进程内单例，析构时关闭 debugger、释放未释放 memory/resource descriptor、销毁 HAL platform。[src/musa/core/platform.cpp:478-541]
- Context：创建默认流和 barrier stream；`Dispose`/析构前触发资源回调。[src/musa/core/context.cpp:1875-1984]
- Stream：创建 HAL queue/cmd pool/cmd buffer/semaphore，并拥有 submit/wait 线程；析构时停止并 join 线程。[src/musa/core/stream.cpp:804-1020] [src/musa/core/stream.cpp:849-908]
