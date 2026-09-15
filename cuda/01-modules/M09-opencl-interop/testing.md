# M09 测试与验证

- 文档目的：解释 01-modules/M09-opencl-interop/testing.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M09-opencl-interop/testing.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


- ICD loader 的 vendor 缺失、非法 `(num_entries, platforms, num_platforms)` 组合和 platform handle 校验由 dispatch 源码覆盖（静态确认：[src/icd_rel/icd_dispatch.c:43-108]）。
- dispatch table 的固定 256-entry 顺序及平台条件是 ABI 静态检查重点（静态确认：[src/cl/cliicd.c:6-14,24-144,146-235]）。
- public/internal object tree、context destroy 顺序、event marker aggregation、GL resource registration 和 external memobj error rollback 可通过对应 host-side 源码核对（静态确认：[src/cl/cliobject.h:98-169]；[src/cl/clicontext.c:467-545]；[src/cl/clevent.c:106-217]；[src/cl/clgl.c:93-204]；[src/cui/cuiextinterop.c:21-171]）。
- OpenCL context/device/queue/memory、GL/D3D interop 的真实 vendor dispatch 需要运行时库和图形设备；本项目未执行。

“源码存在入口”不等于 vendor 能加载或对象操作成功，当前没有已验证的 OpenCL 结果。

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
