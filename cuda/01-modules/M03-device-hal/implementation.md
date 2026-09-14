# M03 实现与动态选择

## Remap 与排序

设备表先建立 identity mapping，再通过 predicate 过滤；过滤器在 predicate 失败时停止并报告失败。PCI 排序比较 domain/bus/device，速度排序以 physical TPC、SM 数、架构 multiplier 和 Windows TCC 权重计算（静态确认：[src/cui/devmgr.c:39-103,209-277]）。这解释了 CUDA ordinal、可见设备列表和 PCI 顺序可能不同。

## HAL 初始化

`InitDeviceArch` 的返回值是具体 `deviceInitArch*` 是否成功；未知架构直接返回 false。架构实现分别位于 `src/cui/hal/{kepler,maxwell,pascal,volta,turing,ampere}`，并由 `cuda.nvmk` 中 `NVCFG_GLOBAL_ARCH_*` 条件纳入构建（静态确认：[src/cui/hal/hal.c:35-79]、[cuda.nvmk:513-597]）。

## DMAL 选择

`channelInit` 会调用 `ctx->device->dmal.channelInitDmal(channel)`，随后调用已选 driver model 的 `channel->dmal.Init(channel)`；这一接口是 CUI 与平台后端的明确边界（静态确认：[src/cui/channel.c:78-101]）。RM/MRM/WDDM 的具体选择条件由外部构建和 device 状态共同决定，本快照无法确认某次运行选择哪一个。

## 架构 ABI

HAL 还负责 memblock 大小、QMD、GPFIFO、semaphore、launch 和 memcpy 等操作；通用代码通过 `CUdev->hal` 调用。每个架构文件族结构相似并不证明实现语义完全相同，必须逐目标读取。
