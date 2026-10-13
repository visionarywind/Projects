# Driver API 导出层：实现说明

- 文档目的：driver api：核心函数/流程的逐段实现解读。
- 适用范围：M02 模块的《Driver API 导出层：实现说明》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


## 正常实现路径

```text
外部 C ABI（mu*）-> 包装/观测 -> muapi* 入口 -> Core 对象 -> MUresult
```

- 包装层：每个 `mu*` 符号构造 `ApiTrace`，在 tools 回调启用时通过 `toolsIssueCallback` 发 ENTER/EXIT，否则直通 `muapi*`。[src/driver/mu_wrappers_generated.cpp:3819-3857]
- 入口层：先 `InitPlatform()`，再做参数检查与句柄转换，最后调用 Core 方法并返回 `MUresult`。[src/driver/internal.h:307-321] [src/driver/mu_context.cpp:121-133]

## 关键实现观察

- 入口与内部对象之间存在显式转换/查找：`Musa::IntrusiveCast<Musa::Context>(ctx)` 与 `Musa::ICast<Musa::IEvent>(hEvent)` 取代裸指针解引用；`MUdevice` 走序号 `GetIDeviceView(ordinal)`。[src/driver/mu_context.cpp:24-28] [src/driver/mu_device.cpp:14-37]
- 失败统一沿 `MUresult` 向上传递；HAL/底层错误在 Core 边界转换成 MUSA 错误码，Driver 层只透传。
- 检查顺序有固定模式：`flags`/枚举合法性 -> 输出指针非空 -> `TlsCtxTop()` 非空 -> 对象校验 -> 调 Core。`muapiStreamCreateWithPriority` 是典型样例。[src/driver/mu_stream.cpp:21-42]
- 资源登记、追踪和析构是实现正确性的组成部分：`LockedAddCtx`/`LockedRemoveCtx`、`DestroyModule` 回滚、`delete pMemoryPool`、detached stream 清理均只在对应路径出现。[src/driver/mu_mempool.cpp:108-117] [src/driver/mu_stream.cpp:177-194]

## 分支矩阵

| 分支 | 触发 | 结果 | 当前证据 |
|---|---|---|---|
| 参数 NULL/越界 | `dptr`/`pi`/输出指针为 NULL | `MUSA_ERROR_INVALID_VALUE` | 已确认，[src/driver/mu_memory.cpp:271-280] |
| 无当前 context | `TlsCtxTop()` 为 NULL | `MUSA_ERROR_INVALID_CONTEXT` | 已确认，[src/driver/mu_stream.cpp:25-31] |
| 无效 handle/对象 | `ValidateContext` 等校验失败 | `MUSA_ERROR_INVALID_CONTEXT`/`MUSA_ERROR_INVALID_HANDLE`/`MUSA_ERROR_CONTEXT_IS_DESTROYED` | 已确认，[src/driver/mu_context.cpp:195-198] |
| flags 非法 | `muapiInit` flags!=0；stream flags 非 DEFAULT/NON_BLOCKING | `MUSA_ERROR_INVALID_VALUE` | 已确认，[src/driver/mu_context.cpp:126-127] |
| 平台未初始化 | `Musa::GetPlatform` 失败 | `MUSA_ERROR_NOT_INITIALIZED` | 已确认，[src/driver/internal.h:307-321] |
| 不支持的能力 | ctx/launch attribute 白名单外 | `MUSA_ERROR_NOT_SUPPORTED` | 已确认，[src/driver/mu_module.cpp:298-311] |
| 文件不存在 | `muapiModuleLoad` 打不开文件 | `MUSA_ERROR_FILE_NOT_FOUND` | 已确认，[src/driver/mu_module.cpp:96-99] |
| 加载失败回滚 | `LoadFatBinary` 失败 | `DestroyModule` 回滚，不暴露模块 | 已确认，[src/driver/mu_module.cpp:134-141] |
| detached stream | 销毁 detached stream | `MUSA_ERROR_STREAM_DETACHED`（[src/driver/mu_stream.cpp:54]）或 delete 路径 | 已确认，[src/driver/mu_stream.cpp:177-194] |
| 异步/批量 | stream 上 alloc/free/memcpy | 入队后返回，blocking 取 settings | 已确认于 Memory/Stream |

## 尚未证明

该模块是否在所有错误分支都释放中间对象、是否所有底层错误都有稳定映射，必须结合完整函数体和目标机测试确认。特别地：`muapiCtxGetCacheConfig` 恒返回 `MU_FUNC_CACHE_PREFER_NONE`、`muapiCtxGetSharedMemConfig` 恒返回 `MU_SHARED_MEM_CONFIG_DEFAULT_BANK_SIZE`，语义等价性未在硬件验证。[src/driver/mu_context.cpp:355-372] [src/driver/mu_context.cpp:431-449]

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/driver/mu_context.cpp`（6 处，例：[src/driver/mu_context.cpp:121-133]）
- `src/driver/mu_stream.cpp`（5 处，例：[src/driver/mu_stream.cpp:21-42]）
- `src/driver/mu_module.cpp`（3 处，例：[src/driver/mu_module.cpp:298-311]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
