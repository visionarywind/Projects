# M04 Driver 测试

- 文档目的：解释 01-modules/M04-driver/testing.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cann HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：测试目录和构建开关已确认；结果未执行
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M04-driver/testing.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。

## 分层测试

- 参数校验：空指针、长度、设备 ID、vector 数量和地址。
- 生命周期：HDC client/session 创建、关闭、busy destroy；queue open/init/uninit/release。
- ABI：esched ioctl 命令、结构体布局、32/64 位和版本兼容。
- 设备集成：HAL、SDK-driver、内核模块、设备节点、固件和 NPU。
- 故障注入：连接失败、DMA map/unmap 失败、设备 reset、超时和热插拔（若平台支持）。

## SVM cache 专项

- V2：mapped/unmapped 命中、exact/upper-bound、node split/merge、cache/nocache、threshold shrink 和底层释放失败回滚。
- V3：cache 条件 bypass、多个 range、area split/merge、expand 失败回滚、完整 idle range、`DRV_ERROR_BUSY` 延迟 recycle。
- 产品矩阵：至少分别构建/验证 `ascend910B`（V2）与 `ascend950`（V3），不能将一套产物的结果外推到另一套。

这些是待执行回归项；本环境未运行构建、Driver UT、NPU、固件或性能测试。


1. 活动 session 时必须拒绝 HDC client 销毁。
2. file release 后 `private_data` 必须清空且 context 不再使用。
3. 非法 vector 不应进入 DMA/HDC 路径。
4. ioctl 错误应保留命令、设备和底层错误上下文。

没有匹配硬件时只能运行编译和不依赖设备的 UT，不能宣称设备集成测试通过。

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
