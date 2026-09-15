# 跨模块接口契约

- 文档目的：解释 90-cross-module/interface-contracts.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/LeetCUDA HEAD 4513b31（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 90-cross-module/interface-contracts.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## 1. 契约层次

| 层次 | 典型输入/输出 | 事实来源 | 统一性 |
|---|---|---|---|
| Python API | tensor、shape、CLI flags | 各模块 `.py` | 不统一 |
| PyBind | `torch::Tensor`、scalar、stage | `pybind/*.cc/.cu` | 不统一 |
| kernel launcher | raw pointer、M/N/K、grid/block/SMEM | `.cu` launcher | 变体专属 |
| device kernel | lane/warp/block participation | `__global__` body | 依赖模板 |
| benchmark/reference | sync、copy、error、TFLOPS | Python/C++ harness | 不统一 |

## 2. 共同但非强制假设

- CUDA device tensor：NMS binding 明确检查；HGEMM/FlashAttention 需要逐入口确认。
- dtype：NMS 明确 float32；HGEMM/FA 通常围绕 half/F32 accumulator，但所有变体需看 binding。
- contiguous/stride：NMS 对 sorted boxes 显式 contiguous；HGEMM TN/CuTe/FA layout 依变体而异。
- shape：代表 HGEMM 使用 tile 假设；FA tiling-QKV 对 D 和 N 有 dispatch/assert。
- stream：benchmark 有 synchronize/event，但 current stream 契约未全仓库统一。
- error：Interview 有 `check()`；普通 PyBind/kernel launch error 检查不统一。

## 3. 代表契约证据

### NMS

`boxes=(N,4)`、`scores=(N,)`、float32、CUDA、长度一致；empty 返回 int64 CUDA empty。[kernels/nms/nms.cu:126-150]

### HGEMM staged MMA

模板/launcher 暗含 128×128 block tile、BK=16、256 threads、stage/padding/SMEM 配置，输入 layout 和尾 tile 需逐变体确认。[kernels/hgemm/mma/basic/hgemm_mma_stage.cu:120-142,1900-1959]

### FlashAttention tiling-QKV

输入逻辑布局 `[B,H,N,D]`，D dispatch 为 32/64/96/128/256/512/1024，N 要满足整 tile assert，stage 参数折叠到 1/2。[kernels/flash-attn/mma/basic/flash_attn_mma_tiling_qkv.cu:125-132,799-946]

### Interview

显式 host/device allocation、launch/sync/D2H/error/free；`check()` 错误直接退出。[kernels/interview/notes-v2.cu:92-97,510-542]

## 4. 修改影响规则

改变 layout、dtype、stage、tile 或 output store 时，必须同步检查：Python/reference、PyBind signature、launcher SMEM/grid、kernel lane participation、correctness threshold、benchmark timing 和对应 Demo。任何一项未核对都应标为“部分完成/未知”，不能推断为兼容。

## 5. 未验证事项

全仓库没有一个源码级统一的 rank/device/stride/stream/launch-error contract；没有 GPU CI 或本机运行结果。

## 相关文档
- [项目入口](../README.md)
- [分析状态](../00-overview/analysis-state.md)
- [源码证据索引](../00-overview/evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
