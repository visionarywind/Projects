# M03 设计

M03 将“可见 device ordinal”“物理 device”“运行时 HAL”分开：remap table 决定用户看到的 ordinal，`CUdev.state.ccArch` 决定架构，编译宏决定候选实现是否进入 binary。设备性能排序是策略，不是硬件 ID 的同义词（静态确认：[src/cui/devmgr.c:39-121,201-277]）。

DMAL 为平台/RM/WDDM/MRM 等 backend 提供统一 channel 操作，HAL 为架构编码和能力查询提供函数表。
