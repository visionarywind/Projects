# M07 源码地图

| 区域 | 作用 | 证据 | 深度状态 |
|---|---|---|---|
| `flash_attn_mma.py` 参数 | shape/reference/benchmark CLI | `[kernels/flash-attn/flash_attn_mma.py:22-55]` | 已完成入口 |
| sources builder | 收集 basic/swizzle/CuTe/pybind 源文件 | `[kernels/flash-attn/flash_attn_mma.py:80-138]` | 已完成 |
| CUDA flags | GPU-name/架构/debug 宏 | `[kernels/flash-attn/flash_attn_mma.py:150-203]` | 已完成 |
| `load` | 动态扩展入口 | `[kernels/flash-attn/flash_attn_mma.py:220-227]` | 已完成 |
| PyBind | variant ABI boundary | `[kernels/flash-attn/pybind/flash_attn.cc:7-223]` | 已完成索引 |
| tiling-QKV template | Br/Bc、stage、padding、F16/F32 contract | `[kernels/flash-attn/mma/basic/flash_attn_mma_tiling_qkv.cu:50-124]` | 已完成代表 |
| Q/K/V SMEM + registers | alias、m/l、S/P/O 状态 | `[kernels/flash-attn/mma/basic/flash_attn_mma_tiling_qkv.cu:156-200]` | 已完成代表 |
| QK/softmax/PV/store | 完整融合计算路径 | `[kernels/flash-attn/mma/basic/flash_attn_mma_tiling_qkv.cu:201-797]` | 已完成代表 |
| launcher/dispatch | grid、D、stage、整 tile assert | `[kernels/flash-attn/mma/basic/flash_attn_mma_tiling_qkv.cu:799-946]` | 已完成代表 |
| split/share/swizzle/CuTe | 其他 kernel families | `flash_attn_mma.py` source list + PyBind | 部分完成 |

`setup.py` 和 `tools/utils.py` 当前为空；主要构建入口是 Python 动态 `load`，不是有效 setup.py 路径。