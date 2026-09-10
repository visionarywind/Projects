# 技术债与后续路线

## 当前知识库缺口

1. M02 的 `llama_context::decode` 内部 graph builder 尚未逐阶段记录。
2. M01 仅覆盖统一 scheduler/device 边界，未覆盖 CUDA/Metal/MUSA/Vulkan 等具体 kernel。
3. M04 router、MCP、sleep、resumable stream 的实现仍以开发文档和关键片段为主。
4. M06 各视觉/音频模型和媒体 subprocess 未做真实资产 Demo。
5. M09 缺少 routes -> stores -> services -> API 的源码级矩阵。
6. 未执行 server pytest、UI tests、`test-backend-ops` 或真实模型命令；GGUF unittest 已执行但因缺少 `pytest` 导入依赖而失败。
7. CTest `-N` 已执行并确认注册信息，但测试 target 尚未构建；链接、行号和证据只做了人工首版整理，尚未自动审计。

## 下一步优先级

1. 在固定模型和 CPU build 上执行 D01/D02，保存命令、环境和结果。
2. 深入 `llama_context::decode`、构造函数、KV memory 和一个具体 `src/models/*.cpp`。
3. 在一个可用 accelerator 上走 `test-backend-ops` 和 benchmark。
4. 对 server streaming/sleep/router 做独立 call-chain 和错误清理审计。
5. 用 UI 真实 route/service 和 e2e 补齐 M09。
6. 每次源码升级先更新 [analysis-state](../00-overview/analysis-state.md)，再更新受影响引用。

## 审计结论

当前交付达到“静态阶段 0-6 首版”而非“全部源码深度完成”：关键公共 wrapper 未作为终点，已追踪到 model load、context creation、scheduler 和 server decode；Demo 路径已完整描述但未运行。该限制是已确认事实，不应隐藏。

---

**页尾：** 后续工作以实际输出补充，不用预填性能或通过状态。
