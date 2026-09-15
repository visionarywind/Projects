# M09 行级分析

- 文档目的：解释 01-modules/M09-opencl-interop/line-level-analysis.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M09-opencl-interop/line-level-analysis.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


- `[src/icd_rel/icd.c:43-51]`：全局 vendor list 和一次性初始化入口。
- `[src/icd_rel/icd.c:63-208]`：动态库、extension、platform IDs、suffix、vendor node 和临时资源清理。
- `[src/icd_rel/icd_dispatch.c:43-90]`：platform IDs 参数校验、输出清零和 vendor 枚举。
- `[src/cl/cliglobals.c:58-118]`：CUI 初始化、配置、dispatch/platform 创建和失败清零。
- `[src/cl/cliobject.h:98-169]`：public object 首字段、ICD dispatch、public/internal refcount、parent/children 树和 destroy contract。
- `[src/cl/clcommandqueue.c:15-76,189-215]`：queue 以 context 为 parent 创建，CUI queue 失败回滚及 public retain/release。
- `[src/cl/clicontext.c:467-545]`：worker/callback thread、task graph、pinned memory、CUI context 和 GL TLS 的销毁顺序。
- `[src/cl/clevent.c:106-217]`：event submitted 等待、按 device marker 聚合、flush/wait 和 pinned memcpy 清理。
- `[src/cl/clgl.c:93-204]`：GL mem object 建立、resource register、image/size 查询和错误 unregister。
- `[src/cui/cuiextinterop.c:21-171]`：external handle open、semaphore/buffer memobj backing 及错误 free。

这些证据闭合了 host-side 对象和等待路径；图形驱动 fence、DMAL close、worker 实际消费和 GPU interop 完成语义仍未验证。

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
