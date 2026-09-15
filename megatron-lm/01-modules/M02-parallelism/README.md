# M02 并行状态与 Pipeline 调度

- 文档目的：解释 Megatron Core 如何从并行配置建立 process groups，并用 schedule 驱动 microbatch。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：`3703d4e33a3a2b2d11ebcc8e41f45af7ce7d1eda`
- 证据状态：静态源码分析；未进行 GPU/NCCL 动态验证。
- 最后更新：2026-09-11
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M02-parallelism/README.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。

## 模块边界

M02 包含 `megatron/core/parallel_state.py` 与 `megatron/core/pipeline_parallel/`。前者负责 rank 拓扑、通信组、全局 buffer 和销毁；后者负责 forward/backward 顺序、microbatch bookkeeping、P2P 激活/梯度通信和梯度 finalize 的调用时机。它不拥有模型参数更新，也不负责构造数据集。

## 主链

```text
initialize_megatron
  -> torch.distributed.init_process_group
  -> initialize_model_parallel
    -> RankGenerator
    -> create_group(NCCL/Gloo)
  -> get_forward_backward_func
    -> no-pipeline / PP / interleaved schedule
      -> forward_step + backward_step
      -> P2P communicator
      -> finalize_model_grads
```

## 并行轴

| 轴 | 作用 | 主要状态 |
|---|---|---|
| TP | 切分单个权重/算子 | tensor model parallel group |
| PP | 切分层和 stage | pipeline group、P2P |
| VP | 每个 PP rank 持有多个虚拟 chunk | virtual pipeline rank/world size |
| DP | 复制模型并归约梯度 | data parallel group |
| CP | 切分序列激活 | context parallel group，梯度归约复用 DP 域 |
| EP | 切分 routed experts | expert model/data groups |
| GTP | 权重 rematerialization 轴 | GTP weight remat group |

## 关键不变量

- `torch.distributed` 必须先初始化；world size 必须能被模型并行规模整除。
- VP 只有在 PP 大于 1 时合法。
- GTP rematerialization 与多个 distributed-optimizer instance 当前由初始化逻辑禁止组合。
- decoder 与 expert rank generator 产生的 PP group 必须一致。
- schedule 的 model/data iterator 数量必须与选择的调度类型匹配。

## 证据索引

- 并行组初始化：[parallel_state.py:600-1029]
- 调度选择与输出释放：[schedules.py:53-220]
- 无 pipeline 调度：[schedules.py:723-928]
- 非交错 pipeline：[schedules.py:2147-约 2660]
- D01 调用：[examples/run_simple_mcore_train_loop.py:246-266]

## 未验证项

实际 rank placement、NCCL communicator 建立顺序、通信 overlap 和多节点性能需要相应硬件与 launcher 验证。

## 文档元数据（规范补充）

- 文档目的：说明 `01-modules/M02-parallelism/README.md` 的源码分析范围、结论和维护入口。
- 适用范围：当前项目对应模块/入口的静态源码与测试分析。
- 对应源码版本：以本项目 `00-overview/analysis-state.md` 或同页版本字段为准。
- 证据状态：静态源码证据；未执行的构建、测试、GPU、网络或多进程行为保持“未验证”。
- 最后更新：2026-09-15
- 前置阅读：本项目根 README 与 `00-overview/analysis-state.md`。
- 后续阅读：本模块/示例的实现、测试和风险页面。

## 深度审计

| 分析对象 | 入口落地 | 正常路径 | 分支 | 异常 | 清理 | 数据生命周期 | 执行上下文 | 行级证据 | Demo 映射 | 状态/缺口 |
|---|---|---|---|---|---|---|---|---|---|---|
| `01-modules/M02-parallelism/README.md` | 已完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 已映射或不适用 | 部分完成：动态行为、边界或专用变体仍需验证 |

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
