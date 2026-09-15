# M09 风险与技术债

- 文档目的：解释 01-modules/M09-opencl-interop/risks-and-debt.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M09-opencl-interop/risks-and-debt.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


| 风险 | 静态依据 | 后果 |
|---|---|---|
| dispatch table 漂移 | 256-entry 顺序填充 `[src/cl/cliicd.c:6-14,32-109]` | ABI 入口错位 |
| 平台选择歧义 | properties/device type/first vendor fallback `[src/icd_rel/icd_dispatch.c:217-247]` | context 落到错误 vendor |
| 动态库生命周期 | vendor add 临时 unload 与节点保存 `[src/icd_rel/icd.c:70-208]` | function pointer/platform 失效 |
| feature macro 差异 | OpenCL 1.1/1.2/2.0 条件 `[src/cl/cliicd.c:128-235]` | 入口在不同 binary 不同 |
| interop 链未闭合 | GL/D3D 文件存在但转换实现未全读 | 无法确认 sync/ownership |
| 多重引用计数 | `CLIobjectData` public/internal/tree `[src/cl/cliobject.h:98-169]` | public release 可能早于 CUI/GPU 完成或反之 |
| context 销毁顺序敏感 | thread/task graph/pinned memory/CUctx 顺序 `[src/cl/clicontext.c:467-545]` | 后台 worker 或异步 memcpy 使用已释放状态 |
| event fast path 旁路普通 wait | marker completed 分支手动 clear pinned tracking `[src/cl/clevent.c:182-213]` | 新增等待路径若漏清理会留下 pinned memory |
| external memobj owner 不明 | DMAL open 与 `memobjAlloc` `[src/cui/cuiextinterop.c:21-171]` | handle close、device pointer 和 backing 释放时机不清 |

**深度等级：中→深（host-side）。** ICD/vendor、public/internal object tree、context destroy、event marker aggregation、GL registration 和 external memobj creation 已覆盖；OpenCL enqueue 到 worker/CUI 的所有任务类型、D3D/GL fence 的 RM 语义、外部 handle close 和实际运行仍未闭合。

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
