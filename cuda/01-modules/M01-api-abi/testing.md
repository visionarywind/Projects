# M01 测试、示例与开发指南

- 文档目的：解释 01-modules/M01-api-abi/testing.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M01-api-abi/testing.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## 现有测试证据

- `tests/cuda_test/basic_sanity.cu` 同时使用 Runtime API、Driver 内部对象和 CUI 导出符号，验证初始化、context、内存和 launch（静态确认：[tests/cuda_test/basic_sanity.cu:49-100,169-237]）。
- `tests/cuda_test/dispatch_main.cpp` 的 `--listGpus` 会列出设备并停止测试；与 `--forceRun` 的组合才适合实际执行（静态确认：[tests/cuda_test/dispatch_main.cpp:22-50]）。
- `tests/makefile.nvmk` 规定先构建 `libcuda_test` 再构建 `cuda_test`（静态确认：[tests/makefile.nvmk:3-18]）。构建未执行。

## 修改配方

1. 新增 API：先在 `inc/cuda.h` 建立声明/版本契约，再实现 `src/api` wrapper，最后检查 generated API 输入、`.def` 和 `cuda.nvmk`。
2. 修改参数宽度：同时核对 v1/v2 common 实现及输出转换；用 `basic_sanity` 或专门 API 测试检查错误路径。
3. 新增 PTDS/PTSZ 入口：确认宏展开、stream handle type 和导出符号一致。
4. 修改 wrapper 错误：保留输出参数初始化和局部回滚，不把未验证的内部对象暴露给调用者。

## 未验证命令

源码记录的 nvmake/MODS 测试命令依赖外部树、工具链和 GPU；本知识库未执行这些命令，因此没有运行结果。

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
