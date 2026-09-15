# D01-S05 失败路径、错误边界与清理

- 文档目的：解释 80-demos/D01-nms-python/failure-paths.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/LeetCUDA HEAD 4513b31（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 80-demos/D01-nms-python/failure-paths.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


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

## 相关文档
- [项目入口](../../README.md)
- [分析状态](../../00-overview/analysis-state.md)
- [源码证据索引](../../00-overview/evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](../../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
