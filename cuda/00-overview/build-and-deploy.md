# 构建与部署

- 文档目的：解释 00-overview/build-and-deploy.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](analysis-state.md)。
## 结论摘要

本页聚焦 00-overview/build-and-deploy.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## 主构建

该快照使用 NVIDIA nvmake/nvmk，不是独立 CMake 项目。`cuda.nvmk` 组织 API、CUI、架构 HAL、工具和 kernel/syscall 源，并生成 Linux `libcuda.so.1.1` 等目标（静态确认：[cuda.nvmk:348-492,513-597,1695-1817]）。

## 测试构建

`tests/makefile.nvmk` 先构建 `libcuda_test`，再构建 `cuda_test`；`tests/cuda_test/cuda_test.nvmk` 为每个 case 建 binary，并生成 dispatcher 列表（静态确认：[tests/makefile.nvmk:3-18]；[tests/cuda_test/cuda_test.nvmk:60-153]）。Linux DVS 包装入口见 `[tests/dvs-build-unix.sh:1-43]`。

## 部署边界

需要外部 driver/common、RM/NVRM、compiler、工具链、目标 OS 和 GPU。当前没有运行 nvmake、nvcc、DVS 或安装部署，因此所有命令仅为源码记录，结果未验证。

## 相关文档
- [项目入口](../README.md)
- [分析状态](analysis-state.md)
- [源码证据索引](evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
