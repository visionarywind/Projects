# M04 Driver 调用链

- 文档目的：解释 01-modules/M04-driver/call-chains.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cann HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：关键链已确认；用户态 HAL 到具体内核设备命令部分未知
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M04-driver/call-chains.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。

## SoC 查询

```text
halGetSocVersion(devId, out, len)
  -> validate output/length/device
  -> halGetDeviceInfo
  -> drvGetDevInfo
  -> strncpy_s(out, soc version)
  -> drvError_t
```

证据：`driver/src/ascend_hal/dms/dc/devdrv_manager_adapter.c:16-52`。

## HDC client 生命周期

```text
drvHdcClientCreate
  -> parameter checker
  -> allocate client/session arrays
  -> initialize magic/config/state/mutex
  -> return client

drvHdcClientDestroy
  -> validate magic
  -> lock
  -> active session? --yes--> DRV_ERROR_CLIENT_BUSY
  -> destroy PCIe client
  -> free arrays/client
```

证据：`driver/src/ascend_hal/hdc/common/hdc_client.c:20-179`。

## Queue 文件生命周期

```text
open
  -> TGID queue context
  -> add process info
  -> file->private_data
release
  -> delete process info
  -> free context
  -> private_data = NULL
```

证据：`driver/src/sdk_driver/queue/host/queue_fops.c:62-95`。

## 普通 rtMalloc 的 Driver cache

```text
Runtime NpuDriver::DevMemAlloc
  -> halMemAlloc
  -> V2: devmm_alloc_proc -> heap list -> mapped/size tree -> node split/map
  -> V3: svm_mem_malloc -> malloc manager -> cache_malloc
       -> svm_ga_alloc -> size_area_tree -> area split
  -> 返回 VA
```

cache miss 的后半段不同：V2 必要时创建 heap 或建立 mapping；V3 计算扩展大小，调用 `svm_normal_malloc` 获取 backing，`svm_ga_add_range` 成功后重试。证据与 free/shrink 链见 [driver-memory-pool-analysis.md](driver-memory-pool-analysis.md)。

## 产品选择

`driver/src/ascend_hal/svm/CMakeLists.txt` 按 `PRODUCT` 选择 V2/V3：`ascend910B` 使用 V2，`ascend950` 使用 V3；`build.sh` 将 `ascend910b` 和 `ascend910_93` 映射到 `ascend910B`。源码规则不能替代当前构建产物或设备验证。

## HDC 连接

`queue_drv_host_init` 检查重复连接，调用 `hdcdrv_kernel_connect` 并保存 session；uninit 调用 close 并清理 `[queue_fops.c:97-147]`。

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
