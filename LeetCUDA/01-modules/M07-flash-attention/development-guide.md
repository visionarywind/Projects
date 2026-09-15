# M07 开发指南

- 文档目的：解释 01-modules/M07-flash-attention/development-guide.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/LeetCUDA HEAD 4513b31（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M07-flash-attention/development-guide.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## 修改前

1. 先确定目标变体和真实 export；不要只改名字相近的 `.cu`。
2. 记录 `[B,H,N,D]` layout、Br/Bc、stage、padding、accumulator 和架构宏。
3. 检查 `QKV_seqlen % max(Br,Bc)==0` 约束和 D dispatch。[kernels/flash-attn/mma/basic/flash_attn_mma_tiling_qkv.cu:799-945]

## 修改后

1. 对 torch/SDPA/reference 做 correctness；
2. 覆盖整 tile、尾 tile、D dispatch、stage 1/2；
3. 检查 shared alias、barrier、shuffle mask；
4. 再用 sanitizer 和 Nsight；
5. 更新 source-map、line-level、performance、risk 和 evidence-index。

当前仓库没有统一 GPU CI；命令是否可运行未验证。`setup.py` 和 `tools/utils.py` 为空，主要构建入口仍是 `flash_attn_mma.py` 的动态 load，这一事实不能被文档入口混淆。

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
