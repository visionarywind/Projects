# M08 Tools/Debug：工具、调试、Profiler 与 Memcheck

- 文档目的：解释 01-modules/M08-tools-debug/README.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M08-tools-debug/README.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


- **职责**：通过 Tools callback、debugger attach、integrated memcheck、profiler/perfmon 和 DRS 配置观察或改变 driver 的执行行为。
- **边界**：M08 不是 CUDA API 主资源管理器；它通过 callback、patch、同步请求和错误报告插入 M02/M04/M05/M06 生命周期。
- **证据等级**：callback enable/dispatch、launch callback 时序、debugger 状态检查、memcheck device table 和 profiler 全局模式已静态确认；外部工具客户端和 RM 端协议未验证。

## 入口地图

```text
launch/memory/context
 → toolsCallbackEnabled
 → toolsIssueCallback
 → debugger / profiler / memcheck callback
 → 可选 patch、同步、报告或跳过内部 launch
```

M06 的 launch callback 明确提供 `LAUNCH_BEGIN`、syscall setup、function setup、pushbuffer begin/end、launch pushed、grid launched、launch end 等观察点；工具在 `LAUNCH_BEGIN` 可通过 `pSkipInternalLaunch` 请求跳过内部 launch（静态确认：[src/cui/cuilaunch.c:468-503,899-1174]）。

## 生命周期补证

- **静态确认**：callback enable 表按 domain/cbid 快速短路，启用后才进入统一 dispatch；因此“没有工具监听”是主路径的显式快速路径，而不是缺少 callback 实现（[src/etbl/tools/tools_callbacks_internal.h:22-41]）。
- **静态确认**：launch begin 传入的 `pSkipInternalLaunch` 是可写控制字段。工具可以跳过 driver 内部提交，但错误/退出路径仍需执行 launch end、UVM running 回滚和 push 资源清理，不能把 skip 当作同步完成（[src/cui/cuilaunch.c:468-503,779-817]）。
- **静态确认**：debugger API 在读写 module/function/grid 或 device 状态前检查 initialized、TLS callback inactive、context/device/suspended/SM/warp/lane 等前置状态；共享控制变量中的 session、blocking、integrated memcheck 和 preemption 标志构成 attach 协议的一部分（[src/devtools/debugger/cudbgapi.c:401-557]；[src/devtools/debugger/cudbgdriver.c:103-210]）。
- **静态确认**：memcheck 的 error entry、device allocation table 与 host allocation mirror 属于额外的 device-visible 资源；表内容通过 HtoD 传输后才供设备侧使用，分配或拷贝失败分别走 host/device memory error（[src/devtools/memcheck/memcheck.c:120-180,206-256]）。
- **静态确认**：profiler mode 由全局 mutex 保护，`CUPTI_PROFILE_MODE` 可参与初始模式选择；launch push 区间还包含 wait-for-idle、perfmon begin/end 与 counter trigger，因此 profiler 会改变提交时序（[src/profiler/profiler_common.c:21-56,85-96]；[src/cui/cuilaunch.c:635-710]）。

上述结论仍只说明 host-side 控制流；外部 debugger client、RM 端协议、memcheck device helper 的实际执行和 profiler backend 均未验证。

## 文档元数据（规范补充）

- 文档目的：说明 `01-modules/M08-tools-debug/README.md` 的源码分析范围、结论和维护入口。
- 适用范围：当前项目对应模块/入口的静态源码与测试分析。
- 对应源码版本：以本项目 `00-overview/analysis-state.md` 或同页版本字段为准。
- 证据状态：静态源码证据；未执行的构建、测试、GPU、网络或多进程行为保持“未验证”。
- 最后更新：2026-09-15
- 前置阅读：本项目根 README 与 `00-overview/analysis-state.md`。
- 后续阅读：本模块/示例的实现、测试和风险页面。

## 深度审计

| 分析对象 | 入口落地 | 正常路径 | 分支 | 异常 | 清理 | 数据生命周期 | 执行上下文 | 行级证据 | Demo 映射 | 状态/缺口 |
|---|---|---|---|---|---|---|---|---|---|---|
| `01-modules/M08-tools-debug` | 已完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 已映射或不适用 | 部分完成：动态行为、边界或专用变体仍需验证 |

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
