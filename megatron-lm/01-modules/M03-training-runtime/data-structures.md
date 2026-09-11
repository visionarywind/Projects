# M03 数据结构

| 对象 | 生命周期 | 说明 |
|---|---|---|
| `PretrainConfigContainer` | pretrain 入参 | 汇总配置和 provider |
| model list | setup→train→save | VP 时包含多个 chunk |
| optimizer/scheduler | setup→每步→save | 更新和恢复训练状态 |
| data iterator | dataset build→消费 | 提供 batch 给 forward step |
| `losses_reduced` | 单步产生→日志/聚合 | 不等于完整训练状态 |
| iteration/token counters | loop 持有 | checkpoint 和退出依据 |

不能把 model list 中的 wrapper、底层 module 和 checkpoint state dict 混为同一所有权对象；D01 显式 unwrap DDP 后生成 model shard。