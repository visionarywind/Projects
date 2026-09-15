# 实践实验

## 使用约束

以下实验是可复现的验证方案，不是本工作区已经执行的结果。除非另有授权，不连接真实生产系统、真实凭据、外部写接口或高成本模型服务；默认使用 fake model、fake tool、固定时钟和隔离容器。所有结果记录环境、版本、输入、trace、oracle 和指标，并标记 `[待验证]`。

## 实验清单

| 实验 | 对应题目 | 目标 | 主要 oracle |
|---|---|---|---|
| 最小 Agent loop | B002 | 正确处理 tool call、observation 和终态 | call/result 一一对应、无双终态 |
| Schema 与权限门 | B003/A003 | 区分结构校验和授权 | 非法参数与越权零副作用 |
| 分层评估 | B005/I005/E002 | 发现结果正确但轨迹违规 | 规则 oracle 与人工标注一致性 |
| 可恢复执行 | A001/E001 | 注入崩溃、重复投递、超时 | 幂等计数、fencing、UNKNOWN 对账 |
| 并发与预算 | A002/A004 | 比较串行、并行和早停 | P95、尾延迟、每成功任务成本 |
| 注入纵深防御 | A003/E003 | 检验内容隔离和执行侧拒绝 | 外传/越权副作用为零 |
| 版本灰度 | A005/E004/E005 | 回放、canary、回滚和旧 run | bundle 固定、隔离、回滚时间 |

## 通用记录格式

```text
case_id, tenant, input_fixture, model_version, prompt_version,
tool_version, policy_version, index_version, trace_id,
expected_oracle, observed_events, latency_ms, token_count, cost, verdict
```

## 通过条件

实验只能在 oracle 明确时判定通过；“回答看起来合理”不足以证明工具安全、权限正确或恢复无重复副作用。未覆盖的攻击和故障应记录为残余风险，而不是默认为通过。
