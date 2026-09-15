# 系统设计案例

## 案例 A：可暂停的研究 Agent

设计 run、step、tool call、事件日志、checkpoint、lease/fencing、人工审批和恢复协议。要求说明工具成功但响应丢失时如何进入 UNKNOWN，以及取消和恢复的竞态。验收：终态 CAS、幂等键、租约失效 worker 不能写、事件可重放。

## 案例 B：多租户客服 Agent

设计 API、队列、worker、模型路由、检索、记忆、工具网关、RLS、配额和审计。要求回答租户隔离、敏感信息删除、共享缓存和高风险退款审批。验收：权限不由模型决定；cache/index/log 都有租户边界；删除请求可追踪传播。

## 案例 C：并行资料研究平台

设计 supervisor、受限 worker、fan-out/fan-in、证据契约、冲突合并、早停和预算。要求区分多数投票与来源核验，说明一个 worker 被投毒时的隔离和回滚。验收：每个 claim 关联来源和时间，子任务权限不超过父任务，尾延迟和成本有上界策略。

## 案例 D：单 Agent 到平台化演进

设计不可变 bundle、版本矩阵、shadow、canary、drain、旧 run 兼容、schema/index migration 和停止门槛。验收：模型、prompt、tool、policy、index、runtime 全部可追踪；高风险违规、SLO burn、删除传播失败时停止扩大。

## 评审清单

1. 组件边界和信任边界是否独立？
2. 状态、事件、所有权和终态是否明确？
3. 外部副作用是否具备幂等、查询或补偿？
4. 失败、降级、恢复、回滚和人工接管是否可执行？
5. 是否有容量公式、预算、P95/P99 和成本归因？
6. 是否给出 oracle、故障注入、审计字段和停止标准？
