# M03 实现与动态选择

- 文档目的：解释 01-modules/M03-device-hal/implementation.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M03-device-hal/implementation.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## Remap 与排序

设备表先建立 identity mapping，再通过 predicate 过滤；过滤器在 predicate 失败时停止并报告失败。PCI 排序比较 domain/bus/device，速度排序以 physical TPC、SM 数、架构 multiplier 和 Windows TCC 权重计算（静态确认：[src/cui/devmgr.c:39-103,209-277]）。这解释了 CUDA ordinal、可见设备列表和 PCI 顺序可能不同。

## HAL 初始化

`InitDeviceArch` 的返回值是具体 `deviceInitArch*` 是否成功；未知架构直接返回 false。架构实现分别位于 `src/cui/hal/{kepler,maxwell,pascal,volta,turing,ampere}`，并由 `cuda.nvmk` 中 `NVCFG_GLOBAL_ARCH_*` 条件纳入构建（静态确认：[src/cui/hal/hal.c:35-79]、[cuda.nvmk:513-597]）。

## DMAL 选择

`channelInit` 会调用 `ctx->device->dmal.channelInitDmal(channel)`，随后调用已选 driver model 的 `channel->dmal.Init(channel)`；这一接口是 CUI 与平台后端的明确边界（静态确认：[src/cui/channel.c:78-101]）。RM/MRM/WDDM 的具体选择条件由外部构建和 device 状态共同决定，本快照无法确认某次运行选择哪一个。

## 架构 ABI

HAL 还负责 memblock 大小、QMD、GPFIFO、semaphore、launch 和 memcpy 等操作；通用代码通过 `CUdev->hal` 调用。每个架构文件族结构相似并不证明实现语义完全相同，必须逐目标读取。

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
