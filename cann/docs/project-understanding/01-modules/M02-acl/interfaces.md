# M02 ACL 接口契约

- 证据状态：设备 API 行为已确认；下表其余前置条件需以对应头文件核对

| 接口 | 前置条件 | 主要行为 | 失败处理 |
|---|---|---|---|
| `aclrtSetDevice(deviceId)` | ACL 已初始化；ID 合法 | 选择当前设备并更新统计 | Runtime 错误经 `ACL_GET_ERRCODE_RTS` 映射 `[acl/runtime/device.cpp:47-59]` |
| `aclrtResetDevice(deviceId)` | 设备已选择且资源可释放 | 重置设备 | 返回 ACL 错误 `[acl/runtime/device.cpp:80-91]` |
| `aclrtGetDevice(&id)` | 输出指针非空 | 查询当前设备 | 空指针直接失败 `[acl/runtime/device.cpp:127-138]` |
| `aclrtSynchronizeDevice()` | Runtime 已初始化 | 等待设备完成 | Runtime 错误转换 `[acl/runtime/device.cpp:159-169]` |
| `aclrtGetDeviceUtilizationRate(...)` | 输出指针和保留参数合法 | 查询 Core/AICPU 利用率 | 当前内存利用率不支持 `[acl/runtime/device.cpp:210-225]` |
| Model/Stream/Memory APIs | 对象状态和句柄合法 | 创建、提交、同步、销毁资源 | 具体错误码待逐头文件盘点 |

## 调用者责任

检查返回值；不要复用已销毁句柄；异步执行完成前保留输入输出及回调相关存储；跨线程使用 Context/Stream 时遵守 API 的线程规则。

## ABI

C 导出 API、枚举、结构体布局和 Runtime 错误码属于跨仓 ABI；改变公共结构体或错误映射需要兼容性评审。
