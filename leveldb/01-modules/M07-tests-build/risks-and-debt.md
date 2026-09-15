# M07 风险与技术债务

- 文档目的：登记工程、验证和维护风险。
- 适用范围：M07。
- 对应源码版本：source/leveldb HEAD 7ee830d（2026-09-15 只读确认）。
- 证据状态：机制已确认，严重度需 CI/实验确认。
- 最后更新：2026-09-10
- 前置阅读：[M07 design](design.md)
- 后续阅读：[项目风险](../../99-roadmap/risk-register.md)
## 结论摘要

本页聚焦 01-modules/M07-tests-build/risks-and-debt.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


- 源码版本：`main` / `7ee830d`。

| 风险 | 证据 | 影响 | 验证 |
|---|---|---|---|
| 可选依赖组合未覆盖 | `CMakeLists.txt:39-53` | 分支/链接问题 | 多配置构建 |
| 测试被聚合/注释导致遗漏 | `CMakeLists.txt:312-350` | 回归未执行 | ctest -N + coverage |
| 共享库排除内部测试 | `CMakeLists.txt:326-351` | 静态/共享行为差异 | 两种构建 |
| CTest 配置参数遗漏 | `CMakeLists.txt:364-403` | 目标未运行 | ctest -N |
| benchmark 噪声/旧基线 | `README.md:129-173` | 错误性能结论 | 固定环境重复 |
| 平台矩阵不完整 | `build.yml:20-37` | Windows/macOS 回归 | CI |
| install/export 失配 | `CMakeLists.txt:470-518` | 下游 find_package 失败 | 临时 prefix 消费测试 |
| 有限维护策略 | `README.md:2-6` | 新功能接受度/长期支持 | 变更评审 |

## 技术债务

当前工程配置把大量平台和可选依赖逻辑集中在顶层 CMake；测试目标也混合聚合与独立 executable。项目有限维护意味着大型构建系统重构本身可能超出支持范围，修改应优先解决实际 bug 或兼容需求。

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
