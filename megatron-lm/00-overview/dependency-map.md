# 依赖地图

- 文档目的：区分声明、构建、运行时、数据和生命周期依赖。
- 适用范围：当前源码与 dev CI 环境。
- 对应源码版本：`8190837c2b6ce176a431bc2a6ffd3439507648a7`
- 证据状态：已确认；动态可选依赖为部分推断
- 最后更新：2026-09-10
- 前置阅读：[项目定位](project-overview.md)
- 后续阅读：[构建与部署](build-and-deploy.md)

## 结论摘要

Python 包和 uv lock 描述可复现依赖；训练运行时依赖 PyTorch CUDA/NCCL，性能路径可选 Transformer Engine/Apex/ModelOpt；数据和 checkpoint 依赖文件系统及外部数据。不能用静态 import 图代替运行时 backend 选择。

| 依赖类别 | 依赖 | 由谁使用 | 证据/说明 |
|---|---|---|---|
| 声明 | `torch>=2.6.0`, numpy, packaging | Core 全局 | `pyproject.toml:21-28` |
| 构建 | setuptools, pybind11, C++17 | dataset helper | `pyproject.toml:1-4`; `setup.py:6-13` |
| 开发 | ruff/black/isort/pylint/pytest | lint/test | `pyproject.toml:145-187` |
| 性能 backend | Transformer Engine, Apex（可选） | optimizer/transformer | `megatron/core/optimizer/__init__.py:13-35` |
| 分布式 backend | NCCL / torch.distributed | groups and GPU | `examples/run_simple_mcore_train_loop.py:45-53` |
| 运行时数据 | tokenizer、dataset 文件或 mock | dataset builders | `examples/run_simple_mcore_train_loop.py:81-120` |
| 持久化 | checkpoint directory | dist checkpointing | `examples/run_simple_mcore_train_loop.py:168-217` |

## 依赖方向

`entrypoint → training → core model/parallel/data/optimizer → PyTorch → CUDA/NCCL`；`optimizer → checkpoint` 是状态持久化关系，不是 optimizer 必须 import checkpoint 实现。`tools` 通过 CLI 使用 Core/训练 API，推理服务是另一条运行时入口。

## 可选依赖选择

optimizer 模块优先 import Transformer Engine，再 Apex，最后 Torch fallback。[megatron/core/optimizer/__init__.py:13-35] 这是已确认的选择顺序；是否得到性能等价行为未知，应分别测试。

## 相关文档

- [构建运行](build-and-deploy.md)
- [设计取舍](design-principles.md)
- [M05](../01-modules/M05-optimizer-checkpointing/README.md)

## 源码证据摘要

见表格。

## 未解决问题

缺少目标容器实际 `uv lock` resolution、GPU driver 和 optional package 版本的本机验证。

## 下一步阅读建议

先读构建技能，再读 CI lint/test workflow。
