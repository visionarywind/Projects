# K03：Context、Stream、Event 与 Queue 生命周期

## 边界

本知识域讨论 Context、Stream、Event 与 Queue 生命周期，并与其他资源域通过设备、任务、内存、通信、错误和 SLO 交叉连接。特定厂商 API、驱动、固件和硬件行为必须绑定目标版本。

## 核心机制

执行对象要按依赖关系创建、绑定、完成、teardown 和引用归零；销毁通常是依赖图逆拓扑序。

## 常见失效条件

句柄非空就销毁 Context，或在 pending task 时释放 child object。

## 题目索引

HRS-B003, HRS-B015, HRS-B027, HRS-B039, HRS-B051, HRS-B063, HRS-B075, HRS-B087, HRS-B099, HRS-I003, HRS-I015, HRS-I027, HRS-I039, HRS-I051, HRS-I063, HRS-I075, HRS-I087, HRS-I099, HRS-A003, HRS-A015, HRS-A027, HRS-A039, HRS-A051, HRS-A063, HRS-A075, HRS-A087, HRS-A099, HRS-E003, HRS-E015, HRS-E027, HRS-E039, HRS-E051, HRS-E063, HRS-E075, HRS-E087, HRS-E099

## 证据入口

`cann/docs/project-understanding/90-cross-module/memory-and-resource-lifecycle.md:38-55`。来源文档能够支持的事实、由多项证据推导的结论和需要设备实验的行为在题目中分别标记。

## 学习提示

先完成 B 级概念和最小模型，再完成 I 级流程/测试，最后用 A/E 级容量、故障域、设计和演进题检验是否能把局部机制连接成稳定性策略。
