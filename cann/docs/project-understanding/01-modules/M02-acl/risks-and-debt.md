# M02 ACL 风险与技术债务

- 文档目的：解释 01-modules/M02-acl/risks-and-debt.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cann HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分为风险评估，不等同于已发现缺陷
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M02-acl/risks-and-debt.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。

## 高风险

- ACL、Runtime、Driver 和 Toolkit 版本不匹配导致 ABI 或错误码不兼容 `[runtime/README.md:16-19]`。
- 异步任务仍引用 buffer、stream 或 callback 时提前销毁，可能造成未定义行为。
- 设备 reset 与其他线程并发使用资源的边界尚未从已读代码完整确认。

## 中风险

- 同一功能在 ACL 仓和 Runtime 仓出现，公开符号、打包目标和链接优先级需要安装包验证 `[acl/CMakeLists.txt:167-190]`、`[runtime/src/acl/aclrt/acl_rt.cpp:17-79]`。
- 错误映射可能丢失底层扩展错误上下文；应保留原始错误和阶段日志。

## 待补技术债务

- 补齐公共 API 的参数、线程安全、所有权和版本废弃表。
- 用 ASAN/TSAN 或 mock 覆盖异常生命周期。
- 补齐无设备 mock 与真实 NPU 的分层测试矩阵。

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
