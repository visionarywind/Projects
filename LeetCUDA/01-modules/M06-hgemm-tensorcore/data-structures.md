# M06 数据结构与实例

## 1. 编译期结构

| 结构 | 代表定义 | 作用 |
|---|---|---|
| block tile | `BM=128, BN=128` | 一个 block 的 C 输出范围 |
| K tile | `BK=16` | 一个 MMA K 迭代 |
| MMA atom | `m16n8k16` | 单次 Tensor Core 乘加 |
| stage buffer | `s_a[K_STAGE]`, `s_b[K_STAGE]` | A/B 的 shared-memory 环形阶段 |
| fragment | `RA`, `RB` | `ldmatrix` 载入的 lane-owned 数据 |
| accumulator | `RC[WARP_TILE_M][WARP_TILE_N][2]` | warp 输出累加寄存器 |

证据：[kernels/hgemm/mma/basic/hgemm_mma_stage.cu:120-142,678-686]。

## 2. 地址与所有权

A/B/C 通常来自 PyTorch tensor，kernel 只借用 device pointer；shared/register 数组由 kernel invocation 自动拥有，调用结束即失效。动态 `smem[]` 的切分由模板尺寸和 launcher 字节数共同决定。[kernels/hgemm/mma/basic/hgemm_mma_stage.cu:632-661,1900-1926]

## 3. 最小源码阅读实例

```text
选择 `hgemm_mma_m16n8k16...stages_kernel` 时：
1. 先找 PyBind 的同名/相近导出；
2. 找 launcher 的 BM/BN/BK 和 grid；
3. 找 shared stage 声明；
4. 找 `CP_ASYNC_*` 初始化与主循环；
5. 找 `LDMATRIX_*`/`HMMA16816`；
6. 找 store 和边界条件。
```

这是一条阅读方法，不是运行结果；当前 GPU correctness 未验证。
