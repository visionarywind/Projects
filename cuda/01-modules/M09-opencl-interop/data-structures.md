# M09 数据结构

| 对象 | 作用 |
|---|---|
| `KHRicdVendor` | vendor library、platform 数组和 suffix 链表节点 |
| `CLIicdDispatchTable` | 固定 256-entry OpenCL ABI 函数表 |
| `CLIglobals` | OpenCL config、初始化状态、NVIDIA platform |
| public platform/device/context handles | 验证后转发到 vendor/platform dispatch |
| `CLIobjectData` | 所有 public object 的 ICD 指针、public/internal refcount 和 parent/children 树 |
| `CUctxMarker` / event task | 将多个 OpenCL event 的 device 完成状态聚合为一个等待对象 |
| GL resource/map bookkeeping | 外部 GL name、resource array、map hash 与 OpenCL mem object 的关联 |
| `CUextMemory` / external `CUmemobj` | 外部 handle、shared memshare、buffer/semaphore backing 与 CUDA context 的关联 |

## 生命周期分层

`CLIobjectData` 的 public refcount、internal parent/child refcount、CUI `CUctx`/`CUmemobj` 引用、GPU marker completion 彼此独立。context destroy 必须先停后台线程和 task graph，再清 pinned memory 与 CUI contexts；external memory 的 handle close 则由 DMAL/CUI interop 层决定（静态确认：[src/cl/cliobject.h:98-169]；[src/cl/clicontext.c:467-545]；[src/cui/cuiextinterop.c:21-171]）。
