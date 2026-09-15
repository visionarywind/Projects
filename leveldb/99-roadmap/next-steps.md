# 后续步骤

- 文档目的：列出知识库维护和代码理解的下一步工作。
- 适用范围：项目理解知识库。
- 对应源码版本：source/leveldb HEAD 7ee830d（2026-09-15 只读确认）。
- 证据状态：任务状态按当前文档生成进度记录；命令结果仍需实际执行。
- 最后更新：2026-09-10
- 前置阅读：[技术债务](technical-debt.md)
- 后续阅读：[知识库入口](../README.md)
## 结论摘要

本页聚焦 99-roadmap/next-steps.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


- 源码版本：`main` / `7ee830d`。

## 立即验证

1. 运行 `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug`。
2. 运行 `cmake --build build`。
3. 运行 `ctest --test-dir build --output-on-failure`，保存失败输出。
4. 运行 `ctest --test-dir build -N`，核对文档测试矩阵。
5. 若成功，按 CI 运行 `db_bench` 和 install；记录实际依赖探测。

## 文档维护

- 在目标提交更新时复核所有源码行号和链接。
- 新增/删除模块时同步 README、module-registry、evidence-index、analysis-state。
- 代码改变持久化、所有权、线程或 ABI 时，更新风险登记和跨模块影响地图。
- 把未知设计动机和未执行实验保留为显式状态，不用推断替代证据。

## 深入研究

- 用 fault injection 验证 MANIFEST/CURRENT、WAL Sync 和 obsolete file cleanup。
- 用 ASan/TSan 验证 Snapshot、Version、Cache、Arena 和后台关闭。
- 用 benchmark 比较 block/filter/cache/compression/compaction 配置。
- 结合历史提交确认压缩阈值、Level 触发参数和维护取舍的设计动机。

## 完成定义

知识库可从根 README 导航到所有模块、关联层和实践层；关键结论有提交绑定的源码证据；构建/测试/性能结果与未验证项分开；重大变更能由影响地图找到相关源码和测试。

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
