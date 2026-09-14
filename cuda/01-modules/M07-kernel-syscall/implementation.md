# M07 实现、调用链与边界

## Kernel launch syscall 链

```text
cuiLaunchKernel_nonreentrant
 → cuiProfilerLaunch
 → cuiLaunchSetup_common
 → cuiSyscallOnKernelLaunch
 → syscall resources / generated device ABI
 → hal.launchCheck + encode ABI/QMD
 → stream push
```

`cuiSyscallOnKernelLaunch` 在通用 launch setup 中先于 HAL 检查和常量编码；失败映射为 launch out of resources，并终止提交（静态确认：[src/cui/cuilaunch.c:229-319]）。CNP 的 module-level imports 与 function imports 合并，child launch 继续依赖父 stream 的异步资源（静态确认：[src/cui/cuilaunch.c:164-218]）。

## Syscall trampoline、模块引用与资源

`cuiInitSyscallAsmFunc` 按当前设备架构从内建 cubin 表中取 syscall SASS/asm function module；`cuiSyscallInitTrampoline` 再查出原始 syscall function，将每个原函数的 device PC 填入 extern symbol table，链接 trampoline module，并把 trampoline/original function 与 syscall ID 绑定。这里的 PC 表和 trampoline ELF 是 runtime 与 device-side ABI 的连接点，但具体 cubin 内容和机器指令由生成/外部 compiler 链提供（静态确认：[src/cui/cuisyscall.c:124-205]；ABI 细节未知）。

module load 的 syscall import bitset 驱动 refcount：0→1 才执行一次 `OnModuleLoad`，即使之后引用归零也保留 initialized 状态；按需注册 timeout ISR 并激活 trap handler。module unload 则递减 active/refcount，必要时注销 ISR、deactivate trap handler。这个生命周期使同一 context 中多个 module 可以共享 syscall 初始化，但也要求 unload 的 bitset 和 callback 失败回滚保持一致（静态确认：[src/cui/cuisyscall.c:1229-1322]；卸载：[src/cui/cuimod.c:2880-2921]）。

launch callback 在 `callbackLock` 下串行执行，按 syscall 的 shared-memory 需求递增窗口；`cuiSyscallGetResources` 对 imports 汇总最大寄存器/同步栈和 shared memory 总量，供后续 HAL launchCheck 使用。callback 返回值被限制在受控错误集合，失败不会继续写 ABI/QMD（静态确认：[src/cui/cuisyscall.c:1151-1227,1325-1347]）。

## Context syscall library

`cuiSyscallInit` 先按编译生成的 syscall 名称表填充 context library，依据设备 DMAL/preemption/CILP 与 cooperative launch 能力决定是否开启 timeout service routine；随后创建 callback lock、reason buffer，必要时加载架构 syscall asm module，再加载 syscall cubin、trampoline、entry/exit 与 cooperative-groups entry modules。各 syscall 的 `OnModuleLoad`、`OnKernelLaunch`、trap/debugger/context-destroy 回调在此绑定；失败统一 `cuiSyscallDestroy`（静态确认：[src/cui/cuisyscall.c:378-568]）。reason buffer 是 host、driver-owned、host/device mapped 的 memobj，大小按 warp 数和启用 SM 数计算（静态确认：[src/cui/cuisyscall.c:570-607]）。

module load 时，syscall imports bitset 驱动引用计数；0→1 才调用一次 `OnModuleLoad` 并激活 syscall。API 版本过低返回 `CUDA_ERROR_INVALID_IMAGE`，assert 在已知坏平台返回 OS error；激活 timeout ISR 和 trap handler 也属于 module-load 成功条件（静态确认：[src/cui/cuisyscall.c:1229-1322]）。

launch 时把 function imports 与 CNP imports 合并，在 callback lock 下依次调用每个 syscall 的 `OnKernelLaunch`；shared-memory window 按 syscall 需求递增，并在需要时更新 per-context syscall constant bank。合法 callback 错误仅限 out-of-memory、launch-failed 和 shared-object-init-failed 等受控集合（静态确认：[src/cui/cuisyscall.c:1151-1227]）。资源聚合取所有引用 syscall 的最大 register/sync-stack 需求和 shared-memory 总量（静态确认：[src/cui/cuisyscall.c:1325-1347]）。

## Trap、timeout 与销毁

timeout ISR 调用 software trap handler 和 cooperative-group timeout service；destroy 先注销 ISR，逆序执行每个已初始化 syscall 的 context-destroy callback，再卸载 trampoline、CG entry、syscall module，释放 reason buffer 和 callback lock，最后同步可能由 destroy callback 推入的 GPU work（静态确认：[src/cui/cuisyscall.c:610-734]）。trap handler 从 reason buffer 读取每个 SM/warp 的 syscall reason，生成 trapped syscall bitset，并可清零已处理 reason（静态确认：[src/cui/cuisyscall.c:762-829]）。

## 生成物边界

D01 的 `gpuIncrement` 在 device 上递归发起 child launch；主测试依据 `supportsCnp` 决定是否将 child depth 纳入 expected（静态确认：[tests/cuda_test/basic_sanity.cu:136-145,214-225]）。这把 M07 的 CNP syscall/launch queue 与 M06/M05 的 stream 提交连接起来，但测试只证明静态代码意图，未证明运行成功。

## 生成物边界

`cudaAsm.nvmk`、`cudaSyscalls.nvmk` 是构建输入，不等于生成后的 device code。`.cubins.c`、`.bin` 等产物应作为边界/回归资料，不能替代源级 ABI 分析；生成器和 compiler/gpgpucomp 不在当前树中完整提供。
