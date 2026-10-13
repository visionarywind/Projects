# Loader 薄加载层：实现要点

- 文档目的：loader：核心函数/流程的逐段实现解读。
- 适用范围：M11 模块的《Loader 薄加载层：实现要点》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`risks-and-debt.md`](risks-and-debt.md)


## 驱动定位算法

`ResolveDriverLocation` [src/loader/driver_resolver.cpp:301-377] 的流程：

1. 判定平台 thunk 与驱动根目录 [src/loader/driver_resolver.cpp:272-287]。
2. `PlatformThunks::Load` 加载 thunk 并解析三个入口 [src/loader/driver_resolver.cpp:71-92]。
3. 枚举适配器，对每个适配器调用 `QueryPackageId` 得到包 ID。
4. 与参考适配器的包 ID 比对 [src/loader/driver_resolver.cpp:258]，选出驱动路径。
5. `fs::is_regular_file` 校验存在性 [src/loader/driver_resolver.cpp:356]。
6. `fs::canonical` 计算规范化路径 [src/loader/driver_resolver.cpp:361]。
7. 全程 `try/catch` 包裹 [src/loader/driver_resolver.cpp:303]、[src/loader/driver_resolver.cpp:373-377]。

### 路径穿越防护

`IsPackageCharacter` 是严格白名单 [src/loader/driver_resolver.cpp:168-171]，仅允许 `a-z A-Z 0-9 _ - .`；另有额外拒绝规则 [src/loader/driver_resolver.cpp:196]：拒绝 `"."`、`".."`、前导 `.`、结尾 `.`。

`ParsePackageId` 要求容量内存在 NUL 终止符 [src/loader/driver_resolver.cpp:177]，并在缓冲区被完全填满（无终止符）时报错 [src/loader/driver_resolver.cpp:190]。

由于 `fs::path driverRoot / packageId` [src/loader/driver_resolver.cpp:354] 使用的是已净化字符串，这构成一条完整的路径穿越防线。

### 大小写敏感性

Windows 用 `std::tolower` 比较 [src/loader/driver_resolver.cpp:260-265]，WSL 用精确 `==` [src/loader/driver_resolver.cpp:267]。这是必要的，因为 `DriverStore\FileRepository` 在 Windows 上大小写不敏感。

## 错误处理策略

| 层级 | 机制 | 证据 |
|---|---|---|
| 最外层 | `catch(std::bad_alloc)` → `MUSA_ERROR_OUT_OF_MEMORY` | [src/loader/driver_resolver.cpp:374] |
| 最外层 | `catch(...)` → `MUSA_ERROR_UNKNOWN` | [src/loader/driver_resolver.cpp:376] |
| 文件系统 | 使用 `error_code` 重载，不抛异常 | [src/loader/driver_resolver.cpp:356]、[src/loader/driver_resolver.cpp:361] |
| thunk 加载 | 三个独立失败折叠为单个 `bool` | [src/loader/driver_resolver.cpp:71-79] |
| 枚举循环 | `do{...}while(0)` + `break` 提前退出 | [src/loader/driver_resolver.cpp:127-163] |
| 符号缺失 | `Forward` 每次调用惰性检查空指针 | [src/loader/loader.cpp:180-208] |

`Forward` 返回 `MUSA_ERROR_SHARED_OBJECT_SYMBOL_NOT_FOUND` [src/loader/loader.cpp:180-208]，这是 `ResolveDriverExports` 之后仍为 `nullptr` 的符号的唯一兜底。

## 线程安全边界

| 机制 | 覆盖范围 | 证据 |
|---|---|---|
| `InitOnceExecuteOnce` / `std::call_once` | 驱动加载与符号解析 | [src/loader/loader.cpp:101]、[src/loader/loader.cpp:107] |
| 无锁 | `driverStatus` 读取 | [src/loader/loader.cpp:104]、[src/loader/loader.cpp:108] |
| 无锁 | `driverFunctions` 读取 | [src/loader/loader.cpp:195] |
| 无锁 | `SynchronizeDebuggerData` 对 `muDebugFlag`/`muGdbPid`/`muGdbSessionId` 的非原子读写 | [src/loader/loader.cpp:165-175] |

once-init 提供了足够的 happens-before 边，因此前三项当前是安全的。第四项是真实缺口：应用可能从任意线程写这三个全局变量，而 `SynchronizeDebuggerData` 没有任何同步（见 [`risks-and-debt.md`](risks-and-debt.md) 风险 7）。

## `muDebugInit` 的特殊处理

`Forward` 用 `if constexpr` 识别调试入口 [src/loader/loader.cpp:180-208]：

```cpp
if constexpr (std::is_same_v<Function, decltype(&::muDebugInit)>)
```

`muDebugSendTrapInfo` 和 `muDebugSendRunningKernelInfo` 签名与 `muDebugInit` **完全相同**（均为 `MUresult()`，[src/loader/loader_api_list.inc:566-568]），因此 `if constexpr` 对三者都为真，正确性完全依赖同函数内的运行期指针比较。

## `probe.cpp` 的实际作用

| 平台 | 入口 | 输出 | 证据 |
|---|---|---|---|
| Windows | `wmain` + `std::wcout` | `packageId`、`driverPath`、`resolvedDriverPath` | [src/loader/probe.cpp:7]、[src/loader/probe.cpp:15] |
| WSL | `main` + 窄字符流 | 同上 | [src/loader/probe.cpp:26]、[src/loader/probe.cpp:31] |

退出码为原始 `MUresult`，成功时强制为 0 [src/loader/probe.cpp:18]、[src/loader/probe.cpp:34]。Windows 分支从 `std::string` 宽构造包 ID [src/loader/probe.cpp:14]。

该文件当前**不参与构建**，[src/loader/CMakeLists.txt:74-93] 整段被注释；`resolvedDriverPath` 字段 [src/loader/driver_resolver.h:13] 存在的唯一理由就是它。

## 相关文档

- [`README.md`](README.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)
- [`../module-registry.md`](../module-registry.md)

## 源码证据摘要

- `src/loader/driver_resolver.cpp`（21 处，例：[src/loader/driver_resolver.cpp:301-377]）
- `src/loader/loader.cpp`（9 处，例：[src/loader/loader.cpp:180-208]）
- `src/loader/probe.cpp`（7 处，例：[src/loader/probe.cpp:7]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`risks-and-debt.md`](risks-and-debt.md)。
