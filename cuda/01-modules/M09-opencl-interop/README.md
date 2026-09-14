# M09 OpenCL/Interop：ICD、OpenCL 对象与图形互操作

- **职责**：提供 OpenCL ICD loader/dispatch，以及 NVIDIA OpenCL 实现和 CUDA/GL/D3D 等共享资源入口。
- **层次**：Khronos ICD loader 负责 vendor 枚举和入口转发；`src/cl` 负责 NVIDIA vendor 的 platform/device/context/queue/memory/program/kernel/event；底层仍复用 CUDA CUI、memobj、stream 和同步设施。
- **证据等级**：ICD vendor enumeration、dispatch table、OpenCL globals 初始化和配置已静态确认；具体 `src/cl` 到 `cuiextinterop.c` 的每条资源转换链尚未全部闭合。

`OpenCL application → ICD exported entrypoint → vendor platform/device dispatch → src/cl CLI object → CUI context/memobj/stream/event → HAL/RM` 是当前树可见的主边界。

## 已补齐的生命周期证据

- **静态确认**：所有 public OpenCL object 的首字段是 `CLIobjectData`，同时保存 ICD dispatch、API/internal refcount 和 parent/children 树；因此 OpenCL public retain/release 与底层 CUI 引用不是同一计数器（[src/cl/cliobject.h:98-169]）。
- **静态确认**：context destroy 先停 worker/callback thread、销毁 task graph，再清 pinned-memory tracking、销毁每个 `CUctx` 和迁移锁；OpenGL interop context 还会触碰 GL library TLS 以避免 close-context 崩溃（[src/cl/clicontext.c:467-545]）。
- **静态确认**：`clWaitForEvents` 先等待各 event 标记为 submitted，再按 device 合并 marker，flush channel，必要时 wait；GPU 已完成的快速路径显式清理 pinned memcpy tracking（[src/cl/clevent.c:106-217]）。
- **静态确认**：GL object 创建建立 `CLI_OBJECT_MEM`、GL resource registration、map bookkeeping；失败时 unregister resource 并销毁 object（[src/cl/clgl.c:93-204]）。D3D 入口同样是外部资源登记层，底层 acquire/release 的设备语义仍需结合 CUI interop。
- **静态确认**：CUDA external memory import 经 DMAL handle open，映射 buffer/semaphore 时创建带 external sharing 标志的 `CUmemobj`；分配失败走 `memobjFree`，具体 handle close 和返回给外部 API 的最终所有权仍受 DMAL 实现控制（[src/cui/cuiextinterop.c:21-110,114-171]）。

OpenCL enqueue、worker task graph 与每类 GL/D3D acquire/release 的完整动态时序，以及外部 vendor/RM 端行为，仍标为未知或未验证。
