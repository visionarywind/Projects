# M04 Driver：模块入口

- 文档目的：解释 01-modules/M04-driver/README.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：`977336913`
- 证据状态：主要目录、HAL/SDK-driver 分层和 HDC/Queue 入口已确认
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M04-driver/README.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


Driver 为用户态/内核态之间提供设备管理、通信、队列、内存和调度能力，仓库 README 将代码分为 DCMI、HAL 和 SDK-driver `[driver/README.md:9-16]`。

## 关键分层

```text
Runtime / HAL client
  -> ascend_hal
      -> build/bbox/esched/hdc/queue/buff/dms/svm/dpa/trs/comm...
  -> sdk_driver
      -> queue/esched/buff/trsdrv/hdc/vmng/vnic/vpc/kernel_adapt...
  -> device nodes / kernel modules
  -> NPU
```

HAL 构建组成见 `[driver/src/ascend_hal/CMakeLists.txt:9-29]`；SDK-driver 组成见 `[driver/src/sdk_driver/CMakeLists.txt:9-30]`。

## 关键入口

- `halGetSocVersion`：通过设备信息查询 SoC 版本 `[driver/src/ascend_hal/dms/dc/devdrv_manager_adapter.c:16-52]`。
- `drvHdcClientCreate/Destroy`：主机设备通信 client 生命周期 `[driver/src/ascend_hal/hdc/common/hdc_client.c:49-179]`。
- `queue_drv_open/release`：进程 queue context 生命周期 `[driver/src/sdk_driver/queue/host/queue_fops.c:62-95]`。
- `queue_drv_host_init/uninit`：内核侧 HDC session 连接 `[queue_fops.c:97-147]`。

## 内存池专题

普通 `rtMalloc` 下沉到 `halMemAlloc` 后，不是必然直接向设备申请，也不是 Runtime SOMA。Driver 根据产品和请求属性使用 ordinary cache：

- `ascend910B`/`ascend910_93` 编译 V2：heap + 多棵 VA/size/mapped 红黑树；
- `ascend950` 编译 V3：`cache_allocator` + 多个 `ga_range`/`ga_area`；
- 两者都支持按大小 exact/upper-bound 复用、切分、相邻合并和延迟 shrink。

请求是否实际进入 cache 还取决于 size、align、flag、NUMA、设备能力和当前状态。完整设计、Demo、行级走读和验证边界见 [driver-memory-pool-analysis.md](driver-memory-pool-analysis.md)。

## 边界

Driver 用户态源码可确认 HAL、SVM cache、VA/映射元数据和 ioctl 调用边界；闭源内核/固件中的物理页算法、设备完成时机和性能收益仍未验证。

## 文档元数据（规范补充）

- 文档目的：说明 `01-modules/M04-driver/README.md` 的源码分析范围、结论和维护入口。
- 适用范围：当前项目对应模块/入口的静态源码与测试分析。
- 对应源码版本：以本项目 `00-overview/analysis-state.md` 或同页版本字段为准。
- 证据状态：静态源码证据；未执行的构建、测试、GPU、网络或多进程行为保持“未验证”。
- 最后更新：2026-09-15
- 前置阅读：本项目根 README 与 `00-overview/analysis-state.md`。
- 后续阅读：本模块/示例的实现、测试和风险页面。

## 深度审计

| 分析对象 | 入口落地 | 正常路径 | 分支 | 异常 | 清理 | 数据生命周期 | 执行上下文 | 行级证据 | Demo 映射 | 状态/缺口 |
|---|---|---|---|---|---|---|---|---|---|---|
| `01-modules/M04-driver/README.md` | 已完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 已映射或不适用 | 部分完成：动态行为、边界或专用变体仍需验证 |

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
