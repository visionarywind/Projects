# 跨模块：调用链

- 证据状态：跨层入口已确认；带动态分发的末端部分推断

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
