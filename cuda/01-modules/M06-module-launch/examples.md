# M06 代表性例子

- 文档目的：解释 01-modules/M06-module-launch/examples.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M06-module-launch/examples.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


### 普通 launch

`CUIlaunchData` 由 function 参数、grid/block/shared memory 填充，setup 分配当前 stream QMD/constant banks，之后通过 channel push。

### Capture

同一 `cuLaunchKernel` 在 capturing stream 上只创建 graph kernel node；graph validation 失败会使 capture invalidated 或回滚 node，而不是直接执行 GPU kernel。

### Graph exec 资源示例（静态推演）

包含两个同 context kernel node 的 graph 在 instantiate 时按两节点分配 QMD、parameter constant-bank node、HAL staging 和 completion marker；可复用同一 internal stream 的依赖链会继承 parent stream。若 conditional/device scheduler 被启用，还会创建 host graph/index backing 和一个 driver-owned device `CUmemobj`。重复 launch 复用这些已实例化资源，但每次仍重建异步 memory-tracking list；destroy 最后统一 detach stream 并释放 QMD/constant-bank/marker/backing（证据：[src/cui/cuigraph.c:1835-1933,2054-2141,3495-3575,1035-1205]）。

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
