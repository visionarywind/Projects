# M02 ACL 接口契约

- 文档目的：解释 01-modules/M02-acl/interfaces.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cann HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：设备 API 行为已确认；下表其余前置条件需以对应头文件核对
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M02-acl/interfaces.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


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
