# M04 Driver 接口契约

- 文档目的：解释 01-modules/M04-driver/interfaces.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cann HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：关键 C 接口和 ioctl ABI 已确认；完整公共头文件清单待补
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M04-driver/interfaces.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


| 接口 | 前置条件 | 行为 | 错误/后置条件 |
|---|---|---|---|
| `halGetSocVersion(devId, socVersion, len)` | 输出 buffer、长度和设备 ID 合法 | 查询设备信息并复制 SoC 字符串 | 返回 Driver 错误码；使用 `strncpy_s` `[devdrv_manager_adapter.c:16-52]` |
| `drvHdcClientCreate(params, &client)` | 参数和输出指针合法 | 分配并初始化 client/session/mutex | 分配或底层连接失败返回错误 `[hdc_client.c:20-111]` |
| `drvHdcClientDestroy(client)` | magic 合法 | 加锁检查活动 session 后销毁 | 活动 session 返回 `DRV_ERROR_CLIENT_BUSY` `[114-179]` |
| `queue_drv_open(file)` | 设备已注册、文件操作有效 | 建立进程 queue context | 写入 `file->private_data` `[queue_fops.c:62-77]` |
| `queue_drv_release(file)` | context 存在 | 清理进程信息和 context | 清空 `private_data` `[80-95]` |
| `queue_check_vector(vector)` | vector 地址可访问 | 检查数量、地址和长度 | 非法 vector 拒绝 `[149-175]` |
| esched ioctl | ioctl 结构布局匹配 | 设置/等待/提交事件、查询调度信息 | 用户态/内核态 ABI 错误需诊断 |

| `halMemAlloc/halMemFree` | Runtime 普通内存进入 HAL；由 SVM V2/V3 判断 cache 或 normal | 返回 Driver 错误；cache 是否命中由 size/align/flag/NUMA/产品决定 `[driver-memory-pool-analysis.md]` |
| Driver V2 ordinary cache | heap 内 mapped/unmapped node 按大小复用、切分、合并和 shrink | `DRV_ERROR_OUT_OF_MEMORY` 可能触发其他 heap/扩展；释放失败需检查回滚 |
| Driver V3 ordinary cache | cache allocator 管理多个 range/area，normal backing 扩展后纳入 gen allocator | 完整 idle range 才 shrink；`DRV_ERROR_BUSY` 进入延迟 recycle |

不要在 session 活动时销毁 HDC client；不要复用已释放 queue context；传入 ioctl 的结构体、指针、长度和版本必须匹配目标 Driver。

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
