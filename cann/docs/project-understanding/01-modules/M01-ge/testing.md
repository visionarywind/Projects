# M01 GE 测试

- 文档目的：解释 01-modules/M01-ge/testing.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cann HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：测试边界已确认；本环境未执行
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M01-ge/testing.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。

## 测试层次

- Graph/Node/Tensor/Anchor 元数据和图合法性 UT。
- Parser 到 AscendIR 的模型解析测试。
- Compiler Pass、Engine、内存/流规划和模型序列化测试。
- V1/V2 Executor 的 Load、Execute、UnLoad、重复调用和异常清理测试。
- 与 Runtime/ACL/Driver 和 NPU 的模型集成、动态 shape、异步 callback 测试。

## V2 回归矩阵

1. 未 Load 时 Execute，应返回状态错误。
2. 输入/输出数量不匹配、空指针和不允许的 IO 地址复用。
3. 外部 Stream 与内部 allocator 的组合。
4. init graph、main graph、de-init graph 任一路径失败后的清理。
5. ExecuteSync 超时、异步完成和重复 UnLoad。

构建入口见 [examples.md](examples.md)；无设备时不宣称模型端到端通过。

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
