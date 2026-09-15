# 配置影响地图

- 文档目的：解释 90-cross-module/configuration-impact-map.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：`main` / `4513b31`。
- 证据状态：构建脚本和 CLI 的显式影响已确认；环境变量的完整影响矩阵未知。
- 最后更新：2026-09-10
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 90-cross-module/configuration-impact-map.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


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

## 相关文档
- [项目入口](../README.md)
- [分析状态](../00-overview/analysis-state.md)
- [源码证据索引](../00-overview/evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题

仓库没有集中配置 schema，也没有自动生成架构支持矩阵；当前表需要随脚本新增选项维护。

## 下一步阅读建议
先阅读 [分析状态](../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
