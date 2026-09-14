# M07 Kernel/Syscall：内建 Kernel、设备系统调用与生成物

- **职责**：提供驱动注入/辅助 kernel、device-side syscall 入口和架构汇编/生成代码；它不是普通用户 kernel 的编译前端。
- **证据状态**：syscall init/load/destroy、import refcount、资源聚合、kernel callback、trap/timeout/reason buffer、trampoline PC binding 和 per-arch cubin 生成规则已静态确认；device ABI、CNP queue 消费和 compiler/firmware 未闭合。
- **源码范围**：`src/kernels/`、`src/syscalls/`、`src/asm/`、`cudaSyscalls.nvmk`、`cudaAsm.nvmk`、HAL 的 `*_sass.c`/`*_qmd.c`。

## 在 launch 中的位置

CUI launch setup 在 HAL launch validation 前后调用 kernel syscall 处理，并将 syscall 引用纳入 launch memory tracking（静态确认：[src/cui/cuilaunch.c:163-218,229-319]）。因此 syscall 资源是 launch 的异步依赖，不是独立于 stream 的同步调用。

## 目录职责

`src/syscalls` 按 assert、cgs、cnp、debugger、graphs、malloc、printf、profile 等主题组织；`src/kernels` 按 CPU 架构和生成/内建 kernel 组织；`src/asm` 包含架构汇编和大量生成/目标相关文件。具体函数 ABI 受编译配置和外部 compiler/toolchain 影响。

## 构建证据

`cuda.nvmk` 将 kernel/syscall/asm 相关对象作为 `libcuda` 的共享构建输入，同时按架构宏纳入对应 HAL 源（静态确认：[cuda.nvmk:380-492,513-597]）。
