# 修改影响图

- 文档目的：解释 90-cross-module/change-impact-map.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 90-cross-module/change-impact-map.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


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

## 相关文档
- [项目入口](../README.md)
- [分析状态](../00-overview/analysis-state.md)
- [源码证据索引](../00-overview/evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
