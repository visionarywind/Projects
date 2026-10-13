# D01：muInfo 设备信息查询

- 入口：`src/tools/muInfo.cpp`
- 构建：`src/tools/CMakeLists.txt` 自动为 `.cpp` 生成可执行文件并链接 driver 动态库
- 文档目的：80-demos：总览：职责、结构、关键结论与文档导航。
- 适用范围：80-demos《D01：muInfo 设备信息查询》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：源码静态轨迹已确认到 Driver API、Core Platform/Device、HAL/M3D platform 边界；目标机输出、设备数量和运行耗时未验证
- 最后更新：2026-10-08
- 前置阅读：[`../demo-registry.md`](../demo-registry.md)、[`../../00-overview/project-overview.md`](../../00-overview/project-overview.md)
- 后续阅读：[`../../01-modules/module-registry.md`](../../01-modules/module-registry.md)、[`../../99-roadmap/risk-register.md`](../../99-roadmap/risk-register.md)

## 目标

用最短真实入口观察：

```text
muInfo main
  -> muInit
  -> Platform/HAL/M3D 初始化和设备枚举
  -> muDeviceGetCount
  -> muDevicePrimaryCtxRetain + muCtxSetCurrent
  -> muDeviceGet / muDeviceGetName / muDeviceTotalMem / muDeviceGetAttribute / muDeviceGetUuid
  -> peer 查询 + mem info
  -> stdout
```

它覆盖 M10、M02、M03，并通过 primary context 和 `muMemGetInfo` 触及 M04/M05 的边界；它不能代表 stream、kernel 或 graph 的完整异步执行行为。

## 关键结论

1. `muInfo` 不只是“打印工具”：`main` 会显式 `muInit(0)`，然后对每个设备 retain primary context 并设置为当前 context。[src/tools/muInfo.cpp:372-385]
2. 属性采集由 `GetDeviceProps` 的局部静态 vector 缓存完成，首次调用会批量调用大量 Driver API；后续返回缓存项。[src/tools/muInfo.cpp:119-274]
3. 普通属性查询多走 `Platform::GetIDeviceView`，不触发 `Device::LaterInit`；primary context retain 走 `Platform::GetDevice`，会触发 `Device::LaterInit`。[src/driver/mu_device.cpp:71-94; src/driver/mu_context.cpp:472-488; src/musa/core/platform.cpp:140-147]
4. `muMemGetInfo_v2` 依赖当前 TLS context；因此 `muCtxSetCurrent(ctx)` 是 `printDeviceProp` 后半段内存信息输出的前置条件。[src/tools/muInfo.cpp:381-384; src/driver/mu_memory.cpp:542-560]

## 关键代码位置

| 位置 | 观察 | 行级证据 |
|---|---|---|
| 错误检查宏 | `__checkMusaErrors` 收到非成功 `MUresult` 就打印文件/行号并 `exit(EXIT_FAILURE)`，首个错误即终止进程 | [src/tools/muInfo.cpp:9-20] |
| 编译器标识 | `printCompilerInfo` 在 `muInit` 之前先输出 `compiler: mcc` | [src/tools/muInfo.cpp:276-278] |
| 设备循环 | 每个 ordinal 都先 `muDevicePrimaryCtxRetain`、`muCtxSetCurrent`，然后才 `printDeviceProp` | [src/tools/muInfo.cpp:380-385] |
| peer 矩阵 | peers 与 non-peers 两段各扫一遍设备，逐对调用 `muDeviceCanAccessPeer` | [src/tools/muInfo.cpp:341-359] |
| 属性缓存 | `GetDeviceProps` 的局部静态 `devProps` 由 lambda 一次性采集 | [src/tools/muInfo.cpp:119-124] |
| 工具目标 | `file(GLOB_RECURSE ...)` 收集 `.cpp`，对每个文件 `add_executable` 并链接 `${DRIVER_LIB_NAME}_dynamic` | [src/tools/CMakeLists.txt:10-15] |
| 子目录挂载 | `src/CMakeLists.txt` 末尾 `add_subdirectory` 把 `tools` 目录挂进构建 | [src/CMakeLists.txt:15] |

## 文档导航

- [`execution-trace.md`](execution-trace.md)：逐阶段源码调用链，含 HAL/M3D 初始化边界。
- [`data-and-state-trace.md`](data-and-state-trace.md)：字段来源、缓存、句柄和状态表。
- [`debug-walkthrough.md`](debug-walkthrough.md)：推荐断点和变量观察点。
- [`failure-paths.md`](failure-paths.md)：失败分类、错误传播和诊断顺序。
- [`build-and-run.md`](build-and-run.md)：构建/运行命令候选，均标注未验证。
- [`modification-exercises.md`](modification-exercises.md)：适合从工具层入手的练习。

## 验证边界

当前只进行了远端源码读取，没有在目标机执行 `cmake`、`make`、`muInfo` 或 gdb。任何设备型号、内存容量、peer 矩阵和性能数据都必须等目标环境运行后补充。

## 相关文档

- [`../../00-overview/analysis-state.md`](../../00-overview/analysis-state.md)
- [`../../01-modules/module-registry.md`](../../01-modules/module-registry.md)

## 源码证据摘要

- `src/tools/muInfo.cpp`（7 处，例：[src/tools/muInfo.cpp:372-385] `main`）
- `src/tools/CMakeLists.txt`（2 处，例：[src/tools/CMakeLists.txt:10-18] `target_link_libraries`）
- `src/CMakeLists.txt`（1 处，例：[src/CMakeLists.txt:15] `add_subdirectory`）
- 其它分段引用：`src/driver/mu_device.cpp`、`src/driver/mu_context.cpp`、`src/musa/core/platform.cpp`、`src/driver/mu_memory.cpp`（各见正文多路径括号）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 风险与未决项见知识库风险登记。

## 下一步阅读建议

阅读 [`../../00-overview/analysis-state.md`](../../00-overview/analysis-state.md)；阅读 [`../../01-modules/module-registry.md`](../../01-modules/module-registry.md)。
