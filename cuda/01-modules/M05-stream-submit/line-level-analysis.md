# M05 行级分析

- 文档目的：解释 01-modules/M05-stream-submit/line-level-analysis.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M05-stream-submit/line-level-analysis.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


- `[src/api/apistream.c:43-109]`：检查 flags、锁 context、创建 stream、注册 QMD semaphore pool，失败 detach。
- `[src/cui/cuistream.c:1741-1877]`：pool stream、QMD、UVM prefetch semaphore、CPU semaphore、public handle 和 UVM registration。
- `[src/cui/channel.c:43-173]`：DMAL Init、tracking/cross-engine semaphore、manager 插入与 cleanup。
- `[src/cui/cuistream.c:1926-1986]`：marker 完成后 detached stream reclaim。
- `[src/cui/cuilaunch.c:584-724]`：begin push、HAL finalize/control、QMD tracking、end push。
- `[src/cui/gpfifo.c:313-416,437-490]`：entry 编码、tracking completion 和 pushbuffer get reclaim。
- `[src/cui/gpfifo.c:716-796]`：flush unit 封存 entry、推进 gpuPut 和 DMAL advance 边界。
- `[src/cui/pushbuffer.c:22-99,181-235]`：put/get 环形游标、对齐和回收条件。
- `[src/cui/marker.c:655-813,1008-1140]`：异构 marker entry、flush、blocking wait 和完成检查。

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
