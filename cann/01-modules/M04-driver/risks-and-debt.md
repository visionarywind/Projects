# M04 Driver 风险与技术债务

- 文档目的：解释 01-modules/M04-driver/risks-and-debt.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cann HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：风险评估；不是已验证缺陷
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M04-driver/risks-and-debt.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。

## 高风险

- 用户态 HAL、SDK-driver、内核模块、固件和 Runtime 版本/ABI 不匹配。
- ioctl、DMA、HDC 和 queue 参数涉及用户指针、长度和异步生命周期；校验或释放顺序错误可能造成崩溃或数据损坏。
- 活动 HDC session/queue 仍存在时释放 client/context，可能破坏底层通信；现有代码已对 HDC client 返回 busy `[driver/src/ascend_hal/hdc/common/hdc_client.c:114-179]`。

## 中风险

- SoC 条件编译（如 Ascend910B）使不同平台的组件集合和行为不同 `[driver/src/ascend_hal/CMakeLists.txt:9-29]`。
- 设备节点、PCI 设备 ID、内核版本和权限要求未形成统一兼容矩阵。
- Driver 错误向 Runtime/ACL 转换时可能丢失原始上下文。

## SVM ordinary cache 风险

- V2/V3 是产品编译分支；误以为当前环境使用另一代实现会导致错误定位和回归遗漏 `[driver/src/ascend_hal/svm/CMakeLists.txt:11-15]`。
- V2 的 mapped/unmapped 状态、V3 的 range/area 状态和底层 backing 必须一致；静态源码不能确认闭源内核中的物理页生命周期。
- cache threshold、fragmentation、shrink 和 `DRV_ERROR_BUSY` 处理需要长期压力与故障注入验证；当前没有命中率或性能数据。


- 追踪每个 HAL API 到 ioctl、内核模块和设备命令。
- 生成 ioctl ABI、结构体版本和符号导出清单。
- 增加故障注入、DMA 边界、并发销毁、热插拔和长期压力测试。

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
