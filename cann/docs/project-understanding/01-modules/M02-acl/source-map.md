# M02 ACL 源码地图

- 对应源码版本：`50be4c922`
- 证据状态：主要路径已确认，非全量清单

| 路径 | 职责 | 证据/重点 |
|---|---|---|
| `runtime/device.cpp` | 设备选择、重置、查询、同步 | `aclrtSetDeviceImpl` `[acl/runtime/device.cpp:47-59]` |
| `runtime/context.cpp` | Context API | 构建源文件 `[acl/CMakeLists.txt:167-190]` |
| `runtime/stream.cpp` | Stream API | 同上 |
| `runtime/event.cpp` | Event API | 同上 |
| `runtime/memory.cpp`、`allocator.cpp` | 内存分配和释放 | 同上 |
| `runtime/model_ri.cpp` | Runtime model interface | 同上 |
| `runtime/data_buffer.cpp` | 数据 buffer 句柄 | 同上 |
| `runtime/callback.cpp` | 异步回调 | 同上 |
| `runtime/kernel.cpp` | Kernel/算子启动 | 同上 |
| `CMakeLists.txt` | 依赖、源文件和打包 | `[acl/CMakeLists.txt:110-190]` |
| `tests/` | ACL UT 和测试脚本 | `tests/run_test.sh`（仓库文档） |

Runtime 仓还包含公开 ACL Runtime 符号表和 Hook 入口 `[runtime/src/acl/aclrt/acl_rt.cpp:17-104]`；两处代码的最终打包归属需用安装包清单确认。
