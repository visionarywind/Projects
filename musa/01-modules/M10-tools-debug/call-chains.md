# 工具与调试接口：调用链

- 文档目的：tools debug：关键调用链：从公共入口到真实副作用的追踪。
- 适用范围：M10 模块的《工具与调试接口：调用链》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


## 主链：`muInfo` 查询路径

```text
muInfo main                                src/tools/muInfo.cpp:372-388
  -> printCompilerInfo                      src/tools/muInfo.cpp:276-278
  -> muInit(0)                              src/tools/muInfo.cpp:375
     -> [M11] loader 转发                    src/loader/loader.cpp:212
        -> MiniLoader::Forward              src/loader/loader.cpp:180
           -> driverFunctions.muInit         src/loader/loader.cpp:195
     -> [M02] muapiInit                     src/driver/mu_context.cpp:121-133
        -> Musa::Platform::Get().Init()      src/musa/core/platform.cpp:84-140
  -> muDeviceGetCount                       src/tools/muInfo.cpp:377-378
  -> for each device
     -> muDevicePrimaryCtxRetain            src/tools/muInfo.cpp:341
     -> muCtxSetCurrent                     src/tools/muInfo.cpp:345
     -> printDeviceProp                     src/tools/muInfo.cpp:283-370
        -> GetDeviceProps (static 缓存)     src/tools/muInfo.cpp:119-274
        -> muDriverGetVersion               src/tools/muInfo.cpp:298
        -> muDeviceCanAccessPeer 逐对       src/tools/muInfo.cpp:346-359
        -> muMemGetInfo_v2                  src/tools/muInfo.cpp:364
```

完整逐跳轨迹见 [`../../80-demos/D01-muinfo/execution-trace.md`](../../80-demos/D01-muinfo/execution-trace.md)。

## 主链：ExportTable 获取与填充

这是 M10 各类消费方（profiler / gdb / asan / 工具）取内部状态的统一入口。全部实现在 `src/driver/mu_entry.cpp`。

### 链 A：`MUgdb`（直接 `dlopen`，绕过 loader）

```
MusaGdb::GetDriverImportTable            src/gdb/musaGdb.cpp:58
  -> Util::Library musaLib("libmusa.so") src/gdb/musaGdb.cpp:60
  -> musaLib.GetSymbol<...>("muGetExportTable")  src/gdb/musaGdb.cpp:66
  -> muGetExportTable(table, &uuid)      src/gdb/musaGdb.cpp:69
     uuid = Client::MUgdb                src/gdb/musaGdb.cpp:68
     -> [M02 包装] muGetExportTable       src/driver/mu_wrappers_generated.cpp:3819
        -> muapiGetExportTable           src/driver/mu_entry.cpp:1255
           -> switch (*(const Client*)pExportTableId)   src/driver/mu_entry.cpp:2164
              case Client::MUgdb:        src/driver/mu_entry.cpp:2241
                 *ppExportTable = &gdbTable              src/driver/mu_entry.cpp:2242
```

注意 GDB 客户端**自己 `dlopen("libmusa.so")`**，不经 M11 loader——它需要拿到驱动内部符号而非转发包装。

### 链 B：`MUpti` / `Runtime` / `MUasan` / `Photon`（返回静态表指针）

```
muapiGetExportTable                     src/driver/mu_entry.cpp:1255
  -> case Client::MUpti                  src/driver/mu_entry.cpp:2165
     *ppExportTable = &muptiDriverTable  src/driver/mu_entry.cpp:2166
        (static 定义于 src/driver/mu_entry.cpp:1471-1494，21 个分组)
  -> case Client::Runtime                src/driver/mu_entry.cpp:2168
     *ppExportTable = &runtimeTable      src/driver/mu_entry.cpp:2169
        (static 定义于 src/driver/mu_entry.cpp:1503-1506)
  -> case Client::MUasan                 src/driver/mu_entry.cpp:2244
  -> case Client::Photon                 src/driver/mu_entry.cpp:2247
```

这一族**不需要调用方分配缓冲区**——驱动直接交出静态表指针，其中每个分组的成员在函数首次进入时就已用真实函数地址初始化。

### 链 C：`Driver` / `Tools` / `Injection` / `ToolsMuapi`（拷贝进调用方缓冲区）

```
muapiGetExportTable                     src/driver/mu_entry.cpp:1255
  -> case Client::Driver                 src/driver/mu_entry.cpp:2171
     -> FillAccessors(dst->general,  &GeneralApiAccessors,  stop)   src/driver/mu_entry.cpp:2172
     -> FillAccessors(dst->device,   &DeviceApiAccessors,   stop)   src/driver/mu_entry.cpp:2176
     -> FillAccessors(dst->deprecated, ...)                          src/driver/mu_entry.cpp:2180
     -> ... 逐分组，共约 20 个
  -> case Client::Tools                  src/driver/mu_entry.cpp:2250
     -> callback / context / stream / greenCtx / internal / module   src/driver/mu_entry.cpp:2251-2273
  -> case Client::Injection              src/driver/mu_entry.cpp:2276
  -> case Client::ToolsMuapi             src/driver/mu_entry.cpp:2282
     -> general / context / module / virtualMemory                  src/driver/mu_entry.cpp:2283-2297
```

`FillAccessors` 本体 [src/driver/mu_entry.cpp:2146-2155]：

```cpp
auto FillAccessors = [](uintptr_t* pDst, uintptr_t* pSrc, bool& stop) {
    if (pDst == nullptr || pSrc == nullptr) {
        stop = true;
    }
    while (!stop && *pDst == AccessorHint && *pSrc != 0) {
        *pDst = *pSrc;
        pDst++;
        pSrc++;
    }
};
```

调用方先声明一个 `ExportTable`（成员因 `ADDMEMBER` 全部为 `AccessorHint`），再把地址交给驱动；驱动逐槽覆盖真实地址。**调用方必须自己持有这张表的内存**，生命周期不受驱动管理。

## 默认分支：向后兼容

```
muapiGetExportTable -> switch 无匹配      src/driver/mu_entry.cpp:2300
  -> *ppExportTable = &muptiDriverTable    src/driver/mu_entry.cpp:2302
```

同一行注释明确这是为当前 MUPTI 版本保持向后兼容的临时措施，并标注了 `FIXME(Zhangjian.Zhao)`：待 MUPTI 升级后应改为返回 `MUSA_ERROR_INVALID_VALUE`（该错误码定义在 [src/musa_shared_include/musa.h:2631]-[src/musa_shared_include/musa.h:3488]）。当前**任何未知 client id 都会静默拿到 MUPTI 表**。

## 参数校验

`ppExportTable` 或 `pExportTableId` 为空时返回 `MUSA_ERROR_INVALID_VALUE`，不进入 switch [src/driver/mu_entry.cpp:2160-2161]。

## `muGetExportTable` 的双路径包装

`mu_wrappers_generated.cpp` 为该函数生成两条路径 [src/driver/mu_wrappers_generated.cpp:3819]：

- 经参数结构体： [src/driver/mu_wrappers_generated.cpp:3846]
- 直调： [src/driver/mu_wrappers_generated.cpp:3853]

前者供 MUPTI/MUASAN 拦截参数使用，后者是常规路径。

## 链路缺口

1. `Client::Tools` / `ToolsMuapi` / `Injection` 的实际调用方不在本快照——`src/tools/CMakeLists.txt` 只构建 `muInfo`，而 `muInfo` 不使用导出表。
2. `FillAccessors` 的源端 accessor 列表（如 `GeneralApiAccessors` [src/driver/mu_entry.cpp:1510]、[src/driver/mu_entry.cpp:1514]）是否含 0 空洞未逐项核对，而空洞会导致后续全部静默不填。
3. 进入 `src/hal/m3d/m3d` 私有子模块后的链路仍未展开。

## 相关证据

[src/tools/CMakeLists.txt:10-18] [src/tools/muInfo.cpp:120-388] [src/driver/mu_entry.cpp:1255-2307] [src/gdb/musaGdb.cpp:58-78] [src/musa_shared_include/export_table.h:1954-1964]

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/driver/mu_entry.cpp`（5 处，例：[src/driver/mu_entry.cpp:2146-2155]）
- `src/driver/mu_wrappers_generated.cpp`（3 处，例：[src/driver/mu_wrappers_generated.cpp:3819]）
- `src/musa_shared_include/musa.h`（2 处，例：[src/musa_shared_include/musa.h:2631]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
