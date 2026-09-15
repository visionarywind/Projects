# 端到端流程

- 文档目的：解释 90-cross-module/end-to-end-flows.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 90-cross-module/end-to-end-flows.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


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

Graph 的 instantiate 是一次面向设备能力的执行计划编译：先 flatten/lower，再拓扑排序，将简单同 context kernel 串分到 QMD chaining，将异构/跨 context/CNP/分叉边分到 pushbuffer 或 device scheduler；launch 通过 per-node 和 per-context marker 把计划重新接入用户 API stream。该调度选择和资源生命周期属于 M06/M05/M04 的交界，当前为 host-side 静态证据（[src/cui/cuigraph.c:2534-2923,3495-4162]）。

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

## 相关文档
- [项目入口](../README.md)
- [分析状态](../00-overview/analysis-state.md)
- [源码证据索引](../00-overview/evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
