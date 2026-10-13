# MUSA Driver 知识库

- 文档目的：作为 `source/musa` 的源码理解总入口，串联总览、模块、Demo、跨模块和开发实践文档。
- 适用范围：MUSA Driver / MUSA Runtime driver API 用户态库源码；已生成总览、模块、Demo、跨模块和开发实践层，仍明确标注源码/硬件验证边界。
- 对应源码版本：远端 `HEAD`，提交 `5.3.0 develop`，CMake 版本 `5.3.0 develop`。
- 证据状态：静态源码分析已扩展；构建与运行未在当前环境执行。
- 最后更新：2026-10-07
- 前置阅读：无
- 后续阅读：[`00-overview/architecture.md`](00-overview/architecture.md)、[`01-modules/module-registry.md`](01-modules/module-registry.md)、[`00-overview/analysis-state.md`](00-overview/analysis-state.md)

## 1. 项目一句话介绍

MUSA Driver 是摩尔线程 MUSA 软件栈中的用户态 Driver API 库：对外导出 `libmusa.so` 和工具 `muInfo`，对内把 `muapi*` Driver API 调用转成 `Musa::Platform`、`Musa::Device`、`Musa::Context`、`Musa::Memory`、`Musa::Stream`、`Musa::Command` 等核心对象操作，再通过 HAL/M3D 层访问底层 GPU 驱动与设备资源。

状态：已确认 + 推断。  
证据：README 将项目标为 `MUSA-Runtime` 并列出 `src/driver`、`src/musa`、`src/hal` 目录职责 [README.md:51-64]；顶层 CMake 设置项目名 `musa_driver` 和版本 [CMakeLists.txt:101-103]；Linux 下库名为 `musa` [src/CMakeLists.txt:1-5]，driver 目录构建共享库 [src/driver/CMakeLists.txt:9-18]。

## 2. 5 分钟快速理解

```mermaid
flowchart LR
    A[应用/工具<br/>muInfo 或 MUSA Driver API 调用方] --> B[Driver API 导出层<br/>src/driver/mu_*.cpp]
    B --> C[核心对象层<br/>src/musa/core]
    C --> D[HAL 接口层<br/>src/hal/*.h]
    D --> E[M3D 适配层<br/>src/hal/m3d]
    E --> F[M3D 子模块/内核驱动/设备]
```

核心结论：

1. **构建主产物是 `libmusa.so`**：`src/driver/CMakeLists.txt` 用 `add_library(${DRIVER_LIB_NAME}_dynamic SHARED ...)` 构建动态库，并链接静态库 `musaCore` [src/driver/CMakeLists.txt:9-18]。
2. **核心对象和 HAL 是静态链接进 driver 库的内部实现**：`musaCore` 汇总 `src/musa/core`、`command`、`node`、`graph`、`copyManager2`，并链接 `halM3d` [src/musa/core/CMakeLists.txt:1-11]；`halM3d` 再链接 `m3d`、`scpc`、`util` [src/hal/m3d/CMakeLists.txt:36-59]。
3. **初始化入口是 `muapiInit`**：Driver API 的 `muapiInit` 调用 `Musa::Platform::Get().Init()` [src/driver/mu_context.cpp:121-133]，平台初始化再发现 HAL 设备、重排设备、读取可见设备并创建 `Musa::Device` [src/musa/core/platform.cpp:84-140]。
4. **上下文是资源归属中心**：`Musa::Context` 负责创建/销毁 memory、stream、event、module、graph 等对象 [src/musa/core/context.cpp:1037-1099]、[src/musa/core/context.cpp:1207-1247]。
5. **内存与执行路径最终落到 HAL/M3D**：例如 `muapiMemAlloc_v2` 通过当前上下文创建内存 [src/driver/mu_memory.cpp:271-320]；`Musa::Memory::GeneralAlloc` 调用 HAL device 分配 [src/musa/core/memory.cpp:470-520]。

## 3. 当前分析对应源码版本

| 项 | 值 | 状态 |
|---|---|---|
| 分析基准源码 | `source/musa`（本地快照，5.3.0 develop） | 已确认 |
| Git 分支 | `HEAD` detached/当前 HEAD 名称输出为 `HEAD` | 已确认 |
| Git 提交 | `5.3.0 develop` | 已确认 |
| 最近提交 | `Merge [SW-71661] Add PHOTON_BVH_PRE_COMPACT_TRI3 and update m3d` | 已确认 |
| CMake 版本 | `5.3.0 develop` | 已确认 |
| 本地文档目录 | `Projects/musa` | 已确认 |

## 4. 知识库目录树

```text
musa/
├── README.md
├── 00-overview/
│   ├── project-overview.md
│   ├── architecture.md
│   ├── design-principles.md
│   ├── runtime-model.md
│   ├── global-data-flow.md
│   ├── dependency-map.md
│   ├── global-error-model.md
│   ├── glossary.md
│   ├── decision-log.md
│   ├── build-and-deploy.md
│   ├── evidence-index.md
│   └── analysis-state.md
├── 01-modules/
│   ├── module-registry.md
│   └── M01..M10-* / （每个模块含 README、实现、调用链、测试、风险等）
├── 80-demos/
│   ├── demo-registry.md
│   ├── D01-muinfo/
│   ├── D02-memory-copy/
│   └── D03-graph/
├── 90-cross-module/
│   ├── system-wiring.md
│   ├── interface-contracts.md
│   ├── runtime-trace.md
│   ├── end-to-end-flows.md
│   ├── cross-module-call-chains.md
│   ├── shared-data-and-types.md
│   ├── configuration-impact-map.md
│   ├── error-boundaries.md
│   ├── change-impact-map.md
│   └── performance-critical-paths.md
└── 99-roadmap/
    ├── quick-start.md
    ├── reading-guide.md
    ├── debugging-guide.md
    ├── feature-development-recipes.md
    ├── testing-recipes.md
    ├── performance-guide.md
    ├── risk-register.md
    ├── technical-debt.md
    └── next-steps.md
```

说明：总览层、M01-M10 模块层、D01-D03 Demo 层、跨模块层和实践路线均已生成。源码深链、目标机硬件行为和构建测试结果仍按证据状态分别标注，不将未执行内容写成已验证事实。

## 5. 模块摘要表

| 模块 ID | 模块名称 | 一句话职责 | 核心证据 | 当前状态 |
|---|---|---|---|---|
| M01 | 构建/发布/安装系统 | 定义版本、编译选项、产物、安装路径和 CI 包装 | [CMakeLists.txt:21-31], [src/driver/CMakeLists.txt:9-64], [install.sh:27-105] | 部分完成 |
| M02 | Driver API 导出层 | 提供 `muapi*` 入口、参数校验、错误转换和 API 表导出 | [src/driver/mu_context.cpp:121-220], [src/driver/mu_memory.cpp:271-320], [src/driver/mu_entry.cpp:1241-1245] | 部分完成 |
| M03 | 平台与设备管理 | 初始化 HAL、发现设备、维护全局对象注册表和设备视图 | [src/musa/core/platform.cpp:84-140], [src/musa/core/device.cpp:593-672] | 部分完成 |
| M04 | 上下文与资源生命周期 | 作为设备资源归属中心，创建/销毁 stream、memory、event、module、graph 等对象 | [src/musa/core/context.cpp:1037-1099], [src/musa/core/context.cpp:1207-1247] | 部分完成 |
| M05 | 内存与内存池 | 实现 device/host/managed/virtual/pool memory 的创建、映射、释放；HAL pool 负责 chunk/segment/bucket/merge/trim | [src/driver/mu_memory.cpp:271-449,716-755], [src/driver/mu_mempool.cpp:1-515], [src/hal/m3d/memoryPool.cpp:85-555] | 显存池专题已静态深挖 |
| M06 | Stream/Command 调度 | 将同步/异步操作组织成 Command 并提交到队列 | [src/musa/core/stream.cpp:237-260], [src/musa/core/stream.cpp:429-475] | 部分完成 |
| M07 | Module/Kernel 执行 | 加载模块、解析函数并发起 kernel launch | [src/driver/mu_module.cpp:88-130], [src/driver/mu_module.cpp:231-269] | 部分完成 |
| M08 | Graph 执行 | 构造 graph/node/graphExec 并支持 capture/launch | [src/musa/core/context.cpp:1398-1456], [src/musa/core/graph.cpp] | 待深入 |
| M09 | HAL/M3D 适配 | 将核心对象操作映射为 M3D device、queue、memory、cmdBuffer 操作 | [src/hal/m3d/CMakeLists.txt:1-59] | 部分完成 |
| M10 | 工具与调试接口 | 提供 `muInfo`、GDB/MUPTI/MUASAN hook 和调试/观测接口 | [src/tools/CMakeLists.txt:10-18], [src/driver/CMakeLists.txt:4-11] | 部分完成 |

详见 [`01-modules/module-registry.md`](01-modules/module-registry.md)。

## 6. 三条最重要端到端流程（首批静态确认版）

1. **初始化与设备发现**  
   `muapiInit` → `Musa::Platform::Get().Init()` → HAL platform init / device discovery → `Musa::Device` 创建。  
   状态：部分完成；后续需补 `Hal::Platform` 与 `m3d::Device` 具体实现。

2. **设备内存分配/释放**  
   普通路径：`muapiMemAlloc_v2` → `Context::CreateMemory` → `Memory::GeneralAlloc` → `MemMgr::Allocate` → key 查找/创建自动 pool → `MemoryPool::FullAllocate` → `SubAllocate` 或 `ChunkAllocate` → `ResourceSplit` → 返回 chunk + offset；释放反向经过 tracker、`Memory::~Memory`、精确 range 查找、左右 merge 和完整 chunk reuse/destroy。  
   async/graph 路径：先保留 virtual range，再按 stream callback 或 graph host-device submission 创建/销毁 physical mapping。状态：Core/HAL/M3D 适配边界已静态确认；M3D 子模块和硬件未验证。

3. **Kernel 启动**  
   `muapiModuleLoad*` → `Musa::Module`/`Musa::Function` → `muapiLaunchKernel` → stream 解析 → dispatch command → queue/cmdBuffer 提交。  
   状态：部分完成；后续需补 `DispatchCommand` 与 `halM3d::Queue` 提交细节。

## 7. 主 Demo 入口

首批选择候选：

- `src/tools/muInfo.cpp`：真实工具入口，构建目标由 `src/tools/CMakeLists.txt` 自动发现并链接 `libmusa.so`，适合做“初始化 + 设备枚举 + 属性查询”的主 Demo。
- `tests/*.cu`：上游示例（graph、memcpy batch、conditional node 等）可作为后续 Demo；**本快照无 `tests/` 目录**，仅作候选说明。

详见 [`80-demos/demo-registry.md`](80-demos/demo-registry.md)。

## 8. 构建和运行入口

- 最小构建（来自 README，当前未执行）：
  ```bash
  mkdir build
  cd build
  cmake ..
  make
  ```
- Debug 构建（来自 README，当前未执行）：
  ```bash
  mkdir build
  cd build
  cmake .. -DMUSA_BUILD_DEBUG=ON
  make
  ```
- 单元测试构建开关：`-DMUSA_BUILD_UT=ON`，顶层 CMake 会拉取 googletest [CMakeLists.txt:146-154] 并 `add_subdirectory(unittest)` [CMakeLists.txt:167-169]（本快照缺该目录）。
- 安装脚本：`./install.sh`，会复制 `build/lib/libmusa*` 到系统库目录，并复制 `muInfo` 到 bin 目录 [install.sh:27-105]。

详见 [`00-overview/build-and-deploy.md`](00-overview/build-and-deploy.md)。

## 9. 按角色推荐阅读路径

- **30 分钟快速了解**：本 README → [`00-overview/project-overview.md`](00-overview/project-overview.md) → [`00-overview/architecture.md`](00-overview/architecture.md)。
- **Driver API 开发者**：模块注册表 M02/M04/M05/M06 → 后续 `M02-driver-api`、`M04-context-lifecycle`、`M05-memory`。
- **GPU 运行时/资源管理开发者**：M03/M04/M05/M09 → `system-wiring.md`；M05 专题阅读顺序为 `design.md` → `data-structures.md` → `implementation.md` → `call-chains.md` → `execution-flows.md`。
- **构建发布维护者**：M01 → [`00-overview/build-and-deploy.md`](00-overview/build-and-deploy.md)。
- **新入职开发者**：[`99-roadmap/quick-start.md`](99-roadmap/quick-start.md) → `muInfo` Demo → 内存分配调用链。

## 10. 文档状态、覆盖范围和未解决问题

M05 显存池专题已完成一轮函数级静态深挖；全库仍有 Graph、wrapper/export、M3D 子模块和硬件验证缺口。专题还覆盖 IPC pool metadata、SplayTree registry 与 pool ownership 的静态审计；其中 `mmap`、跨进程 owners、live allocation destroy 和 lookup 比较方向均未通过运行验证。详见 [`00-overview/analysis-state.md`](00-overview/analysis-state.md) 和 [`01-modules/M05-memory-mempool/risks-and-debt.md`](01-modules/M05-memory-mempool/risks-and-debt.md)。


## 11. 全量文档索引

> 覆盖本知识库全部文档（每条：路径链接 — H1 标题）。

### 00-overview/（总览，12 篇）

- [`00-overview/analysis-state.md`](00-overview/analysis-state.md) — 分析状态
- [`00-overview/architecture.md`](00-overview/architecture.md) — 总体架构
- [`00-overview/build-and-deploy.md`](00-overview/build-and-deploy.md) — 构建、运行、测试和部署总览
- [`00-overview/decision-log.md`](00-overview/decision-log.md) — 设计决策与验证记录
- [`00-overview/dependency-map.md`](00-overview/dependency-map.md) — 依赖地图
- [`00-overview/design-principles.md`](00-overview/design-principles.md) — 设计原则
- [`00-overview/evidence-index.md`](00-overview/evidence-index.md) — 源码证据索引
- [`00-overview/global-data-flow.md`](00-overview/global-data-flow.md) — 全局数据流
- [`00-overview/global-error-model.md`](00-overview/global-error-model.md) — 全局错误模型
- [`00-overview/glossary.md`](00-overview/glossary.md) — 术语表
- [`00-overview/project-overview.md`](00-overview/project-overview.md) — 项目总览
- [`00-overview/runtime-model.md`](00-overview/runtime-model.md) — 运行时模型

### 01-modules/（模块，161 篇）

- [`01-modules/M01-build-release-install/README.md`](01-modules/M01-build-release-install/README.md) — 构建/发布/安装系统（M01）
- [`01-modules/M01-build-release-install/call-chains.md`](01-modules/M01-build-release-install/call-chains.md) — 构建/发布/安装系统：调用链
- [`01-modules/M01-build-release-install/data-structures.md`](01-modules/M01-build-release-install/data-structures.md) — 构建/发布/安装系统：数据结构与所有权
- [`01-modules/M01-build-release-install/design.md`](01-modules/M01-build-release-install/design.md) — 构建/发布/安装系统：设计分析
- [`01-modules/M01-build-release-install/development-guide.md`](01-modules/M01-build-release-install/development-guide.md) — 构建/发布/安装系统：开发指南
- [`01-modules/M01-build-release-install/diagrams.md`](01-modules/M01-build-release-install/diagrams.md) — 构建/发布/安装系统：图示
- [`01-modules/M01-build-release-install/examples.md`](01-modules/M01-build-release-install/examples.md) — 构建/发布/安装系统：例子
- [`01-modules/M01-build-release-install/execution-flows.md`](01-modules/M01-build-release-install/execution-flows.md) — 构建/发布/安装系统：执行流
- [`01-modules/M01-build-release-install/implementation.md`](01-modules/M01-build-release-install/implementation.md) — 构建/发布/安装系统：实现说明
- [`01-modules/M01-build-release-install/interfaces.md`](01-modules/M01-build-release-install/interfaces.md) — 构建/发布/安装系统：接口契约
- [`01-modules/M01-build-release-install/line-level-analysis.md`](01-modules/M01-build-release-install/line-level-analysis.md) — 构建/发布/安装系统：行级分析
- [`01-modules/M01-build-release-install/risks-and-debt.md`](01-modules/M01-build-release-install/risks-and-debt.md) — 构建/发布/安装系统：风险与技术债
- [`01-modules/M01-build-release-install/source-map.md`](01-modules/M01-build-release-install/source-map.md) — 构建/发布/安装系统：源码地图
- [`01-modules/M01-build-release-install/testing.md`](01-modules/M01-build-release-install/testing.md) — 构建/发布/安装系统：测试策略
- [`01-modules/M02-driver-api/README.md`](01-modules/M02-driver-api/README.md) — Driver API 导出层（M02）
- [`01-modules/M02-driver-api/call-chains.md`](01-modules/M02-driver-api/call-chains.md) — Driver API 导出层：调用链
- [`01-modules/M02-driver-api/data-structures.md`](01-modules/M02-driver-api/data-structures.md) — Driver API 导出层：数据结构与所有权
- [`01-modules/M02-driver-api/design.md`](01-modules/M02-driver-api/design.md) — Driver API 导出层：设计分析
- [`01-modules/M02-driver-api/development-guide.md`](01-modules/M02-driver-api/development-guide.md) — Driver API 导出层：开发指南
- [`01-modules/M02-driver-api/diagrams.md`](01-modules/M02-driver-api/diagrams.md) — Driver API 导出层：图示
- [`01-modules/M02-driver-api/examples.md`](01-modules/M02-driver-api/examples.md) — Driver API 导出层：例子
- [`01-modules/M02-driver-api/execution-flows.md`](01-modules/M02-driver-api/execution-flows.md) — Driver API 导出层：执行流
- [`01-modules/M02-driver-api/implementation.md`](01-modules/M02-driver-api/implementation.md) — Driver API 导出层：实现说明
- [`01-modules/M02-driver-api/interfaces.md`](01-modules/M02-driver-api/interfaces.md) — Driver API 导出层：接口契约
- [`01-modules/M02-driver-api/line-level-analysis.md`](01-modules/M02-driver-api/line-level-analysis.md) — Driver API 导出层：行级分析
- [`01-modules/M02-driver-api/risks-and-debt.md`](01-modules/M02-driver-api/risks-and-debt.md) — Driver API 导出层：风险与技术债
- [`01-modules/M02-driver-api/source-map.md`](01-modules/M02-driver-api/source-map.md) — Driver API 导出层：源码地图
- [`01-modules/M02-driver-api/testing.md`](01-modules/M02-driver-api/testing.md) — Driver API 导出层：测试策略
- [`01-modules/M03-platform-device/README.md`](01-modules/M03-platform-device/README.md) — 平台与设备管理（M03）
- [`01-modules/M03-platform-device/call-chains.md`](01-modules/M03-platform-device/call-chains.md) — 平台与设备管理：调用链
- [`01-modules/M03-platform-device/data-structures.md`](01-modules/M03-platform-device/data-structures.md) — 平台与设备管理：数据结构与所有权
- [`01-modules/M03-platform-device/design.md`](01-modules/M03-platform-device/design.md) — 平台与设备管理：设计分析
- [`01-modules/M03-platform-device/development-guide.md`](01-modules/M03-platform-device/development-guide.md) — 平台与设备管理：开发指南
- [`01-modules/M03-platform-device/diagrams.md`](01-modules/M03-platform-device/diagrams.md) — 平台与设备管理：图示
- [`01-modules/M03-platform-device/examples.md`](01-modules/M03-platform-device/examples.md) — 平台与设备管理：例子
- [`01-modules/M03-platform-device/execution-flows.md`](01-modules/M03-platform-device/execution-flows.md) — 平台与设备管理：执行流
- [`01-modules/M03-platform-device/implementation.md`](01-modules/M03-platform-device/implementation.md) — 平台与设备管理：实现说明
- [`01-modules/M03-platform-device/interfaces.md`](01-modules/M03-platform-device/interfaces.md) — 平台与设备管理：接口契约
- [`01-modules/M03-platform-device/line-level-analysis.md`](01-modules/M03-platform-device/line-level-analysis.md) — 平台与设备管理：行级分析
- [`01-modules/M03-platform-device/risks-and-debt.md`](01-modules/M03-platform-device/risks-and-debt.md) — 平台与设备管理：风险与技术债
- [`01-modules/M03-platform-device/source-map.md`](01-modules/M03-platform-device/source-map.md) — 平台与设备管理：源码地图
- [`01-modules/M03-platform-device/testing.md`](01-modules/M03-platform-device/testing.md) — 平台与设备管理：测试策略
- [`01-modules/M04-context-lifecycle/README.md`](01-modules/M04-context-lifecycle/README.md) — Context 与资源生命周期（M04）
- [`01-modules/M04-context-lifecycle/call-chains.md`](01-modules/M04-context-lifecycle/call-chains.md) — Context 与资源生命周期：调用链
- [`01-modules/M04-context-lifecycle/data-structures.md`](01-modules/M04-context-lifecycle/data-structures.md) — Context 与资源生命周期：数据结构与所有权
- [`01-modules/M04-context-lifecycle/design.md`](01-modules/M04-context-lifecycle/design.md) — Context 与资源生命周期：设计分析
- [`01-modules/M04-context-lifecycle/development-guide.md`](01-modules/M04-context-lifecycle/development-guide.md) — Context 与资源生命周期：开发指南
- [`01-modules/M04-context-lifecycle/diagrams.md`](01-modules/M04-context-lifecycle/diagrams.md) — Context 与资源生命周期：图示
- [`01-modules/M04-context-lifecycle/examples.md`](01-modules/M04-context-lifecycle/examples.md) — Context 与资源生命周期：例子
- [`01-modules/M04-context-lifecycle/execution-flows.md`](01-modules/M04-context-lifecycle/execution-flows.md) — Context 与资源生命周期：执行流
- [`01-modules/M04-context-lifecycle/implementation.md`](01-modules/M04-context-lifecycle/implementation.md) — Context 与资源生命周期：实现说明
- [`01-modules/M04-context-lifecycle/interfaces.md`](01-modules/M04-context-lifecycle/interfaces.md) — Context 与资源生命周期：接口契约
- [`01-modules/M04-context-lifecycle/line-level-analysis.md`](01-modules/M04-context-lifecycle/line-level-analysis.md) — Context 与资源生命周期：行级分析
- [`01-modules/M04-context-lifecycle/risks-and-debt.md`](01-modules/M04-context-lifecycle/risks-and-debt.md) — Context 与资源生命周期：风险与技术债
- [`01-modules/M04-context-lifecycle/source-map.md`](01-modules/M04-context-lifecycle/source-map.md) — Context 与资源生命周期：源码地图
- [`01-modules/M04-context-lifecycle/testing.md`](01-modules/M04-context-lifecycle/testing.md) — Context 与资源生命周期：测试策略
- [`01-modules/M05-memory-mempool/README.md`](01-modules/M05-memory-mempool/README.md) — M05：Memory 与 MemoryPool
- [`01-modules/M05-memory-mempool/async-rollback-and-locking.md`](01-modules/M05-memory-mempool/async-rollback-and-locking.md) — M05 专题：异步分配回滚、访问更新与锁域
- [`01-modules/M05-memory-mempool/call-chains.md`](01-modules/M05-memory-mempool/call-chains.md) — Memory 与 MemoryPool：调用链
- [`01-modules/M05-memory-mempool/data-structures.md`](01-modules/M05-memory-mempool/data-structures.md) — M05：MemoryPool 数据结构与所有权
- [`01-modules/M05-memory-mempool/design.md`](01-modules/M05-memory-mempool/design.md) — M05：显存池设计专题
- [`01-modules/M05-memory-mempool/development-guide.md`](01-modules/M05-memory-mempool/development-guide.md) — Memory 与 MemoryPool：开发指南
- [`01-modules/M05-memory-mempool/diagrams.md`](01-modules/M05-memory-mempool/diagrams.md) — Memory 与 MemoryPool：图示
- [`01-modules/M05-memory-mempool/examples.md`](01-modules/M05-memory-mempool/examples.md) — Memory 与 MemoryPool：例子
- [`01-modules/M05-memory-mempool/execution-flows.md`](01-modules/M05-memory-mempool/execution-flows.md) — M05：显存池执行流与生命周期
- [`01-modules/M05-memory-mempool/implementation.md`](01-modules/M05-memory-mempool/implementation.md) — M05：MemoryPool 逐函数实现
- [`01-modules/M05-memory-mempool/interfaces.md`](01-modules/M05-memory-mempool/interfaces.md) — Memory 与 MemoryPool：接口契约
- [`01-modules/M05-memory-mempool/line-level-analysis.md`](01-modules/M05-memory-mempool/line-level-analysis.md) — Memory 与 MemoryPool：行级分析
- [`01-modules/M05-memory-mempool/risks-and-debt.md`](01-modules/M05-memory-mempool/risks-and-debt.md) — Memory 与 MemoryPool：风险与技术债
- [`01-modules/M05-memory-mempool/source-map.md`](01-modules/M05-memory-mempool/source-map.md) — Memory 与 MemoryPool：源码地图
- [`01-modules/M05-memory-mempool/testing.md`](01-modules/M05-memory-mempool/testing.md) — Memory 与 MemoryPool：测试与验证策略
- [`01-modules/M06-stream-command/README.md`](01-modules/M06-stream-command/README.md) — M06：Stream 与 Command 调度
- [`01-modules/M06-stream-command/call-chains.md`](01-modules/M06-stream-command/call-chains.md) — Stream 与 Command 调度：调用链
- [`01-modules/M06-stream-command/data-structures.md`](01-modules/M06-stream-command/data-structures.md) — Stream 与 Command 调度：数据结构与所有权
- [`01-modules/M06-stream-command/design.md`](01-modules/M06-stream-command/design.md) — Stream 与 Command 调度：设计分析
- [`01-modules/M06-stream-command/development-guide.md`](01-modules/M06-stream-command/development-guide.md) — Stream 与 Command 调度：开发指南
- [`01-modules/M06-stream-command/diagrams.md`](01-modules/M06-stream-command/diagrams.md) — Stream 与 Command 调度：图示
- [`01-modules/M06-stream-command/examples.md`](01-modules/M06-stream-command/examples.md) — Stream 与 Command 调度：例子
- [`01-modules/M06-stream-command/execution-flows.md`](01-modules/M06-stream-command/execution-flows.md) — Stream 与 Command 调度：执行流
- [`01-modules/M06-stream-command/implementation.md`](01-modules/M06-stream-command/implementation.md) — Stream 与 Command 调度：实现说明
- [`01-modules/M06-stream-command/interfaces.md`](01-modules/M06-stream-command/interfaces.md) — Stream 与 Command 调度：接口契约
- [`01-modules/M06-stream-command/line-level-analysis.md`](01-modules/M06-stream-command/line-level-analysis.md) — Stream 与 Command 调度：行级分析
- [`01-modules/M06-stream-command/risks-and-debt.md`](01-modules/M06-stream-command/risks-and-debt.md) — Stream 与 Command 调度：风险与技术债
- [`01-modules/M06-stream-command/source-map.md`](01-modules/M06-stream-command/source-map.md) — Stream 与 Command 调度：源码地图
- [`01-modules/M06-stream-command/testing.md`](01-modules/M06-stream-command/testing.md) — Stream 与 Command 调度：测试策略
- [`01-modules/M07-module-kernel/README.md`](01-modules/M07-module-kernel/README.md) — Module 与 Kernel 执行（M07）
- [`01-modules/M07-module-kernel/call-chains.md`](01-modules/M07-module-kernel/call-chains.md) — Module 与 Kernel 执行：调用链
- [`01-modules/M07-module-kernel/data-structures.md`](01-modules/M07-module-kernel/data-structures.md) — Module 与 Kernel 执行：数据结构与所有权
- [`01-modules/M07-module-kernel/design.md`](01-modules/M07-module-kernel/design.md) — Module 与 Kernel 执行：设计分析
- [`01-modules/M07-module-kernel/development-guide.md`](01-modules/M07-module-kernel/development-guide.md) — Module 与 Kernel 执行：开发指南
- [`01-modules/M07-module-kernel/diagrams.md`](01-modules/M07-module-kernel/diagrams.md) — Module 与 Kernel 执行：图示
- [`01-modules/M07-module-kernel/examples.md`](01-modules/M07-module-kernel/examples.md) — Module 与 Kernel 执行：例子
- [`01-modules/M07-module-kernel/execution-flows.md`](01-modules/M07-module-kernel/execution-flows.md) — Module 与 Kernel 执行：执行流
- [`01-modules/M07-module-kernel/implementation.md`](01-modules/M07-module-kernel/implementation.md) — Module 与 Kernel 执行：实现说明
- [`01-modules/M07-module-kernel/interfaces.md`](01-modules/M07-module-kernel/interfaces.md) — Module 与 Kernel 执行：接口契约
- [`01-modules/M07-module-kernel/line-level-analysis.md`](01-modules/M07-module-kernel/line-level-analysis.md) — Module 与 Kernel 执行：行级分析
- [`01-modules/M07-module-kernel/risks-and-debt.md`](01-modules/M07-module-kernel/risks-and-debt.md) — Module 与 Kernel 执行：风险与技术债
- [`01-modules/M07-module-kernel/source-map.md`](01-modules/M07-module-kernel/source-map.md) — Module 与 Kernel 执行：源码地图
- [`01-modules/M07-module-kernel/testing.md`](01-modules/M07-module-kernel/testing.md) — Module 与 Kernel 执行：测试策略
- [`01-modules/M08-graph/README.md`](01-modules/M08-graph/README.md) — Graph 执行（M08）
- [`01-modules/M08-graph/call-chains.md`](01-modules/M08-graph/call-chains.md) — Graph 执行：调用链
- [`01-modules/M08-graph/data-structures.md`](01-modules/M08-graph/data-structures.md) — Graph 执行：数据结构与所有权
- [`01-modules/M08-graph/design.md`](01-modules/M08-graph/design.md) — Graph 执行：设计分析
- [`01-modules/M08-graph/development-guide.md`](01-modules/M08-graph/development-guide.md) — Graph 执行：开发指南
- [`01-modules/M08-graph/diagrams.md`](01-modules/M08-graph/diagrams.md) — Graph 执行：图示
- [`01-modules/M08-graph/examples.md`](01-modules/M08-graph/examples.md) — Graph 执行：例子
- [`01-modules/M08-graph/execution-flows.md`](01-modules/M08-graph/execution-flows.md) — Graph 执行：执行流
- [`01-modules/M08-graph/implementation.md`](01-modules/M08-graph/implementation.md) — Graph 执行：实现说明
- [`01-modules/M08-graph/interfaces.md`](01-modules/M08-graph/interfaces.md) — Graph 执行：接口契约
- [`01-modules/M08-graph/line-level-analysis.md`](01-modules/M08-graph/line-level-analysis.md) — Graph 执行：行级分析
- [`01-modules/M08-graph/risks-and-debt.md`](01-modules/M08-graph/risks-and-debt.md) — Graph 执行：风险与技术债
- [`01-modules/M08-graph/source-map.md`](01-modules/M08-graph/source-map.md) — Graph 执行：源码地图
- [`01-modules/M08-graph/testing.md`](01-modules/M08-graph/testing.md) — Graph 执行：测试策略
- [`01-modules/M09-hal-m3d/README.md`](01-modules/M09-hal-m3d/README.md) — HAL/M3D 适配（M09）
- [`01-modules/M09-hal-m3d/call-chains.md`](01-modules/M09-hal-m3d/call-chains.md) — HAL/M3D 适配：调用链
- [`01-modules/M09-hal-m3d/data-structures.md`](01-modules/M09-hal-m3d/data-structures.md) — HAL/M3D 适配：数据结构与所有权
- [`01-modules/M09-hal-m3d/design.md`](01-modules/M09-hal-m3d/design.md) — HAL/M3D 适配：设计分析
- [`01-modules/M09-hal-m3d/development-guide.md`](01-modules/M09-hal-m3d/development-guide.md) — HAL/M3D 适配：开发指南
- [`01-modules/M09-hal-m3d/diagrams.md`](01-modules/M09-hal-m3d/diagrams.md) — HAL/M3D 适配：图示
- [`01-modules/M09-hal-m3d/examples.md`](01-modules/M09-hal-m3d/examples.md) — HAL/M3D 适配：例子
- [`01-modules/M09-hal-m3d/execution-flows.md`](01-modules/M09-hal-m3d/execution-flows.md) — HAL/M3D 适配：执行流
- [`01-modules/M09-hal-m3d/implementation.md`](01-modules/M09-hal-m3d/implementation.md) — HAL/M3D 适配：实现说明
- [`01-modules/M09-hal-m3d/interfaces.md`](01-modules/M09-hal-m3d/interfaces.md) — HAL/M3D 适配：接口契约
- [`01-modules/M09-hal-m3d/line-level-analysis.md`](01-modules/M09-hal-m3d/line-level-analysis.md) — HAL/M3D 适配：行级分析
- [`01-modules/M09-hal-m3d/risks-and-debt.md`](01-modules/M09-hal-m3d/risks-and-debt.md) — HAL/M3D 适配：风险与技术债
- [`01-modules/M09-hal-m3d/source-map.md`](01-modules/M09-hal-m3d/source-map.md) — HAL/M3D 适配：源码地图
- [`01-modules/M09-hal-m3d/testing.md`](01-modules/M09-hal-m3d/testing.md) — HAL/M3D 适配：测试策略
- [`01-modules/M10-tools-debug/README.md`](01-modules/M10-tools-debug/README.md) — 工具与调试接口（M10）
- [`01-modules/M10-tools-debug/call-chains.md`](01-modules/M10-tools-debug/call-chains.md) — 工具与调试接口：调用链
- [`01-modules/M10-tools-debug/data-structures.md`](01-modules/M10-tools-debug/data-structures.md) — 工具与调试接口：数据结构与所有权
- [`01-modules/M10-tools-debug/design.md`](01-modules/M10-tools-debug/design.md) — 工具与调试接口：设计分析
- [`01-modules/M10-tools-debug/development-guide.md`](01-modules/M10-tools-debug/development-guide.md) — 工具与调试接口：开发指南
- [`01-modules/M10-tools-debug/diagrams.md`](01-modules/M10-tools-debug/diagrams.md) — 工具与调试接口：图示
- [`01-modules/M10-tools-debug/examples.md`](01-modules/M10-tools-debug/examples.md) — 工具与调试接口：例子
- [`01-modules/M10-tools-debug/execution-flows.md`](01-modules/M10-tools-debug/execution-flows.md) — 工具与调试接口：执行流
- [`01-modules/M10-tools-debug/implementation.md`](01-modules/M10-tools-debug/implementation.md) — 工具与调试接口：实现说明
- [`01-modules/M10-tools-debug/interfaces.md`](01-modules/M10-tools-debug/interfaces.md) — 工具与调试接口：接口契约
- [`01-modules/M10-tools-debug/line-level-analysis.md`](01-modules/M10-tools-debug/line-level-analysis.md) — 工具与调试接口：行级分析
- [`01-modules/M10-tools-debug/risks-and-debt.md`](01-modules/M10-tools-debug/risks-and-debt.md) — 工具与调试接口：风险与技术债
- [`01-modules/M10-tools-debug/source-map.md`](01-modules/M10-tools-debug/source-map.md) — 工具与调试接口：源码地图
- [`01-modules/M10-tools-debug/testing.md`](01-modules/M10-tools-debug/testing.md) — 工具与调试接口：测试策略
- [`01-modules/M11-loader/README.md`](01-modules/M11-loader/README.md) — Loader 薄加载层（M11）
- [`01-modules/M11-loader/call-chains.md`](01-modules/M11-loader/call-chains.md) — Loader 薄加载层：调用链
- [`01-modules/M11-loader/data-structures.md`](01-modules/M11-loader/data-structures.md) — Loader 薄加载层：数据结构与线程模型
- [`01-modules/M11-loader/execution-flows.md`](01-modules/M11-loader/execution-flows.md) — Loader 薄加载层：执行流程
- [`01-modules/M11-loader/implementation.md`](01-modules/M11-loader/implementation.md) — Loader 薄加载层：实现要点
- [`01-modules/M11-loader/interfaces.md`](01-modules/M11-loader/interfaces.md) — Loader 薄加载层：接口与构建目标
- [`01-modules/M11-loader/risks-and-debt.md`](01-modules/M11-loader/risks-and-debt.md) — Loader 薄加载层：风险与技术债
- [`01-modules/M11-loader/source-map.md`](01-modules/M11-loader/source-map.md) — Loader 薄加载层：源码地图
- [`01-modules/M12-util/README.md`](01-modules/M12-util/README.md) — Util 基础库（M12）
- [`01-modules/M12-util/call-chains.md`](01-modules/M12-util/call-chains.md) — Util 基础库：调用链
- [`01-modules/M12-util/data-structures.md`](01-modules/M12-util/data-structures.md) — Util 基础库：数据结构与关键实现
- [`01-modules/M12-util/interfaces.md`](01-modules/M12-util/interfaces.md) — Util 基础库：接口
- [`01-modules/M12-util/risks-and-debt.md`](01-modules/M12-util/risks-and-debt.md) — Util 基础库：风险与技术债
- [`01-modules/M12-util/source-map.md`](01-modules/M12-util/source-map.md) — Util 基础库：源码地图
- [`01-modules/M13-musa-shared-include/README.md`](01-modules/M13-musa-shared-include/README.md) — 公共 API 头文件层（M13）
- [`01-modules/M13-musa-shared-include/data-structures.md`](01-modules/M13-musa-shared-include/data-structures.md) — 公共 API 头文件层：数据结构
- [`01-modules/M13-musa-shared-include/interfaces.md`](01-modules/M13-musa-shared-include/interfaces.md) — 公共 API 头文件层：接口
- [`01-modules/M13-musa-shared-include/risks-and-debt.md`](01-modules/M13-musa-shared-include/risks-and-debt.md) — 公共 API 头文件层：风险与技术债
- [`01-modules/M13-musa-shared-include/source-map.md`](01-modules/M13-musa-shared-include/source-map.md) — 公共 API 头文件层：源码地图
- [`01-modules/module-registry.md`](01-modules/module-registry.md) — 模块注册表

### 80-demos/（Demo，10 篇）

- [`80-demos/D01-muinfo/README.md`](80-demos/D01-muinfo/README.md) — D01：muInfo 设备信息查询
- [`80-demos/D01-muinfo/build-and-run.md`](80-demos/D01-muinfo/build-and-run.md) — D01：构建与运行
- [`80-demos/D01-muinfo/data-and-state-trace.md`](80-demos/D01-muinfo/data-and-state-trace.md) — D01：`muInfo` 数据、状态与控制点
- [`80-demos/D01-muinfo/debug-walkthrough.md`](80-demos/D01-muinfo/debug-walkthrough.md) — D01：`muInfo` 调试 walkthrough
- [`80-demos/D01-muinfo/execution-trace.md`](80-demos/D01-muinfo/execution-trace.md) — D01：`muInfo` 设备信息查询——完整源码轨迹
- [`80-demos/D01-muinfo/failure-paths.md`](80-demos/D01-muinfo/failure-paths.md) — D01：`muInfo` 失败路径
- [`80-demos/D01-muinfo/modification-exercises.md`](80-demos/D01-muinfo/modification-exercises.md) — D01：修改练习
- [`80-demos/D02-memory-copy/README.md`](80-demos/D02-memory-copy/README.md) — D02：内存拷贝 / 批量异步候选
- [`80-demos/D03-graph/README.md`](80-demos/D03-graph/README.md) — D03：Graph 候选
- [`80-demos/demo-registry.md`](80-demos/demo-registry.md) — Demo 注册表

### 90-cross-module/（跨模块，10 篇）

- [`90-cross-module/change-impact-map.md`](90-cross-module/change-impact-map.md) — 变更影响地图
- [`90-cross-module/configuration-impact-map.md`](90-cross-module/configuration-impact-map.md) — 配置影响地图
- [`90-cross-module/cross-module-call-chains.md`](90-cross-module/cross-module-call-chains.md) — 跨模块调用链索引
- [`90-cross-module/end-to-end-flows.md`](90-cross-module/end-to-end-flows.md) — 端到端流程
- [`90-cross-module/error-boundaries.md`](90-cross-module/error-boundaries.md) — 跨模块错误边界
- [`90-cross-module/interface-contracts.md`](90-cross-module/interface-contracts.md) — 跨模块接口契约
- [`90-cross-module/performance-critical-paths.md`](90-cross-module/performance-critical-paths.md) — 性能关键路径
- [`90-cross-module/runtime-trace.md`](90-cross-module/runtime-trace.md) — 跨模块运行时轨迹
- [`90-cross-module/shared-data-and-types.md`](90-cross-module/shared-data-and-types.md) — 共享数据与类型
- [`90-cross-module/system-wiring.md`](90-cross-module/system-wiring.md) — 系统串联

### 99-roadmap/（实践与路线图，10 篇）

- [`99-roadmap/debugging-guide.md`](99-roadmap/debugging-guide.md) — 调试指南
- [`99-roadmap/feature-development-recipes.md`](99-roadmap/feature-development-recipes.md) — 功能开发配方
- [`99-roadmap/next-steps.md`](99-roadmap/next-steps.md) — 下一步
- [`99-roadmap/performance-guide.md`](99-roadmap/performance-guide.md) — 性能指南
- [`99-roadmap/quick-start.md`](99-roadmap/quick-start.md) — 快速开始
- [`99-roadmap/rca-splaytree-memmgr.md`](99-roadmap/rca-splaytree-memmgr.md) — RCA：`MemMgr` 内存池查找链（`SplayTree::Search` 分支反转 + 无防护解引用）
- [`99-roadmap/reading-guide.md`](99-roadmap/reading-guide.md) — 阅读指南
- [`99-roadmap/risk-register.md`](99-roadmap/risk-register.md) — 风险登记
- [`99-roadmap/technical-debt.md`](99-roadmap/technical-debt.md) — 技术债
- [`99-roadmap/testing-recipes.md`](99-roadmap/testing-recipes.md) — 测试配方
## 12. 最终交付摘要

> 对应 project-prompt §十三；本节为精选，全量细节按链接展开。

1. **目录树**：见 §4；全量文件清单见 §11（204 篇）。
2. **每文件一句话**：§11 每条链接后的 H1 标题即摘要；页头“文档目的”补充适用范围。
3. **阅读路径**：见 §9（按角色的六条路径）。

4. **最关键 10 个源码入口**：

| 入口 | 位置 | 作用 |
|---|---|---|
| `muapiInit` | [src/driver/mu_context.cpp:121-133] | 驱动初始化入口 |
| `Platform::Init` | [src/musa/core/platform.cpp:84-140] | 设备发现与 Device 创建 |
| `muapiMemAlloc_v2` | [src/driver/mu_memory.cpp:271-320] | 同步内存分配入口 |
| `Memory::GeneralAlloc` | [src/musa/core/memory.cpp:470-520] | Core 分配落地 |
| `MemMgr::Allocate` | [src/hal/m3d/memMgr.cpp:81-147] | HAL 池分配与 FreeSlot |
| `muapiMemSetMemPool` | [src/driver/mu_mempool.cpp:453-515] | current pool 绑定 |
| `muapiLaunchKernel` | [src/driver/mu_module.cpp:231-269] | Kernel 启动入口 |
| `DispatchCommand::Build` | [src/musa/core/command/dispatchCommand.cpp:78-301] | 派发命令构建 |
| `muapiGetExportTable` | [src/driver/mu_entry.cpp:1255-1265] | 导出表分派总闸 |
| `Function::Init` | [src/musa/core/symbol.cpp:434-449] | kernel 元数据与 smem 计算 |

5. **最关键 10 条调用链**（均在对应文档内带行级引用）：
   INIT-001 初始化与设备发现、MEM-001 同步分配、MEMFREE-001 释放、MEMASYNC-001 异步分配、MEMGRAPH-001 graph 内存、KERNEL-001 启动 — 见 [`90-cross-module/cross-module-call-chains.md`](90-cross-module/cross-module-call-chains.md)；
   loader 首跳与系统主线 — 见 [`90-cross-module/system-wiring.md`](90-cross-module/system-wiring.md)；
   导出表两种分发 — 见 [`01-modules/M10-tools-debug/call-chains.md`](01-modules/M10-tools-debug/call-chains.md)；
   graph capture→instantiate→launch — 见 [`01-modules/M08-graph/call-chains.md`](01-modules/M08-graph/call-chains.md)；
   D01 全程轨迹 — 见 [`80-demos/D01-muinfo/execution-trace.md`](80-demos/D01-muinfo/execution-trace.md)。

6. **主 Demo 执行阶段**（D01 `muInfo`，静态解剖、运行未验证）：§0 loader 首跳 → §1 从 `main` 到设备循环 → §2 `printDeviceProp` 子调用（属性缓存+输出）→ §3 数据与所有权 → §4 错误路径 → §5 运行时未验证项；入口 [`80-demos/D01-muinfo/README.md`](80-demos/D01-muinfo/README.md)。

7. **Demo—模块—源码矩阵**：见 [`80-demos/demo-registry.md`](80-demos/demo-registry.md)（D01-D07 × M01-M13）；各模块的“Demo 覆盖”节列出经过它的具体步骤与未覆盖实现。

8. **最关键 10 个设计决策**（全表见 [`00-overview/decision-log.md`](00-overview/decision-log.md)）：
   `SplayTree::Search` 分支互换是缺陷而非特性；`Get` 返回“相等或最近”而 `Find` 才返回空，MemMgr 必须按 `Find` 语义判空；`AccessorHint = 0xDEADBEEF` 是未填充哨兵；导出表分静态指针/缓冲区填充两种分发；`FillAccessors` 遇源端 `0` 即停；未知 `Client` id 静默返回 MUPTI 表（源码自带 FIXME）；`MUSA_ASSERT` 在 `NDEBUG` 下无效；池键五字段位打包受 `static_assert` 约束；命令状态与 Graph 节点类型枚举只允许尾部追加；不执行构建/安装/测试（`install.sh` 写系统目录）。

9. **最关键 10 个风险**（全表 35 条见 [`99-roadmap/risk-register.md`](99-roadmap/risk-register.md)）：
   `Search` 分支反转（严重，已 RCA）；`FindPool` 无条件解引用（严重，已 RCA）；`MemMgr::Free` 可能错池/空指针（严重，已 RCA）；`FillAccessors` 漏填与不支持不可区分（严重）；loader 在本快照无法配置编译（阻断）；未知 client 静默返回 MUPTI 表（高）；静态表族无所有权标注（高）；导出表前 12 成员被 legacy ABI 锁定（高）；loader 导出清单无一致性检查（高）；`SplayTree` 析构为空实现泄漏节点（高）。

10. **覆盖范围与深度审计**：见 [`00-overview/analysis-state.md`](00-overview/analysis-state.md) §10（覆盖表）、§11（深度审计）、§13（实质性度量：204 篇 / 4888 引用 / 21548 行、双校验 0、100% ≥5 处引用）。
11. **未解决问题**：同上 §12/§13.2、§14 缺口列 + 本文件 §10。
12. **下一步建议**：见 [`99-roadmap/next-steps.md`](99-roadmap/next-steps.md)（补测试与构建验证、HAL 私有子模块、运行期复现 SplayTree 路径）。

## 相关文档

- [`00-overview/project-overview.md`](00-overview/project-overview.md)
- [`00-overview/architecture.md`](00-overview/architecture.md)
- [`00-overview/build-and-deploy.md`](00-overview/build-and-deploy.md)
- [`00-overview/evidence-index.md`](00-overview/evidence-index.md)
- [`01-modules/module-registry.md`](01-modules/module-registry.md)
- [`80-demos/demo-registry.md`](80-demos/demo-registry.md)
- [`90-cross-module/system-wiring.md`](90-cross-module/system-wiring.md)
- [`99-roadmap/quick-start.md`](99-roadmap/quick-start.md)

## 源码证据摘要

核心证据集中维护在 [`00-overview/evidence-index.md`](00-overview/evidence-index.md)。

## 未解决问题

1. `src/hal/m3d` 到 M3D 子模块的具体动态调用链尚未逐行展开。
2. `mu_wrappers_generated.cpp` 的生成来源、导出表和 API 包装细节尚未完成专门分析。
3. 当前环境未执行构建、单元测试和 `muInfo`，所有命令结果均标记为未验证。
4. Graph、ray tracing、green context、debug/profiling hook 属于后续重点模块。

## 下一步阅读建议

先读 [`00-overview/architecture.md`](00-overview/architecture.md)，再读 [`01-modules/module-registry.md`](01-modules/module-registry.md)，然后从 `muInfo` Demo 或内存分配调用链进入逐模块深挖。
