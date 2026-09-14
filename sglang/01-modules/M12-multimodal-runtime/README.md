# M12 多模态 Runtime

- 源码证据：`/home/mtuser/workspace/repos/Github/sglang` 当前 checkout。
- 证据状态：处理器、缓存、特征传输和 rank 分片的静态路径已确认；真实图像/音频/视频与 GPU encoder 未验证。
- 关联：[M02 HTTP/API](../M02-http-api-protocol/README.md)、[M05 模型执行](../M05-model-execution/README.md)、[M09 Attention/CUDA Graph](../M09-attention-cuda-graph/README.md)。

## 1. 边界

M12 将请求中的 media 变成 tokenizer/model 可消费的文本 token、placeholder、特征和位置元数据。它不负责 HTTP schema 本身，也不负责 scheduler admission；M02 负责协议转换，M03 负责请求状态，M05 在 forward 中消费最终输入。

```text
OpenAI/native media
  -> BaseMultimodalProcessor.load_mm_data
  -> process_and_combine_mm_data
  -> MultimodalInputs / mm_items
  -> placeholder expansion + M-RoPE positions
  -> encoder features
  -> CUDA IPC/CPU transport
  -> ForwardBatch/model forward
```

## 2. 关键对象

`python/sglang/srt/multimodal/processors/base_processor.py:70-237` 定义 `BaseMultiModalProcessorOutput`、`MultimodalSpecialTokens` 和 `BaseMultimodalProcessor`。处理器保存 HF config、server args、tokenizer、processor、preprocess cache 与 CPU executor；`build_input_ids`、`get_mm_data`、`get_validated_mm_data` 是文本和媒体的边界。

媒体 artifact 契约位于 `multimodal/media_artifacts/base.py:49-75,94-259`：artifact 必须能报告 `has_feature`、`cache_value`、`cache_size_items`；`MediaArtifactInput` 描述原始 media、modality 与预处理参数。cache identity 位于 `multimodal/cache/identity.py:65-380`，避免把相同媒体和不同 processor 配置误认为同一个结果。

## 3. 处理调用链

```text
TokenizerManager._tokenize_one_request
  -> processor.load_mm_data()                         [base_processor.py:1214-1515]
  -> processor.process_and_combine_mm_data()          [base_processor.py:1773-2007]
  -> _expand_input_ids / collect_mm_items
  -> MultimodalInputs / processor output
  -> ModelRunner/ForwardBatch 消费 mm features
```

异步预处理可通过 `MultimodalProcessorExecutor.run`（`processors/executor.py:15-54`）在线程/进程 executor 中执行；`shutdown` 必须在 server 退出时回收 worker。`encoder_preprocessing.py:16-138` 的 `EncoderPreprocessOutput` 提供按 rank 选择 item 和 `materialize_for_rank`，使 DP encoder 不必在每个 rank 重复处理全部媒体。

视觉路径可调用 `mm_utils.py:397-555` 的 image processing、DP sharded vision 和 M-RoPE helper；具体模型 processor 再实现 `process_mm_data`/`compute_mrope_positions`。

## 4. 缓存、传输和生命周期

初始化阶段解析模型专用 processor、特殊 token、cache fingerprint 和 executor。稳态阶段先加载/校验原始 media，再预处理、缓存、生成 placeholder 对齐的 input ids，最后将 feature 留在 device、移至 CPU 或包成 CUDA IPC handle，取决于配置。失败时不能留下未完成的 cache miss、临时文件或 executor task；`BaseMultimodalProcessor.clear_preprocess_cache`、`shutdown` 是清理边界。

`_wrap_tensor_for_cuda_ipc`（`base_processor.py:1678-1707`）说明跨进程 feature transport 不是普通 Python pickle；接收端必须在 feature 生命周期结束后释放句柄。特征数量与 placeholder 数量必须一致，否则 M05 的 input ids、position ids 和 embedding splice 会错位。

## 5. 小例子和断点

例：一条 chat 消息含一张 image：协议层保留 image item；processor 读取并缩放图片，生成视觉 feature；文本中 image placeholder 被扩展为模型需要的 token 数；M-RoPE/position metadata 随输入传给 model runner。

断点：`BaseMultimodalProcessor.load_mm_data`、`process_and_combine_mm_data`、`_expand_input_ids`、`materialize_multimodal_features` 和 `EncoderPreprocessOutput.materialize_for_rank`。

## 6. 变体和测试

处理器目录包含 LLaVA、InternVL、Gemma、Kimi、DeepSeek OCR、音频和视频实现；应以 base contract 加一个模型 processor 作为主链，而非逐文件抄录。`test/manual/test_vlm_accuracy.py`、`test/manual/test_whisper_cuda_graph.py`、`test/registered` 下 multimodal 测试和 `benchmark/llava_bench` 是定位入口。本次未运行真实媒体、模型下载、CUDA Graph、DP encoder 或跨进程 CUDA IPC。

## 7. 修改影响

修改 placeholder/token count 会影响 M03 token budget、M04 admission、M08 KV positions、M09 graph eligibility 和 M05 embedding splice；修改 cache identity 会影响内存占用与复用正确性；修改 rank sharding 会影响 DP/TP collectives。**已确认**接口边界；**未验证**具体模型的真实特征数值和设备传输。
