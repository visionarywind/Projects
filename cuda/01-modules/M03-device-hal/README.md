# M03 Device/HAL：设备发现、平台与架构分派

- **职责**：发现和筛选设备、维护 `CUdev` 状态、按 compute capability 选择架构 HAL，并把通用操作交给 RM/MRM/WDDM/amodel 等 DMAL。
- **证据状态**：设备重排和 HAL 静态分派已确认；具体运行时平台配置未知。
- **源码范围**：`src/cui/devmgr.c`、`device.c`、`cuidevice.c`、`dmal/`、`hal/`。

## 设备选择

`devmgr.c` 维护 remap table，校验范围和重复项，并可依据 PCI bus ID 或计算能力排序；还处理 emulation、Windows NVAPI、MPS 和环境筛选（静态确认：[src/cui/devmgr.c:39-121,175-199,201-277]）。因此 `CUdevice` ordinal 不应被简单理解成物理 PCI 顺序。

## HAL 选择

`InitDeviceArch` 按 `device->state.ccArch` switch；Kepler、Maxwell、Pascal、Volta、Turing、Ampere case 只有在对应 `NVCFG(GLOBAL_ARCH_*)` 编译宏启用时才存在（静态确认：[src/cui/hal/hal.c:8-79]）。`isSupportedComputeClass` 和 `bigPageSizeForArch` 采用同样编译时过滤（静态确认：[src/cui/hal/hal.c:82-157]）。

## 外部边界

DMAL 目录包含 RM、MRM、WDDM、amodel、MPS 等平台实现；当前文档确认其文件和接口存在，但没有外部 RM/NVRM 源码，不能将 `dmal.Init` 的最终内核行为写成已确认。
