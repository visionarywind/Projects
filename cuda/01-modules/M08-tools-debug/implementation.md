# M08 设计与实现

- 文档目的：解释 01-modules/M08-tools-debug/implementation.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M08-tools-debug/implementation.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## Callback 设计

`toolsCallbackEnabled` 读取每个 domain/cbid 的 volatile enable flag，关闭时避免进入通用 dispatch；打开后由 `toolsIssueCallback` 广播给已注册 callback（静态确认：[src/etbl/tools/tools_callbacks_internal.h:22-41]）。driver 用私有结构翻译 memory flags 为稳定的 tools descriptor，避免 tools 直接依赖内部 `nvtypes`（静态确认：[src/etbl/tools/tools_callbacks_internal.h:45-219]）。

## Launch 插入点

`cuiLaunch` 在 launch begin 后允许工具跳过内部提交；普通情况下先做 syscall/HAL/QMD/setup，再进入 pushbuffer begin、HAL finalize/control、pushbuffer end 和 stream end push；最终始终调用 launch end（静态确认：[src/cui/cuilaunch.c:468-503,582-624,666-730,779-817]）。这形成严格的 begin/end 配对契约，即使中途失败也需经过 `EndPushAndExit`/`Exit`。

## Debugger

debugger API 维护 module/context/function/grid 的映射；其 API check 将依赖标志逐级展开：device、status、suspended、SM、warp、lane、TLS callback 和 initialized 检查按层叠加（静态确认：[src/devtools/debugger/cudbgapi.c:401-557]）。attach 相关共享变量同时承担 debugger 与应用进程之间的控制协议，例如 launch blocking、integrated memcheck、preemption debugging 和 session id（静态确认：[src/devtools/debugger/cudbgdriver.c:103-198]）。

## Memcheck

memcheck 为 error entry 分配 pinned host/device 可见内存；全局 allocation table 在 host 填充后通过 inline HtoD copy 下载到 device，失败分别映射为 host/device memory 错误（静态确认：[src/devtools/memcheck/memcheck.c:120-256]）。因此 memcheck 的资源也有 context、stream 和 device memory 生命周期，不能只看成日志功能。

## Profiler

全局 profiler mode 由 mutex 保护；profile mode 允许“同模式或 none”兼容，初始化可读取 `CUPTI_PROFILE_MODE`（静态确认：[src/profiler/profiler_common.c:21-56,85-96]）。launch 中的 perfmon begin/end、wait-for-idle、PM trigger 和 counter start/stop 位于 HAL push 期间（静态确认：[src/cui/cuilaunch.c:635-710]）。

## 资源回滚与销毁

工具资源不是独立于 context 的日志缓存。memcheck 为 error entry 和 allocation table 建立 host/device 对，先在 host 填充，再通过 inline HtoD copy 同步 device 镜像；任一阶段失败都要释放已建立的 entry/table 并返回对应内存错误（静态确认：[src/devtools/memcheck/memcheck.c:120-180,206-256]）。因此 context destroy、allocation teardown 和 launch failure 必须检查工具侧仍持有的 device-visible 资源。

debugger 的 shared control variables 连接 attach/session 与 launch blocking 等协议状态；API check 通过 TLS callback inactive 条件防止 callback 重入。静态源码能确认状态闸门和字段用途，但不能确认另一进程何时更新这些变量、RPC/IPC 如何完成握手（静态确认：[src/devtools/debugger/cudbgapi.c:401-557]；[src/devtools/debugger/cudbgdriver.c:103-210]）。

profiler 的全局模式与 launch 内 perfmon 操作由不同层次管理：前者决定是否接受 profile client，后者插入具体提交窗口。故 callback end 或 host API 返回不等价于 counter 已停止，最终完成仍依赖 stream/marker 路径；这部分设备端时序未知。

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
