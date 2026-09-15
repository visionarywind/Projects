# D01-S03 数据和状态追踪

- 文档目的：解释 80-demos/D01-nms-python/data-and-state-trace.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/LeetCUDA HEAD 4513b31（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 80-demos/D01-nms-python/data-and-state-trace.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## 状态表

| 时点 | 对象 | 形状/类型 | 所在位置 | 变化 |
|---|---|---|---|---|
| S0 | `boxes`, `scores` | `(N,4)`, `(N,)`, float32 | CUDA | 调用者拥有 |
| S1 | `order_t` | `(N,)`, int64 | CUDA | stable descending 原始位置 |
| S2 | `boxes_sorted` | `(N,4)`, float32 contiguous | CUDA | 物理重排 |
| S3 | `mask` | `N*ceil(N/32)`, uint32 | CUDA | 全零后 Phase 1 写 rows |
| S4 | `keep` | `(N,)`, int32 | CUDA | 全零后 resolve 写 1 |
| S5 | `keep_cpu`, `order_cpu` | CPU copies | Host | 观察/映射边界 |
| S6 | `keep_indices` | vector<int64> | Host | 只保留排序位置对应原始 index |
| S7 | return | `(K,)`, int64 | CUDA | 输出原始输入索引 |

证据：[kernels/nms/nms.cu:152-192]。

## Phase 1 mask 语义

`mask[i * mask_words + w]` 表示排序后第 i 个 box 对后续候选 word 的 suppress verdict。因为每个 warp 只写自己的 row，当前实现避免了多个 warp 写同一 row；`j>i` 保持 NMS 的排序方向。[kernels/nms/nms.cu:22-71]。

## Phase 2 状态机

```text
suppressed = 0
for i in sorted order:
  if bit(suppressed,i)==0:
    keep[i]=1
    suppressed |= mask[i,:]
  barrier
```

shared `suppressed` 是 block 共享状态；barrier 不是装饰，而是下一轮读取全 block OR 完成结果的必要条件。[kernels/nms/nms.cu:85-109]。

## 语义边界

- GPU IoU 使用连续坐标面积；
- CPU `nms.cc` 参考使用 `+1` 面积；
- fixed/random Python 对拍使用 torchvision，参考路径未必等同于 `nms.cc` 的面积实现；
- ties 由 stable sort 决定相同分数的顺序，输出应恢复原始位置但运行输出未验证。

## 生命周期

PyTorch allocator 持有输入、临时 tensor 和结果；extension module/cache 由 Python 进程管理。CPU vector 在 binding 返回前存在，之后释放；CUDA `mask/keep/boxes_sorted` 的 tensor 引用离开函数后按 PyTorch 生命周期回收。后半句是 C++ 局部 tensor + PyTorch intrusive ownership 的静态推断，未运行检查 allocator。

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
