# 跨模块接口契约

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