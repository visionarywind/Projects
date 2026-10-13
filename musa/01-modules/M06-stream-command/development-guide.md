# Stream 与 Command 调度：开发指南

- 文档目的：stream command：常见修改场景的落地入口、注意点与验证方式。
- 适用范围：修改 `src/driver/mu_stream.cpp`、`src/musa/core/stream.{h,cpp}`、`src/musa/core/context.cpp` 的流/依赖部分、`src/musa/core/command/*`、`src/hal/m3d/{queue,cmdBuffer}.*`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)
- 前置：本文所有行号对应 `5.3.0 develop` 本地快照；改代码前先重跑一次行号核对。

## 修改前

1. **锁定快照**：确认源码版本与证据基线（`5.3.0 develop`），更新本模块 `README.md` 的元信息。
2. **归类改动**：
   - ABI/导出符号 → 归 M02，不在本模块动导出表。
   - 状态机/容器/锁 → 本模块核心，必须重读 `AsyncSubmit`/`AsyncWait`。
   - HAL 提交参数 → 需同时看 `Command::SubmitToQueue` 与 `Hal::M3d::Queue::Submit`。
   - capture 分支 → 必须同时看 `Stream::Capture*` 与 `Context::General*`。
3. **画出调用链**：在 [`call-chains.md`](call-chains.md) 对应链条上标出你要改的边。
4. **明确资源所有者**：新对象由谁持有、活到哪一步释放，先写清楚再动手。

## 修改中

### 状态机

- `Command::Status` 只能沿 `created -> queued -> built -> submitted -> completed` 推进，非法转移由 `SetStatus` 返回 false 拦住；**不要**加绕过 CAS 的直接赋值。[src/musa/core/command/command.cpp:92-120]
- 新增状态必须追加在枚举**末尾**前先确认 MUpti 是否依赖该枚举数值。[src/musa/core/command/command.h:57-75]

### 依赖与 semaphore

- 新的依赖来源要加进 `Context::ResolveDependencyAndQueueCommand`，不要在 `Stream::QueueCommand` 里另起一套。[src/musa/core/context.cpp:2012-2084]
- 依赖上游时用 `FilterDependency` 先丢掉已完成的，避免无谓的 semaphore wait。[src/musa/core/command/command.cpp:156-166]
- 选择 hardware 还是 timeline semaphore 必须走 `GetPreferredSemaphoreType()`，不要散落判断。[src/musa/core/command/command.cpp:288-291]

### 并发

- 三把锁的既有顺序：`m_SubmitMtx`（改 `m_LastCommand`/入队）→ `m_MergingMtx`（合批与 Build）→ `m_WaitMtx`（完成与释放）。新代码不要引入反向获取。
- 背压检查与 `m_AsyncCount.fetch_add` 之间存在已知的非原子窗口，改动时不要扩大这个窗口。[src/musa/core/stream.cpp:1056-1116]
- 两个后台线程的退出依赖 `m_SubmitStopToken`/`m_WaitStopToken` 与 `notify`，新增等待点必须同时挂在等待谓词里（提交线程 1310-1314、等待线程 1344-1346）。[src/musa/core/stream.cpp:1310-1346]

### capture

- 每加一个 Cmd* 方法，都要同时加对应 Capture 分支，否则 capture 中会误入队。[src/musa/core/stream.cpp:673-694]
- capture 失败统一走 `m_CaptureStatus = MU_STREAM_CAPTURE_STATUS_INVALIDATED`，不要自创状态。

### 错误处理

- HAL 返回码在 Core 边界转 `MUresult`，不要把 HAL 码直接外抛。
- 命令级错误用 `SetLastError`，stream 级 sticky 用 `Stream::m_LastError`，两者不要混用。[src/musa/core/stream.h:217-217]

## 修改后

1. **静态检查**
   - `rg -n "<你改的符号>" src/` 确认所有调用点都已覆盖。
   - 检查新引用的资源在 `Stream::~Stream` 与 `Command::ReleaseResources` 中有对应释放。[src/musa/core/stream.cpp:850-912]
   - 本文档行号如已漂移，同步更新受影响的 `.md`。
2. **构建**：`MUSA_BUILD_UT` 选项存在，但本快照缺 `unittest/` 目录，直接开单测会失败；见 [`testing.md`](testing.md)。
3. **运行验证**：需目标机 + 私有子模块 + 工具链 + 驱动 + GPU。只在明确授权时执行安装脚本。
4. **回归重点**
   - 默认流语义（同步默认流要等全部非 non-blocking 流）。
   - capture 正常/异常两条路径。
   - 错误注入后的 sticky 行为与恢复条件。
   - 多流并发入队的背压与合批。

## 不要做的事

- 不要改 `MUstream` 句柄布局、导出表顺序（M02 的硬约束）。
- 不要在没有硬件证据时改变同步语义（`blocking` 参数含义、默认流规则）。
- 不要删除 `AsyncSubmit`/`AsyncWait` 里的错误快照与 `ErrorHandler` 调用——它们是唯一的事后取证入口。
- 不要假设 `s_CmdBufferListSize = 0` 是 bug；它是惰性分配的设计。
- 不要在未读完整函数体时，把本模块文档中的"推断/未验证"改成"已确认"。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/musa/core/stream.cpp`（4 处，例：[src/musa/core/stream.cpp:1056-1116]）
- `src/musa/core/command/command.cpp`（3 处，例：[src/musa/core/command/command.cpp:92-120]）
- `src/musa/core/command/command.h`（1 处，例：[src/musa/core/command/command.h:57-75]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
