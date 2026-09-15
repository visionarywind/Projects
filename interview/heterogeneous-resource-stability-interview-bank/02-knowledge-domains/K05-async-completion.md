# K05：异步提交、完成语义与内存可见性

## 边界

本知识域讨论 异步提交、完成语义与内存可见性，并与其他资源域通过设备、任务、内存、通信、错误和 SLO 交叉连接。特定厂商 API、驱动、固件和硬件行为必须绑定目标版本。

## 核心机制

提交、设备消费、事件完成、主机可见和资源可复用是不同完成边界。

## 常见失效条件

用 sleep 或 API 返回值代替事件/同步完成证明。

## 题目索引

HRS-B005, HRS-B017, HRS-B029, HRS-B041, HRS-B053, HRS-B065, HRS-B077, HRS-B089, HRS-I005, HRS-I017, HRS-I029, HRS-I041, HRS-I053, HRS-I065, HRS-I077, HRS-I089, HRS-A005, HRS-A017, HRS-A029, HRS-A041, HRS-A053, HRS-A065, HRS-A077, HRS-A089, HRS-E005, HRS-E017, HRS-E029, HRS-E041, HRS-E053, HRS-E065, HRS-E077, HRS-E089

## 证据入口

`cann/docs/project-understanding/90-cross-module/memory-and-resource-lifecycle.md:88-100`。来源文档能够支持的事实、由多项证据推导的结论和需要设备实验的行为在题目中分别标记。

## 学习提示

先完成 B 级概念和最小模型，再完成 I 级流程/测试，最后用 A/E 级容量、故障域、设计和演进题检验是否能把局部机制连接成稳定性策略。
