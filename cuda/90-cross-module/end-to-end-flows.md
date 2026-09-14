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

## 关键分叉

- API 参数错误在 wrapper 终止。
- capture 与普通 stream 分叉。
- 工具可 skip launch，debugger/profiler 可改变等待策略。
- 异步 stream destroy 分叉为 detached 或 marker 完成后的 free。

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

