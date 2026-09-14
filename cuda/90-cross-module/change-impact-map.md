# 修改影响图

| 修改点 | 直接模块 | 必查关联 |
|---|---|---|
| API/ABI 声明 | M01 | generated API、`.def`、wrapper、测试 |
| context/TLS | M02 | M04 memmgr、M05 stream、M06 module、sticky error |
| memobj/VA/suballocator | M04 | M05 async tracking、M06 launch tracking/scheduler backing、tools；同步 descriptor compatibility、DMAL free 和碎片验证 |
| stream/channel/QMD | M05 | M03 HAL/DMAL、M06 launch、marker/sync、M10 tests |
| module/function/launch/graph | M06 | M04 memobj/suballocator、M05 submit/QMD/marker、M07 syscall、M08 callbacks；graph 的 per-context resources、scheduler backing、capture/update/destroy |
| syscall/asm | M07 | M03 架构 HAL、M05 channel、compiler/firmware |
| tools/debug/profiler | M08 | M02 context、M04 allocation、M06 launch control；memcheck device table、debugger shared state、profiler perfmon completion |
| OpenCL/interop | M09 | M02 context、M04 memory、M05 queue/event、M08 tools、外部 vendor；public/internal refcount、GL/D3D fence、external handle close |
| test/build rule | M10 | libcuda_test、dispatcher、DVS、package；binary/case registry、timeout/no-result/waived semantics、CI/MODS |

影响关系是源码和构建关系的静态归纳；动态配置和外部版本仍未知。

Graph 资源修改的最低联查集合：`cuigraph.c` instantiate/launch/destroy、`cuistream.c` capture/detach、`cuilaunch.c` memory tracking、`memobj.c`/`suballocator.c` backing 生命周期和 `qmd.c`/constant-bank pool。当前 `tests.nvmk` 未注册专用 Graph API 测试，任何资源回收修复都应先补测试再声明完成。
