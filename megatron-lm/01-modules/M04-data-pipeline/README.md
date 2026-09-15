# M04 数据管线

- 对应源码版本：`3703d4e33a3a2b2d11ebcc8e41f45af7ce7d1eda`
- 证据状态：静态源码分析；未运行真实数据构建。

## 主链

```text
GPTDatasetConfig
  -> Indexed/Mock low-level dataset
  -> GPTDataset
    -> document_index/sample_index/shuffle_index
    -> __getitem__ tokens/labels/masks/positions
  -> blended builder
  -> sampler/DataLoader
  -> pretrain_gpt.get_batch
    -> CUDA/TP broadcast/CP slicing
    -> forward_step
```

## 样本语义

`GPTDataset.__getitem__` 将文本转为 long tensor。配置 `add_extra_token_to_sequence` 时使用 `text[:-1]`/`text[1:]`；否则 labels 是左移后的 text，最后位置填 pad token。pad 位置从 loss mask 排除，输入中的 pad token 映射为安全 token。[`gpt_dataset.py:264+`]

mask 与 position ids 可缓存；`reset_position_ids`、`reset_attention_mask`、`eod_mask_loss` 和 inter-document masking 会改变缓存和 `cu_seqlens` 生成。`get_batch` 再处理 packed sequence、CP 和 TP rank。[`pretrain_gpt.py:111+`]

## 不变量

- tokens/labels 长度和 sequence-length 约束一致。
- loss mask 必须与有效 label 对齐。
- packed sequence 的 flatten tensor 必须伴随 cu-sequence metadata。
- sampler 的 DP rank/size 决定每个进程看到的 batch。