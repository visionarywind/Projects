# 调试指南

| 问题 | 首查文档 | 首查源码 |
|---|---|---|
| 分布式启动失败 | M02 execution | `initialize.py`, `parallel_state.py` |
| loss 异常 | M04/M01 | `get_batch`, `GPTModel.forward` |
| pipeline hang | M02 | schedules 和 P2P |
| optimizer skip | M05 | optimizer step/loss scaling |
| checkpoint 不兼容 | M05 | `check_checkpoint_args`, sharded state dict |
| inference 请求卡住 | M06 | scheduler/engine/server rank gate |

调试原则：先确认 rank/env/config，再确认 tensor shape，再进入 backend/kernel。文档中没有实测日志时，不应假设具体 NCCL 错误原因。