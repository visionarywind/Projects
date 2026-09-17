# M02 ACL：模块入口

- 文档目的：解释 01-modules/M02-acl/README.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：`50be4c922`
- 证据状态：主要 API 和构建边界已确认
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M02-acl/README.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


ACL 是面向应用的 C/C++ AI 计算接口层，封装设备、Context、Stream、内存、模型、算子和媒体能力 `[acl/README.md:20-41]`。它不是硬件驱动；运行时能力通过 `rt*`/Runtime 接口获得。

## 关键入口

- `aclrtSetDeviceImpl`：ACL 设备选择包装 `[acl/runtime/device.cpp:47-59]`。
- `aclrtResetDeviceImpl`、`aclrtGetDeviceImpl`、`aclrtSynchronizeDeviceImpl`：设备生命周期和同步 `[acl/runtime/device.cpp:80-169]`。
- `aclrtGetDeviceUtilizationRateImpl`：设备资源查询 `[acl/runtime/device.cpp:210-225]`。
- Runtime 仓的 `acl_rt.cpp`：公开 ACL Runtime 符号映射、Hook 和 callback 注册 `[runtime/src/acl/aclrt/acl_rt.cpp:17-104]`。

## 子域

`runtime/`、`model/`、`op/`、`tensor`、`data_transfer`、媒体和轻量 API；具体目录清单需结合版本持续补全。

## 推荐路径

先读 [design.md](design.md)，再读 [interfaces.md](interfaces.md) 和 [call-chains.md](call-chains.md)。

## 限制

构建依赖 Toolkit、第三方库和安装路径；UT 文档注明不支持 root。无匹配 NPU 时不能验证真实设备行为。

## 文档元数据（规范补充）

- 文档目的：说明 `01-modules/M02-acl/README.md` 的源码分析范围、结论和维护入口。
- 适用范围：当前项目对应模块/入口的静态源码与测试分析。
- 对应源码版本：以本项目 `00-overview/analysis-state.md` 或同页版本字段为准。
- 证据状态：静态源码证据；未执行的构建、测试、GPU、网络或多进程行为保持“未验证”。
- 最后更新：2026-09-15
- 前置阅读：本项目根 README 与 `00-overview/analysis-state.md`。
- 后续阅读：本模块/示例的实现、测试和风险页面。

## 深度审计

| 分析对象 | 入口落地 | 正常路径 | 分支 | 异常 | 清理 | 数据生命周期 | 执行上下文 | 行级证据 | Demo 映射 | 状态/缺口 |
|---|---|---|---|---|---|---|---|---|---|---|
| `01-modules/M02-acl/README.md` | 已完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 已映射或不适用 | 部分完成：动态行为、边界或专用变体仍需验证 |

## 相关文档
- [项目入口](../../README.md)
- [分析状态](../../00-overview/analysis-state.md)
- [源码证据索引](../../00-overview/evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](../../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
