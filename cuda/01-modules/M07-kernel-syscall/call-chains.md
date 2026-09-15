# M07 调用链与边界

- 文档目的：解释 01-modules/M07-kernel-syscall/call-chains.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M07-kernel-syscall/call-chains.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


```text
module/function launch
 → doLaunchMemTracking
 → cuiLaunchSetup_common
 → cuiSyscallOnKernelLaunch
 → syscall imports / device-side support
 → hal.launchCheck
 → ABI constant-bank/QMD encoding
 → streamBeginPushWithFlags
 → architecture launchControl
```

`cuiSyscallOnKernelLaunch` 在通用 launch setup 中先于 HAL 检查和常量编码；失败映射为 launch out of resources，并终止提交（静态确认：[src/cui/cuilaunch.c:229-319]）。CNP 的 module-level imports 与 function imports 合并，child launch 继续依赖父 stream 的异步资源（静态确认：[src/cui/cuilaunch.c:164-218]）。

## Syscall 初始化与模块引用

```text
cuiSyscallInit
 → syscall name table
 → callback lock + reason buffer
 → optional asm module
 → syscall cubin
 → trampoline / entry-exit / CG entry modules
 → bind per-syscall callbacks

module load
 → mod->syscallImports
 → refcount 0→1
 → OnModuleLoad once
 → timeout ISR / trap handler activation

module unload
 → decrement refcount
 → clear active bit at zero
 → unregister ISR/trap when no longer needed
```

静态确认：[src/cui/cuisyscall.c:378-568,668-734,1229-1322]。

## Kernel syscall resource path

```text
function/module syscallImports
 → cuiSyscallGetResources
    max register / total shared memory / max sync stack
 → cuiSyscallOnKernelLaunch
    function imports | CNP imports
    callback-serialized shmem window
    per-context syscall const-bank update
 → hal.launchCheck + ABI/QMD encoding
```

静态确认：[src/cui/cuisyscall.c:1151-1227,1325-1347]。

## 生成物边界

各 syscall 的设备 ABI、CNP queue 的生产/消费、compiler/gpgpucomp 生成 cubin 和 firmware 消费不在当前源码证据中完整出现，不能推断具体寄存器或最终机器码。

## Trampoline 与生成 cubin

```text
cudaSyscalls.nvmk
 → nvcc per DEV_ARCHS compile syscall_device_shared.cu
 → bin2c embed per-arch cubin
 → generated syscalls.cubins.c BIKernel table
 → cuiGetBIKCubin(arch)
 → cuiInitSyscallAsmFunc / cuiSyscallInitTrampoline
 → runtime CUfunc + device-side syscall entry
```

构建规则同时生成 CUDA 与 OpenCL syscall cubin 数组；每个架构先生成 `.cubin`，再由 `bin2c --static --const --type longlong` 转为 C 数组，并通过数组 entry 记录架构名、地址和大小。该链能静态确认“生成输入—内嵌镜像—运行时按架构选择”，但不能证明当前环境实际执行过 nvcc 或生成物与设备匹配（静态确认：[cudaSyscalls.nvmk:46-71,111-160]）。

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
