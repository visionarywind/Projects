# M01 数据结构与接口

M01 没有稳定的对外 Python API。它共享 device helper、向量类型、warp 常量、shared-memory/TMA 辅助；实际参数和模板由 `base.cuh`、`hgemm.cuh`、`flash_attn.cuh` 等消费者决定。改变公共宏可能改变所有包含者的编译结果。

证据：`[kernels/interview/README.md:6-16]`。
