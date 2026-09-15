# M10 行级分析

- 文档目的：解释 01-modules/M10-tests-experiments/line-level-analysis.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M10-tests-experiments/line-level-analysis.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


- `[tests/cuda_test/cuda_test.nvmk:60-87]`：separate compilation device-link 和 binary object 规则。
- `[tests/cuda_test/cuda_test.nvmk:93-120]`：test name/disposition/binary 引用的 make-time 检查。
- `[tests/cuda_test/cuda_test.nvmk:140-153]`：生成 test list 并链接 dispatcher。
- `[tests/cuda_test/dispatch_main.cpp:22-50]`：初始化、GPU list、环境检查和 run/list。
- `[tests/run_tests.py:8-73,85-132]`：子进程双 pipe 消费、DVS 提取、timeout 和结果状态。
- `[tests/run_tests.py:171-189,191-245]`：no-result→failure、Waived/Pass/Failure 聚合、zero-pass/nonzero exit 和 Linux persistence/GPU 前置动作。
- `[tests/dvs-build-unix.sh:1-43]`：Linux nvmake/DVS 构建、复制和 tar 打包。

这些行只支持构建规则和 runner 控制流；本工作区没有由此产生的已验证 GPU/DVS 结果。

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
