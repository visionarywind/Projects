# M09 源码地图

| 文件 | 作用 |
|---|---|
| `build.sh` | nvcc、架构、库、clean、链接 |
| `common.cuh` | 公共 CUDA/PTX/TMA helpers |
| `base.cuh` | 基础原语 |
| `sgemv.cuh`/`sgemm.cuh` | CUDA Core GEMV/GEMM |
| `hgemm.cuh` | Tensor Core HGEMM |
| `flash_attn.cuh`/`ffpa_attn.cuh` | attention 变体 |
| `notes-v2.cu` | tests/bench/CLI |

证据：`[kernels/interview/README.md:6-16]`。
