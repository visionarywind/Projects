# K05 Collective/RDMA

## 学习目标

围绕 Collective/RDMA，把概念、调用链、生命周期、并发、容量、性能、故障和验证组织成可复述的证据链。题库中的结论优先采用通用模型；目标版本源码与个人项目细节标为 `[待验证]` 或 `[P-待补充]`。

## 覆盖题目

| ID | 难度 | 主题 | 角度 |
|---|---|---|---|
| B073 | 入门级 | 通信与计算的基本区别 | 概念辨析 |
| B074 | 入门级 | 通信与计算的基本区别 | 流程追踪 |
| B075 | 入门级 | 通信与计算的基本区别 | 故障排查 |
| B076 | 入门级 | 通信与计算的基本区别 | 验证设计 |
| B077 | 入门级 | AllReduce 基本语义 | 概念辨析 |
| B078 | 入门级 | AllReduce 基本语义 | 流程追踪 |
| B079 | 入门级 | AllReduce 基本语义 | 故障排查 |
| B080 | 入门级 | AllReduce 基本语义 | 验证设计 |
| I073 | 中等级 | Ring 与 Tree AllReduce | 概念辨析 |
| I074 | 中等级 | Ring 与 Tree AllReduce | 流程追踪 |
| I075 | 中等级 | Ring 与 Tree AllReduce | 故障排查 |
| I076 | 中等级 | Ring 与 Tree AllReduce | 验证设计 |
| I077 | 中等级 | 通信 buffer 生命周期 | 概念辨析 |
| I078 | 中等级 | 通信 buffer 生命周期 | 流程追踪 |
| I079 | 中等级 | 通信 buffer 生命周期 | 故障排查 |
| I080 | 中等级 | 通信 buffer 生命周期 | 验证设计 |
| A073 | 高级 | 集合通信 overlap pipeline | 概念辨析 |
| A074 | 高级 | 集合通信 overlap pipeline | 流程追踪 |
| A075 | 高级 | 集合通信 overlap pipeline | 故障排查 |
| A076 | 高级 | 集合通信 overlap pipeline | 验证设计 |
| A077 | 高级 | RDMA QP/CQ/MR 资源路径 | 概念辨析 |
| A078 | 高级 | RDMA QP/CQ/MR 资源路径 | 流程追踪 |
| A079 | 高级 | RDMA QP/CQ/MR 资源路径 | 故障排查 |
| A080 | 高级 | RDMA QP/CQ/MR 资源路径 | 验证设计 |
| E073 | 专家级 | 大规模 Collective 的拓扑与弹性 | 概念辨析 |
| E074 | 专家级 | 大规模 Collective 的拓扑与弹性 | 流程追踪 |
| E075 | 专家级 | 大规模 Collective 的拓扑与弹性 | 故障排查 |
| E076 | 专家级 | 大规模 Collective 的拓扑与弹性 | 验证设计 |
| E077 | 专家级 | RDMA/NCCL/HCCL 故障治理 | 概念辨析 |
| E078 | 专家级 | RDMA/NCCL/HCCL 故障治理 | 流程追踪 |
| E079 | 专家级 | RDMA/NCCL/HCCL 故障治理 | 故障排查 |
| E080 | 专家级 | RDMA/NCCL/HCCL 故障治理 | 验证设计 |

## 通用检查框架

1. 先画入口、队列、设备/网络、完成和回收边界。
2. 明确 owner、借用者、状态机和 happens-before。
3. 分离 active/reserved/free/deferred、排队/计算/传输/同步时间。
4. 为异常路径定义超时、取消、隔离、重试、清理和回滚。
5. 用 baseline、控制变量、oracle、故障注入和回归阈值形成证据。

## 个人经历映射

- `[P-已确认]`：13 年系统软件经验及用户自述的 MindSpore、UMD、显存池、多 Stream、Host 网络、检索引擎、Java 经历类别。
- `[P-待补充]`：具体模块、提交、故障、指标、规模、本人职责和版本。不得用通用答案替代这些事实。
