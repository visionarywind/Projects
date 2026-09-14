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

## 重要约束

工具回调与主路径共享 context、stream、module、function 和 channel 句柄。callback 本身不应被当作异步完成通知；真正的 GPU 完成仍由 marker/同步路径决定。debugger 可强制 blocking launch，memcheck 可为每次 launch 准备额外 device 结构，profiler 可禁用并发或插入 perfmon 操作。
