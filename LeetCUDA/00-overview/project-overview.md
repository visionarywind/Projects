# 项目定位、能力与边界

- 文档目的：说明 LeetCUDA 为什么存在、提供什么能力以及不承诺什么。
- 适用范围：当前仓库 `main` / `4513b31`。
- 对应源码版本：`4513b31`。
- 证据状态：部分已确认，设计意图部分来自已有文档。
- 最后更新：2026-09-10
- 前置阅读：根 [README](../README.md)
- 后续阅读：[architecture.md](architecture.md)

## 结论摘要

LeetCUDA 不是单一运行时或生产库，而是围绕“先实现，再优化”的 CUDA 学习与实验集合。它用一组由易到难的 kernel、PyTorch 绑定和 benchmark，把 GPU 架构、内存层次、并行归约、矩阵乘、Tensor Core、FlashAttention 和性能分析串成可运行示例。仓库同时导出/复用若干相对完整的 toy library，但已有模块明确提示：追求最优生产性能时应使用 cuBLAS、cuDNN、官方 FlashAttention 或 TensorRT。

## 解决的问题

| 问题 | 仓库中的解决方式 | 状态 |
|---|---|---|
| 如何从 CUDA 基础逐步学习性能优化 | 按 easy → hard++ 列出算子和专题，并在代码中保留 WHY/HOW 注释 | 已确认，[README.md:265-279] |
| 如何比较自定义 kernel 与框架实现 | Python 侧即时编译扩展、预热、同步、计时并打印误差/TFLOPS | 已确认，[kernels/elementwise/elementwise.py:9-24,27-66] |
| 如何集中复习高频 CUDA 面试题 | `interview/notes-v2.cu` include 多个 `.cuh`，提供 test/bench CLI | 已确认，[kernels/interview/README.md:6-16] |
| 如何展示 Tensor Core 逐级优化 | HGEMM 从 naive/WMMA/MMA/CuTe/WGMMA 到多 stage、swizzle | 已确认，[kernels/hgemm/README.md:38-79] |

## 能力边界

- 输入/输出主要是 CUDA device tensor 或裸 CUDA 指针；没有统一的跨算子 API、服务进程或数据库。
- Python 示例往往在 import/执行脚本时就构建扩展和运行 benchmark，并非可直接嵌入的稳定库接口。
- kernel 正确性、支持的 dtype、布局、尺寸和 SM 架构由每个模块单独决定；不能从某个模块的 README 推广到全仓库。
- benchmark 数据与硬件、CUDA、PyTorch、编译选项和迭代参数绑定；仓库历史输出不是普遍性能保证。
- 本知识库不把第三方 CUTLASS/CUDNN frontend 代码纳入分析，只追踪它们作为头文件/子模块的构建边界。

## 读者模型

初学者先掌握线程/warp/block、global/shared/register memory 与同步，再阅读 `kernels/elementwise` 和 `kernels/reduce`；需要修改扩展的读者应先看 M08；需要研究高性能矩阵/注意力的读者进入 M06/M07；面试读者从 M09 的统一入口开始。

## 事实、推断与建议

- **已确认**：仓库 README 将自己描述为“Modern CUDA Learn Notes with PyTorch for Beginners”，并列出 200+ CUDA kernels。[README.md:0-15]
- **已确认**：贡献指南强调学习/实践优先，最优性能不是最终目标。[CONTRIBUTE.md:2-3]
- **推断**：目录按学习主题而非严格软件分层组织；这是因为多个目录各自包含可独立运行的教学样例。
- **建议**：将每个 kernel 的 Python 脚本视为实验入口，将 `.cu/.cuh` 视为实现证据，将 README 中的 benchmark 视为参考而非测试基线。

## 相关文档

- [architecture.md](architecture.md)
- [design-principles.md](design-principles.md)
- [../01-modules/module-registry.md](../01-modules/module-registry.md)

## 源码证据摘要

- `[README.md:15-16]`：项目能力总览。
- `[README.md:265-279]`：学习难度、kernel 与 PyTorch workflow。
- `[CONTRIBUTE.md:2-3]`：目标与边界。

## 未解决问题

- 没有统一的版本发布/兼容性策略文件。
- 没有统一的全仓库测试编排入口；各目录脚本独立运行。

## 下一步阅读建议

阅读架构图后，根据任务进入 [模块注册表](../01-modules/module-registry.md)。
