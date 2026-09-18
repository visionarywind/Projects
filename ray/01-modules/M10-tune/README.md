# M10 Ray Tune

- 文档目的：说明 Tune 如何管理搜索、Trial、调度、结果和恢复。
- 对应源码版本：HEAD `cfe4725d23`（2026-09-17）。
- 证据状态：Tuner、TunerInternal、TuneController、Trial 的代表链已静态确认；运行和测试未验证。
- 前置阅读：[模块注册表](../module-registry.md)。后续阅读：[实现](implementation.md) 与 [调用链](call-chains.md)。

## 结论摘要

`Tuner.fit` 接受 trainable、param space 和 RunConfig，交给 `TunerInternal`；TuneController 管理 Trial 状态和调度循环，searcher/scheduler 产生配置与停止决策，TrialExecutor 负责实际试验资源、结果和 checkpoint。[`tuner.py:43-425`; `tuner_internal.py:63-698`; `tune_controller.py:72-2034`; `trial.py:216-1081`]

## 代表链

```text
Tuner.fit
→ TunerInternal.fit
→ TuneController setup/add trials
→ searcher 产生配置 + scheduler 决策
→ TrialExecutor 启动/恢复 trainable
→ result/checkpoint
→ step 继续、暂停、停止或完成
→ cleanup
```

## 状态与分支

Trial 的 pending/running/paused/terminated 状态由 controller 推进；ASHA/PBT 等 scheduler、resume、资源不足、同步/异步结果和 error limit 改变路径。checkpoint 必须在保存、恢复和清理之间保持 ownership 契约。

## 深度审计

|对象|入口落地|正常|分支|异常|清理|数据|上下文|证据|Demo|状态|
|---|---|---|---|---|---|---|---|---|---|---|
|M10|Tuner/Controller/Trial|fit→trial result|search/scheduler/resume/resource|trainable/save-restore failure|cleanup/trial stop|Trial/Result/Checkpoint|driver/controller/trial|代表源码已列|D02|动态未验证|

## 相关文档

[项目架构](../../00-overview/architecture.md) · [实现](implementation.md) · [D02](../../80-demos/D02-data-tune-resource/README.md)

## 源码证据摘要

`python/ray/tune/tuner.py:43-425`；`python/ray/tune/impl/tuner_internal.py:63-698`；`python/ray/tune/execution/tune_controller.py:72-2034`；`python/ray/tune/experiment/trial.py:216-1081`。

## 未解决问题

真实 scheduler/searcher 策略、checkpoint 恢复、资源复用和 failure recovery 仍需运行专项测试；静态代表链已覆盖这些状态边界。

## 下一步阅读建议

先读 `Tuner.fit`，再沿 `TuneController.step` 和 `Trial` 状态转移阅读。
