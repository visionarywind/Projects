# RLlib：行级分析

## 证据锚点
- `rllib/algorithms/algorithm.py:211-4863`、`algorithm_config.py:109-6238`、EnvRunnerGroup/Learner

## 结论
- 入口负责验证/归一化，副作用发生于：Algorithm 构建 env runners、learners、modules，step 驱动采样学习评估。
- 分支：新旧 API stack、multi-agent、evaluation、offline、device。
- 错误：env/model/learner/worker/config failure。

动态行为未运行，不写成已验证。
