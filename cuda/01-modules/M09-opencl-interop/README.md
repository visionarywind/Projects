# M09 OpenCL/Interop：ICD、OpenCL 对象与图形互操作

- **职责**：提供 OpenCL ICD loader/dispatch，以及 NVIDIA OpenCL 实现和 CUDA/GL/D3D 等共享资源入口。
- **层次**：Khronos ICD loader 负责 vendor 枚举和入口转发；`src/cl` 负责 NVIDIA vendor 的 platform/device/context/queue/memory/program/kernel/event；底层仍复用 CUDA CUI、memobj、stream 和同步设施。
- **证据等级**：ICD vendor enumeration、dispatch table、OpenCL globals 初始化和配置已静态确认；具体 `src/cl` 到 `cuiextinterop.c` 的每条资源转换链尚未全部闭合。

```text
OpenCL application
 → ICD exported entrypoint
 → vendor platform/device dispatch
 → src/cl CLI object
 → CUI context/memobj/stream/event
 → HAL/RM
```
