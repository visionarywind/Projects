# 源码证据索引

- 文档目的：集中维护首批分析使用的源码、构建、CI 和脚本证据，便于后续增量校正行号与结论。
- 适用范围：MUSA Driver 知识库全部文档。
- 对应源码版本：`b8dce2b2f23849e8e99350c20d7eaac3c129caba`。
- 证据状态：部分推断
- 最后更新：2026-09-16
- 前置阅读：[`project-overview.md`](project-overview.md)
- 后续阅读：所有模块文档

## 结论摘要

本索引只记录已在本轮远端只读检查中确认过的证据范围。行号基于远端目标版本；后续若源码变更，需要重新确认。

## 1. 仓库与版本证据

| 结论 | 状态 | 证据 | 解释 |
|---|---|---|---|
| 远端仓库路径是 `/home/shanfeng/workspace/linux-ddk/musa` | 已确认 | 远端 `pwd` 输出 | 通过 ssh 进入目标目录确认 |
| 当前 HEAD 是 `b8dce2b2f23849e8e99350c20d7eaac3c129caba` | 已确认 | 远端 `git rev-parse HEAD` | 文档版本锚点 |
| 最近提交是 `Merge [SW-71661] Add PHOTON_BVH_PRE_COMPACT_TRI3 and update m3d` | 已确认 | 远端 `git log --oneline -5` | 与 M3D/Photon 相关 |
| CMake 项目名和版本为 `musa_driver 5.2.0 develop` | 已确认 | [CMakeLists.txt:101-103] | 顶层版本定义 |

## 2. 顶层 README 与目录职责

| 证据 | 证明内容 |
|---|---|
| [README.md:1] | README 标题为 `MUSA-Runtime` |
| [README.md:3-6] | 构建前依赖包含 `libelf-dev`、`gcc-multilib` |
| [README.md:9-16] | 最小构建命令 `mkdir build && cmake .. && make` |
| [README.md:17-23] | 默认依赖参数：`PVR_ARCH`、`PLATFORM`、`ARCH`、`WINDOW`、`BUILD_VERSION` |
| [README.md:26-37] | README 记录项目相关选项：ASAN、TSAN、Debug、CSV、UT |
| [README.md:39-45] | Debug 构建示例 |
| [README.md:46-50] | 安装命令 `./install.sh` |
| [README.md:51-64] | 目录结构：include/src/runtime/driver/musa/hal/tools/build |

## 3. CMake/构建证据

| 证据 | 证明内容 |
|---|---|
| [CMakeLists.txt:1] | CMake 最低版本 3.15 |
| [CMakeLists.txt:21-35] | 顶层选项：DDK_2_0、ASAN、TSAN、Debug、CSV、M3D trace、UT、MTUB、WSL2 |
| [CMakeLists.txt:38-61] | `MTUB_BUILD` 集成 mt-toolchain 和 package 配置 |
| [CMakeLists.txt:101-103] | `version_string 5.2.0`、`version_status develop`、`project(musa_driver)` |
| [CMakeLists.txt:127-130] | `MUSA_BUILD_DEBUG` 控制 Debug/Release |
| [CMakeLists.txt:165-169] | UNIX C++17 与安全链接选项 |
| [CMakeLists.txt:178-186] | ASAN/TSAN 编译选项 |
| [CMakeLists.txt:202-204] | `CSV_UNSPPORTED` 拼写分支 |
| [CMakeLists.txt:214-237] | `MUSA_BUILD_UT` 拉取 googletest 并加入 unittest |
| [src/CMakeLists.txt:1-5] | Linux driver 库名 `musa`，Windows `mtmusa64` |
| [src/CMakeLists.txt:7-12] | 子目录：util/hal/musa/driver/gdb/tools |
| [src/driver/CMakeLists.txt:4-11] | driver 源文件、MUPTI/MUGDB/MUASAN 源文件加入动态库 |
| [src/driver/CMakeLists.txt:17] | driver 动态库链接 `musaCore` |
| [src/driver/CMakeLists.txt:38-50] | 输出名、SONAME、安装规则 |
| [src/musa/core/CMakeLists.txt:1-11] | `musaCore` 静态库聚合 core/command/node/graph/copyManager2，并链接 `halM3d` |
| [src/hal/m3d/CMakeLists.txt:1-23] | M3D 编译宏、Linux/WSL2 设置和 `add_subdirectory(m3d)` |
| [src/hal/m3d/CMakeLists.txt:36-59] | `halM3d` 静态库链接 `m3d/scpc/util/dl/pthread/rt` |
| [src/tools/CMakeLists.txt:10-18] | tools 中每个 `.cpp` 生成可执行并链接 driver 动态库 |
| [unittest/CMakeLists.txt:1-46] | 每个 unittest `.cpp` 生成 gtest 可执行并注册 CTest |

## 4. 子模块与 CI 证据

| 证据 | 证明内容 |
|---|---|
| [.gitmodules:1-13] | 子模块：`module_version`、`src/hal/m3d/m3d`、`src/musa_shared_include` |
| [.ciConfig.yaml:13-16] | CI 参数中 `submoduleConfig` 包含 `libdrm-mt`、`m3d`、`shared_include` |
| [.ciConfig.yaml:19-24] | CI artifact 包含 `libmusadebugger.so` 与 `libmusa.so` |
| [.ciConfig.yaml:25-34] | CI 构建 `linux-ddk musa build` 使用 `./ddk_build.sh -a 0 -m 1` |
| [.mthreads-ci.yml:37-52] | MTUB release 构建安装 mt-toolchain/LLVM，CMake 使用 `LIBDRM_PATH`、`SHARED_INCLUDE_PATH` |
| [.mthreads-ci.yml:114-123] | Photon 测试安装 MUSA toolkit 和 mtcc |
| [.mthreads-ci.yml:125-148] | CI 安装 DDK 包、移除/加载 `mtgpu` 内核模块 |
| [.mthreads-ci.yml:181-207] | CI 构建 Photon 与 photon_samples 并运行测试 |

## 5. API 与核心对象证据

| 证据 | 证明内容 |
|---|---|
| [src/driver/mu_context.cpp:121-133] | `muapiInit(flags)` 检查 flags 并调用 `Musa::Platform::Get().Init()` |
| [src/driver/mu_context.cpp:156-184] | `muapiCtxCreate_v2` 创建上下文；`muapiCtxCreate` 包装 v2 |
| [src/driver/mu_context.cpp:188-220] | `muapiCtxDestroy_v2`/`muapiCtxDestroy` 销毁上下文 |
| [src/driver/mu_context.cpp:472-548] | primary context retain/reset/release 入口 |
| [src/driver/mu_device.cpp:14-45] | `muapiDeviceGet`、`muapiDeviceGetCount` |
| [src/driver/mu_device.cpp:71-135] | device attribute/name/total memory 查询 |
| [src/driver/mu_memory.cpp:271-320] | `muapiMemAlloc_v2` 和 legacy 包装 |
| [src/driver/mu_memory.cpp:716-758] | `muapiMemFree_v2` 和 legacy 包装 |
| [src/driver/mu_memory.cpp:1004-1020] | `muapiMemcpy`/`muapiMemcpyAsync` |
| [src/driver/mu_stream.cpp:16-60] | stream create with priority |
| [src/driver/mu_stream.cpp:131-150] | stream get context |
| [src/driver/mu_module.cpp:88-130] | module load/load data |
| [src/driver/mu_module.cpp:232-285] | kernel launch 入口 |
| [src/driver/mu_entry.cpp:1241-1245] | 导出表包含 `muapiInit`、`muapiDeviceGetCount`、`muapiCtxCreate`、`muapiCtxDestroy` |

## 6. Platform/Device/Context 证据

| 证据 | 证明内容 |
|---|---|
| [src/musa/core/platform.cpp:13-18] | `Platform::Get()` 单例和构造函数入口 |
| [src/musa/core/platform.cpp:84-140] | `Platform::Init` 初始化和设备访问入口 |
| [src/musa/core/platform.cpp:366-410] | platform memory 创建/查找/销毁和 context 注册 |
| [src/musa/core/platform.cpp:416-428] | context 注册和 `ValidateContext` |
| [src/musa/core/platform.cpp:478-506] | platform 析构释放未释放 memory/resource desc |
| [src/musa/core/device.cpp:593-635] | `Device::CreateContext`/`DestroyContext` |
| [src/musa/core/device.cpp:648-672] | `Device` 构造/析构 |
| [src/musa/core/device.cpp:856-1012] | `Device::LaterInit`、同步、显存查询 |
| [src/musa/core/device.cpp:1005-1145] | copy manager 初始化、内部内存分配/释放 |
| [src/musa/core/context.cpp:293-475] | `Context::CriticalBase` 管理资源集合 |
| [src/musa/core/context.cpp:1037-1099] | context 创建/销毁 memory |
| [src/musa/core/context.cpp:1207-1247] | context 创建 stream 与默认 stream 访问 |
| [src/musa/core/context.cpp:1875-1988] | context 构造、析构、初始化、Dispose、命令提交入口附近 |

## 7. Memory/Stream/Module 证据

| 证据 | 证明内容 |
|---|---|
| [src/musa/core/memory.cpp:345-431] | Memory 构造、析构、Init 起点 |
| [src/musa/core/memory.cpp:470-520] | GeneralAlloc 分配路径 |
| [src/musa/core/memory.cpp:563-646] | Pinned host alloc/register 路径 |
| [src/musa/core/memory.cpp:704-746] | Managed/Ipc import alloc 路径 |
| [src/musa/core/stream.cpp:72-102] | Stream capture begin/end |
| [src/musa/core/stream.cpp:237-260] | Stream query/synchronize |
| [src/musa/core/stream.cpp:429-475] | Stream 命令方法入口片段 |
| [src/musa/core/context.cpp:625-673] | `Context::GeneralLaunchKernel` / rays |
| [src/musa/core/context.cpp:696-890] | GeneralMemcpy/Batch/Memset/HostFunc 路径 |

## 8. 安装脚本证据

| 证据 | 证明内容 |
|---|---|
| [install.sh:27-30] | 若 `./build/lib` 不存在则报错退出 |
| [install.sh:32-49] | 支持 `-l` 和 `-b` 参数 |
| [install.sh:51-80] | 根据架构/发行版选择库目录 |
| [install.sh:83-105] | 创建目录并复制 `libmusa*`、`musa_driver_version`、`muInfo` |

## 8. MemoryPool 深挖证据

| 证据 | 证明内容 |
|---|---|
| [src/driver/mu_mempool.cpp:1-563] | pool create/destroy、attribute、access、trim、IPC handle、default/current pool API |
| [src/musa/core/memoryPool.h:85-152] | Core pool wrapper 字段：HAL pool、stream、access map、allocation set、logical-byte statistics、pool type |
| [src/musa/core/memoryPool.cpp:101-198,201-437] | Core pool access mapping、memory tracking、attribute、trim、IPC 和 HAL delegation |
| [src/musa/core/memory.cpp:345-379,431-460] | Core Memory 对 chunk+offset 的保存、pool allocation granularity、destructor free boundary |
| [src/musa/core/device.cpp:439-519] | device default/graph pool 的创建、32 MiB chunk quantum、user-managed ownership |
| [src/musa/core/device.cpp:1091-1152] | internal pool lazy init、prealloc memory 和显式 internal free |
| [src/musa/core/platform.cpp:478-541,544-645] | Platform teardown、host/NUMA default/current pool 创建和所有权 |
| [src/musa/core/stream.cpp:538-671] | capture/async alloc/free：virtual reserve、physical init、bind、paging、callback cleanup |
| [src/musa/core/stream.cpp:1113-1121] | `WaitFinish` 后更新 user pools |
| [src/musa/core/command/command.cpp:238-247] | command 未完成时更新 user pools |
| [src/musa/core/context.cpp:1037-1097,2368-2394] | Core memory 登记/销毁和 graph alloc/free node 创建 |
| [src/musa/core/node/graphMemoryAllocNode.cpp:15-48] | graph capture 阶段从 graph pool 保留 virtual memory |
| [src/musa/core/node/graphMemoryFreeNode.cpp:6-13] | graph free node 初始化当前为 no-op |
| [src/musa/core/graph/graph1/universalManager.cpp:220-367] | graph 执行期间 physical allocation、peer/paging、unmap 和 cleanup |
| [src/musa/core/graph.cpp:22-29] | graph resource 析构时清理 physical 并归还 virtual pool memory |
| [src/hal/m3d/memMgr.h:37-39,41-115,121-129] | internal/user/automatic pool registry 和 property/type/heap/NUMA key bit packing |
| [src/hal/m3d/memMgr.cpp:81-147,163-197,229-235] | automatic pool lookup/create、registry、UpdateUserPools trim |
| [src/hal/m3d/memoryPool.h:48-150] | ResSegment、free buckets、bitmap、segment list、virtual range tracker、watermark |
| [src/hal/m3d/memoryPool.cpp:14-40,42-61,82-211] | FindBucket、默认 policy、FullAllocate、SubAllocate、ChunkAllocate |
| [src/hal/m3d/memoryPool.cpp:214-259,318-331,358-510] | Free、左右合并、lazy reuse、ResourceSplit、free list、TrimPool 和析构 |

这些证据把 M05 从 API/Core 入口追到 HAL pool 的 chunk/segment 副作用；M3D 子模块内部、kernel driver 和硬件完成结果仍未验证。


- [`project-overview.md`](project-overview.md)
- [`architecture.md`](architecture.md)
- [`build-and-deploy.md`](build-and-deploy.md)
- [`../01-modules/module-registry.md`](../01-modules/module-registry.md)

## 源码证据摘要

本文即为证据摘要。后续模块文档应链接本索引，并在结论附近保留最小必要引用。

## 未解决问题

1. 需要补充公开头文件的声明证据，尤其 `musa_shared_include`。
2. 需要补充 HAL/M3D wrapper 到 M3D 子模块具体函数的行级证据。
3. 需要补充 `mu_wrappers_generated.cpp` 生成链和导出符号策略。

## 下一步阅读建议

从 M05 内存路径开始补“入口到真实副作用”的完整链路，并把新增行号追加到本索引。
