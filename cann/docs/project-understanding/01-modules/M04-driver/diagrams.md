# M04 Driver 图示

- 证据状态：主要边界已确认

```mermaid
flowchart TB
  R[Runtime / HAL caller] --> H[ascend_hal]
  H --> C[HDC / Queue / DMS / SVM / DPA]
  C --> S[sdk_driver]
  S --> I[ioctl / device nodes / DMA]
  I --> K[Kernel driver]
  K --> N[NPU]
```

## 普通内存 cache

```mermaid
flowchart LR
  R[rtMalloc] --> H[halMemAlloc]
  H --> M{V2/V3 cache 条件}
  M -->|V2| V2[heap + mapped/size trees]
  M -->|V3| V3[cache_allocator + range/area trees]
  M -->|不满足| N[normal allocator]
  V2 --> S[split/reuse]
  V3 --> S
  S --> F[free/merge]
  F --> T{shrink 条件}
  T -->|否| C[保留 backing]
  T -->|是| B[释放完整 backing/range]
```

图示表达用户态源码边界；底层物理页和设备完成时序仍未验证。

## HDC/Queue 生命周期

```mermaid
stateDiagram-v2
  [*] --> NoClient
  NoClient --> ClientReady: drvHdcClientCreate
  ClientReady --> SessionActive: connect/create session
  SessionActive --> ClientReady: close session
  ClientReady --> NoClient: destroy client
  NoClient --> QueueOpen: device file open
  QueueOpen --> QueueActive: init/connect
  QueueActive --> QueueOpen: uninit
  QueueOpen --> NoClient: release
```

销毁 client 的活动 session 检查和 queue 的 file 生命周期分别由 HDC/queue 源码确认；图中连接名称为概念化表示。
