# arXiv PDF 中文详译索引

本目录保存 KV Cache 池化专题相关 arXiv PDF 的中文详译/译注。译文依据 [`../`](../) 中保存的原始 PDF、`pdftotext -layout` 抽取文本和人工解析整理。

## 翻译范围说明

- 译文覆盖：摘要、引言/问题背景、核心机制、系统/算法、实验与作者报告结果、局限和 KV pool 相关术语。
- 非逐字覆盖：参考文献、版权页、作者机构、公式完整推导、不可可靠抽取的图表 OCR 噪声和与 KV pool 主题弱相关的重复段落。
- 论文中的 throughput、latency、cost、memory saving 等数字仍是作者报告，不是本仓库复现实验。

## 译文清单

| arXiv ID | 论文 | 中文译文 | 原始材料 |
|---|---|---|---|
| 2309.06180 | PagedAttention / vLLM | [中文详译](2309.06180-pagedattention-vllm.zh.md) | [metadata](../2309.06180-pagedattention-vllm.md) |
| 2405.04437 | vAttention | [中文详译](2405.04437-vattention.zh.md) | [metadata](../2405.04437-vattention.md) |
| 2411.01142 | NEO CPU offloading | [中文详译](2411.01142-neo-cpu-offloading.zh.md) | [metadata](../2411.01142-neo-cpu-offloading.md) |
| 2411.17089 | KVPR partial recomputation | [中文详译](2411.17089-kvpr-partial-recompute.zh.md) | [metadata](../2411.17089-kvpr-partial-recompute.md) |
| 2408.08147 | P/D-Serve | [中文详译](2408.08147-pd-serve.zh.md) | [metadata](../2408.08147-pd-serve.md) |
| 2506.15155 | eLLM elastic memory | [中文详译](2506.15155-ellm-elastic-memory.zh.md) | [metadata](../2506.15155-ellm-elastic-memory.md) |
| 2508.13231 | Dynamic KV placement | [中文详译](2508.13231-dynamic-kv-placement.zh.md) | [metadata](../2508.13231-dynamic-kv-placement.md) |
| 2510.09665 | LMCache | [中文详译](2510.09665-lmcache.zh.md) | [metadata](../2510.09665-lmcache.md) |
| 2607.16892 | Robust KV management | [中文详译](2607.16892-robust-kv-management.zh.md) | [metadata](../2607.16892-robust-kv-management.md) |

## 阅读建议

1. 先读 [`../../03-arxiv-reading-notes.md`](../../03-arxiv-reading-notes.md) 获取横向对比。
2. 再按兴趣阅读单篇中文详译。
3. 若需要核对术语或数字，回到对应的英文抽取文本和 PDF metadata。
