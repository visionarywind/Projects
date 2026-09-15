# M09 OpenCL/Interop：ICD、OpenCL 对象与图形互操作

- 文档目的：解释 01-modules/M09-opencl-interop/README.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M09-opencl-interop/README.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


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

## 文档元数据（规范补充）

- 文档目的：说明 `01-modules/M09-opencl-interop/README.md` 的源码分析范围、结论和维护入口。
- 适用范围：当前项目对应模块/入口的静态源码与测试分析。
- 对应源码版本：以本项目 `00-overview/analysis-state.md` 或同页版本字段为准。
- 证据状态：静态源码证据；未执行的构建、测试、GPU、网络或多进程行为保持“未验证”。
- 最后更新：2026-09-15
- 前置阅读：本项目根 README 与 `00-overview/analysis-state.md`。
- 后续阅读：本模块/示例的实现、测试和风险页面。

## 深度审计

| 分析对象 | 入口落地 | 正常路径 | 分支 | 异常 | 清理 | 数据生命周期 | 执行上下文 | 行级证据 | Demo 映射 | 状态/缺口 |
|---|---|---|---|---|---|---|---|---|---|---|
| `01-modules/M09-opencl-interop` | 已完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 已映射或不适用 | 部分完成：动态行为、边界或专用变体仍需验证 |

## 相关文档
- [项目入口](../../README.md)
- [分析状态](../../00-overview/analysis-state.md)
- [源码证据索引](../../00-overview/evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](../../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
