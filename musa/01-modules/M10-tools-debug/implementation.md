# 工具与调试接口：实现说明

- 文档目的：tools debug：核心函数/流程的逐段实现解读。
- 适用范围：M10 模块的《工具与调试接口：实现说明》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


## 正常实现路径

`muInfo` 实现路径：

```text
main -> muInit -> 逐设备 primary ctx -> GetDeviceProps 缓存 -> stdout
```

- `GetDeviceProps` 用函数内 static 缓存全部设备的属性，首次调用批量发出 104 次 `checkMuErrors(...)` 包裹的设备查询，其中 99 次为 `muDeviceGetAttribute` [src/tools/muInfo.cpp:119-274]。
- `checkMuErrors` 宏把每个 API 错误立即转换为 `exit(EXIT_FAILURE)`，无恢复路径 [src/tools/muInfo.cpp:9-20]。

GDB 客户端实现路径：

```text
GetDriverImportTable(dlopen libmusa.so) -> muGetExportTable(Client::MUgdb)
  -> driverTable->MUgdb->Enable(InitGdbDriverHooks)
  -> InitGdbDriverAccessors(driverTable) 绑定 26 个 accessor
  -> FIFO + fork agent
```

[src/gdb/musaGdb.cpp:58-78]、[src/gdb/interface.cpp:38-41]、[src/gdb/interface.cpp:43-70]。

## 关键实现观察

- **工具不消费导出表**：`src/tools/CMakeLists.txt` 只把每个 `*.cpp` 构建成可执行文件并链接 driver 动态库 [src/tools/CMakeLists.txt:10-18]，而 `muInfo` 只用标准 `mu*` API，其源码中不出现导出表入口 `muGetExportTable` [src/gdb/musaGdb.cpp:58-78]（对照消费方取表路径）。
- **GDB 客户端绕过 loader**：`GetDriverImportTable` 直接 `dlopen("libmusa.so")` 取 `muGetExportTable` 符号，而非经 M11 loader 转发 [src/gdb/musaGdb.cpp:58-78]。驱动侧 `InitDebugger` 则反向 `dlopen("libmusadebugger.so")` 形成闭环 [src/driver/mugdb/tracepoints.cpp:53-79]。
- **hook 只读表、tracepoint 才调用**：驱动侧 `Enable*` 仅把回调函数指针写入静态存储并置 `ready` [src/driver/mupti/hooks.cpp:9-12]；真正调用发生在 core 侧 `tracepoints.h` 内联函数里，每个入口先检查 `CheckMUptiEnabled()`/`IsEnabled()` 且 hook 不等于 `AccessorHint` [src/driver/mupti/tracepoints.h:13-21]、[src/driver/muasan/tracepoints.h:7-14]。
- **`MUpti::Context` 作为观测生命周期载体**：`Enter*`/`Register*` 返回 `MUpti::Context*`，`Exit*/Mark*` 消耗它；`ReleaseContext` 复用 `ExitMemcpy` 释放 [src/driver/mupti/tracepoints.h:41-46]。

## 分支矩阵

| 分支 | 触发 | 结果 | 当前证据 |
|---|---|---|---|
| `muInfo` API 错误 | 任一 `mu*` 返回非 SUCCESS | `checkMuErrors` 打印后 `exit(EXIT_FAILURE)` | [src/tools/muInfo.cpp:9-20] |
| gdb 表获取失败 | `dlopen libmusa.so` 或符号缺失 | `MUSA_ERROR_NOT_INITIALIZED` | [src/gdb/musaGdb.cpp:61-63]、[src/gdb/musaGdb.cpp:66-72] |
| `muDebugInit` 时调试器未加载 | `muDebugFlag` 为假 | 静默跳过 `InitDebugger` | [src/driver/mugdb/tracepoints.cpp:17-19]、[src/driver/mugdb/tracepoints.cpp:56-76] |
| `muDebugSendTrapInfo` 符号缺失 | 调试器句柄中无 `muGdbSendMemInfo` | `MUSA_ERROR_NOT_INITIALIZED` | [src/driver/mugdb/tracepoints.cpp:21-34] |
| FIFO 创建失败 | `mkfifo`/`open` 返回 -1 | `MUSA_ERROR_NOT_INITIALIZED`，中止 Init | [src/gdb/musaGdb.cpp:104-113] |
| hook 未 ready | `ready` 为假 | 静默跳过 tracepoint 调用 | [src/driver/mupti/tracepoints.h:9-21] |

## 尚未证明

- `muInfo` 在无设备/驱动未安装时的实际行为未在目标机验证。
- GDB agent（`execl(s_AgentPath...)` [src/gdb/musaGdb.cpp:129]）的真实调试流程未在本快照展开。
- `MUpti::Context` 对象在 `Enter`/`Exit` 配对外的完整生命周期未逐条核对。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/gdb/musaGdb.cpp`（7 处，例：[src/gdb/musaGdb.cpp:58-78]）
- `src/driver/mugdb/tracepoints.cpp`（4 处，例：[src/driver/mugdb/tracepoints.cpp:53-79]）
- `src/tools/muInfo.cpp`（3 处，例：[src/tools/muInfo.cpp:119-274]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
