# M05 源码地图

| 区域 | 作用 | 证据 |
|---|---|---|
| `sgemm_naive_f32_kernel` | 每线程一个输出 | `[kernels/sgemm/sgemm.cu:20-34]` |
| sliced-K kernel | 共享 A/B tile | `[kernels/sgemm/sgemm.cu:36-86]` |
| thread-tile kernel | 8×8 register tile、vec4 | `[kernels/sgemm/sgemm.cu:88-166]` |
| double-buffer variants | 预取和 buffer parity | `[kernels/sgemm/sgemm.cu:168-482]` |
| Python loader | 动态加载多个实现 | `[kernels/sgemm/sgemm.py:9-29]` |

