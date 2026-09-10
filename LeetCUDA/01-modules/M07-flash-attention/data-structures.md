# M07 数据结构与实例

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
