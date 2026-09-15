# M10 NMS 专题

- 文档目的：解释当前 GPU NMS 的算法、输入契约、修复背景和测试。
- 适用范围：`kernels/nms/nms.cu`, `nms.cc`, `nms.py`。
- 对应源码版本：`4513b31`。
- 证据状态：核心实现已逐行分析，benchmark 未在本机执行。
- 最后更新：2026-09-10
- 前置阅读：[M08](../M08-pytorch-extension/README.md)
- 后续阅读：[跨模块错误边界](../../90-cross-module/error-boundaries.md)

## 结论摘要

当前 NMS 先按分数 stable descending sort；Phase 1 每个 box 一个 warp，用 32 lanes 计算候选 IoU 并 ballot 写自己的 bitmask 行；Phase 2 一个 block 按排序顺序串行决定保留项，所有线程并行 OR 保留项的 suppression row 到 shared bitmap，并用 `__syncthreads` 保证决策可见；最后把 sorted positions 映射回原始 int64 indices。

## 行级调用链

```text
nms.py:lib.nms
 -> nms() 输入 dtype/device/dim 检查
 -> scores.sort(stable=true, descending=true)
 -> nms_iou_mask_kernel (one warp/box)
 -> nms_resolve_kernel (one block/shared bitmap)
 -> keep/order CPU copy + original-index mapping
 -> int64 CUDA tensor
```

证据：`nms.cu:126-189`；Phase 1 `22-71`；Phase 2 `85-109`。

## 数据结构与语义

`mask[num_boxes * mask_words]` 是 uint32 bitset；`keep` 是 int32 initialized zero；输出是输入 device 上 int64 index。GPU 使用连续坐标 `(x2-x1)*(y2-y1)`，CPU `nms.cc` 使用 `+1` 像素面积，不能混作同一参考语义。

## 测试

Python 固定 6 框 issue repro 重复 5 次，再对 N×seed×threshold 与 torchvision 逐元素比较，最后对 1024/2048/4096/8192 benchmark。[kernels/nms/nms.py:87-135]。修改同步/索引时必须保留 ties in scores 的随机 sweep。

## 风险

NMS Phase 2 的时间复杂度包含按 box/word 的串行 resolve；mask 内存为 O(N²/32)，大 N 可能受限。IoU、stable sort、未初始化 buffer、cross-block visibility 和输出 index dtype 是正确性高风险。

## 相关文档

- [design.md](design.md)
- [source-map.md](source-map.md)
- [interfaces.md](interfaces.md)
- [call-chains.md](call-chains.md)
- [testing.md](testing.md)
- [risks-and-debt.md](risks-and-debt.md)

## 源码证据摘要

- `[kernels/nms/nms.cu:9-20]`：算法设计注释。
- `[kernels/nms/nms.cu:142-150]`：zero initialization。
- `[kernels/nms/nms.cu:173-189]`：index mapping。
- `[kernels/nms/README.md:10-16]`：已知语义差异。

## 未解决问题

当前实现对超大 N、异常 IoU threshold 和异步 stream 的边界需要实测。

## 深度审计

| 分析对象 | 入口落地 | 正常路径 | 分支 | 异常 | 清理 | 数据生命周期 | 执行上下文 | 行级证据 | Demo 映射 | 状态/缺口 |
|---|---|---|---|---|---|---|---|---|---|---|
| `01-modules/M10-nms/README.md` | 已完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 已映射或不适用 | 部分完成：动态行为、边界或专用变体仍需验证 |

## 下一步阅读建议
先阅读 [分析状态](../../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
