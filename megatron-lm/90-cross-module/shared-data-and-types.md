# 共享数据与类型

| 类型/数据 | 使用方 | 说明 |
|---|---|---|
| `TransformerConfig` | M01/M02/M03/M06 | 模型结构和并行/内核配置 |
| `ProcessGroupCollection` | M01/M02/M03/M06 | 显式 group 容器 |
| `ModuleSpec` | M01 | layer/block 规格 |
| batch dict | M04/M03/M01 | tokens、labels、mask、position、packed metadata |
| `PackedSeqParams` | M01/M04/M06 | packed/varlen attention metadata |
| model list | M03/M02/M05 | VP/PP chunk 集合 |
| sharded state dict | M01/M05 | checkpoint mapping |
| `SamplingParams`/request | M06 | 推理请求行为 |

共享类型的风险在于字段来源分散：部分来自 args/YAML，部分来自 dataset，部分来自 runtime PG。修改字段时需追踪生产方和所有消费方。