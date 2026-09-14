# M07 接口契约

- `cuiFuncUsesCnp`/syscall imports：决定额外 imports 和 module-level CNP imports。
- `cuiSyscallOnKernelLaunch`：在 HAL launchCheck 前完成 syscall setup；失败映射为 launch resource error。
- `doLaunchMemTracking`：为每个 syscall function memobj 建立异步引用。
- `cudaSyscalls.nvmk`/`cudaAsm.nvmk`：构建输入，不是运行时 callable ABI 的完整定义。

外部 compiler、RM 和 firmware 是该契约的不可见参与者。
