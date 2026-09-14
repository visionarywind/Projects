# M03 证据、测试与开发指南

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
