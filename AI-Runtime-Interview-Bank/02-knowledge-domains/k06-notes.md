# K06 LLM Inference/KV Cache

## 学习目标

围绕 LLM Inference/KV Cache，把概念、调用链、生命周期、并发、容量、性能、故障和验证组织成可复述的证据链。题库中的结论优先采用通用模型；目标版本源码与个人项目细节标为 `[待验证]` 或 `[P-待补充]`。

## 覆盖题目

| ID | 难度 | 主题 | 角度 |
|---|---|---|---|
| I081 | 中等级 | KV bytes/token | 概念辨析 |
| I082 | 中等级 | KV bytes/token | 流程追踪 |
| I083 | 中等级 | KV bytes/token | 故障排查 |
| I084 | 中等级 | KV bytes/token | 验证设计 |
| I085 | 中等级 | prefill 与 decode | 概念辨析 |
| I086 | 中等级 | prefill 与 decode | 流程追踪 |
| I087 | 中等级 | prefill 与 decode | 故障排查 |
| I088 | 中等级 | prefill 与 decode | 验证设计 |
| A081 | 高级 | Paged KV Cache allocator | 概念辨析 |
| A082 | 高级 | Paged KV Cache allocator | 流程追踪 |
| A083 | 高级 | Paged KV Cache allocator | 故障排查 |
| A084 | 高级 | Paged KV Cache allocator | 验证设计 |
| A085 | 高级 | continuous batching 调度 | 概念辨析 |
| A086 | 高级 | continuous batching 调度 | 流程追踪 |
| A087 | 高级 | continuous batching 调度 | 故障排查 |
| A088 | 高级 | continuous batching 调度 | 验证设计 |
| E081 | 专家级 | KV Cache 全局容量与 admission | 概念辨析 |
| E082 | 专家级 | KV Cache 全局容量与 admission | 流程追踪 |
| E083 | 专家级 | KV Cache 全局容量与 admission | 故障排查 |
| E084 | 专家级 | KV Cache 全局容量与 admission | 验证设计 |
| E085 | 专家级 | 推理调度的公平、吞吐和 P99 | 概念辨析 |
| E086 | 专家级 | 推理调度的公平、吞吐和 P99 | 流程追踪 |
| E087 | 专家级 | 推理调度的公平、吞吐和 P99 | 故障排查 |
| E088 | 专家级 | 推理调度的公平、吞吐和 P99 | 验证设计 |

## 通用检查框架

1. 先画入口、队列、设备/网络、完成和回收边界。
2. 明确 owner、借用者、状态机和 happens-before。
3. 分离 active/reserved/free/deferred、排队/计算/传输/同步时间。
4. 为异常路径定义超时、取消、隔离、重试、清理和回滚。
5. 用 baseline、控制变量、oracle、故障注入和回归阈值形成证据。

## 个人经历映射

- `[P-已确认]`：13 年系统软件经验及用户自述的 MindSpore、UMD、显存池、多 Stream、Host 网络、检索引擎、Java 经历类别。
- `[P-待补充]`：具体模块、提交、故障、指标、规模、本人职责和版本。不得用通用答案替代这些事实。
