# M09 数据结构

| 对象 | 作用 |
|---|---|
| `KHRicdVendor` | vendor library、platform 数组和 suffix 链表节点 |
| `CLIicdDispatchTable` | 固定 256-entry OpenCL ABI 函数表 |
| `CLIglobals` | OpenCL config、初始化状态、NVIDIA platform |
| public platform/device/context handles | 验证后转发到 vendor/platform dispatch |

vendor/platform 的 loader 所有权与 CUI context/memory/queue 所有权分层；完整对象销毁与 interop backing 仍需继续追踪（静态确认：[src/icd_rel/icd.c:43-227]；[src/cl/cliicd.c:6-14,24-235]）。
