# M06 源码地图

| 文件 | 重点 |
|---|---|
| `core/inference/engines/abstract_engine.py` | engine 抽象接口 |
| `engines/static_engine.py` | static 兼容层和 fallback |
| `engines/dynamic_engine.py` | 请求调度、生命周期和 dynamic batch |
| `contexts/static_context.py` | 固定容量推理状态 |
| `contexts/dynamic_context.py` | 动态 token/block 状态 |
| `contexts/kv_block_allocator.py` | KV block 分配 |
| `sampling/` | torch/FlashInfer sampling |
| `model_inference_wrappers/gpt/gpt_inference_wrapper.py` | GPT 推理适配 |
| `text_generation_controllers/text_generation_controller.py` | tokenize/detokenize/输出 |
| `tools/run_text_generation_server.py` | 服务启动 |
| `examples/inference/` | offline/advanced inference examples |

推荐代表样例：先读 `examples/inference/offline_inference.py`，再读 dynamic advanced example，最后读 server。