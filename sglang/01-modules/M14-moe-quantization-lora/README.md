# M14 MoE、量化与 LoRA

- 源码证据：`/home/mtuser/workspace/repos/Github/sglang` 当前 checkout。
- 证据状态：公共抽象和静态调用链已确认；量化 kernel、EP 通信和动态 adapter GPU 行为未验证。
- 关联：[M06 模型加载](../M06-model-loading/README.md)、[M07 分布式并行](../M07-分布式并行.md)、[M05 模型执行](../M05-model-execution/README.md)。

## 1. 三条子链

```text
HF/user quant config -> QuantizationConfig -> layer method -> packed weights/scales -> kernel
router/top-k -> logical/physical expert map -> EP dispatch -> expert compute -> combine
LoRA request model/path -> LoRAManager load/slot -> ForwardBatch lora ids -> adapter delta -> unload/evict
```

## 2. 量化契约

`python/sglang/srt/layers/quantization/base_config.py:24-281` 定义 `QuantizeMethodBase`、`LinearMethodBase`、`FusedMoEMethodBase` 和 `QuantizationConfig`。核心方法是 `create_weights`、`get_quant_method`、`apply`、`process_weights_after_loading`；前者决定参数 shape/layout，后者在 M06 checkpoint 完成后进行 repack/workspace 等处理。AWQ/FP8/GPTQ/ModelOpt/GGUF 等实现替换该契约，不应混写成单一算法。

## 3. MoE 与 EPLB

`eplb/expert_location.py:63-343` 的 `ExpertLocationMetadata` 保存 layer、logical/physical/local expert 映射；`expert_location_dispatch.py:25-156` 将 logical top-k 映射为 physical dispatch。`eplb_manager.py:29-352` 在 forward pass 结束后根据利用率决定 rebalance，并通过 `update_expert_location_with_recovery` 更新权重位置。EPLB 改变物理布局而不应改变 logical router 语义；更新期间需要保护并行通信和正在执行的 batch。

## 4. LoRA 生命周期

`lora/lora_manager.py:65-1078` 的 `LoRAManager` 初始化 backend、形状和 memory pool；`load_lora_adapter`/`load_lora_adapter_from_tensors` 加载 adapter，`unload_lora_adapter` 释放或回收 slot，`validate_lora_batch` 检查同一 forward batch 的 adapter 组合，`prepare_lora_batch` 生成执行元数据。`lora/backend/lora_registry.py:11-61` 通过名称选择 Triton、Torch、Ascend、FlashInfer 等 backend。

HTTP 的 model 参数可携带 adapter 名称，M02 基类规定 model 中 adapter 优先于显式 `lora_path`；因此 LoRA 选择在请求进入 tokenizer 前已影响状态，不能只在 kernel 层处理。

## 5. 代表调用链、例子和断点

```text
ModelConfig/HF quant metadata
  -> get_quant_method(layer)
  -> create_weights
  -> M06 AutoWeightsLoader + weight_loader
  -> process_weights_after_loading
  -> M05 layer apply / fused kernel

GenerateReqInput.lora_path
  -> TokenizerManager resolve lora
  -> LoRAManager.load/fetch_new_loras
  -> prepare_lora_batch(ForwardBatch)
  -> backend apply adapter delta
  -> unload/evict
```

例：一个 batch 同时包含 base 与 adapter-A 请求，manager 为 adapter-A 分配 slot，ForwardBatch 保存每 token 的 adapter id，backend 只对对应 token 加 delta；下一批 reset 不能泄漏 A 的 metadata。

断点：`QuantizationConfig.get_quant_method`、`AutoWeightsLoader`、`EPLBManager.on_forward_pass_end`、`LoRAManager.load_lora_adapter`、`validate_lora_batch` 和 `prepare_lora_batch`。

## 6. 测试与未验证

`test/manual/test_srt_engine_with_quant_args.py`、`test/manual/test_modelopt.py`、`test/manual/test_moe_quant_once.py`、`test/manual/test_triton_moe_wna16.py`、`test/manual/test_weight_loader_v2_equiv.py`、`test/registered` 下 lora/model loader 测试以及 `benchmark/lora/` 是主要入口。本次未执行 GPU kernel、EP/EPLB rebalance、动态 load/unload 或多 adapter 性能测试。

## 7. 修改影响

量化参数 shape 改动会跨 M06 loader、M07 TP/EP、M05 forward 和 M16 kernel；expert map 改动会跨 dispatch、collective、权重更新和 metrics；LoRA slot/adapter 改动会跨 M02 model listing、M03 request state、M05 graph eligibility、M09 CUDA Graph 和 M10 sampling metadata。**已确认**公共契约；**未验证**具体设备的数值和性能。
