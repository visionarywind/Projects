# 延伸阅读与维护

## 本仓库资料

- `cann/docs/project-understanding/00-overview/architecture.md`
- `cann/docs/project-understanding/00-overview/global-error-model.md`
- `cann/docs/project-understanding/90-cross-module/memory-and-resource-lifecycle.md`
- `cann/docs/project-understanding/90-cross-module/error-boundaries.md`
- `cann/docs/project-understanding/90-cross-module/performance-critical-paths.md`
- `cann/docs/project-understanding/99-roadmap/debugging-guide.md`
- `MUSA/` 与 `LeetCUDA/` 中关于 Runtime、Context、Stream、内存和性能的证据文档

## 外部方向

按目标平台选择官方 Runtime/Driver API、设备编程指南、通信库、容器/虚拟化和监控文档。版本、硬件和行为差异必须记录到 sources-and-evidence.md，不用二手总结替代一手证据。

## 维护规则

每次升级目标版本、驱动、固件或设备拓扑时，复核源码路径、行号、错误映射、生命周期和性能假设；新增题目只能分配未使用 ID，并同步覆盖矩阵、术语、追问、状态和审计。
