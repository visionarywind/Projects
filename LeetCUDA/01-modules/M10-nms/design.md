# M10 设计

NMS 先 stable descending sort，Phase 1 以 warp-per-box 计算后续候选 IoU bitmask，Phase 2 以单 block 按排序顺序依次决定保留并 OR suppression rows。单 block + `__syncthreads()` 将顺序决策和并行 bitset 更新分离；最后恢复输入原始 index。

证据：`[kernels/nms/nms.cu:22-109,126-189]`。
