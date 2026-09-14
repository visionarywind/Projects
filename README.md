# 概述
仓库对开源代码仓使用 AI 进行分析和使用 vibe-coding 设计实现一些功能
仓库使用 git submodule 链接相关的开源仓库，对开源仓库进行分析的 prompt 为 project-prompt.md

## 上游源码 submodule

上游源码统一检出到 `source/`，分析文档与源码目录分离。CANN 由四个独立仓库组成，放在 `source/cann/` 下：

| 目录 | 仓库 |
| --- | --- |
| `source/concurrency-queue` | `git@github.com:cameron314/concurrentqueue.git` |
| `source/LeetCUDA` | `git@github.com:xlite-dev/LeetCUDA.git` |
| `source/leveldb` | `git@github.com:google/leveldb.git` |
| `source/llamacpp` | `git@github.com:ggml-org/llama.cpp.git` |
| `source/megatron-lm` | `git@github.com:NVIDIA/Megatron-LM.git` |
| `source/sglang` | `https://github.com/sgl-project/sglang.git` |
| `source/cann/runtime` | `https://gitcode.com/cann/runtime.git` |
| `source/cann/driver` | `https://gitcode.com/cann/driver.git` |
| `source/cann/ge` | `https://gitcode.com/cann/ge.git` |
| `source/cann/shmem` | `https://gitcode.com/cann/shmem.git` |

首次获取源码或同步仓库配置时执行：

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

## Vibe-coding
- data-structure
目前实现了rbtree
