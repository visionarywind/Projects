# 开发快速入口

- 文档目的：给熟悉 C++ 但未参与项目的开发者一个真实、可执行的阅读和修改入口。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/concurrency-queue HEAD 683b9e3（2026-09-15 只读确认）。
- 证据状态：命令来自仓库 Make/CMake/CI；本页不替未执行命令报告成功。
- 最后更新：2026-09-10
- 前置阅读：[总入口](../README.md)
- 后续阅读：[开发工作流](development-workflow.md)
## 结论摘要

本页聚焦 99-roadmap/quick-start.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。

## 先看什么

1. [项目概览](../00-overview/project-overview.md)
2. [M01 实现](../01-modules/M01-core-queue/implementation.md)
3. [D01 执行轨迹](../80-demos/D01-unit-test-smoke/execution-trace.md)
4. 与改动类型对应的 M02–M07 页面

## 常用命令

```sh
cd build && make tests
./build/bin/unittests --help
./build/bin/unittests --disable-prompt --run test_threaded --iterations 1
cd build && make benchmarks
cmake -S . -B cmake-build && cmake --build cmake-build
```

执行前确认依赖和工作区；实际结果记录到 `00-overview/analysis-state.md`。

## 修改前检查

- 明确修改属于 M01–M07 哪个模块。
- 阅读对应 line-level-analysis 和 change-impact-map。
- 查清是否影响 token、block、atomic memory order、semaphore count、C ABI 或安装头。
- 先定义失败语义和 shutdown 行为，再改代码。

## 修改后检查

- 编译受影响的最小目标。
- 运行单线程、bulk、异常、threaded；涉及 blocking/C ABI 时运行对应测试。
- atomic/回收改动增加模型检查或压力验证。
- 更新证据索引、状态页和受影响文档的行号。

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
