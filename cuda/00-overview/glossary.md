# 术语表

- 文档目的：解释 00-overview/glossary.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](analysis-state.md)。
## 结论摘要

本页聚焦 00-overview/glossary.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


| 术语 | 本项目含义 |
|---|---|
| API wrapper | `src/api` 中校验 ABI 和参数并调用 CUI 的公开入口 |
| CUI | CUDA User Interface 内部运行时实现层 |
| TLS | 每线程 current context 等状态 |
| memobj | 记录地址、描述、backing 和映射关系的内存对象 |
| UVA/UVM | 统一虚拟地址与托管/迁移支持管理器 |
| stream | 异步工作序列及其 QMD、marker、semaphore 资源 |
| channel | 面向 GPU engine 的提交通道，后端由 DMAL 初始化 |
| QMD | 架构相关 kernel/dispatch 描述结构 |
| HAL | 架构操作函数表及编码实现 |
| DMAL | OS/RM/driver-model 抽象层 |
| CNP | CUDA Dynamic Parallelism 的 device-side child launch 支持 |
| ICD | OpenCL Installable Client Driver loader/dispatch ABI |
| DVS | 驱动验证套件的测试结果格式与聚合流程 |

若没有源码证据，术语不延伸为标准 CUDA 文档中的额外语义。

## 相关文档
- [项目入口](../README.md)
- [分析状态](analysis-state.md)
- [源码证据索引](evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
