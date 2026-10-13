# Driver API 导出层：例子

- 文档目的：driver api：用法示例与边界/错误输入说明。
- 适用范围：M02 模块的《Driver API 导出层：例子》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


以下例子是从源码走读出的调用序列，不是已运行的示例。可用 D01 `muInfo` 验证查询类路径，用 D02/D04 验证异步/Graph 路径。

## 例 1：Context 上分配/拷贝/释放

```text
muCtxCreate(&ctx, flags, dev)
  -> muapiCtxCreate_v2                    [src/driver/mu_context.cpp:156-182]
     -> pDevice->CreateContext -> TlsCtxPush -> LockedAddCtx

muMemAlloc_v2(&dptr, bytes)
  -> muapiMemAlloc_v2                     [src/driver/mu_memory.cpp:271-304]
     -> TlsCtxTop() 取当前 ctx
     -> pContext->CreateMemory(memoryTypeGeneral)
     -> *dptr = pMemory->GetDevicePointer()

muMemcpyHtoD_v2(dptr, host, bytes)
  -> muapiMemcpyHtoD_v2                   [src/driver/mu_memory.cpp:845-855]
     -> GetMemcpy3DFrom1D(...) + GeneralMemcpy(TlsCtxTop(), nullptr, copy3D, true)

muMemFree_v2(dptr)
  -> muapiMemFree_v2                      [src/driver/mu_memory.cpp:722-786]
     -> Synchronize() -> pMemory->GetContext()->DestroyMemory(pMemory)
```

## 例 2：`muGetExportTable` 拿 Driver 导出表

```text
muGetExportTable(&tbl, &client_uuid)      [src/driver/mu_wrappers_generated.cpp:3819-3857]
  -> muapiGetExportTable(&tbl, &id)       [src/driver/mu_entry.cpp:1255-2308]
     -> switch(*Client*(id))
     -> Client::Driver:
        FillAccessors(general, &GeneralApiAccessors, stop)  [src/driver/mu_entry.cpp:2172-2175]
        ... 17 个分组逐组回填 ...
        -> tbl->general->muInit 等指向 muInit/muDriverGetVersion/muGetProcAddress_v2
```

调用方需要预先把 `Driver::ExportTable` 槽位填成 `AccessorHint`，`FillAccessors` 才会回填；直接读 `tbl->graph` 前必须先让它指向本进程的 `Driver::GraphApiAccessors` 副本。

## 例 3：带优先级 stream

```text
muStreamCreateWithPriority(&s, MU_STREAM_NON_BLOCKING, prio)
  -> muapiStreamCreateWithPriority        [src/driver/mu_stream.cpp:21-42]
     -> flags 非 DEFAULT/NON_BLOCKING -> MUSA_ERROR_INVALID_VALUE
     -> pContext->CreateStream(&pStream, flags, priority)
     -> *phStream = pStream
muStreamCreate 等价于 priority=0 的 muapiStreamCreateWithPriority。[src/driver/mu_stream.cpp:17-19]
```

## 例 4：profiler 侧访问 command 计时（观测层示例）

```text
MUpti 持 AccessorHint 槽位 -> 调 muGetExportTable 拿 MUpti 表
  -> commandBaseAccessors.GetQueuedTimestamp   = CommandGetQueuedTimestamp
  -> CommandGetQueuedTimestamp(command)        [src/driver/mu_entry.cpp:157-159]
     -> command->GetQueuedTimestamp()
```

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/driver/mu_memory.cpp`（3 处，例：[src/driver/mu_memory.cpp:271-304]）
- `src/driver/mu_entry.cpp`（3 处，例：[src/driver/mu_entry.cpp:1255-2308]）
- `src/driver/mu_stream.cpp`（2 处，例：[src/driver/mu_stream.cpp:21-42]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
