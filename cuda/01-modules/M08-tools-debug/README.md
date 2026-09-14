# M08 Tools/Debug：工具、调试、Profiler 与 Memcheck

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
