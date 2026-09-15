# D01-S04 调试 walkthrough

- 文档目的：解释 80-demos/D01-nms-python/debug-walkthrough.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/LeetCUDA HEAD 4513b31（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 80-demos/D01-nms-python/debug-walkthrough.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## 推荐观察顺序

1. 在 `nms.py` 的 `lib = load(...)` 处确认扩展构建和加载；
2. 在 `nms.cu:nms` 的输入检查处确认 dtype/device/shape；
3. 观察 `order_t` 和 `boxes_sorted`，确认 stable descending 与 contiguous；
4. 在两个 kernel launch 后加入临时 `C10_CUDA_KERNEL_LAUNCH_CHECK()` 或使用 blocking/sanitizer（仅作为本地调试修改，不是当前仓库事实）；
5. 检查 `mask` 的 `mask_words` 和 N²/32 分配；
6. 在 resolve 的每个 i 观察 `suppressed` 与 `keep`；
7. 在 CPU mapping 处打印 `order_t`、排序位置和原始 index；
8. 检查返回 tensor 的 dtype/device。

## 调试命令（未验证）

```bash
CUDA_LAUNCH_BLOCKING=1 python3 nms.py
compute-sanitizer --tool memcheck python3 nms.py
compute-sanitizer --tool racecheck python3 nms.py
```

这些工具可能改变同步和性能；当前没有执行结果。

## 三个最小诊断输入

- 两个完全重叠框：检查只保留高分框；
- 两个不重叠框：检查两者都保留；
- 相同 score 的框：检查 stable sort 和原始 index 映射。

不要在没有记录 threshold、坐标语义和 reference 的情况下解释差异。

## 典型定位结论

- 输入检查报错：先看 `nms.cu:126-150`，不是 CUDA race；
- output 顺序错误：先看 `order_t` 映射 `nms.cu:166-192`；
- 随机 race/未定义结果：看 mask 初始化、resolve barrier 和 launch error；
- CPU 对拍边界差异：先比较 `nms.cc` 的 `+1` 面积与 GPU 连续面积。

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
