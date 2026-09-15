# 性能关键路径

- 文档目的：解释 90-cross-module/performance-critical-paths.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：`main` / `4513b31`。
- 证据状态：性能机制来自源码和 README；数值均不视为本机实测。
- 最后更新：2026-09-10
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 90-cross-module/performance-critical-paths.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。

## 路径一：内存带宽型逐元素

`global load → arithmetic → global store` 是主路径；`float4`、`half2` 和 pack 版本通过一次线程处理多个元素降低指令开销，但尾部和对齐会影响收益。[kernels/elementwise/elementwise.cu:22-127]

## 路径二：CUDA Core GEMM

`global → shared tile → register tile → C store`；性能受 coalescing、shared bank conflict、寄存器占用、occupancy 和 K-loop 同步影响。SGEMM 的 thread tile、vector load 和 double buffer 是逐级优化点。[kernels/sgemm/sgemm.cu:88-482]

## 路径三：Tensor Core HGEMM

`layout transform/prefetch → WMMA/MMA/WGMMA → accumulator → store`；NN padding 与 TN swizzle 解决不同 shared-memory 访问模式，stages 和 double buffer 试图重叠访存计算。[kernels/hgemm/README.md:195-204,305-307]

## 路径四：FlashAttention

`Q/K tile → QKᵀ → scale/online softmax → PV → O` 在片上复用，避免完整 attention matrix；split-Q/KV、multi-stage、swizzle 和 fine-grained tile 改变并行粒度与同步成本。[kernels/flash-attn/README.md:14-26]

## 路径五：NMS

Phase 1 并行计算 mask，Phase 2 按排序顺序 resolve；后者存在按 box/word 的串行控制依赖和 O(N²/32) mask 存储。[kernels/nms/nms.cu:22-109]

## 路径六：HGEMM staged MMA 代表实现

`global A/B → cp.async stage → wait/sync → ldmatrix → mma.sync.m16n8k16 → RC → four-lane shuffle/128-bit store`。stage/padding/swizzle 的收益和 occupancy 代价必须用 profiler 验证；dynamic SMEM launcher 与容量见 `[kernels/hgemm/mma/basic/hgemm_mma_stage.cu:691-839,1900-1959]`。

## 路径七：FlashAttention tiling-QKV

`Q/K shared tile → QKᵀ HMMA → row max/exp/sum → P → V alias/load → P@V HMMA → online O rescale → 1/l → O store`。Q/V shared alias、四 lane reduction 和非整 tile early-return 是同步/边界重点。[kernels/flash-attn/mma/basic/flash_attn_mma_tiling_qkv.cu:151-200,401-797,845-849]

## Demo 可观测路径

- D01：mask allocation、two-phase kernel、CPU mapping 和 NMS output；
- D02：compile/link、CUDA allocation/H2D/kernel/event/D2H/free。

两者当前都未在 GPU 上运行；不可用文档结构替代性能结果。


所有比较应固定 GPU、driver、CUDA/PyTorch、shape、dtype、warmup、iters、同步点和输出复用策略。只报告源码或 README 中存在的性能数字，并注明历史/未验证状态。

## 推荐工具

先用 correctness，再用 `compute-sanitizer` 检查越界/竞态，最后使用 Nsight Systems/Compute 分析 kernel 时间、内存吞吐、occupancy、tensor utilization 和 shared bank conflicts；当前命令尚未在本环境执行。

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
