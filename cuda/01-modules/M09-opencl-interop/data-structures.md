# M09 数据结构

- 文档目的：解释 01-modules/M09-opencl-interop/data-structures.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M09-opencl-interop/data-structures.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


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
