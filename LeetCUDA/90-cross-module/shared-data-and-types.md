# 共享数据、类型与布局

- 文档目的：解释 90-cross-module/shared-data-and-types.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：`main` / `4513b31`。
- 证据状态：代表模块契约已确认；仓库没有全局 schema。
- 最后更新：2026-09-10
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 90-cross-module/shared-data-and-types.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。

## 共享边界

| 数据/类型 | 典型生产者 | 典型消费者 | 关键约束 |
|---|---|---|---|
| `torch::Tensor` | Python | binding | dtype/device/shape 检查由模块自行决定 |
| raw `data_ptr` | binding | CUDA kernel | tensor 必须在异步工作完成前保持有效 |
| FP16/BF16/FP32 | Python + binding | vector/Tensor Core kernel | accumulator 和误差阈值随实现变化 |
| `[M,K]×[K,N]` | HGEMM/SGEMM Python | CUDA/cuBLAS | layout/stride 必须与变体匹配 |
| `[B,H,N,D]` Q/K/V | FlashAttention Python | attention binding/kernel | 某些变体需要转置或 swizzle view |
| NMS `mask` uint32 bitset | Phase 1 | Phase 2 | `mask_words=ceil(N/32)`，按排序位置编码 |
| NMS `order` int64 | `scores.sort` | CPU mapping | 输出必须恢复输入原始 index |

## 所有权和生命周期

PyTorch tensor 的存储由 PyTorch 管理；binding 通常只在当前调用中借用 data pointer。Interview 测试还会显式 `malloc`/`cudaMalloc`，因此由测试函数负责 `free`/`cudaFree`。HGEMM cuBLAS handle 是显式 init/destroy 资源。

证据：`kernels/interview/notes-v2.cu:100-499`；`kernels/hgemm/hgemm.py:242-244,319-322`。

## 布局风险

- elementwise 的 pack load/store 假定对应元素访问合法，尾部需 fallback。
- SGEMM/HGEMM 的 NN padding 与 TN swizzle 不是可互换布局。
- FlashAttention Python 构造多种 view；变体函数名不足以证明 layout。
- NMS 的 bitset index 是排序后位置，输出 index 是原始位置，二者不可混用。

## 相关文档
- [项目入口](../README.md)
- [分析状态](../00-overview/analysis-state.md)
- [源码证据索引](../00-overview/evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题

没有统一 `TensorContract`、stride policy、dtype trait 或错误码；新模块必须在自身文档中声明这些契约。

## 下一步阅读建议
先阅读 [分析状态](../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
