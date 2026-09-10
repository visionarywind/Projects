# D01-S05 失败路径、错误边界与清理

## 输入失败

| 输入 | 预期源码行为 | 状态 |
|---|---|---|
| CPU boxes/scores | device check 抛错 | 已确认 |
| 非 float32 | dtype check 抛错 | 已确认 |
| boxes 非 `(N,4)` | shape check 抛错 | 已确认 |
| scores 非 `(N,)`/长度不同 | shape/length check 抛错 | 已确认 |
| N=0 | 返回 device 上 int64 empty | 已确认 |
| `N` 很大 | mask/SMEM/单 block 成本可能不可行 | 风险，未验证 |
| NaN/Inf threshold/coordinates | 显式 finite 检查未见 | 未知 |

证据：[kernels/nms/nms.cu:126-150]。

## 异步和 kernel 失败

Phase 1 和 Phase 2 launch 之间没有在已读 binding 中看到统一 `cudaGetLastError`。后续 CPU copy 可能观察到异步错误，但错误归因和时点不应假定。建议用 `CUDA_LAUNCH_BLOCKING=1`、`compute-sanitizer` 或临时 launch check 复现；这些命令未验证。

## 语义失败

GPU 使用连续 coordinate area，CPU `nms.cc` 使用 `+1` area；若只在极小框和阈值边界失败，先审计语义而不是立即判定 mask race。[kernels/nms/nms.cu:33-63]、[kernels/nms/nms.cc:4-40]。

## 清理路径

- Python tensors：PyTorch 管理；
- extension cache：PyTorch extension loader/进程环境管理；
- C++ 局部 `mask/keep/boxes_sorted`：离开 binding 后由 tensor 引用管理；
- host vector/accessor：函数结束时释放；
- 没有本 Demo 手工 `cudaFree` 路径。

## 不应声称

当前不能声称：随机 sweep 已通过、最大 N 安全、所有 stream 可用、launch error 已统一报告或 sanitizer 无问题；这些都需要实际环境验证。
