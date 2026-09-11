# 错误边界

| 边界 | 典型错误 | 传播 |
|---|---|---|
| distributed init | CUDA/NCCL/rank env | 初始化阶段直接失败 |
| model builder | spec/import/非法组合 | setup_model 阶段失败 |
| dataset build | 文件、tokenizer、index helper | build iterator 阶段失败 |
| schedule | shape、PP stage、unsupported overlap | train_step 失败 |
| optimizer | overflow、state mismatch | step 返回 skip 或抛错 |
| checkpoint | 路径、metadata、配置不兼容 | save/load 失败 |
| inference | capacity、KV handoff、server rank | request 或 server loop 失败 |

当前文档没有实测异常输出；只记录源码中显式 assert、RuntimeError 和调用边界。