# K11：性能、容量、碎片、成本与降级

## 边界

本知识域讨论 性能、容量、碎片、成本与降级，并与其他资源域通过设备、任务、内存、通信、错误和 SLO 交叉连接。特定厂商 API、驱动、固件和硬件行为必须绑定目标版本。

## 核心机制

稳定性容量要同时建模吞吐、尾延迟、队列、内存、碎片、通信和成本，并设计可验证降级。

## 常见失效条件

凭空承诺百分比收益，或只增加资源而不定位瓶颈。

## 题目索引

HRS-B011, HRS-B023, HRS-B035, HRS-B047, HRS-B059, HRS-B071, HRS-B083, HRS-B095, HRS-I011, HRS-I023, HRS-I035, HRS-I047, HRS-I059, HRS-I071, HRS-I083, HRS-I095, HRS-A011, HRS-A023, HRS-A035, HRS-A047, HRS-A059, HRS-A071, HRS-A083, HRS-A095, HRS-E011, HRS-E023, HRS-E035, HRS-E047, HRS-E059, HRS-E071, HRS-E083, HRS-E095

## 证据入口

`cann/docs/project-understanding/90-cross-module/performance-critical-paths.md:24-43`。来源文档能够支持的事实、由多项证据推导的结论和需要设备实验的行为在题目中分别标记。

## 学习提示

先完成 B 级概念和最小模型，再完成 I 级流程/测试，最后用 A/E 级容量、故障域、设计和演进题检验是否能把局部机制连接成稳定性策略。
