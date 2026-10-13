# Stream 与 Command 调度：图示

- 图中节点名来自源码标识符；箭头表示调用或数据流转，不代表已观测到的运行时顺序。
- 文档目的：stream command：核心关系与流程图示，及节点到源码的映射。
- 适用范围：M06 模块的《Stream 与 Command 调度：图示》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：静态确认；并发时序与硬件提交顺序**未验证**。
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)

## 分层图

```mermaid
flowchart LR
 U[调用方 / muapi*] --> I[Driver 层 src/driver/mu_stream.cpp]
 I --> O[Core 层 Context / Stream]
 O --> C[Command 对象]
 C --> H[HAL 接口 Hal::IQueue / ICmdBuffer / ISemaphore]
 H --> D[HAL 实现 src/hal/m3d]
 D --> R[结果、MUresult、异步完成]
 O -.时间戳/异常.-> P[MUpti 观测]
```

## Command 生命周期状态图

```mermaid
stateDiagram-v2
 [*] --> created
 created --> queued: QueueCommand
 queued --> built: Command::Build
 built --> submitted: submitMergingList -> Submit
 submitted --> completed: AsyncWait 等到 semaphore
 submitted --> error: Submit 失败 / 引擎错误
 queued --> error: 批次 Submit 失败
 built --> error: 批次 Submit 失败
 completed --> [*]
 error --> [*]
```

状态值与推进规则见 [src/musa/core/command/command.h:35-42]，CAS 实现见 [src/musa/core/command/command.cpp:92-120]。

## Stream 内部队列流转图

```mermaid
flowchart LR
 subgraph API线程
  A[API 线程] --> B[QueueCommand]
  B --> Q1[m_CommandList]
 end
 subgraph 提交线程
  Q1 --> E[buildCommand]
  E --> Q2[m_MergingList]
  Q2 --> F[submitMergingList]
  F --> Q3[m_InflightList]
 end
 subgraph 等待线程
  Q3 --> G[Wait semaphore]
  G --> Q4[m_WaitingList]
  G --> S[completed / error]
  Q4 --> R[ReleaseResources]
 end
 S --> K[m_LastCommand / Query / Synchronize]
```

四个容器与 `m_LastCommand` 的字段位置见 [src/musa/core/stream.h:195-203]。

## 提交时序图

```mermaid
sequenceDiagram
 participant API as API 线程
 participant SUB as AsyncSubmit 线程
 participant HAL as Hal::IQueue
 participant WAIT as AsyncWait 线程
 API->>SUB: m_CommandList.push_back + notify_one
 SUB->>SUB: buildCommand: CanMergeTo / Command::Build
 SUB->>SUB: submitMergingList: status = front()->Submit()
 SUB->>HAL: pQueue->Submit(submitInfo)
 HAL-->>SUB: MUresult
 SUB->>SUB: splice Merging -> Inflight, notify m_WaitCv
 WAIT->>HAL: 等待 signal timeline/hardware semaphore
 HAL-->>WAIT: 到值
 WAIT->>WAIT: SetStatus(completed|error), m_AsyncCount--
 WAIT->>WAIT: ReleaseResources（异步执行）
 API->>API: Query / Synchronize / LastCommand->Wait 读状态
```

## 依赖解析决策树

```mermaid
flowchart TD
 S[发命令的流] --> Q{是默认流?}
 Q -->|是| R1[依赖本 context 全部非默认/非 barrier/非 non-blocking 流]
 Q -->|否| B{是 barrier 流?}
 B -->|是| R2[依赖其它所有流]
 B -->|否| N{是普通 blocking 流?}
 N -->|是| R3[依赖默认流]
 N -->|否| R4[不加流间依赖]
 R1 --> X[再叠加 barrier 流 LastCommand]
 R2 --> X
 R3 --> X
 R4 --> X
 X --> Y[合并 pStream GetCurrentDependencies 并清空]
 Y --> Z[QueueCommand]
 Z --> W{blocking 或 pfmSerialMode?}
 W -->|是| WW[command->Wait]
 W -->|否| NN[返回]
```

分支代码位于 [src/musa/core/context.cpp:2012-2084]。

## capture 状态图

```mermaid
stateDiagram-v2
 [*] --> NONE
 NONE --> ACTIVE: BeginCapture 成功
 ACTIVE --> ACTIVE: CaptureNode 追加节点
 ACTIVE --> NONE: EndCapture 成功并清理
 ACTIVE --> INVALIDATED: CaptureNode 失败
 ACTIVE --> INVALIDATED: capture 中调用 Synchronize
 INVALIDATED --> NONE: 重新 BeginCapture
```

状态值是 `MUstreamCaptureStatus`，字段为 `m_CaptureStatus`。[src/musa/core/stream.h:249-250]

## semaphore 选择

```mermaid
flowchart TD
 P[Command::Build 需要 wait 依赖] --> C{双方 preferred 都是 Hardware 且同 device?}
 C -->|是| H[用上游的 hardware semaphore]
 C -->|否| T[用上游的 timeline semaphore]
 H --> M[push 进 m_WaitSemaphoreInfos]
 T --> M
 M --> S2[Submit 阶段展开成 Hal::QueueSubmitInfo]
 S2 --> Q[Hal::IQueue::Submit]
```

`GetPreferredSemaphoreType` 由 `Stream::EnableUserQueue(engine)` 决定。[src/musa/core/command/command.cpp:288-291]

## Stream 生命周期图

```mermaid
stateDiagram-v2
 [*] --> Created: Context::CreateStream + Stream::Init
 Created --> Running: 线程启动 / 有命令进出
 Running --> Running: 合批 / 提交 / 完成
 Running --> Draining: 析构置 stop token
 Draining --> [*]: join 两条线程后释放 HalPtr
```

`Init` 在起线程前完成全部 HAL 资源创建。[src/musa/core/stream.cpp:930-1033]

## 状态名说明

图中的 `created`/`queued`/`built`/`submitted`/`completed`/`error` 是 `Musa::Command::Status` 的枚举名；`NONE`/`ACTIVE`/`INVALIDATED` 是 `MUstreamCaptureStatus` 三个取值 `MU_STREAM_CAPTURE_STATUS_NONE`、`MU_STREAM_CAPTURE_STATUS_ACTIVE`、`MU_STREAM_CAPTURE_STATUS_INVALIDATED` 的简写。其它节点名是为可读性起的标签，不能替代源码中的具体符号。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/musa/core/command/command.cpp`（2 处，例：[src/musa/core/command/command.cpp:92-120]）
- `src/musa/core/stream.h`（2 处，例：[src/musa/core/stream.h:195-203]）
- `src/musa/core/command/command.h`（1 处，例：[src/musa/core/command/command.h:35-42]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
