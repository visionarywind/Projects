# 设计原则与取舍

- 文档目的：从源码行为归纳可验证的设计约束，区分事实与推断。
- 适用范围：核心 Driver runtime。
- 对应源码版本：CUDA 10.2 API 字段；提交未知。
- 证据状态：部分已确认，设计意图多为推断。
- 最后更新：2026-09-11
- 前置阅读：[总体架构](architecture.md)
- 后续阅读：[模块实现](../01-modules/M04-memory-uvm/implementation.md)

## 结论摘要

1. **兼容优先**：公开旧符号映射到 v2/PTDS/PTSZ 变体，内部统一使用 v2 类型，降低 ABI 演进成本。
2. **状态集中管理**：TLS 决定线程当前 context，global state 管理进程级资源，context/stream pool 管理生命周期。
3. **资源对象化**：内存以 `CUmemobj` 描述，流以 `CUIstream` 描述，便于统一查找、跟踪、工具通知和清理。
4. **异步提交与完成分离**：stream/channel/marker/semaphore 组合，允许 API 返回早于设备完成。
5. **平台策略编译时选择、运行时分派**：nvmk 宏选择 OS/架构代码，`CUdev->hal` 再选择具体硬件操作。

## 证据与取舍

| 原则 | 源码行为 | 收益 | 代价 |
|---|---|---|---|
| ABI 兼容 | `inc/cuda.h:85-170` 大量别名 | 老调用者可链接 | 旧/新语义并存，容易改错入口 |
| 统一初始化检查 | `cuiInitCheckEx:2910-2961` | 错误语义一致、sticky error 可集中处理 | 每个 API 都有检查成本 |
| 内存统一对象 | `apimem.c:89-116` → `memobjAlloc` | UVA/P2P/tools 可复用 | 指针查找和多种 owner 状态复杂 |
| stream pool | `cuistream.c:1900-1924` | 复用对象、延迟回收 | detached 状态和异步完成复杂 |
| 多架构 HAL | `cuda.nvmk:513` 起 | 覆盖多个 GPU 世代 | 编译矩阵大、动态目标难静态确认 |
| 生成 API/导出表 | `cuda.nvmk:348-376,1701-1704` | 降低手写分派错误 | 生成链依赖外部工具，源码阅读门槛高 |

## 稳定契约与实现细节

**稳定契约（已确认）**：公开函数名/参数/返回码、context/stream handle 语义、导出表；**实现细节（已确认）**：`memobj` 字段、QMD pool 深度、stream detached/free pool；**推断**：锁顺序和工具回调接口是内部协作契约，但没有独立版本规范可读。

## 相关文档

- [全局错误模型](global-error-model.md)
- [配置影响](../90-cross-module/configuration-impact-map.md)

## 源码证据摘要

- `[src/cui/cuimem.c:116-136]` 默认内存 flags 与 32/64 位地址策略。
- `[src/cui/cuistream.c:1840-1866]` stream priority、ID、UVM 登记和 tools 通知。

## 未解决问题

设计决策原始 ADR 不在当前树中；本页的“为什么”只在有源码行为支撑时归纳，其余标为推断。

## 下一步阅读建议

先掌握对象生命周期，再读硬件提交路径。
