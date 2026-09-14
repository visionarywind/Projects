# M02 Runtime/Context：全局状态、TLS、错误与上下文

- **职责**：建立进程级 runtime、维护线程 current context、创建/销毁 context，并把全局初始化和 sticky error 统一成 API 前置检查。
- **证据状态**：初始化检查、context 分配/销毁静态确认；TLS 深层实现和所有平台退出路径仍部分未知。
- **核心文件**：`src/cui/cuiinit.c`、`cuitls.c`、`cuictx.c`、`cuierror.c`、`cuimutex.c`。

## 状态模型

```text
globals.initialized
  ├─ TLS / current context (per thread)
  └─ CUctx
       ├─ device
       ├─ memmgr / streamManager / channelManager
       └─ persistentState + sticky error
```

`cuiInitCheckEx` 的检查顺序是“线程是否允许调用 → globals 是否 initialized → TLS current context → context 活跃/API 版本/sticky error”（静态确认：[src/cui/cuiinit.c:2910-2961,2979-3039]）。

## 生命周期

`cuiCtxAllocate` 只分配并初始化持久状态和 mutex；`cuiCtxCreate` 随后根据 MPS/primary-context 策略保留资源，再调用 `cuiCtxInitialize`。失败时回滚已经初始化的部分；`cuiCtxDestroy` 先 finalize、再 deinitialize，最后释放 primary TSG（静态确认：[src/cui/cuictx.c:294-385,387-405]）。

## 关键风险

- current context 属于线程状态，不是进程全局变量；跨线程使用 handle 不代表 context 已在目标线程成为 current。
- sticky error 会在后续 API 的初始化检查阶段出现，不能只在产生错误的异步提交点寻找返回值。
- Context lock、全局 init mutex、stream/channel 锁的完整锁序依赖大量调用点，当前未完成全量证明。
