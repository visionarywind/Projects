# M03 Runtime 源码地图

- 文档目的：解释 01-modules/M03-runtime/source-map.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cann HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：构建目录和关键入口已确认；非全量符号清单
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M03-runtime/source-map.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


| 路径 | 职责 | 证据 |
|---|---|---|
| `src/runtime/api/api_c_device.cc` | Device C API 门面 | `[49-119,165-215]` |
| `src/runtime/api/api_c.cc` | 通用 C API、句柄验证、错误处理 | `[118-153]` |
| `src/runtime/core/src/pool/` | KernelMemoryPool、MemoryList、固定槽位 BufferAllocator | `[memory_pool*.{hpp,cc}]` |
| `src/runtime/feature/soma/` | SOMA SegmentManager、PoolRegistry、异步内存池语义 | `[stream_mem_pool.{hpp,cc}]` `[soma.{hpp,cc}]` |
| `src/runtime/api/impl/api_impl_soma.cc` | SOMA C++ API 到 HAL/AICPU 配置 | `[64-267]` |
| `src/runtime/api/impl/api_impl_memory.cc` | 普通内存 policy、对齐和 Driver 委托 | `[765-842]` |
| `src/acl/aclrt.cpp` | ACL Runtime 符号映射、Hook、Dump callback | `[17-104]` |
| `src/acl/aclrt_impl/acl_rt_impl_base.cpp` | 初始化、SoC 和平台缓存 | `[25-199]` |
| `src/dfx/` | log、trace、error manager、msprof、adump | `[runtime/src/CMakeLists.txt:13-32]` |
| `src/aicpu_sched`、`queue_schedule` | 设备任务/队列调度 | 同上 |
| `tests/` | UT/COV/集成测试 | `runtime/AGENTS.md` |
| `example/` | 运行示例 | `runtime/README.md` |

第三方依赖由顶层 CMake 组织，包括 json、csec、protobuf 和 acl-compat `[runtime/CMakeLists.txt:39-70]`。

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
