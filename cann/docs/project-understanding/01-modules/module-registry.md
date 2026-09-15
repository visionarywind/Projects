# 模块注册表

- 文档目的：登记稳定模块边界和依赖
- 适用范围：四个独立仓库
- 对应源码版本：见 [../00-overview/analysis-state.md](../00-overview/analysis-state.md)
- 证据状态：已确认目录与构建边界；跨仓依赖部分推断
- 最后更新：2026-09-10
- 前置阅读：`00-overview/architecture.md`
- 后续阅读：各模块 README
## 结论摘要

本页聚焦 01-modules/module-registry.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


| ID | 模块名称 | 一句话职责 | 源码目录 | 入口 | 对外接口 | 依赖/被依赖 | 测试边界 | 风险 |
|---|---|---|---|---|---|---|---|---|
| M01 | GE Graph Engine | 图表示、图编译优化、模型加载与执行 | `ge/api`、`base`、`graph_metadef`、`compiler`、`runtime`、`parser` | `ge::GEInitialize`、`GeSession`、`ModelV2Executor` | GE API、Session、ATC、Executor | 依赖 MetaDef/Runtime/ACL；被框架和 ACL/应用使用 | `ge/tests` | 高 |
| M02 | ACL | 应用侧 AI 编程 API 与模型/算子支持 | `acl/inc`、`runtime`、`model`、`single_op`、`c` | `aclrt*`、模型/单算子 API | `inc/external/acl` 下 ACL C/C++ API | 依赖 GE、Runtime、HAL、算子/工具包 | `acl/tests` | 高 |
| M03 | Runtime | `rt*` 核心运行时、资源管理、任务调度和维测 | `runtime/src/acl`、`src/runtime`、`src/dfx`、`src/platform` | `rtSetDevice`、`rtDeviceSynchronize`、`Api::Instance` | `include`、`pkg_inc/runtime` | 依赖 Driver、CSEC、平台包；被 ACL/GE 使用 | `runtime/tests` | 高 |
| M04 | Driver | HAL、SDK-driver、设备通信、队列和内核模块 | `driver/src/ascend_hal`、`src/sdk_driver`、`pkg_inc` | HAL C 接口、设备节点/IOCTL | `pkg_inc` HAL/包接口 | 依赖 Linux/内核/固件；被 Runtime 使用 | `driver/test` | 高 |

## 边界判定

- M01 与 M02：GE 负责图/模型语义和编译执行，ACL 负责应用 API；部分 ACL 目录内嵌/依赖 GE 头文件，因此不能简单按目录判定边界。
- M02 与 M03：ACL 的 `runtime/` 是 ACL API 适配层；Runtime 仓库的 `src/acl/` 同样提供 ACL Runtime 实现，版本配套时需区分其所有权。
- M03 与 M04：Runtime 对外导出 `rt*`，Driver 通过 HAL/设备接口提供硬件能力；具体函数映射因产品和闭源依赖存在动态分发。
- M04 内部：`ascend_hal` 是用户态/公共 HAL 层，`sdk_driver` 含设备侧/内核适配、队列、HDC、调度等，不能合并成单一实现层。

## 相关文档

- [M01-GE](M01-ge/README.md) · [M02-ACL](M02-acl/README.md) · [M03-Runtime](M03-runtime/README.md) · [M04-Driver](M04-driver/README.md)

## 源码证据摘要

`[ge/AGENTS.md:8-19]`、`[acl/README.md:20-41]`、`[runtime/AGENTS.md:35-49]`、`[driver/README.md:32-115]`。

## 未解决问题

ACL 仓与 Runtime 仓都含 `aclrt` 相关代码，需按目标 CANN 版本和打包清单确认最终链接哪个实现。

## 下一步阅读建议

按调用方向 M02 → M03 → M04，按模型方向 M01 → M03 → M04。
