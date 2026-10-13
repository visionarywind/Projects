# 平台与设备管理：设计分析

- 文档目的：platform device：设计意图、职责边界与关键取舍。
- 适用范围：M03 模块的《平台与设备管理：设计分析》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


## 设计问题

该模块把「进程单例 -> HAL 平台创建 -> 设备枚举/排序/可见性筛选 -> 惰性初始化」拆成稳定边界：`Platform` 承担全局状态与登记，`Device` 承担单设备状态与初始化，`IPlatform`/`IDevice` 提供解耦接口。HAL 对象由 Core 持有（`m_pHalPlatform`、`m_pHalDevice`），对外不暴露。

## 关键机制

- 进程单例：`Platform::Get()` 用函数内静态对象实现，构造时 `m_InitStatus = MUSA_ERROR_NOT_INITIALIZED`。[src/musa/core/platform.cpp:14-31]
- 一次性初始化：`std::call_once` + 末尾写入 `m_InitStatus`，保证并发线程看到完整初始化结果。[src/musa/core/platform.cpp:85-87] [src/musa/core/platform.cpp:133-138]
- 惰性设备初始化：`GetDevice` 首次触发 `LaterInit`，避免启动阶段初始化全部设备。[src/musa/core/platform.cpp:141-150]
- 登记集合：Platform 与 Device 各自用 CriticalBase（`shared_mutex`）登记 context/library/pool/stream。[src/musa/core/platform.h:63-93]

## 设计取舍

| 取舍 | 好处 | 代价 | 证据状态 |
|---|---|---|---|
| 函数内静态对象单例 | 免手工释放、进程内全局唯一 | 析构顺序依赖静态生命周期，HAL 清理发生在 static 析构期 | 已确认 |
| 惰性初始化 | 降低启动开销、按需初始化 | 首次调用承担失败与 `call_once` 锁开销 | 已确认 |
| CriticalBase 读写锁集合 | 支持多线程登记/查找 | 锁序与死锁风险需审查 | 已确认 |
| 设备按 PCI/IP 重排 | 序数稳定、便于按拓扑排布 | 依赖 HAL properties 可用性 | 已确认 |
| 可见性筛选在 Core 层 | 可屏蔽坏设备 | 非法/重复 index 的 `break` 语义未最终定 | 部分确认（TODO） |

## 不变量

1. 公共句柄（ordinal、pciBusId）在使用前必须映射到有效 `Device`。[src/musa/core/platform.cpp:141-150] [src/musa/core/platform.cpp:152-195]
2. `m_InitStatus` 只在初始化全部完成后写入成功/失败，其他线程不会看到半初始化平台。[src/musa/core/platform.cpp:133-138]
3. Device 资源登记成功前，HAL 初始化失败不得暴露半初始化对象（`LaterInit`/`CreateContext` 失败路径均不登记）。[src/musa/core/device.cpp:911-1017] [src/musa/core/device.cpp:663-674]
4. 销毁路径必须先处理未完成工作（`LockedWait`），再释放 HAL 对象。[src/musa/core/platform.cpp:610-656]

以上 1、2、3、4 项均有源码路径支持；第 3 项对 copy manager/UniversalManager 之外的错误分支仍需逐函数确认。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/musa/core/platform.cpp`（8 处，例：[src/musa/core/platform.cpp:14-31]）
- `src/musa/core/device.cpp`（2 处，例：[src/musa/core/device.cpp:911-1017]）
- `src/musa/core/platform.h`（1 处，例：[src/musa/core/platform.h:63-93]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
