# 开发配方

- 文档目的：解释 99-roadmap/feature-development-recipes.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 99-roadmap/feature-development-recipes.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


- 新 API：同步修改 `inc/cuda.h`、`src/api`、generated API 输入、`.def` 和 `cuda.nvmk`。
- 新资源：先定义 owner/lock/异步完成条件，再补创建失败和 destroy 回滚。
- 改 launch：同时核对参数 metadata、memory tracking、QMD/HAL、tools callback 和 stream end push。
- 改显存池化：同步 `canSuballocate`、descriptor compatibility、HAL block size/alignment、radix best-fit/split/coalesce、memblock DMAL free 和碎片测试；不要把 `CUheap` VA 索引改成物理池语义。
- 改 Graph 资源：同步 capture origin/invalidation、per-context `CUIgraphCtxData`、QMD/constant-bank/internal stream/marker、scheduler backing、launch tracking 和 destroy reverse path；为 instantiate/launch 失败补注入测试。
- 改架构：检查 `NVCFG(GLOBAL_ARCH_*)`、`cuda.nvmk` 源清单与具体 HAL 实现。
- 改测试：先在 `cuda_test.nvmk` 定义 binary，再注册 case，覆盖成功、错误和清理。

## 相关文档
- [项目入口](../README.md)
- [分析状态](../00-overview/analysis-state.md)
- [源码证据索引](../00-overview/evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
