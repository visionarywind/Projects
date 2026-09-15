# M02 ACL 行级分析

- 文档目的：解释 01-modules/M02-acl/line-level-analysis.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cann HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：关键设备路径已确认
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M02-acl/line-level-analysis.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。

## `aclrtSetDeviceImpl`

1. 记录 API 统计并输出开始日志 `[acl/runtime/device.cpp:47-50]`。
2. 调用 `rtSetDevice(deviceId)`，将设备选择交给 Runtime `[acl/runtime/device.cpp:51]`。
3. 失败时通过 `ACL_GET_ERRCODE_RTS` 转换 Runtime 错误并返回 `[acl/runtime/device.cpp:52-55]`。
4. 成功时更新统计并返回 `ACL_SUCCESS` `[acl/runtime/device.cpp:56-59]`。

## `aclrtGetDeviceImpl`

先检查输出指针，再调用 `rtGetDevice`；因此空指针错误在 ACL 层即可拦截 `[acl/runtime/device.cpp:127-138]`。

## 利用率查询

函数检查保留参数，然后分别读取 AICore、Vector Core 和 AICPU 使用率；内存利用率当前不支持 `[acl/runtime/device.cpp:210-225]`。调用方不应将未支持字段当作有效数值。

## 复杂度

设备 API 的参数检查和单次委托为 O(1)；模型 IO、buffer 和任务数量相关接口可能为 O(n)，需按具体实现测量。

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
