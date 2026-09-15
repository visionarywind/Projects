# M01 - ggml 张量与后端

- 文档目的：解释 01-modules/M01-ggml-backend/README.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/llamacpp HEAD 093a2f86c（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M01-ggml-backend/README.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


> 状态：首版实现层；关键边界已确认，具体 kernel 未展开。

## 职责

M01 提供 tensor、graph、内存分配、scheduler、backend registry/device 抽象和平台实现。顶层构建把它作为 llama 的底层依赖。[CMakeLists.txt:204-221] ggml 支持 CPU 及 CUDA/HIP/MUSA/Metal/Vulkan/SYCL/CANN 等可选后端；具体开关集中在 ggml CMake。[ggml/CMakeLists.txt:189-281]

## 关键流程

`llama_backend_init` 初始化计时、创建临时 context 触发 F16 表初始化，并在 registry 为空时加载所有 backend。[src/llama.cpp:122-135] 模型设备准备阶段枚举设备、去重 GPU、构造 tensor split 的 meta device，并把设备加入 model。[src/llama.cpp:158-312]

context 的 `graph_compute` 根据 batched 状态选择线程池，向各 backend 设置线程数，最后调用 `ggml_backend_sched_graph_compute_async`。[src/llama-context.cpp:2492-2518]

## 关键约束

- Tensor split 不是任意架构都支持；runtime 在 model creation 处拒绝不支持架构。[src/llama-model.cpp:349-356]
- meta device 的 split state 会按 tensor name、层号、轴和设备数决定分片；cache/indexer 等模式可能 mirrored。[src/llama-model.cpp:371-509]
- `GGML_SCHED_NO_REALLOC` 是调试配置，不应未经验证当作普遍运行要求。[ggml/CMakeLists.txt:189-191]

## 错误与资源

registry 为空时模型加载失败；设备参数为空或 tensor split 无设备返回 false；scheduler 非成功状态由 context 记录错误。[src/llama.cpp:158-207] [src/llama.cpp:404-409] [src/llama-context.cpp:2511-2518]

## 测试与修改影响

改变 op、backend placement、allocator 或 scheduler 后，应构建 `test-backend-ops`；还需按实际硬件跑对应 backend test。CTest 注册和 target 关系见 [M08](../M08-build-tests-ci/README.md)。

## 资源专题补充

`ggml_tallocr` 是单 buffer bump allocator，`ggml_dyn_tallocr` 是最多 16 chunk、最多 256 free blocks 的动态复用器；`llama_context::sched_reserve` 通过 graph reserve 固定 backend buffer 预期大小，并在 graph/memory 变化时 reset/re-reserve。详见[池化与资源管理专题](../../90-cross-module/pooling-and-resource-management.md)。

## 未覆盖

各平台 kernel、动态 backend ABI、具体 graph op 支持矩阵、设备间 copy 性能均未在本首版逐行确认。

---

**页尾：** 本页描述统一接口边界，不推断任何未实测性能。

## 文档元数据（规范补充）

- 文档目的：说明 `01-modules/M01-ggml-backend/README.md` 的源码分析范围、结论和维护入口。
- 适用范围：当前项目对应模块/入口的静态源码与测试分析。
- 对应源码版本：以本项目 `00-overview/analysis-state.md` 或同页版本字段为准。
- 证据状态：静态源码证据；未执行的构建、测试、GPU、网络或多进程行为保持“未验证”。
- 最后更新：2026-09-15
- 前置阅读：本项目根 README 与 `00-overview/analysis-state.md`。
- 后续阅读：本模块/示例的实现、测试和风险页面。

## 深度审计

| 分析对象 | 入口落地 | 正常路径 | 分支 | 异常 | 清理 | 数据生命周期 | 执行上下文 | 行级证据 | Demo 映射 | 状态/缺口 |
|---|---|---|---|---|---|---|---|---|---|---|
| `01-modules/M01-ggml-backend/README.md` | 已完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 已映射或不适用 | 部分完成：动态行为、边界或专用变体仍需验证 |

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
