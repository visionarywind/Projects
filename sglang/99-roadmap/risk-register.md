# 风险登记

| 风险 | 触发场景 | 防护 |
|---|---|---|
| IPC 字段错位 | 新字段未同步 batch slicing/serializer | 检查 M03/M04/M15 契约和 batch tests |
| state 泄漏 | dispatch 后只删本地 state | 区分 dispatch 前后 cleanup，检查 abort |
| KV 悬挂映射 | 释放 slot 未更新 row/radix lock | 联查 M08 allocator、prefix node 和 Req metadata |
| overlap race | 原地修改共享 batch/metadata | 检查 snapshot、stream/event 和 shared read ends |
| graph 错用 | 动态 shape 或 buffer 不满足 replay | 依赖 `can_run_graph`，验证 eager fallback |
| 输出错配 | filter/merge 后数组顺序改变 | 按 rid 和 batch index 做断言 |
| 配置漂移 | 派生值在不同进程重新计算 | resolve once 后 publish |
| 过度声称验证 | 未跑 GPU/模型却写成功 | 明确“未验证”并记录命令状态 |

该表是静态分析风险登记，不是生产安全评估。
