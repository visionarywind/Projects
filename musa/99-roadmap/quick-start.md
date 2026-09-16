# 快速开始

- 文档目的：给新开发者提供理解、构建、运行和首次修改 MUSA Driver 的最短路径，并明确当前未验证项。
- 适用范围：`/home/shanfeng/workspace/linux-ddk/musa` 远端仓库。
- 对应源码版本：`b8dce2b2f23849e8e99350c20d7eaac3c129caba`。
- 证据状态：部分推断/命令未验证
- 最后更新：2026-09-16
- 前置阅读：[`../README.md`](../README.md)
- 后续阅读：[`../00-overview/build-and-deploy.md`](../00-overview/build-and-deploy.md)

## 结论摘要

这是一个依赖私有子模块、M3D、内核驱动和 GPU 环境的用户态 Driver 仓库。新开发者应先完成只读确认和源码阅读，再在明确授权和目标环境满足时执行构建/安装。当前知识库没有实际运行构建命令，因此所有命令结果均为未验证。

## 1. 准备环境

### 1.1 获取源码和版本

在可访问远端机器时：

```bash
ssh shanfeng@10.20.34.9
cd /home/shanfeng/workspace/linux-ddk/musa
git rev-parse HEAD
git log --oneline -5
```

当前确认版本：`b8dce2b2f23849e8e99350c20d7eaac3c129caba`。

### 1.2 确认子模块

```bash
git submodule status
```

预期至少包含：

- `module_version`
- `src/hal/m3d/m3d`
- `src/musa_shared_include`

证据：[.gitmodules:1-13]。  
状态：命令未执行；子模块条目已确认。

### 1.3 安装依赖（未验证）

README 提到：

```bash
sudo apt-get install libelf-dev gcc-multilib
```

证据：[README.md:3-6]。  
注意：还需要 gr-umd 的 `libsrv_um` 和 `libusc`，README 只列出标题未给具体命令 [README.md:7]。

## 2. 构建

### 2.1 最小构建（未验证）

```bash
mkdir build
cd build
cmake ..
make
```

来源：[README.md:9-16]。  
预期产物：`build/lib/libmusa.so*`、`build/bin/muInfo` 等。产物推断来自 CMake 和 install 脚本 [src/driver/CMakeLists.txt:9-50]、[src/tools/CMakeLists.txt:10-18]、[install.sh:100-105]。

### 2.2 Debug 构建（未验证）

```bash
mkdir build
cd build
cmake .. -DMUSA_BUILD_DEBUG=ON
make
```

来源：[README.md:39-45]。

### 2.3 单元测试构建（未验证）

```bash
cmake -S . -B build -DMUSA_BUILD_UT=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

说明：这是从 CMake/CTest 结构推导的命令；当前未执行。启用后会通过 FetchContent 拉取 googletest [CMakeLists.txt:214-221]，每个 `unittest/**/*.cpp` 会成为一个 gtest 可执行和 CTest 项 [unittest/CMakeLists.txt:1-46]。

## 3. 运行最小示例

首选最小工具：`muInfo`。

未验证运行方式候选：

```bash
LD_LIBRARY_PATH=$PWD/build/lib:$LD_LIBRARY_PATH ./build/bin/muInfo
```

或安装后：

```bash
muInfo
```

状态：未运行。  
证据：tools CMake 会构建 `muInfo` 并链接 `libmusa.so` [src/tools/CMakeLists.txt:10-18]；install 脚本会复制 `muInfo` [install.sh:102-105]。

## 4. 安装（需要明确授权）

```bash
./install.sh
```

或：

```bash
./install.sh -l /usr/lib/x86_64-linux-gnu -b /usr/local/bin
```

警告：安装脚本会写系统库目录和 bin 目录，并复制 `libmusa*`/`muInfo` [install.sh:83-105]。不要在未授权的共享机器上执行。

## 5. 30 分钟阅读路线

1. 读 [`../README.md`](../README.md)：掌握一图、一表、主流程。
2. 读 [`../00-overview/project-overview.md`](../00-overview/project-overview.md)：确认边界、版本、依赖。
3. 读 [`../00-overview/architecture.md`](../00-overview/architecture.md)：理解 API → Core → HAL/M3D 分层。
4. 读 [`../01-modules/module-registry.md`](../01-modules/module-registry.md)：知道 M01-M10 的职责。
5. 读 [`../80-demos/demo-registry.md`](../80-demos/demo-registry.md)：选择 `muInfo` 作为第一条真实轨迹。

## 6. 半天理解核心流程

建议顺序：

1. 初始化：`muapiInit` → `Platform::Init`。
2. 设备查询：`muapiDeviceGetCount`、`muapiDeviceGetAttribute` → `Platform/Device`。
3. Context：`muapiCtxCreate_v2` → `Device::CreateContext` → `Context::Init`。
4. Memory：`muapiMemAlloc_v2` → `Context::CreateMemory` → `Memory::Init/GeneralAlloc`。
5. Stream：`muapiStreamCreate` → `Context::CreateStream`；再看 `Stream::Synchronize`。
6. Kernel：`muapiModuleLoad`、`muapiLaunchKernel` → `Context::GeneralLaunchKernel`。

## 7. 首次修改建议（尚未执行）

### 场景：修正文档/构建选项说明

低风险起点：核对 `CSV_UNSUPPORTED` 与 CMake `CSV_UNSPPORTED` 拼写差异。

阅读入口：

- [README.md:34-35]
- [CMakeLists.txt:202-204]

可能修改：

1. 若确认 CMake 拼写错误，修改 CMake 变量或兼容两种拼写；
2. 若确认 README 错误，修改 README；
3. 增加 CMake 配置验证说明。

验证：

```bash
cmake -S . -B build -DCSV_UNSUPPORTED=ON
cmake -S . -B build -DCSV_UNSPPORTED=ON
```

状态：建议，未执行；实际修复需维护者确认意图。

### 场景：新增一个 API 参数校验

阅读入口：M02 Driver API + 对应 core 实现。例如 memory API：

- `src/driver/mu_memory.cpp`
- `src/musa/core/context.cpp`
- `src/musa/core/memory.cpp`

步骤：

1. 在 API 层确认 public ABI 和错误码；
2. 在 core 层确认现有前置条件；
3. 添加最小校验；
4. 添加 gtest 或 `.cu` 回归；
5. 运行对应测试（当前未验证测试命令）。

### 场景：排查内存泄漏

阅读入口：

- `Platform::~Platform`/`ReleaseUnfreedMemories` [src/musa/core/platform.cpp:478-506]
- `Context::DestroyMemory` [src/musa/core/context.cpp:1089-1099]
- `Memory::~Memory` [src/musa/core/memory.cpp:360-431]

可选构建：

```bash
cmake -S . -B build-asan -DENABLE_ASAN_CHECK=ON -DMUSA_BUILD_DEBUG=ON
cmake --build build-asan
```

状态：未执行。

## 8. 不建议初期修改的模块

| 模块 | 原因 |
|---|---|
| M09 HAL/M3D | 硬件相关、外部子模块多、行为影响面大 |
| M06 Stream/Command | 异步、默认流、capture、同步语义复杂 |
| M08 Graph | 拓扑、节点生命周期和 capture/update 语义复杂 |
| M02 ABI 导出表 | 兼容性和符号导出风险高，需先理解 wrapper/export 机制 |

## 9. 常见问题快速定位

| 问题 | 首查文档 | 首查源码 |
|---|---|---|
| 构建目标是什么 | [`../00-overview/build-and-deploy.md`](../00-overview/build-and-deploy.md) | `src/driver/CMakeLists.txt` |
| `muInit` 做了什么 | [`../90-cross-module/system-wiring.md`](../90-cross-module/system-wiring.md) | `src/driver/mu_context.cpp`, `src/musa/core/platform.cpp` |
| 设备怎么枚举 | architecture/system-wiring | `src/musa/core/platform.cpp`, `src/musa/core/device.cpp` |
| 内存分配怎么落地 | analysis-state 下一批起点 | `src/driver/mu_memory.cpp`, `src/musa/core/memory.cpp`, `src/hal/m3d/memory.cpp` |
| Kernel 怎么提交 | analysis-state 下一批起点 | `src/driver/mu_module.cpp`, `src/musa/core/command/dispatchCommand.cpp`, `src/hal/m3d/queue.cpp` |
| Graph 从哪里开始 | module registry M08 | `src/driver/mu_graph.cpp`, `src/musa/core/graph*`, `src/musa/core/node/*` |

## 相关文档

- [`../00-overview/build-and-deploy.md`](../00-overview/build-and-deploy.md)
- [`../00-overview/analysis-state.md`](../00-overview/analysis-state.md)
- [`../80-demos/demo-registry.md`](../80-demos/demo-registry.md)

## 源码证据摘要

- README 构建：[README.md:9-16], [README.md:39-45]
- CMake 测试：[CMakeLists.txt:214-237], [unittest/CMakeLists.txt:1-46]
- tools/`muInfo`：[src/tools/CMakeLists.txt:10-18]
- 安装：[install.sh:27-105]

## 未解决问题

1. 未执行构建/测试/运行；需要目标环境授权。
2. `muInfo` 的源码轨迹尚未展开。
3. 上述首次修改建议没有经过维护者确认和测试。

## 下一步阅读建议

读取 [`../00-overview/analysis-state.md`](../00-overview/analysis-state.md) 的“下一批起点”，继续完成 D01 `muInfo` 和 M05 内存路径深挖。
