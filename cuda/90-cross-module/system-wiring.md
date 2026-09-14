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
