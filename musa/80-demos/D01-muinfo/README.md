# D01：muInfo 设备信息查询

- 入口：`src/tools/muInfo.cpp`
- 构建：`src/tools/CMakeLists.txt` 自动为 `.cpp` 生成可执行文件并链接 driver 动态库
- 对应源码版本：`b8dce2b2f23849e8e99350c20d7eaac3c129caba`
- 证据状态：源码静态轨迹已确认到 Driver API、Core Platform/Device、HAL/M3D platform 边界；目标机输出、设备数量和运行耗时未验证

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

1. `muInfo` 不只是“打印工具”：它会显式 `muInit(0)`，然后对每个设备 retain primary context 并设置为当前 context。[src/tools/muInfo.cpp:372-385]
2. 属性采集由 `GetDeviceProps` 的局部静态 vector 缓存完成，首次调用会批量调用大量 Driver API；后续返回缓存项。[src/tools/muInfo.cpp:119-274]
3. 普通属性查询多走 `Platform::GetIDeviceView`，不触发 `Device::LaterInit`；primary context retain 走 `Platform::GetDevice`，会触发 `Device::LaterInit`。[src/driver/mu_device.cpp:71-94; src/driver/mu_context.cpp:472-488; src/musa/core/platform.cpp:140-147]
4. `muMemGetInfo_v2` 依赖当前 TLS context；因此 `muCtxSetCurrent(ctx)` 是 `printDeviceProp` 后半段内存信息输出的前置条件。[src/tools/muInfo.cpp:381-384; src/driver/mu_memory.cpp:542-560]

## 文档导航

- [`execution-trace.md`](execution-trace.md)：逐阶段源码调用链，含 HAL/M3D 初始化边界。
- [`data-and-state-trace.md`](data-and-state-trace.md)：字段来源、缓存、句柄和状态表。
- [`debug-walkthrough.md`](debug-walkthrough.md)：推荐断点和变量观察点。
- [`failure-paths.md`](failure-paths.md)：失败分类、错误传播和诊断顺序。
- [`build-and-run.md`](build-and-run.md)：构建/运行命令候选，均标注未验证。
- [`modification-exercises.md`](modification-exercises.md)：适合从工具层入手的练习。

## 验证边界

当前只进行了远端源码读取，没有在目标机执行 `cmake`、`make`、`muInfo` 或 gdb。任何设备型号、内存容量、peer 矩阵和性能数据都必须等目标环境运行后补充。
