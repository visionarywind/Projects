# 跨模块系统串联

## 主运行时链

```text
cuInit
 → cuiInit / globals / TLS / managers
 → cuCtxCreate_v2 / CUctx / current TLS
 → cuMemAlloc_v2 / memdesc / memobj / UVA registration
 → cuStreamCreate / stream pool / QMD / semaphores
 → cuModule + cuFunction
 → cuLaunchKernel
 → parameter pack + syscall tracking + HAL check
 → streamBeginPush
 → HAL finalize/control + channelTrackQMD
 → streamEndPush
 → marker completion / optional synchronize
 → detach/reclaim / context destroy
```

证据分别见 `[src/api/apiinit.c:19-47]`、`[src/cui/cuiinit.c:3060-3208]`、`[src/api/apimem.c:52-118]`、`[src/cui/cuistream.c:1741-1877]`、`[src/cui/cuilaunch.c:229-338,582-817]`、`[src/cui/cuistream.c:1926-1954,2004-2088]`。

## OpenCL 与工具旁路

- OpenCL ICD/vendor dispatch 最终进入 `src/cl` public object；`CLIobjectData` 的 public/internal refcount tree 独立于 CUI `CUctx`、`CUmemobj` 和 GPU marker（[src/cl/cliobject.h:98-169]）。context destroy 必须先停 worker/callback thread 与 task graph，再清 pinned tracking 和 CUI contexts（[src/cl/clicontext.c:467-545]）。
- `clWaitForEvents` 先等 submitted，再按 device 聚合 marker；GPU completed 快速路径仍需显式 clear pinned memcpy tracking（[src/cl/clevent.c:106-217]）。
- Tools callback 从 launch begin 到 launch end 横跨 M06/M05；debugger 的 shared state、memcheck device table 和 profiler perfmon 可改变或观察主路径，但不取代 marker completion（[src/cui/cuilaunch.c:468-503,635-710,779-817]；[src/devtools/memcheck/memcheck.c:120-256]）。
- GL/external interop 在 host 侧登记 resource 或由 DMAL 打开 handle，再创建 memobj/semaphore backing；fence、handle close 与 RM 所有权仍是外部边界（[src/cl/clgl.c:93-204]；[src/cui/cuiextinterop.c:21-171]）。

## 共享状态

- `CUctx` 是 API 检查、memmgr、stream manager、module/function 和 sticky error 的关联根。
- `CUmemobj` 由 memory manager 建立，也被 launch tracking、UVA/P2P 和 tools 观察。
- `CUIstream` 连接 QMD、channel、constant bank、marker 和 UVM registration；GPFIFO 的 pushbuffer 空间只有在 tracking semaphore 完成后才能回收。
- `CUmod` 卸载同时撤销 function、shared ELF、syscall imports、UVM namespace/atomics、CNP cache 和 tools 状态。
## 异步与错误

提交成功只表示 pushbuffer 已建立/提交，不代表 GPU 完成；stream detach 将 public handle 立即失效，但保留内部对象至 marker 完成。launch 失败需要结束 push、撤销 UVM running 状态并发送 tools end callback。具体设备错误由外部 DMAL/RM 产生的部分未知。

## 关联入口

- 内存释放必须在 base pointer、API source 和 context 同步规则下进行。
- capture launch 不立即 push，而创建 graph node；普通 launch 才进入 M05 提交路径。
- OpenCL ICD 最终把 vendor dispatch 接到相同 CUI/context/memory/stream 体系，但 enqueue/object 生命周期尚未完全闭合。
- M10 通过 `basic_sanity` 和 stream/memory 单测验证这些边界；目前仅静态确认，未运行。
