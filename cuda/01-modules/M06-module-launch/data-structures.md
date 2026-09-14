# M06 数据结构

## 核心对象

| 对象 | 关键内容 | 依赖 |
|---|---|---|
| `CUmod` | module 镜像、context 归属、CNP/syscall imports | context、module memobj |
| `CUfunc` | 函数元数据、参数布局、grid/launch 状态 | `CUmod`、function memobj |
| `CUIlaunchData` | 本次 grid/block/shared memory、参数、QMD 和 ABI staging | stream、QMD、constant bank |
| `CUIkernelParams` | packed/unpacked 参数及大小 | function metadata、调用者参数 |
| `CUqmd` | 架构提交描述和 launch semaphore 信息 | M05 stream/QMD pool、M03 HAL |
| `CUsharedelf` | 共享 ELF image 与引用计数 | `globals.binload_cs`、module unload |
| `CUlinkState` | JIT/link options、elfLink handle、input copies、final cubin | `gpgpucompMutex`、external elfLink |
| `CUIgraphCtxData` | graph 每 context 的 marker、stream、QMD/device-node 资源统计 | context `internalsMutex` |

## 所有权与异步

module/function 由 context 相关链表管理；launch 期间 `doLaunchMemTracking` 把 function、syscall、module/function scope 和 constant-bank 资源纳入 tracking，防止 GPU 尚未完成时提前释放（静态确认：[src/cui/cuilaunch.c:163-218]）。`CUsharedelf` 的 image 由最后一个 module 引用释放；module unload 在释放自身资源前递减 syscall imports、注销 managed variables/system-wide atomics，并在最后发出 tools unload completed（静态确认：[src/cui/cuimod.c:2767-2943]）。`CUlinkState` 在 `cuiLinkComplete` 后释放 input copies，final cubin 由 state 持有到 destroy；`cuiLinkAddCubinData` 必须复制输入，因为 elfLink 要求数据持续到 finish（静态确认：[src/cui/cuijitlink.c:219-255,360-402]）。

Graph exec 保存每 context 的锁、completion marker、内部 stream/QMD 及 device scheduler node；instantiate 的资源分配失败销毁整个 exec graph，launch 期间用 marker 和 UVM DAG 保证跨 context/异步依赖（静态确认：[src/cui/cuigraph.c:2054-2175,3304-3492,4056-4162]）。
