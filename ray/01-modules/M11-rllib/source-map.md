# RLlib：源码地图

|层次|证据|作用|状态|
|---|---|---|---|
|入口|`rllib/algorithms/algorithm.py:211-4863`、`algorithm_config.py:109-6238`、EnvRunnerGroup/Learner|公共/模块入口|已确认|
|实现|Algorithm 构建 env runners、learners、modules，step 驱动采样学习评估。|真实状态改变|已确认|
|测试|rllib/tests/|回归边界|已定位、未运行|

阅读顺序：入口 → 实现函数体 → 异常/清理 → 测试。
