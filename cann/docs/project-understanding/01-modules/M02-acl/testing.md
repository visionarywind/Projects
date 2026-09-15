# M02 ACL 测试

- 文档目的：解释 01-modules/M02-acl/testing.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cann HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：测试入口已确认；测试结果未验证
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M02-acl/testing.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。

## 测试层次

1. **Wrapper UT**：空指针、非法设备 ID、错误码映射和统计路径。
2. **Resource UT**：Context/Stream/Event/Memory 的创建、重复销毁和异常顺序。
3. **Model/Op UT**：模型加载、参数校验、异步 callback 和输出 buffer。
4. **Integration**：与 Runtime、Driver、固件和真实 NPU 联调。

## 回归重点

- Runtime 错误到 ACL 错误的每个分支。
- 异步 callback 在成功、失败、超时和对象提前销毁场景下的行为。
- 多设备/多 Context 和设备 reset 后的句柄失效。
- 利用率查询的保留参数和不支持字段。

执行入口见 [examples.md](examples.md)；没有硬件时只可运行不依赖设备的单元测试，不能宣称端到端通过。

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
