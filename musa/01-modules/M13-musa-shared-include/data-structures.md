# 公共 API 头文件层：数据结构

- 文档目的：musa shared include：核心数据结构、字段含义与持有关系。
- 适用范围：M13 模块的《公共 API 头文件层：数据结构》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`risks-and-debt.md`](risks-and-debt.md)


## 句柄类型（全部在 `musa.h`）

| 类型 | 定义 | 证据 |
|---|---|---|
| `MUdeviceptr_v2` | 64 位平台 `unsigned long long`，否则 `unsigned int` | [src/musa_shared_include/musa.h:222-227] |
| `MUdeviceptr` | `= MUdeviceptr_v2` | [src/musa_shared_include/musa.h:227] |
| `MUdevice_v1` / `MUdevice` | `int` | [src/musa_shared_include/musa.h:229-230] |
| `MUcontext` | `struct MUctx_st*` | [src/musa_shared_include/musa.h:231] |
| `MUmodule` | `struct MUmod_st*` | [src/musa_shared_include/musa.h:232] |
| `MUfunction` | `struct MUfunc_st*` | [src/musa_shared_include/musa.h:233] |
| `MUlibrary` | `struct MUlib_st*` | [src/musa_shared_include/musa.h:234] |
| `MUkernel` | `struct MUkern_st*` | [src/musa_shared_include/musa.h:235] |
| `MUarray` | `struct MUarray_st*` | [src/musa_shared_include/musa.h:236] |
| `MUmipmappedArray` | `struct MUmipmappedArray_st*` | [src/musa_shared_include/musa.h:237] |
| `MUtexref` | `struct MUtexref_st*` | [src/musa_shared_include/musa.h:238] |
| `MUsurfref` | `struct MUsurfref_st*` | [src/musa_shared_include/musa.h:239] |
| `MUevent` | `struct MUevent_st*` | [src/musa_shared_include/musa.h:240] |
| `MUstream` | `struct MUstream_st*` | [src/musa_shared_include/musa.h:241] |
| `MUgraphicsResource` | `struct MUgraphicsResource_st*` | [src/musa_shared_include/musa.h:242] |
| `MUtexObject` | `unsigned long long` | [src/musa_shared_include/musa.h:243-244] |
| `MUsurfObject` | `unsigned long long` | [src/musa_shared_include/musa.h:245-246] |
| `MUexternalMemory` | `struct MUextMemory_st*` | [src/musa_shared_include/musa.h:247] |
| `MUexternalSemaphore` | `struct MUextSemaphore_st*` | [src/musa_shared_include/musa.h:248] |
| `MUgraph` | `struct MUgraph_st*` | [src/musa_shared_include/musa.h:249] |
| `MUgraphNode` | `struct MUgraphNode_st*` | [src/musa_shared_include/musa.h:250] |

## 枚举

| 枚举 | 证据 |
|---|---|
| `MUresult`（`MUSA_SUCCESS = 0` 起，[src/musa_shared_include/musa.h:2631]，收尾 [src/musa_shared_include/musa.h:3488]） | [src/musa_shared_include/musa.h:2631]-[src/musa_shared_include/musa.h:3488] |
| `MUdevice_attribute_enum` | [src/musa_shared_include/musa.h:769] |
| `MUdriverProcAddressQueryResult` | 配合 `muGetProcAddress`，[src/musa_shared_include/musa.h:27237] |

`driver_types.h` 中另有 driver 私有扩展，例如 `musaStream_t` [src/musa_shared_include/driver_types.h:3473] 与 `MUstreamWriteValue_flags` 相关字段 [src/musa_shared_include/driver_types.h:4629]。

## `export_table.h` 的 accessor 机制

### 哨兵与宏

```cpp
constexpr uintptr_t AccessorHint = 0xDEADBEEF;                              // export_table.h:859
#define ADDMEMBER(type, name) type name = reinterpret_cast<type>(AccessorHint)  // export_table.h:860
```

全文件共 **652 处** `ADDMEMBER`。每个 accessor 结构体末尾都带 `uintptr_t reserved = 0;` 或 `uintptr_t* reserved = nullptr;`，用于版本扩展。

`AccessorHint = 0xDEADBEEF` 是**未初始化占位值**：`ADDMEMBER` 把函数指针默认设为这个魔数，因此“未填充”与“真实地址”在类型层面无法区分，只能靠运行时比较 `AccessorHint` 判断——这是一个易错点。

### 命名空间与 8 个 `ExportTable`

| 命名空间 | `ExportTable` 位置 | 分组数 | 面向 |
|---|---|---|---|
| `Musa` | —（仅前置声明 [src/musa_shared_include/export_table.h:24-37]） | — | 核心类前置声明 |
| `MUpti` | [src/musa_shared_include/export_table.h:1151] `DriverExportTable`；[src/musa_shared_include/export_table.h:1175] `RuntimeExportTable` | 20 / 1 | profiler |
| `Runtime` | 命名空间 [src/musa_shared_include/export_table.h:1182]，表 [src/musa_shared_include/export_table.h:1191-1194] | 1 | runtime 通用 |
| `Driver` | 命名空间 [src/musa_shared_include/export_table.h:1198-1661]，表 [src/musa_shared_include/export_table.h:1640] | 约 29 组 | profiler 主接口 |
| `PhotonEx` | 命名空间 [src/musa_shared_include/export_table.h:1663-1683]，表 [src/musa_shared_include/export_table.h:1678] | 1 | 光追 |
| `MUgdb` | 命名空间 [src/musa_shared_include/export_table.h:1687-1775]，表 [src/musa_shared_include/export_table.h:1760] | 约 23 组 | 调试器 |
| `MUasan` | 命名空间 [src/musa_shared_include/export_table.h:1777-1837]，表 [src/musa_shared_include/export_table.h:1830] | 约 18 组 | sanitizer |
| `Tools` | 命名空间 [src/musa_shared_include/export_table.h:1839-1892]，表 [src/musa_shared_include/export_table.h:1882] | 约 17 组 | `muInfo` 等工具 |
| `Injection` | 命名空间 [src/musa_shared_include/export_table.h:1894-1906]，表 [src/musa_shared_include/export_table.h:1901] | 1 | 注入库加载 |
| `ToolsMuapi` | 命名空间 [src/musa_shared_include/export_table.h:1908-1952]，表 [src/musa_shared_include/export_table.h:1944] | 约 6 组 | `muapi*` 兼容层 |

注意 `MUpti::DriverExportTable` [src/musa_shared_include/export_table.h:1151-1172] 的成员带 `legacy interface starts` / `legacy interface ends` 注释 [src/musa_shared_include/export_table.h:1152]、[src/musa_shared_include/export_table.h:1164]，说明前 12 个成员被旧 profiler ABI 依赖，**不能重排**。

### `MUpti::DriverExportTable` 分组

`MUpti`、`TidInfo`、`CommandBase`、`DispatchCommand`、`MemcpyCommand`、`Device`、`Context`、`Stream`、`Function`、`Profiler`、`MemsetCommand`、`Event`、`ProfilerPfm`、`Internal`、`Graph`、`Msys`、`MemoryAtomicCommand`、`MemoryAtomicValueCommand`、`PCSampling`、`MemoryTransferCommand`，以及末尾 `uintptr_t* reserved` [src/musa_shared_include/export_table.h:1151-1172]。

`Runtime::ExportTable` 仅一个 `general` 分组。

`Driver::ExportTable` 的首批分组为 `general`、`device`、`deprecated`、`gfxInterop` [src/musa_shared_include/export_table.h:1640-1645]。

### accessor 结构体清单（前段）

| 结构体 | 位置 |
|---|---|
| `ThreadLocalInfoAccessors` | [src/musa_shared_include/export_table.h:864] |
| `CommandBaseAccessors` | [src/musa_shared_include/export_table.h:872] |
| `DispatchCommandAccessors` | [src/musa_shared_include/export_table.h:887] |
| `MemcpyCommandAccessors` | [src/musa_shared_include/export_table.h:896] |
| `MemoryTransferCommandAccessors` | [src/musa_shared_include/export_table.h:909] |
| `DeviceAccessors` | [src/musa_shared_include/export_table.h:916] |
| `ContextAccessors` | [src/musa_shared_include/export_table.h:921] |
| `StreamAccessors` | [src/musa_shared_include/export_table.h:930] |
| `FunctionAccessors` | [src/musa_shared_include/export_table.h:936] |
| `ProfilerControllers` | [src/musa_shared_include/export_table.h:944] |
| `MemsetCommandAccessors` | [src/musa_shared_include/export_table.h:961] |
| `EventAccessors` | [src/musa_shared_include/export_table.h:969] |
| `ProfilerPfmControllers` | [src/musa_shared_include/export_table.h:975] |
| `InternalAccessors` | [src/musa_shared_include/export_table.h:981] |
| `GraphAccessors` | [src/musa_shared_include/export_table.h:990] |
| `MsysAccessors` | [src/musa_shared_include/export_table.h:1023] |
| `MemoryAtomicCommandAccessors` | [src/musa_shared_include/export_table.h:1028] |
| `MemoryAtomicValueCommandAccessors` | [src/musa_shared_include/export_table.h:1041] |
| `PCSamplingControllers` | [src/musa_shared_include/export_table.h:1049] |
| `MUptiDriverHooks` | [src/musa_shared_include/export_table.h:1058] |
| `MUptiRuntimeHooks` | [src/musa_shared_include/export_table.h:1124] |
| `MUptiDriverControllers` | [src/musa_shared_include/export_table.h:1139] |
| `MUptiRuntimeControllers` | [src/musa_shared_include/export_table.h:1145] |
| `GeneralAccessors` | [src/musa_shared_include/export_table.h:1184] |
| `GeneralApiAccessors` | [src/musa_shared_include/export_table.h:1200] |
| `DeviceApiAccessors` | [src/musa_shared_include/export_table.h:1208] |
| `DeprecatedApiAccessors` | [src/musa_shared_include/export_table.h:1234] |
| `GfxInteropApiAccessors` | [src/musa_shared_include/export_table.h:1252] |
| `GraphApiAccessors` | [src/musa_shared_include/export_table.h:1270] |
| `ContextApiAccessors` | [src/musa_shared_include/export_table.h:1383] |

## 生成文件的参数结构

`generated_musa_meta.h`（657 个）与 `generated_musa_runtime_api_meta.h`（446 个）**只包含参数打包结构**，不含函数指针表。样例 [src/musa_shared_include/generated_musa_meta.h:14-36]：

```c
typedef struct muGetErrorString_params_st {
    MUresult error;
    const char **pStr;
} muGetErrorString_params;

typedef struct muInit_params_st {
    unsigned int Flags;
} muInit_params;

typedef struct muDeviceGet_params_st {
    MUdevice *device;
    int ordinal;
} muDeviceGet_params;
```

这些结构供 MUPTI/MUASAN 的参数拦截使用，字段顺序必须与函数原型严格一致。

## `Musa::` 前置声明

[src/musa_shared_include/export_table.h:24-37] 前置声明 12 个核心类：`Command`、`MemcpyCommand`、`MemoryTransferCommand`、`DispatchCommand`、`MemsetCommand`、`GraphNode`、`GraphExec`、`GraphExec2`、`GraphCommand`、`MemoryAtomicCommand`、`MemoryAtomicValueCommand`、`Memory`。

其中 `GraphExec` 与 `GraphExec2` 并存 [src/musa_shared_include/export_table.h:33]、[src/musa_shared_include/export_table.h:34]，是 graph 两代实现共存的痕迹。

## 相关文档

- [`README.md`](README.md)
- [`risks-and-debt.md`](risks-and-debt.md)
- [`../module-registry.md`](../module-registry.md)

## 源码证据摘要

- `src/musa_shared_include/export_table.h`（57 处，例：[src/musa_shared_include/export_table.h:24-37]）
- `src/musa_shared_include/musa.h`（27 处，例：[src/musa_shared_include/musa.h:222-227]）
- `src/musa_shared_include/driver_types.h`（2 处，例：[src/musa_shared_include/driver_types.h:3473]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`risks-and-debt.md`](risks-and-debt.md)。
