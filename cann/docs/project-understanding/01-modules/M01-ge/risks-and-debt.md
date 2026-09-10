# M01 GE 风险与技术债务

- 证据状态：风险评估；不是已验证缺陷

## 高风险

- Graph/Model/Runtime/算子包版本不匹配会导致编译、加载或执行失败。
- V2 Executor 状态顺序、init/de-init 图失败和 Stream/Event/allocator 生命周期错误可能泄漏资源或产生异步故障 `[ge/runtime/v2/core/model_v2_executor.cc:201-318]`。
- GE 到 Runtime 的跨仓 ABI 和设备错误码转换未完全静态追踪。

## 中风险

- V1/V2 自动选择、Sink/SuperKernel 启用条件和动态 shape 行为需要按场景确认。
- Compiler Pass 顺序、EnginePartitioner、Memory Planner 和流分配细节尚未形成完整结构图。

## 待补

- 完成 V1/V2、Compiler、Parser、MetaDef 的符号级调用图。
- 建立模型格式、Graph/Tensor ABI 和算子包兼容矩阵。
- 增加 ASAN、异步故障注入、重复生命周期和无设备 mock 回归。
