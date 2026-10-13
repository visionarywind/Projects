# 工具与调试接口：图示

- 文档目的：tools debug：核心关系与流程图示，及节点到源码的映射。
- 适用范围：M10 模块的《工具与调试接口：图示》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


## ExportTable 分发图（三种模式）

```mermaid
flowchart TD
  A[消费方] -->|Client id| B[muGetExportTable / muapiGetExportTable]
  B --> C{ppExportTable 或 id 为空?}
  C -->|是| E1[MUSA_ERROR_INVALID_VALUE]
  C -->|否| D{switch Client}

  D -->|MUpti / Runtime / MUgdb / MUasan / Photon| M1[模式1: 返回驱动静态表指针]
  M1 --> T1[muptiDriverTable / runtimeTable / gdbTable / asanTable / photonTable]
  T1 --> U1[消费方只读使用, 不得释放]

  D -->|Driver / Tools / Injection / ToolsMuapi| M2[模式2: 拷贝进调用方缓冲区]
  M2 --> T2[FillAccessors 逐槽覆盖 AccessorHint]
  T2 --> U2[表内存由调用方所有, 生命周期自管]

  D -->|未知 client id| M3[模式3: 默认回退 muptiDriverTable]
  M3 --> U3[向后兼容临时措施, 带 FIXME]
```

证据：分发 switch [src/driver/mu_entry.cpp:2164-2304]、参数校验 [src/driver/mu_entry.cpp:2160-2161]、`FillAccessors` [src/driver/mu_entry.cpp:2146-2155]、默认回退 [src/driver/mu_entry.cpp:2300-2303]。

## `muInfo` 查询流

```mermaid
flowchart LR
  A[main] --> B[printCompilerInfo]
  B --> C[muInit]
  C --> D[muDeviceGetCount]
  D --> E[逐设备 retain primary ctx + set current]
  E --> F[GetDeviceProps 首次采集/缓存]
  E --> G[driverVersion / peers / memInfo]
  F --> H[stdout]
  G --> H
```

证据：`main` [src/tools/muInfo.cpp:372-388]、`GetDeviceProps` 缓存 [src/tools/muInfo.cpp:119-274]。

## GDB 双向接入流

```mermaid
sequenceDiagram
  participant Driver as 驱动 core
  participant Tp as mugdb tracepoints
  participant DBG as libmusadebugger.so
  participant Agent as agent 进程

  Driver->>Tp: InitDebugger (platform init 末尾)
  Tp->>Tp: dlopen libmusadebugger.so
  Tp->>DBG: muGdbInit
  DBG->>DBG: dlopen libmusa.so 取导出表
  DBG->>DBG: Enable hooks + Init accessors
  DBG-->>Agent: fork + 两路 FIFO
  Tp->>Driver: KernelLoadingProcess 回调
  Driver-->>Tp: 事件
  Tp->>DBG: KernelLoadingInternal 组包
  DBG-->>Agent: FIFO 写入 MSG_ID_KERNEL_LOADED
```

证据：`InitDebugger` [src/driver/mugdb/tracepoints.cpp:53-79]、`MusaGdb::Init` [src/gdb/musaGdb.cpp:80-136]、`KernelLoadingInternal` [src/gdb/interface.cpp:155-258]。

## 生命周期图：hook 存储

```mermaid
stateDiagram-v2
  [*] --> Inactive: 驱动加载
  Inactive --> Active: Enable*(initer) 写回调并置 ready
  Active --> Active: tracepoint 门控调用
  Active --> Inactive: Disable*() 清 ready
  Inactive --> [*]
```

状态名是跨模块抽象，不能替代源码中具体枚举；实际 `ready` 原子标志与回调表见 [`data-structures.md`](data-structures.md)。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/driver/mu_entry.cpp`（4 处，例：[src/driver/mu_entry.cpp:2164-2304]）
- `src/tools/muInfo.cpp`（2 处，例：[src/tools/muInfo.cpp:372-388]）
- `src/driver/mugdb/tracepoints.cpp`（1 处，例：[src/driver/mugdb/tracepoints.cpp:53-79]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
