# M06 推理与工具

- 文档目的：解释 01-modules/M06-inference-and-tools/README.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：`3703d4e33a3a2b2d11ebcc8e41f45af7ce7d1eda`
- 证据状态：静态源码分析；未启动推理服务。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M06-inference-and-tools/README.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。

## 组成

```text
server/CLI
  -> model builder + checkpoint load
  -> GPTInferenceWrapper
  -> TextGenerationController
  -> Static/DynamicInferenceEngine
    -> Scheduler
    -> Dynamic/StaticInferenceContext
    -> KV block allocator
    -> sampling backend
  -> request/output/stream
```

## Dynamic engine

动态 engine 允许不同请求拥有不同 prompt/output 当前长度，并以总 token 容量约束 batch；context 管理 block-level KV cache，engine 管理 waiting/running/paused 等请求状态。[`core/inference/engines/dynamic_engine.py:292-427`]

## Static compatibility

`StaticInferenceEngine` 接收 static context，但非 legacy 路径会创建 `DynamicInferenceContext` 和 `DynamicInferenceEngine`；初始化异常时恢复原 static context 并回退 legacy。代码明确将其标记为未来弃用。[`core/inference/engines/static_engine.py:35-132`]

## 服务入口

`tools/run_text_generation_server.py` 解析参数、初始化 Megatron、构建/加载模型、创建 static engine，并在首个 PP/TP/EP rank 上运行 `MegatronServer`。[`tools/run_text_generation_server.py:51-192`]

## 未验证

KV cache 命中率、跨 rank 服务行为、KV handoff 和网络部署都需硬件/依赖实测。CUDA Graph 的 graph pool、static input、capture/replay 和 reset 语义见 [CUDA Graph 与显存池生命周期](../../90-cross-module/cuda-graph-resource-lifecycle.md)，同样尚未 GPU 实测。

## 文档元数据（规范补充）

- 文档目的：说明 `01-modules/M06-inference-and-tools/README.md` 的源码分析范围、结论和维护入口。
- 适用范围：当前项目对应模块/入口的静态源码与测试分析。
- 对应源码版本：以本项目 `00-overview/analysis-state.md` 或同页版本字段为准。
- 证据状态：静态源码证据；未执行的构建、测试、GPU、网络或多进程行为保持“未验证”。
- 最后更新：2026-09-15
- 前置阅读：本项目根 README 与 `00-overview/analysis-state.md`。
- 后续阅读：本模块/示例的实现、测试和风险页面。

## 深度审计

| 分析对象 | 入口落地 | 正常路径 | 分支 | 异常 | 清理 | 数据生命周期 | 执行上下文 | 行级证据 | Demo 映射 | 状态/缺口 |
|---|---|---|---|---|---|---|---|---|---|---|
| `01-modules/M06-inference-and-tools/README.md` | 已完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 已映射或不适用 | 部分完成：动态行为、边界或专用变体仍需验证 |

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
