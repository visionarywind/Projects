# M03 证据、测试与开发指南

- 文档目的：解释 01-modules/M03-device-hal/source-map.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M03-device-hal/source-map.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## 行级证据

- `[src/cui/devmgr.c:39-72]` remap table 初始化与合法性检查。
- `[src/cui/devmgr.c:75-103]` predicate 过滤和列表压缩。
- `[src/cui/devmgr.c:209-237]` PCI bus 排序。
- `[src/cui/devmgr.c:241-277]` 速度排序及架构/TCC 权重。
- `[src/cui/hal/hal.c:35-79]` architecture switch。
- `[src/cui/channel.c:78-101]` DMAL channel 初始化边界。

## 测试

`basic_sanity` 对每个 `getCudaGpus()` 结果运行 context/memory/kernel 逻辑，而不是假设单个 GPU；测试还检查 context device 与枚举 GPU 对象相同（静态确认：[tests/cuda_test/basic_sanity.cu:67-100,181-195]）。测试未运行。

## 开发规则

新增架构支持至少要同步：HAL include、`InitDeviceArch` case、compute class、`cuda.nvmk` 源文件条件、设备状态/limits、launch/QMD/GPFIFO 等实现；不可只复制一个 `*_init.c`。具体完整清单应以 `cuda.nvmk` 和目标架构目录为准。

## 未知

当前外部构建没有提供 `NVCFG_GLOBAL_ARCH_*` 的实际值、`NV_TARGET_OS`、DMAL 最终选择以及 RM 控制调用结果。

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
