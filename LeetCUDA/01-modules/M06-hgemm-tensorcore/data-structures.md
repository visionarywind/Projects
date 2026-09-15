# M06 数据结构与实例

- 文档目的：解释 01-modules/M06-hgemm-tensorcore/data-structures.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/LeetCUDA HEAD 4513b31（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M06-hgemm-tensorcore/data-structures.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


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
