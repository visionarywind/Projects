# AG-K08 评估、可观测性与调试

## 核心问题
如何分别衡量最终结果、工具行为、轨迹安全、成本、延迟和可复现性。

## 先修与不变量
先修：测试设计、统计和 OpenTelemetry。关键不变量：每个动作可关联 trace；oracle 独立于被评估输出；judge 不被无关格式分数替代；敏感数据脱敏。

## 题目索引
代表题目：AG-B005、AG-I005、AG-A005、AG-E002；完整索引见 `../00-overview/coverage-matrix.md`。

## 实践入口
用固定 golden set 和人工抽样校准 judge，报告分层指标与置信区间。
