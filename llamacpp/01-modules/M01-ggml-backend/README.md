# M01 - ggml 张量与后端

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
