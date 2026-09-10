# D01-S02 执行追踪

## 控制流

```text
nms.py import
  → torch cpp_extension.load
  → lib.nms(boxes, scores, threshold)
  → dtype/device/shape/empty checks
  → scores.sort(stable, descending)
  → boxes.index_select(...).contiguous()
  → allocate zero keep + zero mask
  → nms_iou_mask_kernel<<<grid,256>>>
  → nms_resolve_kernel<<<1,256,smem>>>
  → keep/order copy to CPU
  → construct int64 CUDA indices
```

入口/加载证据：[kernels/nms/nms.py:8-23]；binding 和 launches：[kernels/nms/nms.cu:126-192]。

## D01-S03：输入检查和排序

1. boxes/scores 必须 float32；
2. 两者必须是 CUDA tensor；
3. boxes 为 `(N,4)`，scores 为 `(N,)` 且长度一致；
4. N=0 立即返回 device 上的 int64 empty；
5. `scores.sort(stable=true, descending=true)` 返回排序分数和原始 `order_t`；
6. `boxes_sorted` 通过 `index_select(...).contiguous()` 供 CUDA kernel 读取。[kernels/nms/nms.cu:126-164]

## D01-S04：Phase 1

grid 线程按 warp 映射 box：`warp_id=(blockIdx.x*blockDim.x+threadIdx.x)/32`。lane 计算 `j` 候选框，比较连续坐标 IoU，仅对 `j>i` 的后续排序框设置 suppress；lane 0 写 ballot 得到的 word。[kernels/nms/nms.cu:22-71]

## D01-S05：Phase 2

唯一 block 先把 shared `suppressed[]` 清零并同步。随后从 i=0 顺序扫描：未被抑制时 thread 0 写 `keep[i]=1`，全 block 分片 OR 当前 row；每轮同步后进入下一 i。[kernels/nms/nms.cu:85-109]。

## D01-S06：输出映射

CPU 读取 `keep` 和 `order_t`，只把排序位置 i 对应的原始 `order_data[i]` 推入 `keep_indices`，最后构造输入 device 上的 int64 tensor。[kernels/nms/nms.cu:166-192]。该 CPU extraction 是当前源码实际控制流，不要把它描述成完全 device-side resolve。

## 执行上下文

两个 kernel launch 默认异步；紧随其后的 `keep.to(torch::kCPU)` 是可观察的 device→host 边界，但当前 binding 未展示统一 `cudaGetLastError`。是否能稳定报告第一阶段 launch error，需要用 blocking/sanitizer 单独验证。

## 预期观察点（未验证）

- 动态扩展首次 load 的编译/cache 日志；
- fixed/random correctness 的 pass/fail 文本；
- 输出 index 的 device、dtype 和顺序；
- N 增大时 mask 分配和 resolve 时间；
- ties 下 stable sort 对原始 index 的影响。
