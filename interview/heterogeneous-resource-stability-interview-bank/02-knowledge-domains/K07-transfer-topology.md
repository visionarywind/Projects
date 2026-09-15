# K07：Host-device 传输、NUMA、IOMMU 与 P2P

## 边界

本知识域讨论 Host-device 传输、NUMA、IOMMU 与 P2P，并与其他资源域通过设备、任务、内存、通信、错误和 SLO 交叉连接。特定厂商 API、驱动、固件和硬件行为必须绑定目标版本。

## 核心机制

传输路径受内存位置、pin/map、DMA、NUMA、IOMMU、PCIe 和 P2P 能力共同约束。

## 常见失效条件

把能访问指针当作高效 DMA，把设备可见当作传输已完成。

## 题目索引

HRS-B007, HRS-B019, HRS-B031, HRS-B043, HRS-B055, HRS-B067, HRS-B079, HRS-B091, HRS-I007, HRS-I019, HRS-I031, HRS-I043, HRS-I055, HRS-I067, HRS-I079, HRS-I091, HRS-A007, HRS-A019, HRS-A031, HRS-A043, HRS-A055, HRS-A067, HRS-A079, HRS-A091, HRS-E007, HRS-E019, HRS-E031, HRS-E043, HRS-E055, HRS-E067, HRS-E079, HRS-E091

## 证据入口

`cann/docs/project-understanding/90-cross-module/performance-critical-paths.md:24-30`。来源文档能够支持的事实、由多项证据推导的结论和需要设备实验的行为在题目中分别标记。

## 学习提示

先完成 B 级概念和最小模型，再完成 I 级流程/测试，最后用 A/E 级容量、故障域、设计和演进题检验是否能把局部机制连接成稳定性策略。
