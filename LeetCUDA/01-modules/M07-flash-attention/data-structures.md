# M07 数据结构与实例

- 文档目的：解释 01-modules/M07-flash-attention/data-structures.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/LeetCUDA HEAD 4513b31（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M07-flash-attention/data-structures.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## 1. 输入输出

代表 kernel 使用 `[B,H,N,D]` 的 Q/K/V/O 线性地址计算；Q tile 由 grid.x 选择，batch/head 由 grid.y 选择。[kernels/flash-attn/mma/basic/flash_attn_mma_tiling_qkv.cu:94-132]

## 2. 片上状态

| 状态 | 表示 | 位置 |
|---|---|---|
| Q/K/V tile | `Q_tile_smem/K_tile_smem/V_tile_smem` | shared memory |
| logits/probability | `R_S`，就地 S→P | registers |
| softmax state | row `m/l` old/new | registers |
| output | `R_O/R_D` | registers，最后写 O |

证据：[kernels/flash-attn/mma/basic/flash_attn_mma_tiling_qkv.cu:156-200]。

## 3. 生命周期要点

V 复用 Q 的 shared-memory 区域；因此 Q 的最后一次使用、同步、V 写入构成一个必须保持的顺序约束。Q/K/V/O tensor storage 由 Python/PyTorch 调用边界持有，kernel 不负责释放。

## 4. layout 注意

PyTorch 脚本同时准备多种 reference/layout，不能仅凭 `[B,H,N,D]` 外形判断每个 swizzle/CuTe 变体的 stride。[kernels/flash-attn/flash_attn_mma.py:416-443]。所有非 contiguous、transpose 和 head-dim 组合均需单独核对。

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
