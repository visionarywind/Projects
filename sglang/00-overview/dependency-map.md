# 依赖地图

- 文档目的：解释 00-overview/dependency-map.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/sglang HEAD 78be4b50af（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](analysis-state.md)。
## 结论摘要

本页聚焦 00-overview/dependency-map.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## 1. 逻辑依赖

```text
M01 CLI
  -> M02 API / M03 Tokenizer
  -> M15 IPC
M03 Tokenizer
  -> M04 Scheduler
M04 Scheduler
  -> M05 Model execution
  -> M08 KV cache
  -> M15 IPC
M05 ModelRunner
  -> M06 Model loading
  -> M07 Distributed
  -> M09 Attention/CUDA Graph
  -> M10 Sampling
M06 Model loading
  -> M07 rank/layout
  -> M14 quantization/MoE/LoRA hooks
M11 Speculative
  -> M04 scheduler / M05 execution / M08 KV ownership / M10 sampling
M12 Multimodal
  -> M02 protocol / M03 tokenize state / M05 model input / M09 graph eligibility
M13 Disaggregation/HiCache
  -> M04 admission / M08 KV layout / M15 transfer/control / external backends
M14 MoE/quant/LoRA
  -> M05 execution / M06 weights / M07 TP-EP / M16 kernels
M16 Kernels/platforms
  -> M05/M09/M14
M17 Rust/router/gateway
  -> M02 protocol / M04 worker routing / M08-M13 KV-aware paths
M18 Tests/CI
  -> all modules (verification boundary)
```

## 2. 启动依赖与请求依赖

启动依赖是有向的：配置必须先解析，进程组必须在依赖 collective 的模型/worker 前建立，模型和 KV pool 必须在第一批请求前就绪。请求依赖则由 Scheduler 每轮动态决定，不应把请求加入队列的时序画成启动依赖。

## 3. 外部依赖边界

`python/pyproject.toml` 是 Python 包依赖声明；`python/setup.py` 从 Cargo workspace metadata 发现 PyO3 扩展；`rust/Cargo.toml` 管理 Rust workspace。CUDA/ROCm/NPU/XPU 等设备库由平台和安装环境提供，源码仓库不能保证本机具备它们。

## 4. 读图方法

看到一个 import 只说明静态依赖；要判断运行依赖，应继续追踪：谁实例化对象、谁调用方法、谁写状态/设备/网络，以及异常如何跨边界传播。后续 `cross-module-call-chains.md` 会记录这些动态链。

## 相关文档
- [项目入口](../README.md)
- [分析状态](analysis-state.md)
- [源码证据索引](evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
