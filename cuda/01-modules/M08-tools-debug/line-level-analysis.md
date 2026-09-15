# M08 行级分析

- 文档目的：解释 01-modules/M08-tools-debug/line-level-analysis.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M08-tools-debug/line-level-analysis.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


- `[src/etbl/tools/tools_callbacks_internal.h:33-41]`：callback enable 快速路径与统一 dispatch。
- `[src/cui/cuilaunch.c:468-503]`：launch begin 后 tools 可跳过内部 launch。
- `[src/cui/cuilaunch.c:582-730]`：pushbuffer/HAL 前后 callback 和 perfmon 插入点。
- `[src/cui/cuilaunch.c:779-817]`：错误回滚、launch end 和 profiler 收尾。
- `[src/devtools/memcheck/memcheck.c:120-256]`：error entry、allocation table、host/device 镜像拷贝及失败释放。
- `[src/devtools/debugger/cudbgapi.c:401-557]`：module/context/function/grid 查询前的状态检查与 TLS callback inactive 闸门。
- `[src/devtools/debugger/cudbgdriver.c:103-210]`：attach/session、launch blocking、integrated memcheck、preemption debugging 的共享控制状态。
- `[src/profiler/profiler_common.c:21-56,85-96]`：profile mode mutex、兼容检查和 `CUPTI_PROFILE_MODE`。
- `[src/cui/cuilaunch.c:635-710]`：wait-for-idle、perfmon begin/end、PM trigger 及 counter 操作所在 push 区间。

这些行号支持 host-side 生命周期和错误分支；RM、设备 helper、客户端握手以及最终计数器结果仍是未知/未验证。

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
