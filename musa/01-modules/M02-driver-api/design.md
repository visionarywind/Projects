# Driver API 导出层：设计分析

- 文档目的：driver api：设计意图、职责边界与关键取舍。
- 适用范围：M02 模块的《Driver API 导出层：设计分析》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


## 设计问题

本模块把 `外部 C ABI（mu*）-> 包装/观测 -> muapi* 入口 -> Core 对象 -> MUresult` 拆成稳定边界：

1. **生成包装层**（`mu_wrappers_generated.cpp`）统一注入 `ApiTrace` 与 tools 回调，业务实现集中在 `muapi*`。[src/driver/mu_wrappers_generated.cpp:3819-3857]
2. **入口层**负责契约（参数检查、错误码、句柄转换），把状态与资源登记交给 Core。
3. **导出表层**（`muapiGetExportTable`）按 client 分派，观测工具（MUPTI/mugdb/muasan/tools）不依赖公共 ABI 符号。[src/driver/mu_entry.cpp:1255-2304]

代价是多层错误传播和生命周期耦合：一次调用要穿过「包装 -> 入口校验 -> Core -> HAL」，任一层的失败都折叠成 `MUresult`，定位成本高。

## 设计取舍

| 取舍 | 好处 | 代价 | 证据状态 |
|---|---|---|---|
| 分层而非 API 直达硬件 | 可替换 HAL、集中管理对象、观测可注入 | 调用链长、排查成本高 | 已确认 |
| 生成 wrapper + 双路径 | trace/callback 一处注入，实现只写一次 | 符号多、每次调用多一次回调判断 | 已确认 |
| 导出表按 client 分派 | 工具与公共 API 解耦，可裁剪 | 表结构与 ABI 耦合，未知 client 静默回退 | 已确认 |
| 句柄即 C++ 对象指针 | 转换开销小、零查找 | 伪造/悬垂句柄直接崩或 UB | 已确认风险，未验证防御 |
| 同步/异步由 settings 决定 | 灵活适配负载 | 行为依赖配置，同步语义不稳 | 部分确认 |
| 错误统一折叠为 `MUresult` | 契约简单 | 底层错误细节丢失 | 已确认 |

## 不变量

1. 公共句柄在使用前必须能映射到有效对象：`ValidateContext`/`ValidateMemoryPool` 在入口校验。[src/driver/mu_context.cpp:25-27] [src/driver/mu_mempool.cpp:147-148]
2. Core 资源登记成功前，底层初始化失败不得暴露半初始化对象：`LoadFatBinary` 失败回滚 `DestroyModule`；`MemoryPool::Init` 失败 `delete`。[src/driver/mu_module.cpp:134-141] [src/driver/mu_mempool.cpp:112-117]
3. 异步操作的资源必须活到对应 Command 完成：`std::shared_ptr<Command>` 在 `QueueCommand` 中 `std::move` 入列。[src/musa/core/stream.cpp:1052-1113]
4. 销毁路径必须先处理未完成工作，再释放 HAL 对象：`muapiMemFree_v2` 先 `Synchronize()` 再 `DestroyMemory`；stream 销毁先 `LockedWait`。[src/driver/mu_memory.cpp:766-778] [src/driver/mu_stream.cpp:191-194]
5. `muapiGetExportTable` 对已知 client 返回对应表；未知 client 落入 `default` 返回 MUPTI 表（源码注释称向后兼容，未来应改 `INVALID_VALUE`）。[src/driver/mu_entry.cpp:2300-2303]

第 3 项是异步资源管理的实现约束，完整证明仍需覆盖所有 Command；第 5 项是一个有 FIXME 注释的兼容性回退。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/driver/mu_entry.cpp`（2 处，例：[src/driver/mu_entry.cpp:1255-2304]）
- `src/driver/mu_mempool.cpp`（2 处，例：[src/driver/mu_mempool.cpp:147-148]）
- `src/driver/mu_wrappers_generated.cpp`（1 处，例：[src/driver/mu_wrappers_generated.cpp:3819-3857]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
