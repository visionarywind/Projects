# M04 开发指南

- 文档目的：解释 01-modules/M04-memory-uvm/development-guide.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M04-memory-uvm/development-guide.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


1. 新 memory API 先定义 `CUmemdesc` 的 owner/API source/location/mapping 组合。
2. 同步修改 memobj registry、UVA lookup、tools notify、context destroy 和 IPC/P2P 路径。
3. 任何异步 launch 引用必须加入 M06 tracking；free 不能只依赖 host 引用计数。
4. 分析 UVM fault/migration 时区分当前树可见的 host-side DAG 与外部 UVM/RM 行为。
5. 修改 suballocator 时同时检查 `canSuballocate`、descriptor compatibility、HAL block size/alignment、split/coalesce 和最后一个 memobj 的 block 释放；不要只修改 memobj pointer lookup。
6. 使用 memmgr/memobj/memcpy 测试做静态覆盖映射；本工作区未执行。新增池化行为应补充固定 seed/可重复的 best-fit、碎片和 DMAL 失败测试。

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
