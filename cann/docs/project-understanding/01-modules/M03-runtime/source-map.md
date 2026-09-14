# M03 Runtime 源码地图

- 证据状态：构建目录和关键入口已确认；非全量符号清单

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
