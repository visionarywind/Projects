# M09 源码地图、测试与开发配方

- 文档目的：解释 01-modules/M09-opencl-interop/source-map.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M09-opencl-interop/source-map.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


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
