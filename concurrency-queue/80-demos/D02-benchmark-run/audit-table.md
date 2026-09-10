# D02 深度审计表

- 文档目的：检查性能 Demo 的输入、测量和产物是否可复现。
- 证据状态：静态确认；没有运行数据。
- 最后更新：2026-09-10
- 前置阅读：[执行轨迹](execution-trace.md)
- 后续阅读：[结果格式](../../01-modules/M06-benchmarks/result-format.md)

| 维度 | 必须记录 | 当前状态 |
|---|---|---|
| 版本 | commit、编译器、flags | 待运行 |
| 硬件 | CPU、核数、OS | 待运行 |
| 场景 | producer/consumer/batch | 需从输出确认 |
| 被测实现 | adapter 和 traits | 源码已确认 |
| 原始数据 | benchmark log | 未生成 |
| 转换 | script、CSV schema | 脚本存在，格式待确认 |
| 结论 | 置信区间/重复次数 | 未知 |

禁止在缺少上述信息时提交性能排名或回归阈值。
