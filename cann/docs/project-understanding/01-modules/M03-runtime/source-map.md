# M03 Runtime 源码地图

- 证据状态：构建目录和关键入口已确认；非全量符号清单

| 路径 | 职责 | 证据 |
|---|---|---|
| `src/runtime/api/api_c_device.cc` | Device C API 门面 | `[49-119,165-215]` |
| `src/runtime/api/api_c.cc` | 通用 C API、句柄验证、错误处理 | `[118-153]` |
| `src/runtime/` | Runtime 内部对象、任务和资源 | `src/CMakeLists.txt` |
| `src/acl/aclrt.cpp` | ACL Runtime 符号映射、Hook、Dump callback | `[17-104]` |
| `src/acl/aclrt_impl/acl_rt_impl_base.cpp` | 初始化、SoC 和平台缓存 | `[25-199]` |
| `src/dfx/` | log、trace、error manager、msprof、adump | `[runtime/src/CMakeLists.txt:13-32]` |
| `src/aicpu_sched`、`queue_schedule` | 设备任务/队列调度 | 同上 |
| `tests/` | UT/COV/集成测试 | `runtime/AGENTS.md` |
| `example/` | 运行示例 | `runtime/README.md` |

第三方依赖由顶层 CMake 组织，包括 json、csec、protobuf 和 acl-compat `[runtime/CMakeLists.txt:39-70]`。
