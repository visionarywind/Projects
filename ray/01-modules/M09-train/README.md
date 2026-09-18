# M09 Ray Train

- 文档目的：说明 Train 如何编排 trainer、controller、worker group、training loop、metrics 和 checkpoint。
- 对应源码版本：HEAD `cfe4725d23`（2026-09-17）。
- 证据状态：BaseTrainer、DataParallelTrainer、TrainController、WorkerGroup 代表链已静态确认；运行和测试未验证。
- 前置阅读：[模块注册表](../module-registry.md)。后续阅读：[实现](implementation.md)。

## 结论摘要

`BaseTrainer.fit` 将 `ScalingConfig`、`RunConfig` 和 user training loop 交给 controller；controller 创建 WorkerGroup，worker 在 distributed context 中执行 training loop 并报告 metrics/checkpoint。[`base_trainer.py:128-936`; `data_parallel_trainer.py:29-603`; `controller.py:114-977`; `worker_group.py:98-1204`]

## 代表链

```text
BaseTrainer.fit
→ TrainController
→ WorkerGroup.create
→ worker setup/context
→ DataParallelTrainer.training_loop
→ report metrics/checkpoint
→ aggregate Result
→ WorkerGroup.shutdown
```

故障恢复、checkpoint、dataset、resource scaling、rank/world size 和 v1/v2 controller 改变状态路径；具体分布式运行拓扑未验证。

## 深度审计

|对象|入口落地|正常|分支|异常|清理|数据|上下文|证据|Demo|状态|
|---|---|---|---|---|---|---|---|---|---|---|
|M09|Trainer/controller/worker group|fit→workers→loop→Result|scale/checkpoint/recovery/v1-v2|TrainingFailedError/worker/user failure|controller/WorkerGroup shutdown|Scaling/Context/Checkpoint/Result|driver/controller/workers|代表源码已列|无专门 Demo|动态未验证|

## 相关文档

[项目架构](../../00-overview/architecture.md) · [实现](implementation.md) · [风险](risks-and-debt.md)

## 源码证据摘要

`python/ray/train/base_trainer.py:128-936`；`python/ray/train/data_parallel_trainer.py:29-603`；`python/ray/train/v2/_internal/execution/controller/controller.py:114-977`；`worker_group.py:98-1204`。

## 未解决问题

训练 worker failure recovery、checkpoint ownership、v1/v2 差异、GPU/多节点性能仍需专项运行测试；静态代表链已覆盖主要生命周期。

## 下一步阅读建议

先读 `BaseTrainer.fit` 和 `DataParallelTrainer.training_loop`，再沿 controller 和 WorkerGroup 的 create/shutdown 阅读。
