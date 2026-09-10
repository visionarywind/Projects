# M03 Runtime 接口契约

- 证据状态：设备 C API 和通用门面模式已确认；完整 API 清单待补

| 接口/模式 | 行为 | 错误/状态 |
|---|---|---|
| `rtGetDeviceCount` | 获取 `Api::Instance()`，调用 `GetDeviceCount` | 统一错误转换 `[api_c_device.cc:49-55]` |
| `rtSetDevice` | 等待全局状态锁，调用 `SetDevice(devId)` | 返回 Runtime/ACL RT 错误 `[api_c_device.cc:74-84]` |
| `rtGetDevice` | 调用内部 `GetDevice` | 统一转换 `[api_c_device.cc:87-95]` |
| `rtDeviceReset` | 调用内部 reset | 统一转换 `[api_c_device.cc:112-119]` |
| `rtDeviceSynchronize` | 等待设备任务完成 | 统一转换 `[api_c_device.cc:208-215]` |
| `rtsSet/Reset/GetDeviceResLimit` | 设置或查询设备资源限制 | 资源参数错误/内部错误 `[api_c_device.cc:165-205]` |
| 通用句柄 API | 解包并验证内部对象 | `RT_VALIDATE_AND_UNWRAP_OBJECT*`、错误宏 `[api_c.cc:118-153]` |

调用者须遵守初始化、Context/Stream 归属、异步 buffer 生命周期和销毁顺序；精确线程安全规则以头文件和实现为准。
