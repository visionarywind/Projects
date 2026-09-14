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

## 内存接口分层契约

| 接口族 | 实际资源层 | 关键后置条件 |
|---|---|---|
| `rtMalloc/rtFree` | Runtime policy + Driver ordinary allocation/cache | `rtFree` 的 Driver cache 保留与否取决于 flag、size、align、SoC 和 cache 状态；不能假设每次立即归还底层 |
| `rtMemPoolCreate/Destroy` | Runtime `SegmentManager` + Driver pool | pool handle、地址区间、`CanDelete` 和异步任务状态必须一致 |
| `rtMemPoolMallocAsync/FreeAsync` | 本地 Segment 更新 + `SomaMemMng` AICPU 配置 | API 成功表示提交/元数据路径成功，不自动表示设备已完成 |
| `rtFreeWithDevSync` | 设备同步后普通释放 | 与普通 `rtFree` 的异步释放边界不同 `[runtime/src/runtime/api/api_c_memory.cc:173-192]` |

普通 `rtMalloc` 的具体 Driver V2/V3 cache 条件和回收策略见 [M04 Driver 专题](../M04-driver/driver-memory-pool-analysis.md)。
