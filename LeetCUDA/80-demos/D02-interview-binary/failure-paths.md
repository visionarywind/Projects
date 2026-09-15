# D02-S05 失败路径、清理和架构边界

- 文档目的：解释 80-demos/D02-interview-binary/failure-paths.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/LeetCUDA HEAD 4513b31（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 80-demos/D02-interview-binary/failure-paths.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## 构建失败

| 失败 | 可能边界 | 证据/状态 |
|---|---|---|
| `/usr/local/cuda/bin/nvcc` 不存在 | 脚本硬编码路径 | `[kernels/interview/build.sh:13-43]`，已确认 |
| include/library 缺失 | CUTLASS/cudnn-frontend 子模块或 CUDA/cuDNN | `[kernels/interview/build.sh:29-43]`，当前未验证 |
| 架构不支持 | gencode/宏与 GPU 不匹配 | `[kernels/interview/build.sh:45-81]`，风险 |
| 编译某架构失败 | WGMMA/TMA/CuTe 分支条件 | 运行未知 |

## 运行失败

- `check()` 遇到 CUDA error 会打印位置和错误字符串并 `exit(EXIT_FAILURE)`。[kernels/interview/notes-v2.cu:92-97]
- `check_smem_feasible` 可能跳过不可行 shared-memory 配置，而不是让其 launch。[kernels/interview/notes-v2.cu:82-90]
- sequence length、alignment 或 D 不满足条件的 attention 配置可能被跳过；具体 phase 需看 CLI dispatch。[kernels/interview/notes-v2.cu:3094-3190]
- warmup 后未包 `check` 的同步是错误观察边界，需要后续修正或记录。

## 资源失败和清理

正常路径明确释放 host/device buffers、events、streams、cuBLAS handle 和 TMA map。[kernels/interview/notes-v2.cu:2420-2579,3094-3190,3400-3497]。若中途 `exit`，依赖进程/driver 回收；函数级异常安全不是统一 RAII 契约，完整覆盖未知。

## 未验证事项

当前没有真实 binary、GPU 输出、SMEM 可行性矩阵、TMA map 运行结果、sanitizer 或 profile。文档中所有命令仅是复现配方。

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
