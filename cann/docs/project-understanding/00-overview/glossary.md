# 术语表

- 文档目的：统一四仓共享术语
- 适用范围：全工作区
- 对应源码版本：见 `analysis-state.md`
- 证据状态：术语定义结合源码和仓库文档；部分为工程惯用语
- 最后更新：2026-09-10
- 前置阅读：architecture.md
- 后续阅读：模块接口文档

| 术语 | 含义 |
|---|---|
| CANN | Compute Architecture for Neural Networks，昇腾软件栈。 |
| GE | Graph Engine，图编译器和执行器。 |
| AscendIR/AIR | GE 使用的图中间表示。 |
| Graph/Node/Tensor | 图、算子节点、张量；图边在实现中通过 DataAnchor/CtrlAnchor 表达。 |
| ACL | 面向应用的 AI 计算语言/开发运行平台及 API。 |
| `aclrt*` | ACL Runtime 对外 C API 家族。 |
| `rt*` | Runtime 核心 C API 家族。 |
| Context/Stream/Event | 执行上下文、异步任务流、同步事件等运行时对象。 |
| OM/Model | GE 编译产物或可加载模型。 |
| V1/V2 Executor | GE 已知形状/传统执行路径和 RT2 动态执行路径。 |
| HAL | Hardware Abstraction Layer，硬件抽象层。 |
| SDK-driver | Driver 软件开发套件/设备与内核适配层。 |
| HDC | Host-Device Communication，主机与设备通信。 |
| esched | Event Schedule，事件调度。 |
| SVM | Shared Virtual Memory，共享虚拟内存。 |
| DFX | Diagnostics/Debug/Performance 等维测能力。 |
| Sink | 将完整模型执行序列下沉到设备侧，减少主机逐任务下发。 |
| SoC | System on Chip；文档中的 SoC version 用于选择平台能力。 |

## 相关文档

- [architecture.md](architecture.md)

## 源码证据摘要

GE 图元素和 Sink 见 `[ge/docs/zh/design/architecture.md:54-93,172-185]`；Driver 缩写和目录见 `[driver/README.md:9-16,52-114]`。

## 未解决问题

不同产品对“Model/OM”“Runtime/RTS”等术语的精确包名需以目标版本发布文档为准。

## 下一步阅读建议

读模块接口时同时查本表，避免将 ACL API、Runtime API 和 Driver HAL 混为一层。
