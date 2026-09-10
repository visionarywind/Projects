# D02-S05 失败路径、清理和架构边界

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
