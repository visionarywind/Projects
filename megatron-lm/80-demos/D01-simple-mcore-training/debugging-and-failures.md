# D01 调试与失败模式

- 文档目的：解释 80-demos/D01-simple-mcore-training/debugging-and-failures.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/megatron-lm HEAD 3703d4e33（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 80-demos/D01-simple-mcore-training/debugging-and-failures.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


| 失败点 | 可能原因 | 排查 |
|---|---|---|
| `RANK/WORLD_SIZE` 缺失 | 未用 torchrun 启动 | 检查 launcher 环境变量 |
| CUDA device 设置失败 | 无 GPU 或 local rank 错误 | 检查 `CUDA_VISIBLE_DEVICES` 与 nproc |
| NCCL init 失败 | 驱动/NCCL/网络问题 | 先运行最小 torch distributed smoke test |
| helper 编译失败 | 编译器或依赖缺失 | 查看 dataset helper build 输出 |
| schedule assertion | model/iterator 数量不匹配 | D01 应为单 model、单 iterator |
| checkpoint load 失败 | 文件缺失或 sharded metadata 不匹配 | 检查 `ckpt` 目录和 rank 数 |

当前文档未执行命令，因此不提供实测错误输出。

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
