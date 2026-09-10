# M06 源码地图

| 文件/目录 | 责任 | 深度状态 |
|---|---|---|
| `hgemm.py` | CLI、算法筛选、benchmark、TFLOPS | 已完成入口 |
| `tools/utils.py` | source 集合、flags、wheel/source fallback、layout helper | 已完成代表入口 |
| `setup.py` | CUDAExtension、架构和 include | 已完成构建边界 |
| `makefile` | standalone binary targets | 部分完成 |
| `pybind/hgemm.cc` | Python export boundary 和 variant 命名 | 已完成接口索引 |
| `wmma/` | WMMA implementations | 部分完成 |
| `mma/basic/hgemm_mma_stage.cu` | 代表 staged MMA、dsmem、collective store | 已完成深度代表 |
| `mma/basic/hgemm_mma_stage_tn.cu` | TN layout staged variants | 部分完成 |
| `mma/swizzle/` | padding/swizzle variants | 部分完成 |
| `wgmma/` | Hopper warp-group paths | 部分完成 |
| `cutlass/` | CuTe/CUTLASS paths | 部分完成 |
| `naive/`, `cublas/` | baseline/reference families | 入口已知、逐文件未完成 |

## 代表源码锚点

- Python/构建：`[kernels/hgemm/hgemm.py:18-177]`、`[kernels/hgemm/tools/utils.py:18-32,109-155]`、`[kernels/hgemm/setup.py:42-95]`。
- PyBind：`[kernels/hgemm/pybind/hgemm.cc:7-181]`。
- staged MMA template/shared：`[kernels/hgemm/mma/basic/hgemm_mma_stage.cu:120-142,632-686]`。
- pipeline/MMA/store：`[kernels/hgemm/mma/basic/hgemm_mma_stage.cu:691-839,1850-1959]`。

本文中的“部分完成”表示源码目录或入口已定位，不表示该变体已构建或运行。