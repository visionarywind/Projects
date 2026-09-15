# 测试配方

- 文档目的：解释 99-roadmap/testing-recipes.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：`main` / `4513b31`。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-10
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 99-roadmap/testing-recipes.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


- 状态：GPU 命令未在本环境执行。

## 正确性矩阵

| 层 | 最小测试 | 扩展测试 |
|---|---|---|
| elementwise | 0/1/非 pack 长度、多个 dtype | 二维 K 分支、非 contiguous |
| reduction/norm | 短行、极值、NaN/Inf | 大 row、多个 axis/dtype |
| GEMM | M/N/K=1、非 tile 倍数 | 多架构、stages/layout |
| attention | B/H/N/D 小组合 | D、架构宏、reference variants |
| NMS | 空、6-box regression、ties | N×seed×threshold、超大 N |
| interview | 每个 phase smoke | all arch + benchmark |

## 参考实现

优先使用 torch、torchvision、cuBLAS 或 CPU reference；记录语义差异。NMS CPU `+1` 面积与 GPU 连续坐标不同，不能未经调整直接判定失败。[kernels/nms/nms.cc:4-40]；[kernels/nms/nms.cu:36-57]

## 性能测试

warmup 后同步，重复固定次数，记录均值/分位数和 shape；不要把首次编译时间混入 kernel 时间。HGEMM benchmark 已显式使用同步。[kernels/hgemm/hgemm.py:255-282]

## 安全性/工具

推荐 `compute-sanitizer --tool memcheck`、`racecheck` 和 Nsight；这些在当前环境均未验证。

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
