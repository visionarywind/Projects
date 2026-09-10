# M02 ACL 行级分析

- 证据状态：关键设备路径已确认

## `aclrtSetDeviceImpl`

1. 记录 API 统计并输出开始日志 `[acl/runtime/device.cpp:47-50]`。
2. 调用 `rtSetDevice(deviceId)`，将设备选择交给 Runtime `[acl/runtime/device.cpp:51]`。
3. 失败时通过 `ACL_GET_ERRCODE_RTS` 转换 Runtime 错误并返回 `[acl/runtime/device.cpp:52-55]`。
4. 成功时更新统计并返回 `ACL_SUCCESS` `[acl/runtime/device.cpp:56-59]`。

## `aclrtGetDeviceImpl`

先检查输出指针，再调用 `rtGetDevice`；因此空指针错误在 ACL 层即可拦截 `[acl/runtime/device.cpp:127-138]`。

## 利用率查询

函数检查保留参数，然后分别读取 AICore、Vector Core 和 AICPU 使用率；内存利用率当前不支持 `[acl/runtime/device.cpp:210-225]`。调用方不应将未支持字段当作有效数值。

## 复杂度

设备 API 的参数检查和单次委托为 O(1)；模型 IO、buffer 和任务数量相关接口可能为 O(n)，需按具体实现测量。
