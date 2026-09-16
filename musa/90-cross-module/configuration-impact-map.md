# 配置影响地图

| 配置 | 影响模块 | 影响面 | 证据 |
|---|---|---|---|
| `MUSA_BUILD_DEBUG` | M01 | Debug/Release 编译 | [CMakeLists.txt:127-130] |
| `ENABLE_ASAN_CHECK`/`TSAN` | M01/M10 | sanitizer 编译和 hook | [CMakeLists.txt:178-186] |
| `MUSA_BUILD_UT` | M01/测试 | FetchContent/gtest/CTest | [CMakeLists.txt:214-237] |
| visible devices/order | M03 | Device ordinal 和 muInfo结果 | [src/musa/core/platform.cpp:32-80] |
| `launchBlocking` | M06/M07 | kernel API 返回时机 | [src/driver/mu_module.cpp:266-267] |
| `memBlocking` | M05/M06 | async allocation/copy等待 | [src/driver/mu_memory.cpp:336-437] |
| `graphUserQ` | M08/M09 | GraphExec资源路径 | [src/musa/core/context.cpp:1429-1442] |
| `MUSA_TRACE_SUBMIT_FILE` | M06/M10 | submit trace文件输出 | [src/musa/core/stream.cpp:46-63] |

README/CMake 的 `CSV_UNSUPPORTED`/`CSV_UNSPPORTED` 差异应在配置验证前视为风险。[CMakeLists.txt:202-204]
