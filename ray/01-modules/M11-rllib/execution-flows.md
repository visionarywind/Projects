# RLlib：执行流

## 正常流
```text
入口 → 配置/参数归一化 → 内部状态创建 → config → Algorithm.setup → sample → Learner.update → step result/checkpoint → 结果/事件
```

## 分支
新旧 API stack、multi-agent、evaluation、offline、device。

## 异常
env/model/learner/worker/config failure。

## 终止
Algorithm.cleanup 与 worker/learner shutdown。
