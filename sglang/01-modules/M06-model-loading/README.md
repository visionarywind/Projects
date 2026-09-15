# M06 模型加载与权重路由

- 文档目的：说明 ModelConfig/LoadConfig 如何选择模型架构、loader 和权重变换，并把 checkpoint 变成各并行 rank 可消费的参数。
- 源码证据：`/home/mtuser/workspace/repos/Github/sglang` 当前 checkout。
- 证据状态：初始化、权重路由和并行 barrier 已静态确认；真实 checkpoint、GPU 权重加载未验证。
- 关联专题：[模型加载与并行初始化](../../03-model-loading/01-模型加载与并行初始化.md)、[M05 模型执行](../M05-model-execution/README.md)、[M07 分布式并行](../M07-分布式并行.md)。

## 1. 边界

M06 的输入是 resolved `ServerArgs`、模型配置和 checkpoint 来源；输出是已经实例化、加载并 post-process 的模型模块，交给 M05 `ModelRunner`。它决定“参数如何到 module”，但不决定每个请求的 batch，也不拥有 KV request row。

```text
ServerArgs
  -> ModelConfig / LoadConfig
  -> architecture resolution
  -> get_model_loader
  -> instantiate model on target device/meta
  -> iterate checkpoint weights
  -> PP filter / remap / stacked dispatch / weight_loader
  -> post-load + quant setup
  -> barrier / LoadedModel
```

## 2. 配置对象

`python/sglang/srt/configs/model_config.py:366-629` 的 `ModelConfig` 从模型路径、HF config、dtype、context length、embedding/generation/multimodal 和量化信息派生运行时模型描述。构造时会读取或拉取配置，深拷贝缓存的 HF config，并根据 architecture、`language_model_only`、vision/audio 子配置和 `enable_multimodal` 推导 generation、embedding、multimodal、encoder-decoder 等标志。[`python/sglang/srt/configs/model_config.py:366-429`][`python/sglang/srt/configs/model_config.py:437-475`][`python/sglang/srt/configs/model_config.py:556-629`]

需要区分三层：

1. raw server arguments：命令行或 API 输入；
2. `ModelConfig`：HF config 与 SGLang 模型能力的解析结果；
3. `LoadConfig`：load format、device、download/cache、remote/presharded 等加载策略。

`ModelConfig.from_server_args` 的完整多模态/HF 配置来源仍需逐字段复核，不应把所有 architecture helper 误写为同一模型路径。

## 3. loader 抽象和分派

`python/sglang/srt/model_loader/loader.py:274-336` 定义架构实例化、post-load hook 和 `BaseModelLoader` 的 `download_model/load_model` 契约。`_initialize_model` 先通过 `get_model_architecture` 得到模型类，再把 HF config、quant config 和 draft index 传给构造函数；`_post_load_weights` 为绕过 `model.load_weights()` 的 loader 显式调用模型 post-load fixup。[`python/sglang/srt/model_loader/loader.py:274-336`]

代表 loader（实现范围以当前 checkout 为准）：

| Loader | 当前实现 | 触发条件/用途 |
|---|---|---|
| `DummyModelLoader` (`:1596-1651`) | 基础 loader | `LoadFormat.DUMMY` |
| `DefaultModelLoader` (`:361-1060`) | 常规 checkpoint 遍历与参数加载 | 最终 fallback |
| `LayeredModelLoader` (`:1061-1122`) | 分层/分段加载 | `LoadFormat.LAYERED` |
| `QuantizedRLModelLoader` (`:1123-1595`) | RL/原生 FP8 加载 | `LoadFormat.FLASH_RL` |
| `ShardedStateLoader` (`:1652-1837`) | sharded state 来源 | `LoadFormat.SHARDED_STATE` |
| `PreshardedModelLoader` (`:1838-2671`) | 已按并行布局切分的权重 | `LoadFormat.PRESHARDED` |
| `BitsAndBytesModelLoader` (`:2672-3145`) | bitsandbytes 权重 | `LoadFormat.BITSANDBYTES` |
| `GGUFModelLoader` (`:3146-3259`) | GGUF 格式 | `LoadFormat.GGUF` |
| `RemoteInstanceModelLoader` (`:3260-3486`) | 远程实例复用/传输 | `LoadFormat.REMOTE_INSTANCE` |
| `RemoteModelLoader` (`:3487-3667`) | 远程权重来源 | `LoadFormat.REMOTE` |
| `IncModelLoader` (`:3668-3771`) | AutoRound/Intel 路径 | `auto-round-int8` |
| `ModelOptModelLoader` (`:3772-4040`) | ModelOpt 校准、导出或预量化 | ModelOpt workflow/flags |
| `RunaiModelStreamerLoader` (`:4041-4281`) | Run:ai streamer | `LoadFormat.RUNAI_STREAMER` |

`get_model_loader` 的当前分派顺序是源码中的显式优先级，而不是文件后缀猜测：先处理 `DUMMY` 和可调用的自定义 load format；随后处理 `auto-round-int8`；再根据 ModelOpt 配置决定是否进入 `ModelOptModelLoader`（`modelopt_fp4` 的 online 非预量化路径以及 Run:ai/remote-instance 会避开它）；之后依次检查 sharded state、presharded、bitsandbytes、GGUF、expert pack、layered、Flash RL、remote、remote-instance、private、Run:ai streamer 和 IPC cache，均未命中时返回 `DefaultModelLoader`。[`python/sglang/srt/model_loader/loader.py:4282-4417`]。因此同一个量化名字在不同 `load_format`、是否已经量化、是否存在 ModelOpt workflow 配置下可能进入不同 loader；精确判断必须同时查看 `LoadConfig` 和 `ModelConfig`。

## 3.1 `get_model_loader` 的实际优先级

当前 `get_model_loader` 不是按 checkpoint 后缀简单选择，而是按以下顺序短路：

1. `DUMMY` 直接使用 `DummyModelLoader`；
2. `load_format` 本身是 loader class 时实例化该 class；
3. `auto-round-int8` 选择 `IncModelLoader`；
4. ModelOpt 配置/量化满足条件时选择 `ModelOptModelLoader`，但 `modelopt_fp4` 在线量化且未请求 calibration/checkpoint/export 时保留 `DefaultModelLoader`；
5. 再按 `SHARDED_STATE`、`PRESHARDED`、`BITSANDBYTES`、`GGUF`、`EXPERT_PACK`、`LAYERED`、`FLASH_RL`、`REMOTE`、`REMOTE_INSTANCE`、`PRIVATE`、`RUNAI_STREAMER` 和 `IPC_CACHE` 分派；
6. 其余情况回退到 `DefaultModelLoader`。

因此同一个量化名称可能因“已量化/在线量化”“是否请求 ModelOpt workflow”以及 load format 而进入不同 loader；排查时应同时打印 `LoadConfig.load_format`、`ModelConfig.quantization` 和 ModelOpt workflow 字段。[`python/sglang/srt/model_loader/loader.py:4282-4416`](../../../source/sglang/python/sglang/srt/model_loader/loader.py)

## 3.2 `ModelConfig.from_server_args` 的配置投影

`from_server_args` 先通过 `resolving_view(server_args)` 读取已经解析的配置，而不是重新读取 raw argparse。普通模型与 draft 模型分别选择 quantization 和 decrypted override 文件；随后将 model path/revision、context length、dtype、multimodal、embedding、encoder-only、language-only、draft、speculative algorithm 等字段投影到 `ModelConfig`。[`python/sglang/srt/configs/model_config.py:692-739`](../../../source/sglang/python/sglang/srt/configs/model_config.py)

draft 配置随后可能按架构和 speculative algorithm 改写 architecture，例如 DeepSeek、Glm、Dots3 等 NextN/DSpark 变体；所以“同一个 model path”并不保证 target 和 draft 实例化为同一个 model class。[`python/sglang/srt/configs/model_config.py:741-809`](../../../source/sglang/python/sglang/srt/configs/model_config.py)

排查配置不一致时应在 `from_server_args`、`ModelConfig.__init__` 和 `_config_draft_model` 分别记录：resolved view、HF architecture、quantization、`is_draft_model`、override 文件和最终 architecture。

## 4. 权重路由

`auto_loader.py:62-226` 把通用路由拆成三个步骤：`filter_pp_weights` 丢弃不属于当前 PP layer range 的 checkpoint 项；`WeightsMapper`/remap registry 规范架构特定名称；`StackedParamsDispatch` 把 q/k/v 或 gate/up 映射到 fused 参数和 shard id。最后由 `AutoWeightsLoader` walker 定位 module/parameter，并调用参数自己的 `weight_loader`。[`python/sglang/srt/model_loader/auto_loader.py:62-109`][`python/sglang/srt/model_loader/auto_loader.py:156-226`]

`AutoWeightsLoader` 位于 `models/utils.py:125-281`。它先按权重名的第一个组件分组，再递归进入 child module；若子模块提供自己的 `load_weights` 就把该组委托给子模块，否则定位 parameter/buffer，调用其 `weight_loader`，并对未知或嵌套名称按 skip/ignore 规则报错。[`python/sglang/srt/models/utils.py:125-281`](../../../source/sglang/python/sglang/srt/models/utils.py)

```text
checkpoint name
  -> PP filter
  -> model-specific remap
  -> module parameter lookup
  -> parameter.weight_loader
  -> TP/PP-local tensor
  -> process_weights_after_loading
```

## 5. 代表启动链

```text
ModelRunner.initialize_model()
  -> build_load_config()                                      [model_runner_components/load_model_utils.py:195-236]
  -> load_model_with_memory_saver()                           [load_model_utils.py:256-343]
  -> get_model_loader()                                       [model_loader/loader.py:4282-4417]
  -> _initialize_model()                                      [loader.py:274-301]
  -> loader.load_model()
  -> filter_pp_weights() / AutoWeightsLoader                  [auto_loader.py:156-226; models/utils.py:125-281]
  -> parameter.weight_loader
  -> _post_load_weights()
  -> dist_barrier_after_load()                                [load_model_utils.py:346-368]
  -> ModelRunner attention/KV/graph initialization
```

`load_model_with_memory_saver` 在权重区间内选择 loader，并根据 zero-copy IPC、CPU backup、startup overlap 和 draft worker 状态调整所有权；函数退出前还反向 vLLM monkey patch，返回包含 loader、model 和可选 startup/remote transfer 信息的 `LoadedModel`。[`python/sglang/srt/model_executor/model_runner_components/load_model_utils.py:256-343`]

`dist_barrier_after_load` 使用 TP CPU group 的 `monitored_barrier` 等待所有 rank；若某 rank OOM 或异常而其他 rank 已完成，超时会转换为说明性 `ValueError`，避免一个 rank 继续向 ready 阶段推进。[`python/sglang/srt/model_executor/model_runner_components/load_model_utils.py:346-368`]

## 6. 模型专用权重入口：Llama/Qwen2

Llama 和 Qwen2 都由 `load_weights` 根据 `SGLANG_ENABLE_WEIGHT_LOADER_V2` 在 legacy 路径与 `AutoWeightsLoader` 路径之间选择。legacy 路径在模型类中显式处理 PP layer range、跳过 rotary/projector 等 checkpoint 项、QKV/MLP stacked mapping 和参数 `weight_loader`；v2 路径把这些职责拆给 `filter_pp_weights`、walker 和 remap registry。[`python/sglang/srt/models/llama.py:662-740`][`python/sglang/srt/models/qwen2.py:615-691`]

当前 v2 的精确行为可由源码直接确认：Llama 的 `_load_weights_v2` 使用 FP8 suffix remap，并忽略 projector、vision tower、额外 bias 和旧 `.kv_scale`；Qwen2 使用相同的 PP/filter/skip 结构，并在 tied embedding 时把 `embed_tokens.weight` 复制到 `lm_head.weight`。[`python/sglang/srt/models/llama.py:742-774`][`python/sglang/srt/models/qwen2.py:692-729`]

```text
checkpoint iterator
  -> model.start_layer/end_layer filter
  -> skip tied lm_head / projector / vision / obsolete suffix
  -> AutoWeightsLoader walks module tree
  -> local module load_weights or parameter.weight_loader
  -> tied embedding copy (when enabled)
```

`StackedParamsDispatch` 只负责把 checkpoint source name 映射到 fused parameter 和 shard id；真正的 TP 切片、量化 packed layout 和 dtype 转换仍由目标 parameter 的 `weight_loader` 完成。[`python/sglang/srt/model_loader/auto_loader.py:62-109`]

## 6.1 专用 loader 的边界和失败语义

`get_model_loader` 选中的 loader 不只是“换一种文件后缀读取器”，而是会改变 checkpoint 的 ownership、切片位置、后处理时机和失败条件。以下是当前 checkout 中几条可直接从实现确认的专用路径。

### ShardedState：每个 TP rank 只读取自己的 state

`ShardedStateLoader.load_model` 先在目标 device/meta 上实例化模型，并对每个带 `quant_method` 的 module 执行 `process_weights_after_loading`；随后根据当前 `get_parallel().tp_rank` 拼出 `model-rank-{rank}-part-*` 文件模式，只打开本 rank 的 safetensors 分片。[`python/sglang/srt/model_loader/loader.py:1738-1773`](../../../source/sglang/python/sglang/srt/model_loader/loader.py)

加载时将 checkpoint tensor copy 到模型 `state_dict` 的对应参数；若 LoRA padding 使模型参数更宽，则对目标参数逐维 `narrow` 后再 copy，并在所有文件处理结束后以剩余 key 检查缺失权重。`_filter_subtensors` 会按 storage pointer 去重共享 storage/subtensor；非 contiguous tensor 不使用 flat-view 推断范围，而是保留到写出阶段再 contiguous 化。[`python/sglang/srt/model_loader/loader.py:1677-1720,1774-1799`](../../../source/sglang/python/sglang/srt/model_loader/loader.py)

因此该格式的关键不变量是“文件命名的 rank 必须与当前 TP rank 对齐，且每个 rank 的 state 覆盖完整”。未找到 rank 分片时不会悄悄回退到普通未分片 checkpoint，而是明确报错；当前实现也明确标注未支持 un-sharded checkpoint。

### Presharded：带配置指纹的 post-process cache

`PreshardedModelLoader` 并非直接假设目录可用。它根据 TP/DP/EP/PP、MoE dense/DP、LM head、dtype、quantization、EPLB 和初始 expert location 收集 `shard_config`，再将排序后的配置哈希成子目录名；target 和 draft 还可使用不同的 root override。[`python/sglang/srt/model_loader/loader.py:1838-1918,1920-1947`](../../../source/sglang/python/sglang/srt/model_loader/loader.py)

`load_model` 只有同时存在 `READY` 文件且 `checksum.json` 中保存的 shard config 完全匹配时才走 `_load_from_presharded`；否则进入首次普通加载、post-load 处理和分 rank dump。加载计划包含版本号和 `rank_to_reads`，当前 rank 按计划读取文件、恢复 tensor，再可选验证 hash；版本不匹配或参数缺失会失败，而不是把旧 cache 当作兼容格式继续用。[`python/sglang/srt/model_loader/loader.py:1872-1898,2589-2649`](../../../source/sglang/python/sglang/srt/model_loader/loader.py)

这条路径要求 dump 目录在各 rank/节点之间共享。`READY` 是完成标记，配置指纹是兼容性标记，二者都不能被简化成“目录存在就命中”；并行拓扑、量化或 expert placement 改变都可能使同一模型路径产生新的 presharded 子目录。

### BitsAndBytes：pre-quant 与在线 quant 的 TP 语义不同

`BitsAndBytesModelLoader` 首先根据 QLoRA adapter 的 `adapter_config.json` 读取 target modules；没有 adapter 配置时使用默认的 attention/MLP module 列表。权重可来自 safetensors、bin 或 pt；pre-quantized 路径先收集 4-bit quant state 或 8-bit `.scb`，再把 quant state 绑定到目标参数。[`python/sglang/srt/model_loader/loader.py:2695-2739,2774-2793,2871-2937`](../../../source/sglang/python/sglang/srt/model_loader/loader.py)

未量化权重则在 iterator 中把目标 `.weight` 改为 `.qweight`，依据 column-parallel 或 row-parallel 方向按 TP rank 切片，再在 GPU 上执行 NF4 `quantize_4bit`。相反，源码明确拒绝 pre-quantized BitsAndBytes 与 TP>1 的组合，因为 pre-quant 的 quant state 不能安全用于拆分后的 weight tensor；该场景应改用 PP 或其他受支持布局。[`python/sglang/srt/model_loader/loader.py:2939-2990,3022-3048`](../../../source/sglang/python/sglang/srt/model_loader/loader.py)

完成 `model.load_weights` 后，loader 将不同模型的 stacked parameter 名称归一化，写入 `bnb_quant_state`、`bnb_shard_offsets`，8-bit 时另设 `matmul_state`。所以 BitsAndBytes 的正确性不仅取决于 qweight 数值，还取决于 stacked mapping、pack factor 和 offsets 是否与 kernel 预期一致。[`python/sglang/srt/model_loader/loader.py:3050-3121`](../../../source/sglang/python/sglang/srt/model_loader/loader.py)

### GGUF：外部命名映射后进入模型自己的 weight loader

`GGUFModelLoader` 要求 `model_path` 是单个文件，并在需要时懒加载 `gguf` 包；它优先使用 SGLang 注册的 model-specific name-map builder，否则依据 GGUF architecture 和 hidden layers 生成 `blk.N.BB` 到 HF/SGLang parameter 名称的映射。为建立映射，源码会在 meta device 上实例化一个 dummy model 并遍历其 state dict。[`python/sglang/srt/model_loader/loader.py:3153-3218`](../../../source/sglang/python/sglang/srt/model_loader/loader.py)

正式加载前会检查映射后的额外 tensor 是否含 `lm_head.weight`，据此修正 `tie_word_embeddings`；随后通过 `gguf_quant_weights_iterator` 调用模型 `load_weights`，最后在 target device 上执行各量化 module 的 post-load 处理。[`python/sglang/srt/model_loader/loader.py:3220-3257`](../../../source/sglang/python/sglang/srt/model_loader/loader.py)

该路径的失败点包括 gguf 依赖缺失、model type 无法映射、路径不是文件以及模型参数命名不兼容；它不接受 `model_loader_extra_config`，也不能被理解为普通 safetensors loader 的别名。

### Remote 与 RemoteInstance：传输层替代本地 checkpoint

`RemoteModelLoader` 根据 connector 类型在 KV 和 FS 两条路径之间选择：KV connector 按 TP rank 取远端 weight iterator，并复用 state-dict copy、LoRA padding narrow 和 `_post_load_weights`；FS connector 则把远端 iterator 交给模型 `load_weights`，并在 quant post-process 时用 `device_loading_context` 临时把参数放到 target device。[`python/sglang/srt/model_loader/loader.py:3495-3510,3542-3591`](../../../source/sglang/python/sglang/srt/model_loader/loader.py)

`RemoteInstanceModelLoader` 先在本地 target device/meta 上创建空模型，再按 backend 分派：NCCL 建立 model-update group，由源实例广播每个 parameter，完成后销毁该 group；Transfer Engine 先注册模型 memory regions，再按 seed instance 的 session/metadata 传输；ModelExpress 则委托外部 `MxModelLoader`，缺少 `modelexpress` 依赖会立即报错。[`python/sglang/srt/model_loader/loader.py:3292-3378,3380-3427`](../../../source/sglang/python/sglang/srt/model_loader/loader.py)

这些 loader 的 `download_model` 不是普通本地下载语义：RemoteInstance 明确抛出 `NotImplementedError`，Remote 由 connector 自己提供数据；因此启动排障时应分别记录 connector type、source instance、TP rank、传输 session 和 post-load 是否执行，而不能只记录 model path。

### ModelOpt 与 Run:ai Streamer：workflow/streaming 的额外生命周期

`ModelOptModelLoader.load_model` 对已经量化的模型直接复用父类加载；未量化时进入 standard workflow，先加载 base model，再解析 `modelopt_quant` 或 unified quantization 名称，取得 `mtq` 配置，最后由 `_setup_modelopt_quantization` 执行 restore、calibration、checkpoint save 和可选 HF export。当前 calibration 使用 ModelOpt dataset utility 创建 `cnn_dailymail` dataloader（batch size 36、512 samples）；ModelOpt 不可用、配置无效或 workflow 出错均是显式依赖/配置边界。[`python/sglang/srt/model_loader/loader.py:3772-3877,3942-4038`](../../../source/sglang/python/sglang/srt/model_loader/loader.py)

`RunaiModelStreamerLoader` 将本地目录、Run:ai object-storage URI 或 HF 下载目录解析成 safetensors 文件集合，过滤 index 中未使用的重复 consolidated/sharded 文件；iterator 支持 distributed streaming、对象存储并发/内存配置，并在 draft worker 中只保留指定 MTP layer、重写为 layer 0。正式加载只接受 CUDA/CPU，通过 `DefaultModelLoader.load_weights_and_postprocess` 消费 primary 和 model 声明的 secondary sources；当前源码明确拒绝 ModelOpt quantization。[`python/sglang/srt/model_loader/loader.py:4041-4054,4088-4110,4112-4219,4221-4279`](../../../source/sglang/python/sglang/srt/model_loader/loader.py)

## 7. quantization 与 post-load

量化不是单独的“把 dtype 改成 int4”。`_get_quantization_config` 选择 `QuantizationConfig`，模型层通过 `get_quant_method` 创建 `LinearMethodBase` 或 `FusedMoEMethodBase`；参数创建阶段决定 packed weight/scale 的 shape，post-load 阶段可能执行 repack、workspace 准备或 kernel 兼容转换。M14 进一步解释运行时 MoE/LoRA。

必须区分：

- HF quantization metadata；
- 用户 `--quantization` 覆盖；
- device capability 与 backend 支持；
- KV cache quantization（由 ModelRunner/KV pool 另行处理）；
- weight-only 与 activation-aware quantization。

## 8. 生命周期和错误

### 初始化

读取 HF config、解析 architecture/dtype/context、创建 parallel-local model、选择 loader、分配权重容器。

### 正常

loader 遍历 checkpoint shards；每个 parameter 经过过滤、映射和 `weight_loader`；rank-local model 完成 post-load，随后 barrier 并交给 ModelRunner。

### 异常

- architecture 不支持：模型实例化或 registry 失败；
- checkpoint name 无法映射：AutoWeightsLoader 报错或记录未加载权重；
- shape/dtype/TP 不匹配：weight_loader 或 quant method 失败；
- load format/依赖不可用：loader dispatch 失败；
- 某 rank 加载失败：必须阻止后续 worker 假装 ready，并清理已分配资源。

### 清理

加载异常由 worker/Engine 启动清理回收进程和 device allocation；远程 loader、weight cache daemon、临时文件和通讯组需要额外关闭。权重加载成功后，M05 负责 model runner 的后续 graph/KV 生命周期。

## 9. 小例子和断点

例：TP=2 的普通 sharded checkpoint：

- 两个 rank 解析同一 architecture；
- `filter_pp_weights` 在 PP>1 时过滤层；
- column/row parameter 的 `weight_loader` 各自切片；
- packed quant parameter 同时加载 scale/weight；
- barrier 后每个 rank 只持有本地 shard。

断点：

- `model_runner_components/load_model_utils.py:build_load_config`：确认最终 load format/quant；
- `loader.py:get_model_loader`：确认 loader 分派；
- `auto_loader.py:filter_pp_weights`：确认 PP 过滤；
- parameter 的 `weight_loader`：确认 shape、rank 和 packed mapping；
- `_post_load_weights` 与 `dist_barrier_after_load`：确认 ready 前置条件。

## 10. 测试与未验证

- `test/registered/model_loading/`：模型加载专项；
- `test/registered/unit/model_loader/`：loader/weight mapping 单测；
- `test/manual/test_weight_loader_v2_equiv.py`、`test_manual/test_weight_validation.py`（实际路径以 checkout 为准）：v2 等价性和验证；
- `test/manual/test_srt_engine_with_quant_args.py`、`test/manual/test_modelopt.py`：量化分支。

本次未下载 checkpoint、未执行 GPU loader、未验证多 rank barrier、remote/presharded/GGUF/ModelOpt 等分支。上述测试文件的存在不是运行成功证据。

## 11. 修改影响

修改 ModelConfig 会影响 M01 参数解析、M05 runner、M07 parallel groups、M09 attention/KV layout、M10 sampling capacity 和 M12 multimodal capability。修改 weight mapping 会影响 checkpoint compatibility、TP/PP rank、quant kernel 和 LoRA base weights。修改 loader cleanup 还影响 M15 Engine ready/error propagation。

**已确认**：loader 通过配置、架构、参数路由和 post-load 形成 rank-local model；`get_model_loader` 的主要短路优先级、`from_server_args` 的代表字段投影和 Llama/Qwen2 v2 路由已由当前 checkout 静态确认。**仍待复核**：复杂架构的全部专用 loader 行为、所有模型的完整权重映射以及 `ModelConfig` 全部 HF/multimodal 字段来源。**未验证**：真实权重加载。
