# M10 源码地图

| 区域 | 作用 | 证据 |
|---|---|---|
| `nms.py` | load、数据、correctness、benchmark | `[kernels/nms/nms.py:8-135]` |
| `nms_iou_mask_kernel` | warp IoU/ballot row | `[kernels/nms/nms.cu:22-71]` |
| `nms_resolve_kernel` | ordered resolve/shared bitmap | `[kernels/nms/nms.cu:85-109]` |
| `nms` binding | checks/sort/launch/index mapping | `[kernels/nms/nms.cu:126-189]` |
| `hard_nms` | CPU reference | `[kernels/nms/nms.cc:4-40]` |
