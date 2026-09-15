# M08 源码地图、测试与开发配方

## 源码地图

| 区域 | 主要文件 | 作用 |
|---|---|---|
| Tools callback | `src/etbl/tools/tools_callbacks_internal.h` | enable 检查、参数翻译、dispatch 接口 |
| Launch hooks | `src/cui/cuilaunch.c` | launch 生命周期 callback 与 profiler/debugger 插入 |
| Debugger | `src/devtools/debugger/cudbgapi.c`, `cudbgdriver.c` | attach、状态检查、module/context/grid 映射 |
| Memcheck | `src/devtools/memcheck/memcheck.c`, `check_mc_*.c` | patch、allocation table、错误报告 |
| Profiler | `src/profiler/profiler_common.c`, `perfmon_common.c` | profile mode、perfmon 和信号配置 |
| DRS | `src/drs/*`, `src/drs/rkygen/*` | profile/config 生成；构建规则使用 autogen |

## 测试策略

1. 无工具运行 baseline：检查 launch、stream、memory 的结果。
2. 开启 tools callback：确认 callback 时序和 callback 参数生命周期。
3. debugger：验证 attach 前后 `cudbgApiCheck` 的 init/TLS/device/suspended 条件。
4. memcheck：验证 error entry、allocation table HtoD copy 和 launch failure cleanup。
5. profiler：分别验证 profile mode 兼容、blocking/concurrency 和 PM trigger。

当前没有执行 GPU、debugger client、memcheck 或 profiler 验证。DRS 生成依赖外部 `nvGenerateRules.nvmk`（静态确认：[src/drs/makefile.nvmk:1-8]）。

## 修改配方

- 新 callback：更新 public tools callback ABI、enable table、参数结构、调用点和 begin/end 配对。
- 修改 launch hook：保留错误路径的 `LAUNCH_END` 与 `streamEndPush` 约束。
- 修改 debugger check：同步 flags 依赖闭包，不能只新增独立条件。
- 修改 memcheck patch：同步 host/device allocation、stream copy、context destroy 和架构 interop。
- 修改 profiler：检查 mutex、并发 launch 策略、HAL perfmon 以及 blocking 语义。
