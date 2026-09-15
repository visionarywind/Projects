# 80-demos：真实端到端示例

- 文档目的：解释 80-demos/demo-registry.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/LeetCUDA HEAD 4513b31（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 80-demos/demo-registry.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## 目的

本层把模块文档串成可复现的源码路径。所有命令均来自仓库入口，但当前环境没有 GPU/CUDA/PyTorch 运行验证，因此命令和输出均标为“未验证”。

## Demo 注册表

| ID | Demo | 真实入口 | 覆盖 | 状态 |
|---|---|---|---|---|
| D01 | [NMS Python](D01-nms-python/README.md) | `kernels/nms/nms.py` | M08、M10；动态扩展、stable sort、两阶段 kernel、reference | 部分完成 |
| D02 | [Interview binary](D02-interview-binary/README.md) | `kernels/interview/build.sh`、`notes-v2.cu` | M01、M05-M07、M09；编译、CLI、显式资源和 benchmark | 部分完成 |

## 双向映射

| Demo 步骤 | 模块文档 | 源码证据 |
|---|---|---|
| D01-S01 load / 参数 | M08、M10 | `[kernels/nms/nms.py:8-23]`、`[kernels/nms/nms.cu:126-150]` |
| D01-S02 stable order | M10 | `[kernels/nms/nms.cu:152-164]` |
| D01-S03 mask | M10 | `[kernels/nms/nms.cu:22-71]` |
| D01-S04 resolve/mapping | M10 | `[kernels/nms/nms.cu:85-109,166-192]` |
| D02-S01 arch build | M09 | `[kernels/interview/build.sh:134-185]` |
| D02-S02 CLI dispatch | M09 | `[kernels/interview/notes-v2.cu:510-542,641-699]` |
| D02-S03 HGEMM | M06 | `[kernels/interview/notes-v2.cu:2420-2579]` |
| D02-S04 FlashAttention | M07 | `[kernels/interview/notes-v2.cu:3094-3190,3400-3497]` |

## 使用规则

1. 先读 Demo README，再读 execution/data/failure；
2. 运行前记录 GPU、driver、CUDA、PyTorch 和子模块 commit；
3. 不把历史 README benchmark 当作当前输出；
4. 修改 Demo 后同时更新对应模块的 source-map、evidence-index 和 analysis-state；
5. 失败路径和清理路径必须和正常路径同等记录。

## 当前缺口

- D01/D02 尚未在当前环境执行；
- 具体 CLI phase dispatch 仍需继续补充全量行级证据；
- Demo 文档与所有 M02-M05 小算子尚未建立一对一覆盖。

## 相关文档
- [项目入口](../README.md)
- [分析状态](../00-overview/analysis-state.md)
- [源码证据索引](../00-overview/evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
