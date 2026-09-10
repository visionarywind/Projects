# M10 风险与债务

- GPU 连续坐标和 CPU `+1` 像素面积语义不同。
- mask 为 O(N²/32)，resolve 有顺序控制依赖和单 block 限制。
- stable sort/ties、keep/mask 初始化、同步和 index remapping 都是高风险不变量。
- threshold 异常值、超大 N、非 contiguous 输入和 stream 行为仍需实测。

证据：`[kernels/nms/nms.cu:36-57,85-109]`、`[kernels/nms/nms.cc:4-40]`。
