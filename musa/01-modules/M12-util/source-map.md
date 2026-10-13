# Util 基础库：源码地图

- 文档目的：util：源码文件/目录到文档章节的映射。
- 适用范围：M12 模块的《Util 基础库：源码地图》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`risks-and-debt.md`](risks-and-debt.md)


| 文件 | 行数 | 提供内容 | 链接需要 |
|---|---|---|---|
| `CMakeLists.txt` | 4 | `util` STATIC 目标 | — |
| `utils.cpp` | 17 | `SteadyClock::SetStartTime`、`CalDurationUs` | 是 |
| `perfStatistics.cpp` | 35 | `Util::TimeStatistics` | 是 |
| `utilLog.h` | 227 | **日志机制**：`MUSA_LOG` 位掩码、`tprintf`、core dump 模板 | 否 |
| `utilLibrary.h` | 181 | `Util::Library`：`dlopen`/`LoadLibraryExW`/`dlsym` 包装 | 否 |
| `utilObjectPool.h` | 169 | `SpinLock`、`ObjectPool<T>`、`ShardedPool<T>` | 否 |
| `utilExecutor.h` | 90 | `Util::Executor`：单工作线程 + 任务队列 | 否 |
| `utilSplayTree.h` | 228 | `Util::SplayTree<K,V>`：自顶向下 Splay | 否 |
| `utilSched.h` | 61 | `Schedule<S>()`：Sleep/Yield/Spin 退避 | 否 |
| `utilMath.h` | 94 | 位运算与对齐数学（全部 `constexpr`） | 否 |
| `utilOs.h` | 87 | `LIKELY`/`UNLIKELY`、`Tid`、页大小、对齐分配 | 否 |
| `utils.h` | 137 | 断言宏、`Utils::iReadEnv`、`SteadyClock` | 否 |
| `logging.h` | 30 | 全局命名空间 `ToString`/`ToHexString` 辅助 | 否 |

## 命名陷阱

| 文件名 | 实际内容 |
|---|---|
| `logging.h` | **不含任何日志机制**，只有字符串化辅助函数 |
| 真正的日志 | `utilLog.h` |
| `utilMath.h` | 文件头注释误写为 `@file utilOs.h`（[src/util/utilMath.h:2]） |

## 消费者

`util` 目标仅在一处被链接：[src/hal/m3d/CMakeLists.txt:36-39] 列入 `M3D_LIBS`，在 [src/hal/m3d/CMakeLists.txt:59] 被 `halM3d` 消费。

但头文件被广泛直接包含，包括 `src/driver/internal.h:84`、`src/loader/loader.cpp:22`、`src/loader/driver_resolver.cpp:24`、`src/musa/core/platform.h:171` 等。

## 证据锚点

[src/util/CMakeLists.txt:1-4] [src/util/utilLog.h:92-101] [src/util/utilObjectPool.h:44-167] [src/util/utilSplayTree.h:216-226]

## 搜索建议

```bash
rg -n "getenv" src/util
rg -n "utilSplayTree|utilObjectPool|utilExecutor" src --glob '!src/util/*'
```

上述命令是阅读建议，不代表已在本地或远端执行。

## 相关文档

- [`README.md`](README.md)
- [`call-chains.md`](call-chains.md)
- [`risks-and-debt.md`](risks-and-debt.md)
- [`../module-registry.md`](../module-registry.md)

## 源码证据摘要

- `src/hal/m3d/CMakeLists.txt`（2 处，例：[src/hal/m3d/CMakeLists.txt:36-39]）
- `src/util/utilMath.h`（1 处，例：[src/util/utilMath.h:2]）
- `src/util/CMakeLists.txt`（1 处，例：[src/util/CMakeLists.txt:1-4]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`risks-and-debt.md`](risks-and-debt.md)。
