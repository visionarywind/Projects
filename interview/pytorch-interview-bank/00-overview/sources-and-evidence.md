# 资料与证据

## 版本基线

- `[已确认]` PyTorch `v2.14.0` 是调研时官方仓库列出的最新稳定发布版，发布时间 2026-09-02。来源：[PyTorch releases](https://github.com/pytorch/pytorch/releases/tag/v2.14.0)。
- `[已确认]` 本题库不把仓库中存在的命令写成已经运行成功。性能、GPU、通信和部署结果必须有对应运行记录。
- `[待验证]` 当前工作区没有 PyTorch 源码副本，因此具体源码行号需要在目标版本源码可用后补核。

## 主要资料入口

| 主题 | 资料 |
|---|---|
| Tensor 与 autograd | [官方 Tensor 教程](https://pytorch.org/tutorials/beginner/basics/tensorqs_tutorial.html)、[Autograd](https://pytorch.org/docs/stable/autograd.html) |
| Module 与训练 | [官方训练教程](https://pytorch.org/tutorials/beginner/basics/intro.html) |
| AMP | [Automatic Mixed Precision](https://pytorch.org/docs/stable/amp.html) |
| 分布式 | [Distributed overview](https://pytorch.org/docs/stable/distributed.html) |
| 编译 | [torch.compile](https://pytorch.org/docs/stable/torch.compiler.html) |
| 性能 | [Profiler](https://pytorch.org/docs/stable/profiler.html) |
| 序列化/导出 | [Serialization](https://pytorch.org/docs/stable/notes/serialization.html)、[Export](https://pytorch.org/docs/stable/export.html) |

链接中的具体 API 说明应以目标版本文档为准；若稳定文档与 `v2.14.0` 标签不一致，记录差异。
