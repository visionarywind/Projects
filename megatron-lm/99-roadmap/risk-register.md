# 风险登记

| 风险 | 影响 | 缓解 |
|---|---|---|
| 静态分析替代动态验证 | 错误归因 | 明确标注未验证，补 smoke test |
| 并行配置组合爆炸 | 漏掉分支 | 以配置影响图逐项覆盖 |
| checkpoint 兼容性复杂 | 恢复失败 | round-trip 测试和 args 校验 |
| optional backend 差异 | TE/FlashInfer/ModelOpt 路径不同 | 按 backend 建立测试矩阵 |
| 隐式全局状态 | 调用顺序 bug | 记录初始化/销毁边界 |
| 推理 dynamic/static 兼容层 | 实际 engine 与名称不一致 | 明确记录 fallback 条件 |

当前最大风险是文档覆盖速度高于实测覆盖；后续应优先补 D01 和 focused unit tests。