# 配置影响地图

- 对应源码版本：`main` / `4513b31`。
- 证据状态：构建脚本和 CLI 的显式影响已确认；环境变量的完整影响矩阵未知。
- 最后更新：2026-09-10

| 配置 | 影响模块 | 影响方式 | 证据 |
|---|---|---|---|
| `TORCH_CUDA_ARCH_LIST` | 普通 PyTorch extension | 改变即时编译目标架构 | `kernels/elementwise/elementwise.py:9-24` |
| `--arch sm_86/sm_89/sm_90a/sm_120a` | M09 | 选择 gencode、宏、库和输出名 | `kernels/interview/build.sh:45-81` |
| `--clean` | M09 | 删除 object/binary 生成物 | `kernels/interview/build.sh:83-126` |
| CUDA bare-metal version | M06 | setup.py 是否追加 `sm_90` | `kernels/hgemm/setup.py:21-38` |
| GPU name | M07 | 设置 L20/4090/3080 编译宏 | `kernels/flash-attn/flash_attn_mma.py:150-203` |
| `--D/--B/--H/--N` | M07 | shape、支持过滤、参考路径和显存压力 | `kernels/flash-attn/flash_attn_mma.py:22-55,214-227` |
| `--wmma/--mma/--cuda/--cute-tn` | M06 | 选择 benchmark kernel family | `kernels/hgemm/hgemm.py:18-177` |
| `--warmup/--iters` | M06/M07/普通脚本 | 计时稳定性和总运行时间 | `kernels/hgemm/hgemm.py:210-328` |
| `FLASH_ATTN_MMA_DEBUG` | M07 | debug 编译路径 | `kernels/flash-attn/flash_attn_mma.py:150-203` |

## 变更规则

修改编译宏时必须重新检查 include、PTX 指令可用性、shared-memory 上限和对应 GPU 的 correctness；修改 benchmark 参数时不能把历史 TFLOPS 与新配置直接比较。

## 未解决问题

仓库没有集中配置 schema，也没有自动生成架构支持矩阵；当前表需要随脚本新增选项维护。
