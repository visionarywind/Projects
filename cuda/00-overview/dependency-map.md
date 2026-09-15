# 依赖地图

- 文档目的：解释 00-overview/dependency-map.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](analysis-state.md)。
## 结论摘要

本页聚焦 00-overview/dependency-map.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


```text
公开 CUDA/OpenCL ABI
 → src/api 或 ICD dispatch
 → CUI globals/TLS/context
 → device + memobj + stream/channel
 → HAL/DMAL
 → 外部 common/RM/NVRM/compiler/firmware
```

## 依赖分类

- **编译依赖**：nvmake/nvmk、common build rules、nvcc、Boost、testutils、外部 compiler/gpgpucomp。
- **源码依赖**：M01 wrapper 依赖 M02；M02 建立 M03/M04/M05/M06 所需全局对象；M06 依赖 M05 QMD/stream 和 M07 syscall。
- **运行依赖**：RM/NVRM、firmware、GPU、OS driver model；OpenCL 还依赖 vendor library 和 platform dispatch。
- **验证依赖**：DVS runner、`nvidia-smi`、persistence service、权限和多 GPU 条件。

上述方向由源码清单、入口调用和模块文档归纳；外部版本和具体链接图未知。

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
