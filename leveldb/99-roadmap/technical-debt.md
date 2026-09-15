# 技术债务

- 文档目的：区分源码既定约束、维护成本和可验证债务。
- 适用范围：LevelDB 全仓库。
- 对应源码版本：source/leveldb HEAD 7ee830d（2026-09-15 只读确认）。
- 证据状态：现象来自源码/README；优先级为工程判断。
- 最后更新：2026-09-10
- 前置阅读：[项目风险](risk-register.md)
- 后续阅读：[下一步](next-steps.md)
## 结论摘要

本页聚焦 99-roadmap/technical-debt.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


- 源码版本：`main` / `7ee830d`。

## 主要债务

1. **手工生命周期复杂**：裸指针、引用计数、iterator cleanup、Arena 和 Cache handle 分布在多模块，局部重构容易造成 UAF/泄漏。
2. **持久化格式兼容成本**：WAL、InternalKey、SSTable、MANIFEST 都是隐含 ABI，变更需要旧数据和损坏测试。
3. **平台分支维护**：POSIX/Windows/MemEnv/port/可选依赖必须保持接口同步。
4. **构建配置集中**：顶层 CMake 同时负责 feature detection、源文件、测试、benchmark 和 install。
5. **测试粒度混合**：聚合 `leveldb_tests` 与独立 CTest 目标并存，部分故障/issue 测试在当前 CMake 中注释。
6. **有限维护政策**：README 只承诺有限类型变更，新功能的长期支持和评审资源受限。[README.md:2-6](../../source/leveldb/README.md#L2-L6)
7. **性能结论缺少当前基线**：README 的历史数字不能代替当前硬件 benchmark。
8. **行号证据会漂移**：知识库必须绑定提交并在源码更新后重核。

## 不应贸然偿还的债务

不要仅为“现代化”目的改变公共 ABI、异常/RTTI 约束、持久化布局、平台抽象或 Cache ownership；先证明真实 bug/兼容需求，再扩大设计。

## 可执行偿还计划

- 增加格式兼容/损坏/fuzz 和资源 Sanitizer 覆盖。
- 记录可重复 benchmark，而非手填性能数字。
- 保持模块文档与提交、CMake 目标和测试清单同步。
- 对高风险 ownership 改动采用小 patch、局部回归和重开验证。

## 相关文档
- [项目入口](../README.md)
- [分析状态](../00-overview/analysis-state.md)
- [源码证据索引](../00-overview/evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
