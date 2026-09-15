# 专家评审清单

## 评审前

- [ ] 明确任务、租户、主体、工具、副作用和数据保留。
- [ ] 区分已确认事实、推断、建议和待验证实验。
- [ ] 锁定模型、prompt、tool、policy、index、runtime 版本。

## 运行时

- [ ] 状态转移有 guard、事件、所有权和终态 CAS。
- [ ] worker 有 lease/fencing；恢复不靠模型重算历史副作用。
- [ ] 每个 call 有唯一 ID、幂等键和可查询的结果语义。
- [ ] UNKNOWN、取消、暂停、人工审批过期均有明确处理。

## 安全

- [ ] observation 与 policy 分层，工具网关独立拒绝。
- [ ] capability 绑定资源、参数 hash、主体和有效期。
- [ ] 具备 sandbox、网络出口、秘密脱离 prompt、RLS 和审计。
- [ ] 有检测、撤销、隔离、通知、回滚和残余风险登记。

## 评估与上线

- [ ] 结果、轨迹、安全、grounding、成本、延迟分别有 oracle。
- [ ] judge 经人工/硬 oracle 校准，并报告误报漏报与置信区间。
- [ ] 有回放、对抗、mutation、故障注入、shadow 和 canary。
- [ ] 发布/停止门包含高风险违规、SLO burn、预算和删除传播。

若任一高影响动作缺少独立执行侧授权、可对账语义或回滚/人工路径，应停止自动化，而不是用更强 prompt 掩盖缺口。
