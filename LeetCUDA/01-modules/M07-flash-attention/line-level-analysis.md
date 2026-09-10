# M07 行级分析与审计

## 1. 符号/区域索引

| 源码锚点 | 区域 | 关键事实 | 状态 |
|---|---|---|---|
| `kernels/flash-attn/flash_attn_mma.py:22-55` | CLI | B/H/N/D、check、reference、warmup/iters | 已确认 |
| `kernels/flash-attn/flash_attn_mma.py:80-138` | source builder | basic/swizzle/CuTe/PyBind 源集合 | 已确认 |
| `kernels/flash-attn/flash_attn_mma.py:150-203` | CUDA flags | device/架构宏 | 已确认 |
| `kernels/flash-attn/flash_attn_mma.py:220-227` | dynamic load | extension 入口 | 已确认 |
| `kernels/flash-attn/pybind/flash_attn.cc:7-223` | exports | variant ABI boundary | 已确认 |
| `.../flash_attn_mma_tiling_qkv.cu:50-99` | template constraints | MMA/tile/stage/padding contract | 已确认 |
| `.../flash_attn_mma_tiling_qkv.cu:94-132` | tile/grid/offset | Br/Bc/threads and `[B,H,N,D]` offsets | 已确认 |
| `.../flash_attn_mma_tiling_qkv.cu:134-170` | load/SMEM | Q/K mapping and V alias | 已确认 |
| `.../flash_attn_mma_tiling_qkv.cu:172-200` | registers | m/l, Q/K/V/S/O/D state | 已确认 |
| `.../flash_attn_mma_tiling_qkv.cu:201-391` | staged QK loop | cp async, ldmatrix, HMMA | 已确认 |
| `.../flash_attn_mma_tiling_qkv.cu:401-488` | softmax | row max/exp/sum, S→P | 已确认 |
| `.../flash_attn_mma_tiling_qkv.cu:490-658` | V/PV | V load + P@V | 已确认 |
| `.../flash_attn_mma_tiling_qkv.cu:665-797` | output | rescale/normalize/shuffle store | 已确认 |
| `.../flash_attn_mma_tiling_qkv.cu:799-879` | launcher | D dispatch, grid/block, assert | 已确认 |
| `.../flash_attn_mma_tiling_qkv.cu:881-946` | dispatch | supported D and stage mapping | 已确认 |

## 2. 深度审计表

| 审计项 | 当前结论 | 状态 |
|---|---|---|
| 入口是否落地到具体实现 | Python → load → PyBind → tiling-QKV → HMMA/store 已追到 | 已完成 |
| 正常路径 | Q/K tile → QKᵀ → online softmax → P@V → O normalize/store | 已完成 |
| 关键分支 | D、stage、F16/F32 output、shared Q/K/V、split/swizzle/CuTe | 部分完成 |
| 异常路径 | D dispatch、整 tile assert、early return、SMEM 风险已记录 | 部分完成 |
| 清理路径 | PyTorch tensor/extension 总体生命周期；TMA 变体未完整审计 | 部分完成 |
| 数据生命周期 | global Q/K/V、SMEM Q/K/V alias、register S/P/m/l/O | 已完成 |
| 执行上下文 | block/warp sync、cp.async wait、shuffle；stream/launch error 未统一 | 部分完成 |
| 行级证据 | 代表 kernel 关键逻辑均有当前源码行号 | 已完成 |
| Demo 映射 | D02 覆盖 standalone FA benchmark；D01 覆盖扩展观察边界 | 部分完成 |
| 总体状态 | tiling-QKV 深度完成；所有变体逐文件矩阵仍未完成 | 部分完成 |

## 3. 不能过度推断

- `assert` 存在不等于非整 tile 被安全处理。
- `D` dispatch 列表不等于所有 batch/head/sequence 组合支持。
- source builder 编译了某 `.cu` 不等于该变体在当前 GPU 被运行。
- F32 accumulator 导出不等于与 F16 accumulator 有同一误差阈值。
- 共享 Q/V 地址别名不等于可以删除同步。
