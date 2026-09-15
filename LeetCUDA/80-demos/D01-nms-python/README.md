# D01 NMS Python：动态扩展与两阶段 GPU NMS

## Demo 目标

用一个最小但真实的 Python 入口追踪：动态 CUDA extension 加载 → 输入检查 → stable score sort → warp-per-box IoU mask → single-block ordered resolve → 原始索引恢复 → int64 CUDA 输出。该 Demo 对应最近的 NMS 修复提交，适合先理解 M08 扩展边界和 M10 算法路径。

- 目标版本：`main` / `4513b31`。
- 当前状态：源码路径和控制流已确认；构建、运行、输出和性能均**未验证**。
- 相关模块：[M08](../../01-modules/M08-pytorch-extension/README.md)、[M10](../../01-modules/M10-nms/README.md)。

## 文档步骤

| ID | 内容 |
|---|---|
| D01-S01 | [构建与运行](build-and-run.md)：Python load 和命令 |
| D01-S02 | [执行追踪](execution-trace.md)：控制流到两个 kernel |
| D01-S03 | [数据/状态追踪](data-and-state-trace.md)：tensor、mask、keep、index 生命周期 |
| D01-S04 | [调试 walkthrough](debug-walkthrough.md)：断点和检查点 |
| D01-S05 | [失败路径](failure-paths.md)：契约、错误和清理 |
| D01-S06 | [修改练习](modification-exercises.md)：可控实验 |

## 输入输出契约

| 项目 | 源码事实 | 状态 |
|---|---|---|
| `boxes` | 期望二维 `(N,4)`、float32、CUDA | dtype/device/dim/shape 检查见 `[kernels/nms/nms.cu:126-150]` | 已确认 |
| `scores` | 期望一维 `(N,)`、float32、CUDA | 同上 | 已确认 |
| 空输入 | 返回输入 device 上的 int64 空 tensor | `[kernels/nms/nms.cu:138-143]` | 已确认 |
| 输出 | 原始输入索引，int64，CUDA | `[kernels/nms/nms.cu:176-192]` | 已确认 |
| layout | 排序后的 boxes 显式 `.contiguous()`；输入 contiguous/两 tensor 同 device 的完整契约需实测 | `[kernels/nms/nms.cu:159-164]` | 部分完成 |
| threshold | 传给 GPU IoU 比较；finite/range 检查未见 | `[kernels/nms/nms.cu:165-171]` | 未知 |

## 固定六框案例

Python 脚本包含固定 6-box 回归入口和 torchvision 对拍路径。[kernels/nms/nms.py:39-48,87-103]。分数示例可使用 `[0.9, 0.8, 0.7, 0.6, 0.5, 0.4]`、threshold `0.5`；具体保留索引必须以实际运行输出为准，本文不伪造结果。

## 算法核心

### Phase 1：IoU mask

一个 warp 负责一个按分数排序后的 box，32 个 lane 计算后续候选框，`__ballot_sync` 压成一个 `uint32` row。[kernels/nms/nms.cu:22-71]。GPU 面积采用连续坐标 `(x2-x1)*(y2-y1)`。

### Phase 2：ordered resolve

单 block 先清 shared `suppressed`，按排序顺序逐框判断；保留框由全 block 协作 OR mask row，并通过 `__syncthreads()` 让下一轮看见完整状态。[kernels/nms/nms.cu:85-109]。

### 输出映射

`order_t` 是 stable descending sort 得到的原始位置；GPU resolve 产生排序空间中的 keep，随后 CPU 读取 keep/order，再构造原始 index 的 int64 CUDA tensor。[kernels/nms/nms.cu:152-192]。

## 深度审计表

| 审计项 | 结论 | 状态 |
|---|---|---|
| 入口落地 | `nms.py` → `load()` → `nms.cu:nms` → 两个实际 `__global__` kernel | 已完成 |
| 正常路径 | 检查、排序、mask、resolve、CPU 映射、返回 | 已完成 |
| 关键分支 | empty、stable ties、tail mask word、threshold | 部分完成 |
| 异常路径 | dtype/device/shape 有检查；launch error、finite threshold 未统一确认 | 部分完成 |
| 清理 | PyTorch tensor/extension cache 管理；无手工 cudaFree | 已确认总体模型 |
| 数据生命周期 | boxes/scores→sorted boxes→mask/keep→CPU map→output | 已完成 |
| 执行上下文 | CUDA kernel 异步；后续 CPU copy 是观察/同步边界；current stream 未显式记录 | 部分完成 |
| 行级证据 | Python、Phase 1/2、binding 全有 | 已完成 |
| Demo 映射 | D01-S01…S06 已映射 M08/M10 | 已完成 |
| 总体 | 源码级 demo 完成，动态结果未验证 | 部分完成 |

## 已知偏差和风险

- CPU 参考 `kernels/nms/nms.cc` 使用 `+1` 像素面积，GPU 使用连续坐标；两者在小框/阈值边界可能不同。[kernels/nms/nms.cc:4-40]、[kernels/nms/nms.cu:33-63]
- mask storage 为约 `O(N²/32)`；resolve 为单 block 顺序依赖，超大 N 的容量和性能未验证。
- Python 现有 sweep 是 N=10/100/1024/4096/8192、seed 0/1/2、threshold 0.5/0.7；不应把 N=0、NaN、非 contiguous 当作已有测试覆盖。[kernels/nms/nms.py:87-121]
- 当前没有记录本机 GPU 输出、benchmark 或 sanitizer 结果。

## 反向链接

- 模块： [M08](../../01-modules/M08-pytorch-extension/README.md) · [M10](../../01-modules/M10-nms/README.md)
- 总览证据：[证据索引](../../00-overview/evidence-index.md)
- 跨模块：[端到端流程](../../90-cross-module/end-to-end-flows.md)
- 注册表：[demo-registry.md](../demo-registry.md)
