# 修改影响图

| 修改点 | 直接模块 | 必查关联 |
|---|---|---|
| API/ABI 声明 | M01 | generated API、`.def`、wrapper、测试 |
| context/TLS | M02 | M04 memmgr、M05 stream、M06 module、sticky error |
| memobj/VA | M04 | M05 async tracking、M06 launch tracking、tools |
| stream/channel/QMD | M05 | M03 HAL/DMAL、M06 launch、marker/sync、M10 tests |
| module/function/launch | M06 | M04 memobj、M05 submit、M07 syscall、M08 callbacks；module unload 的 shared ELF/UVM/syscall/tool 清理 |
| syscall/asm | M07 | M03 架构 HAL、M05 channel、compiler/firmware |
| tools/debug/profiler | M08 | M02 context、M04 allocation、M06 launch control；memcheck device table、debugger shared state、profiler perfmon completion |
| OpenCL/interop | M09 | M02 context、M04 memory、M05 queue/event、M08 tools、外部 vendor；public/internal refcount、GL/D3D fence、external handle close |
| test/build rule | M10 | libcuda_test、dispatcher、DVS、package；binary/case registry、timeout/no-result/waived semantics、CI/MODS |

影响关系是源码和构建关系的静态归纳；动态配置和外部版本仍未知。
