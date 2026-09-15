# K06：调度、并发、隔离、优先级与配额

## 边界

本知识域讨论 调度、并发、隔离、优先级与配额，并与其他资源域通过设备、任务、内存、通信、错误和 SLO 交叉连接。特定厂商 API、驱动、固件和硬件行为必须绑定目标版本。

## 核心机制

共享异构设备需要处理队列、公平性、优先级、租户配额、背压和故障隔离。

## 常见失效条件

只追求平均利用率，忽略尾延迟、饥饿、优先级反转和 noisy neighbor。

## 题目索引

HRS-B006, HRS-B018, HRS-B030, HRS-B042, HRS-B054, HRS-B066, HRS-B078, HRS-B090, HRS-I006, HRS-I018, HRS-I030, HRS-I042, HRS-I054, HRS-I066, HRS-I078, HRS-I090, HRS-A006, HRS-A018, HRS-A030, HRS-A042, HRS-A054, HRS-A066, HRS-A078, HRS-A090, HRS-E006, HRS-E018, HRS-E030, HRS-E042, HRS-E054, HRS-E066, HRS-E078, HRS-E090

## 证据入口

`cann/docs/project-understanding/90-cross-module/performance-critical-paths.md:24-43`。来源文档能够支持的事实、由多项证据推导的结论和需要设备实验的行为在题目中分别标记。

## 学习提示

先完成 B 级概念和最小模型，再完成 I 级流程/测试，最后用 A/E 级容量、故障域、设计和演进题检验是否能把局部机制连接成稳定性策略。
