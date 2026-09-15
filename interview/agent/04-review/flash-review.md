# 闪记复习

## 一张纸

- Agent = 模型候选动作 + runtime 状态/预算/策略 + 工具反馈循环。
- 模型提出候选动作；gateway 依据真实身份、租户、资源和参数重新授权。
- 工具调用要区分语法、类型、业务、权限、配额和副作用状态。
- 超时表示结果未知；不可逆写优先查询/对账，不盲重试。
- 恢复依赖事件、checkpoint、幂等、lease/fencing 和终态 CAS。
- memory 要有来源、时间、租户、置信度、版本、TTL 和删除路径。
- RAG 命中不是事实；observation 不能自动成为权限或永久记忆。
- 评估至少分结果、轨迹/工具、安全、证据、成本和延迟层。
- 多 Agent 要有任务边界、证据契约、权限继承、冲突保留和成本预算。
- 高风险发布门是安全、结果、SLO、成本和审计的 conjunction，不是单一分数。

## 高频公式

```text
expected_cost = model_turns * token_cost + tool_calls * tool_cost
concurrency ≈ arrival_rate * mean_service_time
parallel_tail ≈ max(worker_latency) + merge + verification
pass = result_ok ∧ tool_policy_ok ∧ safety_ok ∧ evidence_ok
```

## 高频状态

`RUNNING → WAIT_TOOL → UNKNOWN/WAIT_HUMAN → RUNNING`；终态 `SUCCEEDED/FAILED/CANCELLED` 只提交一次。具体状态名可因实现变化，但语义和不变量必须明确。
