# M03 接口契约

- `InitDeviceArch(CUdev*)`：输入已探测 `ccArch` 的 device，输出是否成功；不保证所有架构都被编译。
- `dmal.channelInitDmal`：安装 channel-specific backend 函数表。
- `channel->dmal.Init`：创建平台 channel；成功后才分配 tracking/cross-engine semaphores 并插入 manager。
- `devmgr` remap：必须拒绝越界和重复 mapping。

HAL/DMAL 接口不等于外部 RM ABI；最终语义依赖外部树。
