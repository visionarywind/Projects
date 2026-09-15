# K02：设备发现、拓扑、能力协商与兼容

## 边界

本知识域讨论 设备发现、拓扑、能力协商与兼容，并与其他资源域通过设备、任务、内存、通信、错误和 SLO 交叉连接。特定厂商 API、驱动、固件和硬件行为必须绑定目标版本。

## 核心机制

设备身份、静态能力、版本配套、拓扑和动态资源共同决定任务准入。

## 常见失效条件

只检查 device_id 或总显存，不检查 dtype、互联、权限、版本和快照时效。

## 题目索引

HRS-B002, HRS-B014, HRS-B026, HRS-B038, HRS-B050, HRS-B062, HRS-B074, HRS-B086, HRS-B098, HRS-I002, HRS-I014, HRS-I026, HRS-I038, HRS-I050, HRS-I062, HRS-I074, HRS-I086, HRS-I098, HRS-A002, HRS-A014, HRS-A026, HRS-A038, HRS-A050, HRS-A062, HRS-A074, HRS-A086, HRS-A098, HRS-E002, HRS-E014, HRS-E026, HRS-E038, HRS-E050, HRS-E062, HRS-E074, HRS-E086, HRS-E098

## 证据入口

`cann/docs/project-understanding/00-overview/architecture.md:50-53`。来源文档能够支持的事实、由多项证据推导的结论和需要设备实验的行为在题目中分别标记。

## 学习提示

先完成 B 级概念和最小模型，再完成 I 级流程/测试，最后用 A/E 级容量、故障域、设计和演进题检验是否能把局部机制连接成稳定性策略。
