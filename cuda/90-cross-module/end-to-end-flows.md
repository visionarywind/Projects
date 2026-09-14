# 端到端流程

## 初始化到完成

```text
cuInit → globals/TLS/managers → context
 → memobj allocation/registration
 → stream/QMD/channel
 → module/function + launchData
 → syscall/HAL setup
 → pushbuffer/channel submit
 → marker/synchronize
 → stream reclaim/context destroy
```

普通 launch 的 push 细节见 `[src/cui/cuilaunch.c:582-817]`；GPFIFO entry 在 tracking semaphore 完成后才推进 GPU get 和 pushbuffer 回收；capture launch 在 API 层形成 graph node 而非立即提交（`[src/api/apilaunch.c:252-286]`）。

## 显存池化路径

```text
cuMemAlloc
 → memobjAlloc
 → canSuballocate + HAL block size
 → compatible suballocator tree
 → best-fit free region / new memblock
 → DMAL allocation + VA mapping
 → launch/memobj tracking
 → free + coalesce / final memblock release
```

该路径中的 pool 是进程内、descriptor 分组的 memblock suballocator；公开 API、VA heap、DMAL 物理页分配和 Graph 执行资源池不能混为一谈（[src/cui/memobj.c:82-110,265-375]；[src/cui/suballocator.c:163-220,343-405]）。

## Graph 资源路径

```text
cuStreamBeginCapture
 → CUIgraph + origin/TLS/event state
 → cuLaunchKernel creates node
 → cuStreamEndCapture validates join/invalidation
 → cuGraphInstantiate clone/flatten
 → per-context QMD/const-bank/stream/marker/scheduler backing
 → cuGraphLaunch marker/UVM DAG/tracked push
 → cuGraphExecDestroy detach + reverse resource release
```

证据：[src/cui/cuistream.c:2180-2268]；[src/cui/cuigraph.c:3304-3492,1835-1933,4056-4162,1035-1205]。当前无专用 Graph 运行时 Demo，以上为静态源码轨迹。

## 关键分叉

- API 参数错误在 wrapper 终止。
- capture 与普通 stream 分叉。
- 工具可 skip launch，debugger/profiler 可改变等待策略。
- 异步 stream destroy 分叉为 detached 或 marker 完成后的 free。
- Graph instantiate/launch/destroy 形成独立资源分支；其 scheduler backing 进入 M04 memobj 路径，QMD/constant-bank/stream/marker 由 M05/M06 资源池维护。

## OpenCL/Interop 分支

```text
ICD vendor/platform
 → CLIobjectData public/internal refs
 → OpenCL context/queue/event
 → submitted event
 → per-device ctxMarker aggregation
 → flush/wait or GPU-complete pinned cleanup
 → worker/task/context destroy
```

GL/external memory 分支在 host 侧为外部资源建立 registration 或 `CUmemobj`/semaphore backing；它与普通 CUDA memobj 共享 memory manager，但外部 fence、handle close 和实际跨 API 完成语义未验证（[src/cl/clevent.c:106-217]；[src/cl/clgl.c:93-204]；[src/cui/cuiextinterop.c:21-171]）。

## 工具/调试分支

```text
launch begin
 → tools callback / debugger blocking-or-skip
 → memcheck table + profiler/perfmon
 → push/marker completion
 → launch end / context teardown
```

callback end 不代表 GPU 完成；工具的额外 device-visible allocation 必须跟随 context/launch 销毁顺序（[src/cui/cuilaunch.c:468-503,635-710,779-817]；[src/devtools/memcheck/memcheck.c:120-256]）。
