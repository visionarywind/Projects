# M10 接口

输入：`boxes` float32 `(N,4)`、`scores` float32 `(N,)`、IoU threshold；binding 检查 CUDA device、rank 和长度。空输入返回输入 device 上的 int64 空 tensor。[kernels/nms/nms.cu:126-150]

输出是输入原始位置的 int64 index，顺序遵循 stable score sort 后的保留顺序，不是 sorted position。[kernels/nms/nms.cu:173-189]
