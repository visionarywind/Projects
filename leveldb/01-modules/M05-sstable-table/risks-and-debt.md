# M05 风险与技术债务

- 文档目的：登记表格式、读取和资源生命周期风险。
- 适用范围：M05。
- 对应源码版本：source/leveldb HEAD 7ee830d（2026-09-15 只读确认）。
- 证据状态：触发机制已确认，实际严重度需故障注入/压测。
- 最后更新：2026-09-10
- 前置阅读：[M05 design](design.md)
- 后续阅读：[项目风险](../../99-roadmap/risk-register.md)
## 结论摘要

本页聚焦 01-modules/M05-sstable-table/risks-and-debt.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


- 源码版本：`main` / `7ee830d`。

| 风险 | 证据 | 影响 | 验证 |
|---|---|---|---|
| footer/handle 格式不对称 | `format.cc:31-65` | 无法打开旧表 | 损坏/兼容测试 |
| CRC 或长度检查回归 | `format.cc:68-101` | 越界或返回错误数据 | fuzz/ASan |
| Block ownership 错误 | `block.cc:24-45` | double free/UAF/泄漏 | cache + iterator 压测 |
| cache handle 提前释放 | `table.cc:152-204` | iterator 访问失效 block | 并发读 + eviction |
| filter 假阴性 | `filter_block.cc:89-103` | 误报 key 不存在 | 完整 key 回归 |
| comparator 假设字节序 | `block_builder.cc:70-104` | 自定义 comparator 读错 | reverse comparator |
| builder 未 Finish/Abandon | `table_builder.cc:71-75` | 断言失败/文件不完整 | 错误路径测试 |
| metadata 错误被忽略过度 | `table.cc:81-107` | filter 退化或诊断不足 | 损坏 metadata |
| Table 与文件生命周期脱节 | `table.h:27-40` | UAF | 手工 ownership 审计 |

## 技术债务

M05 使用显式裸指针、手工 cleanup 和 `Status` 传播；这些是当前实现的既定风格，不应在局部改动中擅自引入异常或智能指针 ABI 变化。压缩阈值、cache 容量和 filter 参数的实际收益仍需基准数据支持。

## 相关文档
- [项目入口](../../README.md)
- [分析状态](../../00-overview/analysis-state.md)
- [源码证据索引](../../00-overview/evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题

需要在 Debug/ASan、不同文件系统和压缩依赖组合下验证损坏恢复、短读和并发 cache 淘汰边界。

## 下一步阅读建议
先阅读 [分析状态](../../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
