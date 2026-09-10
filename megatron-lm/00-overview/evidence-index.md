# 源码证据索引

- 文档目的：集中登记总览和跨文档反复使用的最小证据。
- 适用范围：目标提交 `8190837c2b6ce176a431bc2a6ffd3439507648a7`。
- 证据状态：已确认（行号按当前工作树源码核对）
- 最后更新：2026-09-10

| 结论 | 证据 | 支持文档 |
|---|---|---|
| 仓库由 LM 参考程序与 Core 组成 | [README.md:15-21] | project-overview, architecture |
| 顶层结构包含 core/training/tests/examples/tools/docs | [README.md:65-87] | project-overview |
| GPT 入口导入 builder、dataset、training | [pretrain_gpt.py:33-80] | architecture, M03 |
| builder 根据 spec/TE/MoE/异构配置选择实现 | [gpt_builders.py:24-110] | M01 |
| provider 可切换 ModelOpt builder | [model_provider.py:19-58] | M01, cross-module |
| 初始化检查 CUDA 并设置 distributed/model parallel | [megatron/training/initialize.py:48-176] | M02, M03 |
| process_state 存储多维 group | [megatron/core/parallel_state.py:28-165] | M02 |
| group wrapper 调用 torch.new_group | [megatron/core/parallel_state.py:232-260] | M02 |
| schedule 按 PP/VP 选择实现 | [megatron/core/pipeline_parallel/schedules.py:53-168] | M02, M03 |
| GPTModel 构造 embedding/decoder/output | [megatron/core/models/gpt/gpt_model.py:98-242] | M01 |
| D01 初始化、数据、训练和 checkpoint 完整存在 | [examples/run_simple_mcore_train_loop.py:28-283] | D01 |
| optimizer 有 TE/Apex/Torch fallback | [megatron/core/optimizer/__init__.py:13-35] | M05 |
| unit test 使用 distributed runner | [skills/mcore-testing/SKILL.md:117-155] | M03, testing-recipes |
| format script 运行 black/isort/pylint/ruff/mypy | [tools/autoformat.sh:37-42] | build-and-deploy |
| docs 使用 Sphinx | [docs/documentation.md:21-60] | build-and-deploy |

## 引用规则

正文使用 `[相对路径:起始行-结束行]`；符号名称不替代行号。源码变化后应重新核对并更新本索引。

## 相关文档

- [分析状态](analysis-state.md)
- [D01 执行轨迹](../80-demos/D01-simple-mcore-training/execution-trace.md)

## 未解决问题

动态调用（TE kernel、torch autograd C++ engine、NCCL）只有静态入口证据，真实目标需运行或 profiler 确认。

## 下一步阅读建议

从入口证据开始，沿每条链路进入模块文档。
