# 工具与调试接口：例子

- 文档目的：tools debug：用法示例与边界/错误输入说明。
- 适用范围：M10 模块的《工具与调试接口：例子》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


## `muInfo` 使用示例

`muInfo` 是 `src/tools/CMakeLists.txt` 自动构建的工具之一：对每个 `*.cpp` 生成同名可执行文件并链接 driver 动态库 [src/tools/CMakeLists.txt:10-18]。

```bash
# 构建（未验证；需目标机）
cmake -S . -B build
cmake --build build --target muInfo

# 运行
./build/bin/muInfo
```

程序输出（源码可确定的静态部分）：

```text
compiler: mcc                          // printCompilerInfo 固定串 [src/tools/muInfo.cpp:276-278]
device#0
Name: ...
Driver Version: X.Y                    // muDriverGetVersion 解析 [src/tools/muInfo.cpp:298-300]
compute capability: X.Y
...
peers: device#1 device#2               // muDeviceCanAccessPeer 逐对 [src/tools/muInfo.cpp:344-350]
non-peers: device#3                    // [src/tools/muInfo.cpp:352-359]
memInfo.total: X GB                    // muMemGetInfo_v2 [src/tools/muInfo.cpp:364-369]
```

注意 `muInfo` 对每个设备执行 `muDevicePrimaryCtxRetain` + `muCtxSetCurrent` [src/tools/muInfo.cpp:382-383]，而 `muMemGetInfo_v2` 在 `printDeviceProp` 内不显式传入 context [src/tools/muInfo.cpp:364-369]（推断：依赖当前线程已设置的 context）。

实际设备名称、内存容量与 peer 矩阵必须等目标机运行补充。构建/运行候选命令均未在目标环境验证。更详细的逐阶段轨迹见 [`../../80-demos/D01-muinfo/README.md`](../../80-demos/D01-muinfo/README.md)。

## GDB 集成示例（源码路径）

GDB 客户端通过导出表接入驱动，两段 `dlopen` 互指：

```cpp
// 驱动侧：初始化末尾加载调试器库（platform.cpp:130）
MUresult InitDebugger() {
    if (muDebugFlag && !G_MUGDB_DRIVER_HOOKS.ready.load(std::memory_order_acquire)) {
        auto handle = Util::Library::Load("libmusadebugger.so");   // [src/driver/mugdb/tracepoints.cpp:57]
        ...
        auto muGdbInit = Util::Library::GetSymbol<...>(handle, "muGdbInit"); // [src/driver/mugdb/tracepoints.cpp:65]
        status = muGdbInit(muGdbPid, muGdbSessionId);
    }
    return status;
}

// 客户端侧：取驱动内部导出表
MUresult MusaGdb::GetDriverImportTable(void const** table) {
    Util::Library musaLib("libmusa.so");                            // [src/gdb/musaGdb.cpp:61]
    auto muGetExportTable = musaLib.GetSymbol<...>("muGetExportTable");
    const Client uuid = Client::MUgdb;
    muGetExportTable(table, reinterpret_cast<const MUuuid*>(&uuid)); // [src/gdb/musaGdb.cpp:68-69]
    return MUSA_SUCCESS;
}
```

接入时序：驱动 `InitDebugger` → 客户端 `muGdbInit` → `GetDriverImportTable` 取 `gdbTable` → `Enable(InitGdbDriverHooks)` 注入 kernel 加载/结束回调 [src/gdb/interface.cpp:38-41] → `InitGdbDriverAccessors` 绑定 accessor [src/gdb/interface.cpp:43-70] → 创建 FIFO 并 fork agent [src/gdb/musaGdb.cpp:100-129]。

## 最小代码片段：取导出表

```cpp
MUuuid uuid;
*reinterpret_cast<Client*>(&uuid) = Client::MUpti;
const void* table = nullptr;
muGetExportTable(&table, &uuid);
// 只读使用 table；不得释放（静态表族所有权见 interfaces.md）
```

这是源码阅读模型，不是已运行的示例。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/tools/muInfo.cpp`（7 处，例：[src/tools/muInfo.cpp:276-278]）
- `src/gdb/musaGdb.cpp`（3 处，例：[src/gdb/musaGdb.cpp:61]）
- `src/driver/mugdb/tracepoints.cpp`（2 处，例：[src/driver/mugdb/tracepoints.cpp:57]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
