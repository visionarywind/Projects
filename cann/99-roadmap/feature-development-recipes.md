# 开发实践：功能开发配方

- 文档目的：解释 99-roadmap/feature-development-recipes.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cann HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 99-roadmap/feature-development-recipes.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## 新增 ACL/Runtime API

1. 定义公共 C ABI、前置状态、所有权和错误码。
2. 增加 ACL wrapper（若属于 ACL）和 Runtime C API/内部实现。
3. 更新导出表、Hook、CMake target、安装头文件和依赖。
4. 增加参数/句柄/错误映射/异步生命周期 UT。
5. 评估 GE、Driver 和应用兼容性。

## 修改 GE 执行行为

1. 确认 V1/V2 状态和模型格式影响。
2. 检查输入输出 shape、地址复用、Stream/Event/allocator。
3. 修改 Load/Execute/UnLoad 对称清理路径。
4. 增加同步、异步、失败和重复调用回归。

## 修改 Driver/HAL

1. 确认 HAL、SDK-driver、内核 ioctl 和产品宏边界。
2. 同步用户态/内核态结构布局和命令 ABI。
3. 审查用户指针、长度、DMA、锁和 session 生命周期。
4. 更新 Runtime 调用者、错误映射和设备矩阵。

详细影响见 `90-cross-module/change-impact-map.md`。

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
