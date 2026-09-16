# Demo 注册表

- 文档目的：登记 MUSA Driver 仓库中的真实 Demo/示例/工具候选，并选择后续端到端源码解剖主线。
- 适用范围：`src/tools`、`tests`、README/CI 中可运行入口。
- 对应源码版本：`b8dce2b2f23849e8e99350c20d7eaac3c129caba`。
- 证据状态：D01 已完成静态源码轨迹；D02/D03/D04 等仍为候选级
- 最后更新：2026-09-16
- 前置阅读：[`../00-overview/project-overview.md`](../00-overview/project-overview.md)
- 后续阅读：[`D01-muinfo/README.md`](D01-muinfo/README.md)、[`D02-memory-copy/README.md`](D02-memory-copy/README.md)、[`D03-graph/README.md`](D03-graph/README.md)

## 结论摘要

结论：首批主 Demo 仍选择 `src/tools/muInfo.cpp`。现在已确认它的真实路径是 `main -> muInit -> Platform/HAL/M3D 初始化 -> device count -> primary context retain/set current -> 属性批量缓存 -> peer/memory 输出`，能作为“初始化 + 设备枚举 + 属性查询 + primary context 边界”的最小源码轨迹。  
状态：已确认 + 运行未验证。  
证据：`muInfo` 主函数和属性采集见 [src/tools/muInfo.cpp:119-388]；构建目标由 `src/tools/CMakeLists.txt` 自动发现并链接 `${DRIVER_LIB_NAME}_dynamic` [src/tools/CMakeLists.txt:10-18]。

## Demo 候选表

| Demo ID | 入口 | 类型 | 覆盖模块 | 核心能力 | 可运行条件 | 当前可验证性 | 选择结论 |
|---|---|---|---|---|---|---|---|
| D01 | `src/tools/muInfo.cpp` | CLI 工具 | M10、M02、M03、M04 边界、M05 边界 | 初始化 driver、枚举设备、retain primary context、查询属性/peer/mem info 并输出 | 已构建 `libmusa.so`/`muInfo`，安装/LD_LIBRARY_PATH，目标机有驱动/设备 | 静态源码确认，未运行 | 主 Demo；已解剖 |
| D02 | `tests/memcpyBatchAsync.cu` | 功能示例/测试 | M02、M04、M05、M06、M09 | 异步批量 memcpy，覆盖 memory + stream + command | 需要 MUSA 编译器/运行时、GPU 设备 | 仅发现文件，未读源码 | 第二优先级 Demo |
| D03 | `tests/memcpy3DBatchAsync.cu` | 功能示例/测试 | M02、M04、M05、M06、M09 | 3D batch copy | 同上 | 仅发现文件 | 候选 |
| D04 | `tests/childGraph.cu` | Graph 示例/测试 | M02、M04、M08、M06、M09 | child graph 节点与 graph exec | 同上 | 仅发现文件 | Graph 深挖 Demo 候选 |
| D05 | `tests/conditionalNode.cu` | Graph 示例/测试 | M02、M04、M08、M06、M09 | conditional node | 同上 | 仅发现文件 | Graph 深挖 Demo 候选 |
| D06 | `tests/allocnode.cu` | Graph/memory 示例 | M02、M04、M05、M08 | graph memory alloc node | 同上 | 仅发现文件 | 候选 |
| D07 | `tests/addnode_v2.cu` | Graph 示例 | M02、M08 | graph add node | 同上 | 仅发现文件 | 候选 |

## 主 Demo 选择依据

| 选择标准 | `muInfo` 情况 | 状态 |
|---|---|---|
| 使用真实公共入口 | 是，作为 tools 目标构建和安装 | 已确认 |
| 覆盖核心模块 | 覆盖 API/Platform/Device，并通过 primary context 和 mem info 触及 Context/Memory 边界 | 已确认 |
| 输入/处理/输出完整 | 无 CLI 参数；输出 compiler、设备属性、peer、memInfo | 已确认 |
| 构建方式明确 | tools CMake 自动构建并链接 driver | 已确认 |
| 环境依赖最少 | 相比 kernel/graph 示例不需要编译 `.cu` 输入；仍需 driver/设备 | 推断 |
| 可观察结果 | stdout/设备属性/错误 stderr | 已确认源码，运行未验证 |

结论：D01 适合作为“最小入门 Demo”；D02 或 D04 仍需作为后续“完整主流程 Demo”补充，因为它们覆盖 memory/stream/command/graph 等异步执行路径。

## Demo—模块覆盖矩阵

| Demo | M01 构建 | M02 API | M03 平台设备 | M04 Context | M05 Memory | M06 Stream | M07 Kernel | M08 Graph | M09 HAL/M3D | M10 工具 |
|---|---|---|---|---|---|---|---|---|---|---|
| D01 `muInfo` | 是 | 是 | 是 | 边界 | 边界 | 否 | 否 | 否 | 初始化边界 | 是 |
| D02 memcpy batch | 是 | 是 | 是 | 是 | 是 | 是 | 否 | 否 | 是 | 否 |
| D04 childGraph | 是 | 是 | 是 | 是 | 可能 | 是 | 可能 | 是 | 是 | 否 |
| D05 conditionalNode | 是 | 是 | 是 | 是 | 可能 | 是 | 可能 | 是 | 是 | 否 |

## D01 已确认执行阶段

```text
D01-S001 main() 打印 compiler                         [src/tools/muInfo.cpp:372-378]
D01-S002 muInit(0) -> muapiInit -> Platform::Init      [src/driver/mu_context.cpp:121-133]
D01-S003 HAL/M3D platform/device 枚举                  [src/hal/m3d/platform.cpp:102-189]
D01-S004 muDeviceGetCount                              [src/driver/mu_device.cpp:40-51]
D01-S005 每个设备 retain primary context 并 set current [src/driver/mu_context.cpp:472-488,280-295]
D01-S006 GetDeviceProps 静态缓存批量查询属性            [src/tools/muInfo.cpp:119-274]
D01-S007 printDeviceProp 输出属性、peer、memInfo        [src/tools/muInfo.cpp:283-370]
```

详见 [`D01-muinfo/execution-trace.md`](D01-muinfo/execution-trace.md)。

## 相关文档

- [`../README.md`](../README.md)
- [`../00-overview/architecture.md`](../00-overview/architecture.md)
- [`../01-modules/module-registry.md`](../01-modules/module-registry.md)
- [`../90-cross-module/cross-module-call-chains.md`](../90-cross-module/cross-module-call-chains.md)

## 源码证据摘要

- tools 目标构建：[src/tools/CMakeLists.txt:10-18]
- `muInfo` 主函数：[src/tools/muInfo.cpp:372-388]
- 属性缓存：[src/tools/muInfo.cpp:119-274]
- 初始化：[src/driver/mu_context.cpp:121-133], [src/musa/core/platform.cpp:84-138]
- M3D platform/device 边界：[src/hal/m3d/lib.cpp:6-14], [src/hal/m3d/platform.cpp:102-189]

## 未解决问题

1. 尚未运行 `muInfo`，无真实 stdout、设备数量、错误码样本。
2. 尚未确认 `tests/*.cu` 是否有独立构建入口或仅依赖外部 MUSA 编译流程。
3. D01 不覆盖 stream/kernel/graph 提交；后续必须补 D02/D04。

## 下一步阅读建议

先读 D01 `execution-trace.md`，再读 `90-cross-module/cross-module-call-chains.md` 中的 MEM-001 和 KERNEL-001，为 D02/D04 深挖做准备。
