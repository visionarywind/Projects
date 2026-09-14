# M09 源码地图、测试与开发配方

## 源码地图

| 区域 | 主要文件 | 作用 |
|---|---|---|
| ICD loader | `src/icd_rel/icd.c`, `icd_dispatch.c` | vendor 动态库、platform 枚举、公开转发 |
| NVIDIA OpenCL | `src/cl/cliglobals.c`, `cliicd.c`, `cliplatform.c`, `clidevice.c` | vendor 全局和 object dispatch |
| Queue/event | `clcommandqueue.c`, `clicommandqueue.c`, `clevent.c`, `clievent.c` | 异步 DAG、事件和提交 |
| Memory | `clmem.c`, `clmemcpy.c`, `clgl.c`, `clid3d.cpp` | buffer/image/copy/graphics sharing |
| Program/kernel | `clprogram.c`, `clkernel.c`, `cliprogram.c`, `clikernel.c` | 编译、binary、kernel 参数和 enqueue |
| Helper | `src/clh/clhmod.c` | CLH module 辅助路径 |

## 验证矩阵

- loader 无 vendor、单 vendor、多 platform：检查 platform count/error。
- 输出参数组合：`num_entries==0` 与 `platforms==NULL` 的规范错误。
- OpenCL globals 初始化失败：确认 platform/table 回滚和可重试。
- dispatch ABI：按 OpenCL feature macro 检查 NULL 占位与入口顺序。
- GL/D3D sharing：分别覆盖 acquire/release、同步、context destroy；当前未执行。
- SVM/managed memory：对照 M04 的 memobj/UVM 路径；当前只确认入口存在，不确认硬件行为。

## 修改配方

1. 新 OpenCL entry：同时修改 ICD table、vendor dispatch、版本宏和构建清单。
2. 新共享资源：同步 external handle、memobj ownership、acquire/release ordering 和 stream synchronization。
3. 修改 worker：检查 `NVCL_IDLE_TIMEOUT_MS`、DAG 空闲、回调线程和 context deinitialize。
4. 修改 platform selection：保持 properties、device type、首 vendor fallback 的优先级。
