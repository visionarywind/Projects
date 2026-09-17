# MUSA Driver 知识库

- 文档目的：作为 `/home/shanfeng/workspace/linux-ddk/musa` 的源码理解总入口，串联总览、模块、Demo、跨模块和开发实践文档。
- 适用范围：MUSA Driver / MUSA Runtime driver API 用户态库源码；已生成总览、模块、Demo、跨模块和开发实践层，仍明确标注源码/硬件验证边界。
- 对应源码版本：远端 `HEAD`，提交 `b8dce2b2f23849e8e99350c20d7eaac3c129caba`，CMake 版本 `5.2.0 develop`。
- 证据状态：静态源码分析已扩展；构建与运行未在当前环境执行。
- 最后更新：2026-09-16
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
| 远端仓库 | `ssh shanfeng@10.20.34.9:/home/shanfeng/workspace/linux-ddk/musa` | 已确认 |
| Git 分支 | `HEAD` detached/当前 HEAD 名称输出为 `HEAD` | 已确认 |
| Git 提交 | `b8dce2b2f23849e8e99350c20d7eaac3c129caba` | 已确认 |
| 最近提交 | `Merge [SW-71661] Add PHOTON_BVH_PRE_COMPACT_TRI3 and update m3d` | 已确认 |
| CMake 版本 | `5.2.0 develop` | 已确认 |
| 本地文档目录 | `/home/mtuser/workspace/Projects/musa` | 已确认 |

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
| M01 | 构建/发布/安装系统 | 定义版本、编译选项、产物、安装路径和 CI 包装 | [CMakeLists.txt:21-35], [src/driver/CMakeLists.txt:9-50], [install.sh:27-105] | 部分完成 |
| M02 | Driver API 导出层 | 提供 `muapi*` 入口、参数校验、错误转换和 API 表导出 | [src/driver/mu_context.cpp:121-220], [src/driver/mu_memory.cpp:271-320], [src/driver/mu_entry.cpp:1241-1245] | 部分完成 |
| M03 | 平台与设备管理 | 初始化 HAL、发现设备、维护全局对象注册表和设备视图 | [src/musa/core/platform.cpp:84-140], [src/musa/core/device.cpp:593-672] | 部分完成 |
| M04 | 上下文与资源生命周期 | 作为设备资源归属中心，创建/销毁 stream、memory、event、module、graph 等对象 | [src/musa/core/context.cpp:1037-1099], [src/musa/core/context.cpp:1207-1247] | 部分完成 |
| M05 | 内存与内存池 | 实现 device/host/managed/virtual/pool memory 的创建、映射、释放；HAL pool 负责 chunk/segment/bucket/merge/trim | [src/driver/mu_memory.cpp:271-449,716-755], [src/driver/mu_mempool.cpp:1-563], [src/hal/m3d/memoryPool.cpp:82-510] | 显存池专题已静态深挖 |
| M06 | Stream/Command 调度 | 将同步/异步操作组织成 Command 并提交到队列 | [src/musa/core/stream.cpp:237-260], [src/musa/core/stream.cpp:429-475] | 部分完成 |
| M07 | Module/Kernel 执行 | 加载模块、解析函数并发起 kernel launch | [src/driver/mu_module.cpp:88-130], [src/driver/mu_module.cpp:232-285] | 部分完成 |
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
- `tests/*.cu`：包括 graph、memcpy batch、conditional node 等 CUDA/MUSA 示例，适合作为图与异步内存路径的后续 Demo。

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
- 单元测试构建开关：`-DMUSA_BUILD_UT=ON`，顶层 CMake 会拉取 googletest 并添加 `unittest` [CMakeLists.txt:214-237]。
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
