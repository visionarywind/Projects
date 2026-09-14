# M07 行级分析

- `[src/cui/cuilaunch.c:177-199]`：function syscall imports 与 CNP module imports 合并，逐个追踪 syscall function memobj。
- `[src/cui/cuilaunch.c:255-269]`：在 HAL launchCheck 前调用 syscall callback，失败返回 launch resource error。
- `[src/cui/cuilaunch.c:291-316]`：cooperative launch 的 CNP/occupancy patch 条件。
- `[src/cui/cuisyscall.c:约 430-560]`：架构 syscall asm module、extern PC 表和 trampoline link。
- `[src/cui/cuisyscall.c:1151-1227]`：serialized `OnKernelLaunch` callback、shared-memory window 和 const-bank update。
- `[src/cui/cuisyscall.c:1229-1322]`：module import refcount、OnModuleLoad、timeout ISR/trap activation。
- `[src/cui/cuimod.c:2880-2921]`：unload 时 syscall refcount、ISR 和 trap deactivation。
- `[cudaSyscalls.nvmk:46-71,111-160]`：按架构 nvcc/bin2c 生成 CUDA/OpenCL syscall cubin arrays。
- `[tests/cuda_test/basic_sanity.cu:136-145]`：device kernel 递归 child launch 的编译条件。
- `[tests/cuda_test/basic_sanity.cu:214-225]`：测试按 CNP capability 计算 expected 并同步观察结果。
