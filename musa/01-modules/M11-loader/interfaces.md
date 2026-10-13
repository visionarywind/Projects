# Loader 薄加载层：接口与构建目标

- 文档目的：loader：接口签名、前置条件、错误码与跨层契约。
- 适用范围：M11 模块的《Loader 薄加载层：接口与构建目标》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`risks-and-debt.md`](risks-and-debt.md)


## 构建目标

| 目标 | 类型 | 源文件 | 输出 | 证据 |
|---|---|---|---|---|
| `musa_loader_resolver` | OBJECT | `driver_resolver.cpp` | 无独立产物，被 `musa_loader` 吸收 | [src/loader/CMakeLists.txt:25-27] |
| `musa_loader` | SHARED | `loader.cpp` | `libmusa_loader.so` / `musa_loader.dll` | 目标名定义于 [src/loader/CMakeLists.txt:12]，`add_library` 于 [src/loader/CMakeLists.txt:51-53] |
| `musa_loader_probe` | 无 | `probe.cpp` | **整段被注释，不构建** | [src/loader/CMakeLists.txt:74-93] |

### 配置门禁（任一不满足即 `FATAL_ERROR`）

| 条件 | 证据 |
|---|---|
| `CMAKE_SIZEOF_VOID_P == 8`（仅 64 位） | [src/loader/CMakeLists.txt:1-3] |
| `WIN32` 或（`UNIX` 且 `MUSA_WSL2_BUILD`） | [src/loader/CMakeLists.txt:5-7] |
| `${loader_shared_include}/windows/kmdif/kmdif.h` 存在 | [src/loader/CMakeLists.txt:20-23] |

`loader_shared_include` 默认取 `${CMAKE_CURRENT_SOURCE_DIR}/../../../shared_include` [src/loader/CMakeLists.txt:14-18]，**该路径在本快照不存在**，必须由调用方传 `-DSHARED_INCLUDE_PATH=...`。

### 链接与导出

- `musa_loader` 链接 `musa_loader_resolver` 与 `${CMAKE_DL_LIBS}` [src/loader/CMakeLists.txt:60-63]，**不含 `util`**。
- UNIX 下追加 `-D_GNU_SOURCE` [src/loader/CMakeLists.txt:65-67]；resolver 侧还追加 `_GNU_SOURCE` 并引入 M3D 头路径 [src/loader/CMakeLists.txt:43-49]。
- 导出符号由 `include(export_symbols)` + `setup_unified_exports(musa_loader)` 生成 [src/loader/CMakeLists.txt:69-70]，清单即两个 `.inc` 文件 [cmake/export_symbols.cmake:11-14]。
- 本模块 `CMakeLists.txt` 内**没有任何 `install()` 规则**；产物拷贝由 `cmake_mtmusa.py` 完成，参见 `src/loader/README.md:132-141`。

## 对外 C ABI

Loader 唯一手写的公开函数是探测入口（供 `probe.cpp` 使用）：

| 签名 | 声明 | 定义 |
|---|---|---|
| `MUresult ResolveDriverLocation(DriverLocation&) noexcept` | [src/loader/driver_resolver.h:16] | [src/loader/driver_resolver.cpp:301] |
| `Musa::MiniLoader::EnsureInitialized() noexcept` | [src/loader/loader.h:7] | [src/loader/loader.cpp:97] |

其余 **570 个 `extern "C"` 符号全部由宏生成** [src/loader/loader.cpp:211-215]：

```cpp
#define MUSA_LOADER_API(name, parameters, arguments)                                       \
    extern "C" MUresult MUSAAPI name parameters {                                          \
        return Musa::MiniLoader::Forward(&Musa::MiniLoader::DriverFunctions::name           \
                                         MUSA_LOADER_UNPAREN arguments);                   \
    }
```

生成清单分布在：
- 通用 API 567 条：[src/loader/loader_api_list.inc:2-568]，首条 `muStreamCreate` [src/loader/loader_api_list.inc:2]，末三条 `muDebugInit`、`muDebugSendTrapInfo`、`muDebugSendRunningKernelInfo` [src/loader/loader_api_list.inc:566-568]。
- Windows 独有 3 条 D3D11 API：[src/loader/windows_api_list.inc:2-4]。

### 数据导出（非 `MUSAAPI`）

| 符号 | 类型 | 证据 |
|---|---|---|
| `muDebugFlag` | `bool` | [src/loader/loader.cpp:27] |
| `muGdbPid` | `uint32_t` | [src/loader/loader.cpp:28] |
| `muGdbSessionId` | `uint32_t` | [src/loader/loader.cpp:29] |

三者由驱动模块持有，loader 通过 `SynchronizeDebuggerData` 把指针同步过来 [src/loader/loader.cpp:165-175]，并在 ELF version script 中以 `DATA` 形式发出 [cmake/export_symbols.cmake:74-78]。

## 清单如何被消费

`loader_api_list.inc` 在 `loader.cpp` 中被**包含三次**，无 include guard：

| 轮次 | 位置 | 宏展开为 | 目的 |
|---|---|---|---|
| 1 | [src/loader/loader.cpp:118-125] | `decltype(&::name) name = nullptr;` | 声明 `DriverFunctions` 函数指针表 |
| 2 | [src/loader/loader.cpp:149-153] | `ResolveFunction(module, #name, resolvedFunctions.name);` | 解析后端符号 |
| 3 | [src/loader/loader.cpp:211-218] | 完整 `extern "C"` 函数体 | 生成 570 个转发包装 |

`MUSA_LOADER_UNPAREN` [src/loader/loader.cpp:210] 用于剥掉 `arguments` 字段前导的逗号和括号：`arguments` 写作 `(, phStream, Flags)`，展开后得到 `, phStream, Flags`。

同一份 `.inc` 同时驱动 CMake 导出表生成 [cmake/export_symbols.cmake:22-50]。**清单漏一条即静默少一个导出符号**，没有任何一致性检查。

## 环境变量

本模块**不读取任何环境变量**（`src/loader/` 下无 `getenv`）。所有配置均为编译期常量或硬编码路径：

| 配置 | 取值 | 证据 |
|---|---|---|
| Windows thunk | `<SystemDir>\gdi32.dll` | [src/loader/driver_resolver.cpp:279] |
| Windows 根目录 | `<SystemDir>\DriverStore\FileRepository` | [src/loader/driver_resolver.cpp:280] |
| WSL thunk | `/usr/lib/wsl/lib/libdxcore.so` | [src/loader/driver_resolver.cpp:283] |
| WSL 根目录 | `/usr/lib/wsl/drivers` | [src/loader/driver_resolver.cpp:284] |
| Windows 后端名 | `mtmusa64.dll` | [src/loader/driver_resolver.cpp:292] |
| WSL 后端名 | `libmusa.so.1.1` | [src/loader/driver_resolver.cpp:294] |
| 最大适配器数 | 256 | [src/loader/driver_resolver.cpp:32] |

`MUSA_LOADER_OUTPUT_DIR` 仅出现在 `README.md:27` 和被注释的 [src/loader/CMakeLists.txt:88]、[src/loader/CMakeLists.txt:91]，**运行期代码从不读取**。

## 相关文档

- [`README.md`](README.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)
- [`../module-registry.md`](../module-registry.md)

## 源码证据摘要

- `src/loader/CMakeLists.txt`（14 处，例：[src/loader/CMakeLists.txt:25-27]）
- `src/loader/loader.cpp`（10 处，例：[src/loader/loader.cpp:97]）
- `src/loader/driver_resolver.cpp`（8 处，例：[src/loader/driver_resolver.cpp:301]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`risks-and-debt.md`](risks-and-debt.md)。
