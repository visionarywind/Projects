# Util 基础库：接口

- 文档目的：util：接口签名、前置条件、错误码与跨层契约。
- 适用范围：M12 模块的《Util 基础库：接口》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`risks-and-debt.md`](risks-and-debt.md)


## 构建目标

`src/util/CMakeLists.txt` 全文仅 4 行：

| 行 | 内容 | 后果 |
|---|---|---|
| [src/util/CMakeLists.txt:1] | `aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} UTIL_SRC)` | 目录内**任何** `.cpp` 会被静默编译；新增文件需重跑 CMake |
| [src/util/CMakeLists.txt:2] | `add_library(util STATIC ${UTIL_SRC})` | STATIC，且未设置位置无关代码属性 |
| [src/util/CMakeLists.txt:3] | `target_include_directories(util PRIVATE ...)` | PRIVATE，不向消费方传播头文件路径 |
| [src/util/CMakeLists.txt:4] | `target_link_libraries(util PRIVATE $<$<BOOL:${UNIX}>:dl>)` | 仅链 `dl`，未链接线程库 |

当前 `UTIL_SRC` 解析为 `perfStatistics.cpp` 与 `utils.cpp` 两个文件。

无 `install()` 规则。

## 出线定义（需链接）

| 签名 | 声明 | 定义 |
|---|---|---|
| `void SteadyClock::SetStartTime()` | [src/util/utils.h:37] | [src/util/utils.cpp:9-11] |
| `uint64_t CalDurationUs(const Clock&, const Clock&)` | [src/util/utils.h:127] | [src/util/utils.cpp:13-17] |
| `TimeStatistics::TimeStatistics(const char* = "defaultName")` | [src/util/perfStatistics.h:11] | [src/util/perfStatistics.cpp:7-10] |
| `void TimeStatistics::Start()` | [src/util/perfStatistics.h:15] | [src/util/perfStatistics.cpp:12-15] |
| `void TimeStatistics::End()` | [src/util/perfStatistics.h:17] | [src/util/perfStatistics.cpp:17-22] |
| `void TimeStatistics::PrintStatistics() const` | [src/util/perfStatistics.h:19] | [src/util/perfStatistics.cpp:24-33] |
| `static thread_local std::chrono::high_resolution_clock::time_point TimeStatistics::m_threadLocalStartTime` | [src/util/perfStatistics.h:26] | [src/util/perfStatistics.cpp:5] |

## 头文件设施速查

| 头文件 | 主要类型/函数 | 位置 |
|---|---|---|
| `utilLog.h` | `Util::ThreadInfo` | [src/util/utilLog.h:26-55] |
| | `Util::Log::Flag`（12 位位掩码枚举） | [src/util/utilLog.h:59-72] |
| | `Util::Log::Setting::Get()`（读 `MUSA_LOG`） | [src/util/utilLog.h:92-101] |
| | `Util::Log::Contains()` | [src/util/utilLog.h:103-105] |
| | `Util::Log::GetSystemTime<Duration>()` | [src/util/utilLog.h:108-134] |
| | `Util::Log::ParseCoreDumpTemplate()` | [src/util/utilLog.h:151-202] |
| | `tprintf()` + 12 个 `LOG_*` 别名 | [src/util/utilLog.h:209-227] |
| `utilLibrary.h` | `Util::Library::LoadMode`（`Lazy`/`Now`/`Global`/`DeepBind`） | [src/util/utilLibrary.h:28-37] |
| | `Util::Library::Load` / `LoadPath` | [src/util/utilLibrary.h:45-68] |
| | `Util::Library::Unload` | [src/util/utilLibrary.h:70-78] |
| | `Util::Library::GetSymbol<T>` | [src/util/utilLibrary.h:85-91] |
| | `Util::Library::GetPath` / `GetError` | [src/util/utilLibrary.h:93-142] |
| | `Util::Library::Reload` / `ReloadPath` | [src/util/utilLibrary.h:153-161] |
| `utilObjectPool.h` | `Musa::SpinLock`（TTAS） | [src/util/utilObjectPool.h:14-34] |
| | `Musa::ObjectPoolBase` | [src/util/utilObjectPool.h:36-39] |
| | `Musa::ObjectPool<T>` | [src/util/utilObjectPool.h:44-139] |
| | `Musa::ShardedPool<T>`（64 分片） | [src/util/utilObjectPool.h:145-167] |
| `utilExecutor.h` | `Util::Executor`（单线程池） | [src/util/utilExecutor.h:13-89] |
| | `Executor::QueueTask` → `std::future` | [src/util/utilExecutor.h:42-63] |
| | `Executor::Worker` | [src/util/utilExecutor.h:66-82] |
| `utilSplayTree.h` | `Util::SplayTreeNode<K,V>` | [src/util/utilSplayTree.h:12-37] |
| | `Util::SplayTree<K,V>::Get` / `Find` / `Insert` / `Delete` | [src/util/utilSplayTree.h:90]、[src/util/utilSplayTree.h:97]、[src/util/utilSplayTree.h:147]、[src/util/utilSplayTree.h:185] |
| | `SplayTree::Splay`（自顶向下递归） | [src/util/utilSplayTree.h:105-142] |
| | `SplayTree::Search` | [src/util/utilSplayTree.h:216-226] |
| `utilSched.h` | `ScheduleMode{Sleep,Yield,Spin}` | [src/util/utilSched.h:19-23] |
| | `Schedule<S>()` 平台特化 | [src/util/utilSched.h:28-52] |
| | `ScheduleWait<S>(pred)` | [src/util/utilSched.h:55-60] |
| `utilMath.h` | `IsPowerOfTwo` / `AlignDown` / `AlignUp` | [src/util/utilMath.h:17-19]、[src/util/utilMath.h:21-24]、[src/util/utilMath.h:26-29] |
| | `IsMultipleOf` / `DivideRoundUp` / `PopCnt` | [src/util/utilMath.h:31-38]、[src/util/utilMath.h:40-43]、[src/util/utilMath.h:45-64] |
| | `RoundUpToNextPowerOfTwo` | [src/util/utilMath.h:71-92] |
| `utilOs.h` | `LIKELY` / `UNLIKELY`（三种实现分支） | [src/util/utilOs.h:27-36] |
| | `Os::Tid`、`GetProcessId`、`GetThreadId` | [src/util/utilOs.h:50-58] |
| | `GetPageSize` / `AlignedAlloc` / `AlignedFree` | [src/util/utilOs.h:60-68]、[src/util/utilOs.h:70-76]、[src/util/utilOs.h:78-84] |
| `utils.h` | `fatal`、`ShouldNotReachHere`、`MUSA_ASSERT` | [src/util/utils.h:16-29] |
| | `Utils::iReadEnv<T>` / `iPrintSetting<T>` | [src/util/utils.h:58-94]、[src/util/utils.h:97-123] |
| | `COMPILER_IGNORE_WARN_BEGIN/END` | [src/util/utils.h:129-137] |
| `logging.h` | `ToHexString` / `ToString`（**全局命名空间**） | [src/util/logging.h:7-30] |

## 环境变量

| 变量 | 读取点 | 语义 |
|---|---|---|
| `MUSA_LOG` | [src/util/utilLog.h:93]、[src/util/utilLog.h:96] | `Log::Flag` 位掩码；用 `std::stoul(env, nullptr, 0)` 解析（base-0，支持 `0x` 前缀）。**每进程只读一次**，缓存在函数内 static 中 [src/util/utilLog.h:95-98] |
| 任意名 | [src/util/utils.h:58-66] | `Utils::iReadEnv<T>(ptr, name1, name2, cb)`：先读 `var_name1`；仅当 `var_name1` 未定义**且** `var_name2 != "0"` 时读 `var_name2`。全部 48 处调用点第二参数均传 `"0"`，唯一消费者是 `src/musa/core/platformSettings.cpp` |

`Log::Flag` 位定义 [src/util/utilLog.h:59-72]：

| 位 | 名称 | 值 |
|---|---|---|
| 0 | `Err` | `0x1` |
| 1 | `Api` | `0x2` |
| 2 | `Sync` | `0x4` |
| 3 | `Mem` | `0x8` |
| 4 | `Copy` | `0x10` |
| 5 | `Warn` | `0x20` |
| 6 | `FB` | `0x40` |
| 7 | `Init` | `0x80` |
| 8 | `Queue` | `0x100` |
| 9 | `Cmd` | `0x200` |
| 10 | `Cache` | `0x400` |
| 11 | `Invoc` | `0x800` |

`iReadEnv<T>` 支持的 `T` [src/util/utils.h:70-88]：`int`、`uint32_t`、`uint64_t`、`std::string`、`bool`、`pair<bool,bool>`、`pair<bool,string>`。其他类型需提供 `envVariableCallback` 重载 [src/util/utils.h:61]、[src/util/utils.h:90-91]。

本模块**不读配置文件、不读注册表、不解析命令行**。

## 相关文档

- [`README.md`](README.md)
- [`call-chains.md`](call-chains.md)
- [`risks-and-debt.md`](risks-and-debt.md)
- [`../module-registry.md`](../module-registry.md)

## 源码证据摘要

- `src/util/utilLog.h`（11 处，例：[src/util/utilLog.h:26-55]）
- `src/util/utils.h`（10 处，例：[src/util/utils.h:37]）
- `src/util/utilSplayTree.h`（7 处，例：[src/util/utilSplayTree.h:12-37]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`risks-and-debt.md`](risks-and-debt.md)。
