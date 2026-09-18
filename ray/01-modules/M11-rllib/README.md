# M11 RLlib

- 文档目的：说明 RLlib Algorithm 如何组装环境采样、Learner、RLModule、评估和 checkpoint。
- 对应源码版本：HEAD `cfe4725d23`（2026-09-17）。
- 证据状态：Algorithm、AlgorithmConfig、EnvRunnerGroup、Learner 代表链已静态确认；运行和测试未验证。
- 前置阅读：[模块注册表](../module-registry.md)。后续阅读：[实现](implementation.md) 与 [调用链](call-chains.md)。

## 结论摘要

`Algorithm` 根据 `AlgorithmConfig` 建立 EnvRunnerGroup、Learner、RL module 和 evaluation components；`step` 驱动采样、学习、评估和结果汇总，`cleanup` 关闭 runners/learners 并完成资源释放。[`algorithm.py:211-4863`; `algorithm_config.py:109-6238`; `env_runner_group.py:70-1419`; `learner.py:112-1772`]

## 代表链

```text
AlgorithmConfig.build/validate
→ Algorithm.setup
→ EnvRunnerGroup sample
→ Learner.update
→ metrics/evaluation
→ checkpoint/state
→ Algorithm.cleanup
```

新旧 API stack、multi-agent、evaluation、offline data、distributed learner 和 device/framework 会改变组件图；不能从 Algorithm 公共入口推断唯一运行拓扑。

## 深度审计

|对象|入口落地|正常|分支|异常|清理|数据|上下文|证据|Demo|状态|
|---|---|---|---|---|---|---|---|---|---|---|
|M11|Algorithm/Config/Runner/Learner|setup→sample→update|API stack/multi-agent/eval/device|env/model/learner/config failure|Algorithm/runner/learner cleanup|batch/RLModule/metrics|driver+actors/threads|代表源码已列|无专门 Demo|动态未验证|

## 相关文档

[项目架构](../../00-overview/architecture.md) · [实现](implementation.md) · [风险](risks-and-debt.md)

## 源码证据摘要

`rllib/algorithms/algorithm.py:211-4863`；`rllib/algorithms/algorithm_config.py:109-6238`；`rllib/env/env_runner_group.py:70-1419`；`rllib/core/learner/learner.py:112-1772`。

## 未解决问题

新旧 API stack 的差异、采样/学习并发、checkpoint ownership、多智能体 batch 和 worker failure recovery 仍需针对性测试；静态代表路径已记录。

## 下一步阅读建议

先读 `Algorithm.setup` 和 `step`，再按配置选择 EnvRunner、Learner 与 RLModule 实现。
