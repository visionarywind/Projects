# M01 源码地图

| 符号/文件 | 作用 | 证据 |
|---|---|---|
| `common.cuh` | 公共 CUDA/C++ 和硬件辅助 | `[kernels/interview/README.md:6-16]` |
| `base.cuh` | 基础 reduce、elementwise、softmax、norm 等 | `[kernels/interview/README.md:6-16]` |
| `notes-v2.cu` | 编译、测试、benchmark 入口 | `[kernels/interview/notes-v2.cu:1-20]` |
| `check_smem_feasible` | 动态 shared memory 可行性检查 | `[kernels/interview/notes-v2.cu:82-90]` |
| `check` | CUDA 错误检查并退出 | `[kernels/interview/notes-v2.cu:92-97]` |

行号是目标提交基准；深层 header 的符号表尚未完整生成。
