# 统一术语表

- 文档目的：固定跨文档术语，避免把不同层概念混用。
- 适用范围：全知识库。
- 对应源码版本：`8190837c2b6ce176a431bc2a6ffd3439507648a7`
- 证据状态：已确认/术语解释部分推断
- 最后更新：2026-09-10

| 术语 | 含义 | 代码证据 |
|---|---|---|
| Megatron Core | 可组合的 GPU 训练基础库 | `README.md:17-19` |
| Megatron-LM | 含参考训练脚本的应用层 | `README.md:15-18` |
| TP | Tensor Parallelism，层内张量切分 | `parallel_state.py:28-32` |
| PP | Pipeline Parallelism，层间/阶段切分 | `parallel_state.py:33-36` |
| DP | Data Parallelism，数据副本并行 | `parallel_state.py:42-44` |
| CP | Context Parallelism，上下文维度并行 | `parallel_state.py:123-127` |
| EP | Expert Parallelism，MoE experts 并行 | `parallel_state.py:49-84` |
| VP | Virtual Pipeline，单物理 stage 的交错虚拟 stage | `schedules.py:161-167` |
| ProcessGroup | torch.distributed 通信参与者集合 | `parallel_state.py:232-260` |
| ProcessGroupCollection | 显式传递的并行通信上下文 | `gpt_model.py:95,120-130` |
| ModuleSpec | 模型层/模块的可组合规格 | `gpt_builders.py:31-54` |
| microbatch | 被 pipeline schedule 单独驱动的 batch 分片 | `schedules.py:70-121` |
| sharded state dict | 按分布式 rank 分片的状态表示 | `examples/run_simple_mcore_train_loop.py:183-216` |
| MCore | Megatron Core 的常用简称 | 仓库文档/代码命名 |
| TE | Transformer Engine，可选 GPU/低精度 backend | `optimizer/__init__.py:13-35` |
| golden value | functional test 的预期数值输出 | `tests/README.md:17-25` |

## 相关文档

- [项目定位](project-overview.md)
- [模块注册表](../01-modules/module-registry.md)

## 源码证据摘要

见表格。

## 未解决问题

部分术语（如具体 CP 算法、GTP remat）需要对应专题文档，不在本词汇表展开。

## 下一步阅读建议

遇到缩写先回到此表，再进入对应模块。
