# K08：跨卡/跨节点通信、集合通信与 RDMA

## 边界

本知识域讨论 跨卡/跨节点通信、集合通信与 RDMA，并与其他资源域通过设备、任务、内存、通信、错误和 SLO 交叉连接。特定厂商 API、驱动、固件和硬件行为必须绑定目标版本。

## 核心机制

集体通信要求参与者、顺序、拓扑、缓冲区和完成协议一致；局部失败可能导致全局阻塞。

## 常见失效条件

只在单卡测试通信，或对 collective 超时无限重试。

## 题目索引

HRS-B008, HRS-B020, HRS-B032, HRS-B044, HRS-B056, HRS-B068, HRS-B080, HRS-B092, HRS-I008, HRS-I020, HRS-I032, HRS-I044, HRS-I056, HRS-I068, HRS-I080, HRS-I092, HRS-A008, HRS-A020, HRS-A032, HRS-A044, HRS-A056, HRS-A068, HRS-A080, HRS-A092, HRS-E008, HRS-E020, HRS-E032, HRS-E044, HRS-E056, HRS-E068, HRS-E080, HRS-E092

## 证据入口

`cann/docs/project-understanding/90-cross-module/error-boundaries.md:40-48`。来源文档能够支持的事实、由多项证据推导的结论和需要设备实验的行为在题目中分别标记。

## 学习提示

先完成 B 级概念和最小模型，再完成 I 级流程/测试，最后用 A/E 级容量、故障域、设计和演进题检验是否能把局部机制连接成稳定性策略。
