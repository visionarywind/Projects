# K04：设备/主机内存、地址空间与内存池

## 边界

本知识域讨论 设备/主机内存、地址空间与内存池，并与其他资源域通过设备、任务、内存、通信、错误和 SLO 交叉连接。特定厂商 API、驱动、固件和硬件行为必须绑定目标版本。

## 核心机制

虚拟地址、物理 backing、主机 pin/map、设备内存池和缓存段具有不同所有权与可见性。

## 常见失效条件

把总显存当可分配容量，或把 cached、free 和 busy 段混为一谈。

## 题目索引

HRS-B004, HRS-B016, HRS-B028, HRS-B040, HRS-B052, HRS-B064, HRS-B076, HRS-B088, HRS-B100, HRS-I004, HRS-I016, HRS-I028, HRS-I040, HRS-I052, HRS-I064, HRS-I076, HRS-I088, HRS-I100, HRS-A004, HRS-A016, HRS-A028, HRS-A040, HRS-A052, HRS-A064, HRS-A076, HRS-A088, HRS-A100, HRS-E004, HRS-E016, HRS-E028, HRS-E040, HRS-E052, HRS-E064, HRS-E076, HRS-E088, HRS-E100

## 证据入口

`cann/docs/project-understanding/90-cross-module/memory-and-resource-lifecycle.md:57-85`。来源文档能够支持的事实、由多项证据推导的结论和需要设备实验的行为在题目中分别标记。

## 学习提示

先完成 B 级概念和最小模型，再完成 I 级流程/测试，最后用 A/E 级容量、故障域、设计和演进题检验是否能把局部机制连接成稳定性策略。
