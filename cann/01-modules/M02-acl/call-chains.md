# M02 ACL 调用链

- 文档目的：解释 01-modules/M02-acl/call-chains.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cann HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：设备路径已确认；到 Driver 的深层路径为推断
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M02-acl/call-chains.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。

## 设备选择

```text
aclrtSetDevice(deviceId)
  -> aclrtSetDeviceImpl
      -> record start statistics/log
      -> rtSetDevice(deviceId)
          -> Api::Instance()->SetDevice(devId)
      -> ACL_GET_ERRCODE_RTS on failure
      -> record success / ACL_SUCCESS
```

实现证据：`acl/runtime/device.cpp:47-59`；Runtime 门面：`runtime/src/runtime/api/api_c_device.cc:74-84`。

## 设备同步

```text
aclrtSynchronizeDevice
  -> aclrtSynchronizeDeviceImpl
  -> rtDeviceSynchronize
  -> Runtime internal device synchronization
```

前两层和 Runtime C API 已在 `[acl/runtime/device.cpp:159-169]`、`[runtime/src/runtime/api/api_c_device.cc:208-215]` 确认。

## 模型执行（抽象链）

```text
ACL model execute
  -> model/data buffer validation
  -> Runtime model/task API
  -> Stream enqueue
  -> Driver queue/HDC
  -> device execution
  -> callback/event or synchronize
```

后半段是跨仓架构推断；具体模型 API、队列命令和异步回调实现需继续追踪。

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
