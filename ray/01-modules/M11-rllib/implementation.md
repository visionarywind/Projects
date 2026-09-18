# RLlib：实现

## 入口到副作用
Algorithm 构建 env runners、learners、modules，step 驱动采样学习评估。

## 正常
config → Algorithm.setup → sample → Learner.update → step result/checkpoint

## 分支与异常
- 新旧 API stack、multi-agent、evaluation、offline、device。
- env/model/learner/worker/config failure。

## 清理
Algorithm.cleanup 与 worker/learner shutdown。

## 数据与执行上下文
- AlgorithmConfig、SampleBatch、RLModule、metrics。
- driver Algorithm、EnvRunner actors、Learner actors/threads。
