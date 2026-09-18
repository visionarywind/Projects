# RLlib：调用链

## 主链
```text
config → Algorithm.setup → sample → Learner.update → step result/checkpoint
```

## 实现落点
Algorithm 构建 env runners、learners、modules，step 驱动采样学习评估。

## 失败链
```text
依赖/输入失败 → env/model/learner/worker/config failure。 → 重试、失败传播或清理
```
