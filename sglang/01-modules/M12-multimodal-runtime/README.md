# M12 多模态 Runtime

- 文档目的：解释 01-modules/M12-multimodal-runtime/README.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/sglang HEAD 78be4b50af（2026-09-15 只读确认）。
- 证据状态：处理器、缓存、特征传输和 rank 分片的静态路径已确认；真实图像/音频/视频与 GPU encoder 未验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M12-multimodal-runtime/README.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


- 源码证据：`/home/mtuser/workspace/repos/Github/sglang` 当前 checkout。
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

## 文档元数据（规范补充）

- 文档目的：说明 `01-modules/M12-multimodal-runtime/README.md` 的源码分析范围、结论和维护入口。
- 适用范围：当前项目对应模块/入口的静态源码与测试分析。
- 对应源码版本：以本项目 `00-overview/analysis-state.md` 或同页版本字段为准。
- 证据状态：静态源码证据；未执行的构建、测试、GPU、网络或多进程行为保持“未验证”。
- 最后更新：2026-09-15
- 前置阅读：本项目根 README 与 `00-overview/analysis-state.md`。
- 后续阅读：本模块/示例的实现、测试和风险页面。

## 深度审计

| 分析对象 | 入口落地 | 正常路径 | 分支 | 异常 | 清理 | 数据生命周期 | 执行上下文 | 行级证据 | Demo 映射 | 状态/缺口 |
|---|---|---|---|---|---|---|---|---|---|---|
| `01-modules/M12-multimodal-runtime/README.md` | 已完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 已映射或不适用 | 部分完成：动态行为、边界或专用变体仍需验证 |

## 相关文档
- [项目入口](../../README.md)
- [分析状态](../../00-overview/analysis-state.md)
- [源码证据索引](../../00-overview/evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](../../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
