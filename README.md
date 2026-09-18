# 概述
仓库对开源代码仓使用 AI 进行分析和使用 vibe-coding 设计实现一些功能
仓库对部分上游源码使用 git submodule 或独立 checkout，对开源仓库进行分析的 prompt 为 project-prompt.md

## 源码与分析文档边界

分析文档与源码目录分离，但 `source/` 不是单一的 Git submodule 集合：当前同时包含根仓库 submodule、独立 Git checkout、根仓库内的源码快照，以及只保留远端源码分析文档的项目。不要仅根据目录名推断其来源。

| 类别 | 当前目录 | 说明 |
| --- | --- | --- |
| 根仓库 submodule | `source/concurrency-queue` | `.gitmodules` 唯一登记的 submodule，对应 `concurrentqueue` |
| 独立源码 checkout | `source/LeetCUDA`、`source/leveldb`、`source/llamacpp`、`source/megatron-lm`、`source/Megatron-LM`、`source/sglang`、`source/ray` | 各目录有自己的 Git 元数据；`source/megatron-lm` 与 `source/Megatron-LM` 是不同 checkout，项目文档以小写目录为准 |
| CANN 独立 checkout | `source/cann/ge`、`source/cann/runtime`、`source/cann/driver`、`source/cann/shmem` | 四个独立仓库，分别维护版本和工作树 |
| 根仓库源码快照 | `source/cuda` | 没有独立 Git 根，版本和提交不能按 submodule 处理 |
| 远端源码分析 | `musa/` | 文档对应远端 `/home/shanfeng/workspace/linux-ddk/musa`，当前目录没有 `source/musa` |

上游地址仅作为来源记录，不代表当前目录一定能通过 `git submodule update` 获取对应源码：

| 项目 | 来源 |
| --- | --- |
| concurrency-queue | `git@github.com:cameron314/concurrentqueue.git` |
| LeetCUDA | `git@github.com:xlite-dev/LeetCUDA.git` |
| leveldb | `git@github.com:google/leveldb.git` |
| llamacpp | `git@github.com:ggml-org/llama.cpp.git` |
| megatron-lm | `git@github.com:NVIDIA/Megatron-LM.git` |
| sglang | `https://github.com/sgl-project/sglang.git` |
| ray | `https://github.com/ray-project/ray.git` |
| CANN GE/Runtime/Driver/SHMEM | `https://gitcode.com/cann/` 下的四个独立仓库 |
| MUSA | 远端工作区，见 `musa/00-overview/analysis-state.md` |

只有需要初始化根仓库登记的 submodule 时执行：

```bash
git submodule sync --recursive
git submodule update --init --recursive
```

本仓库不提供跨项目统一构建入口。各上游项目的构建、测试和依赖要求以对应源码目录及分析文档为准；`data-structure/` 仍可按其 README 独立使用 CMake 构建。

## AI 分析开源仓库

- cann
https://gitcode.com/cann/runtime.git
https://gitcode.com/cann/driver.git
https://gitcode.com/cann/ge.git
https://gitcode.com/cann/shmem.git

- concurrency-queue
git@github.com:cameron314/concurrentqueue.git

- LeetCUDA
git@github.com:xlite-dev/LeetCUDA.git

- leveldb
git@github.com:google/leveldb.git

- llamacpp
git@github.com:ggml-org/llama.cpp.git

- megatron-lm
git@github.com:NVIDIA/Megatron-LM.git

- sglang
https://github.com/sgl-project/sglang.git

- ray
https://github.com/ray-project/ray.git

- musa
远端 `/home/shanfeng/workspace/linux-ddk/musa`，当前工作区只保留分析文档

## Vibe-coding
- data-structure
目前实现了rbtree
