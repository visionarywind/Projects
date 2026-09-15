# M03 行级审计

- 文档目的：修改 wait/signal 协议时的检查清单。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/concurrency-queue HEAD 683b9e3（2026-09-15 只读确认）。
- 证据状态：当前 HEAD 静态整理。
- 最后更新：2026-09-10
- 前置阅读：[M03 README](README.md)
- 后续阅读：[跨模块错误边界](../../90-cross-module/error-boundaries.md)
## 结论摘要

本页聚焦 01-modules/M03-semaphore-and-platform/line-level-analysis.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


| 检查点 | 证据 | 失败后果 |
|---|---|---|
| 自旋只消耗有限次数 | `lightweightsemaphore.h:290-323` | CPU 忙等或延迟变化 |
| count 与平台等待配对 | `290-360` | permit 丢失/虚假唤醒处理错误 |
| signal 使用 release | `421-430` | 数据可见性协议被削弱 |
| bulk wait 恢复数量 | `326-360` | batch 计数错误 |
| timeout 使用正确时钟/宏 | 平台实现分支 | 超时语义漂移 |
| 析构无 waiter | blocking queue 约束 | 访问已释放 semaphore |

## 验证

需要至少覆盖：立即成功、阻塞后 signal、timeout、bulk、重复 signal、消费者停止时 shutdown。未执行前均标为未验证。

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
