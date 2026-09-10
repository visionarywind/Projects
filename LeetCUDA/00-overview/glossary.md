# 统一术语表

- 文档目的：统一 CUDA、PyTorch 扩展和性能分析术语。
- 适用范围：全部知识库。
- 对应源码版本：`0983c65`。
- 证据状态：术语解释依据源码/README；部分为通用 CUDA 定义。
- 最后更新：2026-09-10
- 前置阅读：[README](../README.md)
- 后续阅读：[architecture.md](architecture.md)

| 术语 | 含义 | 本仓库语境 |
|---|---|---|
| kernel | GPU 上执行的 `__global__`/设备函数 | `.cu` 中实际计算入口 |
| launcher | 在 CPU/C++ 侧计算尺寸并发起 kernel 的函数 | PyBind 导出函数或 interview 测试函数 |
| warp | 通常 32 个线程的执行协作组 | NMS ballot、warp reduce、MMA 映射 |
| block | CUDA thread block | shared memory 与 `__syncthreads` 的同步边界 |
| global memory | GPU 全局显存 | 输入/输出 tensor 的主存储 |
| shared memory/SMEM | block 内共享的低延迟存储 | GEMM/attention tile、padding/swizzle |
| register | 线程私有寄存器 | thread tile、累加器、double buffer |
| tile | 分块处理的矩阵/序列子区域 | BM/BN/BK、Br/Bc、head-dim tile |
| vectorize/pack | 一次加载多个元素 | `float4`、`half2`、128-bit LD/ST |
| WMMA/MMA | Tensor Core 编程接口/指令形态 | HGEMM 与 attention 的 Tensor Core 路径 |
| WGMMA | Hopper warp-group MMA | interview/HGEMM Hopper 路径 |
| CuTe/CUTLASS | NVIDIA 的布局/模板抽象 | HGEMM/FlashAttention 高级实现 |
| stage | pipeline 中的缓冲阶段 | 多 stage K/V 加载与计算重叠 |
| swizzle | 改变线程/地址布局以改善 bank/cache 行为 | block/warp/SMEM swizzle |
| online softmax | 流式维护 max/sum，避免保存全矩阵 logits | FlashAttention 的融合 softmax |
| NMS | Non-Maximum Suppression，按分数抑制重叠框 | 两阶段 bitmask + resolve |
| TFLOPS | 每秒万亿次浮点操作 | GEMM/attention benchmark 指标 |
| contiguous | tensor 内存连续布局 | 多数 kernel 的隐含前置条件 |
| SM architecture | GPU compute capability 目标 | `sm_80`、`sm_89`、`sm_90a`、`sm_120a` |

## 相关文档

- [architecture.md](architecture.md)
- [../01-modules/M06-hgemm-tensorcore/glossary.md](../01-modules/M06-hgemm-tensorcore/glossary.md)

## 源码证据摘要

- `[kernels/interview/README.md:8-16]`：公共宏、MMA/WGMMA/TMA 和文件层次。
- `[kernels/nms/nms.cu:10-20]`：warp/bitmask 语义。
- `[kernels/hgemm/README.md:18-24]`：HGEMM 优化词汇。

## 未解决问题

- 各模块对 “stage/swizzle” 的具体 layout 不是同一实现，术语不能替代参数级契约。

## 下一步阅读建议

遇到陌生缩写先回到本表，再读对应模块的源码地图。
