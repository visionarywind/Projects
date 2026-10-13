# Context 与资源生命周期：风险与技术债

- 排序依据：按"生命周期与悬垂 > 并发与锁 > 语义正确性 > 构建/ABI 复现"排列。
- 状态口径：每条风险的**证据**列都给出源码引用；没有引用的结论不进本表。
- 通用前提：本页为静态分析，未验证。所有运行、并发、硬件行为都没有实测证据。
- 文档目的：context lifecycle：风险、技术债及源码证据与修复/验证建议。
- 适用范围：M04 模块的《Context 与资源生命周期：风险与技术债》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`line-level-analysis.md`](line-level-analysis.md)、[`call-chains.md`](call-chains.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)

## 风险表

| 风险 | 成因 | 证据（引用） | 验证/修复建议 |
|---|---|---|---|
| 资源所有权裸指针 | `CriticalBase` 用 `std::unordered_set<Stream*>` 一类裸指针容器，登记与摘除必须成对，漏一处即悬垂或泄漏 | [src/musa/core/context.h:257-294] `m_Streams` | 补漏销毁回归：漏 `Destroy*` 时验证析构 WARNING 输出且无泄漏 |
| 兜底清理依赖 WARNING | `ReleaseClientResources` 按类型遍历 `delete` 并打日志，说明设计依赖事后兜底而非强制配对 | [src/musa/core/context.cpp:293-377] `ReleaseClientResources` | Debug 构建里把漏登记改成断言，Release 才降级为 WARNING |
| `DestroyStream` 不等待命令 | 摘除后直接 `delete stream`，函数体内没有等待流上未完成命令的调用 | [src/musa/core/context.cpp:1219-1236] `DestroyStream` | 删除前对该流做一次同步，或把等待移进 `~Stream` |
| `DestroyEvent` 无同步点 | 函数体留有 `FIXME: not sync yet?` 注释，直接删除事件对象 | [src/musa/core/context.cpp:1334-1347] `DestroyEvent` | 删除前等待事件关联的命令完成，或返回未完成错误 |
| 引用计数非原子 | `m_RefCount` 是普通成员，`Retain` 走 `++`、`Release` 走 `--` 后判 0，跨线程并发会丢更新 | [src/musa/core/context.cpp:926-949] `m_RefCount` | 改 `std::atomic` 或把加减放进平台级临界区 |
| primary context 销毁路径穿透 | `muapiCtxDestroy_v2` 判到 primary 时先写 `MUSA_ERROR_INVALID_CONTEXT`，但没有分支终止，随后仍执行摘登记与 `DestroyContext` 并覆盖 status | [src/driver/mu_context.cpp:188-218] `muapiCtxDestroy_v2` | 把 primary 判定改成提前 `break` 返回，不先摘登记 |
| TLS 栈残留裸指针 | 只有调用线程栈顶命中才 `TlsCtxPop`，其它线程栈内仍指向已释放 context | [src/driver/mu_context.cpp:199-218] `TlsCtxPop` | 记录持有线程，在线程退出钩子里清栈 |
| primary 与用户 context 1:N 销毁 | `Device::~Device` 先遍历删除用户 context 集合，再 `m_PrimaryCtx.reset()`，顺序写死且无断言 | [src/musa/core/device.cpp:718-748] `m_PrimaryCtx` | 加销毁顺序回归用例（用户 ctx → primary → copy manager） |
| 归零与摘除不同步 | `Platform::ValidateContext` 要求计数非 0 且仍在 `ctxs` 集合内，而 `Release` 归零只改 `m_IsActivate` 不摘登记 | [src/musa/core/platform.cpp:430-452] `ValidateContext` | 统一"计数归零"与"从集合摘除"的语义与触发点 |
| per-thread 默认流不随线程清理 | `GetPerThreadDefaultStream` 用 `static thread_local` map 缓存流指针，线程退出不析构，生命周期整体绑到 context | [src/musa/core/context.cpp:1238-1278] `perThreadStreams` | 改成 context 侧管理，或挂线程退出钩子显式清理 |
| `Init` 失败的回滚靠析构链 | `Context::Init` 中途 `break`，已建对象留给 `CreateContext` 的 `delete ctx` → `Dispose` → `ReleaseClientResources` 回收 | [src/musa/core/context.cpp:1906-1958] `Context::Init` | 每个失败点显式置空已建指针，或在目标机逐点验证回滚 |
| `Dispose` 持写锁阻塞等待 | `Context::Dispose` 在持写锁期间调用同步等待，任何取锁方都会被挡住 | [src/musa/core/context.cpp:1967-2005] `Context::Dispose` | 先出锁再等待，或加锁内耗时打点 |
| `LockedReset` HAL 对齐未验证 | 只重建内部流并刷新 L2 persisting 属性，HAL 级对象是否完整对齐没有运行证据 | [src/musa/core/context.cpp:2120-2144] `LockedReset` | 在目标机跑 primary Reset 后全资源再创建用例 |
| `DestroyModule` 回调内嵌取写锁 | `fnRemoveRef` 在 `IterateSymbols` 回调里取写锁，锁序取决于 `IterateSymbols` 自身是否持锁 | [src/musa/core/context.cpp:1288-1313] `fnRemoveRef` | 审计 `IterateSymbols` 的持锁状态并画出完整锁序图 |
| ABI/导出表敏感 | 导出文件由 manifest 生成，`.ver` 少一个符号外部程序即加载失败 | [cmake/export_symbols.cmake:64-109] `generate_export_files` | 增加导出符号回归比对，纳入 CI |
| 构建环境不可复现 | googletest 走内网 URL，且快照缺 `unittest/` 目录 | [CMakeLists.txt:146-154] `googletest` | 固化依赖与 artifact，把单测目录纳入版本管理 |
| HAL 私有实现不可见 | `Queue::Submit` 及更深层的错误码、提交顺序、超时行为不在逐行核对范围内 | [src/hal/m3d/queue.cpp:182-372] `Queue::Submit` | 在目标源码与硬件上补证（M09 边界） |

## 技术债清单（非风险但值得还）

- `Context::Dispose` 把卸库、摘 GraphExec、同步、清理揉在一个函数里，建议按资源类型拆分。
- `GetPerThreadDefaultStream` 用 lambda 闭包直接 `new Stream`，创建与登记散落在闭包内，建议抽成成员函数。
- `Platform::ValidateContext` 用 `GetRefCount()` 与集合成员双重判据，两个判据的更新时机不同步，建议合成单一状态位。
- driver 层 `muapiCtxDestroy_v2` 的错误码赋值没有配 `break`，读起来像早退实则穿透，建议改 `do { ... } while(0)` 风格。

## 文档维护说明

- 本页所有行号来自本地快照，源码演进后会漂移；改动后需重跑行号与符号校验。
- 未执行的测试不标为通过；没有源码引用的结论不进风险表。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/musa/core/context.cpp`（9 处，例：[src/musa/core/context.cpp:293-377]）
- `src/driver/mu_context.cpp`（2 处，例：[src/driver/mu_context.cpp:188-218]）
- `src/musa/core/context.h`（1 处，例：[src/musa/core/context.h:257-294]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 上表风险的运行期复现与修复验证均未执行。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
