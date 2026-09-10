# M08 接口

| 边界 | 责任 |
|---|---|
| Python | 参数、tensor 生成、reference、benchmark |
| PyBind/C++ | 导出函数、输入检查、data pointer、launch |
| CUDA kernel | 并行计算和写回，不拥有 PyTorch storage |
| setup/load | 编译 flags、sources、架构和 ABI |

检查程度不一致：elementwise 代表 binding 主要检查 dtype，NMS 额外检查 device/dim/长度。[kernels/elementwise/elementwise.cu:134-146]；[kernels/nms/nms.cu:126-150]
