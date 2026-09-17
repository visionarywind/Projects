# M02 ACL 图示

- 文档目的：解释 01-modules/M02-acl/diagrams.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cann HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：边界图已由构建和关键源码确认
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M02-acl/diagrams.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。

## API 分层

```mermaid
flowchart TB
  App[Application] --> A[ACL public API]
  A --> W[acl*Impl wrappers]
  W --> R[rt* Runtime API]
  R --> I[Runtime Api::Instance]
  I --> D[Driver/HAL]
  D --> N[NPU]
```

## 资源生命周期

```mermaid
stateDiagram-v2
  [*] --> Uninitialized
  Uninitialized --> Initialized: aclInit
  Initialized --> DeviceSelected: aclrtSetDevice
  DeviceSelected --> ResourcesReady: create Context/Stream/Buffer/Model
  ResourcesReady --> Running: enqueue/execute
  Running --> ResourcesReady: synchronize/callback
  ResourcesReady --> DeviceSelected: destroy resources
  DeviceSelected --> Initialized: reset device
  Initialized --> Uninitialized: aclFinalize
```

状态图表示典型契约，不代表所有 API 都必须经过完全相同的状态；具体例外以公共头文件和实现为准。

## 相关文档
- [项目入口](../../README.md)
- [分析状态](../../00-overview/analysis-state.md)
- [源码证据索引](../../00-overview/evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](../../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
