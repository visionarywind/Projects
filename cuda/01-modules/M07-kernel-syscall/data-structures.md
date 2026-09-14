# M07 数据结构

| 数据 | 作用 | 生命周期 |
|---|---|---|
| syscall imports bitset | 表示函数/模块需要的 device-side syscall | module/function 建立时形成，launch setup 读取 |
| `CUsyscall`/`scFunc` | syscall 对应的 function 与 memobj | context syscall library 持有；launch tracking 保活 |
| CNP launch queue 状态 | 支撑 device child launch | 父/子 launch 异步期间有效，具体实现依赖 `cuicnplaunchqueue.c` 和外部 kernel |
| `ctx->syslib.prt` reason buffer | 每 warp/SM 的 trap reason，host/device mapped | syscall init 创建，context destroy 释放 |
| syscall callback lock | 串行化 module/kernel/trap/debugger/resize 回调 | syscall library 初始化到 destroy |
| `activeSyscalls` + `refcount` | 跟踪模块引用和当前激活 syscall | module load/unload 改变 |
| `CUIsyscallResources` | 聚合 register/shared-memory/sync-stack 需求 | launch setup 临时值 |
| 生成 kernel/asm blob | 驱动辅助代码和架构机器码 | 构建产物或加载镜像，不等同手写 C 源 |

可确认的内存关系是 syscall function 的 `memobj` 在 launch memory tracking 中登记；具体引用计数和销毁函数需结合 `cuisyscall.c` 及各 syscall 子目录继续核对（静态确认：[src/cui/cuilaunch.c:164-218]）。

syscall library 还持有 syscall/trampoline/entry-exit/CG modules、reason buffer、per-context constant data 和 callback lock；这些对象按 context 创建，destroy 逆序释放，并在 destroy callback 可能产生 GPU work 后执行同步（静态确认：[src/cui/cuisyscall.c:378-568,610-734]）。`refcount` 以 module imports 为粒度，`activeSyscalls` 只在引用从 0 变为 1 或回到 0 时切换（静态确认：[src/cui/cuisyscall.c:1229-1273]）。
