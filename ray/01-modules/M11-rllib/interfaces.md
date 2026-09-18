# RLlib：接口契约

## 输入
调用方须提供可验证、可序列化且符合生命周期的输入；入口证据：`rllib/algorithms/algorithm.py:211-4863`、`algorithm_config.py:109-6238`、EnvRunnerGroup/Learner。

## 输出与副作用
- config → Algorithm.setup → sample → Learner.update → step result/checkpoint
- 核心数据：AlgorithmConfig、SampleBatch、RLModule、metrics。

## 失败
env/model/learner/worker/config failure。

跨模块修改需同步 binding/proto/日志/metrics/测试。
