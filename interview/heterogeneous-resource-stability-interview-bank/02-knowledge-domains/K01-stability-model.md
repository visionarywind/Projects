# K01：稳定性目标、边界与故障模型

## 边界

本知识域讨论 稳定性目标、边界与故障模型，并与其他资源域通过设备、任务、内存、通信、错误和 SLO 交叉连接。特定厂商 API、驱动、固件和硬件行为必须绑定目标版本。

## 核心机制

正确性、可用性、资源安全、可恢复性和可诊断性必须同时纳入稳定性契约。

## 常见失效条件

把 API 返回成功等同于任务完成，或把超时直接判定为未执行。

## 题目索引

HRS-B001, HRS-B013, HRS-B025, HRS-B037, HRS-B049, HRS-B061, HRS-B073, HRS-B085, HRS-B097, HRS-I001, HRS-I013, HRS-I025, HRS-I037, HRS-I049, HRS-I061, HRS-I073, HRS-I085, HRS-I097, HRS-A001, HRS-A013, HRS-A025, HRS-A037, HRS-A049, HRS-A061, HRS-A073, HRS-A085, HRS-A097, HRS-E001, HRS-E013, HRS-E025, HRS-E037, HRS-E049, HRS-E061, HRS-E073, HRS-E085, HRS-E097

## 证据入口

`cann/docs/project-understanding/00-overview/global-error-model.md:29-42`。来源文档能够支持的事实、由多项证据推导的结论和需要设备实验的行为在题目中分别标记。

## 学习提示

先完成 B 级概念和最小模型，再完成 I 级流程/测试，最后用 A/E 级容量、故障域、设计和演进题检验是否能把局部机制连接成稳定性策略。
