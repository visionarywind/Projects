# 跨模块：调用链

- 文档目的：解释 90-cross-module/cross-module-call-chains.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cann HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：跨层入口已确认；带动态分发的末端部分推断
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 90-cross-module/cross-module-call-chains.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。

## `aclrtSetDevice` 到设备

```text
aclrtSetDeviceImpl [ACL]
  -> rtSetDevice [Runtime C API]
  -> Api::Instance()->SetDevice [Runtime internal]
  -> device/platform implementation [未完整追踪]
  -> ascend_hal / device manager [Driver]
  -> device node / kernel [推断]
  -> NPU
```

证据：`[acl/runtime/device.cpp:47-59]`、`[runtime/src/runtime/api/api_c_device.cc:74-84]`、Driver HAL 构建 `[driver/src/ascend_hal/CMakeLists.txt:9-29]`。

## GE V2 执行到设备

```text
ModelV2Executor::Execute [GE]
  -> SpecifyInputs/Outputs
  -> OccupyStreamResource
  -> ExecuteGraph
  -> rt*/ACL Runtime [跨仓边界]
  -> queue/HDC/esched [Driver]
  -> NPU
```

GE 入口步骤有源码证据 `[ge/runtime/v2/core/model_v2_executor.cc:260-305]`；末端由架构和 Driver 组件组织推断，不能标为完整静态调用链。

## 释放链

```text
GE Executor UnLoad
  -> main graph UnLoad / de-init graph
  -> Runtime stream/event/model resources
  -> Driver queue/HDC close
  -> client/context free
```

Driver 已明确 HDC client 必须无活动 session 才能销毁 `[driver/src/ascend_hal/hdc/common/hdc_client.c:114-179]`。

## 调试方法

每层记录：API 名称、设备 ID、模型/Graph ID、Context/Stream、底层错误码、线程和时间戳；从最上层失败点反查最后一个成功边界。

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
