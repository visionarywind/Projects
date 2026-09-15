# M03 设计

- 文档目的：解释 01-modules/M03-device-hal/design.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M03-device-hal/design.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


M03 将“可见 device ordinal”“物理 device”“运行时 HAL”分开：remap table 决定用户看到的 ordinal，`CUdev.state.ccArch` 决定架构，编译宏决定候选实现是否进入 binary。设备性能排序是策略，不是硬件 ID 的同义词（静态确认：[src/cui/devmgr.c:39-121,201-277]）。

DMAL 为平台/RM/WDDM/MRM 等 backend 提供统一 channel 操作，HAL 为架构编码和能力查询提供函数表。

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
