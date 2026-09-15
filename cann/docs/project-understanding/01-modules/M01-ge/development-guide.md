# M01 GE 开发指南

- 文档目的：解释 01-modules/M01-ge/development-guide.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cann HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M01-ge/development-guide.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## 修改前

1. 判断变更属于 Graph MetaDef、Parser、Compiler、Executor 还是 Runtime 边界。
2. 读取相关状态机、模型格式、公共头文件和 CMake target。
3. 列出 Graph/Model/Session/Stream/Event/Buffer 的所有权和异常清理路径。
4. 确定 V1、V2、静态 shape、动态 shape 和异步路径是否都受影响。

## 修改中

- 保持 Graph 语义与设备资源实现分离。
- V2 Load/Execute/UnLoad 做对称状态和资源处理。
- 不改变公共 Graph、Tensor、模型格式和 Executor ABI，除非完成兼容性评审。
- 错误路径保留 GE 状态、Runtime 原始错误和 Graph/Model 标识。

## 修改后

补充元数据、编译、执行、异常清理和跨模块测试；在匹配 Toolkit/Driver/Firmware/NPU 上验证动态和异步行为，并记录未验证项。

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
