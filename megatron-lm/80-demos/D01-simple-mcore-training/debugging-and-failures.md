# D01 调试与失败模式

| 失败点 | 可能原因 | 排查 |
|---|---|---|
| `RANK/WORLD_SIZE` 缺失 | 未用 torchrun 启动 | 检查 launcher 环境变量 |
| CUDA device 设置失败 | 无 GPU 或 local rank 错误 | 检查 `CUDA_VISIBLE_DEVICES` 与 nproc |
| NCCL init 失败 | 驱动/NCCL/网络问题 | 先运行最小 torch distributed smoke test |
| helper 编译失败 | 编译器或依赖缺失 | 查看 dataset helper build 输出 |
| schedule assertion | model/iterator 数量不匹配 | D01 应为单 model、单 iterator |
| checkpoint load 失败 | 文件缺失或 sharded metadata 不匹配 | 检查 `ckpt` 目录和 rank 数 |

当前文档未执行命令，因此不提供实测错误输出。