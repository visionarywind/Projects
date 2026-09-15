# M05 测试矩阵

- 文档目的：将功能、并发、异常和资源检查映射到测试入口。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/concurrency-queue HEAD 683b9e3（2026-09-15 只读确认）。
- 证据状态：测试名和目录已静态确认；native 全量 smoke 和 `test_threaded` 已运行通过，其他测试仍待验证。
- 最后更新：2026-09-10
- 前置阅读：[M05 README](README.md)
- 后续阅读：[失败分诊](failure-triage.md)
## 结论摘要

本页聚焦 01-modules/M05-verification/test-matrix.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


| 类别 | 示例测试/入口 | 覆盖意图 |
|---|---|---|
| explicit basic | `enqueue_one_explicit`, `enqueue_and_dequeue_one_explicit` | token producer 基本语义 |
| implicit basic | `enqueue_one_implicit`, `enqueue_and_dequeue_one_implicit` | thread-id producer |
| bulk | `enqueue_bulk`, `try_dequeue_bulk` | 批量构造、领取和计数 |
| concurrent | `try_dequeue_threaded`, `test_threaded`, `test_threaded_bulk` | producer/consumer 交错 |
| object semantics | Foo/Copyable/Moveable/ThrowingMovable | 构造、移动、异常、析构 |
| internal | `core_add_only_list`, `core_free_list` | 资源结构 |
| C API | `c_api_enqueue`, `c_api_try_dequeue` | ABI 转发 |
| fuzz | `tests/fuzztests` | 随机/压力操作 |
| model | `tests/relacy`, `tests/CDSChecker` | 原子交错与模型 |

## 单个测试

源码主程序支持：

```sh
./build/bin/unittests --disable-prompt --run test_threaded --iterations 1
```

具体测试名以该版本 `--help`/注册表为准；上述 `test_threaded` 命令已在本轮执行并通过。

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
