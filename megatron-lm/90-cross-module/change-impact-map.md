# 修改影响图

| 修改点 | 需要检查 |
|---|---|
| 新增 layer spec | M01 builder、TransformerBlock、checkpoint、tests |
| 改并行 group | M02 rank generator、accessor、M05 optimizer、M03 schedule |
| 改 batch 字段 | M04 dataset、pretrain_gpt.get_batch、forward_step、M01 forward |
| 改 optimizer param group | M05 factory、checkpoint、scheduler、日志 |
| 改 checkpoint metadata | M01 sharded state、M05 load/save、兼容性检查 |
| 改 inference context | M06 engine、wrapper、GPTModel inference path |
| 改训练循环退出 | M03 checkpoint、async finalize、profiling、distributed cleanup |

原则：先追生产方，再追所有消费方；能静态确认的写入文档，运行时行为另列验证项。