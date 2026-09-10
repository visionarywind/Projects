# M07 设计

FlashAttention 把 QKᵀ、缩放、online/safe softmax 和 PV 融合在 tile loop 中，核心收益是避免 materialize 完整 attention matrix。split-Q/KV、shared Q/KV/QKV、multi-stage、swizzle 和 fine-grained tiling 是不同并行与片上复用取舍。[kernels/flash-attn/README.md:14-26]

代码是学习实现；README 明确大规模 attention 可能仍落后于官方实现。[kernels/flash-attn/README.md:14-26]
