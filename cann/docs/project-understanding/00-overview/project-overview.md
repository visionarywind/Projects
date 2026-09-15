# 项目定位

- 文档目的：说明四个独立仓库共同解决的问题与边界
- 适用范围：`ge/`、`acl/`、`runtime/`、`driver/`
- 对应源码版本：GE `8ee1b040a`；ACL `50be4c922`；Runtime `50be4c922`；Driver `977336913`
- 证据状态：部分确认；跨仓调用关系中部分依赖通过构建配置推断
- 最后更新：2026-09-10
- 前置阅读：根 README
- 后续阅读：architecture.md、四个模块 README

## 结论摘要

这是一个按软件栈分层组织的 CANN 源码工作区，而不是单一可直接构建的项目。GE、ACL、Runtime、Driver 各自独立演进和发布，通过稳定的 C/C++ 头文件、共享库符号、错误码和 CMake package 查找关系协作。

## 事实与边界

| 层 | 解决的问题 | 主要产物/边界 | 证据 |
|---|---|---|---|
| GE | 将模型或前端图变成可执行模型，并负责图优化和模型级执行控制 | `ge_compiler`、`ge_executor`、`dflow-executor` 等组件 | `[ge/README.md:10-13]`、`[ge/CMakeLists.txt:30-47]` |
| ACL | 面向应用提供 Device、Context、Stream、内存、模型、算子和媒体 API | `libascendcl` 及 C/C++ 头文件 | `[acl/README.md:20-41]`、`[acl/CMakeLists.txt:167-190]` |
| Runtime | 实现 `aclrt*` 和 `rt*` 的资源管理、任务提交、运行时对象和维测 | `cann-npu-runtime` 包，`acl_rt_impl`、runtime 核心库 | `[runtime/README.md:7-14]`、`[runtime/src/CMakeLists.txt:13-32]` |
| Driver | 将用户态请求转为 HAL/设备管理/通信/队列/内核驱动操作 | HAL/SDK-driver 动态库、内核模块和配套包 | `[driver/README.md:9-16]`、`[driver/CMakeLists.txt:31-57]` |

## 用户和系统边界

外部调用者可以是 PyTorch/TorchAir、TensorFlow Adapter、atc、C/C++ 应用或 Python API。GE 文档确认其支持前端接入和 ONNX/PB 等模型格式 `[ge/README.md:10-13]`。硬件设备、驱动固件、CANN 安装包和第三方库属于仓库外部依赖。

## 关键限制

- 版本必须配套：Runtime README 明确要求选择配套 CANN 版本和 Git 标签，使用 master 可能产生版本不匹配 `[runtime/README.md:16-19]`。
- 硬件依赖：Runtime 示例需要驱动与固件；仅编译可以跳过设备安装 `[runtime/README.md:85-94]`。
- 当前工作区无根级 Git 历史；四个目录分别是 Git 仓库，提交号不能混写成一个版本。
- 当前环境未运行构建、UT、硬件样例；所有验证结论均来自源码和文档静态证据。

## 相关文档

## 源码证据摘要

参见上表及 [evidence-index.md](evidence-index.md)。

## 未解决问题

发布系统实际将四仓库绑定到哪个版本矩阵，需查 release-management。

## 下一步阅读建议

阅读 [architecture.md](architecture.md)，再按任务选择模块文档。
