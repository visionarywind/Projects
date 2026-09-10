# 80-demos：真实端到端示例

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
