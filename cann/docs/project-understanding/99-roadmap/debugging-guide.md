# 开发实践：调试指南

- 证据状态：流程建议；未执行硬件验证

## 通用定位

1. 固定版本、SoC、Toolkit、Driver/Firmware 和构建选项。
2. 记录 API、阶段、线程、设备、Context、Stream、Graph/Model 和原始错误码。
3. 找到第一个失败边界，而不是只看顶层 `ACL_ERROR`。
4. 沿 `ACL → Runtime → HAL → SDK/kernel` 或 `GE → Runtime` 反查。
5. 检查异步完成、buffer/handle 生命周期和清理顺序。

## 断点建议

- GE：`GEInitialize`、`GeSession::Impl`、`ModelV2Executor::Load/Execute/UnLoad`。
- ACL：`aclrtSetDeviceImpl`、模型/Stream/Memory 对应 `*Impl`。
- Runtime：`rtSetDevice`、`Api::Instance()` 分发、句柄验证宏。
- Driver：`halGetSocVersion`、`drvHdcClientCreate/Destroy`、queue open/release、HDC connect/close。

## 结果记录

区分参数错误、状态错误、资源错误、设备/固件错误和模型错误；若没有 NPU，只记录静态分析或构建/UT 的实际结果。
