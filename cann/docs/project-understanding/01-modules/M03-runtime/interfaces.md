# M03 Runtime 接口契约

- 文档目的：解释 01-modules/M03-runtime/interfaces.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cann HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：设备 C API 和通用门面模式已确认；完整 API 清单待补
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M03-runtime/interfaces.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


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
