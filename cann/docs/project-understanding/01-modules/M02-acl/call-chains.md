# M02 ACL 调用链

- 证据状态：设备路径已确认；到 Driver 的深层路径为推断

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
