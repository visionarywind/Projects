# M02 源码地图、接口和数据

| 文件 | 责任 | 证据 |
|---|---|---|
| `src/cui/cuiinit.c` | globals、初始化检查、回滚 | `cuiInitCheckEx`、`cuiInitInternal` |
| `src/cui/cuictx.c` | context 参数、分配、初始化、销毁 | `cuiCtxCreateParamsInit`、`cuiCtxCreate` |
| `src/cui/cuitls.c` | TLS/current context | 由 API 和 CUI 多处调用，深层字段仍待整理 |
| `src/cui/cuierror.c` | context/设备错误状态 | sticky error 的被调用方；完整产生点未穷尽 |
| `src/cui/cuimutex.c` | 内部锁封装 | 锁顺序需要跨文件核对 |
| `src/cuda_types.h` | 对象前向声明 | `CUItlsThreadData`、`CUctx` 等 `[src/cuda_types.h:36-43]` |

## 核心字段

- `globals.initialized`、`globals.initMutex`：进程级状态和初始化串行化。
- `CUctx.persistentState.state/device/isPrimary/threadCtxStackRefCount`：context 生命周期、归属设备和 TLS 引用（静态确认：[src/cui/cuictx.c:387-405]）。
- `CUctx.apiVersion`：区分 legacy v3010 与 v3020 wrapper 语义（静态确认：[src/cui/cuiinit.c:3010-3025]）。

## 接口契约

调用者需提供已初始化 TLS；要求 context 的 API 必须接受 current context 或显式处理无 context 结果。context 创建成功后由 context manager/线程栈持有，销毁前必须完成同步和对象清理；具体 `cuiCtxDeinitialize` 释放清单需继续读取。
