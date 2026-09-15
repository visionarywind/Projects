# K09：错误传播、超时、取消、恢复与故障注入

## 边界

本知识域讨论 错误传播、超时、取消、恢复与故障注入，并与其他资源域通过设备、任务、内存、通信、错误和 SLO 交叉连接。特定厂商 API、驱动、固件和硬件行为必须绑定目标版本。

## 核心机制

错误要跨 Driver/HAL/Runtime/ACL/GE 保留上下文，并以状态机区分失败、成功和未知。

## 常见失效条件

丢弃原始错误码、无限重试或未经幂等性判断执行补偿。

## 题目索引

HRS-B009, HRS-B021, HRS-B033, HRS-B045, HRS-B057, HRS-B069, HRS-B081, HRS-B093, HRS-I009, HRS-I021, HRS-I033, HRS-I045, HRS-I057, HRS-I069, HRS-I081, HRS-I093, HRS-A009, HRS-A021, HRS-A033, HRS-A045, HRS-A057, HRS-A069, HRS-A081, HRS-A093, HRS-E009, HRS-E021, HRS-E033, HRS-E045, HRS-E057, HRS-E069, HRS-E081, HRS-E093

## 证据入口

`cann/docs/project-understanding/90-cross-module/error-boundaries.md:25-48`。来源文档能够支持的事实、由多项证据推导的结论和需要设备实验的行为在题目中分别标记。

## 学习提示

先完成 B 级概念和最小模型，再完成 I 级流程/测试，最后用 A/E 级容量、故障域、设计和演进题检验是否能把局部机制连接成稳定性策略。
