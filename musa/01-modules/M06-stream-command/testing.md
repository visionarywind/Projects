# Stream 与 Command 调度：测试策略

- 文档目的：stream command：测试入口、缺口与建议用例（含未验证标注）。
- 适用范围：M06 模块的《Stream 与 Command 调度：测试策略》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


- 结论先行：本知识库**没有执行过**任何构建或测试；下列命令与用例均为设计，状态一律是"未执行"。
- 单测基础设施在本快照中**不完整**：根 `CMakeLists.txt` 声明了 `MUSA_BUILD_UT` 并 `add_subdirectory(unittest)`，但源码快照里没有 `unittest/` 目录，直接开该选项会在配置阶段失败。[CMakeLists.txt:29] [CMakeLists.txt:167-168]

## 静态检查（可在本机执行）

```bash
# 1) 符号与调用点一致性
rg -n "QueueCommand|AsyncSubmit|AsyncWait" src/musa/core
rg -n "ResolveDependencyAndQueueCommand" src/musa/core/context.cpp
rg -n "^MUresult muapiStream" src/driver/mu_stream.cpp

# 2) 状态推进没有绕过 CAS
rg -n "m_Status\s*=" src/musa/core/command

# 3) 资源创建与释放成对
rg -n "new Stream|delete stream|DestroyStream|ReleaseCmdBuffer" src/musa/core

# 4) 导出符号回归（M02 边界）
rg -n "muapiStreamCreate|muapiStreamDestroy|muapiStreamSynchronize" src/driver
```

`git diff --check` 只验证本文档仓库的格式，不验证远端项目。

## 构建（未执行）

```bash
cmake -S . -B build -DMUSA_BUILD_UT=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

**预期失败点**：`MUSA_BUILD_UT=ON` 时 `add_subdirectory(unittest)` 找不到目录。开单测前必须先补上 `unittest/` 或改用上游真实单测仓。目标环境另需私有子模块、MUSA 工具链、驱动与 GPU。

## 运行验证用例（设计，未执行）

| 用例 | 输入 | 预期 | 覆盖的源码点 |
|---|---|---|---|
| T01 创建/查询/销毁 | `muapiStreamCreate` + `GetFlags`/`GetPriority`/`GetId` + `Destroy` | 全部 `MUSA_SUCCESS`；重复 destroy 返回 `MUSA_ERROR_INVALID_HANDLE` | [src/driver/mu_stream.cpp:17-42] [src/driver/mu_stream.cpp:168-199] |
| T02 非法 flags | `muapiStreamCreate(&s, 0x123)` | `MUSA_ERROR_INVALID_VALUE` | [src/driver/mu_stream.cpp:21-42] |
| T03 异步 copy 返回快 | `muapiMemcpyAsync` 后立刻返回 | 立即返回，`muapiStreamQuery` 首次可能 `MUSA_ERROR_NOT_READY` | [src/musa/core/context.cpp:702-734] [src/musa/core/stream.cpp:210-221] |
| T04 同步点语义 | 默认流 vs non-blocking 流上排队后 `Synchronize` | 默认流等全部非 non-blocking 流；non-blocking 流只等自己 | [src/musa/core/context.cpp:2086-2101] [src/musa/core/stream.cpp:223-239] |
| T05 背压 | 连续提交 > 1024 条不完成的命令 | `QueueCommand` 自旋等待，不丢命令、不崩溃 | [src/musa/core/stream.cpp:1056-1116] |
| T06 合批 | 单流连续 100 条可合并命令 | 批次被切成多批，每批 <= 32 | [src/musa/core/stream.cpp:1287-1306] |
| T07 event 路径 | `muapiEventRecord` + `muapiStreamWaitEvent` + `muapiEventSynchronize` | 时序正确，无死锁 | [src/driver/mu_event.cpp:101-131] |
| T08 capture 正常 | `BeginCapture` -> kernel/copy -> `EndCapture` | 产出非空 graph，状态回到 `NONE` | [src/musa/core/stream.cpp:45-139] |
| T09 capture 异常 | capture 中调用 `Synchronize` | 返回 `MUSA_ERROR_STREAM_CAPTURE_UNSUPPORTED`，状态变 `MU_STREAM_CAPTURE_STATUS_INVALIDATED` | [src/musa/core/stream.cpp:223-239] |
| T10 错误传播 | 注入 HAL Submit 失败 | 批次全部置 `error`；后续 `QueueCommand` 返回 sticky error | [src/musa/core/stream.cpp:1160-1246] |
| T11 错误恢复 | T10 之后继续提交 | 确认是否必须重建 stream（**重点待验证**） | [src/musa/core/stream.cpp:1056-1116] |
| T12 多流并发 | N 条流并发提交 + 每条流销毁 | 无 use-after-free、无线程泄漏 | [src/musa/core/stream.cpp:850-912] |
| T13 barrier/event 流 | `muapiEventRecord` 后跟普通流命令 | barrier 依赖正确建立 | [src/driver/mu_event.cpp:101-107] [src/musa/core/context.cpp:2043-2062] |
| T14 销毁时机 | 有未完成命令时 `Destroy` | `LockedWait` 先等完，再释放 HAL 资源 | [src/driver/mu_stream.cpp:168-199] |

## 建议观测点

- 每个命令记录 `m_QueuedTimestamp`、`m_SubmittedTimestamp`、`GetTimestamp(i)`。
- 每条流记录 `m_AsyncCount` 峰值与四个队列长度峰值。
- 每个 engine 记录 `m_InflightSubmissionCounts` 峰值（验证配额是否触顶）。
- 错误场景导出 `TakeAbortedCommandSnapshots()`。
- 用 MUpti 的 submission 关联核对 `m_SubId` 与硬件执行是否一一对应。

## 回归清单

- [ ] 默认流同步语义（T04）
- [ ] capture 正常/异常（T08/T09）
- [ ] 错误 sticky 与恢复（T10/T11）
- [ ] 背压与合批（T05/T06）
- [ ] 多流并发销毁（T12/T14）
- [ ] 导出符号表未变化（M02）
- [ ] `M3D_BUILD_MT_TRACE_CAPTURE` 开/关两种配置均构建（见 [`risks-and-debt.md`](risks-and-debt.md) 第 13 项）

模块专项用例见 `80-demos` 与远端 `tests` 文件；本知识库不声称上述命令或用例已通过。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/musa/core/stream.cpp`（9 处，例：[src/musa/core/stream.cpp:210-221]）
- `src/driver/mu_stream.cpp`（4 处，例：[src/driver/mu_stream.cpp:17-42]）
- `src/musa/core/context.cpp`（3 处，例：[src/musa/core/context.cpp:702-734]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
