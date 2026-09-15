# 性能关键路径

- 对应源码版本：`main` / `4513b31`。
- 证据状态：性能机制来自源码和 README；数值均不视为本机实测。
- 最后更新：2026-09-10

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
