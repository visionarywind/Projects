# 性能关键路径

| 路径 | 相关模块 | 性能因素 |
|---|---|---|
| attention/MLP forward | M01/M02 | TE/local kernel、TP、sequence parallel、activation recompute |
| pipeline P2P | M02/M03 | PP stage balance、warmup/cooldown、overlap、tensor shape |
| data loading | M04/M03 | index cache、sampler、CPU→GPU 搬运、TP broadcast |
| gradient reduce | M02/M05 | DP/CP/SP groups、overlap、distributed optimizer |
| checkpoint | M05/M03 | sharding metadata、async save、filesystem bandwidth |
| inference KV cache | M06/M01 | block allocator、dynamic batching、CUDA graph、sampling |

性能结论必须依赖 profiler 或 benchmark；本文仅标记源码可见的热点和可调参数。