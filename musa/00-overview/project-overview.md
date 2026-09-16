# 项目总览

- 文档目的：说明 MUSA Driver 仓库的定位、边界、版本、目录和首批模块划分依据。
- 适用范围：`/home/shanfeng/workspace/linux-ddk/musa` 远端源码仓库。
- 对应源码版本：`b8dce2b2f23849e8e99350c20d7eaac3c129caba`，CMake `5.2.0 develop`。
- 证据状态：已确认/部分推断
- 最后更新：2026-09-16
- 前置阅读：[`../README.md`](../README.md)
- 后续阅读：[`architecture.md`](architecture.md)、[`../01-modules/module-registry.md`](../01-modules/module-registry.md)

## 结论摘要

结论：该仓库实现 MUSA 用户态 Driver API 动态库和配套工具，不是单纯文档仓库，也不是完整 Linux DDK 根仓库。  
状态：已确认。  
证据：README 标题为 `MUSA-Runtime` 并说明 `src/driver` 是 driver api layer、`src/musa` 是 driver API 使用的对象定义、`src/hal` 是硬件抽象层 [README.md:51-64]；顶层 CMake 项目名为 `musa_driver` [CMakeLists.txt:101-103]；CI 配置将构建产物标为 `libmusa.so`、`libmusadebugger.so` [.ciConfig.yaml:19-24]。

结论：当前首要源码版本是远端 HEAD `b8dce2b2f23849e8e99350c20d7eaac3c129caba`，最近提交与 M3D/Photon BVH 相关。  
状态：已确认。  
证据：通过远端 `git rev-parse HEAD` 和 `git log --oneline -5` 获取。

结论：项目采用“API 导出层 → 核心对象层 → HAL 抽象层 → M3D 实现层”的分层结构。  
状态：已确认 + 推断。  
证据：`src/CMakeLists.txt` 依次添加 `util`、`hal`、`musa`、`driver`、`gdb`、`tools` [src/CMakeLists.txt:7-12]；`driver` 链接 `musaCore` [src/driver/CMakeLists.txt:17]；`musaCore` 链接 `halM3d` [src/musa/core/CMakeLists.txt:7-11]；`halM3d` 链接 `m3d`、`scpc`、`util` [src/hal/m3d/CMakeLists.txt:36-59]。

## 项目用途和业务领域

MUSA 是面向摩尔线程 GPU 的并行计算软件栈。该仓库处在用户态 Driver API 层，主要解决：

1. 对外提供类 CUDA Driver API 风格的 `mu*`/`muapi*` 能力；
2. 维护进程内 GPU 平台、设备、上下文、stream、event、memory、module、graph 等对象；
3. 将高层资源和命令映射到硬件抽象层 HAL/M3D；
4. 生成和安装 `libmusa.so` 以及 `muInfo` 等工具。

与官方公开资料的一致性：MUSA SDK 软件栈包含 MT Linux Driver、编译器、runtime、库、迁移工具、调试和 profiling 工具。当前仓库仅覆盖其中 driver 用户态库的一部分；公开 SDK 说明可作为背景，但本文档以远端源码为准。

## 系统边界

| 边界项 | 内容 | 状态 | 证据 |
|---|---|---|---|
| 对外产物 | Linux 下 `libmusa.so`，工具 `muInfo`，另含 `libmusadebugger.so` 相关调试构建 | 已确认 | [src/CMakeLists.txt:1-5], [src/tools/CMakeLists.txt:10-18], [.ciConfig.yaml:19-24] |
| 对外 API | `muapi*` 函数和导出表；公开头文件来自 `src/musa_shared_include` 子模块 | 部分确认 | [src/driver/mu_context.cpp:121-220], [.gitmodules:8-13] |
| 内部对象 | Platform、Device、Context、Memory、Stream、Command、Graph、Module 等 | 已确认 | 源码清单与符号索引；详见模块注册表 |
| 底层依赖 | M3D 子模块、libdrm-mt/shared_include、内核驱动/设备 | 部分确认 | [.gitmodules:5-13], [.ciConfig.yaml:13-16], [src/hal/m3d/CMakeLists.txt:22-59] |
| 非目标 | 完整 Linux DDK 构建脚本、内核驱动源码、编译器 mtcc、Photon、Samples | 已确认 | CI 从 `linux-ddk` 根仓库拉取其他 repo [.mthreads-ci.yml:346-350] |

## 顶层目录职责

| 路径 | 职责 | 状态 | 证据 |
|---|---|---|---|
| `src/driver` | Driver API 入口、导出表、错误处理、MUPTI/MUGDB/MUASAN hooks | 已确认 | [src/driver/CMakeLists.txt:4-18] |
| `src/musa` | Driver API 背后的 C++ 对象模型和核心实现 | 已确认 | [README.md:56-58], [src/musa/core/CMakeLists.txt:1-11] |
| `src/hal` | HAL 接口与 M3D 适配 | 已确认 | [README.md:59-61], [src/hal/m3d/CMakeLists.txt:1-59] |
| `src/tools` | 构建工具程序；当前明确包含 `muInfo` | 已确认 | [src/tools/CMakeLists.txt:10-18] |
| `src/gdb` | debugger 相关接口/动态库组件 | 部分确认 | [src/CMakeLists.txt:11]，源码清单包含 `src/gdb/interface.cpp` |
| `src/util` | 工具函数、日志、执行器、库加载、OS 抽象 | 部分确认 | [src/CMakeLists.txt:7]，源码清单 |
| `tests` | `.cu` 示例/功能测试候选 | 部分确认 | 文件清单包含 `childGraph.cu`、`memcpyBatchAsync.cu` 等 |
| `unittest` | C++/gtest 单元测试入口 | 已确认 | [unittest/CMakeLists.txt:1-46] |
| `module_version` | 版本生成子模块 | 已确认 | [.gitmodules:1-4], [CMakeLists.txt:134-137] |

## 主要语言、框架和依赖

| 类别 | 内容 | 状态 | 证据 |
|---|---|---|---|
| 语言 | C++17（UNIX）、C++20（Windows），另有 CUDA-like `.cu` 测试 | 已确认 | [CMakeLists.txt:155-174], tests 文件清单 |
| 构建 | CMake >= 3.15 | 已确认 | [CMakeLists.txt:1] |
| 测试 | CTest + GoogleTest（启用 `MUSA_BUILD_UT` 后 FetchContent 拉取） | 已确认 | [CMakeLists.txt:214-237], [unittest/CMakeLists.txt:1-46] |
| 外部子模块 | `module_version`、`src/hal/m3d/m3d`、`src/musa_shared_include` | 已确认 | [.gitmodules:1-13] |
| CI 外部依赖 | `libdrm-mt`、`shared_include`、mt-toolchain、LLVM 14、MUSA toolkit、mtcc、Photon | 已确认 | [.ciConfig.yaml:13-16], [.mthreads-ci.yml:37-52], [.mthreads-ci.yml:114-123] |
| 运行依赖 | MTGPU 内核驱动、GPU 设备、MUSA 安装路径 | 推断/部分确认 | CI 中安装 DDK、`modprobe mtgpu`、设置 `/usr/local/musa` [.mthreads-ci.yml:125-148] |

## 构建、运行、测试、部署入口

| 入口 | 命令/文件 | 当前验证状态 | 证据 |
|---|---|---|---|
| 最小构建 | `mkdir build && cd build && cmake .. && make` | 未执行 | [README.md:9-16] |
| Debug 构建 | `cmake .. -DMUSA_BUILD_DEBUG=ON` | 未执行 | [README.md:39-45], [CMakeLists.txt:127-130] |
| 单元测试构建 | `-DMUSA_BUILD_UT=ON` | 未执行 | [CMakeLists.txt:214-237] |
| 单元测试发现 | `add_test(NAME ... COMMAND ...)` | 未执行 | [unittest/CMakeLists.txt:5-46] |
| 安装 | `./install.sh` 或 `bash ./install.sh -l <libdir> -b <bindir>` | 未执行 | [install.sh:27-105] |
| CI build | `./ddk_build.sh -a 0 -m 1` 从 linux-ddk 根仓库执行 | 未执行 | [.ciConfig.yaml:13-34] |
| 工具运行 | `muInfo` | 未执行 | [src/tools/CMakeLists.txt:10-18] |

## 模块候选清单

首批模块划分不是机械按目录切分，而是按职责、生命周期和调用边界确定：

1. M01 构建/发布/安装系统；
2. M02 Driver API 导出层；
3. M03 平台与设备管理；
4. M04 上下文与资源生命周期；
5. M05 内存与内存池；
6. M06 Stream/Command 调度；
7. M07 Module/Kernel 执行；
8. M08 Graph 执行；
9. M09 HAL/M3D 适配；
10. M10 工具与调试接口。

详见 [`../01-modules/module-registry.md`](../01-modules/module-registry.md)。

## 相关文档

- [`architecture.md`](architecture.md)
- [`build-and-deploy.md`](build-and-deploy.md)
- [`evidence-index.md`](evidence-index.md)
- [`analysis-state.md`](analysis-state.md)
- [`../01-modules/module-registry.md`](../01-modules/module-registry.md)

## 源码证据摘要

- README 目录职责：[README.md:51-64]
- CMake 版本与项目：[CMakeLists.txt:101-103]
- 子目录加入顺序：[src/CMakeLists.txt:7-12]
- driver 动态库：[src/driver/CMakeLists.txt:9-18]
- core 静态库：[src/musa/core/CMakeLists.txt:1-11]
- HAL/M3D 静态库：[src/hal/m3d/CMakeLists.txt:1-59]

## 未解决问题

1. 公开头文件 `src/musa_shared_include` 的 API 声明尚未逐文件分析。
2. `mu_wrappers_generated.cpp` 的生成链、导出符号和 wrapper 策略尚未完成。
3. 当前没有在本环境执行构建；命令仅来自 README/CMake/CI。

## 下一步阅读建议

继续阅读 [`architecture.md`](architecture.md)，理解分层架构与端到端初始化/内存/Kernel 路径。
