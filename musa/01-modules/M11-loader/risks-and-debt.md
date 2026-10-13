# Loader 薄加载层：风险与技术债

- 文档目的：loader：风险、技术债及源码证据与修复/验证建议。
- 适用范围：M11 模块的《Loader 薄加载层：风险与技术债》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`risks-and-debt.md`](risks-and-debt.md)


以下条目均已在源码中定位到具体行号。按"影响面 × 触发概率"排序。

## 阻断级

### 1. 本快照下 loader 无法配置编译（缺头文件）

`driver_resolver.cpp:23` 包含 `"hwdefs/mtgpu_asics.h"`，而 [src/loader/CMakeLists.txt:45] 只加入 `../hal/m3d/m3d/src`。该目录下**不存在** `hwdefs/`；全树唯一的 `mtgpu_asics.h` 位于 `src/hal/m3d/m3d/src/core/imported/addrlib/src/chip/mtgpu_asics.h`，其中**不含** `CheckSum`、`QUERYADAPTERINFO_TAG`、`MT_QUERYADAPTERINFODATA`。因此 `MT_VENDOR_ID` [src/loader/driver_resolver.cpp:234] 和 `CheckSum` [src/loader/driver_resolver.cpp:228] 在当前 include 路径下无定义。

同时 [src/loader/CMakeLists.txt:16-17] 的 `../../../shared_include` 回退路径在本快照缺失，[src/loader/CMakeLists.txt:21-22] 会直接 `FATAL_ERROR`。

**影响**：本模块所有结论均为静态推导，无法编译或链接验证。
**建议**：文档层面记录 include 路径缺口；修复需改动源码或构建配置，不在本次范围。

### 2. 必须外部传入 `SHARED_INCLUDE_PATH`

[src/loader/CMakeLists.txt:14-18] 的默认值指向不存在的目录。实际调用方 `cmake_mtmusa.py:72`、`scripts/clang_tidy.py:49` 会传参，但直接 `cmake ..` 会失败。

## 高危

### 3. `musa_loader` 不链接 `util`，靠 header-only 侥幸通过

[src/loader/CMakeLists.txt:60-63] 只链接 `musa_loader_resolver` 与 `${CMAKE_DL_LIBS}`，但 [src/loader/loader.cpp:22] 和 [src/loader/driver_resolver.cpp:24] 都包含 `util/utilLibrary.h`。当前能链接仅因该头文件全为 inline。**任何一次向 `utilLibrary.h` 添加非 inline 定义都会立刻破坏 loader 链接。**

另注：[src/util/CMakeLists.txt:2] 的 `util` 是 STATIC 且未设 `POSITION_INDEPENDENT_CODE`，本就不能链进 SHARED 库。

### 4. `probe.cpp` 完全不参与构建（死代码）

[src/loader/CMakeLists.txt:74-93] 整段被注释，39 行 `probe.cpp` 不可达。`resolvedDriverPath` 字段 [src/loader/driver_resolver.h:13] 的唯一消费者就是它，另有 `fs::canonical` 计算 [src/loader/driver_resolver.cpp:356-362] 在主路径上完全不用——**主路径为无人使用的值付出了一次文件系统调用**。

### 5. 错误吞掉：非 MUSA 适配器与损坏适配器不可区分

[src/loader/driver_resolver.cpp:220] 和 [src/loader/driver_resolver.cpp:247] 在 `KMTQAITYPE_UMDRIVERPRIVATE` 查询失败或间接显示设备时 `break`，而 `result` 仍为 `MUSA_SUCCESS`。真正损坏的 MUSA 适配器会被当作"非本硬件"静默跳过，最终可能报 `MUSA_ERROR_NOT_INITIALIZED` 而非真实原因。

### 6. `SynchronizeDebuggerData` 无同步

[src/loader/loader.cpp:165-175] 对 `muDebugFlag`、`muGdbPid`、`muGdbSessionId` 做非原子读取，并写入驱动侧内存，全程无锁。两个线程并发调用 `muDebugInit` 会竞争。注释 [src/loader/loader.cpp:219] 记录了意图，但实现无保护。

## 中危

### 7. `driverStatus` 是非原子全局，每次 API 调用都读

[src/loader/loader.cpp:45] 定义，[src/loader/loader.cpp:104]、[src/loader/loader.cpp:108] 读取。当前依赖 once-init 的 happens-before 边才安全。任何把写入移到回调之外的改动都会引入数据竞争。

### 8. `muDebugInit` 特判依赖运行期指针比较

[src/loader/loader.cpp:189] 的 `if constexpr` 对 `muDebugInit`、`muDebugSendTrapInfo`、`muDebugSendRunningKernelInfo` 三者都为真（签名完全相同，[src/loader/loader_api_list.inc:566-568]），正确性完全依赖 [src/loader/loader.cpp:190] 的运行期指针比较。新增同签名调试 API 会静默改变行为。

### 9. 导出清单无一致性检查

`loader_api_list.inc` 同时驱动 570 个包装（[src/loader/loader.cpp:211-218]）和 ELF/PE 导出表（[cmake/export_symbols.cmake:22-50]）。清单漏一条即静默少一个导出符号，无编译期或运行期告警。

### 10. `PlatformThunks::Load` 折叠三种失败

[src/loader/driver_resolver.cpp:71-79] 返回单个 `bool`，把"gdi32 加载失败""符号缺失"两种原因合并，排障时无法区分。

## 低危 / 代码风格

| 问题 | 证据 |
|---|---|
| `EnumerateAdapters` 有一处设错却漏了提前退出语句 | [src/loader/driver_resolver.cpp:125-163] |
| `EnumerateAdapters` 起始处 `Reset()` 是防御性空操作 | [src/loader/driver_resolver.cpp:125-128] |
| `driverModule` 只写不读 | [src/loader/loader.cpp:44]、[src/loader/loader.cpp:78] |
| WSL `try/catch` 不可达（被调函数 `noexcept`） | [src/loader/loader.cpp:106-111]、[src/loader/loader.cpp:58] |
| 本模块无 `install()` 规则，产物拷贝依赖外部脚本 | [src/loader/CMakeLists.txt:1-93]、`README.md:132-141` |
| `MUSA_LOADER_OUTPUT_DIR` 只存在于注释和文档，运行期不读 | [src/loader/CMakeLists.txt:88]、[src/loader/CMakeLists.txt:91]、`README.md:27` |

## 未解决问题

1. 570 个包装与 `libmusa.so` 实际导出符号的 diff 未做（缺编译产物）。
2. `hwdefs/mtgpu_asics.h` 的正确 include 路径需查上游构建脚本才能确定。
3. Windows 与 WSL 两条路径的失败码一致性未验证。

## 相关文档

- [`README.md`](README.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)
- [`../module-registry.md`](../module-registry.md)

## 源码证据摘要

- `src/loader/loader.cpp`（13 处，例：[src/loader/loader.cpp:22]）
- `src/loader/CMakeLists.txt`（9 处，例：[src/loader/CMakeLists.txt:45]）
- `src/loader/driver_resolver.cpp`（9 处，例：[src/loader/driver_resolver.cpp:234]）

## 下一步阅读建议

阅读 [`risks-and-debt.md`](risks-and-debt.md)。
