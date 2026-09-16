# arXiv PDF 原文解析材料

本目录保存 KV Cache 池化专题使用的 arXiv 原始 PDF 与 `pdftotext` 抽取文本。人工归纳结论见 [`../03-arxiv-reading-notes.md`](../03-arxiv-reading-notes.md)。

## 解析方法

- PDF 来源：`https://arxiv.org/pdf/<arxiv-id>`。
- 元数据：`pdfinfo <pdf>`。
- 文本抽取：`pdftotext -layout <pdf> <txt>`。
- 解析日期：各论文 metadata 文件中的 `Parsed on` 字段。

## 文件清单

| arXiv ID | 论文 | 状态 | 页数 | 元数据 | 抽取文本 | 中文详译 |
|---|---|---:|---:|---|---|---|
| 2309.06180 | PagedAttention / vLLM | parsed | 16 | [metadata](2309.06180-pagedattention-vllm.md) | [text](2309.06180-pagedattention-vllm.txt) | [zh](zh/2309.06180-pagedattention-vllm.zh.md) |
| 2405.04437 | vAttention | parsed | 18 | [metadata](2405.04437-vattention.md) | [text](2405.04437-vattention.txt) | [zh](zh/2405.04437-vattention.zh.md) |
| 2408.08147 | P/D-Serve | parsed | 15 | [metadata](2408.08147-pd-serve.md) | [text](2408.08147-pd-serve.txt) | [zh](zh/2408.08147-pd-serve.zh.md) |
| 2411.01142 | NEO CPU offloading | parsed | 13 | [metadata](2411.01142-neo-cpu-offloading.md) | [text](2411.01142-neo-cpu-offloading.txt) | [zh](zh/2411.01142-neo-cpu-offloading.zh.md) |
| 2411.17089 | KVPR partial recomputation | parsed | 15 | [metadata](2411.17089-kvpr-partial-recompute.md) | [text](2411.17089-kvpr-partial-recompute.txt) | [zh](zh/2411.17089-kvpr-partial-recompute.zh.md) |
| 2506.15155 | eLLM elastic memory | parsed | 8 | [metadata](2506.15155-ellm-elastic-memory.md) | [text](2506.15155-ellm-elastic-memory.txt) | [zh](zh/2506.15155-ellm-elastic-memory.zh.md) |
| 2508.13231 | Dynamic KV placement | parsed | 4 | [metadata](2508.13231-dynamic-kv-placement.md) | [text](2508.13231-dynamic-kv-placement.txt) | [zh](zh/2508.13231-dynamic-kv-placement.zh.md) |
| 2510.09665 | LMCache | parsed | 19 | [metadata](2510.09665-lmcache.md) | [text](2510.09665-lmcache.txt) | [zh](zh/2510.09665-lmcache.zh.md) |
| 2607.16892 | Robust KV management | parsed | 10 | [metadata](2607.16892-robust-kv-management.md) | [text](2607.16892-robust-kv-management.txt) | [zh](zh/2607.16892-robust-kv-management.zh.md) |

## 证据边界

- `parsed` 只表示 PDF 下载成功且文本可抽取，不表示论文结论已在本仓库复现。
- PDF 中的 throughput、latency、cost、memory saving 等数字仍是作者在论文环境下的报告。
- 本目录不包含 GPU、模型、RDMA、多机或线上压测运行记录。
